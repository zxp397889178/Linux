/*
 * =====================================================================================
 *
 *       Filename:  asia66_pay.cpp
 *
 *    Description:  亚洲娱乐66支付
 *
 *        Version:  1.0
 *        Created:  2020年05月16日 20时37分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "asia66_pay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginAsia66Pay::CPluginAsia66Pay()
    : CPayWrapper()
{
    //mkdir("./html/asia66", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginAsia66Pay::~CPluginAsia66Pay()
{

}

int32_t CPluginAsia66Pay::Init(void)
{
    CMyString strSect("main/asia66");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    
    return 0;
}

int32_t CPluginAsia66Pay::Update(void)
{
    return this->Init();
}

void CPluginAsia66Pay::Maintance()
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

ENTRY_FUNC_DECLARE(CPluginAsia66Pay, doPay)
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
    CSdk* pSdk = pChannel->getSdk();

///////// 以下为宝丽平台专用调用
    int32_t payMoney(0);
    CMyString strProductId;
    CMyString strRemark;
    req.getParam("productId", strProductId);
    req.getParam("Remark", strRemark);
    req.getParam("amount", payMoney);

    // 生成发送到支付平台的数据
    int32_t nType = StrToInt32(pChannel->getBankCode().c_str());
    int32_t nMchId = StrToInt32(pSdk->getAccount().c_str());
    double dblPrice = payMoney/100.0;
    CMyString strPrice("%.2lf", dblPrice);
    //CMyString strNonce = getNonce(20);
    CMyString strNonce = req.getParam("uin");
    CMyString strParam("%s-%s", req.getParam("uin").c_str(), strIp.c_str());
    http::CHttpRespMaker req_data;
    req_data.add("payId", strOrderNo);
    req_data.add("type", nType);
    req_data.add("mchid", nMchId);
    req_data.add("price", strPrice);
    req_data.add("label", strNonce);
    req_data.add("param", strParam);
    req_data.add("isHtml", 1);
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    req_data.add("notifyUrl", strNotifyUrl);
    req_data.add("returnUrl", strNotifyUrl);

    // 生成sign
    CMyString str1("%s%d%s%d%s%s", 
            strOrderNo.c_str(), nMchId, strParam.c_str(), nType, strPrice.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str1.c_str());
    strMd5.MakeLower();
    LogDebug("origin text[%s]", str1.c_str());
    LogDebug("md5(key) [%s]", strMd5.c_str());
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
        LogWarn("header [ %s ]", strHeader.c_str());
        LogWarn("resp   [ %s ]", strResp.c_str());
#if 0
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealJsonResp(strResp, strTransferUrl, strError, platformOrderNo);
#else
        std::string::size_type pos = strResp.find("window.location.href");
        if (pos != std::string::npos)
        {
            std::string::size_type pos1 = strResp.find('\'');
            std::string::size_type pos2 = strResp.find('\'', pos1+1);
            CMyString strTemp = strResp.substr(pos1+1, pos2-pos1-1);
            std::string::size_type pos3 = strTemp.find_last_of('/');
            platformOrderNo = strTemp.substr(pos3+1);
            LogDebug("url:%s, order:%s", strTemp.c_str(), platformOrderNo.c_str());
            Json::Value keydata = pChannel->getKeyData();
            strTransferUrl.Format("%s/%s", 
                    keydata["payurl"].asString().c_str(), 
                    strTemp.c_str() );
            result = 0;
        }
#endif
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

ENTRY_FUNC_DECLARE(CPluginAsia66Pay, doCallback)
{
    safe_strcpy(szResp, cbResp, "success");
    cbResp = strlen(szResp);

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogInfo("asia66-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("asia66-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    CMyString strOrderNo = req.getParam("payId");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }
    // TODO: 验签

    // 填充返回结果数据
    CMyString strPlatformId = req.getParam("orderId");
    int32_t code(payCallbackOK);
/*
    int32_t result = req.getParamInt("status");
    if (result==2 || result==3)
    {
        code = payCallbackOK;
    }
    else
    {
        code = payCallbackFail;
    }
*/    
    double totalFee(0.00);
    double realFee(0.00);  // 转换成分
    req.getParam("price", totalFee);
    req.getParam("reallyPrice", realFee);

    int32_t nRealFee = realFee*100;
    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, nRealFee);
    return ret;
}

int32_t CPluginAsia66Pay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo)
{
    CJsonWrapper reader;
    if (!reader.Init(strResp.c_str()))
    {
        // 返回串不合法
        strError.Format("resp [%s] not valid json string", strResp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1023;
    }

    CMyString status = reader.getString("code");
    CMyString strNote = reader.getString("msg");
    if (status != "1")
    {
        // 支付失败
        strError.Format("resp[%s], status:%s, error:%s", strResp.c_str(), status.c_str(), strNote.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1025;
    }
    // TODO: 验签

    CMyString strSign = reader.getString("sign");
    platformOrderNo = reader.getString("orderId");
    strTransferUrl = reader.getString("payUrl");

    return 0;
}


