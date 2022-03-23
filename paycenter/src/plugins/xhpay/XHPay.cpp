/*
 * =====================================================================================
 *
 *       Filename:  XHPay.cpp
 *
 *    Description:  xhpay implement
 *
 *        Version:  1.0
 *        Created:  2020年08月08日 17时31分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "XHPay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginXHPay::CPluginXHPay()
    : CPayWrapper()
{
    //mkdir("./html/XH", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginXHPay::~CPluginXHPay()
{
}

int32_t CPluginXHPay::Init(void)
{
    CMyString strSect("main/xhpay");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    return 0;
}

int32_t CPluginXHPay::Update(void)
{
    return this->Init();
}

void CPluginXHPay::Maintance()
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
ENTRY_FUNC_DECLARE(CPluginXHPay, doPay)
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

///////// 以下为宝丽平台专用调用
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
    req_data.add("merchantId", pSdk->getAccount());
    req_data.add("tradeType", pChannel->getBankCode());
    req_data.add("merchantPayNo", strOrderNo);
    req_data.add("amt", strPrice);
    req_data.add("notifyUrl", strNotifyUrl);
    req_data.add("frontBackUrl", strCallbackUrl);
    req_data.add("goodsName", req.getParam("uin"));
    req_data.add("payIp", strIp);

    // 生成sign
    CMyString str1 = req_data.http_str_manual('&', '=', true, true); // req_data.http_str();
    //CMyString str1 = req_data.http_str();
    CMyString str2("%s&key=%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeUpper();
    req_data.add("sign", strMd5);
    CMyString strSign = req_data.http_str_manual('&', '=', true, true);
    //CMyString strSign("%s&sign=%s", str1.c_str(), strMd5.c_str());
    LogDebug("[XH]origin text[%s]", str1.c_str());
    LogDebug("[XH]key text   [%s]", str2.c_str());
    LogDebug("[XH]md5        [%s]", strMd5.c_str());
    LogDebug("[XH]signed text[%s]", strSign.c_str());

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
            CMyString strNote("XH请求支付失败，错误信息：%s", strError.c_str());
            CPayWrapper::postAlert(pGame, strNote);

            strError.Format("host [%s] unreachable, code=%d.", strUrl.c_str(), ret);
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("[XH] header [ %s ]", strHeader.c_str());
        LogWarn("[XH] resp   [ %s ]", strResp.c_str());
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
    LogDebug("XH-PayUrl:%s", strTransferUrl.c_str());
    //CMyString strNewUrl = CHttpWrapper::encodeURI(strTransferUrl);
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginXHPay, doCallback)
{
    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogInfo("XH-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("XH-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    CGame* pGame(NULL);
    CMyString strOrderNo = req.getParam("merchantPayNo");
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret<0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }

    // TODO:验签

    // 填充返回结果数据
    CMyString strPlatformId = req.getParam("payNo");
    CMyString status = req.getParam("tradeStatus");
    int32_t code(payCallbackFail);
    if (status != "1")
    {
        code = payCallbackFail;
    }
    else
    {
        code = payCallbackOK;
    }

    //CMyString strFee = req.getParam("total_fee");
    CMyString strRealFee = req.getParam("tradeAmt");
    double realFee = strRealFee.asDouble();
    int32_t nRealFee = realFee * 100; // 转换成分

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, nRealFee);

    // 回传到回调方
    safe_strcpy(szResp, cbResp, "SUCCESS");
    cbResp = strlen(szResp);
    return ret;
}

int32_t CPluginXHPay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo, CGame *pGame)
{
    CJsonWrapper reader;
    if (!reader.Init(strResp.c_str()))
    {
        CMyString strNote("XH支付返回的结果[%s]异常，请联系程序员", strResp.c_str());
        CPayWrapper::postAlert(pGame, strNote);

        // 返回串不合法
        strError.Format("resp [%s] not valid json string", strResp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1023;
    }

    CMyString status = reader.getString("code");
    if (status != "0000")
    {
        CMyString strMsg = reader.getString("message");
        CMyString strNote("XH支付调起失败，返回值:%s, 错误描述:%s", status.c_str(), strMsg.c_str());
        CPayWrapper::postAlert(pGame, strNote);

        // 支付失败
        strError.Format("code:%s, msg:%s", status.c_str(), strMsg.c_str());
        LogErr("[%s:%s:%d] resp: %s", __FILE__, __func__, __LINE__, strResp.c_str());
        return -1025;
    }

    CJsonWrapper data;
    if (!reader.getReader("data", data))
    {
        CMyString strNote("XH接口错误,请联系开发人员");
        CPayWrapper::postAlert(pGame, strNote);
        strError.Format("XHPay支付接口异常，请联系客服");
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1026;
    }

    strTransferUrl = data.getString("url");
    platformOrderNo = data.getString("payNo");

    return 0;
}
