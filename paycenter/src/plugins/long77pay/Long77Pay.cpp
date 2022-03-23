/*
 * =====================================================================================
 *
 *       Filename:  Long77Pay.cpp
 *
 *    Description:  龙77pay 支付
 *
 *        Version:  1.0
 *        Created:  2021年08月04日 17时49分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Long77Pay.h"
#include <algorithm>
#include "Common.h"
#include "Security.h"
#include "SdkList.h"
#include "SdkChannelList.h"
#include "GameList.h"
#include "MyNet.h"


/////////////////////////////////////////////////////////////////////
CLong77Pay::CLong77Pay()
    : CPayBase("Long77Pay", depositRequest, httpRequestSync)
{
}

CLong77Pay::~CLong77Pay()
{
}

int32_t CLong77Pay::Init(void)
{
    return this->Update();
}

int32_t CLong77Pay::Update(void)
{
    return CPayBase::Init("main/long77pay");
}

void CLong77Pay::Maintance()
{
    CPayBase::Maintance();
}


ENTRY_FUNC_DECLARE(CLong77Pay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CLong77Pay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CLong77Pay, doWithdraw)
{
    return CPayBase::doCreditRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CLong77Pay, doWithdrawCB)
{
    return CPayBase::doCreditCallback(pClient, req, szResp, cbResp, ext_headers);
}

void CLong77Pay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    HTTP_REQ_INT64(payMoney, amount);
    HTTP_REQ_STR(strUin, uin);

    //参与签名，固定顺序
    Json::Value data;
    data["reqtime"] = time(NULL);
    data["pid"] = pSdk->getAccount().asInt();
    data["money"] = (Json::Int64)(payMoney / 100.0);
    data["out_trade_no"] = orderNo;
    data["notifyurl"] = pSdk->getPayCallbackUrl() ;
    data["returnurl"] = pGame->getCallbackUrl() ; //仅content_type='text'时生效

    CMyString strSignData("reqtime=%ld&pid=%d&money=%ld&out_trade_no=%s&notifyurl=%s&returnurl=%s",
                        data["reqtime"].asInt64(), data["pid"].asInt(), data["money"].asInt64(), orderNo.c_str(), 
                        pSdk->getPayCallbackUrl().c_str(),  pGame->getCallbackUrl().c_str());
    strSignData += "&";
    strSignData += pSdk->getApiKey();

    LogDebug("[%s] make deposit request sign data:[%s]", m_sdkTag.c_str(), strSignData.c_str());

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString strMD5 = CMD5Wrapper::encode(strSignData.c_str());
    strMD5.MakeLower();
    req_maker.add("sign", strMD5);

    //不参与签名
    /*
    text商户发送请求后平台自动跳转到付款界面
    json可获取收款卡姓名,卡号银行等信息,由商户自行展现支付界面
    */
    CMyString strContentType;
    Json::Value extraData = pSdk->getPayExtraData();
    if (!extraData.empty())
    {
        strContentType = extraData["content_type"].asString();
    }
    if (strContentType.empty())
    {
        strContentType = "text";
    }
    req_maker.add("content_type", strContentType);
    req_maker.add("remark", strUin);
    /*
    通道代码
    ● ebill：虚拟卡银行转账
    ● bank：银行转账
    ● zfbzk：ZALO转卡
    ● wxzk：MOMO转卡
    ● wxgm：MOMO扫码
    ● zfbgm：ZALO扫码
    */
    req_maker.add("type", pChannel->getBankCode());

    httpData =  req_maker.http_str_manual('&', '=', false, false, true);
    url = pSdk->getPayUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}

bool CLong77Pay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    LogDebug("[%s]deposit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());

    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        errNote.Format("[%s]resp data [%s] not json string", m_sdkTag.c_str(), respData.c_str());
        return false;
    }

    CMyString strCode = jrRoot.getString("code");
    if (strCode != "200")
    {
        errNote = jrRoot.getString("msg");
        return false;
    }

    Json::Value jrData;
    bool bRet = jrRoot.getJsonObject("data", jrData);
    if (!bRet)
    {
        errNote.Format("[%s]resp data [%s] member [data] not object", m_sdkTag.c_str(), respData.c_str());
        return false;
    }

    depositUrlData = jrData["pay_url"].asString();
    depositOrderNo = jrData["id"].asString();

    return true;
}


bool CLong77Pay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &depositOK)
{
    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString params;
    req.getFullParamStr(params);
    LogDebug("[%s]deposit callback, header[%s], param[%s] post[%s]",
            m_sdkTag.c_str(), headers.c_str(), params.c_str(), strPost.c_str());

    HTTP_REQ_STR(strStatus, status);
    depositOK = (strStatus == "success");

    orderNo = req.getParam("out_trade_no");
    platformOrderNo = req.getParam("trade_no");
    fee = (int64_t)(req.getParamInt64("money") *100);           //发起金额
    realFee = (int64_t)(req.getParamInt64("money_real") *100);  //客人实际付款金额
    tax = (int64_t)(req.getParamInt64("fees") *100);            //当前订单佣金（已经在平台账户中扣除）

    return true;
}

