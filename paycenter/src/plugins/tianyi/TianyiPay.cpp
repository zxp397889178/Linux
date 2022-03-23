/*
 * =====================================================================================
 *
 *       Filename:  TianyiPay.cpp
 *
 *    Description:  天逸支付API实现
 *
 *        Version:  1.0
 *        Created:  2020年07月18日 19时52分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "TianyiPay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginTianyiPay::CPluginTianyiPay()
    : CPayWrapper()
{
    //mkdir("./html/tianyi", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginTianyiPay::~CPluginTianyiPay()
{
}

int32_t CPluginTianyiPay::Init(void)
{
    CMyString strSect("main/tianyi");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    return 0;
}

int32_t CPluginTianyiPay::Update(void)
{
    return this->Init();
}

void CPluginTianyiPay::Maintance()
{
    CPayWrapper::Maintance();
}

/*
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
*/

// 以下为调用网银接口
ENTRY_FUNC_DECLARE(CPluginTianyiPay, doPay)
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
    TimeToStr(::time(NULL), szTime, sizeof(szTime), "yyyymmddHHMMSS");
    //CMyString strNonce = getNonce(20);
    //CMyString strNonce = req.getParam("uin");
    //CMyString strParam("%s-%s", req.getParam("uin").c_str(), strIp.c_str());
    // CMyString strRemark2("%s@%s", req.getParam("uin").c_str(), strIp.c_str());
    http::CHttpRespMaker req_data;
    req_data.add("merchantCode", pSdk->getAccount());
    req_data.add("payCode", pChannel->getBankCode());
    req_data.add("orderNumber", strOrderNo);
    req_data.add("amount", dblPrice);
    req_data.add("submitTime", szTime);
    req_data.add("submitIp", strIp);
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    CMyString strCallbackUrl = pGame->getCallbackUrl();
    req_data.add("asyncNotifyUrl", strNotifyUrl);
    req_data.add("syncRedirectUrl", strCallbackUrl);
    //req_data.add("bankCode", );
    req_data.add("remark", req.getParam("uin"));
    CMyString strNote("%s支付专用", pGame->getCode().c_str());
    req_data.add("commodityName", strNote);

    // 生成sign
    CMyString str1 = req_data.http_str_manual('&', '=', true, true); // req_data.http_str();
    //CMyString str1 = req_data.http_str();
    CMyString str2("%s%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeUpper();
    req_data.add("sign", strMd5.c_str());
    CMyString strSign = req_data.http_str_manual('&', '=', true, true);
    //CMyString strSign("%s&sign=%s", str1.c_str(), strMd5.c_str());
    LogDebug("[TianYi]origin text[%s]", str1.c_str());
    LogDebug("[TianYi]key text   [%s]", str2.c_str());
    LogDebug("[TianYi]md5        [%s]", strMd5.c_str());
    LogDebug("[TianYi]signed text[%s]", strSign.c_str());

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
        Json::Value &keydata = pChannel->getKeyData();
        CMyString strUrl("%s/%s", 
                pSdk->getPayUrl().c_str(), 
                keydata["payURI"].asString().c_str());
        ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            strError.Format("host [%s] unreachable, code=%d.", strUrl.c_str(), ret);
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("[TianYi] header [ %s ]", strHeader.c_str());
        LogWarn("[TianYi] resp   [ %s ]", strResp.c_str());
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
    // 记录支付对应的商户KEY
    CPayWrapper::on_save_order_channelId(strOrderNo, req);

    /// 返回给调用端
    LogDebug("Tianyi-PayUrl:%s", strTransferUrl.c_str());
    //CMyString strNewUrl = CHttpWrapper::encodeURI(strTransferUrl);
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

// 以下为调用扫码接口
ENTRY_FUNC_DECLARE(CPluginTianyiPay, doScanPay)
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
    TimeToStr(::time(NULL), szTime, sizeof(szTime), "yyyymmddHHMMSS");
    //CMyString strNonce = getNonce(20);
    //CMyString strNonce = req.getParam("uin");
    //CMyString strParam("%s-%s", req.getParam("uin").c_str(), strIp.c_str());
    // CMyString strRemark2("%s@%s", req.getParam("uin").c_str(), strIp.c_str());

    CMyString strUin = req.getParam("uin");
    http::CHttpRespMaker req_data;
    req_data.add("merchantCode", pSdk->getAccount());
    req_data.add("payCode", pChannel->getBankCode());
    req_data.add("orderNumber", strOrderNo);
    req_data.add("amount", dblPrice);
    req_data.add("submitTime", szTime);
    req_data.add("submitIp", strIp);
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    CMyString strCallbackUrl = pGame->getCallbackUrl();
    req_data.add("asyncNotifyUrl", strNotifyUrl);
    req_data.add("syncRedirectUrl", strCallbackUrl);
    req_data.add("remark", strUin);
    CMyString strNote("%s支付专用", pGame->getCode().c_str());
    req_data.add("commodityName", strNote );

    // 生成sign (忽略空值)
    CMyString str1 = req_data.http_str_manual('&', '=', true, true); // req_data.http_str();
    CMyString str2("%s%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeUpper();
    req_data.add("sign", strMd5.c_str());
    CMyString strSign = req_data.http_str_manual('&', '=', true, true);
    //CMyString strSign("%s&sign=%s", str1.c_str(), strMd5.c_str());
    LogDebug("[TianYi]origin text[%s]", str1.c_str());
    LogDebug("[TianYi]key text   [%s]", str2.c_str());
    LogDebug("[TianYi]md5        [%s]", strMd5.c_str());
    LogDebug("[TianYi]signed text[%s]", strSign.c_str());

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
        Json::Value &keydata = pChannel->getKeyData();
        CMyString strUrl("%s/%s", 
                pSdk->getPayUrl().c_str(), 
                keydata["payURI"].asString().c_str());
        ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            CMyString strMsg;
            m_http.getError(ret, strMsg);
            CMyString strNote("用户(uin=%s)调起天逸支付失败，错误码:%d, 错误说明:%s", strUin.c_str(), ret, strMsg.c_str() );
            CPayWrapper::postAlert(pGame, strNote);

            strError.Format("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("[TianYi] header [ %s ]", strHeader.c_str());
        LogWarn("[TianYi] resp   [ %s ]", strResp.c_str());
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
    // 记录支付对应的商户KEY
    CPayWrapper::on_save_order_channelId(strOrderNo, req);

    /// 返回给调用端
    LogDebug("Tianyi-PayUrl:%s", strTransferUrl.c_str());
    //CMyString strNewUrl = CHttpWrapper::encodeURI(strTransferUrl);
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());
            

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginTianyiPay, doCallback)
{
    safe_strcpy(szResp, cbResp, "success");
    cbResp = strlen(szResp);

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogInfo("TianYi-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("TianYi-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    
    // 校验回调IP
    CMyString strOrderNo = req.getParam("orderNumber");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }
    // TODO: 验签

    // 填充返回结果数据
    CMyString strPlatformId = req.getParam("trxNo");
    CMyString status = req.getParam("payStatus");
    int32_t code(payCallbackFail);
    if (status != "orderPaid")
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
    return ret;
}

int32_t CPluginTianyiPay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo, CSdkChannel *pChannel, int32_t payMoney)
{
    CGame *pGame = pChannel->getGame();
    CJsonWrapper reader;
    if (!reader.Init(strResp.c_str()))
    {
        CMyString strNote("天逸支付调起结果异常, %s", strResp.c_str());
        CPayWrapper::postAlert(pGame, strNote);

        strError.Format("天逸支付调用异常,请联系客服");
        // 返回串不合法
        LogErr("[%s:%s:%d]Tianyi resp [%s] not valid json string", __FILE__, __func__, __LINE__, strResp.c_str());
        return -1023;
    }

    CMyString status = reader.getString("returnCode");
    if (status != "0")
    {
        CMyString strMsg = reader.getString("message");
        // STEP1: 发给IM
        CMyString strNote("天逸支付调起失败,错误描述:%s", strMsg.c_str());
        CPayWrapper::postAlert(pChannel->getGame(), strNote);

        // STEP2: 发给前端
        CMyString strClientMsg;
        int32_t deal = CPayWrapper::dealRespError(pChannel, payMoney,strMsg, strClientMsg);
        strError.Format("[天逸支付]%s", strClientMsg.c_str());

        // STEP4: 记录日志
        LogErr("[%s:%s:%d]Tianyi-Pay call failure, code:%s, error:%s", __FILE__, __func__, __LINE__, status.c_str(), strMsg.c_str());
        return -1025;
    }

    strTransferUrl = reader.getString("content");
    platformOrderNo = "N/A";

    return 0;
}

