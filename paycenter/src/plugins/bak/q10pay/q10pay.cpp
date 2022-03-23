/*
 * =====================================================================================
 *
 *       Filename:  q10pay.cpp
 *
 *    Description:  q10pay 接口实现
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
#include "q10pay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"
#include <algorithm>

const CMyString SIGN_TYPE = "MD5";
enum Q10PAY_RESP_CODE
{
    Q10PAY_RESP_CODE_200 = 200,     //Request Success
    Q10PAY_RESP_CODE_400 = 400,     //Bad Request
};
const CMyString PRODUCT_THAIPAYOUT = "ThaiPayout";
const CMyString PRODUCT_THAISETTLEMENT = "ThaiSettlement";


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
    // status=>                  1  2  3  4  5  6  7  8   9  10  11  12  13
    static int32_t s_result[] = {0, 9, 8, 0, 0, 1, 9, 8, -1,  2,  9,  8,  9};

    if (status<=0 || status>=(int32_t)(sizeof(s_result)/sizeof(int32_t)) )
    {
        return -1;
    }
    return s_result[status-1];
}

/////////////////////////////////////////////////////////////////////
// CPluginQ10pay
CPluginQ10pay::CPluginQ10pay()
    : CPayWrapper("Q10Pay")
{
    //mkdir("./html/v8", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginQ10pay::~CPluginQ10pay()
{
}

int32_t CPluginQ10pay::Init(void)
{
    return this->Update();
}

int32_t CPluginQ10pay::Update(void)
{
    CMyString strSect("main/q10pay");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    m_timer.SetInterval(10*1000);
    m_timer.Startup();
    return 0;
}

void CPluginQ10pay::Maintance()
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

#define HTTP_REQ_STR(var, param)    CMyString var=req.getParam(#param)
#define HTTP_REQ_INT32(var, param)  int32_t var=req.getParamInt(#param)
#define HTTP_REQ_INT64(var, param)  int64_t var=req.getParamInt64(#param)
// 以下为调用网银接口
ENTRY_FUNC_DECLARE(CPluginQ10pay, doPay)
{
    int32_t nOrderId(0);
    
    //支付中心的订单号
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
    pClient->setIp(strIp);

    // 获得相关参数
    CSdk *pSdk = pChannel->getSdk();
    CGame *pGame = pChannel->getGame();
    HTTP_REQ_INT64(payMoney, amount);
    HTTP_REQ_STR(accountNo, bankCard);
    HTTP_REQ_STR(bankCode, bankCode);
    HTTP_REQ_STR(strUin, uin);

    //json串拼接
    Json::Value jsonParam;
    jsonParam["merchant_ref"] = strOrderNo;
    jsonParam["product"] = pChannel->getBankCode();
    jsonParam["amount"] = CMyString("%.2lf", payMoney/100.0);
    Json::Value jsonExtra;
    jsonExtra["account_no"] = accountNo;
    jsonExtra["bank_code"] = bankCode;
    jsonParam["extra"] = jsonExtra;
    // NOTE： jsonParam 不排序
    CMyString strParams = jsonParam.toStyledString();
    strParams.Replace("\n", "");
    strParams.Replace("\r", "");
    strParams.Replace(" ", "");

    int32_t nReqTime = time(NULL);
    CMyString strOrigin("%s%s%s%d%s", 
            pSdk->getAccount().c_str(), 
            strParams.c_str(), 
            SIGN_TYPE.c_str(), 
            nReqTime, 
            pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(strOrigin.c_str());
    strMd5.MakeLower();

    http::CHttpRespMaker req_data;
    req_data.add("merchant_no", pSdk->getAccount());
    req_data.add("timestamp", nReqTime);
    req_data.add("sign_type", SIGN_TYPE);
    req_data.add("sign", strMd5);
    req_data.add("params", strParams);
    CMyString strSign = req_data.http_str_manual('&', '=', false, false);
    LogDebug("[Q10-pay]origin text[%s], md5[%s]", strOrigin.c_str(), strMd5.c_str());
    LogDebug("[Q10-pay]send text[%s]", strSign.c_str());

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
    CMyString strUrl("%s", pSdk->getPayUrl().c_str());
    int64_t timeCurlBegin = Now(true);
    do {
        ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            CMyString strMsg;
            m_http.getError(ret, strMsg);
            CMyString strNote("用户(uin=%s)调起q10pay支付失败，错误码:%d, 错误说明:%s", strUin.c_str(), ret, strMsg.c_str() );
            CPayWrapper::postAlert(pGame, strNote);

            strError.Format("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }

        LogWarn("[Q10-pay resp]header [%s]", strHeader.c_str());
        LogWarn("[Q10-pay resp]data   [%s]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealJsonResp(strResp, strTransferUrl, strError, platformOrderNo, pChannel, payMoney);
    } while(0);
    int64_t timeCurlEnd = Now(true);
    LogDebug("[STAT]curl to [%s] cost %.2lfms", strUrl.c_str(), (timeCurlEnd-timeCurlBegin)/1000.0);

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

ENTRY_FUNC_DECLARE(CPluginQ10pay, doCallback)
{
    LogDebug("[q10pay] doCallback, req_url=%s", req.getFullUrl().c_str());

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);

    // 校验回调IP
    //CMyString strIp = getClientIp(pClient, req);
    CMyString strIp = getClientIpEx(getIpFromHeader, pClient, req, "X-Real-IP");
    LogImpt("q10pay-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    pClient->setIp(strIp);

    //q10pay如果使用在线测试回调，params参数里带有+号，无法解析json，这里过滤掉+号
    CMyString strParams = req.getParam("params");
    CMyString strDecode;
    CHttpWrapper::urlDecode(strParams, strDecode);
    strDecode.Replace("+", "");
    CJsonWrapper jsonParams;
    jsonParams.Init(strDecode.c_str());
    CMyString strOrderNo = jsonParams.getString("merchant_ref");
    LogDebug("[q10pay] orderNo=%s", strOrderNo.c_str());
    CPayWrapper::freeOrderExpired(strOrderNo);

    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        int32_t result = PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
        CMyString strNote("q10pay callback ip [%s] invalid", strIp.c_str());
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
    CMyString strPlatformId = jsonParams.getString("system_ref");
    CMyString status = jsonParams.getString("status");
    //存款订单状态：0：Unpaid；1：Paid
    int32_t code(payCallbackFail);
    if (status != "1")
    {
        code = payCallbackFail;
    }
    else
    {
        code = payCallbackOK;
    }

    CMyString strFee = jsonParams.getString("amount");
    CMyString strRealFee = jsonParams.getString("pay_amount");
    double realFee = strRealFee.asDouble();
    int64_t nRealFee = realFee * 100; // 转换成分

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, strCallbackResp, code, nRealFee);
    LogImpt("[q10pay] _deal_notify return %d", ret);

    // 回传到回调方
    safe_strcpy(szResp, cbResp, "success");
    cbResp = strlen(szResp);
    return ret;
}

int32_t CPluginQ10pay::ValidPayCallbackSign(http::CHttpRequest &req, const CMyString &strOrderNo, char *szResp, uint32_t &cbResp)
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

    int32_t nReqTime = req.getParamInt("timestamp");
    CMyString strParams = req.getParam("params");
    CMyString strDecode;
    CHttpWrapper::urlDecode(strParams, strDecode);
    strDecode.Replace("+", " ");

    CMyString strOrigin("%s%s%s%d%s", pSdk->getAccount().c_str(), strDecode.c_str(), SIGN_TYPE.c_str(), nReqTime, pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(strOrigin.c_str());
    strMd5.MakeLower();
    LogDebug("[q10pay verify sign]origin text:[%s], md5[%s]", 
            strOrigin.c_str(), strMd5.c_str());

    CMyString strMd5Valid = req.getParam("sign");
    if (strMd5 != strMd5Valid)
    {
        LogErr("q10pay resp sign [%s], yet verify sign is [%s]",
                strMd5Valid.c_str(), strMd5.c_str());
        int32_t result = PAYFRAME_ERROR(err_SignMismatch);    // 验签失败
        CMyString strNote("q10pay callback sign mismatch");
        CPayWrapper::OnPayFailure(result, strNote, szResp, cbResp);
        return result;
    }

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginQ10pay, doWithdraw)
{
    LogDebug("[q10pay] doWithdraw, req_url=%s", req.getFullUrl().c_str());

    ext_headers["Content-Type"] = "application/json";
    CGame *pGame(NULL);

    bool signEnable(false);
    CMyString signField;
    this->getSignInfo("main/paycenter/sign/report", signEnable, signField);
    // 校验传入参数是否合法
    std::vector<CMyString>  vecParam;
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    vecParam.push_back("bankName");
    vecParam.push_back("token");
    vecParam.push_back("orderNo");
    vecParam.push_back("fee");
    vecParam.push_back("bankCard");
    vecParam.push_back("bankCode");
    int32_t code = this->valid_req(req, szResp, cbResp, vecParam, 
            signEnable, signField, true, "gameCode", pGame);
    if (0 != code)
    {
        return code;
    }

    // 检查发起的IP是否合法
    CMyString strIp = getClientIpEx(getIpFromHeader, pClient, req, "X-Real-IP");
    if (!this->ValidIp(strIp,"main/q10pay/ip", "main/q10pay/ip/withdraw"))
    {
        Json::Value resp;
        resp["code"] = -1;
        resp["message"] = CMyString("ip %s 不合法", strIp.c_str());
        dealResp(resp, szResp, cbResp);
        return -1;
    }
    pClient->setIp(strIp);

    // 获得相关参数
    CMyString gameCode = req.getParam("gameCode");
    CMyString uin = req.getParam("uin");
    CMyString bankName = req.getParam("bankName");
    CMyString creditToken = req.getParam("token");
    //商户订单号
    CMyString orderNo = req.getParam("orderNo");
    //第三方平台：金额，单位为元，保留两位小数。游服：单位应该是分
    int64_t fee = req.getParamInt64("fee");
    //银行卡号
    CMyString bankCard = req.getParam("bankCard");
    //提现银行代码
    CMyString bankCode = req.getParam("bankCode");

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

    // 向第三方服务器发起请求
    int64_t timeBegin = Now();
    int32_t result = this->dealCreditRequest(pSdk, pGame, fee, bankCard, bankCode, bankName, uin, orderNo, szResp, cbResp);
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
    CMyString creditKey("credit-q10pay#keydata#order_%s", orderNo.c_str());
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

int32_t CPluginQ10pay::dealCreditRequest(CSdk *pSdk, CGame *pGame, int64_t fee, const CMyString &bankCard, const CMyString &bankCode, const CMyString &bankName, const CMyString &uin, const CMyString &orderNo, char *szResp, uint32_t &cbResp)
{
    int32_t nReqTime = time(NULL);
    //json串拼接
    Json::Value jsonRoot;
    jsonRoot["merchant_ref"] = orderNo;
    //产品【ThaiPayout，ThaiSettlement】，提现的范围，跟手续费，不一样。
    CMyString product = PRODUCT_THAIPAYOUT;
    jsonRoot["product"] = product;
    // 需要将元转换成分，再向平台请求支付
    fee /= 100;
    jsonRoot["amount"] = CMyString("%ld", fee);
    jsonRoot["account_name"] = uin;
    jsonRoot["account_no"] = bankCard;
    jsonRoot["bank_code"] = bankCode;

    CMyString jsonStr = jsonRoot.toStyledString();

    // 生成sign
    CMyString str1 = jsonStr;
    //去掉回车和空格
    str1.Replace("\n", "");
    str1.Replace("\r", "");
    str1.Replace(" ", "");
    //MD5(merchant_no+params+sign_type+timestamp+Key)
    CMyString str2("%s%s%s%d%s", pSdk->getAccount().c_str(), str1.c_str(), SIGN_TYPE.c_str(), nReqTime, pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());   
    strMd5.MakeLower();

    http::CHttpRespMaker reqData;
    reqData.add("merchant_no", pSdk->getAccount());
    reqData.add("timestamp", nReqTime);
    reqData.add("sign_type", SIGN_TYPE);
    reqData.add("sign", strMd5);
    reqData.add("params", str1);

    CMyString strContentType("Content-Type: application/x-www-form-urlencoded");
    CMyString str3 = reqData.http_str_manual('&', '=', false, false);
    //CMyString strContentType("Content-Type: application/json");
    //CMyString str3 = req_credit.json_str();
    LogDebug("[q10pay origin text] %s", str1.c_str());
    LogDebug("[q10pay key text   ] %s", str2.c_str());
    LogDebug("[q10pay md5 text   ] %s", strMd5.c_str());
    LogDebug("[q10pay sign text  ] %s", str3.c_str());

    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    CMyString strUrl(pSdk->getWithdrawUrl());
    LogDebug("[q10pay url] %s", strUrl.c_str());
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
            CMyString strNote("用户(uin=%s)调起q10pay付款失败，错误码:%d, 错误说明:%s", uin.c_str(), ret, strMsg.c_str() );
            CPayWrapper::postAlert(pGame, strNote);

            strError.Format("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("[q10pay WITHDRAW HEADER] %s", strHeader.c_str());
        LogWarn("[q10pay WITHDRAW REP   ] %s", strResp.c_str());
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

int32_t CPluginQ10pay::LogCreditStatus(const CMyString &orderNo, int32_t status, const CMyString &remark, int64_t creditTime, const CMyString &creditOp, char *szResp, uint32_t &cbResp)
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

int32_t CPluginQ10pay::LogLoanStatus(const CMyString &orderNo, int32_t status, char *szResp, uint32_t &cbResp)
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

int32_t CPluginQ10pay::NotifyGamesvr(const CMyString &gameCode, const CMyString &uin, const CMyString &orderNo, int64_t fee, int32_t step, int32_t result, CGame *pGame)
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

ENTRY_FUNC_DECLARE(CPluginQ10pay, doWithdrawCallback)
{
    LogDebug("[q10pay] doWithdrawCallback, req_url=%s", req.getFullUrl().c_str());

    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);

    // 校验回调IP
    //CMyString strIp = getClientIp(pClient, req);
    CMyString strIp = getClientIpEx(getIpFromHeader, pClient, req, "X-Real-IP");
//    LogInfo("q10pay-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("q10pay-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    pClient->setIp(strIp);
/*    
    if (!this->ValidIp(strClientIp,"main/q10pay/ip", "main/q10pay/ip/recharge"))
    {
        Json::Value resp;
        resp["code"] = -1;
        resp["message"] = CMyString("ip %s 不合法", strClientIp.c_str());
        dealResp(resp, szResp, cbResp);
        return -1;
    }
*/    

    //q10pay如果使用在线测试回调，params参数里带有+号，无法解析json。
    CMyString strParams = req.getParam("params");
    CMyString strDecode;
    CHttpWrapper::urlDecode(strParams, strDecode);
    strDecode.Replace("+", "");
    CJsonWrapper jsonParams;
    jsonParams.Init(strDecode.c_str());
    CMyString strOrderNo = jsonParams.getString("merchant_ref");
    LogDebug("[q10pay] orderNo=%s", strOrderNo.c_str());

