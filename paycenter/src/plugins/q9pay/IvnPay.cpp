/*
 * =====================================================================================
 *
 *       Filename:  IvnPay.cpp
 *
 *    Description:  Q9 IvnPay
 *
 *        Version:  1.0
 *        Created:  2021年06月17日 23时04分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "IvnPay.h"
#include "Security.h"
#include "SdkList.h"
#include "GameList.h"
#include "SdkChannelList.h"
#include "MyNet.h"

CPluginIvnPay::CPluginIvnPay()
    : CPayBase("Q9Pay", depositRequest, httpRequestSync)
{

}

CPluginIvnPay::~CPluginIvnPay()
{

}

int32_t CPluginIvnPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CPluginIvnPay::Update(void)
{
    return CPayBase::Init("main/q9pay");
}

void CPluginIvnPay::Maintance(void)
{
    CPayBase::Maintance();

}

ENTRY_FUNC_DECLARE(CPluginIvnPay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginIvnPay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginIvnPay, doWithdraw)
{
    return CPayBase::doCreditRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginIvnPay, doWithdrawCB)
{
    return CPayBase::doCreditCallback(pClient, req, szResp, cbResp, ext_headers);
}


void CPluginIvnPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    Json::Value data;
    data["mch_id"] = pSdk->getAccount();
    data["mch_uid"] = req.getParam("uin");
    data["mch_order_id"] = orderNo;
    data["equipment_type"] = 2;
    data["expected_amount"] = (Json::Int64)(req.getParamInt64("amount")/100);
    data["mch_url"] = pGame->getCallbackUrl();
    data["attach"] = req.getParam("productId");

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str();
    str1 += pSdk->getApiKey();
    CMyString str2 = CMD5Wrapper::encode(str1.c_str());
    req_maker.add("hash", str2);
    CMyString str3 = req_maker.json_str(true);
    httpData = CBase64Wrapper::Encode(str3.c_str(), str3.length(), false);

    url = pSdk->getPayUrl();
    url += httpData;

    // 直接返回URL给前端
     contentType = "direct url to client";
}

bool CPluginIvnPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
#if 0    
    LogDebug("[%s]deposit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        errNote.Format("resp data not json string");
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
#endif 
    return true;
}


int32_t UrlEncode2Json(const CMyString &str, Json::Value &resp)
{
    std::vector<KvstrItem> temp;
    int32_t ret = safe_strtok(str.c_str(), temp, "&", "=");
    LogDebug("decode string [%s], kvstr item count %d", str.c_str(), ret);

    for(std::vector<KvstrItem>::iterator it(temp.begin());
            it != temp.end(); ++it)
    {
        KvstrItem &item = *it;
        //LogDebug("key:%s, value:%s", item.key.c_str(), item.value.c_str());
        resp[item.key] = item.value;
    }
    return 0;
}
bool CPluginIvnPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &depositOK)
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

    CMyString str1 = req.getParam("param");
    CMyString strParam = CBase64Wrapper::Decode(str1.c_str(), str1.length(), false);
    LogDebug("[%s]deposit callback resp is [%s]", m_sdkTag.c_str(), strParam.c_str());
    Json::Value jrParam;
    UrlEncode2Json(strParam, jrParam);
    int32_t status(-1);

    try
    {
        orderNo = jrParam["mch_order_id"].asString();
        platformOrderNo = jrParam["svr_transaction_id"].asString();
        CMyString strFee = jrParam["amount"].asString();
        fee = (int64_t)(strFee.asInt64() *100);
        realFee = fee;
        tax = 0;

        // status: 0 unpaid, 1 paid
        CMyString strStatus = jrParam["status"].asString();
        status = strStatus.asInt();
        depositOK = (status == 1);
    }
    catch(...)
    {
        LogFatal("[%s]get deposit callback failure", m_sdkTag.c_str());
    }
    LogDebug("[%s]parse deposit callback result=> orderNo: %s, depositOrderNo: %s, fee:%ld, status:%d",
            m_sdkTag.c_str(), orderNo.c_str(), platformOrderNo.c_str(), fee, status);
    return true;
}

bool CPluginIvnPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString str1 = req.getParam("param");
    CMyString strParam = CBase64Wrapper::Decode(str1.c_str(), str1.length(), false);
    LogDebug("[%s]deposit callback resp is [%s]", m_sdkTag.c_str(), strParam.c_str());
    Json::Value jrParam;
    UrlEncode2Json(strParam, jrParam);

    CMyString strSign1 = jrParam["hash"].asString();

    http::CHttpRespMaker resp_maker;
    resp_maker.AddJson(jrParam);
    resp_maker.erase("hash");
    CMyString str2 = resp_maker.http_str_manual('&', '=', false, false);
    str2 += pSdk->getApiKey();
    CMyString strSign2 = CMD5Wrapper::encode(str2.c_str());

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), str1.c_str(), strSign2.c_str(), strSign1.c_str());
        return false;
    }
    return true;
}

void CPluginIvnPay::doDepositCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "application/json";
    if (bSucceed)
    {
        safe_strcpy(szResp, cbResp, "{\"ret\": 0, \"msg\":\"OK\"}");   
    }
    else
    {
        safe_strcpy(szResp, cbResp, "{\"ret\":-1099, \"msg\":\"failure.\"}");
    }
    cbResp = strlen(szResp);
}

void CPluginIvnPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk,  CMyString& url, CMyString &contentType, CMyString& httpData) 
{
    Json::Value data;
    data["mch_id"] = pSdk->getAccount();
    data["mch_uid"] = req["uin"];
    data["mch_order_id"] = req["orderNo"];
    data["pay_type"] = 4;
    data["amount"] = (double)(req["fee"].asInt64()/100.0);
    data["notify_url"] = pSdk->getWithdrawCallbackUrl();
    data["account"] = req["bankCard"];

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str();
    str1 += pSdk->getApiKey();
    CMyString str2 = CMD5Wrapper::encode(str1.c_str());
    req_maker.add("hash", str2);
     
    httpData = req_maker.json_str(true);
    url = pSdk->getWithdrawUrl();
    contentType = "Content-Type: application/json";
}


bool CPluginIvnPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    LogDebug("[%s]credit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        errNote.Format("credit resp not valid json");
        return false;
    }

    int32_t code = jrRoot.getInt("ret");
    if (code != 0)
    {
        errNote = jrRoot.getString("msg");
        return false;
    }

    Json::Value data;
    if (!jrRoot.getJsonObject("data", data))
    {
        errNote.Format("credit resp not contrain \"data\" field");
        return false;
    }
    CJsonWrapper jrData(data);

    int32_t status = jrData.getInt("status");
    creditOrderNo = jrData.getString("svr_transaction_id");
    // 0 未代付，1-成功，2-失败
    bool bCreditReqOK(false);
    switch(status)
    {
    case 0:
    case 1:
        bCreditReqOK = true;
        break;
    case 2:
        errNote = jrData.getString("error_user_msg");
        break;
    default:
        errNote.Format("unkonw status value [%d]", status);
        break;

    }
    return bCreditReqOK;
}


bool CPluginIvnPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &loanOK)
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

    CMyString str1 = req.getParam("param");
    CMyString strParam = CBase64Wrapper::Decode(str1.c_str(), str1.length(), false);
    LogDebug("[%s]credit callback, real param[%s]", m_sdkTag.c_str(), strParam.c_str());
    Json::Value data;
    UrlEncode2Json(strParam, data);
    
    int32_t status(-1);
    try
    {
        orderNo = data["mch_order_id"].asString();
        creditOrderNo = data["svr_transaction_id"].asString();
        CMyString strFee = data["amount"].asString();
        fee = (int64_t)(strFee.asInt64() *100);
        realFee = fee;
        tax = 0;

        // status: 1 success, 0 pending, 2 reject
        CMyString strStatus = data["status"].asString();
        status = strStatus.asInt();
        loanOK = (status == 1);
    }
    catch(CException &ex)
    {
       LogFatal("[%s]credit data parse failure, msg:%s", m_sdkTag.c_str(), ex.what()); 
       return false;
    }
    catch(std::exception &ex)
    {
       LogFatal("[%s]credit data parse failure, msg:%s", m_sdkTag.c_str(), ex.what()); 
       return false;
    }
    catch(...)
    {
       LogFatal("[%s]credit data parse failure, unknown error", m_sdkTag.c_str()); 
        return false;
    }

    LogDebug("[%s]credit callback data: orderNo:%s, creditOrderNo:%s, fee:%ld, status:%d, loan:%s",
            m_sdkTag.c_str(), orderNo.c_str(), creditOrderNo.c_str(), fee, status, loanOK?"TRUE":"FALSE");
    return true;
}

bool CPluginIvnPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    errNote = req.getParam("error_no");
    CMyString str1 = req.getParam("param");
    CMyString strParam = CBase64Wrapper::Decode(str1.c_str(), str1.length(), false);
    LogDebug("[%s]credit callback, real param[%s]", m_sdkTag.c_str(), strParam.c_str());
    Json::Value data;
    UrlEncode2Json(strParam, data);
    CMyString strSign1 = data["hash"].asString();

    http::CHttpRespMaker resp_maker;
    resp_maker.AddJson(data);
    resp_maker.erase("hash");
    CMyString str2 = resp_maker.http_str_manual('&', '=', false, true, false);
    str2 += pSdk->getApiKey();
    CMyString strSign2 = CMD5Wrapper::encode(str2.c_str());

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]credit callback sign mismatch, origin text [%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), str1.c_str(), strSign2.c_str(), strSign1.c_str());
        errNote = "credit callback sign mismatch";
        return false;
    }
    return true;
}


void CPluginIvnPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "application/json";
    if (bSucceed)
    {
        safe_strcpy(szResp, cbResp, "{\"ret\":0}");   
    }
    else
    {
        safe_strcpy(szResp, cbResp, "{\"ret\":-1}");
    }
    cbResp = strlen(szResp);
}

void CPluginIvnPay::doMakeDepositQueryRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    http::CHttpRespMaker req_maker;
    req_maker.add("mch_id", pSdk->getAccount());
    req_maker.add("mch_order_id", orderNo);
    CMyString str1 = req_maker.http_str_manual('&', '=', false, true, false);
    str1 += pSdk->getApiKey();
    CMyString str2 = CMD5Wrapper::encode(str1.c_str());
    req_maker.add("hash", str2);

    url = pSdk->getPayCheckUrl();
    httpData = req_maker.json_str(true);
    contentType = "Content-Type: application/json";
}

bool CPluginIvnPay::doParseDepositQueryResp(CGame *pGame, const CMyString &respHeader, const CMyString &respData, Json::Value &resp,  CMyString &errNote)
{
    LogDebug("[%s]deposit query resp [%s] header [%s]", m_sdkTag.c_str(), respData.c_str(), respHeader.c_str());

    CJsonWrapper jr;
    if (!jr.Init(respData))
    {
        errNote = "deposit resp not valid json";
        return false;
    }
    int32_t code = jr.getInt("ret");
    if (code != 0)
    {
        errNote = jr.getString("msg");
        return false;
    }
    
    Json::Value data;
    if (!jr.getJsonObject("data", data))
    {
        errNote = "deposit resp not contain \"data\" field";
        return false;
    }
    CJsonWrapper jrData(data);
    
    resp["orderNo"] = jrData.getString("mch_order_id");
    resp["depositOrderNo"] = jrData.getString("svr_transaction_id");
    resp["fee"] = (Json::Int64)jrData.getInt64("amount")*100;
    resp["payType"] = jrData.getInt("pay_type");
    resp["attach"] = jrData.getString("attach");
    // 0-未支付， 1-成功， 2-失败
    resp["status"] = jrData.getInt("status"); 

    return true;
}

void CPluginIvnPay::doMakeCreditQueryRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    http::CHttpRespMaker req_maker;
    req_maker.add("mch_id", pSdk->getAccount());
    req_maker.add("mch_order_id", orderNo);
    CMyString str1 = req_maker.http_str_manual('&', '=', false, true, false);
    str1 += pSdk->getApiKey();
    CMyString str2 = CMD5Wrapper::encode(str1.c_str());
    req_maker.add("hash", str2);

    url = pSdk->getWithdrawCheckUrl();
    httpData = req_maker.json_str(true);
    contentType = "Content-Type: application/json";
}

bool CPluginIvnPay::doParseCreditQueryResp(CGame *pGame, const CMyString &respHeader, const CMyString &respData, Json::Value &resp,  CMyString &errNote)
{
    LogDebug("[%s]credit query resp [%s] header [%s]", m_sdkTag.c_str(), respData.c_str(), respHeader.c_str());

    CJsonWrapper jr;
    if (!jr.Init(respData))
    {
        errNote = "deposit resp not valid json";
        return false;
    }
    int32_t code = jr.getInt("ret");
    if (code != 0)
    {
        errNote = jr.getString("msg");
        return false;
    }
    
    Json::Value data;
    if (!jr.getJsonObject("data", data))
    {
        errNote = "deposit resp not contain \"data\" field";
        return false;
    }
    CJsonWrapper jrData(data);
    
    resp["orderNo"] = jrData.getString("mch_order_id");
    resp["bankCard"] = jrData.getString("account");
    resp["payType"] = jrData.getInt("pay_type");
    resp["fee"] = (Json::Int64)jrData.getInt64("amount")*100;
    resp["attach"] = jrData.getString("attach");
    // 0-未支付， 1-成功， 2-失败
    int32_t status = jrData.getInt("status");
    resp["status"] = status;
    if (status != 1)
    {
        resp["errno"] = jrData.getInt("error_no");
        resp["errmsg"] = jrData.getString("error_user_msg");
    }

    return true;
}

