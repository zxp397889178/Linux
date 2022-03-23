/*
 * =====================================================================================
 *
 *       Filename:  V8Pay.cpp
 *
 *    Description:  V8充值实现
 *
 *        Version:  1.0
 *        Created:  2020年08月05日 16时18分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "V8Pay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginV8Pay::CPluginV8Pay()
    : CPayWrapper()
{
    //mkdir("./html/v8", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginV8Pay::~CPluginV8Pay()
{
}

int32_t CPluginV8Pay::Init(void)
{
    CMyString strSect("main/v8");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    return 0;
}

int32_t CPluginV8Pay::Update(void)
{
    return this->Init();
}

void CPluginV8Pay::Maintance()
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

// 以下为调用网银接口
ENTRY_FUNC_DECLARE(CPluginV8Pay, doPay)
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
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    CMyString strCallbackUrl = pGame->getCallbackUrl();

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
    char szTime[64] = "";
    TimeToStr(::time(NULL), szTime, sizeof(szTime), "yyyy-mm-dd HH:MM:SS");
    CMyString strNonce = getNonce(32);
    //CMyString strNonce = req.getParam("uin");
    //CMyString strParam("%s-%s", req.getParam("uin").c_str(), strIp.c_str());
    // CMyString strRemark2("%s@%s", req.getParam("uin").c_str(), strIp.c_str());
    CMyString strUin = req.getParam("uin");
    http::CHttpRespMaker req_data;
    req_data.add("pay_memberid", pSdk->getAccount());
    req_data.add("pay_bankcode", pChannel->getBankCode());
    req_data.add("pay_orderid", strOrderNo);
    req_data.add("pay_applydate", szTime);
    req_data.add("pay_amount", dblPrice);
    req_data.add("pay_notifyurl", strNotifyUrl);
    req_data.add("pay_callbackurl", strCallbackUrl);

    // 生成sign
    CMyString str1 = req_data.http_str_manual('&', '=', true, true); // req_data.http_str();
    //CMyString str1 = req_data.http_str();
    CMyString str2("%s&key=%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeUpper();
    req_data.add("ctoken", strNonce);
    req_data.add("pay_md5sign", strMd5.c_str());
    req_data.add("pay_attach", req.getParam("uin"));
    req_data.add("pay_productname", strProductId);
    req_data.add("is_json", "1");
    CMyString strSign = req_data.http_str_manual('&', '=', true, true);
    //CMyString strSign("%s&sign=%s", str1.c_str(), strMd5.c_str());
    LogDebug("[V8]origin text[%s]", str1.c_str());
    LogDebug("[V8]key text   [%s]", str2.c_str());
    LogDebug("[V8]md5        [%s]", strMd5.c_str());
    LogDebug("[V8]signed text[%s]", strSign.c_str());

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
        CMyString strUrl("%s", pSdk->getPayUrl().c_str());
        ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            CMyString strMsg;
            m_http.getError(ret, strMsg);
            CMyString strNote("用户(uin=%d)调起富连支付失败，错误码:%d, 错误说明:%s", strUin.c_str(), ret, strMsg.c_str() );
            CPayWrapper::postAlert(pGame, strNote);

            strError.Format("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("[V8] header [ %s ]", strHeader.c_str());
        LogWarn("[V8] resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealJsonResp(strResp, strTransferUrl, strError, platformOrderNo, pChannel, payMoney);
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
    LogDebug("V8-PayUrl:%s", strTransferUrl.c_str());
    //CMyString strNewUrl = CHttpWrapper::encodeURI(strTransferUrl);
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginV8Pay, doCallback)
{
    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogInfo("V8-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("V8-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    
    // 校验回调IP
    CMyString strOrderNo = req.getParam("orderid");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
    }

    // TODO:验签

    // 填充返回结果数据
    CMyString strPlatformId = req.getParam("transaction_id");
    CMyString status = req.getParam("returncode");
    int32_t code(payCallbackFail);
    if (status != "00")
    {
        code = payCallbackFail;
    }
    else
    {
        code = payCallbackOK;
    }

    //CMyString strFee = req.getParam("total_fee");
    CMyString strRealFee = req.getParam("amount");
    double realFee = strRealFee.asDouble();
    int32_t nRealFee = realFee * 100; // 转换成分

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, nRealFee);

    // 回传到回调方
    safe_strcpy(szResp, cbResp, "OK");
    cbResp = strlen(szResp);
    return ret;
}

int32_t CPluginV8Pay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo, CSdkChannel *pChannel, int32_t payMoney)
{
    CGame *pGame = pChannel->getGame();
    CJsonWrapper reader;
    if (!reader.Init(strResp.c_str()))
    {
        CMyString strNote("V8支付调起结果异常, %s", strResp.c_str());
        CPayWrapper::postAlert(pGame, strNote);

        strError.Format("V8支付调用异常,请联系客服");
        // 返回串不合法
        LogErr("[%s:%s:%d]V8 resp [%s] not valid json string", __FILE__, __func__, __LINE__, strResp.c_str());
        return -1023;
    }

    int32_t status = reader.getInt("code");
    if (status != 1)
    {
        CMyString strMsg = reader.getString("msg");
        // STEP1: 发给IM
        CMyString strNote("V8支付调起失败,错误描述:%s", strMsg.c_str());
        CPayWrapper::postAlert(pChannel->getGame(), strNote);

        // STEP2: 发给前端
        CMyString strClientMsg;
        int32_t deal = CPayWrapper::dealRespError(pChannel, payMoney,strMsg, strClientMsg);
        strError.Format("[V8支付]%s", strClientMsg.c_str());

        // STEP4: 记录日志
        LogErr("[%s:%s:%d]V8-Pay call failure, code:%d, error:%s", __FILE__, __func__, __LINE__, status, strMsg.c_str());
        return -1025;
    }

    strTransferUrl = reader.getString("pay_url");
    platformOrderNo = reader.getString("orderid");

    return 0;
}

