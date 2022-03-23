/*
 * =====================================================================================
 *
 *       Filename:  zhpay.cpp
 *
 *    Description:  综合支付
 *
 *        Version:  1.0
 *        Created:  2019年11月14日 22时50分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "zhpay.h"
#include "Security.h"
#include "Config.h"
#include "IClient.h"
#include "Common.h"
#include "pay_error.h"

CPluginZhpay::CPluginZhpay()
    : CPayWrapper()
{
}

CPluginZhpay::~CPluginZhpay()
{
}

int32_t CPluginZhpay::Init(void)
{
    CMyString strSect("main/zhpay");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    
    return 0;
}

int32_t CPluginZhpay::Update(void)
{
    return this->Init();
}

void CPluginZhpay::Maintance(void)
{
    CPayWrapper::Maintance();

}

ENTRY_FUNC_DECLARE(CPluginZhpay, doZhPay)
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
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    CMyString strCallbackUrl = pGame->getCallbackUrl();

////////<<<<<<<<<<<<<<<<<<<< 以下为支付平台专有调用 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    int32_t payMoney(0);
    CMyString strProductId;
    CMyString strRemark;
    req.getParam("productId", strProductId);
    req.getParam("Remark", strRemark);
    req.getParam("amount", payMoney);

    // 生成发到支付平台的数据
    http::CHttpRespMaker request_data;
    request_data.add("appid", pSdk->getAccount());
    request_data.add("order_no", strOrderNo); 
    request_data.add("amount", CMyString("%d", payMoney));
    request_data.add("product_name", strProductId);
    request_data.add("bank_code", pChannel->getBankCode());
    //request_data.add("attach", strRemark);
    request_data.add("notify_url", strNotifyUrl); 
    request_data.add("return_url", strCallbackUrl); 
    CMyString str1 = request_data.http_str();
    CMyString str2("%s&secret=%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeLower();
    request_data.add("sign", strMd5);
    CMyString strSign = request_data.http_str();

    LogDebug("origin txt[%s]", str1.c_str());
    LogDebug("pending md5 [%s]", str2.c_str());
    CMyString txt;
    LogDebug("md5 [%s]", color_text(_color_red, strMd5.c_str(), txt) );
    LogDebug("sign text [%s]", strSign.c_str());

    // 发送到支付平台
    CMyString strResp;
    CMyString strContentType("Content-Type:application/x-www-form-urlencoded");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    CMyString strUrl = pSdk->getPayUrl();
    CMyString strHeader;
    //Json::Value reader_resp;
    CJsonWrapper reader_resp;
    CJsonWrapper reader_data;
    CMyString strError;
    time_t t1 = time(NULL);
    char szReqTime[64] = "";
    TimeToStr(t1, szReqTime, sizeof(szReqTime));
    int32_t result(0);
    CMyString platformOrderNo("N/A");
    do {
        ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            // 远程连接失败
            LogFatal("[%s:%s:%d]url [%s] 不可到达，请检查网络设置或白名单", 
                    __FILE__, __func__, __LINE__, strUrl.c_str());
            strError.Format("url %s can not reach.", strUrl.c_str());
            result = -1021;
            break;
        }

        // 解析strResp
        if (!reader_resp.Init(strResp.c_str()))
        {
            // 返回串不合法
            LogFatal("[%s:%s:%d]resp [%s] 不合法", __FILE__, __func__, __LINE__, strResp.c_str());
            strError.Format("resp data not json string");
            result = -1023;
            break;
        }
        int32_t status = reader_resp.getInt("status");
        if (status != 1)
        {
            strError = reader_resp.getString("message");
            // 支付失败
            LogFatal("[%s:%s:%d]resp [%s], tatus=%d, message=%s", 
                    __FILE__, __func__, __LINE__, strResp.c_str(), status, strError.c_str());
            result = -1025;
            break;
        }
        ret = reader_resp.getReader("data", reader_data);
        if (!ret)
        {
            LogFatal("[%s:%s:%d]resp [%s] 结果数据不合法", __FILE__, __func__, __LINE__, strResp.c_str());
            strError.Format("resp data not json string");
            result = -1027;
            break;
        }
        if (reader_data.getInt("pay_status")==0)
        {
            LogFatal("[%s:%s:%d]pay failure", __FILE__, __func__, __LINE__);
            strError = "pay failure.";
            result = -1029;
            break;
        }
        platformOrderNo = reader_data.getString("platform_order_no");
    }while(0);

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
    CPayWrapper::OnPayResp(0, reader_resp.getString("message"), 
            reader_data.getString("order_no"), 
            reader_data.getString("redirect_url"), 
            szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginZhpay, doZhCallback)
{
    safe_strcpy(szResp, cbResp, "SUCCESS");
    cbResp = strlen(szResp);

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogInfo("Zhpay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());

    CGame* pGame(NULL);
    CMyString strOrderNo = req.getParam("order_no");
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret<0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }

    // TODO:验签
    // 获得返回notify的关键数据
    CMyString strPlatformId = req.getParam("platform_order_no");
    int32_t result = req.getParamInt("pay_status");
    int32_t code = (result<=0) ? payCallbackFail : payCallbackOK;
    int32_t realFee = req.getParamInt("actual_amount");

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, realFee);
    return ret;
}


