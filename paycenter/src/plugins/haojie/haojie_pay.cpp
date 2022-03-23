/*
 * =====================================================================================
 *
 *       Filename:  haojie_pay.cpp
 *
 *    Description:  豪杰支付API 接口实现
 *
 *        Version:  1.0
 *        Created:  03/15/2020 01:15:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "haojie_pay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginHaojiePay::CPluginHaojiePay()
    : CPayWrapper()
{
    //mkdir("./html/haojie", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginHaojiePay::~CPluginHaojiePay()
{

}

int32_t CPluginHaojiePay::Init(void)
{
    CMyString strSect("main/haojie");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    
    return 0;
}

int32_t CPluginHaojiePay::Update(void)
{
    return this->Init();
}

void CPluginHaojiePay::Maintance()
{
    CPayWrapper::Maintance();
}

ENTRY_FUNC_DECLARE(CPluginHaojiePay, doPay)
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
    CJsonWrapper sdk_extdata;
    pSdk->getPayExtraData(sdk_extdata);
    //LogWarn("extra_data:[%s], appid[%s]", keydata.ext_data.c_str(), sdk_extdata.getString("appid").c_str());

    http::CHttpRespMaker req_data;
    req_data.add("mchId", pSdk->getAccount());
    req_data.add("appId", sdk_extdata.getString("appid"));
    req_data.add("productId", pChannel->getBankCode());
    req_data.add("mchOrderNo", strOrderNo);
    req_data.add("currency", "cny");
    req_data.add("amount", payMoney);
    req_data.add("clientIp", strIp);
    //req_data.add("device", );
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    CMyString strCallbackUrl = pGame->getCallbackUrl();
    req_data.add("returnUrl", strCallbackUrl);
    req_data.add("notifyUrl", strNotifyUrl);
    req_data.add("subject", CMyString("pay_item_%.2lf", payMoney/100.0));
    req_data.add("body", CMyString("pay_money_%.2lf-timetick_%lu", payMoney/100.0, Now()));
    //req_data.add("param1", strRemark);
    //req_data.add("param2", );

    // 生成sign
    CMyString str1 = req_data.http_str(true);
    CMyString str2("%s&key=%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeUpper();
    LogDebug("origin text[%s]", str1.c_str());
    LogDebug("with key[%s]", str2.c_str());
    LogDebug("md5(key) [%s]", strMd5.c_str());

    req_data.add("sign", strMd5.c_str());
    CMyString strSign = req_data.json_str(true);
    //CMyString strSign = req_data.http_str(true);
    CMyString strReqData("params=%s", strSign.c_str());
    LogDebug("need send data[%s]", strReqData.c_str());

    // 发送到第三方平台
    CMyString strContentType("Content-Type: application/x-www-form-urlencoded");
    //CMyString strContentType("Content-Type: application/json");
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
        ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strReqData.c_str());
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
        result = dealJsonResp(strResp, strTransferUrl, strError, platformOrderNo);
        if (result >=0)
        {
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
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginHaojiePay, doCallback)
{
    safe_strcpy(szResp, cbResp, "success");
    cbResp = strlen(szResp);

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogInfo("haojie-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("haojie-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());

    // 校验回调IP
    CMyString strOrderNo = req.getParam("mchOrderNo");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }
    // TODO: 验签

    // 填充返回结果数据
    CMyString strPlatformId = req.getParam("payOrderId");
    CMyString strExtOrderNo = req.getParam("channelOrderNo");
    int32_t code(0);
    int32_t result = req.getParamInt("status");
    if (result==2 || result==3)
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

int32_t CPluginHaojiePay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo)
{
    CJsonWrapper reader;
    if (!reader.Init(strResp.c_str()))
    {
        // 返回串不合法
        strError.Format("resp [%s] not valid json string", strResp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1023;
    }

    CMyString status = reader.getString("retCode");
    CMyString strNote = reader.getString("retMsg");
    if (status != "SUCCESS")
    {
        // 支付失败
        strError.Format("resp[%s], status:%s, error:%s", strResp.c_str(), status.c_str(), strNote.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1025;
    }
    // TODO: 验签

    CMyString strSign = reader.getString("sign");
    platformOrderNo = reader.getString("payOrderId");

    CJsonWrapper data;
    if (!reader.getReader("payParams", data))
    {
        // 没有数据
        strError.Format("resp[%s], get payParams filed FAIL", strResp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1027;
    }
    strError = data.getString("payUrl");
    if (strError.empty())
    {
        strError.Format("resp[%s], get payUrl filed FAIL", strResp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1029;
    }

    return 0;
}

