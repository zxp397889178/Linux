/*
 * =====================================================================================
 *
 *       Filename:  Q8Pay.cpp
 *
 *    Description:  Q9支付，PayNow支付
 *
 *        Version:  1.0
 *        Created:  2021年06月16日 00时12分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Q8Pay.h"
#include "Security.h"
#include "SdkList.h"
#include "GameList.h"
#include "SdkChannelList.h"
#include "MyNet.h"

CPluginQ8Pay::CPluginQ8Pay()
    : CPayBase("Q8Pay", depositRequest, httpRequestSync)
{

}

CPluginQ8Pay::~CPluginQ8Pay()
{
    
}

int32_t CPluginQ8Pay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CPluginQ8Pay::Update(void)
{
    int32_t ret = CPayBase::Init("main/q8pay");
    return ret;
}

void CPluginQ8Pay::Maintance(void)
{
    CPayBase::Maintance();

}

ENTRY_FUNC_DECLARE(CPluginQ8Pay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginQ8Pay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginQ8Pay, doWithdraw)
{
    return CPayBase::doCreditRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginQ8Pay, doWithdrawCB)
{
    return CPayBase::doCreditCallback(pClient, req, szResp, cbResp, ext_headers);
}


void CPluginQ8Pay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    Json::Value data;
    data["app_id"] = pSdk->getAccount();
    data["app_order_no"] = orderNo;
    //data["type_id"] = ;
    //data["bank_id"] = ;
    data["app_user"] = req.getParam("uin");    // TODO: 需要配置或指定
    char szTime[64] = "";
    TimeToStr(time(NULL), szTime, sizeof(szTime));
    data["app_order_time"] = szTime;
    data["amount"] = (Json::Int64)(req.getParamInt64("amount")/100);
    data["notify_url"] = pSdk->getPayCallbackUrl();
//  data["note"] = req.getParam("uin");
    data["client_ip"] = req.getParam("playerIp");

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('&', '=', true, true, false);
    str1 += pSdk->getApiKey();
    CMyString str2 = CMD5Wrapper::encode(str1.c_str());
    str2.MakeLower();
    req_maker.add("sign", str2);
     
    httpData = req_maker.http_str_manual('&', '=', false, true, true);
    url = pSdk->getPayUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}

bool CPluginQ8Pay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    LogDebug("[%s]deposit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        errNote.Format("resp data not json string");
        return false;
    }

    int32_t success = jrRoot.getInt("success");
    if (success != 1)
    {
        errNote = jrRoot.getString("message");
        return false;
    }

    depositOrderNo = jrRoot.getString("app_order_no");
    depositUrlData = jrRoot.getString("pay_url");

    return true;
}


bool CPluginQ8Pay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &depositOK)
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

    int32_t success= req.getParamInt("success");
    if (success != 1)
    {
        depositOK = false;
    }
    else
    {
        depositOK = true;
    }

    orderNo = req.getParam("app_order_no");
    platformOrderNo = req.getParam("order_no");
    fee = (int64_t)(req.getParam("amount").asDouble() *100);
    realFee = (int64_t)(req.getParam("pay_amount").asDouble()*100);
    tax = 0;
    return true;
}

bool CPluginQ8Pay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString str1 = resp_maker.http_str_manual('&', '=', true, true, false);
    str1 += pSdk->getApiKey();
    CMyString strSign2 = CMD5Wrapper::encode(str1.c_str());

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), str1.c_str(), strSign2.c_str(), strSign1.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]deposit callback sign mismatch, origin text [%s], sign[%s], sign from platform[%s]", 
                m_sdkTag.c_str(), str1.c_str(), strSign2.c_str(), strSign1.c_str()));
        return false;
    }
    return true;
}

void CPluginQ8Pay::doDepositCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
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


void CPluginQ8Pay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk,  CMyString& url, CMyString &contentType, CMyString& httpData) 
{
    Json::Value data;
    data["app_id"] = pSdk->getAccount();
    data["app_order_no"] = req["orderNo"].asString();
    data["app_user"] = req["uin"].asString(); // TODO: 需要配置或指定
    char szTime[64] = "";
    TimeToStr(time(NULL), szTime, sizeof(szTime));
    data["app_order_time"] = szTime;
    data["amount"] = (Json::Int64)(req["fee"].asInt64()/100);
    data["card_name"] = req["bankName"].asString();
    data["card_no"] = req["bankCard"].asString();
    data["bank_id"] = req["bankCode"].asString();
    data["branc_name"] = "others";
    data["notify_url"] = pSdk->getWithdrawCallbackUrl();
    //data["note"] = req["uin"].asString();
    //data["client_ip"] = req.getParamStr("playerIp");

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('&', '=', true, true, false);
    str1 += pSdk->getApiKey();
    LogDebug("[%s] CPluginQ8Pay::doMakeCreditRequest request data：%s", m_sdkTag.c_str(), str1.c_str());
    CMyString str2 = CMD5Wrapper::encode(str1.c_str());
    str2.MakeLower();
    req_maker.add("sign", str2);
     
    httpData = req_maker.http_str_manual('&', '=', true, true, true);
    url = pSdk->getWithdrawUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}


bool CPluginQ8Pay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    LogDebug("[%s]credit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        errNote.Format("credit resp not valid json");
        return false;
    }

    int32_t success = jrRoot.getInt("success");
    if (success != 1)
    {
        errNote = jrRoot.getString("message");
        return false;
    }

    creditOrderNo = jrRoot.getString("app_order_no");

    return true;
}


bool CPluginQ8Pay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
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

    orderNo = req.getParam("app_order_no");
    creditOrderNo = req.getParam("order_no");
    realFee = (int64_t)(req.getParam("pay_amount").asDouble()*100);
    fee = realFee;
    tax = 0;

    int32_t success = req.getParamInt("success");
    if (1 == success)
    {
        creditOK = true;
    }
    else
    {
        creditOK = false;
    }
    return true;
}


bool CPluginQ8Pay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");
    errNote = req.getParam("message");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString str1 = resp_maker.http_str_manual('&', '=', true, true, false);
    str1 += pSdk->getApiKey();
    CMyString strSign2 = CMD5Wrapper::encode(str1.c_str());

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]credit callback sign mismatch, origin text [%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), str1.c_str(), strSign2.c_str(), strSign1.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]credit callback sign mismatch, origin text [%s], sign[%s], sign from platform[%s]", 
                m_sdkTag.c_str(), str1.c_str(), strSign2.c_str(), strSign1.c_str()));
        errNote = "credit callback sign mismatch";
        return false;
    }
    return true;
}


void CPluginQ8Pay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
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