//    CGame *pGame(NULL);
//    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
//    if (ret < 0)
//    {
//        int32_t result = PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
//        CMyString strNote("q10pay callback ip [%s] invalid", strIp.c_str());
//        CPayWrapper::OnPayFailure(result, strNote, szResp, cbResp);
//        return result;
//    }
//    // 验签
//    int32_t result = ValidPayCallbackSign(req, strOrderNo, szResp, cbResp);
//    if (result != 0)
//    {
//        return result;
//    }

    CMyString withdrawKey("credit-q10pay#keydata#order_%s", strOrderNo.c_str());
    //出金订单状态：1：Success；2：Pending；5：Reject
    CMyString status = jsonParams.getString("status");
    LogDebug("[q10pay] status=%s", status.c_str());
    int32_t withdrawOrderStatus = -1;
#if 0
    if (status == "1")
    {
        withdrawOrderStatus = 10;    // 支付完毕
        std::string creditData;
        int32_t ret = m_redis.Get(withdrawKey, creditData);
        if (ret == 1)
        {
            LogDebug("[q10pay] redis key exist withdrawKey=%s", withdrawKey.c_str());
            // 通知游戏服务器
            int32_t gameResult = getGamesvrResult(withdrawOrderStatus);
            if (gameResult > 0)
            {
                CJsonWrapper jr;
                jr.Init(creditData.c_str());        
                CMyString gameCode = jr.getString("gameCode");
                CMyString uin = jr.getString("uin");
                int64_t fee = jr.getInt64("fee");

                CGame *pGame = CPayWrapper::getGame(gameCode); 
                if (NULL != pGame)
                {
                    LogWarn("orderNo [%s], status[%s], result[%d]", strOrderNo.c_str(), status.c_str(), gameResult);
                    this->NotifyGamesvr(gameCode, uin, strOrderNo, fee, 3, gameResult, pGame);
                }
            }

            // 记录到数据库
            char szResp[65535] = "";
            uint32_t cbResp = sizeof(szResp);
            this->LogLoanStatus(strOrderNo, withdrawOrderStatus, szResp, cbResp);

            m_redis.Del(withdrawKey.c_str());
        }
        //redis没找到相关提现订单key
        else
        {
            LogWarn("redis not found withdraw orderNo[%s]", strOrderNo.c_str());
        }
    }
    else if (status == "2")
    {
        withdrawOrderStatus = 12;    // 失败
        m_redis.Del(withdrawKey.c_str());
    }
    else if (status == "5")
    {
        withdrawOrderStatus = 11;    // 拒绝
        m_redis.Del(withdrawKey.c_str());
    }
