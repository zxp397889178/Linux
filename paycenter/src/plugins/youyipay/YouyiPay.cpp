/*
 * =====================================================================================
 *
 *       Filename:  YouyiPay.cpp
 *
 *    Description:  优亿支付
 *
 *        Version:  1.0
 *        Created:  2021年09月26日 11时24分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "YouyiPay.h"
#include "SdkList.h"
#include "Security.h"
#include "SdkChannelList.h"

CYouyiPay::CYouyiPay()
    : CPayBase("YouyiPay", depositVerify, httpRequestSync)
{

}

CYouyiPay::~CYouyiPay()
{

}

int32_t CYouyiPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CYouyiPay::Update(void)
{
    return CPayBase::Init("main/youyipay");
}

void CYouyiPay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CYouyiPay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CYouyiPay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

void CYouyiPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    
}

bool CYouyiPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

bool CYouyiPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
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

    //0等待支付，1支付成功，2支付失败，3支付取消
    CMyString strStatus = req.getParam("pay_status");
    if (strStatus == "1")
    {
        depositedOK = true;
    }
    else
    {
        depositedOK = false;
    }

    orderNo = req.getParam("out_trade_no");
    platformOrderNo = "N/A";
    fee = (int64_t)(req.getParamInt64("total_fee")); //交易总金额(单位：分)
    realFee = fee; 
    tax = 0;

    return true;
}

bool CYouyiPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("signature");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("signature");
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
    str1 += strAppKey;
    CMyString strSign2= CMD5Wrapper::encode(str1.c_str());

    LogDebug("[%s:%s:%d] singData:%s", __FILE__, __func__, __LINE__, str1.c_str());

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

void CYouyiPay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
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

void CYouyiPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CYouyiPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CYouyiPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CYouyiPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CYouyiPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}
