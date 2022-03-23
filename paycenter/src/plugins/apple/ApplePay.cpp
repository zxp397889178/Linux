/*
 * =====================================================================================
 *
 *       Filename:  ApplePay.cpp
 *
 *    Description:  苹果支付接口
 *
 *        Version:  1.0
 *        Created:  2021年04月09日 14时34分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "ApplePay.h"
#include "Config.h"
#include "Security.h"
#include <algorithm>


CPluginApplePay::CPluginApplePay()
    : CPayBase("ApplePay", depositVerify, httpRequestSync)
{

}

CPluginApplePay::~CPluginApplePay()
{

}

int32_t CPluginApplePay::Init(void)
{
    return this->Update();
}

int32_t CPluginApplePay::Update(void)
{
    int32_t ret = CPayBase::Init("main/apple");
    if (ret < 0)
    {   
        return ret;
    }

    CMyString strSect;
    CXmlFile &xml = CONFIG.getXml();
    try
    {
        m_sandboxUrl = xml.GetString("main/apple/sandbox", "url");
        m_appleUrl = xml.GetString("main/apple/paycenter", "url");
        m_autoSwitchUrl = xml.GetBool("main/apple", "autoswitch");
        strSect = xml.GetString("main/apple", "sect");
        strSect.MakeLower();
        m_sandboxFirst = (strSect == "sandbox");

        CXmlFile &xml = CONFIG.getXml();
        CMyString sect("main/apple/applesdk/callback");
        if (!CPayBase::InitRedisCache(m_cacheAppleOrder, m_redis, xml, sect))
        {
            return -1;
        }
    }
    catch(...)
    {
        LogErr("get config failure, sect [main/apple]");
        return -1;
    }

    return 0;
}


void CPluginApplePay::Maintance(void)
{
    CPayBase::Maintance();
}


// 以下为调用网银接口
ENTRY_FUNC_DECLARE(CPluginApplePay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

// 以下为游服收到前端的数据后，通知支付收验证是否到账
ENTRY_FUNC_DECLARE(CPluginApplePay, doVerify)
{
    return CPayBase::doDepositVerify(pClient, req, szResp, cbResp, ext_headers);
}


void CPluginApplePay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CPluginApplePay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

void CPluginApplePay::addDepositVerifyNeededParam(std::vector<CMyString> &vecParam)
{
    vecParam.push_back("token");
}

void CPluginApplePay::storeDepositVerifyData(CGame *pGame, CSdk *pSdk, http::CHttpRequest &req, Json::Value &data)
{
    data["token"] = req.getParam("token");

    // 保存到cache中，稍后发送
    int32_t nRet = m_cacheAppleOrder.Push(data);
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

int32_t CPluginApplePay::verifyDepositRequest(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode)
{
    CMyString strKey, strData;
    int32_t nRet = 0;
    int32_t ret = m_cacheAppleOrder.Pop(strKey, strData);
    if (ret > 0)
    {
        LogDebug("[%s]getIncrData ret:[%d] strKey:[%s] strData:[%s]", m_sdkTag.c_str(), ret, strKey.c_str(), strData.c_str());
        bool bRet = doAppleCert(strData, depositOK, platformOrderNo, realFee, regionCode);
        if (bRet)
        {
            nRet = 1;
        }
        else
        {
            nRet = -1;
        }
    }

    return nRet;
}

void CPluginApplePay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData) 
{

}


bool CPluginApplePay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}


bool CPluginApplePay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}


bool CPluginApplePay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}


void CPluginApplePay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}


bool CPluginApplePay::doAppleCert(const CMyString &strJsonStr, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode)
{
    CJsonWrapper jr;
    if (!jr.Init(strJsonStr))
    {
        return false;
    }

    CMyString strErr;
    CMyString orderNo = jr.getString("orderNo");
    CMyString clientIp = jr.getString("clientIp");
    CMyString uin = jr.getString("uin");

    // 向苹果方发送请求，验证是否充值到账
    Json::Value apple_req;
    apple_req["receipt-data"] = jr.getString("token");
    CJsonWrapper writer;
    CMyString strData = writer.write(apple_req);
    std::vector<std::string> header_list;
    CMyString cont_type = "Content-Type: application/x-www-form-urlencoded";
    header_list.push_back(cont_type);
    CMyString cont_len("Content-Length: %u", strData.length());
    header_list.push_back(cont_len);
    CMyString strResp;
    CMyString strHeader;
    CMyString strUrl = m_sandboxFirst ? m_sandboxUrl : m_appleUrl;
    CMyString appleOrderNo;
    int32_t result=-1000;
    int32_t query_count(1);
    bool bAgain(true);
    do {
        uint64_t t1 = Now();
        http::CHttpClient net;
        result = net.HttpPostWithHeaders(strUrl.c_str(),
                strResp, strHeader, header_list, strData.c_str());
        if (result != 0)
        {
            CMyString strMsg;
            net.getError(result, strMsg);
            CMyString strNote("用户(uin=%s)调起Apple付款失败，url=%s, 错误码:%d, 错误说明:%s", uin.c_str(), strUrl.c_str(), result, strMsg.c_str() );

            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strNote.c_str());
            this->postAlarm(Alarm_Fatal, CMyString("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strNote.c_str()));
            result = -1001;
            break;
        }
        uint64_t t2 = Now();
        LogDebug("[ApplePay URL]%s", strUrl.c_str());
        LogWarn("[Apple Header]%s", strHeader.c_str());
        LogWarn("[Apple Resp  ]%s", strResp.c_str());
        LogDebug("[STAT]ip:%s, uin:%s, orderNo:%s, url:%s, cert cost:%dms", 
                clientIp.c_str(), uin.c_str(), orderNo.c_str(), strUrl.c_str(), (int32_t)(t2-t1));
        result = this->dealResp(strResp, strErr, appleOrderNo);

        // 以下逻辑用于控制是否切换充值地址
        --query_count;
        if (m_autoSwitchUrl && bAgain)
        {
            bAgain = false;
            if (result == -1005)
            {
                strUrl = m_sandboxUrl;
                ++query_count;
                LogWarn("use sandbox token, switch to url:%s", strUrl.c_str());
            }
            else if (result == -1007)
            {
                strUrl = m_appleUrl;
                ++query_count;
                LogWarn("use realpay token, switch to url:%s", strUrl.c_str());
            }
            strResp.clear();
            strHeader.clear();
        }
    }while(query_count>0);

    if (result != 0)
    {
        depositOK = false;
        return false;
    }

    depositOK = true;
    int64_t fee= jr.getInt64("fee");
    realFee = fee;
    platformOrderNo = appleOrderNo;

    return true;
}

bool CheckPurchaseDate(const CMyString& purchaseDate)
{
    static const int32_t APPLE_PURCHES_DELAY_TIME = 30*60;  // 30minutes

    if (purchaseDate.length() > 0)
    {   
        vector<std::string> vecValue;
        safe_strtok(purchaseDate.c_str(), vecValue, " ");
        if (vecValue.size() < 2)
        {   
            //转换为时间
            return false;
        }
        
        string purches = vecValue[0] + " " + vecValue[1];
        CMyDateTime purchesTime(purches.c_str());   //GMT时间，需要加8小时转换为当地时间
            
        CMyDateTime now;
        if (now.time() - (purchesTime.time()+8*3600) < APPLE_PURCHES_DELAY_TIME)
        {
            return true;
        }
    }

    return false;
}


int32_t CPluginApplePay::dealResp(CMyString &strResp, CMyString &strError, CMyString &appleOrderNo)
{
    CJsonWrapper jr;
    if (!jr.Init(strResp.c_str()))
    {
        return -1003;
    }

    int32_t result(0);
    int32_t status = jr.getInt("status");
    switch(status)
    {
    case 0:         // 成功状态
        break;
    case 21007:     // 沙盒数据发到正式环境
        LogWarn("sandbox data send to apple paycenter");
        result = -1005;
        break;
    case 21008:     // 正式数据发到沙盒
        LogWarn("real pay data send to apple sandbox");
        result = -1007;
        break;
    default:
        LogErr("unprocess status %d", status);
        result = -1009;
        break;
    }
    if (result < 0)
    {
        return result;
    }

    // 处理返回数据
    Json::Value receipt;
    if (!jr.getJsonObject("receipt", receipt))
    {
        // 没有收条，出错
        return -1011;
    }
    Json::Value respInApp;
    if (!jr.getJsonObject("in_app", respInApp))
    {
        // 没有in_app 这一节数据
        return 0;
    }
    if (respInApp.isArray() && !respInApp.empty())
    {
        // 获得第一个订单号
        Json::Value inapp = respInApp[0];
        appleOrderNo = inapp["transaction_id"].asString();

        // TODO: 校验订单时间
        //int64_t purchase_date = inapp["purchase_date_ms"].asInt64();
    }

    //CMyString strDate = receipt.getString("purchase_date");
    //appleOrderNo = receipt.getString("product_id");
    //bool val = CheckPurchaseDate(strDate);
    //if (!val)
    //{
    //    return -1013;
    //} 

    return 0;
}
