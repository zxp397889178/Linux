/*
 * =====================================================================================
 *
 *       Filename:  Pay77.cpp
 *
 *    Description:  pay77 接口实现
 *
 *        Version:  1.0
 *        Created:  2021年01月08日 15时14分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Pay77.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"
#include <algorithm>

static void dealResp(Json::Value &resp, char *szResp, uint32_t &cbResp)
{
    CJsonWrapper writer;
    CMyString strResp = writer.write(resp);
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}
static int32_t CheckMysqlError(ISimDB *pDB, char *szResp, uint32_t &cbResp)
{
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        CMyString strMsg("mysql errno:%d, error:%s", err, szError);
        Json::Value resp;
        resp["code"] = -1;
        resp["message"] = strMsg;
        LogFatal(strMsg.c_str());
        return -1;
    }
    return 0;
}
static int32_t CheckMysqlErrorEx(ISimDB *pDB, Json::Value &resp, char *szResp, uint32_t &cbResp)
{
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        CMyString strMsg("mysql errno:%d, error:%s", err, szError);
        resp["code"] = -1;
        resp["message"] = strMsg;
        LogFatal(strMsg.c_str());
        return -1;
    }
    return 0;
}
#define CHECK_MYSQL_ERROR(err)      int32_t (err) = CheckMysqlError(m_pDB, szResp, cbResp); if (err!=0) { return -1; }
static int32_t getGamesvrResult(int32_t status)
{
    // status => t_withdraw.status 字段的状态序号
    // result => 返回给游戏服务器的状态: 
    //  0 表示状态未结束，需要后处理，不该返回给游服
    // -1 表示状态错误
    // 1/2/8/9 状态正确
    //                           1  2  3  4  5  6  7  8   9  10  11  12  13
    static int32_t s_result[] = {0, 9, 8, 0, 0, 1, 9, 8, -1,  2,  9,  8,  9};

    if (status<=0 || status>=(int32_t)(sizeof(s_result)/sizeof(int32_t)) )
    {
        return -1;
    }
    return s_result[status-1];
}

/////////////////////////////////////////////////////////////////////
// CPluginPay77
CPluginPay77::CPluginPay77()
    : CPayWrapper("Pay77Pay")
{
    //mkdir("./html/v8", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginPay77::~CPluginPay77()
{
}

int32_t CPluginPay77::Init(void)
{
    CMyString strSect("main/pay77");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    m_timer.SetInterval(10*1000);
    m_timer.Startup();
    return 0;
}

int32_t CPluginPay77::Update(void)
{
    return this->Init();
}

void CPluginPay77::Maintance()
{
    CPayWrapper::Maintance();

    // 从redis中读取数据
    if (m_timer.ToNextTime())
    {
        std::vector<std::string> vecCreditOrder;
        m_redis.Keys("credit-pay77#keydata#order_*", vecCreditOrder);
        for(std::vector<std::string>::iterator it(vecCreditOrder.begin());
                it != vecCreditOrder.end(); ++it)
        {
            CMyString creditKey = *it;
            std::string creditData;
            int32_t val = m_redis.Get(creditKey, creditData);
            if (val != 1)
            {
                continue;
            }
            this->dealCreditOrder(creditData);
        }
    }
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
ENTRY_FUNC_DECLARE(CPluginPay77, doPay)
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
    CMyString strClientIp = getClientIpEx(getIpFromHeader, pClient, req, "X-Real-IP");
    CSdk *pSdk = pChannel->getSdk();
    CGame *pGame = pChannel->getGame();
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    CMyString strCallbackUrl = pGame->getCallbackUrl();

///////// 以下为宝丽平台专用调用
    int64_t payMoney(0);
    CMyString strProductId;
    CMyString strRemark;
    req.getParam("productId", strProductId);
    req.getParam("Remark", strRemark);
    req.getParam("amount", payMoney);

    // 生成发送到支付平台的数据
    payMoney = payMoney/100;    // 以元存储
    //CMyString strPrice("%.2lf", dblPrice);
    //CMyString strPrice = double_to_str(dblPrice, 2);
    char szTime[64] = "";
    TimeToStr(::time(NULL), szTime, sizeof(szTime), "yyyy-mm-dd HH:MM:SS");
    CMyString strNonce = getNonce(16);
    //CMyString strNonce = req.getParam("uin");
    //CMyString strParam("%s-%s", req.getParam("uin").c_str(), strIp.c_str());
    // CMyString strRemark2("%s@%s", req.getParam("uin").c_str(), strIp.c_str());
    CMyString strUin = req.getParam("uin");
    http::CHttpRespMaker req_data;
    // 时区校正
    CXmlFile &xml = CONFIG.getXml();
    int32_t adjust = xml.GetLong("main/paycenter/timezone", "adjust");
    int32_t nReqTime = time(NULL) + adjust; 
    req_data.add("reqtime", nReqTime);
    CMyString strAccount = pSdk->getAccount();
    req_data.add("pid", strAccount.asInt());
    req_data.add("money", payMoney);
    req_data.add("out_trade_no", strOrderNo);
    req_data.add("notifyurl", strNotifyUrl);
    req_data.add("returnurl", strCallbackUrl);

    // 生成sign
    CMyString str1 = req_data.http_str_manual('&', '=', false, false); // req_data.http_str();
    //CMyString str1 = req_data.http_str();
    CMyString str2("%s&%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeLower();
    req_data.add("content_type", "text");
    req_data.add("type", pChannel->getBankCode());
    req_data.add("remark", strNonce);
    req_data.add("sign", strMd5.c_str());
    // CMyString strSign = req_data.json_str();
    CMyString strSign = req_data.http_str_manual('&', '=', false, true);
    //CMyString strSign("%s&sign=%s", str1.c_str(), strMd5.c_str());
    LogDebug("[Pay77]origin text[%s]", str1.c_str());
    LogDebug("[Pay77]key text   [%s]", str2.c_str());
    LogDebug("[Pay77]md5        [%s]", strMd5.c_str());
    LogDebug("[Pay77]signed text[%s]", strSign.c_str());

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
            CMyString strNote("用户(uin=%s)调起PAY77支付失败，错误码:%d, 错误说明:%s", strUin.c_str(), ret, strMsg.c_str() );
            CPayWrapper::postAlert(pGame, strNote);

            strError.Format("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("[PAY77] header [ %s ]", strHeader.c_str());
        LogWarn("[PAY77] resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealRechargeResp(http_resp, strTransferUrl, strError, platformOrderNo, pChannel, payMoney);
        //result = dealJsonResp(strResp, strTransferUrl, strError, platformOrderNo, pChannel, payMoney);
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
    LogDebug("Pay77-PayUrl:%s", strTransferUrl.c_str());
    //CMyString strNewUrl = CHttpWrapper::encodeURI(strTransferUrl);
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginPay77, doCallback)
{
    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);

    // 校验回调IP
    //CMyString strIp = getClientIp(pClient, req);
    CMyString strIp = getClientIpEx(getIpFromHeader, pClient, req, "X-Real-IP");
    LogInfo("Pay77-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("Pay77-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
/*    
    if (!this->ValidIp(strClientIp,"main/pay77/ip", "main/pay77/ip/recharge"))
    {
        Json::Value resp;
        resp["code"] = -1;
        resp["message"] = CMyString("ip %s 不合法", strClientIp.c_str());
        dealResp(resp, szResp, cbResp);
        return -1;
    }
*/    
    CMyString strOrderNo = req.getParam("out_trade_no");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        int32_t result = PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
        CMyString strNote("pay77 callback ip [%s] invalid", strIp.c_str());
        CPayWrapper::OnPayFailure(result, strNote, szResp, cbResp);
        return result;
    }

    // 验签
    int32_t result = ValidPayCallbackSign(req, strOrderNo, szResp, cbResp);
    if (result != 0)
    {
        return result;
    }

    // 填充返回结果数据
    CMyString strPlatformId = req.getParam("trade_no");
    CMyString status = req.getParam("status");
    int32_t code(payCallbackFail);
    if (status != "success")
    {
        code = payCallbackFail;
    }
    else
    {
        code = payCallbackOK;
    }

    CMyString strFee = req.getParam("money");
    CMyString strRealFee = req.getParam("money_real");
    double realFee = strRealFee.asDouble();
    int64_t nRealFee = realFee * 100; // 转换成分

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, nRealFee);

    // 回传到回调方
    safe_strcpy(szResp, cbResp, "success");
    cbResp = strlen(szResp);
    return ret;
}

