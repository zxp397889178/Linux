/*
 * =====================================================================================
 *
 *       Filename:  SananPay.cpp
 *
 *    Description:  三安支付
 *
 *        Version:  1.0
 *        Created:  2020年07月03日 14时19分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "SananPay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginSananPay::CPluginSananPay()
    : CPayWrapper()
{
    //mkdir("./html/sanan", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginSananPay::~CPluginSananPay()
{
}

int32_t CPluginSananPay::Init(void)
{
    CMyString strSect("main/sanan");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    
    return 0;
}

int32_t CPluginSananPay::Update(void)
{
    return this->Init();
}

void CPluginSananPay::Maintance()
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

ENTRY_FUNC_DECLARE(CPluginSananPay, doPay)
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
    CGame *pGame = pChannel->getGame();
    CSdk *pSdk = pChannel->getSdk();

///////// 以下为宝丽平台专用调用
    int32_t payMoney(0);
    CMyString strProductId;
    CMyString strRemark;
    req.getParam("productId", strProductId);
    req.getParam("Remark", strRemark);
    req.getParam("amount", payMoney);

    // 生成发送到支付平台的数据
    double dblPrice = payMoney/100.0;
    CMyString strPrice("%.2lf", dblPrice);
    char szTime[64] = "";
    TimeToStr(::time(NULL), szTime, sizeof(szTime), "yyyymmddHHMMSS");
    //CMyString strNonce = getNonce(20);
    CMyString strUin = req.getParam("uin");
    CMyString strParam("%s-%s", req.getParam("uin").c_str(), strIp.c_str());
    http::CHttpRespMaker req_data;
    // req_data.add("method", "bill.trade.nobank");
    req_data.add("amount", strPrice);
    req_data.add("pay_way", pChannel->getBankCode());
    req_data.add("trade_order_sn", strOrderNo);
    req_data.add("remark", strUin);
    req_data.add("visit_ip", strIp);
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    req_data.add("notify_url", strNotifyUrl);
    // req_data.add("returnUrl", m_callbackURL);

    // 生成sign
    CMyString str1 = req_data.http_str(true);
    CMyString str2("%s&encrypt_key=%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeLower();
    LogDebug("[Sanan]origin text[%s]", str1.c_str());
    LogDebug("[Sanan]with key   [%s]", str2.c_str());
    LogDebug("[Sanan]md5(key)   [%s]", strMd5.c_str());
    req_data.add("sign", strMd5);

    CMyString strSign = req_data.http_str(true);
    LogDebug("signed text[%s]", strSign.c_str());

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
        CMyString strUrl = pSdk->getPayUrl();
        ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            strError.Format("host [%s] unreachable.", strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("Sanan header [ %s ]", strHeader.c_str());
        LogWarn("Sanan resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealJsonResp(strResp, strTransferUrl, strError, platformOrderNo);
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

    // 返回给调用端
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginSananPay, doCallback)
{
    safe_strcpy(szResp, cbResp, "SUCCESS");
    cbResp = strlen(szResp);

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogInfo("Sanan-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("Sanan-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    
    // 校验回调IP
    CMyString strOrderNo = req.getParam("order_sn");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }
    // TODO: 验签

    // 填充返回结果数据
    CMyString strCode = req.getParam("is_success");
    //CMyString strStatus = req.getParam("status");
    CMyString strPlatformId = req.getParam("trade_order_sn");
    int32_t code(payCallbackFail);
    if (strCode=="success")
    {
        code = payCallbackOK;
    }
    else
    {
        code = payCallbackFail;
    }

    //double totalFee(0.00);
    //req.getParam("amount", totalFee);
    double realFee(0.00);  // 转换成分
    req.getParam("amount", realFee);

    int32_t nRealFee = realFee*100;
    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, nRealFee);
    return ret;
}

int32_t CPluginSananPay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo)
{
    CJsonWrapper reader;
    if (!reader.Init(strResp.c_str()))
    {
        // 返回串不合法
        strError.Format("resp [%s] not valid json string", strResp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1023;
    }

    int32_t status = reader.getInt("status");
    CMyString strNote = reader.getString("info");
    if (status >2 )
    {
        // 支付失败
        strError.Format("resp[%s], status:%d, error:%s", strResp.c_str(), status, strNote.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1025;
    }
    // TODO: 验签

    //CMyString strSign = reader.getString("sign");
    //platformOrderNo = reader.getString("orderId");
    platformOrderNo = reader.getString("trade_order_sn");
    strTransferUrl = reader.getString("link");
    CMyString strInfo = reader.getString("info");

    return 0;
}






