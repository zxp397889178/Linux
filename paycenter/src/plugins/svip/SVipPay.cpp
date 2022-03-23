/*
 * =====================================================================================
 *
 *       Filename:  SVipPay.cpp
 *
 *    Description:  SVIP支付接口实现
 *
 *        Version:  1.0
 *        Created:  2020年06月30日 15时01分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "SVipPay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginSVipPay::CPluginSVipPay()
    : CPayWrapper()
{
    mkdir("./html/svip", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginSVipPay::~CPluginSVipPay()
{
}

int32_t CPluginSVipPay::Init(void)
{
    CMyString strSect("main/svip");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }

    // 读取额外的数据
    CXmlFile &xml = CONFIG.getXml();
    try
    {
        m_strTransfer = xml.GetString("main/svip/transfer", "url");
    }
    catch(CException &e)
    {
        LogFatal("[%s:%s:%d]get transfer url failure, err:%s", __FILE__, __func__, __LINE__, e.what());
        return -1;
    }
    catch(...)
    {
        LogFatal("[%s:%s:%d]get transfer url failure", __FILE__, __func__, __LINE__);
        return -3;
    }
    return 0;
}

int32_t CPluginSVipPay::Update(void)
{
    return this->Init();
}

void CPluginSVipPay::Maintance()
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

ENTRY_FUNC_DECLARE(CPluginSVipPay, doPay)
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
    //CMyString strPrice("%.2lf", dblPrice);
    CMyString strPrice = double_to_str(dblPrice, 2);
    //char szTime[64] = "";
    //TimeToStr(::time(NULL), szTime, sizeof(szTime), "yyyymmddHHMMSS");
    //CMyString strNonce = getNonce(20);
    //CMyString strNonce = req.getParam("uin");
    //CMyString strParam("%s-%s", req.getParam("uin").c_str(), strIp.c_str());
    CMyString strRemark2("%s@%s", req.getParam("uin").c_str(), strIp.c_str());
    http::CHttpRespMaker req_data;
    req_data.add("user_account", pSdk->getAccount());
    req_data.add("payment_type", pChannel->getBankCode());
    req_data.add("out_trade_no", strOrderNo);
    req_data.add("total_fee", strPrice);
    char szTime[128] = "";
    TimeToStr(time(NULL), szTime, sizeof(szTime));
    req_data.add("trade_time", szTime);
    req_data.add("body", strRemark2);
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    CMyString strCallbackUrl = pGame->getCallbackUrl();
    req_data.add("notify_url", strNotifyUrl);
    req_data.add("return_url", strCallbackUrl);

    // 生成sign
    CMyString str1 = req_data.http_str();
    CMyString str2("%s&key=%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeUpper();
    req_data.add("sign", strMd5.c_str());
    CMyString strSign = req_data.http_str();
    //CMyString strSign("%s&sign=%s", str1.c_str(), strMd5.c_str());
    LogDebug("[SVIP]origin text[%s]", str1.c_str());
    LogDebug("[SVIP]key text   [%s]", str2.c_str());
    LogDebug("[SVIP]md5        [%s]", strMd5.c_str());
    LogDebug("[SVIP]signed text[%s]", strSign.c_str());

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
        LogWarn("[SVIP] header [ %s ]", strHeader.c_str());
        LogWarn("[SVIP] resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());
        // result = dealJsonResp(strResp, strTransferUrl, strError, platformOrderNo);
        result = dealHtmlResp(strResp, strTransferUrl, strError, strUrl);
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
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginSVipPay, doCallback)
{
    safe_strcpy(szResp, cbResp, "SUCCESS");
    cbResp = strlen(szResp);

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogInfo("SVip-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("SVip-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    
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
    CMyString strPlatformId = req.getParam("trade_no");
    CMyString status = req.getParam("status");
    int32_t code(payCallbackFail);
    if (status != "SUCCESS")
    {
        code = payCallbackFail;
    }
    else
    {
        code = payCallbackOK;
    }

    //CMyString strFee = req.getParam("total_fee");
    CMyString strRealFee = req.getParam("total_fee");
    double realFee = strRealFee.asDouble();
    int32_t nRealFee = realFee * 100; // 转换成分

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, nRealFee);
    return ret;
}

ENTRY_FUNC_DECLARE(CPluginSVipPay, doGetHtml)
{
    CMyString strFile = req.getParam("filename");
    CMyString strPath("./html/svip/%s", strFile.c_str());

    ext_headers["Content-Type"] = "application/json";
    FILE *fp = fopen(strPath.c_str(), "rb");
    if (fp == NULL)
    {
        safe_strcpy(szResp, cbResp, "{\"code\":\"404\", \"message\":\"file not found\"}");
        cbResp = strlen(szResp);
        return 404;
    }
    fseek(fp, 0, SEEK_END);
    int32_t size1 = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int32_t size2 = std::min(size1, (int32_t)cbResp);
    cbResp = fread((void*)szResp, 1, size2, fp);
    fclose(fp);
    
    ext_headers["Content-Type"] = "text/html";
    return 200;
}

/*
int32_t CPluginSVipPay::OnVerifyOrder(const CMyString strOrderNo)
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

int32_t CPluginSVipPay::dealVerifyJsonResp(CMyString &strResp)
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

int32_t CPluginSVipPay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo)
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
    //int32_t code = reader.getInt("order_status");
    CMyString strNote = reader.getString("msg");
    if (status != "SUCCESS")
    {
        // 支付失败
        strError.Format("resp[%s], status:%s, error:%s", strResp.c_str(), status.c_str(), strNote.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1025;
    }
    strTransferUrl = reader.getString("url");
    platformOrderNo = reader.getString("trade_no");

    return 0;
}

int32_t CPluginSVipPay::dealHtmlResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &strNewUrl)
{
    // 转储成文件，并发送链接到前端
    CMyString strFile("temp_%lu.html", Now());
    CMyString strPath("./html/svip/%s", strFile.c_str());
    FILE *fp = fopen(strPath.c_str(), "wb");
    if (NULL != fp)
    {
        // 对相对地址进行转换
        
        this->dealRelativePath(strResp, strNewUrl);

        fwrite(strResp.c_str(), 1, strResp.size(), fp);
        fflush(fp);
        fclose(fp);
        strTransferUrl.Format("%s?filename=%s", m_strTransfer.c_str(), strFile.c_str());

        // TODO: 存成文件到数据库
        strResp = strPath;
        return 0;
    }

    // 出错，返回404
    strError.Format("save temp file [%s] to disk failure.", strFile.c_str());
    LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
    return -1035;
}

void CPluginSVipPay::dealRelativePath(CMyString &strData, const CMyString &strUrl)
{
    CMyString strHost;
    char type[128] = "";
    char host[256] = "";
    if (sscanf(strUrl.c_str(), "%[^:]://%[^/]", type, host)==2)
    {
        strHost.Format("%s://%s", type, host);
    }
    else
    {
        return;
    }

    LogDebug("add url director [%s]", strHost.c_str());
    do {
       size_t pos = strData.find("\"/api");
       if (pos == std::string::npos)
       {
           // 已结束
           break;
       }
       strData.insert(pos+1, strHost.c_str());
    }while(true);

    do {
        size_t pos = strData.find("\"/static");
        if (pos == std::string::npos)
        {
            break;
        }
        strData.insert(pos+1, strHost.c_str());
    }while(true);

    CMyString strHost2("%s/", strHost.c_str());
    do {
        size_t pos = strData.find("\"pay.php");
        if (pos == std::string::npos)
        {
            break;
        }
        strData.insert(pos+1, strHost2.c_str());
    }while(true);
}



