/*
 * =====================================================================================
 *
 *       Filename:  baoli_pay.cpp
 *
 *    Description:  宝丽支付SDK
 *
 *        Version:  1.0
 *        Created:  2020年02月02日 15时17分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "baoli_pay.h"
#include "Security.h"
#include "Config.h"
#include "IClient.h"
#include "pay_error.h"

CPluginBaoliPay::CPluginBaoliPay()
    : CPayWrapper()
{
    mkdir("./html/baoli/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginBaoliPay::~CPluginBaoliPay()
{

}

int32_t CPluginBaoliPay::Init(void)
{
    CMyString strSect("main/baoli");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    
    // 读取额外的数据
    CXmlFile &xml = CONFIG.getXml();
    try
    {
        m_strTransfer = xml.GetString("main/baoli/transfer", "url");
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

int32_t CPluginBaoliPay::Update(void)
{
    return this->Init();
}

void CPluginBaoliPay::Maintance()
{
    CPayWrapper::Maintance();
}

ENTRY_FUNC_DECLARE(CPluginBaoliPay, doBaoliPay)
{
    int32_t nOrderId(0);
    CMyString strOrderNo;
    CSdkChannel *pChannel(NULL);
    int32_t ret = CPayWrapper::_before_send_request(req, pChannel, nOrderId, strOrderNo, szResp, cbResp);
    if (ret < 0)
    {
        return ret; 
    }
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
    http::CHttpRespMaker req_data;
    req_data.add("pay_memberid", pSdk->getAccount());
    req_data.add("pay_orderid", strOrderNo);
    time_t tNow = time(0);
    char szTime[64] = "";
    TimeToStr(tNow, szTime, sizeof(szTime));
    req_data.add("pay_applydate", szTime);
    req_data.add("pay_bankcode", pChannel->getBankCode());
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    CMyString strCallbackUrl = pGame->getCallbackUrl();
    req_data.add("pay_notifyurl", strNotifyUrl);      // 充值成功失败的通知接口
    req_data.add("pay_callbackurl", strCallbackUrl);  // 通知游服处理的接口
    req_data.add("pay_amount", CMyString("%.2lf", payMoney/100.0));  // 转成元
    // 生成sign
    CMyString str1 = req_data.http_str();
    CMyString str2("%s&key=%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeUpper();
    LogDebug("origin text[%s]", str1.c_str());
    LogDebug("with key[%s]", str2.c_str());
    LogDebug("md5(key) [%s]", strMd5.c_str());

    req_data.add("pay_productname", strProductId);
    req_data.add("pay_md5sign", strMd5.c_str());
    CMyString strSign = req_data.http_str();
    //str1 = req_data.http_str();  // 要传入，但不参与MD5校验
    //CMyString strSign("%s&sign=%s", str1.c_str(), strMd5.c_str());
    //LogDebug("new str1 [%s]", str1.c_str());
    LogDebug("sign str [%s]", strSign.c_str());

    // 发送到第三方平台
    CMyString strContentType("Content-Type: application/x-www-form-urlencoded");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());

    CMyString strHeader;
    CMyString strResp;
    CMyString strError;
    CMyString strTransferUrl;
    int32_t result(0);
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
        strTransferUrl = strResp;

        LogWarn("header [ %s ]", strHeader.c_str());
        LogWarn("resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());
        CMyString strContType = http_resp.getHeader("Content-Type");

        if (strContType.find("application/json") != std::string::npos)
        {
            result = dealJsonResp(strResp, strError);
        }
        else if (strContType.find("text/html") != std::string::npos)
        {
            int32_t code = http_resp.getStatusCode();
            // 转换成 text / html
            // 存成 ./html 文件，
            if (code == 302) // 需要跳转
            {
                // 重定向，则访问新的 Location, 并把返回值
                result = dealRelocation(http_resp, strResp, strTransferUrl, strError, strUrl);
            }
            else
            {
                // TODO: 在 code=200 时，存在 strResp = 通道异常，。。。
                result = dealHtmlResp(strResp, strTransferUrl, strError);
            }
        }
        else
        {
            // 其他格式，暂不解析
            LogErr("[%s:%s:%d]Content-Type: %s invalid, resp[%s]", __FILE__, __func__, __LINE__, strContType.c_str(), strResp.c_str());
            strError.Format("invalid Content-Type");
            result = -1015;
        }
    }while(0);

    time_t t1 = time(NULL);
    char szReqTime[64] = "";
    CMyString strOrder3rd;
    TimeToStr(t1, szReqTime, sizeof(szReqTime));
    if (result < 0)
    {
        // 记录数据库，失败
        CPayWrapper::_after_send_request(nOrderId, payReqFail, strSign, szReqTime, strHeader, strResp, strOrder3rd);
        this->OnPayFailure(result, strError.c_str(), szResp, cbResp);
        // TODO: 订单数据让其超时
        return result;
    }
    else
    {
        CPayWrapper::_after_send_request(nOrderId, payReqSucceed, strSign, szReqTime, strHeader, strResp, strOrder3rd);
    }

    // 记录支付的主键
    CPayWrapper::on_save_orderId(strOrderNo, nOrderId);

    // 返回给调用端
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginBaoliPay, doBaoliCallback)
{
    safe_strcpy(szResp, cbResp, "OK");
    cbResp = 2;

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = req.getHeader("X-Real-IP");
    LogInfo("baoli-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("baoli-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());

    // 校验回调IP
    CMyString strOrderNo = req.getParam("orderid");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }
    // TODO: 验签

    // 填充返回结果数据
    CMyString strPlatformId = req.getParam("transaction_id");
    int32_t code(0);
    CMyString strResult = req.getParam("returncode");
    if (strResult == "00")
    {
        code = payCallbackOK;
    }
    else
    {
        code = payCallbackFail;
    }
    double dblFee(0);
    req.getParam("amount", dblFee);
    int32_t realFee = dblFee *100;  // 转换成分

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, realFee);
    return ret;
}

ENTRY_FUNC_DECLARE(CPluginBaoliPay, doBaoliHtml)
{
    CMyString strFile = req.getParam("filename");
    CMyString strPath("./html/baoli/%s", strFile.c_str());

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

int32_t CPluginBaoliPay::dealJsonResp(CMyString &strResp, CMyString &strError)
{
    CJsonWrapper reader;
    CJsonWrapper data;
    if (!reader.Init(strResp.c_str()))
    {
        // 返回串不合法
        strError.Format("resp [%s] not valid json string", strResp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1023;
    }

    CMyString status = reader.getString("status");
    status.MakeLower();
    if (status == "error")
    {
        // 支付失败
        strError.Format("resp[%s], status=%s", strResp.c_str(), status.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1025;
    }
    int32_t ret = reader.getReader("data", data);
    if (!ret)
    {
        strError.Format("resp[%s], invalid resp data.", strResp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1027;
    }

    // TODO: 做进一步解析

    return 0;
}

int32_t CPluginBaoliPay::dealRelocation(http::CHttpRespone &respIn, CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, const CMyString &strUrl)
{
    CMyString strNewUrl;
    int32_t code(200);
    do
    {
        strNewUrl = respIn.getHeader("Location");
        if (strNewUrl.empty())
        {
            // 302， 但没有重定位地址
            strError.Format("respone not has \"Location\" field.");
            LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            return -1031;
        }

        LogWarn("transfer to url [%s]", strNewUrl.c_str());
        CMyString strHeader;
        http::CHttpClient http;
        int32_t ret = http.HttpGet(strNewUrl.c_str(), strResp, strHeader);
        if (ret != 0)
        {
            // HTTP 操作失败
            strError.Format("call %s failure, ret=%d", strNewUrl.c_str(), ret);
            LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            return -1033;
        }

        LogDebug("header[%s]", strHeader.c_str());
        if (strResp.size()<256)
        {
            LogDebug("resp [%s]", strResp.c_str());
        }
        else
        {
            char szText[512] = "";
            safe_strcpy(szText, 256, strResp.c_str());
            strcat(szText, "......");
            LogDebug("resp [%s]", szText);
        }

        //TODO: 对chunked 或 multipart/form-data 模式进行解析

        respIn.clear();
        respIn.read((void*)strHeader.c_str(), strHeader.size());
        // 不解析数据，以防 chunked 或 multipart/form-data 格式导致异常
        //LogDebug("try to read %lu bytes to rHttpRespone object.", strResp.size());
        //respIn.read((void*)strResp.c_str(), strResp.size());
        code = respIn.getStatusCode();
    }while(code == 302);


    // 转储成文件，并发送链接到前端
    CMyString strFile("temp302_%lu.html", Now());
    CMyString strPath("./html/baoli/%s", strFile.c_str());
    LogWarn("save html to file [%s]", strPath.c_str());
    FILE *fp = fopen(strPath.c_str(), "wb");
    if (NULL != fp)
    {
        if (strNewUrl.empty())
        {
            strNewUrl = strUrl;
        }
        // 最后对相对地址进行转换
        this->dealRelativePath(strResp, strNewUrl);

        fwrite(strResp.c_str(), 1, strResp.size(), fp);
        fflush(fp);
        fclose(fp);
        strTransferUrl.Format("%s?filename=%s", m_strTransfer.c_str(), strFile.c_str());

        // TODO: 存成文件
        strResp = strPath;
        return 0;
    }

    // 写文件失败
    strError.Format("save temp file [%s] to disk failure.", strFile.c_str());
    LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
    return -1035;
}

int32_t CPluginBaoliPay::dealHtmlResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError)
{
    size_t pos1 = strResp.find("err_code:");
    if (pos1 != std::string::npos)
    {
        CMyString strErr1 = strResp.substr(0, pos1);
        CMyString strError = strResp.substr(pos1+9);
        LogErr("[%s:%s:%d]resp [%s] result: %s, %s", __FILE__, __func__, __LINE__, strResp.c_str(), strErr1.c_str(), strError.c_str());
        return -1041;
    }

    // 转储成文件，并发送链接到前端
    CMyString strFile("temp_%lu.html", Now());
    CMyString strPath("./html/baoli/%s", strFile.c_str());
    FILE *fp = fopen(strPath.c_str(), "wb");
    if (NULL != fp)
    {
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


void CPluginBaoliPay::dealRelativePath(CMyString &strData, const CMyString &strUrl)
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


}
