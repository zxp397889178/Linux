/*
 * =====================================================================================
 *
 *       Filename:  BaiduPay.cpp
 *
 *    Description:  百度 多酷联运SDK 单机 支付
 *
 *        Version:  1.0
 *        Created:  2021年08月16日 15时37分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "BaiduPay.h"
#include "SdkList.h"
#include "SdkChannelList.h"

CBaiduPay::CBaiduPay()
    : CPayBase("BaiduPay", depositVerify, httpRequestSync)
{

}

CBaiduPay::~CBaiduPay()
{

}

int32_t CBaiduPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CBaiduPay::Update(void)
{
    return CPayBase::Init("main/baidupay");
}

void CBaiduPay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CBaiduPay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CBaiduPay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

void CBaiduPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    
}

bool CBaiduPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

bool CBaiduPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
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

    CMyString strFlag = req.getParam("status");
    if (strFlag == "success")
    {
        depositedOK = true;
    }
    else
    {
        depositedOK = false;
    }

    orderNo = req.getParam("cpdefinepart");
    platformOrderNo = req.getParam("orderid");

    int64_t nAmount = req.getParamInt64("amount");
    CMyString strUnit = req.getParam("unit");
    if ("yuan" == strUnit)
    {
        nAmount = nAmount * 100;
    }

    fee = nAmount;
    realFee = fee;
    tax = 0;

    return true;
}

bool CBaiduPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sign");

    CMyString appid = req.getParam("appid");
    CMyString orderid = req.getParam("orderid");
    CMyString amount = req.getParam("amount");
    CMyString unit = req.getParam("unit");
    CMyString jfd = req.getParam("jfd");
    CMyString status = req.getParam("status");
    CMyString paychannel = req.getParam("paychannel");
    CMyString phone = req.getParam("phone");
    CMyString channel = req.getParam("channel");
    CMyString from = req.getParam("from");
    CMyString sign = req.getParam("sign");
    CMyString extchannel = req.getParam("extchannel");
    CMyString cpdefinepart = req.getParam("cpdefinepart");

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

    CMyString strSingData = appid + orderid + amount + unit + status + paychannel + strAppKey;
    CMyString strSign2 = CMD5Wrapper::encode(strSingData.c_str());

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), strSingData.c_str(), strSign2.c_str(), strSign1.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]", 
                m_sdkTag.c_str(), strSingData.c_str(), strSign2.c_str(), strSign1.c_str()));
        return false;
    }
    return true;
}

void CBaiduPay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
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

void CBaiduPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CBaiduPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CBaiduPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CBaiduPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CBaiduPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}
