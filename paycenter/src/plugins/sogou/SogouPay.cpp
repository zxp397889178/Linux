/*
 * =====================================================================================
 *
 *       Filename:  SogouPay.cpp
 *
 *    Description:  搜狗支付渠道
 *
 *        Version:  1.0
 *        Created:  2021年08月16日 17时15分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#include "SogouPay.h"
#include "SdkList.h"
#include "SdkChannelList.h"

CSogouPay::CSogouPay()
    : CPayBase("SogouPay", depositVerify, httpRequestSync)
{

}

CSogouPay::~CSogouPay()
{

}

int32_t CSogouPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CSogouPay::Update(void)
{
    return CPayBase::Init("main/sogoupay");
}

void CSogouPay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CSogouPay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CSogouPay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

void CSogouPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    
}

bool CSogouPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

bool CSogouPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
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

    //平台收到第三方支付成功后，向游戏发送付款结果通知  收到回调就是已支付成功
    depositedOK = true;

    orderNo = req.getParam("appdata");
    platformOrderNo = req.getParam("oid");
    fee = (int64_t)(req.getParamInt64("amount1") * 100); //用户充值金额（人民币元） 
    realFee = (int64_t)(req.getParamInt64("realAmount") * 100); //用户充值真实金额（人民币元） 
    tax = 0;

    return true;
}

bool CSogouPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("auth");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("auth");
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

    CMyString str1 = resp_maker.http_str_manual('&', '=', false, true, true);
    str1 += "&";
    str1 += strAppKey;
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

void CSogouPay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    if (bSucceed)
    {
        safe_strcpy(szResp, cbResp, "OK");
    }
    else
    {
        switch(errCode)
        {
        case err_depositCallbackDataCorrupted:
            safe_strcpy(szResp, cbResp, "ERR_500"); //其他错误 
            break;
        case err_depositDataLost:
            safe_strcpy(szResp, cbResp, "ERR_100");  //参数不符合规则 
            break;
        case err_depositCallbackIpInvalid:
            safe_strcpy(szResp, cbResp, "ERR_400");  //非法 IP 访问 
            break;
        case err_depositCallbackSignMismatch:
            safe_strcpy(szResp, cbResp, "ERR_200"); //验证失败 
            break;
        default:
            safe_strcpy(szResp, cbResp, "OK");
            break;
        }
    }
    cbResp = strlen(szResp);
}

void CSogouPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CSogouPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CSogouPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CSogouPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CSogouPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}
