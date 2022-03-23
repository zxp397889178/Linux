/*
 * =====================================================================================
 *
 *       Filename:  YoulePay.cpp
 *
 *    Description:  有乐支付接口实现
 *
 *        Version:  1.0
 *        Created:  2020年06月30日 00时05分06秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "YoulePay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginYoulePay::CPluginYoulePay()
    : CPayWrapper()
{
    // mkdir("./html/Youle", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginYoulePay::~CPluginYoulePay()
{
}

int32_t CPluginYoulePay::Init(void)
{
    CMyString strSect("main/Youle");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }

    return 0;
}

int32_t CPluginYoulePay::Update(void)
{
    return this->Init();
}

void CPluginYoulePay::Maintance()
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

ENTRY_FUNC_DECLARE(CPluginYoulePay, doPay)
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
    //char szTime[64] = "";
    //TimeToStr(::time(NULL), szTime, sizeof(szTime), "yyyymmddHHMMSS");
    //CMyString strNonce = getNonce(20);
    //CMyString strNonce = req.getParam("uin");
    //CMyString strParam("%s-%s", req.getParam("uin").c_str(), strIp.c_str());
    CMyString strRemark2("%s@%s", req.getParam("uin").c_str(), strIp.c_str());
    http::CHttpRespMaker req_data;
    req_data.add("tenantNo", pSdk->getAccount());
    req_data.add("payType", pChannel->getBankCode());
    req_data.add("tenantOrderNo", strOrderNo);
    req_data.add("amount", strPrice);
    req_data.add("remark", strRemark2);
    CMyString notifyURL = pSdk->getPayCallbackUrl();
    CMyString callbackURL = pGame->getCallbackUrl();
    req_data.add("notifyUrl", notifyURL);
    req_data.add("pageUrl", callbackURL);

    // 生成sign
    CMyString str1 = req_data.http_str();
    CMyString str2("%s&key=%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeUpper();
    req_data.add("sign", strMd5.c_str());
    CMyString strSign = req_data.http_str();
    //CMyString strSign("%s&sign=%s", str1.c_str(), strMd5.c_str());
    LogDebug("origin text[%s]", str1.c_str());
    LogDebug("key text   [%s]", str2.c_str());
    LogDebug("md5        [%s]", strMd5.c_str());
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
        LogWarn("header [ %s ]", strHeader.c_str());
        LogWarn("resp   [ %s ]", strResp.c_str());
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
;
    /// 返回给调用端
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginYoulePay, doCallback)
{
    safe_strcpy(szResp, cbResp, "success");
    cbResp = strlen(szResp);

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogInfo("Youle-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("Youle-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    
    // 校验回调IP
    CMyString strOrderNo = req.getParam("tenantOrderNo");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }
    // TODO: 验签

    // 填充返回结果数据
    CMyString status = req.getParam("status");
    CMyString strPlatformId("N/A");
    int32_t code(payCallbackFail);
    if (status != "200")
    {
        code = payCallbackFail;
    }
    else
    {
        code = payCallbackOK;
    }

    CMyString strFee = req.getParam("oldAmount");
    CMyString strRealFee = req.getParam("amount");
    double realFee = strRealFee.asDouble();
    int32_t nRealFee = realFee * 100; // 转换成分

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, nRealFee);
    return ret;
}

/*
int32_t CPluginYoulePay::OnVerifyOrder(const CMyString strOrderNo)
{
    Keydata kd;
    if (!CPayWrapper::on_get_order_sdkChannel(strOrderNo, kd))
    {
        return -1;
    }

    // 发起HTTP请求
    http::CHttpRespMaker req_data;
    req_data.add("appid", kd.account);
    req_data.add("out_trade_no", strOrderNo);
    CMyString strExtra("&key=%s", kd.apiKey.c_str());
    CMyString strSign = req_data.sign_by_md5("sign", strExtra.c_str(), true, false);
    CMyString strContentType("Content-Type: application/x-www-form-urlencoded");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    CMyString strHeader;
    CMyString strResp;
    CMyString strError;
    int32_t result(-1);
    do {
        CMyString strUrl = kd.keydata.getString("verify_url");
        int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret < 0)
        {
            strError.Format("host [%s] unreachable.", strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("header [ %s ]", strHeader.c_str());
        LogWarn("resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());
        result = this->dealVerifyJsonResp(strResp);        
    }while(0);

    if (result == 0)
    {
        // 单据验证正确
        return 0;
    }

    return -1;
}

int32_t CPluginYoulePay::dealVerifyJsonResp(CMyString &strResp)
{
    CJsonWrapper reader;
    if (!reader.Init(strResp.c_str()))
    {
        // 返回串不合法
        CMyString strError("resp [%s] not valid json string", strResp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1023;
    }

    int32_t status = reader.getInt("status");
    if (status != 4)
    {
        return -1024;
    }

    return 0; 
}
*/

int32_t CPluginYoulePay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo)
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
    if (status != 200)
    {
        CMyString strNote = reader.getString("msg");
        // 支付失败
        strError.Format("resp[%s], status:%d, error:%s", strResp.c_str(), status, strNote.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1025;
    }
    strTransferUrl = reader.getString("url");

    return 0;
}






