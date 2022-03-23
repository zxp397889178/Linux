/*
 * =====================================================================================
 *
 *       Filename:  TangrenPay.cpp
 *
 *    Description:  唐人支付SDK
 *
 *        Version:  1.0
 *        Created:  2020年04月23日 18时28分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "TangrenPay.h"
#include "Config.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginTangrenPay::CPluginTangrenPay()
    : CPayWrapper()
{
}

CPluginTangrenPay::~CPluginTangrenPay()
{
}

int32_t CPluginTangrenPay::Init(void)
{
    CMyString strSect("main/Tangren");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }

    return 0;
}

int32_t CPluginTangrenPay::Update(void)
{
    return this->Init();
}

void CPluginTangrenPay::Maintance(void)
{
    CPayWrapper::Maintance();
}

ENTRY_FUNC_DECLARE(CPluginTangrenPay, doPay)
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

/////////
    int32_t payMoney(0);
    CMyString strProductId;
    CMyString strRemark;
    req.getParam("productId", strProductId);
    req.getParam("Remark", strRemark);
    req.getParam("amount", payMoney);


    // 生成请求发送到鸿鑫平台
    http::CHttpRespMaker req_data;
    req_data.add("order_no", strOrderNo);
    Json::Value &keydata = pChannel->getKeyData();
    req_data.add("sid", keydata["sid"].asString());
    req_data.add("payType", pChannel->getBankCode());
    req_data.add("total_fee", CMyString("%.2lf", payMoney/100.0));

    // 生成加密串
    CMyString strSign= req_data.http_str();
    LogDebug("origin text [%s]", strSign.c_str());

    // 发送到唐人支付平台
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

        LogWarn("header [ %s ]", strHeader.c_str());
        LogWarn("resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealJsonResp(http_resp, strResp, strError, platformOrderNo);
        if (result >=0)
        {
            // 拼装URL，让前端直接访问
            strTransferUrl = strError;
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

ENTRY_FUNC_DECLARE(CPluginTangrenPay, doCallback)
{
    // 返回给前端
    ext_headers["Content-Type"] = "text/plain";
    safe_strcpy(szResp, cbResp, "success");
    cbResp = strlen(szResp);

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogImpt("Tangren-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    
    // 校验回调IP
    CMyString strOrderNo = req.getParam("order_no");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }

    // TODO: 检查订单是否合法

    // 填充返回结果数据
    CMyString strPlatformId = req.getParam("seller_id");
    CMyString status = req.getParam("trade_status");
    int32_t code(0);
    if (status=="TRADE_SUCCESS")
    {
        code = payCallbackOK;
    }
    else
    {
        code = payCallbackFail;
    }
    double total_fee = req.getParam("total_fee").asDouble();
    int32_t realFee = total_fee*100;

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, realFee);
    return ret;
}


int32_t CPluginTangrenPay::dealJsonResp(http::CHttpRespone &resp, const CMyString &strResp, CMyString &strError, CMyString &platformOrderNo)
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
    if (code != 0)
    {
        strError = reader.getString("message");
        LogErr("[%s:%s:%d]resp failure, code:%d, message:%s",
                __FILE__, __func__, __LINE__, code, strError.c_str());
        return -3;
    }

    strError = reader.getString("data");
    //platformOrderNo = data.getString("code");
    return 0;
}

/*
int32_t CPluginTangrenPay::queryOrder(const CMyString &strOrderNo)
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
    // 发送到鸿鑫平台
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




