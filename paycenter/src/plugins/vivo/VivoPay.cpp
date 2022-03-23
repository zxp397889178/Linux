/*
 * =====================================================================================
 *
 *       Filename:  VivoPay.cpp
 *
 *    Description:  VIVO支付渠道
 *
 *        Version:  1.0
 *        Created:  2021年08月16日 17时49分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "VivoPay.h"
#include "SdkList.h"
#include "BillPointList.h"
#include "SdkChannelList.h"

CVivoPay::CVivoPay()
    : CPayBase("VivoPay", depositVerify, httpRequestSync)
{

}

CVivoPay::~CVivoPay()
{

}

int32_t CVivoPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CVivoPay::Update(void)
{
    return CPayBase::Init("main/vivopay");
}

void CVivoPay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CVivoPay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CVivoPay, doGetSign)
{
    // 检查是否合法的paycenter请求
    int32_t nRet = 0;
    CMyString strOrderNo;
    CSdkChannel *pChannel(NULL);
    CSdk *pSdk(NULL);
    CGame *pGame(NULL);
    Json::Value req_data;
    Json::Value resp;
    CMyString strErrMsg;

    bool ret = req.getParam("cpOrderNumber", strOrderNo);
    do {
        if (!ret || strOrderNo.empty())
        {
            nRet = err_depositMissParameter;
            strErrMsg = "not found field \"cpOrderNumber\" in http req parameter";
            LogWarn("[%s]not found field \"cpOrderNumber\" in http req parameter.", m_sdkTag.c_str());
            break;
        }

        // 查询订单数据
        if (!this->fetchDepositOrderData(strOrderNo, req_data))
        {
            nRet = err_depositDataLost;
            strErrMsg.Format("not found orderno %s", strOrderNo.c_str());
            LogWarn("[%s]not found orderno %s", m_sdkTag.c_str(), strOrderNo.c_str());
            // 查无此订单号
            break;
        }

        int32_t channelId = req_data["channelId"].asInt();
        pChannel = SDKCHANNEL_LIST.Find(channelId);
        if (NULL == pChannel)
        {
            nRet = err_depositParameterFault;
            strErrMsg.Format("channelId %d not avaiable in system", channelId);
            LogWarn("[%s]channelId %d not avaiable in system.", m_sdkTag.c_str(), channelId);
            break;
        }

        pSdk = pChannel->getSdk();
        pGame = pChannel->getGame();
        if (NULL == pSdk || NULL==pGame)
        {
            nRet = err_depositSystemCorrupted;
            strErrMsg = "CSdkChannel object maybe corrupted when user request deposit";
            LogFatal("[%s]CSdkChannel object maybe corrupted when user request deposit.", m_sdkTag.c_str());
            break;
        }
    }while(0);

    if (nRet != 0)
    {
        resp["code"] = nRet;
        resp["message"] = strErrMsg;
        OnPayResp(resp, szResp, cbResp);
        return nRet;
    }

    CMyString strAppId, strAppKey, strBpName;
    strAppId = req.getParam("appId");
    Json::Value extraData = pChannel->getKeyData();
    if (strAppId.empty())
    {
        if (!extraData.empty())
        {
            strAppId = extraData["appid"].asString();
            LogDebug("[%s:%s:%d] strAppId: %s", __FILE__, __func__, __LINE__, strAppId.c_str());
        }
    }
    if (!extraData.empty())
    {
        strAppKey = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strAppKey: %s", __FILE__, __func__, __LINE__, strAppKey.c_str());
    }

    int32_t bpId = req_data["billpoint"].asInt();
    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    if (NULL != pBP)
    {
        strBpName = pBP->getBpCode();
    }

    CMyString strServer = req.getParam("channelName");
    CMyString strRole = req.getParam("userName");
    CMyString strIp = req.getParam("playerIp");

    Json::Value data;
    data["appId"] = strAppId;
    data["cpOrderNumber"] = strOrderNo;
    data["orderAmount"] = req_data["fee"].asInt64();
    data["productName"] = strBpName;
    data["productDesc"] = strBpName;
    data["notifyUrl"] = pSdk->getPayCallbackUrl();
    data["expireTime"] = req.getParam("expireTime");
    data["level"] = req.getParam("level");
    data["vip"] = req.getParam("vip");
    data["balance"] = req.getParam("balance");
    data["party"] = req.getParam("party");
    data["roleId"] = req.getParam("roleId");
    data["roleName"] = req.getParam("roleName");
    data["serverName"] = req.getParam("serverName");
    data["extInfo"] = req.getParam("extInfo");

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('&', '=', true, true, false);
    str1 += "&";
    CMyString strAppSecret =  CMD5Wrapper::encode(strAppKey.c_str());
    strAppSecret.MakeLower();
    str1 += strAppSecret;

    LogDebug("[%s:%s:%d] sign data: %s", __FILE__, __func__, __LINE__, str1.c_str());
    CMyString strMD5 = CMD5Wrapper::encode(str1.c_str());
    strMD5.MakeLower();
    data["vivoSignature"] = strMD5;

    resp["code"] = 0;
    resp["message"] = "OK";
    resp["data"] = data;
    OnPayResp(resp, szResp, cbResp);

    return nRet;
}

ENTRY_FUNC_DECLARE(CVivoPay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

void CVivoPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    
}

bool CVivoPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

bool CVivoPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
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

    CMyString strRespCode = req.getParam("respCode");
    if ("200" == strRespCode)
    {
        depositedOK = true;
    }
    else
    {
        depositedOK = false;
    }

    orderNo = req.getParam("cpOrderNumber");
    platformOrderNo = req.getParam("orderNumber");
    fee = (int64_t)(req.getParamInt64("orderAmount")); //单位：分，币种：人民币，为长整型
    realFee = fee;
    tax = 0;

    return true;
}

bool CVivoPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("signature");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("signature");
    vecExcept.push_back("signMethod");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString strAppKey;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strAppKey = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strAppKey: %s", __FILE__, __func__, __LINE__, strAppKey.c_str());
    }
    if (strAppKey.empty())
    {
        strAppKey = pSdk->getApiKey();
    }

    CMyString str1 = resp_maker.http_str_manual('&', '=', true, true, false);
    str1 += "&";
    CMyString str2 = strAppKey;
    str2 = CMD5Wrapper::encode(str2.c_str());
    str2.MakeLower();
    str1 += str2;
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

void CVivoPay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
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

void CVivoPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CVivoPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CVivoPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CVivoPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CVivoPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}

bool CVivoPay::fetchDepositOrderData(const CMyString& orderNo, Json::Value& req_data)
{
    bool bFind(false);
    // 从DB中查询出订单
    m_pDB->Cmd("call pQueryOrderInfo_v3('%s')", orderNo.c_str());
    while(m_pDB->More())
    {
        bFind = true;
        MYSQL_JSON_INT32(payId, 0);
        MYSQL_JSON_STR(uin, 1);
        MYSQL_JSON_STR(billNo, 2);
        MYSQL_JSON_STR(orderNo, 3);
        MYSQL_JSON_INT64(fee, 4);
        MYSQL_JSON_INT64(realFee, 5);
        MYSQL_JSON_INT32(status, 6);
        MYSQL_JSON_INT32(channelId, 7);
        MYSQL_JSON_STR(productId, 8);
        MYSQL_JSON_STR(gameCode, 9);
        MYSQL_JSON_INT32(billpoint, 12);
    }
    char szError[128] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogFatal("[%s]query deposit order %s failure, err:%s, msg:%s",
            m_sdkTag.c_str(), orderNo.c_str(), err, szError);
        this->postAlarm(Alarm_Fatal, CMyString("[%s]query deposit order %s failure, err:%s, msg:%s", 
                    m_sdkTag.c_str(), orderNo.c_str(), err, szError));
        return false;
    }
    if (!bFind)
    {
        LogFatal("[%s]deposit order %s not found.", m_sdkTag.c_str(), orderNo.c_str());
        this->postAlarm(Alarm_Fatal, CMyString("[%s]deposit order %s not found.", 
                    m_sdkTag.c_str(), orderNo.c_str()));
        return false;
    }

    int32_t orderStatus = req_data["status"].asInt();
    bool result(false);
    switch(orderStatus)
    {
    case 2:         // deposit_MoneyFail
    case 6:         // deposit_RequestSucceed
    case 11:        // deposit_UserPayTimeout
        LogDebug("[%s]orderNo %s callback again.", m_sdkTag.c_str(), orderNo.c_str());
        result = true;
        break;
    default:
        break;
    }
    return result;
}

void CVivoPay::OnPayResp(Json::Value &resp, char *szResp, uint32_t& cbResp)
{
    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}