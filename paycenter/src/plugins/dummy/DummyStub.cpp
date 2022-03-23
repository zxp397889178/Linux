/*
 * =====================================================================================
 *
 *       Filename:  DummyStub.cpp
 *
 *    Description:  测试支付中心之用
 *
 *        Version:  1.0
 *        Created:  2021年03月06日 14时47分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "DummyStub.h"
#include "Security.h"
#include "Config.h"
#include "SdkList.h"
#include "SdkChannelList.h"
#include "ChannelList.h"

CPluginDummyStub::CPluginDummyStub()
    : CPayBase("DummyPay", depositRequest, httpRequestSync)
{

}

CPluginDummyStub::~CPluginDummyStub()
{
}

int32_t CPluginDummyStub::Init(void)
{ 
    int32_t ret = this->Update();
    return ret;
}

int32_t CPluginDummyStub::Update(void)
{
    CMyString strXml("main/dummy");
    int32_t ret = CPayBase::Init(strXml);
    return ret;
}

void CPluginDummyStub::Maintance()
{
    CPayBase::Maintance();
}

// 以下为调用网银接口
ENTRY_FUNC_DECLARE(CPluginDummyStub, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginDummyStub, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

void CPluginDummyStub::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    CMyString strIp = req.getParam("playerIp");
    // 金额转换成元
    int64_t payMoney = req.getParamInt64("amount");
    double money = payMoney / 100.0;

    CMyString uin = req.getParam("uin");
    CMyString productId = req.getParam("productId");
    
    // 时区校正
    CXmlFile &xml = CONFIG.getXml();
    int32_t adjust = xml.GetLong("main/paycenter/timezone", "adjust");
    int32_t nReqTime = time(NULL) + adjust; 

    CPayChannel *pPayChannel = pChannel->getPayChannel();

    // 构建发往支付SDK请求的URL
    http::CHttpRespMaker pay_req;
    pay_req.add("merchant", pSdk->getAccount());            // 商户号
    pay_req.add("uin", uin);                                // 玩家ID
    pay_req.add("userIp", strIp);                           // 玩家IP
    pay_req.add("productId", productId);                    // 产品ID
    pay_req.add("paystyle", pPayChannel->getPayStyle());    // 支付类型
    pay_req.add("bankcode", pChannel->getBankCode());       // 银行码
    // 转成 xx.xx 的格式
    CMyString reqMoney("%.2lf", money);
    pay_req.add("money", reqMoney);                         // 金额
    pay_req.add("tradeNo", orderNo);                     // 订单号
    pay_req.add("notify_url", pSdk->getPayCallbackUrl());                // 回调URL
    pay_req.add("return_url", pGame->getCallbackUrl());              // 支付结束返回URL
    pay_req.add("reqtime", nReqTime);                       // 请求时间

    CMyString str1 = pay_req.http_str_manual('&', '=', true, true, false);
    CMyString str2("%s&%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMD5 = CMD5Wrapper::encode(str2.c_str());
    strMD5.MakeLower();
    pay_req.add("sign", strMD5);

    httpData = pay_req.http_str_manual('&', '=', false, true, true);
    url = pSdk->getPayUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}

bool CPluginDummyStub::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    LogDebug("[%s]deposit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());

     CJsonWrapper jrRoot;
    if (!jrRoot.Load(respData))
    {
        errNote.Format("resp data not json string");
        return false;
    }

    int32_t nCode = jrRoot.getInt("code");
    if (nCode != 0)
    {
        errNote = jrRoot.getString("m");
        return false;
    }

    Json::Value data;
    if (!jrRoot.getJsonObject("data", data))
    {
        errNote.Format("resp data d not json string");
        return false;
    }

    depositUrlData = data["url"].asString();
    depositOrderNo = data["orderNo"].asString();
    LogDebug("[%s] doParseDepositResp : depositUrlData[%s]",  m_sdkTag.c_str(), depositUrlData.c_str());
    
    return true;
}

bool CPluginDummyStub::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
{
    orderNo = req.getParam("tradeNo");
    platformOrderNo = req.getParam("orderNo");
    CMyString money = req.getParam("money");
    double realMoney = money.empty() ? 0 : (money.asDouble() * 100);
    fee = realMoney;  
    realFee = fee;
    tax = 0;
    depositedOK = true;

    LogDebug("[%s]pay param: orderNo:%s, platformOrderNo:%s, fee:%ld", m_sdkTag.c_str(), orderNo.c_str(), platformOrderNo.c_str(), fee);

    return true;
}

bool CPluginDummyStub::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    return true;
}

void CPluginDummyStub::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    safe_strcpy(szResp, cbResp, "ok");
    cbResp = strlen(szResp);
}

void CPluginDummyStub::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CPluginDummyStub::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CPluginDummyStub::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CPluginDummyStub::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CPluginDummyStub::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}
