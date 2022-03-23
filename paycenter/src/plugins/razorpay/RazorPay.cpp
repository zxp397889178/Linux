/*
 * =====================================================================================
 *
 *       Filename:  RazorPay.cpp
 *
 *    Description:  RazorPay 实现
 *
 *        Version:  1.0
 *        Created:  2020年09月01日 11时45分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "RazorPay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginRazorPay::CPluginRazorPay()
    : CPayWrapper()
{
    //mkdir("./html/Fun", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginRazorPay::~CPluginRazorPay()
{
}

int32_t CPluginRazorPay::Init(void)
{
    CMyString strSect("main/razor");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    
    return 0;
}

int32_t CPluginRazorPay::Update(void)
{
    return this->Init();
}

void CPluginRazorPay::Maintance()
{
    CPayWrapper::Maintance();
}

CMyString getNonce(int32_t len)
{
    static const char s_szText[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static const size_t s_cbText = (int32_t)strlen(s_szText);

    CMyString str;
    for(int32_t i(0); i<len; ++i)
    {
        int32_t idx = (int32_t)(RandGet(true) % s_cbText);
        char c = s_szText[idx];
        str += c;
    }
    return str;
}

ENTRY_FUNC_DECLARE(CPluginRazorPay, doPay)
{
    int32_t nOrderId(0);
    CMyString strOrderNo;
    CSdkChannel *pChannel(NULL);
    int32_t ret = CPayWrapper::_before_send_request(req, pChannel, nOrderId, strOrderNo, szResp, cbResp);
    if (ret < 0)
    {
        return ret; 
    }

    // 获得用户的真实IP
    CMyString strIp = req.getParam("playerIp");
    CMyString strRealIp = getClientIp(pClient, req);
    if (strIp=="0.0.0.0" || strIp=="127.0.0.1")
    {
        LogDebug("ip in param[%s], real ip[%s]", strIp.c_str(), strRealIp.c_str());
        strIp = strRealIp;
        pClient->setIp(strIp);
    }
    CSdk* pSdk = pChannel->getSdk();
    CGame *pGame = pChannel->getGame();

///////// 以下为宝丽平台专用调用
    int32_t payMoney(0);
    CMyString strProductId;
    CMyString strRemark;
    req.getParam("productId", strProductId);
    req.getParam("Remark", strRemark);
    req.getParam("amount", payMoney);


    // 获得pSDK的扩展配置参数
    bool bUseProxy(false);
    CJsonWrapper jrExtra;
    pSdk->getPayExtraData(jrExtra);
    CJsonWrapper jrProxy;
    if (jrExtra.getReader("proxy", jrProxy))
    {
        bUseProxy = jrProxy.getBool("use_proxy");
    }

    // 生成发送到支付平台的数据
    CMyString strUin = req.getParam("uin");

    // 构建 HTTP 数据
    Json::Value req_data;
    if (bUseProxy)
    {
        CMyString strAppName = jrProxy.getString("appName");
        CMyString strAppKey = jrProxy.getString("appSecret");
        CMyString strNonce = getNonce(8);
        req_data["appName"] = strAppName;
        req_data["noice"] = strNonce;
        req_data["amount"] = payMoney;
        CMyString str1("%s%s%s", strAppName.c_str(), strNonce.c_str(), strAppKey.c_str());
        CMyString strSign = CMD5Wrapper::encode(str1.c_str());
        strSign.MakeLower();
        req_data["sign"] = strSign;
        CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
        CMyString strUrl = CHttpWrapper::encodeURIComponentEx(strNotifyUrl, true);
        req_data["redirect_uri"] = strUrl;
        req_data["receipt"] = strOrderNo;
        Json::Value req_notes;
        req_notes["uin"]=strUin;
        req_notes["clientIp"]=strIp;
        req_data["notes"]=req_notes;
    }
    else
    {
        req_data["amount"] = payMoney;
        req_data["currency"] = "INR";
        req_data["receipt"] = strOrderNo;
        req_data["payment_capture"] = 1;
        Json::Value req_notes;
        req_notes["appName"]="quwan";
        req_notes["uin"]=strUin;
        req_notes["clientIp"]=strIp;
        req_data["notes"]=req_notes;
    }
    CJsonWrapper writer;
    CMyString strSign = writer.write(req_data);


    // 构建HTTP 头
    CMyString strContentType("Content-Type: application/json");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    //Basic 验证： https://blog.csdn.net/zxw75192/article/details/91664061
    if (!bUseProxy)
    {
        CMyString strAuth("%s:%s", pSdk->getAccount().c_str(), pSdk->getApiKey().c_str());
        CMyString strHdr = CBase64Wrapper::Encode(strAuth.c_str(), strAuth.length(), false);
        strAuth.Format("Authorization: Basic %s", strHdr.c_str());
        header_list.push_back(strAuth.c_str());
    }

    CMyString strHeader;
    CMyString strResp;
    CMyString strError;
    int32_t result(-1);
    CMyString platformOrderNo("N/A");
    CMyString strTransferUrl;
    do {
        CMyString strUrl = bUseProxy ? jrProxy.getString("payUrl") : pSdk->getPayUrl();
        LogDebug("razor url [%s]", strUrl.c_str());
        LogDebug("razor data [%s]", strSign.c_str());
        ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            CMyString strMsg;
            m_http.getError(ret, strMsg);
            CMyString strNote("用户(uin=%d)调起Razor支付失败，错误码:%d, 错误说明:%s", strUin.c_str(), ret, strMsg.c_str() );
            CPayWrapper::postAlert(pGame, strNote);

            strError.Format("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("RazorPay header [ %s ]", strHeader.c_str());
        LogWarn("RazorPay resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealJsonResp(strResp, strTransferUrl, strError, platformOrderNo, pChannel, payMoney);
    }while(0);

    time_t t1 = time(NULL);
    char szReqTime[64] = "";
    TimeToStr(t1, szReqTime, sizeof(szReqTime));
    if (result < 0)
    {
        // 记录数据库，失败
        CPayWrapper::_after_send_request(nOrderId, payReqFail, strSign, szReqTime, strHeader, strResp, platformOrderNo);
        this->OnPayFailure(result, strError.c_str(), szResp, cbResp);
        // TODO: 订单数据让其超时
        return result;
    }
    else
    {
        CPayWrapper::_after_send_request(nOrderId, payReqSucceed, strSign, szReqTime, strHeader, strResp, platformOrderNo);
    }

    // 记录支付的主键
    CPayWrapper::on_save_orderId(strOrderNo, nOrderId);

    // 构建返回给前端的JSON串
    if (bUseProxy)
    {
        // 返回给调用端
        CPayWrapper::OnPayResp(0, "transfer to url", strOrderNo,
                strTransferUrl, szResp, cbResp,
                pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());
    }
    else
    {
        Json::Value resp_data;
        resp_data["key"] = pSdk->getAccount();
        resp_data["amount"] = payMoney;
        resp_data["currency"] = "INR";
        resp_data["name"] = "quwan";
        resp_data["order_id"] = platformOrderNo;
        CMyString strRespData = writer.write(resp_data);

        // 返回给调用端
        CPayWrapper::OnPayResp(0, "transfer to sdk", strOrderNo,
                strRespData, szResp, cbResp,
                pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());
    }
    return 0;
}

/* {"gameCode":"INDIAGAME", "uin":123456, "name":"Gaurav Kumar", "email":"gaurav.kumar@example.com", "contact":"9123456789"}
 *
 * {"code":0, "message":"OK", "uin":"12345678", "gameCode":"INDIAGAME", "contactId":"cont_FY9Vve2g22Mq65"}
 */
