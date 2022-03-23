/*
 * =====================================================================================
 *
 *       Filename:  Q11Pay.cpp
 *
 *    Description:  Q11 BPay实时
 *
 *        Version:  1.0
 *        Created:  2021年07月02日 16时44分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#include "Q11Pay.h"
#include "Security.h"
#include "SdkList.h"
#include "GameList.h"
#include "SdkChannelList.h"
#include "MyNet.h"

CPluginQ11Pay::CPluginQ11Pay()
    : CPayBase("Q11Pay", depositRequest, httpRequestSync)
{

}

CPluginQ11Pay::~CPluginQ11Pay()
{

}

int32_t CPluginQ11Pay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CPluginQ11Pay::Update(void)
{
    return CPayBase::Init("main/q11pay");
}

void CPluginQ11Pay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CPluginQ11Pay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginQ11Pay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginQ11Pay, doWithdraw)
{
    return CPayBase::doCreditRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginQ11Pay, doWithdrawCB)
{
    return CPayBase::doCreditCallback(pClient, req, szResp, cbResp, ext_headers);
}


void CPluginQ11Pay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    HTTP_REQ_INT64(payMoney, amount);
    HTTP_REQ_STR(strUin, uin);

    Json::Value data;
    data["version"] = "V1.0";
    data["partner_id"] =  pSdk->getAccount();
    data["pay_type"] = "0001";
    data["bank_code"] = pChannel->getBankCode();
    data["order_no"] = orderNo;
    data["amount"] = CMyString("%.2lf", payMoney/100.0);
    data["return_url"] = pGame->getCallbackUrl() ;
    data["notify_url"] = pSdk->getPayCallbackUrl() ;
    data["attach"] = strUin;

    CMyString strDepositKey;
    Json::Value extraData = pSdk->getPayExtraData();
    if (!extraData.empty())
    {
        strDepositKey = extraData["depositkey"].asString();
    }
    if (strDepositKey.empty())
    {
        strDepositKey = pSdk->getApiKey();
    }

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('&', '=', true, true, false);
    str1 += "&";
    str1 += strDepositKey;

    LogDebug("[%s] CPluginQ11Pay::doMakeDepositRequest request data：%s", m_sdkTag.c_str(), str1.c_str());
    CMyString strMD5 = CMD5Wrapper::encode(str1.c_str());
    strMD5.MakeLower();
    req_maker.add("sign", strMD5);

    httpData = req_maker.http_str_manual('&', '=', false, true, true);
    url = pSdk->getPayUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}

bool CPluginQ11Pay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    LogDebug("[%s]deposit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());

    http::CHttpRespone resp;
    resp.read((void*)respHeader.c_str(), respHeader.length());
    resp.read((void*)respData.c_str(), respData.length());
    if (resp.getStatusCode() != 200)
    {
        errNote.Format("resp code is %d", resp.getStatusCode());
        return false;
    }

    if (respData.empty())
    {
        errNote.Format("not any data respone");
        return false;
    }

    depositUrlData = respData;
    depositUrlData.Erase("<script type='text/javascript'>window.location.href='");
    depositUrlData.Erase("'</script>");

    depositOrderNo = "N/A";
    CMyString strTemp = depositUrlData;
    size_t nPos = strTemp.find("?");
    if (nPos != std::string::npos)
    {
        strTemp = strTemp.substr(nPos);
        CStrSpliter css;
        css.Split(strTemp, '&');
        for(int32_t i(0); i<css.size(); ++i)
        {
            CMyString strparam = css.asString(i);
            nPos = strparam.find("order_no");
            if (nPos != std::string::npos)
            {
                CStrSpliter cssorder;
                cssorder.Split(strparam, '=');
                if(cssorder.size() == 2)
                {
                    depositOrderNo = cssorder.asString(1);
                }
                break;
            }
        }
    }
    return true;
}


bool CPluginQ11Pay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &depositOK)
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

    HTTP_REQ_STR(strCode, code);
    depositOK = (strCode == "00");

    orderNo = req.getParam("order_no");
    platformOrderNo = req.getParam("trade_no");
    fee = (int64_t)(req.getParam("amount").asDouble() *100);
    realFee = fee;
    tax = 0;

    return true;
}

bool CPluginQ11Pay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString strDepositKey;
    Json::Value extraData = pSdk->getPayExtraData();
    if (!extraData.empty())
    {
        strDepositKey = extraData["depositkey"].asString();
    }
    if (strDepositKey.empty())
    {
        strDepositKey = pSdk->getApiKey();
    }

    CMyString str1 = resp_maker.http_str_manual('&', '=', true, true, false);
    str1 += "&";
    str1 += strDepositKey;
    LogDebug("[%s] CPluginQ11Pay::validDepositCallbackSign sign data：%s", m_sdkTag.c_str(), str1.c_str());
    CMyString strSign2= CMD5Wrapper::encode(str1.c_str());

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

void CPluginQ11Pay::doDepositCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    if (bSucceed)
    {
        safe_strcpy(szResp, cbResp, "ok");   
    }
    else
    {
        safe_strcpy(szResp, cbResp, "fail");
    }
    cbResp = strlen(szResp);
}


void CPluginQ11Pay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData) 
{
    Json::Value data;
    data["version"] = "V1.0";
    data["partner_id"] =  pSdk->getAccount();
    data["order_no"] = req["orderNo"].asString();
    data["amount"] = CMyString("%.2lf", req["fee"].asInt64()/100.0); 
    data["bank_code"] = req["bankCode"].asString();
    data["account_no"] = req["bankCard"].asString();
    data["account_name"] = req["bankName"].asString();
    data["notify_url"] = pSdk->getWithdrawCallbackUrl();

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('&', '=', true, true, false);
    str1 += "&";
    str1 += pSdk->getApiKey();
    LogDebug("[%s] CPluginQ11Pay::doMakeCreditRequest request data：%s", m_sdkTag.c_str(), str1.c_str());
    CMyString strMD5 = CMD5Wrapper::encode(str1.c_str());
    strMD5.MakeLower();
    req_maker.add("sign", strMD5);

    httpData = req_maker.http_str_manual('&', '=', false, true, true);
    url = pSdk->getWithdrawUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}


bool CPluginQ11Pay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    LogDebug("[%s]credit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        errNote.Format("credit resp not valid json");
        return false;
    }

    CMyString strCode = jrRoot.getString("code");
    if (strCode != "00")
    {
        errNote = jrRoot.getString("message");
        return false;
    }

    creditOrderNo = jrRoot.getString("trade_no");
    int32_t status = jrRoot.getInt("status");
   if (status==1||status==2)
    {
        return true;
    }

    errNote.Format("credit resp fail, status:[%d]", status);
    return false;
}


bool CPluginQ11Pay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
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

    HTTP_REQ_STR(strMoney, amount);
    HTTP_REQ_STR(strOrderNo, order_no);
    HTTP_REQ_STR(strTradeNo, trade_no);
    HTTP_REQ_STR(strCode, code);

    orderNo = strOrderNo;
    creditOrderNo = strTradeNo;
    fee = (int64_t)(strMoney.asDouble() *100);
    realFee = fee;
    tax = 0;

    if (strCode != "00")
    {
        creditOK = false;
    }
    else
    {
        creditOK = true;
    }
    return true;
}


bool CPluginQ11Pay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");
    errNote = req.getParam("message");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString str1 = resp_maker.http_str_manual('&', '=', true, true, false);
    str1 += "&";
    str1 += pSdk->getApiKey();
    LogDebug("[%s] CPluginQ11Pay::validCreditCallbackSign sign data：%s", m_sdkTag.c_str(), str1.c_str());
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


void CPluginQ11Pay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    if (bSucceed)
    {
        safe_strcpy(szResp, cbResp, "ok");   
    }
    else
    {
        safe_strcpy(szResp, cbResp, "fail");
    }
    cbResp = strlen(szResp);
}


