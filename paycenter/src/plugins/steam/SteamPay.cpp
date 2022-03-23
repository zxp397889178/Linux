/*
 * =====================================================================================
 *
 *       Filename:  SteamPay.cpp
 *
 *    Description:  Steam平台充值
 *
 *        Version:  1.0
 *        Created:  2022年01月15日 11时17分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "SteamPay.h"
#include "SdkList.h"
#include "SdkChannelList.h"
#include "BillPointList.h"
#include "Config.h"

#define Retry_Time 5

CSteamPay::CSteamPay()
    : CPayBase("SteamPay", depositRequest, httpRequestSync)
{

}

CSteamPay::~CSteamPay()
{

}

int32_t CSteamPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CSteamPay::Update(void)
{
    CXmlFile& xml = CONFIG.getXml();
    CMyString sect = "main/steampay/config/getuserinfo_url";
    int32_t nRet = xml.getString(sect, "url", m_strGetUserInfoUrl);
    if (nRet != 0)
    {
        return -1;
    }

    CMyString sect2 = "main/steampay/config/finalizetxn_url";
    nRet = xml.getString(sect2, "url", m_strFinalizeTxnUrl);
    if (nRet != 0)
    {
        return -2;
    }

    CMyString sect3 = "main/steampay/steamsdk/callback";
    if (!CPayBase::InitRedisCache(m_cacheSteamOrder, m_redis, xml, sect3))
    {
        return -3;
    }

    CMyString sect4 = "main/steampay/config/querytxn_url";
    nRet = xml.getString(sect4, "url", m_strQueryTxnUrl);
    if (nRet != 0)
    {
        return -4;
    }

    return CPayBase::Init("main/steampay");
}

void CSteamPay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CSteamPay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CSteamPay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CSteamPay, doVerify)
{
    return CPayBase::doDepositVerify(pClient, req, szResp, cbResp, ext_headers);
}

void CSteamPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    CMyString strAppId, strAppKey;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strAppId = extraData["appid"].asString();
        strAppKey = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strAppKey: %s", __FILE__, __func__, __LINE__, strAppKey.c_str());
    }
    if (strAppKey.empty())
    {
        strAppKey = pSdk->getApiKey();
    }

    int64_t nSteamId = req.getParamInt64("account");
    CMyString strIp = req.getParam("playerIp");
    CMyString strCurrency;
    GetUserInfo(strAppKey, nSteamId, strIp, strCurrency);

    Json::Value root;
    root["key"] = strAppKey;
    root["orderid"] = TransOrder(orderNo);
    root["steamid"] = req.getParamInt64("account");
    root["appid"] = strAppId.asUint();
    root["itemcount"] = 1;
    root["language"] = "zh";
    root["currency"] = strCurrency;
    //root["usersession"] = "web";
    //root["ipaddress"] = strIp;

    CMyString strSubject;
    int32_t bpId = req.getParamInt("bpId");
    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    if (NULL != pBP)
    {
        strSubject = pBP->getBpCode();
    }
    else
    {
        LogErr("[%s]CSteamPay::doMakeDepositRequest >not found billpoint %d", m_sdkTag.c_str(), bpId);
    }

    root["itemid[0]"] = bpId;
    root["qty[0]"] = 1;
    int64_t nAmount = req.getParamInt64("amount");
    root["amount[0]"] = nAmount;
    root["description[0]"] = strSubject;

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(root);

    httpData = httpData = req_maker.http_str_manual('&', '=', false, false, true);
    url = pSdk->getPayUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}

bool CSteamPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    LogDebug("[%s]deposit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());
    CJsonWrapper jrRoot;
    if (!jrRoot.Load(respData))
    {
        errNote.Format("resp data [%s] not json string", respData.c_str());
        LogErr("[%s][%s:%s:%d]resp data not json string", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        return false;
    }

    Json::Value response;
    if (!jrRoot.getJsonObject("response", response))
    {
        errNote.Format("response [%s] not object", respData.c_str());
        LogErr("[%s][%s:%s:%d]response not object", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        return false;
    }

    CMyString strResult = response["result"].asString();
    if (strResult != "OK")
    {
        int32_t nErrorCode = response["error"]["errorcode"].asInt();
        CMyString strError = response["error"]["errordesc"].asString();
        
        errNote.Format("errorcode:%d, errordesc:%s", nErrorCode, strError.c_str());
        LogErr("[%s][%s:%s:%d]strResult != OK, result :%s, errcode:%d, errdesc:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strResult.c_str(), nErrorCode, strError.c_str());
        return false;
    }

    depositOrderNo =response["params"]["orderid"].asString();
    depositUrlData = response["params"]["steamurl"].asString();
    return true;
}

//验证
void CSteamPay::addDepositVerifyNeededParam(std::vector<CMyString> &vecParam)
{
    vecParam.push_back("orderid");
}

void CSteamPay::storeDepositVerifyData(CGame *pGame, CSdk *pSdk, http::CHttpRequest &req, Json::Value &data)
{
    data["orderid"] = req.getParamInt64("orderid");

    CMyString strAppId, strAppKey;
    Json::Value extraData = pSdk->getPayExtraData();
    if (!extraData.empty())
    {
        strAppId = extraData["appid"].asString();
        strAppKey = extraData["appkey"].asString();
    }
    data["appid"] = strAppId.asUint64();
    data["appkey"] = strAppKey;
    data["retry_time"] = Retry_Time;

    // 保存到cache中，稍后发送
    int32_t nRet = m_cacheSteamOrder.Push(data);
    if (nRet < 0)
    {
        LogErr("[%s] SaveData error[%s]", m_sdkTag.c_str(), m_redis.getLastError());
        this->postAlarm(Alarm_Err, CMyString("[%s] SaveData error[%s]", 
                m_sdkTag.c_str(), m_redis.getLastError()));
    }
    else
    {
        LogMsg("[%s] SaveData success", m_sdkTag.c_str());
    }
}

int32_t CSteamPay::verifyDepositRequest(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode)
{ 
    CMyString strKey, strData;
    int32_t nRet = 0;
    int32_t ret = m_cacheSteamOrder.Pop(strKey, strData);
    if (ret > 0)
    {
        LogDebug("[%s]getIncrData ret:[%d] strKey:[%s] strData:[%s]", m_sdkTag.c_str(), ret, strKey.c_str(), strData.c_str());
        
        CJsonWrapper jr;
        if (!jr.parse(strData))
        {
            LogDebug("[%s] dealCallBack parse fail", m_sdkTag.c_str());
            return 0;
        }
        Json::Value &root = jr.json();
        int32_t nRetryTime = root["retry_time"].asInt();
        if (nRetryTime > 0)
        {
            req_data = root;
            ret = verifySteamOrder(req_data, depositOK, platformOrderNo, realFee, regionCode);
            if (ret == 0)
            {
                nRet = 1;
            }
            else
            {
                nRet = -1;

                nRetryTime = nRetryTime -1;
                LogMsg("[%s][%s:%s:%d] retry time %d.", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, nRetryTime);
                root["retry_time"] = nRetryTime;
                CJsonWrapper jw(root);
                CMyString strValue = jw.toString();
                m_cacheSteamOrder.Push(root);
            }
        }
        else
        {
            CMyString strUin = root["uin"].asString();
            CMyString strOrderNo = root["orderNo"].asString();
            int64_t nOrderId = root["orderid"].asUInt64();
            LogErr("[%s][%s:%s:%d] retry verifySteamOrder %d time fail. uin:%s, game orderNo:%s, steam orderid:%ld", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, Retry_Time, strUin.c_str(), strOrderNo.c_str(), nOrderId);
            this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d] retry verifySteamOrder %d time fail. uin:%s, game orderNo:%s, steam orderid:%ld", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, Retry_Time, strUin.c_str(), strOrderNo.c_str(), nOrderId));
        }
    }

    return nRet;
}

bool CSteamPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
{
    return true;
}

bool CSteamPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    return true;
}

void CSteamPay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    
}

void CSteamPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CSteamPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CSteamPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CSteamPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CSteamPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}

bool CSteamPay::GetUserInfo(const CMyString& strKey, int64_t& nSteamId, const CMyString& strIp, CMyString& strCurrency)
{
    strCurrency = "CNY";
    if (m_strGetUserInfoUrl.empty())
    {
        return false;
    }

    CMyString strUrl = m_strGetUserInfoUrl;
    strUrl.Append("?key=%s&steamid=%d&ipaddress=%s", strKey.c_str(), nSteamId, strIp.c_str());

    CMyString strResp;
    CMyString strRespHeader;
    int64_t httpBegin=Now(true);
    http::CHttpClient net;
    int32_t nRet = net.HttpGet(strUrl.c_str(), strResp, strRespHeader);
    int64_t cost = Now(true) - httpBegin;
    if (nRet == 0)
    {
        LogMsg("[%s][%s:%s:%d] url:%s, strResp:%s, strRespHeader:%s, cost:%.2lf",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), strResp.c_str(), strRespHeader.c_str(), cost/1000.0f);

        do
        {
            CJsonWrapper jrRoot;
            if (!jrRoot.Load(strResp))
            {
                LogErr("[%s][%s:%s:%d]resp data not json string", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            Json::Value response;
            if (!jrRoot.getJsonObject("response", response))
            {
                LogErr("[%s][%s:%s:%d]response not object", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            CMyString strResult = response["result"].asString();
            if (strResult != "OK")
            {
                int32_t nErrorCode = response["error"]["errorcode"].asInt();
                CMyString strError = response["error"]["errordesc"].asString();
                LogErr("[%s][%s:%s:%d]strResult != OK, result :%s, errcode:%d, errdesc:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strResult.c_str(), nErrorCode, strError.c_str());
                break;
            }
            
            strCurrency = response["params"]["currency"].asString();

        } while (false);
    }
    else
    {
        CMyString strErrNote;
        net.getError(nRet, strErrNote);  
        LogErr("[%s][%s:%s:%d] url:%s, strResp:%s, err:%d, msg:%s, cost:%.2lf",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), strResp.c_str(), nRet, strErrNote.c_str(), cost/1000.0f);
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d] url:%s, strResp:%s, err:%d, msg:%s, cost:%.2lf", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), strResp.c_str(), nRet, strErrNote.c_str(), cost/1000.0f));
    }

    return false;
}

int64_t CSteamPay::TransOrder(const CMyString& orderNo)
{
    CMyString strOrder;
    for (size_t i = 0; i < orderNo.length(); ++i)
    {
        char temp = orderNo.at(i);
        int n = temp;
        if (n >= 48 && n <= 57)
        {
            strOrder.Append("%c", temp);
        }
        else if (n >= 97 && n <= 102)
        {
            int ntemp = n % 96;
            strOrder.Append("%d", ntemp);
        }
    }

    LogMsg("[%s][%s:%s:%d] orderNo:%s, steamOrder: %s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), strOrder.c_str());
    return strOrder.asUint64();
}

int32_t CSteamPay::verifySteamOrder(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode)
{
    CMyString strKey = req_data["appkey"].asString();
    int64_t orderId = req_data["orderid"].asUInt64();
    int64_t appId = req_data["appid"].asUInt64();
    CMyString strStatus;

    bool bRet = QueryTxn(strKey, appId, orderId, realFee, strStatus);
    if (bRet && strStatus == "Approved") // 用户批准了订单。
    {
        bRet = FinalizeTxn(strKey, appId, orderId, depositOK, platformOrderNo);
        if (!bRet)
        {
            return -1;
        }

        bRet = QueryTxn(strKey, appId, orderId, realFee, strStatus);
        if (!bRet || strStatus != "Succeeded") //订单已成功处理。
        {
            return -2;
        }
    }
    else if (bRet && strStatus == "Succeeded")  //再次查询已付款返回成功
    {
        return 0;
    }
    else
    {
        return -3;
    }
    
    return 0;
}

bool CSteamPay::FinalizeTxn(const CMyString& strKey, uint32_t appid, uint64_t orderid, bool &depositOK, CMyString &platformOrderNo)
{
    depositOK = false;
    if (m_strFinalizeTxnUrl.empty())
    {
        return false;
    }

    Json::Value root;
    root["key"] = strKey;
    root["orderid"] = orderid;
    root["appid"] = appid;
    http::CHttpRespMaker req_maker;
    req_maker.AddJson(root);

    CMyString httpData = req_maker.http_str_manual('&', '=', false, false, true);
    CMyString strResp;
    CMyString strRespHeader;
    CMyString contentType = "Content-Type: application/x-www-form-urlencoded";
    http::VEC_STRING header_list;
    header_list.push_back(contentType);
    int64_t httpBegin=Now(true);

    http::CHttpClient net;
    int32_t nRet = net.HttpPostWithHeaders(m_strFinalizeTxnUrl.c_str(), strResp, strRespHeader, header_list, httpData.c_str());
    int64_t cost = Now(true) - httpBegin;
    if (nRet == 0)
    {
        LogMsg("[%s][%s:%s:%d] url:%s, httpdata:%s, strResp:%s, strRespHeader:%s, cost:%.2lf",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strFinalizeTxnUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str(), cost/1000.0f);

        do
        {
            CJsonWrapper jrRoot;
            if (!jrRoot.Load(strResp))
            {
                LogErr("[%s][%s:%s:%d]resp data not json string", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            Json::Value response;
            if (!jrRoot.getJsonObject("response", response))
            {
                LogErr("[%s][%s:%s:%d]response not object", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            CMyString strResult = response["result"].asString();
            if (strResult != "OK")
            {
                int32_t nErrorCode = response["error"]["errorcode"].asInt();
                CMyString strError = response["error"]["errordesc"].asString();
                LogErr("[%s][%s:%s:%d]strResult != OK, result :%s, errcode:%d, errdesc:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strResult.c_str(), nErrorCode, strError.c_str());
                break;
            }
            
            platformOrderNo = response["params"]["orderid"].asString();
            depositOK = true;
            return true;

        } while (false);
    }
    else
    {
        CMyString strErrNote;
        net.getError(nRet, strErrNote);  
        LogErr("[%s][%s:%s:%d] url:%s, httpdata:%s, strResp:%s, err:%d, msg:%s, cost:%.2lf",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strFinalizeTxnUrl.c_str(), httpData.c_str(), strResp.c_str(), nRet, strErrNote.c_str(), cost/1000.0f);
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d] url:%s, httpdata:%s, strResp:%s, err:%d, msg:%s, cost:%.2lf", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strFinalizeTxnUrl.c_str(), httpData.c_str(), strResp.c_str(), nRet, strErrNote.c_str(), cost/1000.0f));
    }

    return false;
}

bool CSteamPay::QueryTxn(const CMyString& strKey, uint32_t appid, uint64_t orderid, int64_t &realFee, CMyString& strStatus)
{
    if (m_strQueryTxnUrl.empty())
    {
        return false;
    }

    CMyString strUrl = m_strQueryTxnUrl;
    strUrl.Append("?key=%s&appid=%d&orderid=%ld", strKey.c_str(), appid, orderid);

    CMyString strResp;
    CMyString strRespHeader;
    int64_t httpBegin=Now(true);
    http::CHttpClient net;
    int32_t nRet = net.HttpGet(strUrl.c_str(), strResp, strRespHeader);
    int64_t cost = Now(true) - httpBegin;
    if (nRet == 0)
    {
        LogMsg("[%s][%s:%s:%d] url:%s, strResp:%s, strRespHeader:%s, cost:%.2lf",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), strResp.c_str(), strRespHeader.c_str(), cost/1000.0f);

        do
        {
            CJsonWrapper jrRoot;
            if (!jrRoot.Load(strResp))
            {
                LogErr("[%s][%s:%s:%d]resp data not json string", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            Json::Value response;
            if (!jrRoot.getJsonObject("response", response))
            {
                LogErr("[%s][%s:%s:%d]response not object", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            CMyString strResult = response["result"].asString();
            if (strResult != "OK")
            {
                int32_t nErrorCode = response["error"]["errorcode"].asInt();
                CMyString strError = response["error"]["errordesc"].asString();
                LogErr("[%s][%s:%s:%d]strResult != OK, result :%s, errcode:%d, errdesc:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strResult.c_str(), nErrorCode, strError.c_str());
                break;
            }
            
            Json::Value items = response["params"]["items"];
            if (items.isArray())
            {
                Json::Value::iterator iter = items.begin();
                Json::Value& item = *iter;
                realFee = item["amount"].asInt64();
            }

            strStatus = response["params"]["status"].asString();
            // if (strStatus != "Succeeded")
            // {
            //     break;
            // }

            return true;

        } while (false);
    }
    else
    {
        CMyString strErrNote;
        net.getError(nRet, strErrNote);  
        LogErr("[%s][%s:%s:%d] url:%s, strResp:%s, err:%d, msg:%s, cost:%.2lf",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), strResp.c_str(), nRet, strErrNote.c_str(), cost/1000.0f);
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d] url:%s, strResp:%s, err:%d, msg:%s, cost:%.2lf", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), strResp.c_str(), nRet, strErrNote.c_str(), cost/1000.0f));
    }

    return false;
}