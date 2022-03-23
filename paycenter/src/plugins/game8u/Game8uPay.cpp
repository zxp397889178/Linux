/*
 * =====================================================================================
 *
 *       Filename:  Game8uPay.cpp
 *
 *    Description:  8U游戏平台支付-用于天趣H5支付
 *
 *        Version:  1.0
 *        Created:  2021年11月25日 14时42分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#include "Game8uPay.h"
#include "SdkChannelList.h"
#include "SdkList.h"

CGame8UPay::CGame8UPay()
    : CPayBase("Game8UPay", depositVerify, httpRequestSync)
{

}

CGame8UPay::~CGame8UPay()
{

}

int32_t CGame8UPay::Init(void)
{
    return this->Update();
}

int32_t CGame8UPay::Update(void)
{
    return CPayBase::Init("main/game8upay");
}

void CGame8UPay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CGame8UPay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CGame8UPay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}


void CGame8UPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    
}

bool CGame8UPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

bool CGame8UPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
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

    orderNo = req.getParam("cpOrderId");
    platformOrderNo = req.getParam("orderid");
    //fee = (int64_t)(req.getParamInt64("money") * 100); //充值金额，单位：RMB元
    
    double dMoney = 0.0f;
    req.getParam("money", dMoney);
    CMyString strMoney("%.02f", dMoney);
    fee = strMoney.asDouble() * 100;//充值金额，单位：RMB元
    realFee = fee; 
    tax = 0;

    return true;
}

bool CGame8UPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");

    CMyString strUid = req.getParam("uid");
    CMyString strAppId = req.getParam("appid");
    CMyString strOrderId = req.getParam("orderid");
    CMyString strExt = req.getParam("ext");
    int64_t nTime = req.getParamInt64("time");

    double dMoney = 0.0f;
    req.getParam("money", dMoney);
    CMyString strMoney("%.02f", dMoney);

    CMyString strSignData("uid=%s&appid=%s&money=%s&orderid=%s&ext=%s&time=%ld", 
                strUid.c_str(), strAppId.c_str(), strMoney.c_str(), strOrderId.c_str(), strExt.c_str(), nTime);

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

    strSignData += "&appkey=";
    strSignData += strAppKey;
    LogDebug("[%s:%s:%d] sign data: %s", __FILE__, __func__, __LINE__, strSignData.c_str());

    CMyString strSign2 = CMD5Wrapper::encode(strSignData.c_str());

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), strSignData.c_str(), strSign2.c_str(), strSign1.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]", 
                m_sdkTag.c_str(), strSignData.c_str(), strSign2.c_str(), strSign1.c_str()));
        return false;
    }
    return true;
}

void CGame8UPay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";

    Json::Value resp;
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

void CGame8UPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CGame8UPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CGame8UPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CGame8UPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CGame8UPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}