#else
    if (status=="1")
    {
        withdrawOrderStatus = 10; // 扣款
    }
    else
    {
        withdrawOrderStatus = 11; // 原路退款
    }

    std::string creditData;
    int32_t ret = m_redis.Get(withdrawKey, creditData);
    if (ret == 1)
    {
        LogDebug("[q10pay] redis key exist withdrawKey=%s", withdrawKey.c_str());
        // 通知游戏服务器
        int32_t gameResult = getGamesvrResult(withdrawOrderStatus);
        if (gameResult > 0)
        {
            CJsonWrapper jr;
            jr.Init(creditData.c_str());        
            CMyString gameCode = jr.getString("gameCode");
            CMyString uin = jr.getString("uin");
            int64_t fee = jr.getInt64("fee");

            CGame *pGame = CPayWrapper::getGame(gameCode); 
            if (NULL != pGame)
            {
                LogWarn("orderNo [%s], status[%s], result[%d]", strOrderNo.c_str(), status.c_str(), gameResult);
                this->NotifyGamesvr(gameCode, uin, strOrderNo, fee, 3, gameResult, pGame);
            }
        }

        // 记录到数据库
        char szResp[65535] = "";
        uint32_t cbResp = sizeof(szResp);
        this->LogLoanStatus(strOrderNo, withdrawOrderStatus, szResp, cbResp);

        m_redis.Del(withdrawKey.c_str());
    }
    //redis没找到相关提现订单key
    else
    {
        LogWarn("redis not found withdraw orderNo[%s]", strOrderNo.c_str());
    }