int32_t CPluginPay77::ValidPayCallbackSign(http::CHttpRequest &req, const CMyString &strOrderNo, char *szResp, uint32_t &cbResp)
{
    CSdkChannel *pChannel = CPayWrapper::on_get_order_sdkChannel(strOrderNo);
    if (NULL == pChannel)
    {
        Json::Value resp;
        resp["code"] = -1;
        resp["message"] = CMyString("not found orderNo %s", strOrderNo.c_str());
        dealResp(resp, szResp, cbResp);
        return -1;
    }

    CSdk *pSdk = pChannel->getSdk();
    if (NULL == pSdk)
    {
        Json::Value resp;
        resp["code"] = -1;
        resp["message"] = CMyString("not found SDK %s", pChannel->getSdkCode().c_str());
        dealResp(resp, szResp, cbResp);
        return -3;
    }

    http::CHttpRespMaker resp_data;
    resp_data.add("callback_time", req.getParam("callback_time"));
    resp_data.add("pid", req.getParam("pid"));
    resp_data.add("trade_no", req.getParam("trade_no"));
    resp_data.add("out_trade_no", req.getParam("out_trade_no"));
    resp_data.add("money", req.getParam("money"));
    resp_data.add("type", req.getParam("type"));
    CMyString str1 = resp_data.http_str_manual('&', '=', false, false); // req_data.http_str();

    CMyString str2("%s&%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeLower();
    CMyString strMd5Valid = req.getParam("sign");
    if (strMd5 != strMd5Valid)
    {
        LogErr("pay77 resp sign [%s], yet verify sign is [%s]",
                strMd5Valid.c_str(), strMd5.c_str());
        int32_t result = PAYFRAME_ERROR(err_SignMismatch);    // 验签失败
        CMyString strNote("pay77 callback sign mismatch");
        CPayWrapper::OnPayFailure(result, strNote, szResp, cbResp);
        return result;
    }

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginPay77, doWithdraw)
{
    ext_headers["Content-Type"] = "application/json";
    CGame *pGame(NULL);

    bool signEnable(false);
    CMyString signField;
    this->getSignInfo("main/paycenter/sign/report", signEnable, signField);
    // 校验传入参数是否合法
    std::vector<CMyString>  vecParam;
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    vecParam.push_back("orderNo");
    vecParam.push_back("bankCode");
    vecParam.push_back("bankCard");
    vecParam.push_back("bankName");
    vecParam.push_back("fee");
    //vecParam.push_back("creditRemark");
    //vecParam.push_back("creditTime");
    //vecParam.push_back("operator");
    vecParam.push_back("token");
    int32_t code = this->valid_req(req, szResp, cbResp, vecParam, 
            signEnable, signField, true, "gameCode", pGame);
    if (0 != code)
    {
        return code;
    }

    // 检查发起的IP是否合法
    CMyString strIp = getClientIpEx(getIpFromHeader, pClient, req, "X-Real-IP");
    if (!this->ValidIp(strIp,"main/pay77/ip", "main/pay77/ip/withdraw"))
    {
        Json::Value resp;
        resp["code"] = -1;
        resp["message"] = CMyString("ip %s 不合法", strIp.c_str());
        dealResp(resp, szResp, cbResp);
        return -1;
    }

    // 获得相关参数
    CMyString gameCode = req.getParam("gameCode");
    CMyString uin = req.getParam("uin");
    CMyString orderNo = req.getParam("orderNo");
    CMyString bankCode = req.getParam("bankCode");
    CMyString bankCard = req.getParam("bankCard");
    CMyString bankName = req.getParam("bankName");
    int64_t fee = req.getParamInt64("fee");
    CMyString creditToken = req.getParam("token");

    CSdk *pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
    if (NULL == pSdk)
    {
        Json::Value resp;
        resp["code"] = -5;
        resp["message"] = CMyString("invalid gamecode %s", gameCode.c_str());
        dealResp(resp, szResp, cbResp);
        return -5;
    }

    // 从redis中获得TOKEN
    CMyString strKey("credit#token#order_%s", orderNo.c_str());
    std::string creditTokenSaved;
    int32_t ret = m_redis.Get(strKey, creditTokenSaved);
    if (ret != 1)
    {
        // 订单号失效，或者订单号过期
        Json::Value resp;
        resp["code"] = -1;
        resp["message"] = "invalid orderNo or time expired.";
        dealResp(resp, szResp, cbResp);
        return -1;
    }
    m_redis.Del(strKey);

    // 检查token是否匹配
    if (strcmp(creditTokenSaved.c_str(), creditToken.c_str()) != 0)
    {
        Json::Value resp;
        resp["code"] = -3;
        resp["message"] = CMyString("orderNo %s token mismatch.", orderNo.c_str());
        dealResp(resp, szResp, cbResp);
        return -3;
    }

    // 检查是否有该订单
    CMyString strError;
    CMyString strOrderResp;
    int32_t state(-1);
    int32_t result = this->CheckCreditOrder(pGame, pSdk->getWithdrawCheckUrl(),
            pSdk->getAccount(), uin, orderNo, pSdk->getApiKey(),
            strError, strOrderResp, state);
    if (result != 0)
    {
        Json::Value resp;
        resp["code"] = -4;
        resp["message"] = strError;
        dealResp(resp, szResp, cbResp);
        return -4;
    }
    if (state != creditNotFound)
    {
        Json::Value resp;
        resp["code"] = -5;
        resp["message"] = CMyString("order %s already found", orderNo.c_str());
        dealResp(resp, szResp, cbResp);
        return -5;
    }
        
    // 向第三方服务器发起请求
    int64_t timeBegin = Now();
    result = this->dealCreditRequest(pSdk, pGame, fee, bankCard, bankCode, bankName, uin, orderNo, szResp, cbResp);
    if (result != 0)
    {
        return result;
    }
    int64_t timeEnd = Now();

    // 写临时数据表redis表中
    Json::Value credit_data;
    credit_data["url"] = pSdk->getWithdrawCheckUrl();
    credit_data["pid"] = pSdk->getAccount();
    credit_data["uin"] = uin;
    credit_data["orderNo"] = orderNo;
    credit_data["key"] = pSdk->getApiKey();
    credit_data["gameCode"] = pGame->getCode();
    credit_data["fee"] = (Json::Int64)fee;
    credit_data["3rdcost"] = (int32_t)(timeEnd - timeBegin);    // 第三方调用的耗时，ms
    CJsonWrapper writer;
    CMyString creditData = writer.write(credit_data);
    CMyString creditKey("credit-pay77#keydata#order_%s", orderNo.c_str());
    m_redis.Set(creditKey, creditData);
    
    // 记录到数据库
    CMyString creditRemark = req.getParam("creditRemark");
    int64_t creditTime = req.getParamInt64("creditTime");
    CMyString creditOperator = req.getParam("operator");
    result = this->LogCreditStatus(orderNo, 5, creditRemark, creditTime, creditOperator, szResp, cbResp);
    if (result != 0)
    {
        return result;
    }

    return 0;
}

int32_t CPluginPay77::dealCreditRequest(CSdk *pSdk, CGame *pGame, int64_t fee, const CMyString &bankCard, const CMyString &bankCode, const CMyString &bankName, const CMyString &uin, const CMyString &orderNo, char *szResp, uint32_t &cbResp)
{
    http::CHttpRespMaker req_credit;
    CXmlFile &xml = CONFIG.getXml();
    //int32_t adjust = xml.GetLong("main/paycenter/timezone", "adjust");
    //int32_t nReqTime = time(NULL) + adjust; 
    int32_t nReqTime = time(NULL);
    req_credit.add("reqtime", nReqTime);
    req_credit.add("pid", pSdk->getAccount());
    // 需要将元转换成分，再向平台请求支付
    fee /= 100;
    req_credit.add("money", fee);
    req_credit.add("outwithdrawno", orderNo);
    req_credit.add("bankno", bankCard);
    CMyString str1 = req_credit.http_str_manual('&', '=', false, false);
    CMyString str2("%s&%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeLower();
    req_credit.add("sign", strMd5);
    req_credit.add("bankname", bankCode);
    req_credit.add("customer_name", bankName);
    // TODO:
    // req_credit.add("notifyurl", <回调地址>)
    req_credit.add("remark", getNonce(16));

    CMyString strContentType("Content-Type: application/x-www-form-urlencoded");
    CMyString str3 = req_credit.http_str_manual('&', '=', false, true);
    //CMyString strContentType("Content-Type: application/json");
    //CMyString str3 = req_credit.json_str();
    LogDebug("[pay77 origin text] %s", str1.c_str());
    LogDebug("[pay77 key text   ] %s", str2.c_str());
    LogDebug("[pay77 md5 text   ] %s", strMd5.c_str());
    LogDebug("[pay77 sign text  ] %s", str3.c_str());

    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    CMyString strUrl(pSdk->getWithdrawUrl());
    LogDebug("[pay77 url] %s", strUrl.c_str());
    CMyString strHeader;
    CMyString strResp;
    CMyString strError;
    CMyString platformOrderNo;
    int32_t result(-1);
    do {
        int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, str3.c_str());
        if (ret != 0)
        {
            CMyString strMsg;
            m_http.getError(ret, strMsg);
            CMyString strNote("用户(uin=%s)调起PAY77付款失败，错误码:%d, 错误说明:%s", uin.c_str(), ret, strMsg.c_str() );
            CPayWrapper::postAlert(pGame, strNote);

            strError.Format("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("[PAY77 WITHDRAW HEADER] %s", strHeader.c_str());
        LogWarn("[PAY77 WITHDRAW REP   ] %s", strResp.c_str());
        //http::CHttpRespone credit_resp;
        //credit_resp.read((void*)strHeader.c_str(), strHeader.size());
        //credit_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealCreditSyncResp(strResp, strError, platformOrderNo);
    }while(0);

    if (result != 0)
    {
        // 出错
        Json::Value resp;
        resp["code"] = -7;
        resp["message"] = strError;
        dealResp(resp, szResp, cbResp);
        return -7;
    }
    return 0;
}

int32_t CPluginPay77::LogCreditStatus(const CMyString &orderNo, int32_t status, const CMyString &remark, int64_t creditTime, const CMyString &creditOp, char *szResp, uint32_t &cbResp)
{
    int32_t retval(-1);
    Json::Value resp;
    resp["code"] = -1000;
    resp["message"] = "store to database failure";
    m_pDB->Cmd("call pWithdrawCredit('%s', %d, '%s', %ld, '%s', '%s')",
            orderNo.c_str(), status, remark.c_str(), creditTime, creditOp.c_str(), 
            m_sdkCode.c_str());
    while(m_pDB->More())
    {
        retval = m_pDB->GetInt(0);
        resp["code"] = retval;
        resp["message"] = m_pDB->GetString(1);
    }
    CHECK_MYSQL_ERROR(err);
    dealResp(resp, szResp, cbResp);
    if (retval != 0)
    {
        return -1;
    }
    return 0;
}

int32_t CPluginPay77::LogLoanStatus(const CMyString &orderNo, int32_t status, char *szResp, uint32_t &cbResp)
{
    int32_t retval(-1);
    Json::Value resp;
    resp["code"] = -1000;
    resp["message"] = "store to database failure";
    m_pDB->Cmd("call pWithdrawCreditSDK('%s', %d)",
            orderNo.c_str(), status );
    while(m_pDB->More())
    {
        retval = m_pDB->GetInt(0);
        resp["code"] = retval;
        resp["message"] = m_pDB->GetString(1);
    }
    CHECK_MYSQL_ERROR(err);
    dealResp(resp, szResp, cbResp);
    if (retval != 0)
    {
        return -1;
    }
    return 0;
}

int32_t CPluginPay77::NotifyGamesvr(const CMyString &gameCode, const CMyString &uin, const CMyString &orderNo, int64_t fee, int32_t step, int32_t result, CGame *pGame)
{
    // 查询orderNo 的游戏服务器订单号
    CMyString gameOrderNo;
    m_pDB->Cmd("SELECT `third_order` FROM `t_withdraw` WHERE `order_id`='%s';", orderNo.c_str());
    while(m_pDB->More())
    {
        gameOrderNo = m_pDB->GetString(0);
    }

    // 构建请求数据
    http::CHttpRespMaker resp_maker;
    resp_maker.add("gameCode", gameCode);
    resp_maker.add("uin", uin);
    resp_maker.add("orderNo", orderNo);
    resp_maker.add("gameOrderNo", gameOrderNo);
    resp_maker.add("fee", fee);
    resp_maker.add("step", step);   //放款阶段
    resp_maker.add("result", result);
    CMyString str1 = resp_maker.http_str();
    CMyString str2("%s&key=%s", str1.c_str(), pGame->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeLower();
    resp_maker.add("sign", strMd5);
    CMyString strSign = resp_maker.json_str();

    LogDebug("before key : [%s]", str1.c_str());
    LogDebug("add signkey: [%s]", str2.c_str());
    LogDebug("md5:         [%s]", strMd5.c_str());
    CMyString strUrl = pGame->getApiTranferUrl();
    LogDebug("url:         [%s]", strUrl.c_str());
    LogDebug("data:        [%s]", strSign.c_str());

    // 发送Http请求
    CMyString strNotifyResp;
    CMyString strHeader;
    CMyString strContentType = "Content-Type:application/json";
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strNotifyResp, strHeader, header_list, strSign.c_str() );
    if (ret != 0)
    {
        CMyString strErr;
        m_http.getError(ret, strErr);
        CMyString strNote("用户(uin=%s)提现审核%ld成功，通知游服失败，请安排人工操作，错误信息:%s", 
                uin.c_str(), fee/100, strErr.c_str()); 
        this->postAlert(pGame, strNote);

        // 远程连接失败
        LogFatal("[%s:%s:%d]uin %s withdraw-audit %ld succeed, but write to gamesvr failure, orderNo:%s, url:%s, error:%s",
                __FILE__, __func__, __LINE__, uin.c_str(), fee/100,
                orderNo.c_str(), strUrl.c_str(), strErr.c_str());
        return err_PayDataSendtoGameFailure;
    }
    return 0;
}

ENTRY_FUNC_DECLARE(CPluginPay77, doWithdrawCallback)
{
    // TODO: 订单回调
    return 0;
}

ENTRY_FUNC_DECLARE(CPluginPay77, doWithdrawOrderInquiry)
{
    ext_headers["Content-Type"] = "application/json";
    CGame *pGame(NULL);

    bool signEnable(false);
    CMyString signField;
    this->getSignInfo("main/paycenter/sign/withdraw", signEnable, signField);
    // 检查参数合法
    std::vector<CMyString> vecParam;
    vecParam.push_back("gameCode");
    vecParam.push_back("orderNo");
    int32_t code = this->valid_req(req, szResp, cbResp, vecParam, 
            signEnable, signField, true, "gameCode", pGame);
    if (0 != code)
    {
        return code;
    }

    // 获得数据
    CMyString gameCode = req.getParam("gameCode");
    CMyString orderNo = req.getParam("orderNo");
    CMyString uin;
    bool bFind(false);
    m_pDB->Cmd("SELECT `uuid` FROM `t_withdraw` WHERE `order_id`='%s';", orderNo.c_str());
    while(m_pDB->More())
    {
        uin = m_pDB->GetString(0);
        bFind = true;
    }
    CHECK_MYSQL_ERROR(err);
    if (!bFind)
    {
        Json::Value resp;
        resp["code"] = -1;
        resp["message"] = CMyString("orderNo %s not found", orderNo.c_str());
        dealResp(resp, szResp, cbResp);
        return -1;
    }
    CSdk *pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
    if (NULL == pSdk)
    {
        Json::Value resp;
        resp["code"] = -5;
        resp["message"] = CMyString("invalid gameCode : %s", gameCode.c_str());
        dealResp(resp, szResp, cbResp);
        return -5;
    }

    // 检查是否有该订单
    CMyString strError;
    CMyString strOrderResp;
    int32_t state(-1);
    int32_t result = this->CheckCreditOrder(pGame, pSdk->getWithdrawCheckUrl(),
            pSdk->getAccount(), uin, orderNo, pSdk->getApiKey(),
            strError, strOrderResp, state);
    if (result != 0)
    {
        Json::Value resp;
        resp["code"] = -7;
        resp["message"] = strError;
        dealResp(resp, szResp, cbResp);
        return -4;
    }

    safe_strcpy(szResp, cbResp, strOrderResp.c_str());
    cbResp = strlen(szResp);

    return 0;
}

int32_t CPluginPay77::dealRechargeResp(http::CHttpRespone &http_resp, 
        CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo, 
        CSdkChannel *pChannel, int64_t payMoney)
{
/*
 * HEADER:
 * HTTP/1.1 200 OK
 * Server: nginx
 * Date: Fri, 08 Jan 2021 10:56:45 GMT
 * Content-Type: text/html;charset=utf-8
 * Transfer-Encoding: chunked
 * Connection: keep-alive
 * Vary: Accept-Encoding
 * Set-Cookie: PHPSESSID=ddh37ie0q5kgoqtvfblap4j7e5; path=/
 * Expires: Thu, 19 Nov 1981 08:52:00 GMT
 * Cache-Control: no-store, no-cache, must-revalidate
 * Pragma: no-cache
 * Refresh: 0;url=http://vi.long77.net/gateway/pay/automaticBank.do?id=2750920210108100575
 *
 * <若没有Refresh字段，则表示出错>
 *
 * DATA:
 * 请稍后..
 *
 * */    

    platformOrderNo = "N/A";
    CMyString strUrl = http_resp.getHeader("Refresh");
    if (strUrl.empty())
    {
        CByteStream buff = http_resp.getBuff();
        buff.Read(strError);

        // STEP1: 记录错误日志
        LogErr("[%s:%s:%d]Pay77 occur error [%s] ", __FILE__, __func__, __LINE__, strError.c_str());

        // STEP2：IM告警
        CMyString strNote("Pay77支付调起失败，错误信息：%s", strError.c_str());
        CPayWrapper::postAlert(pChannel->getGame(), strNote);

        // STEP3: 发送给前端
        CMyString strClientMsg;
        int32_t deal = CPayWrapper::dealRespError(pChannel, payMoney, strError, strClientMsg);

        return -1023;
    }

    // 解析 Refresh
    CMyString str2(strUrl);
    CStrSpliter spliter;
    if (spliter.Split(strUrl, ';'))
    {
        for(int32_t i(0); i<spliter.size(); ++i)
        {
            CMyString str1 = spliter.at(i);
            if (str1.find("url=") != std::string::npos)
            {
                str2 = str1;
                break;
            }
        }
    }
    std::string::size_type pos = str2.find("url=");
    if (pos != std::string::npos)
    {
        str2.erase(pos, 4);
    }

    // 获得回调的URL
    strTransferUrl = str2;   
    
    return 0;
}

int32_t CPluginPay77::dealCreditSyncResp(CMyString &strResp, CMyString &strError, CMyString &platformOrderNo)
{
    platformOrderNo= "N/A";
    CJsonWrapper jr;
    if (!jr.Init(strResp.c_str()))
    {
        strError = "invalid json string";
        return -1;
    }
    if (jr.getInt("code") != 200)
    {
        strError = jr.getString("msg");
        return -2;
    }

    Json::Value data;
    if (jr.getJsonObject("data", data))
    {
        platformOrderNo = data["outwithdrawno"].asString();
    }
    return 0;
}

int32_t CPluginPay77::valid_req(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::vector<CMyString> &vecParam, bool bSign, const CMyString &signField, bool bSignFullParam, const CMyString &gameCodeField, CGame *&pGame)
{
    int32_t code(0);
    CMyString strMsg;

    if (bSign)
    {
        vecParam.push_back(signField);
    }
    // 检查参数是否有缺省，或空值
    if (!req.ValidParam(vecParam, strMsg))
    {
        code = 401;
        safe_sprintf(szResp, cbResp, "{\"code\":%d, \"message\":\"%s\"}", code, strMsg.c_str());
        cbResp = strlen(szResp);
        return code;
    }

    // 检查游戏code是拾法性
    CMyString gameCode = req.getParam(gameCodeField);
    pGame = this->getGame(gameCode);
    if (NULL == pGame)
    {
        strMsg.Format("无此游服代码 %s", gameCode.c_str());
        Json::Value resp;
        resp["code"] = PAYFRAME_ERROR(err_InvalidGamesvr);
        resp["message"] = CMyString("not found gamesvr %s", gameCode.c_str());
        dealResp(resp, szResp, cbResp);
        return err_HttpOK;
    }
    
    // 验签
    if (bSign)
    {
        CMyString strSign = req.getParam(signField);

        http::CHttpRespMaker reqVerify;
        if (bSignFullParam)
        {
            std::vector<CMyString> vecExcept;
            vecExcept.push_back(signField);
            req.BuildRespMaker_Except(vecExcept, reqVerify);
        }
        else
        {
            // 移除sign
            std::vector<CMyString>::iterator it = std::find(vecParam.begin(), vecParam.end(), signField);
            vecParam.erase(it);
            req.BuildRespMaker(vecParam, reqVerify);
        }
        CMyString str1 = reqVerify.http_str();
        CMyString str2("%s&merchantKey=%s", str1.c_str(), pGame->getApiKey().c_str());
        CMyString strMD5 = CMD5Wrapper::encode(str2.c_str());
        strMD5.MakeLower();
        if (strMD5 != strSign)
        {
            code = 401;
            LogWarn("sign text [%s], sign md5 [%s], client md5 [%s]", str2.c_str(), strMD5.c_str(), strSign.c_str());
            safe_sprintf(szResp, cbResp, "{\"code\":%d, \"message\":\"%s\"}", code, "验签失败");
            cbResp = strlen(szResp);
            return code;
        }
    }

    return 0;
}

int32_t CPluginPay77::CheckCreditOrder(CGame *pGame, const CMyString &strUrl, const CMyString &pid, const CMyString &uin, const CMyString &orderNo, const CMyString &key, CMyString &strError, CMyString &strResp, int32_t &state)
{
    http::CHttpRespMaker req_maker;
    req_maker.add("pid", pid);
    req_maker.add("outwithdrawno", orderNo);
    int32_t nReqTime = time(NULL);
    req_maker.add("reqtime", nReqTime);
    CMyString str1 = req_maker.http_str_manual('&', '=', false, false);
    CMyString str2("%s&%s", str1.c_str(), key.c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeLower();
    req_maker.add("sign", strMd5);
    CMyString str3 = req_maker.http_str_manual('&', '=', false, true);

    CMyString strContentType("Content-Type: application/x-www-form-urlencoded");
    LogDebug("[pay77 credit text] %s", str1.c_str());
    LogDebug("[pay77 credit key ] %s", str2.c_str());
    LogDebug("[pay77 credit md5 ] %s", strMd5.c_str());
    LogDebug("[pay77 credit sign] %s", str3.c_str());

    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    LogDebug("[pay77 credit url] %s", strUrl.c_str());
    CMyString strHeader;
    // CMyString strResp;
    int32_t result(-1);
    do {
        int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, str3.c_str());
        if (ret != 0)
        {
            CMyString strMsg;
            m_http.getError(ret, strMsg);
            CMyString strNote("用户(uin=%s)调起PAY77付款失败，错误码:%d, 错误说明:%s", uin.c_str(), ret, strMsg.c_str() );
            CPayWrapper::postAlert(pGame, strNote);

            strError.Format("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("[PAY77 CREDIT HEADER] %s", strHeader.c_str());
        LogWarn("[PAY77 CREDIT REP   ] %s", strResp.c_str());
        http::CHttpRespone credit_resp;
        credit_resp.read((void*)strHeader.c_str(), strHeader.size());
        credit_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealCreditCheckResp(credit_resp, strResp, strError, state);
    }while(0);

    return result;
}

int32_t CPluginPay77::dealCreditCheckResp(http::CHttpRespone &resp, const CMyString &strResp, CMyString &strError, int32_t &result)
{
    result = -1;
    int32_t status = resp.getStatusCode();
    if (status != 200 && status != 0)
    {
        // 返回的状态码不对
        strError.Format("resp code %d invalid", status);
        return -1;
    }

    CJsonWrapper jr;
    if (!jr.Init(strResp.c_str()))
    {
        // 返回的串不是json格式
        strError.Format("resp [%s] not valid json", strResp.c_str());
        return -2;
    }

    CMyString strMsg = jr.getString("msg");
    int32_t code = jr.getInt("code");
    if (code == 0)
    {
        // 无此订单号
        Json::Value data;
        if (jr.getJsonObject("data", data))
        {
            strError.Format("%s, orderNo:%s", strMsg.c_str(), data["outwithdrawno"].asString().c_str());
        }
        else
        {
            strError = strMsg;
        }
        result = 0;
        return 0;
    }
    else if (-1 == code || code != 200)
    {
        strError = strMsg;
        return -3;
    }
    
    Json::Value data;
    if (!jr.getJsonObject("data", data))
    {
        // 数据格式不正确，没有data字段
        strError.Format("data field invalid, resp[%s]", strResp.c_str());
        return -4;
    }

    // 处理成功，依据result来判断结果
    // 1:待处理;2:已付款;3:已驳回;4:订单异常;5:付款中;6:代付失败
    result = data["is_state"].asInt();
    return 0;
}

int32_t CPluginPay77::dealCreditOrder(const CMyString &creditData)
{
    CJsonWrapper jr;
    if (!jr.Init(creditData.c_str()))
    {
        // JSON 数据不合法
        return -1;
    }
    
    CMyString gameCode = jr.getString("gameCode");
    if (gameCode.empty())
    {
        return -3;
    }
    CGame *pGame = CPayWrapper::getGame(gameCode); 
    if (NULL == pGame)
    {
        return -5;
    }

    CMyString orderNo = jr.getString("orderNo");
    CMyString uin = jr.getString("uin");
    int64_t fee = jr.getInt64("fee");
    CMyString creditKey("credit-pay77#keydata#order_%s", orderNo.c_str());
    CMyString strError;
    CMyString strOrderResp;
    int32_t state(-1);
    int32_t result = this->CheckCreditOrder(pGame, jr.getString("url"),
            jr.getString("pid"), uin, orderNo,
            jr.getString("key"), strError, strOrderResp, state);

    if (result != 0)
    {
        return -7;
    }

    int32_t status(-1);
    switch(state)
    {
    case creditNotFound:
        m_redis.Del(creditKey);
        break;
    case creditPending:
    case creditPaying:
        // 下一次循环处理
        break;
    case creditPaied:
        status = 10;    // 支付完毕
        m_redis.Del(creditKey);
        break;
    case creditReject:
        status = 11;    // 拒绝
        m_redis.Del(creditKey);
        break;
    case creditFailed:
        status = 12;    // 失败
        m_redis.Del(creditKey);
        break;
    case creditCorrupted:
        status = 13;    //订单错误
        m_redis.Del(creditKey);
        break;
    default:
        break;
    }

    if (status <=0)
    {
        return 0;
    }

    // 通知游戏服务器
    int32_t gameResult = getGamesvrResult(status);
    if (gameResult >0)
    {
        LogWarn("orderNo [%s], status[%d], result[%d]", orderNo.c_str(), status, gameResult);
        this->NotifyGamesvr(gameCode, uin, orderNo, fee, 3, gameResult, pGame);
    }

    // 记录到数据库
    char szResp[65535] = "";
    uint32_t cbResp = sizeof(szResp);
    result = this->LogLoanStatus(orderNo, status, szResp, cbResp);

    return 0;
}

bool CPluginPay77::ValidIp(const CMyString &strIp, const CMyString &sect_enable, const CMyString &sect_ips)
{
    CXmlFile &xml = CONFIG.getXml();
    bool bEnable = xml.GetBool(sect_enable.c_str(), "enable");
    if (!bEnable)
    {
        // 禁用IP检查，所有IP都合法
        return true;
    }

    CMyString strIps = xml.GetString(sect_ips.c_str(), "valid");
    CStrSpliter css;
    css.Split(strIps, ',');
    if (css.empty())
    {
        // 未配置允许IP，所有IP都合法
        return true;
    }

    bool bValid(false);
    for(int32_t i(0); i<css.size(); ++i)
    {
        CMyString strIp1 = css.asString(i);
        if (strIp1=="0.0.0.0")
        {
            // 放过所有IP
            bValid = true;
            break;
        }
        if (strIp1 == strIp)
        {
            // IP匹配
            bValid = true;
            break;
        }
    }
    return bValid;
}

void CPluginPay77::getSignInfo(const CMyString &sect, bool &bEnable, CMyString &field)
{
    CXmlFile &xml = CONFIG.getXml();
    bEnable = xml.GetBool(sect.c_str(), "enable");
    field = xml.GetString(sect.c_str(), "field");
}
