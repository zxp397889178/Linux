/*
 * =====================================================================================
 *
 *       Filename:  OneCow.cpp
 *
 *    Description:  一牛H5支付
 *
 *        Version:  1.0
 *        Created:  2021年12月24日 16时30分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#include "OneCow.h"
#include "SdkList.h"
#include "SdkChannelList.h"

COneCowPay::COneCowPay()
    : CPayBase("OneCow", depositVerify, httpRequestSync)
{

}

COneCowPay::~COneCowPay()
{

}

int32_t COneCowPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t COneCowPay::Update(void)
{
    return CPayBase::Init("main/onecowpay");
}

void COneCowPay::Maintance(void)
{
    CPayBase::Maintance();
}


ENTRY_FUNC_DECLARE(COneCowPay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(COneCowPay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(COneCowPay, doGetSign)
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
    int32_t nConfigId = 0;

    bool ret = req.getParam("configId", nConfigId);
    do {
        if (!ret)
        {
            nRet = err_depositMissParameter;
            strErrMsg = "not found field \"configId\" in http req parameter";
            LogWarn("[%s]not found field \"configId\" in http req parameter.", m_sdkTag.c_str());
            break;
        }

        std::vector<CSdkChannel*> vecSdkChannel;
        SDKCHANNEL_LIST.GetChannel(nConfigId, vecSdkChannel);
        if (vecSdkChannel.empty())
        {
            nRet = 1702;
            strErrMsg.Format("not avaiable pay channel, configId :%d", nConfigId);
            LogErr(strErrMsg.c_str());
            break;
        }

        pChannel = vecSdkChannel[0];
        if (NULL == pChannel)
        {
            nRet = err_depositParameterFault;
            strErrMsg.Format("channel is not avaiable in system");
            LogWarn("[%s]channelId is not avaiable in system.", m_sdkTag.c_str());
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

        CJsonWrapper wr(resp);
        CMyString strResp = wr.toString();
        safe_strcpy(szResp, cbResp, strResp.c_str());
        cbResp = strlen(szResp);
        return nRet;
    }

    {
        CMyString strSign = req.getParam("sign");

        http::CHttpRespMaker reqVerify;
        std::vector<CMyString> vecExcept;
        vecExcept.push_back("sign");
        req.BuildRespMaker_Except(vecExcept, reqVerify);

        CMyString str1 = reqVerify.http_str_manual('&', '=', true, true, false);
        str1.Append("&merchantKey=%s", pGame->getApiKey().c_str());
        CMyString strSign2 = CMD5Wrapper::encode(str1.c_str());
        if (0 != strcasecmp(strSign.c_str(), strSign2.c_str()))
        {
            LogErr("[%s][%s:%s:%d]doGetSign sign mismatch, origin[%s], sign[%s], sign from request[%s]", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, str1.c_str(), strSign2.c_str(), strSign.c_str());

            resp["code"] = -1;
            resp["message"] = "sign mismatch";

            CJsonWrapper wr(resp);
            CMyString strResp = wr.toString();
            safe_strcpy(szResp, cbResp, strResp.c_str());
            cbResp = strlen(szResp);
            return 200;
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

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    vecExcept.push_back("configId");
    vecExcept.push_back("gameCode");
    vecExcept.push_back("uin");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString str1 = resp_maker.http_str_manual('&', '=', false, true, false);
    str1.Append("%s", strAppKey.c_str());
    CMyString strSign2 = CMD5Wrapper::encode(str1.c_str());

    LogDebug("[%s:%s:%d] sign data: %s", __FILE__, __func__, __LINE__, str1.c_str());
    CMyString strMD5 = CMD5Wrapper::encode(str1.c_str());
    strMD5.MakeLower();

    resp["code"] = 0;
    resp["message"] = "OK";
    resp["data"] = strMD5;
    
    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);

    return nRet;
}

void COneCowPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    
}

bool COneCowPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

bool COneCowPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
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

    orderNo = req.getParam("trade_no");
    platformOrderNo = req.getParam("out_trade_no");
    fee = (int64_t)(req.getParamInt64("amount")); //金额，单位为分
    realFee = fee;
    tax = 0;

    //运营方只对充值成功的订单进行通知
    depositedOK = true;

    return true;
}

bool COneCowPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
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

    CMyString str1 = resp_maker.http_str_manual('&', '=', false, true, false);
    str1 += strAppKey;
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

void COneCowPay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    Json::Value resp;
    if (bSucceed)
    {
        resp["status"] = "success";
    }
    else
    {
        resp["status"] = "failure";
    }
    
    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

void COneCowPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool COneCowPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool COneCowPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool COneCowPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void COneCowPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}