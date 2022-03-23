/*
 * =====================================================================================
 *
 *       Filename:  BotaiPay.cpp
 *
 *    Description:  博泰支付实现
 *
 *        Version:  1.0
 *        Created:  07/04/2020 11:01:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BotaiPay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginBotaiPay::CPluginBotaiPay()
    : CPayWrapper()
{
    //mkdir("./html/botai", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginBotaiPay::~CPluginBotaiPay()
{
}

int32_t CPluginBotaiPay::Init(void)
{
    CMyString strSect("main/botai");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }

    CXmlFile &xml = CONFIG.getXml();
    try
    {
        CMyString sect2("%s/verify", strSect.c_str());
        m_strVerifyUrl = xml.GetString(sect2.c_str(), "url");  
    }
    catch(CException &e)
    {
        LogErr("[%s:%s:%d]config corrupted,%s", __FILE__, __func__, __LINE__, e.what());
        return -1;
    }
    catch(...)
    {
        LogErr("botai config corrupted, miss verify sect");
        return -3;
    }
    
    return 0;
}

int32_t CPluginBotaiPay::Update(void)
{
    return this->Init();
}

void CPluginBotaiPay::Maintance()
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

ENTRY_FUNC_DECLARE(CPluginBotaiPay, doPay)
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
    CGame *pGame= pChannel->getGame();

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
    req_data.add("mch_id", pSdk->getAccount());
    req_data.add("child_type", "H5");
    req_data.add("out_trade_no", strOrderNo);
    //req_data.add("pay_type", "PERSONAL_RED_PACK");
    req_data.add("pay_type", pChannel->getBankCode());
    req_data.add("total_fee", strPrice);
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    CMyString strUrl2 = CHttpWrapper::encodeURIComponentEx(strNotifyUrl, true);
    req_data.add("notify_url", strUrl2);
    req_data.add("timestamp", (uint32_t)time(NULL));
    req_data.add("mch_secret", pSdk->getApiKey().c_str());

    // 生成sign
    CMyString str1 = req_data.http_str(true);
    CMyString strMd5 = CMD5Wrapper::encode(str1.c_str());
    strMd5.MakeUpper();
    //LogDebug("[Botai]origin text[%s]", str1.c_str());
    LogDebug("[Botai]encode(%s) => [%s]", strNotifyUrl.c_str(), strUrl2.c_str());
    LogDebug("[Botai]with key   [%s]", str1.c_str());
    LogDebug("[Botai]md5(key)   [%s]", strMd5.c_str());
    req_data.erase("mch_secret");
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
        CMyString strApiUrl = pSdk->getPayUrl();
        ret = m_http.HttpPostWithHeaders(strApiUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            strError.Format("host [%s] unreachable.", strApiUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("Botai header [ %s ]", strHeader.c_str());
        LogWarn("Botai resp   [ %s ]", strResp.c_str());
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
    // 记录支付对应的商户KEY
    CPayWrapper::on_save_order_channelId(strOrderNo, req);

    // 返回给调用端
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginBotaiPay, doCallback)
{
    safe_strcpy(szResp, cbResp, "SUCCESS");
    cbResp = strlen(szResp);

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogInfo("Botai-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("Botai-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());

    // 校验回调IP
    CMyString strOrderNo = req.getParam("out_trade_no");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }
    // TODO: 验签

    // 填充返回结果数据
    CMyString strPlatformId = req.getParam("order_no");
    // 查询订单是否成功
    int32_t val = this->VerifyOrder(strOrderNo, strPlatformId);
    int32_t code = (val<0) ? payCallbackFail : payCallbackOK;

    double totalFee(0.00);
    req.getParam("total_fee", totalFee);
    int32_t nRealFee = totalFee * 100;
    //req.getParam("amount", nRealFee);

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, nRealFee);
    return ret;
}

int32_t CPluginBotaiPay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo)
{
    CJsonWrapper reader;
    if (!reader.Init(strResp.c_str()))
    {
        // 返回串不合法
        strError.Format("resp [%s] not valid json string", strResp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1023;
    }

    CMyString status = reader.getString("status");
    int32_t code = reader.getInt("code");
    CMyString strNote = reader.getString("msg");
    if (status != "true" && code!=100)
    {
        // 支付失败
        strError.Format("resp[%s], status:%s,code:%d, error:%s", 
                strResp.c_str(), status.c_str(), code, strNote.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1025;
    }
    // TODO: 验签

    CJsonWrapper data;
    if (!reader.getReader("data", data))
    {
        LogErr("[%s:%s:%d]resp json string not found data field.", 
                __FILE__, __func__, __LINE__);
        return -1026;
    }

    //CMyString strSign = reader.getString("sign");
    platformOrderNo = data.getString("order_id");
    strTransferUrl = data.getString("url");

    return 0;
}

int32_t CPluginBotaiPay::VerifyOrder(const CMyString strOrderNo, const CMyString strPlatformId)
{
    CMyString strUrl;
    CSdkChannel *pChannel = CPayWrapper::on_get_order_sdkChannel(strOrderNo);
    if (NULL != pChannel)
    {
        CSdk *pSdk = CPayWrapper::getSdk(pChannel->getGameCode());
        if (NULL != pSdk)
        {
            strUrl = pSdk->getPayCheckUrl();
        }
    }
    if (strUrl.empty())
    {
        strUrl = m_strVerifyUrl;
    }

    // 发起HTTP请求
    CMyString strError;
    int32_t code;
    CMyString strMchOrderNo;
    CMyString status;
    int32_t result(-1);
    do {
        strUrl.Append("/%s", strPlatformId.c_str());
        LogDebug("[Botai-verify]request verify order, url[%s]", strUrl.c_str());
        CMyString strContentType("Content-Type: application/json");
        std::vector<std::string> header_list;
        CMyString strHeader;
        CMyString strResp;
        int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, "");
        if (ret < 0)
        {
            strError.Format("host [%s] unreachable.", strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("Botai-verify header [ %s ]", strHeader.c_str());
        LogWarn("Botai-verify resp   [ %s ]", strResp.c_str());
        //http::CHttpRespone http_resp;
        //http_resp.read((void*)strHeader.c_str(), strHeader.size());
        //http_resp.read((void*)strResp.c_str(), strResp.size());
        //result = this->dealVerifyJsonResp(strResp);        
        CJsonWrapper reader;
        if (!reader.Init(strResp.c_str()))
        {
            // 返回串不合法
            CMyString strError("resp [%s] not valid json string", strResp.c_str());
            LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1023;
            break;
        }
        status = reader.getString("status");
        code = reader.getInt("code");
        strMchOrderNo = reader.getString("mch_order_no");
        if (status=="true" && (code==2 || code==4))
        {
            result = 0;
        }
        else
        {
            result = -1025;
        }
    }while(0);

    if (result < 0)
    {
        return -1;
    }

    CMyString strMd5 = CMD5Wrapper::encode(strOrderNo.c_str());
    strMd5.MakeUpper();
    if (strMd5 != strMchOrderNo)
    {
        LogErr("[Botai-verify]order not match, md5(order)=%s, but resp is [%s]",
                strMd5.c_str(), strMchOrderNo.c_str());
        return -3;
    }

    return 0;
}




