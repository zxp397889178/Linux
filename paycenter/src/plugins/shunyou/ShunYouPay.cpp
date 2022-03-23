/*
 * =====================================================================================
 *
 *       Filename:  ShunYouPay.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2021年08月16日 18时28分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#include "ShunYouPay.h"
#include "SdkList.h"
#include "Security.h"
#include "SdkChannelList.h"

CShunYouPay::CShunYouPay()
    : CPayBase("ShunYouPay", depositVerify, httpRequestSync)
{

}

CShunYouPay::~CShunYouPay()
{

}

int32_t CShunYouPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CShunYouPay::Update(void)
{
    return CPayBase::Init("main/shunyoupay");
}

void CShunYouPay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CShunYouPay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CShunYouPay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

void CShunYouPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    
}

bool CShunYouPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

bool CShunYouPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
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

    CMyString strAttach = req.getParam("attach");
    CMyString strOrderNo = strAttach;
    CJsonWrapper jr;
    if (jr.Load(strAttach))
    {
        strOrderNo = jr.getString("orderNumber");
    }

    orderNo = strOrderNo;
    platformOrderNo = req.getParam("orderid");
    fee = (int64_t)(req.getParamInt64("amount") * 100); //成功充值⾦额(单位：元)
    realFee = fee; 
    tax = 0;

    return true;
}

bool CShunYouPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");

    //参与签名，固定顺序
    CMyString strOrderId = req.getParam("orderid");
    CMyString strUserName = req.getParam("username");
    int64_t gameId = req.getParamInt64("gameid");
    CMyString strRoleId = req.getParam("roleid");
    int64_t serverId = req.getParamInt64("serverid");
    CMyString strPayType = req.getParam("paytype");
    int64_t amount = req.getParamInt64("amount");
    int64_t paytime = req.getParamInt64("paytime");
    CMyString strAttach = req.getParam("attach");

    CMyString strSignData("orderid=%s&username=%s&gameid=%ld&roleid=%s&serverid=%ld&paytype=%s&amount=%ld&paytime=%ld&attach=%s",
                        CHttpWrapper::urlEncode(strOrderId).c_str(), CHttpWrapper::urlEncode(strUserName).c_str(), gameId,
                        CHttpWrapper::urlEncode(strRoleId).c_str(), serverId, CHttpWrapper::urlEncode(strPayType).c_str(),
                        amount, paytime, CHttpWrapper::urlEncode(strAttach).c_str());
    
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

    LogDebug("[%s:%s:%d] singData:%s", __FILE__, __func__, __LINE__, strSignData.c_str());
    CMyString strSign2 = CMD5Wrapper::encode(strSignData.c_str());

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), strSignData.c_str(), strSign2.c_str(), strSign1.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]deposit callback sign mismatch, origin text [%s], sign[%s], sign from platform[%s]", 
                m_sdkTag.c_str(), strSignData.c_str(), strSign2.c_str(), strSign1.c_str()));
        return false;
    }
    return true;
}

void CShunYouPay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    if (bSucceed)
    {
        safe_strcpy(szResp, cbResp, "success");
    }
    else
    {
        switch(errCode)
        {
        case err_depositCallbackDataCorrupted:
        case err_depositDataLost:
        case err_depositCallbackIpInvalid:
            safe_strcpy(szResp, cbResp, "error");  //未知错误
            break;
        case err_depositCallbackSignMismatch:
            safe_strcpy(szResp, cbResp, "errorSign"); //签名错误 
            break;
        default:
            safe_strcpy(szResp, cbResp, "success");
            break;
        }
    }
    cbResp = strlen(szResp);
}

void CShunYouPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CShunYouPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CShunYouPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CShunYouPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CShunYouPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}