#endif

    // 回传到回调方
    safe_strcpy(szResp, cbResp, "success");
    cbResp = strlen(szResp);
    return 0;
}

int32_t CPluginQ10pay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl,  CMyString &strError, CMyString &platformOrderNo, CSdkChannel *pChannel, int32_t payMoney)
{
    CJsonWrapper reader;
    if (!reader.Init(strResp.c_str()))
    {
        strError.Format("q10pay resp [%s] not valid json string", strResp.c_str());
        CGame *pGame = pChannel->getGame();
        CPayWrapper::postAlert(pGame, strError);

        // 返回串不合法
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1023;
    }

    CMyString code = reader.getString("code");
    if (code != "200")
    {
        CMyString strMsg = reader.getString("message");
        // STEP1: 发给IM
        CMyString strNote("q10pay pay failure, code:%s, msg:%s", code.c_str(), strMsg.c_str());
        CGame *pGame = pChannel->getGame();
        CPayWrapper::postAlert(pGame, strNote);

        // STEP2: 发给前端
        CMyString strClientMsg;
        int32_t deal = CPayWrapper::dealRespError(pChannel, payMoney,strMsg, strClientMsg);
        strError.Format("[q10pay]%s", strClientMsg.c_str());

        // STEP4: 记录日志
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strNote.c_str());
        return -1025;
    }

    // params 返回的是字符串，不是JSON串
    CMyString strParams = reader.getString("params");
    CJsonWrapper respParams;
    if (!respParams.Init(strParams.c_str()))
    {
        strError.Format("q10pay resp PARAMS [%s] not valid json string", strParams.c_str());
        CGame *pGame = pChannel->getGame();
        CPayWrapper::postAlert(pGame, strError);

        // 返回串不合法
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
        return -1027;
    }
    strTransferUrl = respParams.getString("payurl");
    platformOrderNo = respParams.getString("system_ref");

    return 0;
}