ENTRY_FUNC_DECLARE(CPluginRazorPay, doKYC)
{
    // 检校参数是否合法
    CMyString strErr;
    std::vector<CMyString> vecParam;
    vecParam.push_back("uin");
    vecParam.push_back("gameCode");
    vecParam.push_back("name");
    vecParam.push_back("email");
    vecParam.push_back("contact");
    if (!req.ValidParam(vecParam, strErr))
    {
        int32_t err = PAYFRAME_ERROR(err_MissingParameters); 
        LogErr(strErr.c_str());
        CPayWrapper::OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;

    }

    // 向Razor发送请求
    // https://razorpay.com/docs/razorpayx/api/payouts-cards/#create-a-contact
    CMyString strGameCode = req.getParam("gameCode");
    CSdk *pSDK = SDK_LIST.FindByCode(strGameCode, m_sdkCode);
    if (NULL == pSDK)
    {
        int32_t err = PAYFRAME_ERROR(err_invalidGameCode);
        strErr.Format("unknown gameCode:%s", strGameCode.c_str());
        LogErr(strErr.c_str());
        CPayWrapper::OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }

    // 获得pSDK的扩展配置参数
    bool bUseProxy(false);
    CJsonWrapper jrExtra;
    pSDK->getPayExtraData(jrExtra);
    CJsonWrapper jrProxy;
    if (jrExtra.getReader("proxy", jrProxy))
    {
        bUseProxy = jrProxy.getBool("use_proxy");
    }

    CMyString strIp = getClientIp(pClient, req);
    CMyString strUin = req.getParam("uin");

    // 构建请求数据
    Json::Value req_data;
    req_data["name"] = req.getParam("name");
    req_data["email"] = req.getParam("email");
    req_data["contact"] = req.getParam("contact");
    req_data["type"] = "customer";
    req_data["reference_id"] = "quwan";
    Json::Value req_note;
    req_note["uin"] = strUin;
    req_note["ip"] = strIp;
    req_data["notes"] = req_note;
    if (bUseProxy)
    {
        CMyString strAppName = jrProxy.getString("appName");
        CMyString strAppKey = jrProxy.getString("appSecret");
        CMyString strNonce = getNonce(8);
        req_data["appName"] = strAppName;
        req_data["noice"] = strNonce;
        CMyString str1("%s%s%s", strAppName.c_str(), strNonce.c_str(), strAppKey.c_str());
        CMyString strSign = CMD5Wrapper::encode(str1.c_str());
        strSign.MakeLower();
        req_data["sign"] = strSign;
    }
    CJsonWrapper writer;
    CMyString strSign = writer.write(req_data);


    // 构建请求头
    CMyString strContType("Content-Type: application/json");
    std::vector<std::string> header_list;
    header_list.push_back(strContType.c_str());
    if (!bUseProxy)
    {
        //Basic 验证： https://blog.csdn.net/zxw75192/article/details/91664061
        CMyString strAuth("%s:%s", pSDK->getAccount().c_str(), pSDK->getApiKey().c_str());
        CMyString strHdr = CBase64Wrapper::Encode(strAuth.c_str(), strAuth.length(), false);
        strAuth.Format("Authorization: Basic %s", strHdr.c_str());
        header_list.push_back(strAuth.c_str());
    }

    Json::Value resp_data;
    int32_t result(-1);
    do {
        CMyString strUrl = bUseProxy ? jrProxy.getString("kycUrl") : jrExtra.getString("kycUrl");
        CMyString strHeader;
        CMyString strResp;
        int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            CMyString strMsg;
            m_http.getError(ret, strMsg);
            CMyString strNote("用户(uin=%d)调起Razor KYC失败，错误码:%d, 错误说明:%s", strUin.c_str(), ret, strMsg.c_str() );
            CGame *pGame = CPayWrapper::getGame(strGameCode);
            CPayWrapper::postAlert(pGame, strNote);

            CMyString strError("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            resp_data["code"] = -99;
            resp_data["message"] = strMsg;
            break;
        }
        LogWarn("[RAZOR] contact header [ %s ]", strHeader.c_str());
        LogWarn("[RAZOR] contact resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        //strError = "invalid resp data.";
        result = this->dealKycResp(strResp, bUseProxy, resp_data);
    }while(0);

    // 构建返回值
    resp_data["uin"] = strUin;
    resp_data["gameCode"] = strGameCode;
    CMyString strData = writer.write(resp_data);
    safe_strcpy(szResp, cbResp, strData.c_str());
    cbResp = strlen(szResp);

    return 200;
}

/* 
 * {"gameCode":"INDIAGAME","uin":"12345678","contactId":"cont_FY9Vve2g22Mq65","account_name":"Gaurav Kumar","ifsc":"HDFC0000053","account_number":"5104015555555558"}
 *
 * {"code":0, "message":"OK", "uin":"12345678", "gameCode":"INDIAGAME", "contactId":"cont_FY9Vve2g22Mq65", "fundId":"fa_00000000000001", "active":true}
 */
ENTRY_FUNC_DECLARE(CPluginRazorPay, doBindCard)
{
    // 校验参数是否合法
    CMyString strErr;
    std::vector<CMyString> vecParam;
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    vecParam.push_back("contactId");
    vecParam.push_back("account_name");
    vecParam.push_back("account_number");
    vecParam.push_back("ifsc");
    if (!req.ValidParam(vecParam, strErr))
    {
        int32_t err = PAYFRAME_ERROR(err_MissingParameters); 
        LogErr(strErr.c_str());
        CPayWrapper::OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;

    }

    // 向Razor发送请求
    // https://razorpay.com/docs/razorpayx/api/payouts-cards/#create-a-fund-account-for-a-contacts-card
    CMyString strGameCode = req.getParam("gameCode");
    CSdk *pSDK = SDK_LIST.FindByCode(strGameCode, m_sdkCode);
    if (NULL == pSDK)
    {
        int32_t err = PAYFRAME_ERROR(err_invalidGameCode);
        strErr.Format("unknown gameCode:%s", strGameCode.c_str());
        LogErr(strErr.c_str());
        CPayWrapper::OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }

    // 获得pSDK的扩展配置参数
    bool bUseProxy(false);
    CJsonWrapper jrExtra;
    pSDK->getPayExtraData(jrExtra);
    CJsonWrapper jrProxy;
    if (jrExtra.getReader("proxy", jrProxy))
    {
        bUseProxy = jrProxy.getBool("use_proxy");
    }

    CMyString strIp = getClientIp(pClient, req);
    CMyString strUin = req.getParam("uin");
    CMyString strContactId = req.getParam("contactId");

    // 构建请求数据
    Json::Value req_data;
    req_data["contact_id"] = strContactId;
    req_data["account_type"] = "bank_account";
    Json::Value card_data;
    card_data["account_number"] = req.getParam("account_number");
    card_data["name"] = req.getParam("account_name");
    card_data["ifsc"] = req.getParam("ifsc");
    req_data["bank_account"] = card_data;
    if (bUseProxy)
    {
        CMyString strAppName = jrProxy.getString("appName");
        CMyString strAppKey = jrProxy.getString("appSecret");
        CMyString strNonce = getNonce(8);
        req_data["appName"] = strAppName;
        req_data["noice"] = strNonce;
        CMyString str1("%s%s%s", strAppName.c_str(), strNonce.c_str(), strAppKey.c_str());
        CMyString strSign = CMD5Wrapper::encode(str1.c_str());
        strSign.MakeLower();
        req_data["sign"] = strSign;
    }
    CJsonWrapper writer;
    CMyString strSign = writer.write(req_data);


    // 构建请求头
    CMyString strContType("Content-Type: application/json");
    std::vector<std::string> header_list;
    header_list.push_back(strContType.c_str());
    if (!bUseProxy)
    {
        //Basic 验证： https://blog.csdn.net/zxw75192/article/details/91664061
        CMyString strAuth("%s:%s", pSDK->getAccount().c_str(), pSDK->getApiKey().c_str());
        CMyString strHdr = CBase64Wrapper::Encode(strAuth.c_str(), strAuth.length(), false);
        strAuth.Format("Authorization: Basic %s", strHdr.c_str());
        header_list.push_back(strAuth.c_str());
    }

    Json::Value resp_data;
    int32_t result(-1);
    do {
        CMyString strUrl = bUseProxy ? jrProxy.getString("fundUrl") : jrExtra.getString("fundUrl");
        CMyString strHeader;
        CMyString strResp;
        int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            CMyString strMsg;
            m_http.getError(ret, strMsg);
            CMyString strNote("用户(uin=%d)调起Razor 创建Fund账户失败，错误码:%d, 错误说明:%s", strUin.c_str(), ret, strMsg.c_str() );
            CGame *pGame = CPayWrapper::getGame(strGameCode);
            CPayWrapper::postAlert(pGame, strNote);

            CMyString strError("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            resp_data["code"] = -99;
            resp_data["message"] = strMsg;
            break;
        }
        LogWarn("[RAZOR] fund header [ %s ]", strHeader.c_str());
        LogWarn("[RAZOR] fund resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        int32_t result = this->dealFundResp(strResp, bUseProxy, resp_data);
    }while(0);

    // 构建返回值
    resp_data["uin"] = strUin;
    resp_data["gameCode"] = strGameCode;
    resp_data["contactId"] = strContactId;
    CMyString strData = writer.write(resp_data);
    safe_strcpy(szResp, cbResp, strData.c_str());
    cbResp = strlen(szResp);

    return 200;
}

/**
 * { "gameCode":"INDIAGAME", "uin":"123456", "fundId":"fa_00000000000001", "amount":1000, "note":"扩展数据"}
 *
 *
 * {"code": -1, "gameCode": "INDIAGAME", "message": "The id provided does not exist", "uin": "123456" }
 * {"code": 0, "gameCode": "INDIAGAME", "message": "OK", "uin": "123456", "withdraw_id":"pout_00000000000001", "fundId":"fa_00000000000001", "amount":10000, "fees":50, "tax":0 }
 */
ENTRY_FUNC_DECLARE(CPluginRazorPay, doWithdraw)
{
    // 检查重要数据是否有传过来
    CMyString strErr;
    std::vector<CMyString> vecParam;
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    vecParam.push_back("fundId");
    vecParam.push_back("amount");
    vecParam.push_back("withdrawId");
    if (!req.ValidParam(vecParam, strErr))
    {
        int32_t err = PAYFRAME_ERROR(err_MissingParameters); 
        LogErr(strErr.c_str());
        CPayWrapper::OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;

    }

    // 向Razor请求
    // https://razorpay.com/docs/razorpayx/api/payouts-cards/#create-a-payout
    CMyString strGameCode = req.getParam("gameCode");
    CSdk *pSDK = SDK_LIST.FindByCode(strGameCode, m_sdkCode);
    if (NULL == pSDK)
    {
        int32_t err = PAYFRAME_ERROR(err_invalidGameCode);
        strErr.Format("unknown gameCode:%s", strGameCode.c_str());
        LogErr(strErr.c_str());
        CPayWrapper::OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }
    // 获得pSDK的扩展配置参数
    bool bUseProxy(false);
    CJsonWrapper jrExtra;
    pSDK->getPayExtraData(jrExtra);
    CJsonWrapper jrProxy;
    if (jrExtra.getReader("proxy", jrProxy))
    {
        bUseProxy = jrProxy.getBool("use_proxy");
    }

    CMyString strIp = getClientIp(pClient, req);
    CMyString strUin = req.getParam("uin");
    CMyString withdrawId = req.getParam("withdrawId");

    CMyString strAccountNo = jrExtra.getString("account_no");
    // 构建请求数据
    Json::Value req_data;
    req_data["account_number"] = strAccountNo;
    req_data["fund_account_id"] = req.getParam("fundId");
    req_data["amount"] = req.getParamInt("amount");
    req_data["currency"] = "INR";
    req_data["mode"] = "IMPS";
    req_data["purpose"] = "refund";
    req_data["queue_if_low_balance"] = true;
    req_data["reference_id"] = withdrawId;
    req_data["narration"] = strGameCode;
    Json::Value notes;
    notes["uin"] = strUin;
    notes["ip"] = strIp;
    notes["vendor"] = "quwan";
    req_data["notes"] = notes;
    if (bUseProxy)
    {
        CMyString strAppName = jrProxy.getString("appName");
        CMyString strAppKey = jrProxy.getString("appSecret");
        CMyString strNonce = getNonce(8);
        req_data["appName"] = strAppName;
        req_data["noice"] = strNonce;
        CMyString str1("%s%s%s", strAppName.c_str(), strNonce.c_str(), strAppKey.c_str());
        CMyString strSign = CMD5Wrapper::encode(str1.c_str());
        strSign.MakeLower();
        req_data["sign"] = strSign;
    }
    CJsonWrapper writer;
    CMyString strSign = writer.write(req_data);

    // 构建请求头
    CMyString strContType("Content-Type: application/json");
    std::vector<std::string> header_list;
    header_list.push_back(strContType.c_str());
    if (!bUseProxy)
    {
        //Basic 验证： https://blog.csdn.net/zxw75192/article/details/91664061
        CMyString strAuth("%s:%s", pSDK->getAccount().c_str(), pSDK->getApiKey().c_str());
        CMyString strHdr = CBase64Wrapper::Encode(strAuth.c_str(), strAuth.length(), false);
        strAuth.Format("Authorization: Basic %s", strHdr.c_str());
        header_list.push_back(strAuth.c_str());
    }

    Json::Value resp_data;
    resp_data["gameCode"] = strGameCode;
    resp_data["uin"] = strUin;
    resp_data["fundId"] = req.getParam("fundId");
    int32_t result(-1);
    do {
        CMyString strUrl = bUseProxy ? jrProxy.getString("payoutUrl") : pSDK->getWithdrawUrl();
        LogDebug("withdraw url [%s]", strUrl.c_str());
        CMyString strHeader;
        CMyString strResp;
        int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            CMyString strMsg;
            m_http.getError(ret, strMsg);
            CMyString strNote("用户(uin=%d)调起Razor 提现转账失败，错误码:%d, 错误说明:%s", strUin.c_str(), ret, strMsg.c_str() );
            CGame *pGame = CPayWrapper::getGame(strGameCode);
            CPayWrapper::postAlert(pGame, strNote);

            CMyString strError("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            resp_data["code"] = -99;
            resp_data["message"] = strMsg;
            break;
        }
        LogWarn("[RAZOR] withdraw header [ %s ]", strHeader.c_str());
        LogWarn("[RAZOR] withdraw resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        result = this->dealWithdrawResp(strResp, bUseProxy, resp_data);
    }while(0);

    Json::Value resp_out;
    resp_out["data"] = resp_data;
    CMyString strData = writer.write(resp_data);
    safe_strcpy(szResp, cbResp, strData.c_str());
    cbResp = strlen(szResp);

    return 200;
}

ENTRY_FUNC_DECLARE(CPluginRazorPay, doCallback)
{
    safe_strcpy(szResp, cbResp, "FAIL");
    cbResp = strlen(szResp);

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    // LogInfo("RazorPay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("[RAZOR] resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());

    // TODO: 验签
    CJsonWrapper jr;
    if (!jr.Init(strCallbackResp.c_str()))
    {
        LogErr("[RAZOR] resp [%s] not valid json string.", strCallbackResp.c_str());
        safe_strcpy(szResp, cbResp, "FAIL");
        cbResp = strlen(szResp);
        return -1;
    }
    CMyString strEntity = jr.getString("entity");
    CMyString strEvent = jr.getString(strEntity);
    if (strEvent == "order.paid")
    {
        // 解析所需要的信息
        CMyString strOrderNo;
        CMyString strPlatformId;
        CMyString strStatus;
        int32_t nFee(0);
        int32_t nRealFee(0);
        int32_t ret = this->decode_order(jr, strOrderNo, strPlatformId, strStatus, nFee, nRealFee);
        if (ret != 0)
        {
            return PAYFRAME_ERROR(err_orderPayResp);
        }

        // 校验回调IP是否合法
        CGame *pGame(NULL);
        ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
        if (ret < 0)
        {
            return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
        }

        // TODO: 查询该UIN所赠送的金币等信息
        // 验签
        //
        // 确认是否回调充值
        int32_t code(payCallbackFail);
        if (strStatus == "paid")
        {
            code = payCallbackOK;
        }

        safe_strcpy(szResp, cbResp, "SUCCESS");
        cbResp = strlen(szResp);

        // 调用封装的通知函数
        ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
                strCallbackResp, code, nRealFee);
        return ret;
    }
    else if (strEvent == "payout.processed" || strEvent == "payout.reversed")
    {
        // https://razorpay.com/docs/razorpayx/api/webhooks/#payoutprocessed
        //
        safe_strcpy(szResp, cbResp, "fail");
        cbResp = strlen(szResp);

        http::CHttpRespMaker resp;
        CMyString strGameCode;
        CMyString strUin;
        int32_t ret = this->decode_payout(jr, strGameCode, strUin, resp);
        if (ret != 0)
        {
            return PAYFRAME_ERROR(err_payoutResp);
        }

        // TODO:
        // 检查回调IP是否合法
        //
        // 验签 
        //
        // 调用封装的扣款通知函数
        CGame *pGame = CPayWrapper::getGame(strGameCode);

        CMyString str1 = resp.http_str_manual('&','=', true, true);
        CMyString str2("%s&key=%s", str1.c_str(), pGame->getApiKey().c_str());
        CMyString strMD5 = CMD5Wrapper::encode(str2.c_str());
        strMD5.MakeLower();
        resp.add("sign", strMD5);
        CMyString strSign = resp.json_str();
        LogDebug("[RAZOR] origin withdraw [%s]", str2.c_str());
        LogDebug("[RAZOR] sign string     [%s]", strSign.c_str());

        CMyString strContType("Content-Type: application/json");
        std::vector<std::string> header_list;
        header_list.push_back(strContType.c_str());
        int32_t result(0);
        do {
            CMyString strUrl = pGame->getApiTranferUrl();
            CMyString strResp;
            CMyString strHeader;
            int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
            if (ret != 0)
            {
                CMyString strMsg;
                m_http.getError(ret, strMsg);
                CMyString strNote("用户(uin=%d) 提现扣款失败，错误码:%d, 错误说明:%s", strUin.c_str(), ret, strMsg.c_str() );
                CPayWrapper::postAlert(pGame, strNote);
                CMyString strError("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
                // 远程连接失败
                LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
                result = -1021;
                break;
            }
            LogWarn("[RAZOR] Server withdraw header[%s]", strHeader.c_str());
            LogWarn("[RAZOR] Server withdraw resp  [%s]", strResp.c_str());
        }while(0);

        safe_strcpy(szResp, cbResp, "success");
        cbResp = strlen(szResp);
        return 200;
    }
    else
    {
        szResp[0] = '\0';
        cbResp = 0;
        LogErr("[RAZOR] no deal entity[%s], event[%s]", strEntity.c_str(), strEvent.c_str());
    }

    return -1;
}

int32_t CPluginRazorPay::decode_payout(CJsonWrapper &jRoot, CMyString &strGameCode, CMyString &strUin, http::CHttpRespMaker &resp)
{
    // https://razorpay.com/docs/razorpayx/api/webhooks/#payoutprocessed
    // https://razorpay.com/docs/razorpayx/api/webhooks/#payoutreversed

    Json::Value jCont;
    if (!jRoot.getObject("contains", jCont))
    {
        resp.add("code", -1);
        resp.add("message", "invalid payout webhood resp");
        return -1;
    }
    
    bool val1(false);
    if (!jCont.isArray())
    {
        resp.add("code", -2);
        resp.add("message", "invalid payout webhood resp");
        return -2;
    }
    for(int32_t idx(0); idx<(int32_t)jCont.size(); ++idx)
    {
        CMyString str1 = jCont[idx].asString();
        if (str1 == "payout")
        {
            val1 = true;
        }
    }
    if (!val1)
    {
        resp.add("code", -3);
        resp.add("message", "invalid payout webhood resp");
        return -3;
    }

    CJsonWrapper jPayload;
    if (!jRoot.getReader("payload", jPayload))
    {
        resp.add("code", -4);
        resp.add("message", "invalid payout webhood resp");
        return -4;
    }
    CJsonWrapper jPayout;
    if (!jPayload.getReader("payout", jPayout))
    {
        resp.add("code", -5);
        resp.add("message", "invalid payout webhood resp");
        return -5;
    }
    CJsonWrapper jEntity;
    if (!jPayout.getReader("entity", jEntity))
    {
        resp.add("code", -6);
        resp.add("message", "invalid payout webhood resp");
        return -6;
    }

    resp.add("code", 0);
    resp.add("id", jEntity.getString("id"));
    resp.add("withdrawId", jEntity.getString("reference_id"));
    strGameCode = jEntity.getString("narration");
    resp.add("gameCode", strGameCode);
    CMyString strResult = jEntity.getString("status");
    if (strResult == "processed")
    {
        resp.add("deal", 1);
        resp.add("message", "OK");
    }
    else
    {
        resp.add("deal", -1);
        resp.add("message", jEntity.getString("failure_reason"));
    }
    resp.add("fundId", jEntity.getString("fund_account_id"));
    resp.add("amount", jEntity.getInt("amount"));
    resp.add("fee", jEntity.getInt("fees"));
    resp.add("tax", jEntity.getInt("tax"));
    Json::Value notes;
    if (jEntity.getObject("notes", notes))
    {
        strUin = notes["uin"].asString();
        resp.add("uin", strUin);
        resp.add("ip", notes["ip"].asString());
    }

    return 0;
}

int32_t CPluginRazorPay::decode_order(CJsonWrapper &jRoot, CMyString &strOrderNo, CMyString &strPlatformId, CMyString &strStatus, int32_t &nFee, int32_t &nRealFee)
{
    // https://razorpay.com/docs/payment-pages/webhooks/#event-payload---orderpaid
    Json::Value jCont;
    if (!jRoot.getObject("contains", jCont))
    {
        return -1;
    }

    if (!jCont.isArray())
    {
        return -2;
    }
    bool val1(false);
    bool val2(false);
    for(int32_t idx(0); idx<(int32_t)jCont.size(); ++idx)
    {
        CMyString str1 = jCont[idx].asString();
        if (str1 == "payment")
        {
            val1 = true;
        }
        if (str1 == "order")
        {
            val2 = true;
        }
    }
    if (!val1 || !val2)
    {
        return -2;
    }

    CJsonWrapper jPayload;
    if (!jRoot.getReader("payload", jPayload))
    {
        return -3;
    }

    CJsonWrapper jPayment;
    CJsonWrapper jOrder;
    val1 = jPayload.getReader("payment", jPayment);
    val2 = jPayload.getReader("order", jOrder);
    if (!val1 || !val2)
    {
        return -4;
    }

    CJsonWrapper jPaymentEntity;
    CJsonWrapper jOrderEntity;
    val1 = jPayment.getReader("entity", jPaymentEntity);
    val2 = jOrder.getReader("entity", jOrderEntity);
    if (!val1 || !val2)
    {
        return -5;
    }

    // 查找出平台的orderId, 以及订单的实际金额
    strPlatformId = jPaymentEntity.getString("order_id");
    strOrderNo = jOrderEntity.getString("receipt");
    strStatus = jOrderEntity.getString("status");
    nFee = jOrderEntity.getInt("amount");
    nRealFee = jOrderEntity.getInt("amount_paid");

    return 0;
}

int32_t CPluginRazorPay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo, CSdkChannel *pChannel, int32_t payMoney)
{
    CGame* pGame = pChannel->getGame();
    CSdk *pSdk = pChannel->getSdk();

    Json::Value root;
    CJsonWrapper reader;
    if (!reader.parse(strResp.c_str(), root, false))
    {
        CMyString strNote("RazorPay支付调起结果异常, %s", strResp.c_str());
        CPayWrapper::postAlert(pGame, strNote);

        strError.Format("RazorPay支付调用异常,请联系客服");
        // 返回串不合法
        LogErr("[%s:%s:%d]RazorPay resp [%s] not valid json string", __FILE__, __func__, __LINE__, strResp.c_str());
        return -1023;
    }

    // TODO: 必须要验证签名
    //
    //

    CJsonWrapper jrExtra;
    pSdk->getPayExtraData(jrExtra);
    bool bUseProxy(false);
    CJsonWrapper jrProxy;
    if (jrExtra.getReader("proxy", jrProxy))
    {
        bUseProxy = jrProxy.getBool("use_proxy");
    }

    if (bUseProxy)
    {
        int32_t code = root["errcode"].asInt();
        if(code != 0)
        {
            CMyString strMsg = root["errmsg"].asString(); 

            // STEP1: 发送给IM
            CMyString strNote("RazorPay 支付调用失败，错误描述:%s", strMsg.c_str());
            CPayWrapper::postAlert(pChannel->getGame(), strNote);

            // STEP2: 发给前端
            CMyString strClientMsg;
            int32_t deal = CPayWrapper::dealRespError(pChannel, payMoney,strMsg, strClientMsg);
            strError.Format("[RazorPay支付]%s", strClientMsg.c_str());

            // STEP4: 记录日志
            LogErr("[%s:%s:%d]RazorPay call failure, code:%d, error:%s", __FILE__, __func__, __LINE__, code, strMsg.c_str());
            return -1025;
        }

        platformOrderNo = "N/A";
        strTransferUrl = root["data"]["url"].asString();
    }
    else
    {

        // 判断有没有 "error" 字段
        if (root.isMember("error"))
        {
            Json::Value err = root["error"];
            CMyString strMsg = err["description"].asString();
            CMyString strCode = err["code"].asString();
            // STEP1: 发给IM
            CMyString strNote("RazorPay支付调起失败,错误描述:%s", strMsg.c_str());
            CPayWrapper::postAlert(pChannel->getGame(), strNote);

            // STEP2: 发给前端
            CMyString strClientMsg;
            int32_t deal = CPayWrapper::dealRespError(pChannel, payMoney,strMsg, strClientMsg);
            strError.Format("[RazorPay支付]%s", strClientMsg.c_str());

            // STEP4: 记录日志
            LogErr("[%s:%s:%d]RazorPay call failure, code:%s, error:%s", __FILE__, __func__, __LINE__, strCode.c_str(), strMsg.c_str());
            return -1025;
        }

        platformOrderNo = root["id"].asString();
        strTransferUrl = platformOrderNo;
    }

    return 0;
}

int32_t CPluginRazorPay::dealKycResp(CMyString &strResp, bool bUseProxy, Json::Value &resp)
{
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(strResp.c_str()))
    {
        resp["code"] = -1;
        resp["message"] = "invalid razor KYC resp";
        return -1;
    }

    if (bUseProxy)
    {
        int32_t code = jrRoot.getInt("errcode"); 
        if (code != 0)
        {
            resp["code"] = -2;
            resp["message"] = jrRoot.getString("errmsg");
            return -2;
        }

        resp["code"] = 0;
        resp["message"] = "OK";
        CJsonWrapper jrData;
        jrRoot.getReader("data", jrData);
        resp["contactId"] = jrData.getString("id");
    }
    else
    {
        CJsonWrapper jrError;
        if (jrRoot.getReader("error", jrError))
        {
            resp["code"] = -3;
            resp["message"] = jrError.getString("description");
            return -3;
        }

        resp["code"] = 0;
        resp["message"] = "OK";
        resp["contactId"] = jrRoot.getString("id");
    }

    return 0;
}

int32_t CPluginRazorPay::dealFundResp(CMyString &strResp, bool bUseProxy, Json::Value &resp)
{
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(strResp.c_str()))
    {
        resp["code"] = -1;
        resp["message"] = "resp of razor fund account failure.";
        return -1;
    }

    if (bUseProxy)
    {
        int32_t code = jrRoot.getInt("errcode");
        if (code != 0)
        {
            resp["code"] = -2;
            resp["message"] = jrRoot.getString("errmsg");
            return -2;
        }

        CJsonWrapper jrData;
        if (!jrRoot.getReader("data", jrData))
        {
            resp["code"] = -3;
            resp["messagae"] = "resp of razor-fund-account proxy data failure. ";
            return -3;
        }

        resp["code"] = 0;
        resp["message"] = "OK";
        resp["fundId"] = jrData.getString("id");
        resp["active"] = jrData.getBool("active");
        return 0;
    }

    CJsonWrapper jrError;
    if (jrRoot.getReader("error", jrError))
    {
        resp["code"] = -4;
        resp["message"] = jrError.getString("description");
        return -4;
    }

    resp["code"] = 0;
    resp["message"] = "OK";
    resp["fundId"] = jrRoot.getString("id");
    resp["active"] = jrRoot.getBool("active");

    return 0;
}

