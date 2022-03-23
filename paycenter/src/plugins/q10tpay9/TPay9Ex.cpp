/*
 * =====================================================================================
 *
 *       Filename:  TPay9.cpp
 *
 *    Description:  tpay9的支付实现
 *
 *        Version:  1.0
 *        Created:  2021年06月05日 16时29分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "TPay9Ex.h"
#include "Security.h"
#include "SdkList.h"
#include "GameList.h"
#include "SdkChannelList.h"
#include "MyNet.h"

/////////////////////////////////////////////////////////////////////
// CPluginTPay9Ex
CPluginTPay9Ex::CPluginTPay9Ex()
    : CPayBase("Q10TPay9", depositRequest, httpRequestSync)
{
   
}

CPluginTPay9Ex::~CPluginTPay9Ex()
{
}

int32_t CPluginTPay9Ex::Init(void)
{
    int32_t ret = CPayBase::Init("main/q10tpay9");
    return ret;
}

int32_t CPluginTPay9Ex::Update(void)
{
    CPayBase::Init("main/q10tpay9");
    return 0;
}

void CPluginTPay9Ex::Maintance()
{
    CPayBase::Maintance();
}

// 以下为调用网银接口
ENTRY_FUNC_DECLARE(CPluginTPay9Ex, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginTPay9Ex, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginTPay9Ex, doWithdraw)
{
    return CPayBase::doCreditRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginTPay9Ex, doWithdrawCB)
{
    return CPayBase::doCreditCallback(pClient, req, szResp, cbResp, ext_headers);
}

bool CPluginTPay9Ex::GetSecret(CMyString& strOrderNo, CMyString& strSecret)
{
    CMyString strKeyData;
    bool bFind = false;
    m_pDB->Cmd("SELECT `keyData`, `channelId` FROM `t_sdkchannel` WHERE `channelId`=(SELECT `sdkChannelId` FROM `t_payorder` WHERE `orderNo`='%s');", strOrderNo.c_str());
    while (m_pDB->More())
    {
        bFind = true;
        strKeyData = m_pDB->GetString(0);
    }
    char szError[128] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("[%s]GetSecret get keyData fail, orderNo text[%s], Error[%s]",
               m_sdkTag.c_str(), strOrderNo.c_str(), szError);
        return false;
    }
    if (!bFind)
    {
        LogErr("[%s]GetSecret orderNo not found, orderNo text[%s]",
               m_sdkTag.c_str(), strOrderNo.c_str());
        return false;
    }

    CJsonWrapper keyData;
    if (!keyData.Init(strKeyData))
    {
        LogErr("[%s]GetSecret t_sdkchannel keyData field not valid json, orderNo text[%s], KeyData text[%s]",
               m_sdkTag.c_str(), strOrderNo.c_str(), strKeyData.c_str());
        return false;
    }

    strSecret = keyData.getString("secret");
    return true;
}

//充值
void CPluginTPay9Ex::doMakeDepositRequest(http::CHttpRequest &req, CGame *pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString &url, CMyString &contentType, CMyString &httpData)
{
    LogDebug("[%s] doMakeDepositRequest",  m_sdkTag.c_str());

    CJsonWrapper jrKeyData;
    pChannel->getKeyData(jrKeyData);

    Json::Value data;
    data["appid"] = jrKeyData.getString("appid");
    data["cliIP"] = req.getParam("playerIp");
    data["cliNA"] = req.getParam("clientType");
    data["uid"] = req.getParam("uin");
    data["order"] = orderNo;
    data["price"] = (int32_t)(req.getParamInt64("amount") / 100.0);
    data["payBank"] = pChannel->getBankCode();
    data["payAcc"] = req.getParam("bankCard");

    LogDebug("[%s] doMakeDepositRequest secret = [%s]",  m_sdkTag.c_str(),jrKeyData.getString("secret").c_str());
    LogDebug("[%s] doMakeDepositRequest apikey = [%s]",  m_sdkTag.c_str(),pSdk->getApiKey().c_str());

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('\0', '=', false, true, false);
    str1.Append("secret=%s", jrKeyData.getString("secret").c_str());
    CMyString strMD5 = CMD5Wrapper::encode(str1.c_str());
    strMD5.MakeLower();
    req_maker.add("notifyUrl", pSdk->getPayCallbackUrl());
    req_maker.add("sn", strMD5);

    httpData = req_maker.http_str_manual('&', '=', false, true, true);
    url = pSdk->getPayUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}

bool CPluginTPay9Ex::doParseDepositResp(CGame *pGame, const CMyString &respHeader, const CMyString &respData, CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    LogDebug("[%s] doParseDepositResp",  m_sdkTag.c_str());

    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        errNote.Format("resp data not json string");
        return false;
    }

    int32_t nCode = jrRoot.getInt("e");
    if (nCode != 0)
    {
        errNote = jrRoot.getString("m");
        return false;
    }

    Json::Value data;
    if (!jrRoot.getJsonObject("d", data))
    {
        errNote.Format("resp data d not json string");
        return false;
    }

    depositUrlData = data["h5"].asString();
    depositOrderNo = "N/A";
    LogDebug("[%s] doParseDepositResp : depositUrlData[%s]",  m_sdkTag.c_str(), depositUrlData.c_str());

    return true;
}

bool CPluginTPay9Ex::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &depositOK)
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

    orderNo = req.getParam("order");
    platformOrderNo = req.getParam("transaction_id");
    realFee = (int32_t)(req.getParamInt64("amount") * 100);
    fee = realFee;
    tax = 0;

    // 请求参数中不包含订单状态值,有触发回调的均为成功订单
    depositOK = true;
    return true;
}

bool CPluginTPay9Ex::validDepositCallbackSign(http::CHttpRequest &req, CSdk *pSdk, CSdkChannel *pChannel, CMyString &errNote)
{
    LogDebug("[%s] validDepositCallbackSign",  m_sdkTag.c_str());

    CMyString strSign1 = req.getParam("sn");
    HTTP_REQ_STR(strOrderNo, order);

    CMyString strSecret;
    if (!GetSecret(strOrderNo, strSecret))
    {
        LogErr("[%s]deposit callback sign get Secret fail, orderNo text[%s]",
               m_sdkTag.c_str(), strOrderNo.c_str());
        
        return false;
    }

    Json::Value data;
    data["order"] = strOrderNo;
    data["uid"] = req.getParam("uid");
    data["gid"] = "";
    data["transaction_id"] = req.getParam("transaction_id");
    data["amount"] = (int32_t)(req.getParamInt64("amount"));

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('\0', '=', false, true, false);
    str1.Append("secret=%s", strSecret.c_str());
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

void CPluginTPay9Ex::doDepositCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t &cbResp)
{
    LogDebug("[%s] doDepositCallbackResp bSucceed[%d]",  m_sdkTag.c_str(), bSucceed);
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

//提现
void CPluginTPay9Ex::doMakeCreditRequest(Json::Value &req, CGame *pGame, CSdk *pSdk, CMyString &url, CMyString &contentType, CMyString &httpData)
{
    LogDebug("[%s] doMakeCreditRequest",  m_sdkTag.c_str());
    CMyString strOrderNo =  req["orderNo"].asString();
    Json::Value data;
    data["appid"] = pSdk->getAccount();
    data["orderId"] = strOrderNo; // TODO: 需要改到DB配置
    data["name"] = req["bankName"].asString();
    data["money"] = (Json::Int64)(req["fee"].asInt64()/100);
    data["bankMark"] = req["bankCode"].asString();
    data["recAcc"] = req["bankCard"].asString();
    data["remark"] = "";
    
    LogDebug("[%s] doMakeCreditRequest apikey = [%s]",  m_sdkTag.c_str(),pSdk->getApiKey().c_str());

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('\0', '=', false, true, false);
    str1.Append("secret=%s", pSdk->getApiKey().c_str());
    CMyString strMD5 = CMD5Wrapper::encode(str1.c_str());
    strMD5.MakeLower();
    req_maker.add("notifyUrl", pSdk->getWithdrawCallbackUrl());
    req_maker.add("sn", strMD5);

    httpData = req_maker.http_str_manual('&', '=', false, true, true);
    url = pSdk->getWithdrawUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}

bool CPluginTPay9Ex::doParseCreditResp(CGame *pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    LogDebug("[%s]credit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());

    //ok为成功，其他均为失败
    if (strcasecmp(respData.c_str(), "ok") == 0)
    {
        creditOrderNo = "N/A";
        return true;
    }

    errNote.Format(respData.c_str());
    return false;
}

bool CPluginTPay9Ex::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString params;
    req.getFullParamStr(params);
    LogDebug("[%s]credit callback, header[%s], param[%s], post[%s]",
             m_sdkTag.c_str(), headers.c_str(), params.c_str(), strPost.c_str());

    //只有在完成或者拒绝订单后，才会回调订单状态

    orderNo = req.getParam("order");
    creditOrderNo = "N/A";
    realFee = (int64_t)(req.getParam("amount").asDouble()*100);
    fee = realFee;
    tax = 0;

    int32_t status = req.getParamInt("status"); //订单状态(完成:4拒绝:5)
    if (5 == status)
    {
        creditOK = false;
        LogDebug("[%s]credit callback refuse, orderNo[%s]", m_sdkTag.c_str(), orderNo.c_str());
    }
    else
    {
        creditOK = true;
    }

    return true;
}

bool CPluginTPay9Ex::validCreditCallbackSign(http::CHttpRequest &req, CGame *pGame, CSdk *pSdk, CMyString &errNote)
{
    LogDebug("[%s] validCreditCallbackSign",  m_sdkTag.c_str());
    CMyString strSign1 = req.getParam("sn");  
    CMyString strOrderNo =  req.getParam("orderNo");

   
    LogDebug("[%s] doMakeCreditRequest apikey = [%s]",  m_sdkTag.c_str(),pSdk->getApiKey().c_str());

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sn");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString str1 = resp_maker.http_str_manual('\0', '=', false, true, false);
    str1.Append("secret=%s", pSdk->getApiKey().c_str());
    CMyString strSign2 = CMD5Wrapper::encode(str1.c_str());

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]credit callback sign mismatch, origin text [%s], sign[%s], sign from platform[%s]",
               m_sdkTag.c_str(), str1.c_str(), strSign2.c_str(), strSign1.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]credit callback sign mismatch, origin text [%s], sign[%s], sign from platform[%s]", 
                m_sdkTag.c_str(), str1.c_str(), strSign2.c_str(), strSign1.c_str()));
        errNote = "credit callback sign mismatch";
        return false;
    }
    return true;
}

void CPluginTPay9Ex::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t &cbResp)
{
    LogDebug("[%s] doCreditCallbackResp bSucceed:[%d]",  m_sdkTag.c_str(), bSucceed);
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
