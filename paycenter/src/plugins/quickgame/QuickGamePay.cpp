/*
 * =====================================================================================
 *
 *       Filename:  QuickSDKPay.cpp
 *
 *    Description:  Quicksdk 支付
 *
 *        Version:  1.0
 *        Created:  2021年08月31日 11时36分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "QuickGamePay.h"
#include "QuickEncrypt.h"
#include "SdkList.h"
#include "Config.h"
#include "SdkChannelList.h"

CQuickGamePay::CQuickGamePay()
    : CPayBase("QuickGamePay", depositRequest, httpRequestSync)
{

}

CQuickGamePay::~CQuickGamePay()
{

}

int32_t CQuickGamePay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CQuickGamePay::Update(void)
{
    CMyString strXmlSect = "main/quickgame";
    int32_t ret = CPayBase::Init(strXmlSect);

    CXmlFile &xml = CONFIG.getXml();
    void *pRoot(NULL);
    try
    {
        pRoot = xml.GetElement("main/quickgame/callback/key");
        while(pRoot != NULL)
        {
            CMyString strName = xml.GetValue(pRoot, "name");
            CMyString strCallbackKey = xml.GetValue(pRoot, "callbackkey");
            
            m_mapCallbackKey[strName] = strCallbackKey;

            pRoot = xml.GetNextSibling(pRoot, "key");
        }
    }
    catch(CException &ex)
    {
        LogErr("get quickgame key failure, err:%s", ex.what());
        ret = -103;
    }
    catch(std::exception &ex)
    {
        LogErr("get quickgame key failure, err:%s", ex.what());
        ret = -105;
    }
    catch(...)
    {
        LogErr("get quickgame key failure, unknown err");
        ret = -107;
    }

    return ret;
}

void CQuickGamePay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CQuickGamePay, doDeposit)
{
    int32_t nRet = CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
    CJsonWrapper jw;
    if (jw.Load(szResp))
    {
        Json::Value resp = jw.getJson();
        Json::Value data;
        if (jw.getJsonObject("data", data))   
        {
            CMyString strUrl = data["url"].asString();
            data["callback_url"] = strUrl;
            data["url"] = "";
        }
        resp["data"] = data;

        CJsonWrapper wr(resp);
        CMyString strResp = wr.toString();
        safe_strcpy(szResp, strResp.length() + 1, strResp.c_str());
        cbResp = strlen(szResp);
    }
    return nRet;
}

ENTRY_FUNC_DECLARE(CQuickGamePay, doDepositCB)
{
    m_strCallbackKey.clear();
    map<CMyString, CMyString>::iterator iter = m_mapCallbackKey.find("callback");
    if (iter != m_mapCallbackKey.end())
    {
        m_strCallbackKey = iter->second;
    }
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CQuickGamePay, doDepositCB_hy)
{
    m_strCallbackKey.clear();
    map<CMyString, CMyString>::iterator iter = m_mapCallbackKey.find("callback_hy");
    if (iter != m_mapCallbackKey.end())
    {
        m_strCallbackKey = iter->second;
    }
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

void CQuickGamePay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    CMyString strCallbackUrl;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strCallbackUrl = extraData["callback_url"].asString();
        LogDebug("[%s:%s:%d] strCallbackUrl: %s", __FILE__, __func__, __LINE__, strCallbackUrl.c_str());
    }
    if (strCallbackUrl.empty())
    {
        strCallbackUrl = pSdk->getPayCallbackUrl();
    }

    url = strCallbackUrl;
    contentType = "application/x-www-form-urlencoded";
}

bool CQuickGamePay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

bool CQuickGamePay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
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

    CMyString strNtData = req.getParam("nt_data");
    strNtData = QuickEncrypt::decode(strNtData, m_strCallbackKey);
    LogDebug("[%s][%s:%d] strNtData:%s", m_sdkTag.c_str(), __FILE__, __LINE__, strNtData.c_str());
    CXmlFile xml;
    if (!xml.Parse(strNtData.c_str(), strNtData.length()))
    {
        LogDebug("[%s][%s:%d] nt_data is not xml, strNtData:%s", m_sdkTag.c_str(), __FILE__, __LINE__, strNtData.c_str());
        return false;
    }

    CMyString strStatus;
    if (xml.getString("quick_message/message/status",  strStatus) != 0)
    {
        LogDebug("[%s][%s:%d] not has quick_message/message/status ", m_sdkTag.c_str(), __FILE__, __LINE__);
        return false;
    }

    if (strStatus == "0")
    {
        depositedOK = true;
    }
    else
    {
        depositedOK = false;
    }

    CMyString strAmount;
    xml.getString("quick_message/message/out_order_no",  orderNo);
    xml.getString("quick_message/message/order_no",  platformOrderNo);
    xml.getString("quick_message/message/amount",  strAmount); //用户支付金额，单位元，游戏最终发放道具金额应以此为准

    fee = strAmount.asDouble() * 100; //用户支付金额，单位元，游戏最终发放道具金额应以此为准
    realFee = fee; 
    tax = 0;

    return true;
}

bool CQuickGamePay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("md5Sign");

    CMyString strNtData = req.getParam("nt_data");
    CMyString strSign = req.getParam("sign");

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

    CMyString str1 = strNtData + strSign;
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

void CQuickGamePay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    if (bSucceed)
    {
        safe_strcpy(szResp, cbResp, "SUCCESS");
    }
    else
    {
        safe_strcpy(szResp, cbResp, "FAILED");
    }
    cbResp = strlen(szResp);
}

void CQuickGamePay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CQuickGamePay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CQuickGamePay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CQuickGamePay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CQuickGamePay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}