int32_t CPluginRazorPay::dealWithdrawResp(CMyString &strResp, bool bUseProxy, Json::Value &resp)
{
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(strResp.c_str()))
    {
        resp["code"] = -1;
        resp["message"] = "invalid razor-withdraw resp";
        return -1;
    }


    if (bUseProxy)
    {
        int32_t code = jrRoot.getInt("errcode");
        if (code != 0)
        {
            resp["code"] = -2;
            resp["message"] = jrRoot.getString("errmsg");
            return -2;
        }

        CJsonWrapper jrData;
        if (!jrRoot.getReader("data", jrData))
        {
            resp["code"] = -3;
            resp["message"] = "invalid razor-withdraw proxy data";
            return -3;
        }

        resp["code"] = 0;
        resp["message"] = "OK";
        resp["id"] = jrData.getString("id");
        resp["withdrawId"] = jrData.getString("reference_id");
        resp["fundId"] = jrData.getString("fund_account_id");
        resp["amount"] = jrData.getInt("amount");
        resp["fees"] = jrData.getInt("fees");
        resp["tax"] = jrData.getInt("tax");
        CMyString strStatus = jrData.getString("status");
        if (strStatus == "queued")
            resp["status"] = 1;
        else
            resp["status"] = 0;
    }
    else
    {
        CJsonWrapper jrError;
        if (jrRoot.getReader("error", jrError))
        {
            resp["code"] = -4;
            resp["message"] = jrError.getString("description");
            return -4;
        }

        resp["code"] = 0;
        resp["message"] = "OK";
        resp["id"] = jrRoot.getString("id");
        resp["withdrawId"] = jrRoot.getString("reference_id");
        resp["fundId"] = jrRoot.getString("fund_account_id");
        resp["amount"] = jrRoot.getInt("amount");
        resp["fees"] = jrRoot.getInt("fees");
        resp["tax"] = jrRoot.getInt("tax");
        CMyString strStatus = jrRoot.getString("status");
        if (strStatus == "queued" || strStatus == "processing")
            resp["status"] = 1;
        else
            resp["status"] = 0;
    }

    return 0;
}


