/*
 * =====================================================================================
 *
 *       Filename:  chaofan_pay.cpp
 *
 *    Description:  超凡支付API
 *
 *        Version:  1.0
 *        Created:  03/13/2020 07:22:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "chaofan_pay.h"
#include "Security.h"
#include "Config.h"
#include "IClient.h"

CPluginChaofanPay::CPluginChaofanPay()
    : CPayWrapper()
{
    //mkdir("./html/chaofan", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginChaofanPay::~CPluginChaofanPay()
{

}

int32_t CPluginChaofanPay::Init(void)
{
    CMyString strSect("main/chaofan");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    
    return 0;
}

int32_t CPluginChaofanPay::Update(void)
{
    return this->Init();
}

void CPluginChaofanPay::Maintance()
{
    CPayWrapper::Maintance();
}

ENTRY_FUNC_DECLARE(CPluginChaofanPay, doPay)
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
    http::CHttpRespMaker req_data;
    req_data.add("service", pChannel->getBankCode());
    req_data.add("version", "1.0");
    req_data.add("merchantId", pSdk->getAccount());
    req_data.add("orderNo", strOrderNo);
    time_t tNow = time(0);
    char szTime[64] = "";
    char szDate[64] = "";
    TimeToStr(tNow, szDate, sizeof(szDate), "yyyymmdd");
    TimeToStr(tNow, szTime, sizeof(szTime), "HHMMSS");
    req_data.add("tradeDate", szDate);
    req_data.add("tradeTime", szTime);
    req_data.add("amount", payMoney);  // 转成分
    // req_data.add("cellPhone", );
    req_data.add("clientIp", strIp);
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    CMyString strCallbackUrl = pGame->getCallbackUrl();
    req_data.add("merchantUrl", strCallbackUrl);  // 通知游服处理的接口
    req_data.add("notifyUrl", strNotifyUrl);      // 充值成功失败的通知接口
    req_data.add("attach", "DAISHU");
    req_data.add("resultType", "json");
    req_data.add("key", pSdk->getApiKey());

    // 生成sign
    CMyString str1 = req_data.http_str(true);
    CMyString strMd5 = CMD5Wrapper::encode(str1.c_str());
    strMd5.MakeUpper();
    LogDebug("origin text[%s]", str1.c_str());
    //LogDebug("with key[%s]", str2.c_str());
    LogDebug("md5(key) [%s]", strMd5.c_str());

    req_data.add("sign", strMd5.c_str());
    req_data.erase("key");
    CMyString strSign = req_data.http_str(true);
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
    CMyString platformOrderNo("N/A");
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
        result = dealJsonResp(strResp, strTransferUrl, strError);
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

ENTRY_FUNC_DECLARE(CPluginChaofanPay, doCallback)
{
    safe_strcpy(szResp, cbResp, "success");
    cbResp = strlen(szResp);

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = this->getClientIp(req, pClient);
    LogInfo("chaofan-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("chaofan-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());

    // 校验回调IP
    CMyString strOrderNo = req.getParam("orderNo");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }
    // TODO: 验签

    // 填充返回结果数据
    CMyString strPlatformId("N/A");
    int32_t code(0);
    int32_t nResult = req.getParamInt("code");
    if (nResult == 0)
    {
        code = payCallbackOK;
    }
    else
    {
        code = payCallbackFail;
    }
    int32_t realFee(0);  // 转换成分
    req.getParam("amount", realFee);

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, realFee);
    return ret;
}

ENTRY_FUNC_DECLARE(CPluginChaofanPay, doGetHtml)
{
    CMyString strFile = req.getParam("filename");
    CMyString strPath("./html/chaofan/%s", strFile.c_str());

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

int32_t CPluginChaofanPay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError)
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

    // TODO: 验签

    CMyString status = reader.getString("repCode");
    strError = reader.getString("repMsg");
    if (status != "0001")
    {
        // 支付失败
        strError.Format("resp[%s], status=%s", strResp.c_str(), status.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1025;
    }
    strTransferUrl = reader.getString("resultUrl");

    return 0;
}

/*
void CPluginChaofanPay::dealRelativePath(CMyString &strData, const CMyString &strUrl)
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

*/

CMyString CPluginChaofanPay::getClientIp(http::CHttpRequest &req, IClient *pClient)
{
    // TODO: 需要依据XML配置来查找真实IP 
    CMyString strIp1 = pClient->getIpStr();
    CMyString strIp2 = req.getHeader("X-Real-IP");
    if (strIp1 == "127.0.0.1" || strIp1 == "0.0.0.0")
    {
        if (strIp2.empty())
        {
            return strIp1;
        }
        return strIp2;
    }
    return strIp1;
}


