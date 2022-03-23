/*
 * =====================================================================================
 *
 *       Filename:  Pay58.cpp
 *
 *    Description:  58paypay实现，用于TeenPatti的充值合作
 *
 *        Version:  1.0
 *        Created:  2020年08月06日 16时10分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Pay58.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginPay58::CPluginPay58()
    : CPayWrapper()
{
    //mkdir("./html/v8", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginPay58::~CPluginPay58()
{
}

int32_t CPluginPay58::Init(void)
{
    CMyString strSect("main/pay58");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    return 0;
}

int32_t CPluginPay58::Update(void)
{
    return this->Init();
}

void CPluginPay58::Maintance()
{
    CPayWrapper::Maintance();
}

static CMyString getNonce(int32_t len)
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

// 以下为调用网银接口
ENTRY_FUNC_DECLARE(CPluginPay58, doPay)
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
    CSdk *pSdk = pChannel->getSdk();
    CGame *pGame = pChannel->getGame();
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    CMyString strCallbackUrl = pGame->getCallbackUrl();

    int32_t payMoney(0);
    CMyString strProductId;
    CMyString strRemark;
    req.getParam("productId", strProductId);
    req.getParam("Remark", strRemark);
    req.getParam("amount", payMoney);

    // 生成发送到支付平台的数据
    double dblPrice = payMoney/100.0;
    //CMyString strPrice("%.2lf", dblPrice);
    CMyString strPrice = double_to_str(dblPrice, 2);
    char szTime[64] = "";
    TimeToStr(::time(NULL), szTime, sizeof(szTime), "yyyy-mm-dd HH:MM:SS");
    CMyString strNonce = getNonce(32);
    //CMyString strNonce = req.getParam("uin");
    //CMyString strParam("%s-%s", req.getParam("uin").c_str(), strIp.c_str());
    // CMyString strRemark2("%s@%s", req.getParam("uin").c_str(), strIp.c_str());
    http::CHttpRespMaker req_data;
    req_data.add("pay_memberid", pSdk->getAccount());
    req_data.add("pay_orderid", strOrderNo);
    req_data.add("pay_applydate", szTime);
    req_data.add("pay_bankcode", pChannel->getBankCode());
    req_data.add("pay_notifyurl", strNotifyUrl);
    req_data.add("pay_callbackurl", strCallbackUrl);
    req_data.add("pay_amount", dblPrice);
    req_data.add("pay_name", "TeenPattiGameUser");
    req_data.add("pay_mobile", "13800000001");
    req_data.add("pay_account", req.getParam("uin"));
    req_data.add("pay_email", strIp);

    // 生成sign
    CMyString str1 = req_data.http_str_manual('&', '=', true, true); // req_data.http_str();
    //CMyString str1 = req_data.http_str();
    CMyString str2("%s&key=%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeLower();
    req_data.add("pay_md5sign", strMd5.c_str());
    CMyString strSign = req_data.http_str_manual('&', '=', true, true);
    //CMyString strSign("%s&sign=%s", str1.c_str(), strMd5.c_str());
    LogDebug("[PAY58]origin text[%s]", str1.c_str());
    LogDebug("[PAY58]key text   [%s]", str2.c_str());
    LogDebug("[PAY58]md5        [%s]", strMd5.c_str());
    LogDebug("[PAY58]signed text[%s]", strSign.c_str());

    // 发送到第三方平台
    CMyString strContentType("Content-Type: application/x-www-form-urlencoded");
    //CMyString strContentType("Content-Type: application/json");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());

    CMyString strHeader;
    CMyString strResp;
    CMyString strError;
    int32_t result(-1);
    CMyString platformOrderNo("N/A");
    CMyString strTransferUrl;
    do {
        CMyString strUrl("%s", pSdk->getPayUrl().c_str());
        ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            CMyString strError;
            m_http.getError(ret, strError);
            CMyString strNote("PAY58请求支付失败，错误信息：%s", strError.c_str());
            this->postAlter(pGame, strNote);

            strError.Format("host [%s] unreachable, code=%d, error:%s", strUrl.c_str(), ret, strError.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("[PAY58] header [ %s ]", strHeader.c_str());
        LogWarn("[PAY58] resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealJsonResp(strResp, strTransferUrl, strError, platformOrderNo, pGame);
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
    // 记录支付对应的商户KEY
    CPayWrapper::on_save_order_channelId(strOrderNo, req);

    /// 返回给调用端
    LogDebug("PAY58-PayUrl:%s", strTransferUrl.c_str());
    //CMyString strNewUrl = CHttpWrapper::encodeURI(strTransferUrl);
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginPay58, doCallback)
{
    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogInfo("PAY58-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("PAY58-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    
    // 校验回调IP
    CMyString strOrderNo = req.getParam("orderid");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }

    // TODO:验签

    // 填充返回结果数据
    CMyString strPlatformId = "N/A";
    //CMyString strPlatformId = req.getParam("transaction_id");
    CMyString status = req.getParam("returncode");
    int32_t code(payCallbackFail);
    if (status != "00")
    {
        code = payCallbackFail;
    }
    else
    {
        code = payCallbackOK;
    }

    //CMyString strFee = req.getParam("total_fee");
    CMyString strRealFee = req.getParam("amount");
    double realFee = strRealFee.asDouble();
    int32_t nRealFee = realFee * 100; // 转换成分

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, nRealFee);

    // 回传到回调方
    safe_strcpy(szResp, cbResp, "OK");
    cbResp = strlen(szResp);
    return ret;
}

int32_t CPluginPay58::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo, CGame *pGame)
{
    CJsonWrapper reader;
    if (!reader.Init(strResp.c_str()))
    {
        CMyString strNote("PAY58支付返回的结果[%s]异常，请联系程序员", strResp.c_str());
        this->postAlter(pGame, strNote);

        // 返回串不合法
        strError.Format("resp [%s] not valid json string", strResp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1023;
    }

    CMyString status = reader.getString("status");
    if (status != "success")
    {
        CMyString strMsg = reader.getString("msg");
        CMyString strNote("PAY58支付调起失败，返回:%s", strMsg.c_str());
        this->postAlter(pGame, strNote);

        // 支付失败
        strError.Format("resp[%s], code:%s, msg:%s", strResp.c_str(), status.c_str(), strMsg.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1025;
    }

    strTransferUrl = "";
    platformOrderNo = "N/A";

    //strTransferUrl = reader.getString("pay_url");
    //platformOrderNo = reader.getString("orderid");

    return 0;
}

void CPluginPay58::postAlter(CGame *pGame, const CMyString &strText)
{
    CJsonWrapper alter;
    pGame->getAlter(alter);
    int32_t alterId = alter.getInt("alterId");
    if (alterId <= 0)
    {
        return;
    }
    
    CMyString alterUrl = alter.getString("url");
    
    http::CHttpClient net;
    CMyString strData("{\"chatId\":%d,\"text\":\"%s\"}", alterId, strText.c_str());
    http::VEC_STRING headers;
    headers.push_back("Content-Type: application/json");
    CMyString strHeader;
    CMyString strResp;
    int32_t ret = net.HttpPostWithHeaders(alterUrl.c_str(), strResp, strHeader, headers, strData.c_str());
    LogDebug("alter resp[%s]", strResp.c_str());
    LogDebug("alter header[%s]", strHeader.c_str());
    LogDebug("alter result:%d", ret);
}

