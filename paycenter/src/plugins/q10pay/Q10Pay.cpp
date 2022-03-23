/*
 * =====================================================================================
 *
 *       Filename:  Q10Pay.cpp
 *
 *    Description:  Q10 BPay实时
 *
 *        Version:  1.0
 *        Created:  2021年06月16日 15时58分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Q10Pay.h"
#include "Security.h"
#include "SdkList.h"
#include "GameList.h"
#include "SdkChannelList.h"
#include "MyNet.h"

CPluginQ10Pay::CPluginQ10Pay()
    : CPayBase("Q10Pay", depositRequest, httpRequestSync)
{

}

CPluginQ10Pay::~CPluginQ10Pay()
{

}

int32_t CPluginQ10Pay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CPluginQ10Pay::Update(void)
{
    return CPayBase::Init("main/q10pay");
}

void CPluginQ10Pay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CPluginQ10Pay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginQ10Pay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginQ10Pay, doWithdraw)
{
    return CPayBase::doCreditRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginQ10Pay, doWithdrawCB)
{
    return CPayBase::doCreditCallback(pClient, req, szResp, cbResp, ext_headers);
}


void CPluginQ10Pay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    Json::Value params;
    params["merchant_ref"] = orderNo;
    params["product"] = pChannel->getBankCode();
    params["amount"] = CMyString("%.2lf", req.getParamInt64("amount")/100.0);
    CJsonWrapper wr;
    CMyString strParam = wr.write(params);
    
    Json::Value data;
    data["merchant_no"] = pSdk->getAccount();
    data["timestamp"] = (Json::Int64)time(NULL);
    data["sign_type"] = "MD5";
    data["params"] = strParam;

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_values('\0', true, false);
    str1 += pSdk->getApiKey();
    CMyString str2 = CMD5Wrapper::encode(str1.c_str());
    str1.MakeLower();
    req_maker.add("sign", str2);
    httpData = req_maker.http_str_manual('&', '=', false, true, false);

    url = pSdk->getPayUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}

bool CPluginQ10Pay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    LogDebug("[%s]deposit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        errNote.Format("resp data [%s] not json string", respData.c_str());
        return false;
    }

    CMyString strCode = jrRoot.getString("code");
    if (strCode != "200")
    {
        errNote = jrRoot.getString("message");
        return false;
    }

    CMyString strParam = jrRoot.getString("params");
    CJsonWrapper jrParam;
    if (!jrParam.Init(strParam))
    {
        errNote.Format("params [%s] not valid json", strParam.c_str());
        return false;
    }

    depositOrderNo = jrParam.getString("system_ref");
    depositUrlData = jrParam.getString("payurl");
    return true;
}


bool CPluginQ10Pay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &depositOK)
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

    CMyString str1 = req.getParam("params");
    CMyString strParam;
    CHttpWrapper::urlDecode(str1, strParam);
    strParam.Replace("+", " ");
    LogDebug("[%s]deposit callback resp is [%s]", m_sdkTag.c_str(), strParam.c_str());
    CJsonWrapper jrParam;
    if (!jrParam.Init(strParam))
    {
        LogErr("[%s]deposit callback corrupted, params [%s] not valid json", 
                m_sdkTag.c_str(), strParam.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]deposit callback corrupted, params [%s] not valid json", 
                m_sdkTag.c_str(), strParam.c_str()));
        return false;
    }

    orderNo = jrParam.getString("merchant_ref");
    platformOrderNo = jrParam.getString("system_ref");
    fee = (int64_t)(jrParam.getString("amount").asDouble() *100);
    realFee = (int64_t)(jrParam.getString("pay_amount").asDouble()*100);
    tax = (int64_t)(jrParam.getString("fee").asDouble() *100);

    // status: 0 unpaid, 1 paid
    int32_t status = jrParam.getInt("status");
    depositOK = (status == 1);
    return true;
}

bool CPluginQ10Pay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString str1 = resp_maker.http_str_values('\0', true, false);
    str1.Replace("+", " ");
    str1 += pSdk->getApiKey();
    CMyString strSign2 = CMD5Wrapper::encode(str1.c_str());

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), str1.c_str(), strSign2.c_str(), strSign1.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]", 
                m_sdkTag.c_str(), str1.c_str(), strSign2.c_str(), strSign1.c_str()));
        return false;
    }
    return true;
}

void CPluginQ10Pay::doDepositCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    if (bSucceed)
    {
        safe_strcpy(szResp, cbResp, "SUCCESS");   
    }
    else
    {
        safe_strcpy(szResp, cbResp, "FAIL");
    }
    cbResp = strlen(szResp);
}


void CPluginQ10Pay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData) 
{
    static CMyString PRODUCT_THAIPAYOUT = "ThaiPayout";
    static CMyString PRODUCT_THAISETTLEMENT = "ThaiSettlement";

    Json::Value params;
    params["merchant_ref"] = req["orderNo"].asString();
    params["product"] = PRODUCT_THAIPAYOUT;  // TODO: 需要改到DB配置
    params["amount"] = CMyString("%.2lf", req["fee"].asInt64()/100.0); 
    Json::Value extra_param;
    extra_param["account_name"] = req["bankName"].asString();
    extra_param["account_no"] = req["bankCard"].asString();
    extra_param["bank_code"] = req["bankCode"].asString();
    params["extra"] = extra_param;
    //params["extend_params"] = req["uin"].asString();
    CJsonWrapper wr;
    CMyString strParam = wr.write(params);

    Json::Value data;
    data["merchant_no"] = pSdk->getAccount();
    data["timestamp"] = (Json::Int64)::time(NULL);
    data["sign_type"] = "MD5";
    data["params"] = strParam;

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_values('\0', true, false);
    str1 += pSdk->getApiKey();
    CMyString str2 = CMD5Wrapper::encode(str1.c_str());
    str2.MakeLower();
    req_maker.add("sign", str2);
     
    httpData = req_maker.http_str_manual('&', '=', false, true, false);
    url = pSdk->getWithdrawUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}


bool CPluginQ10Pay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    LogDebug("[%s]credit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        errNote.Format("credit resp not valid json");
        return false;
    }

    CMyString strCode = jrRoot.getString("code");
    if (strCode != "200")
    {
        errNote = jrRoot.getString("message");
        return false;
    }

    CMyString strParam = jrRoot.getString("params");
    CJsonWrapper jrParam;
    if (!jrParam.Init(strParam))
    {
        errNote.Format("params [%s] not valid json", strParam.c_str());
        return false;
    }

    creditOrderNo = jrParam.getString("system_ref");
    int32_t status = jrParam.getInt("status");
    if (status==1||status==2)
    {
        return true;
    }
    errNote.Format("credit resp fail, status:[%d]", status);
    return false;
}


bool CPluginQ10Pay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
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

    CMyString str1 = req.getParam("params");
    CMyString strParam;
    CHttpWrapper::urlDecode(str1, strParam);
    strParam.Replace("+", " ");
    CJsonWrapper jrParam;
    if (!jrParam.Init(strParam))
    {
        LogErr("[%s]credit callback corrupted, params [%s] not valid json", 
                m_sdkTag.c_str(), strParam.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]credit callback corrupted, params [%s] not valid json", 
                m_sdkTag.c_str(), strParam.c_str()));
        return false;
    }

    orderNo = jrParam.getString("merchant_ref");
    creditOrderNo = jrParam.getString("system_ref");
    fee = (int64_t)(jrParam.getString("amount").asDouble() *100);
    realFee = (int64_t)(jrParam.getString("pay_amount").asDouble() *100);
    tax = (int64_t)(jrParam.getString("fee").asDouble() *100);

    // status: 1 success, 2 pending, 5 reject
    int32_t status = jrParam.getInt("status");
    creditOK = (status == 1);

    LogDebug("[%s]credit callback: orderNo:%s, creditOrderNo:%s, fee:%ld, realFee:%ld, tax:%ld, status:%d",
            m_sdkTag.c_str(), orderNo.c_str(), creditOrderNo.c_str(), fee, realFee, tax, status);
    return true;
}


bool CPluginQ10Pay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString str1 = resp_maker.http_str_values('\0', true, false);
    str1.Replace("+", " ");
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


void CPluginQ10Pay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
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


