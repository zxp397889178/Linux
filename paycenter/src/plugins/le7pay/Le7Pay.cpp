/*
 * =====================================================================================
 *
 *       Filename:  Le7Pay.cpp
 *
 *    Description:  乐7联运sdk支付
 *
 *        Version:  1.0
 *        Created:  2021年08月17日 10时40分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "Le7Pay.h"
#include "SdkList.h"
#include "BillPointList.h"
#include "SdkChannelList.h"

CLe7Pay::CLe7Pay()
    : CPayBase("Le7Pay", depositVerify, httpRequestSync)
{

}

CLe7Pay::~CLe7Pay()
{

}

int32_t CLe7Pay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CLe7Pay::Update(void)
{
    return CPayBase::Init("main/le7pay");
}

void CLe7Pay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CLe7Pay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CLe7Pay, doGetSign)
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

    bool ret = req.getParam("attach", strOrderNo);
    do {
        if (!ret || strOrderNo.empty())
        {
            nRet = err_depositMissParameter;
            strErrMsg = "not found field \"attach\" in http req parameter";
            LogWarn("[%s]not found field \"attach\" in http req parameter.",  m_sdkTag.c_str());
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
            CMyString strErrMsg("[%s]CSdkChannel object maybe corrupted when user request deposit.", m_sdkTag.c_str());
            LogFatal(strErrMsg.c_str());
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

    CMyString strAppId, strTitle;
    strAppId = req.getParam("app_id");
    if (strAppId.empty())
    {
        Json::Value extraData = pChannel->getKeyData();
        if (!extraData.empty())
        {
            strAppId = extraData["appid"].asString();
            LogDebug("[%s:%s:%d] strAppId: %s", __FILE__, __func__, __LINE__, strAppId.c_str());
        }
    }

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

    int32_t bpId = req_data["billpoint"].asInt();
    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    if (NULL != pBP)
    {
        strTitle = pBP->getBpCode();
    }   

    CMyString strUserId = req.getParam("user_id");
    CMyString strServer = req.getParam("server");
    CMyString strRole = req.getParam("role");
    CMyString strLevel = req.getParam("level");
    CMyString strIp = req.getParam("ip");

    Json::Value data;
    data["user_id"] = CHttpWrapper::urlEncode(strUserId);
    data["app_id"] = CHttpWrapper::urlEncode(strAppId);
    data["title"] = CHttpWrapper::urlEncode(strTitle);
    data["attach"] = CHttpWrapper::urlEncode(strOrderNo);
    data["money"] = req_data["fee"].asInt64();
    data["server"] = CHttpWrapper::urlEncode(strServer);
    data["role"] = CHttpWrapper::urlEncode(strRole);
    data["level"] = CHttpWrapper::urlEncode(strLevel);
    data["ip"] = CHttpWrapper::urlEncode(strIp);
    data["app_key"] = CHttpWrapper::urlEncode(strAppKey);

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('&', '=', false, true, false);
    LogDebug("[%s:%s:%d] sign data: %s", __FILE__, __func__, __LINE__, str1.c_str());
    CMyString strMD5 = CMD5Wrapper::encode(str1.c_str());
    strMD5.MakeLower();
    data["sign"] = strMD5;

    resp["code"] = 0;
    resp["message"] = "OK";
    resp["data"] = data;
    OnPayResp(resp, szResp, cbResp);

    return nRet;
}

ENTRY_FUNC_DECLARE(CLe7Pay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

void CLe7Pay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    
}

bool CLe7Pay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

bool CLe7Pay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
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

    //只有充值成才会回调通知结果，充值失败的不会通知
    depositedOK = true;

    orderNo = req.getParam("attach");
    platformOrderNo = req.getParam("order_sn");
    fee = (int64_t)(req.getParamInt64("money")); //订单金额（单位:分）
    realFee = fee; 
    tax = 0;

    return true;
}

string signParam(const string& strUrl)
{
    string strReturnUrl; 
    CStrSpliter splite;
    splite.Split(strUrl, '&');
    if (splite.empty())
    {
        return strUrl;
    }
    for(int32_t i(0); i < splite.size(); ++i)
    {
        CMyString strData = splite.asString(i);
        CStrSpliter data;
        data.Split(strData, '=');
        if (!data.empty())
        {
            if (strReturnUrl.empty())
            {
                strReturnUrl += data.asString(0);
                strReturnUrl += "=";
                if (data.size() == 2)
                {
                    CMyString strParam = data.asString(1);
                    strReturnUrl += CHttpWrapper::urlEncode(strParam);
                }
            }
            else
            {
                strReturnUrl += "&";
                strReturnUrl += data.asString(0);
                strReturnUrl += "=";
                if (data.size() == 2)
                {
                    CMyString strParam = data.asString(1);
                    strReturnUrl += CHttpWrapper::urlEncode(strParam);
                }
            }
        }
    }

    return strReturnUrl;
}

bool CLe7Pay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString strErr;
    if (!req.ValidParam("role", strErr))
    {
        resp_maker.add("role", "");
        LogWarn("[%s][%s:%d] role is lost", m_sdkTag.c_str(), __FILE__, __LINE__);
    }
    if (!req.ValidParam("server", strErr))
    {
        resp_maker.add("server", "");
        LogWarn("[%s][%s:%d] server is lost", m_sdkTag.c_str(), __FILE__, __LINE__);
    }
    if (!req.ValidParam("add_time", strErr))
    {
        resp_maker.add("add_time", "");
        LogWarn("[%s][%s:%d] add_time is lost", m_sdkTag.c_str(), __FILE__, __LINE__);
    }
    if (!req.ValidParam("ip", strErr))
    {
        resp_maker.add("ip", "");
        LogWarn("[%s][%s:%d] ip is lost", m_sdkTag.c_str(), __FILE__, __LINE__);
    }
    if (!req.ValidParam("attach", strErr))
    {
        resp_maker.add("attach", "");
        LogWarn("[%s][%s:%d] attach is lost", m_sdkTag.c_str(), __FILE__, __LINE__);
    }
    if (!req.ValidParam("order_sn", strErr))
    {
        resp_maker.add("order_sn", "");
        LogWarn("[%s][%s:%d] order_sn is lost", m_sdkTag.c_str(), __FILE__, __LINE__);
    }

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

    resp_maker.add("app_key", strAppKey);
    CMyString strParam = resp_maker.http_str_manual('&', '=', false, true, false);
    LogDebug("[%s:%s:%d] sign data: %s", __FILE__, __func__, __LINE__, strParam.c_str());
    CMyString str1 = signParam(strParam);
    LogDebug("[%s:%s:%d] sign data: %s", __FILE__, __func__, __LINE__, str1.c_str());
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

void CLe7Pay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";

    Json::Value resp;
    if (bSucceed)
    {
        resp["status"] = "success";
        resp["msg"] = "";
    }
    else
    {
        resp["status"] = "failed";
        resp["msg"] = CMyString("%d", errCode);
    }

    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

void CLe7Pay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CLe7Pay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CLe7Pay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CLe7Pay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CLe7Pay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}

bool CLe7Pay::fetchDepositOrderData(const CMyString& orderNo, Json::Value& req_data)
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

void CLe7Pay::OnPayResp(Json::Value &resp, char *szResp, uint32_t& cbResp)
{
    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}