bool CLong77Pay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");

    //参与签名，固定顺序
    Json::Value data;
    data["callback_time"] =  (Json::Int64)(req.getParamInt64("callback_time"));
    data["pid"] =  (Json::Int64)(req.getParamInt64("pid"));
    data["trade_no"] =  req.getParam("trade_no");
    data["out_trade_no"] =  req.getParam("out_trade_no");
    data["money"] =  (Json::Int64)(req.getParamInt64("money"));
    data["type"] =  req.getParam("type");

    CMyString strSignData("callback_time=%ld&pid=%ld&trade_no=%s&out_trade_no=%s&money=%ld&type=%s",
                        data["callback_time"].asInt64(), data["pid"].asInt64(), data["trade_no"].asString().c_str(),
                        data["out_trade_no"].asString().c_str(), data["money"].asInt64(), data["type"].asString().c_str());
    strSignData += "&";
    strSignData += pSdk->getApiKey();
    LogDebug("[%s] valid deposit callback sign data:[%s]", m_sdkTag.c_str(), strSignData.c_str());

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString strSign2 = CMD5Wrapper::encode(strSignData.c_str());

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), strSignData.c_str(), strSign2.c_str(), strSign1.c_str());
        return false;
    }
    return true;
}

void CLong77Pay::doDepositCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    if (bSucceed)
    {
        safe_strcpy(szResp, cbResp, "success");   
    }
    else
    {
        safe_strcpy(szResp, cbResp, "fail");
    }
    cbResp = strlen(szResp);
}

void CLong77Pay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk,  CMyString& url, CMyString &contentType, CMyString& httpData) 
{
    Json::Value data;
    data["reqtime"] = time(NULL);
    data["pid"] =  pSdk->getAccount().asInt();
    data["money"] =  (Json::Int64)(req["fee"].asInt64()/100.0); 
    data["outwithdrawno"] = req["orderNo"].asString();
    data["bankno"] = req["bankCard"].asString();

    //固定顺序签名
    CMyString strSignData("reqtime=%ld&pid=%d&money=%ld&outwithdrawno=%s&bankno=%s",
                    data["reqtime"].asInt64(), data["pid"].asInt(), data["money"].asInt64(),
                    data["outwithdrawno"].asString().c_str(), data["bankno"].asString().c_str());
    strSignData += "&";
    strSignData += pSdk->getApiKey();
    LogDebug("[%s] make credit request sign data:[%s]", m_sdkTag.c_str(), strSignData.c_str());

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str = CMD5Wrapper::encode(strSignData.c_str());
    str.MakeLower();
    req_maker.add("sign", str);

    //不参与签名
    req_maker.add("bankname", req["bankCode"].asString());
    req_maker.add("customer_name", req["bankName"].asString());
    req_maker.add("notifyurl", pSdk->getWithdrawCallbackUrl());
     
    httpData = req_maker.http_str_manual('&', '=', false, false, true);
    url = pSdk->getWithdrawUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}


bool CLong77Pay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    LogDebug("[%s]credit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());

    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        errNote.Format("[%s]credit resp [%s] not valid json", m_sdkTag.c_str(), respData.c_str() );
        return false;
    }

    int64_t nCode = jrRoot.getInt64("code");
    if (nCode != 200) //返回结果代码，不等于200，代表出错
    {
        errNote = jrRoot.getString("msg");
        return false;
    }
    
    Json::Value jrData;
    bool bRet = jrRoot.getJsonObject("data", jrData);
    if (!bRet)
    {
        errNote.Format("[%s]credit resp data [%s] member [data] not object", m_sdkTag.c_str(), respData.c_str());
        return false;
    }

    creditOrderNo = jrData["outwithdrawno"].asString();
    return true;
}


bool CLong77Pay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString params;
    req.getFullParamStr(params);
    LogDebug("[%s]credit callback, header[%s], param[%s], post[%s]",
            m_sdkTag.c_str(), headers.c_str(), params.c_str(), strPost.c_str());

    orderNo = req.getParam("outwithdrawno");
    creditOrderNo = "N/A";
    fee = (int64_t)(req.getParamInt64("money") *100);
    tax = (int64_t)(req.getParamInt64("fees") *100);
    realFee = fee - tax;

    //1:待处理;2:已付款;3:已驳回;4:订单异常;5:付款中;6:代付失败
    int64_t nStatus = req.getParamInt64("is_state");
    if (nStatus == 1 || nStatus == 2 || nStatus == 5)
    {
        creditOK = true;
    }
    else
    {
        creditOK = false;
    }

    return true;
}


bool CLong77Pay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");

    //参与签名，固定顺序
    Json::Value data;
    data["reqtime"] =  (Json::Int64)(req.getParamInt64("reqtime"));
    data["pid"] =  (Json::Int64)(req.getParamInt64("pid"));
    data["money"] =  (Json::Int64)(req.getParamInt64("money"));
    data["fees"] =  (Json::Int64)(req.getParamInt64("fees"));
    data["outwithdrawno"] =  req.getParam("outwithdrawno");
    data["bankno"] =  req.getParam("bankno");

    CMyString strSignData("reqtime=%ld&pid=%d&money=%ld&fees=%ld&outwithdrawno=%s&bankno=%s",
                    data["reqtime"].asInt64(), data["pid"].asInt(), data["money"].asInt64(),
                    data["fees"].asInt64(), data["outwithdrawno"].asString().c_str(), data["bankno"].asString().c_str());
    strSignData += "&";
    strSignData += pSdk->getApiKey();
    LogDebug("[%s] valid credit callback sign data:[%s]", m_sdkTag.c_str(), strSignData.c_str());

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString strSign2 = CMD5Wrapper::encode(strSignData.c_str());

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]credit callback sign mismatch, origin text [%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), strSignData.c_str(), strSign2.c_str(), strSign1.c_str());
        errNote = "credit callback sign mismatch";
        return false;
    }

    return true;
}


void CLong77Pay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    if (bSucceed)
    {
        safe_strcpy(szResp, cbResp, "success");   
    }
    else
    {
        safe_strcpy(szResp, cbResp, "fail");
    }
    cbResp = strlen(szResp);
}



