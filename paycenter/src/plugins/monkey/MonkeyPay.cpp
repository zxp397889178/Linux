/*
 * =====================================================================================
 *
 *       Filename:  MonkeyPay.cpp
 *
 *    Description:  灵猴游戏SDK，用于捕鱼对接
 *
 *        Version:  1.0
 *        Created:  2020年03月27日 09时48分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "MonkeyPay.h"
#include "Config.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginMonkeyPay::CPluginMonkeyPay()
    : CPayWrapper()
{

}

CPluginMonkeyPay::~CPluginMonkeyPay()
{

}

int32_t CPluginMonkeyPay::Init(void)
{
    CMyString strSect("main/monkey");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }

    return 0;
}

int32_t CPluginMonkeyPay::Update(void)
{
    return this->Init();
}

void CPluginMonkeyPay::Maintance(void)
{
    CPayWrapper::Maintance();
}

ENTRY_FUNC_DECLARE(CPluginMonkeyPay, doPay)
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

///////// 以下为众币平台专用调用
    int32_t payMoney(0);
    CMyString strProductId;
    CMyString strRemark;
    req.getParam("productId", strProductId);
    req.getParam("Remark", strRemark);
    req.getParam("amount", payMoney);


    // 生成请求发送到众币平台
    http::CHttpRespMaker req_data;
    req_data.add("gameId", pSdk->getAccount());
    req_data.add("uid", req.getParamStr("uin"));
    req_data.add("orderId", strOrderNo);
    req_data.add("amount", payMoney);
    // 生成加密串
    CMyString str1 = req_data.http_str();
    CMyString strKey = pSdk->getApiKey();
    CMyString str2("%s%s", str1.c_str(), strKey.c_str());
    CMyString str3 = CMD5Wrapper::encode(str2.c_str());
    str3.MakeUpper();
    LogDebug("origin text [%s]", str1.c_str());
    LogDebug("with key:   [%s]", strKey.c_str());
    LogDebug("before md5: [%s]", str2.c_str());
    LogDebug("signature   [%s]", str3.c_str());
    req_data.add("sign", str3);
    CMyString strSign = req_data.json_str();
    LogDebug("sign str:   [%s]", strSign.c_str());

    // 发送到众币平台
    //CMyString strContentType("Content-Type: application/x-www-form-urlencoded");
    CMyString strContentType("Content-Type: application/json");
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

        LogWarn("header [ %s ]", strHeader.c_str());
        LogWarn("resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealJsonResp(http_resp, strResp, strError, platformOrderNo);
        if (result >=0)
        {
            // 拼装URL，让前端直接访问
            Json::Value &extdata = pChannel->getKeyData();
            CMyString strAccessUrl = extdata["access_url"].asString();
            strTransferUrl.Format("%s%s%s",strAccessUrl.c_str(),
                    "?O=", platformOrderNo.c_str());
        }
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
    CPayWrapper::OnPayResp(0, "access url by resp", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    // TODO: 记录查询订单所需要的数据
    return 0;
}

ENTRY_FUNC_DECLARE(CPluginMonkeyPay, doCallback)
{
    // 返回给前端
    ext_headers["Content-Type"] = "application/json";
    safe_strcpy(szResp, cbResp, "{\"code\":10}");
    cbResp = strlen(szResp);

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogImpt("monkey-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    
    // 校验回调IP
    CMyString strOrderNo = req.getParam("orderId");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }

    // TODO: 检查订单是否合法

    // 填充返回结果数据
    CMyString strPlatformId = req.getParam("tradeId");
    int32_t code = req.getParamInt("status");
    if (code==2)
    {
        code = payCallbackOK;
    }
    else
    {
        code = payCallbackFail;
    }
    int32_t realFee(0);
    req.getParam("amount", realFee);

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, realFee);
    return ret;
}


int32_t CPluginMonkeyPay::dealJsonResp(http::CHttpRespone &resp, const CMyString &strResp, CMyString &strError, CMyString &platformOrderNo)
{
    CJsonWrapper reader;
    if (!reader.Init(strResp.c_str()))
    {
        strError.Format("resp data [%s] not valid json", strResp.c_str());
        LogErr("[%s:%s:%d]%s",
                __FILE__, __func__, __LINE__, strError.c_str());
        return -1;
    }

    int32_t code = reader.getInt("code");
    if (code != 10)
    {
        strError = reader.getString("message");
        LogErr("[%s:%s:%d]resp failure, code:%d, message:%s",
                __FILE__, __func__, __LINE__, code, strError.c_str());
        return -3;
    }

    CJsonWrapper data;
    int32_t ret = reader.getReader("data", data);
    if (!ret)
    {
        strError.Format("resp [%s] invalid ", strResp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -3;
    }

    platformOrderNo = data.getString("tradeId");
    //CMyString strUrl = data.getString("paymentUrl");
    LogImpt("monkey resp, platformOrderNo [%s]", platformOrderNo.c_str());
    //strError = strUrl;
    return 0;
}

/*
int32_t CPluginMonkeyPay::queryOrder(const CMyString &strOrderNo)
{
    http::CHttpRespMaker req_data;
    req_data.add("merchantId", m_strAccount);
    req_data.add("timestamp", CMyString("%lu", Now()) );
    req_data.add("signatureMethod", "HmacSHA256");
    req_data.add("signatureVersion", "1");
    // req_data.add("jUserId", req.getParamStr("uin"));
    // 生成加密串
    CMyString str1 = req_data.http_str();
    CMyString str2 = ShaWrapper::Hmac("sha256", m_strApiKey.c_str(), str1.c_str());
    req_data.add("signature", str2);
    CMyString strSign = req_data.http_str();
    // 发送到众币平台
    CMyString strContentType("Content-Type: application/x-www-form-urlencoded");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());

    CMyString strHeader;
    CMyString strResp;
    CMyString strError;
    CMyString strTransferUrl;
    int32_t result(0);
    do {
        int32_t ret = m_http.HttpPostWithHeaders(m_sdkURL.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret < 0)
        {
            strError.Format("host [%s] unreachable.", m_sdkURL.c_str());
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
        // TODO: 检查数据是否合理
    }while(0);

    return 0;
}
*/