int32_t CPluginQ10pay::dealCreditSyncResp(CMyString &strResp, CMyString &strError, CMyString &platformOrderNo)
{
    platformOrderNo= "N/A";
    CJsonWrapper jr;
    if (!jr.Init(strResp.c_str()))
    {
        strError = "invalid json string";
        return -1;
    }
    int32_t code = jr.getInt("code");
    if (Q10PAY_RESP_CODE_200 != code)
    {
        strError = jr.getString("message");
        return -2;
    }

    CMyString strParams = jr.getString("params");
    CJsonWrapper jsonParams;
    jsonParams.Init(strParams.c_str());
    platformOrderNo = jsonParams.getString("system_ref");

    return 0;
}

int32_t CPluginQ10pay::valid_req(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::vector<CMyString> &vecParam, bool bSign, const CMyString &signField, bool bSignFullParam, const CMyString &gameCodeField, CGame *&pGame)
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

int32_t CPluginQ10pay::CheckCreditOrder(CGame *pGame, const CMyString &strUrl, const CMyString &pid, const CMyString &uin, const CMyString &orderNo, const CMyString &key, CMyString &strError, CMyString &strResp, int32_t &state)
{
    int32_t nReqTime = time(NULL);
    //json串拼接
    Json::Value jsonRoot;
    Json::Value jsonOrderNos;
    jsonOrderNos.append(orderNo);
    jsonRoot["merchant_refs"] = jsonOrderNos;
    CMyString extendParams = "";
    jsonRoot["extend_params"] = extendParams;

    CMyString jsonStr = jsonRoot.toStyledString();

    // 生成sign
    CMyString str1 = jsonStr;
    //去掉回车和空格
    str1.Replace("\n", "");
    str1.Replace("\r", "");
    str1.Replace(" ", "");
    //MD5(merchant_no+params+sign_type+timestamp+Key)
    CMyString str2("%s%s%s%d%s", pid.c_str(), str1.c_str(), SIGN_TYPE.c_str(), nReqTime, key.c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeLower();

    http::CHttpRespMaker req_data;
    req_data.add("merchant_no", pid);
    req_data.add("timestamp", nReqTime);
    req_data.add("sign_type", SIGN_TYPE);
    req_data.add("sign", strMd5);
    req_data.add("params", str1);

    // CMyString strSign = req_data.json_str();
    CMyString strSign = req_data.http_str_manual('&', '=', false, false);
    //CMyString strSign("%s&sign=%s", str1.c_str(), strMd5.c_str());
    LogDebug("[q10pay]origin text[%s]", str1.c_str());
    LogDebug("[q10pay]key text   [%s]", str2.c_str());
    LogDebug("[q10pay]md5        [%s]", strMd5.c_str());
    LogDebug("[q10pay]signed text[%s]", strSign.c_str());

    CMyString strContentType("Content-Type: application/x-www-form-urlencoded");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    LogDebug("[q10pay credit url] %s", strUrl.c_str());
    CMyString strHeader;
    // CMyString strResp;
    int32_t result(-1);
    do {
        int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
        if (ret != 0)
        {
            CMyString strMsg;
            m_http.getError(ret, strMsg);
            CMyString strNote("用户(uin=%s)调起q10pay付款失败，错误码:%d, 错误说明:%s", uin.c_str(), ret, strMsg.c_str() );
            CPayWrapper::postAlert(pGame, strNote);

            strError.Format("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("[q10pay CREDIT HEADER] %s", strHeader.c_str());
        LogWarn("[q10pay CREDIT REP   ] %s", strResp.c_str());
        http::CHttpRespone credit_resp;
        credit_resp.read((void*)strHeader.c_str(), strHeader.size());
        credit_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealCreditCheckResp(credit_resp, strResp, strError, state);
    }while(0);

    return result;
}

int32_t CPluginQ10pay::dealCreditCheckResp(http::CHttpRespone &resp, const CMyString &strResp, CMyString &strError, int32_t &result)
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

    CMyString strMsg = jr.getString("message");
    int32_t code = jr.getInt("code");
    if (code != Q10PAY_RESP_CODE_200)
    {
        // 无此订单号
        CJsonWrapper data;
        if (jr.getReader("params", data))
        {
            strError.Format("%s, orderNo:%s", strMsg.c_str(), data.getString("merchant_ref").c_str());
        }
        else
        {
            strError = strMsg;
        }
        result = 0;
        return 0;
    }
    
    CJsonWrapper data;
    if (!jr.getReader("params", data))
    {
        // 数据格式不正确，没有data字段
        strError.Format("data field invalid, resp[%s]", strResp.c_str());
        return -4;
    }

    // 存款订单状态: 0：Unpaid；1：Paid；出金订单状态：1: Success； 2: Pending；5: Reject
    result = data.getInt("status");
    return 0;
}

bool CPluginQ10pay::ValidIp(const CMyString &strIp, const CMyString &sect_enable, const CMyString &sect_ips)
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

void CPluginQ10pay::getSignInfo(const CMyString &sect, bool &bEnable, CMyString &field)
{
    CXmlFile &xml = CONFIG.getXml();
    bEnable = xml.GetBool(sect.c_str(), "enable");
    field = xml.GetString(sect.c_str(), "field");
}
