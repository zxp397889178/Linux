/*
 * =====================================================================================
 *
 *       Filename:  ivnpay.cpp
 *
 *    Description:  ivnpay 接口实现
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
#include "ivnpay.h"
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

CPluginIvnPay::CPluginIvnPay()
    : CPayWrapper("Q10Pay")
{
    
}

CPluginIvnPay::~CPluginIvnPay()
{

}

int32_t CPluginIvnPay::Init(void)
{ 
    CMyString strSect("main/ivnpay");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    m_timer.SetInterval(10*1000);
    m_timer.Startup();
    return 0;
}

int32_t CPluginIvnPay::Update(void)
{
    return this->Init();
}

static const char* getWithdrawKeyPrefix(void)
{
    static CMyString strKey="credit-ivnpay#keydata#order_";
    return strKey.c_str();
}

void CPluginIvnPay::Maintance()
{
    CPayWrapper::Maintance();

    // 从redis中读取数据
    if (m_timer.ToNextTime())
    {
        // LogDebug("=====CPluginIvnPay======================");
        CMyString strKeys("%s*", getWithdrawKeyPrefix());
        std::vector<std::string> vecCreditOrder;
        m_redis.Keys(strKeys.c_str(), vecCreditOrder);
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

            // LogDebug("=====CPluginIvnPay===========%s===========", creditData.c_str());
            this->dealCreditOrder(creditData);
        }
    }
    
}

// 以下为调用网银接口
ENTRY_FUNC_DECLARE(CPluginIvnPay, doPay)
{
    int32_t nOrderId(0);
    CMyString strOrderNo;
    CSdkChannel *pChannel(NULL);
    int32_t ret = CPayWrapper::_before_send_request(req, pChannel, nOrderId, strOrderNo, szResp, cbResp);
    if (ret < 0 || NULL==pChannel)
    {
        return ret; 
    }

    // 获得用户的真实IP
    CSdk *pSdk = pChannel->getSdk();
    CGame* pGame =  pChannel->getGame();

    if (NULL==pSdk||NULL==pGame)
    {
        return -1;
    }

    int64_t payMoney(0);
    CMyString strProductId;
    CMyString strNick;
    CMyString strUin;
    req.getParam("productId", strProductId);
    req.getParam("amount", payMoney);
    req.getParam("userName", strNick);
    req.getParam("uin", strUin);

    // 生成发送到支付平台的数据
    http::CHttpRespMaker req_data;
    req_data.add("mch_id", pSdk->getAccount());
    req_data.add("mch_uid", strUin);
    req_data.add("mch_order_id", strOrderNo);
    req_data.add("equipment_type", 2);
    req_data.add("expected_amount", payMoney/100);
    req_data.add("mch_url", pGame->getCallbackUrl());
    req_data.add("attach", strProductId);

    // 生成sign
    CMyString strSignSrc = req_data.http_str();
    strSignSrc.Append("%s", pSdk->getApiKey().c_str());
    CMyString strSign = CMD5Wrapper::encode(strSignSrc.c_str());
    LogDebug("origin text[%s] sign[%s]", strSignSrc.c_str(), strSign.c_str());
    req_data.add("hash", strSign.c_str());

    CMyString strHeader = "";
    CMyString strResp = "";
    CMyString platformOrderNo = "";
    CMyString strReqData = req_data.json_str(true);

    LogDebug("reqdata text[%s]", strReqData.c_str());
    CMyString strReqParam = CBase64Wrapper::Encode(strReqData.c_str(), strReqData.length(), false);
    CMyString strTransferUrl("%s%s",pSdk->getPayUrl().c_str(), strReqParam.c_str());
    // 发送到第三方平台    

    time_t t1 = time(NULL);
    char szReqTime[64] = "";
    TimeToStr(t1, szReqTime, sizeof(szReqTime));
    CPayWrapper::_after_send_request(nOrderId, payReqSucceed, strSign, szReqTime, strHeader, strResp, platformOrderNo);

    // 记录支付的主键
    CPayWrapper::on_save_orderId(strOrderNo, nOrderId);
    // 记录支付对应的商户KEY
    CPayWrapper::on_save_order_channelId(strOrderNo, req);

    // 返回给调用端
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

int32_t UrlEncode2Json(const CMyString &str, Json::Value &resp)
{
    std::vector<KvstrItem> temp;
    int32_t ret = safe_strtok(str.c_str(), temp, "&", "=");
    if (ret <=0)
    {
        return -1;
    }

    for(std::vector<KvstrItem>::iterator it(temp.begin());
            it != temp.end(); ++it)
    {
        KvstrItem &item = *it;
        resp[item.key] = item.value;
    }
    return 0;
}

ENTRY_FUNC_DECLARE(CPluginIvnPay, doCallback)
{
    // 保存post数据
    CMyString str1= req.getParam("param");
    CMyString strIp = getClientIp(pClient, req);
    CMyString strResp = CBase64Wrapper::Decode(str1.c_str(), str1.length(), false);
    LogImpt("ivnpay-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strResp.c_str());

    // 解析到json
    Json::Value resp;
    int32_t ret = UrlEncode2Json(strResp, resp);
    if (ret != 0)
    {
        safe_strcpy(szResp, cbResp, "{\"ret\":-1001, \"msg\":\"invalid pay callback resp\"}");
        cbResp = strlen(szResp);
        return 200;
    }
    CMyString strOrderNo = resp["mch_order_id"].asString();
    CMyString strMd5Valid = resp["hash"].asString();
    CMyString strPlatformId = resp["svr_transaction_id"].asString();
    CMyString strRealFee = resp["amount"].asString();
    CMyString status = resp["status"].asString();

    // 校验回调IP
    CGame *pGame(NULL);
    ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        CMyString strMsg("{\"ret\":-1003, \"msg\":\"callback ip %s invalid.\"}", strIp.c_str());
        safe_strcpy(szResp, cbResp, strMsg.c_str());
        cbResp = strlen(szResp);
        return 200;
    }

    // 验签
    http::CHttpRespMaker resp_data;
    resp_data.AddJson(resp);
    resp_data.erase("hash");
    int32_t result = ValidPayCallbackSign(resp_data, strMd5Valid, strOrderNo, szResp, cbResp);
    if (result != 0)
    {
        // 错误信息在 ValidPayCallbackSign 中填充
        return result;
    }

    // 填充返回结果数据
    int32_t code(payCallbackFail);
    if (status != "1")
    {
        code = payCallbackFail;
    }
    else
    {
        code = payCallbackOK;
    }

    double realFee = strRealFee.asDouble();
    int32_t nRealFee = realFee * 100; // 转换成分

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strResp, code, nRealFee);
    if (ret >= 0)
    {
        // 回传到回调方
        safe_strcpy(szResp, cbResp, "{\"ret\": 0, \"msg\":\"OK\"}");
        cbResp = strlen(szResp);
    }
    else
    {
        safe_strcpy(szResp, cbResp, "{\"ret\":-1099, \"msg\":\"failure.\"}");
        cbResp = strlen(szResp);
    }

    return 200;
}

ENTRY_FUNC_DECLARE(CPluginIvnPay, doWithdraw)
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
    vecParam.push_back("bankCard");
    vecParam.push_back("fee");
    //LogDebug("----------------------doWithdraw before parameter check--------------------------");
    //vecParam.push_back("token");
    int32_t code = this->valid_req(req, szResp, cbResp, vecParam, 
            signEnable, signField, true, "gameCode", pGame);
    if (0 != code)
    {
        return code;
    }

    // 检查发起的IP是否合法
    CMyString strIp = getClientIpEx(getIpFromHeader, pClient, req, "X-Real-IP");
    if (!this->ValidIp(strIp,"main/ivnpay/ip", "main/ivnpay/ip/withdraw"))
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
#if 0 
    // TODO: 是否需要先行检查订单是否存在？
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
#else
    int32_t result(0);
#endif

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
    CMyString creditKey("%s%s", getWithdrawKeyPrefix(), orderNo.c_str());
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

    LogDebug("doWithdraw -------------10---------------");

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginIvnPay, doWithdrawCallback)
{
    // TODO: 订单回调
    LogDebug("ivnpay doWithdrawCallback");

    CMyString strCallbackResp = req.getParam("param");
    CMyString strIp = getClientIp(pClient, req);
    LogMsg("ivnpay-withdraw resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    strCallbackResp = CBase64Wrapper::Decode(strCallbackResp.c_str(), strCallbackResp.length(), false);
    LogImpt("ivnpay-withdraw resp after decode, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());

    // json解析
    std::vector<KvstrItem> resp;
    safe_strtok(strCallbackResp.c_str(), resp, "&","=");
    if (resp.size() == 0)
    {
        int32_t result = PAYFRAME_ERROR(err_InvalidParamter); // 回调的IP不合法
        CMyString strNote("ivnpay withdraw callback ip [%s] invalid", strIp.c_str());
        CPayWrapper::OnPayFailure(result, strNote, szResp, cbResp);
        return result;
    }

    CMyString strOrderNo;
    CMyString strMd5Valid;
    http::CHttpRespMaker resp_data;
    for (size_t i = 0; i < resp.size(); i++)
    {
        if (resp[i].key != "hash")
        {
            if (resp[i].key == "mch_order_id")
            {
                strOrderNo = resp[i].value;
            }
            resp_data.add(resp[i].key, resp[i].value);
        }
        else
        {
            strMd5Valid = resp[i].value;
        }      
    }

    // 校验回调IP
#if 0
    // TODO: 此处有BUG，CPayWrapper没有存储提现订单的游服相关信息
    // TODO: 重新验签
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        int32_t result = PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
        CMyString strNote("ivnpay callback ip [%s] invalid", strIp.c_str());
        CPayWrapper::OnPayFailure(result, strNote, szResp, cbResp);
        return result;
    }

    // 验签
    int32_t result = ValidPayCallbackSign(resp_data, strMd5Valid, strOrderNo, szResp, cbResp);
    if (result != 0)
    {
        return result;
    }
#else
    // bool val = this->ValidWithdrawResp(resp_data, strOrderNo, strMd5Valid, szResp, cbResp);
    CMyString withdrawKey("%s%s", getWithdrawKeyPrefix(), strOrderNo.c_str());
    std::string creditData;
    int32_t ret = m_redis.Get(withdrawKey, creditData);
    if (ret ==1)
    {
        ret = this->dealCreditOrder(creditData);
        LogDebug("deal with credit order %s, result:%d", withdrawKey.c_str(), ret);
        if (ret ==0)
        {
            m_redis.Del(withdrawKey.c_str());
        }
    }
#endif

    //LogImpt("ivnpay-withdraw resp, ip:[%s], sign result %d", strIp.c_str(), result);

    safe_strcpy(szResp, cbResp, "{\"ret\": 0}");
    cbResp = strlen(szResp);
    
    return 0;
}

int32_t CPluginIvnPay::dealCreditRequest(CSdk *pSdk, CGame *pGame, int64_t fee, const CMyString &bankCard, const CMyString &bankCode, const CMyString &bankName, const CMyString &uin, const CMyString &orderNo, char *szResp, uint32_t &cbResp)
{
    
    http::CHttpRespMaker req_credit;
    //CXmlFile &xml = CONFIG.getXml();
    //int32_t adjust = xml.GetLong("main/paycenter/timezone", "adjust");
    //int32_t nReqTime = time(NULL) + adjust; 


    req_credit.add("mch_id", pSdk->getAccount());
    req_credit.add("mch_uid", uin);
    req_credit.add("mch_order_id", orderNo);
    req_credit.add("pay_type", 4);  //  平台目录只支持momo代付(4)
    // 需要将元转换成分，再向平台请求支付
    fee /= 100;
    req_credit.add("amount", fee);
    req_credit.add("notify_url", pSdk->getWithdrawCallbackUrl());
    req_credit.add("account", bankCard);


    CMyString strSignSrc = req_credit.http_str();
    strSignSrc.Append("%s", pSdk->getApiKey().c_str());
    CMyString strSign = CMD5Wrapper::encode(strSignSrc.c_str());
    req_credit.add("hash", strSign);

    CMyString strContentType("Content-Type: application/json");
    CMyString str3 = req_credit.json_str(true);
    //CMyString strContentType("Content-Type: application/json");
    //CMyString str3 = req_credit.json_str();
    LogDebug("[ivnpay md5 text   ] %s", strSign.c_str());
    LogDebug("[ivnpay sign text  ] %s", str3.c_str());

    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    CMyString strUrl(pSdk->getWithdrawUrl());
    LogDebug("[ivnpay url] %s", strUrl.c_str());
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
            CMyString strNote("用户(uin=%s)调起ivnpay付款失败，错误码:%d, 错误说明:%s", uin.c_str(), ret, strMsg.c_str() );
            CPayWrapper::postAlert(pGame, strNote);

            strError.Format("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("[ivnpay WITHDRAW HEADER] %s", strHeader.c_str());
        LogWarn("[ivnpay WITHDRAW REP   ] %s", strResp.c_str());
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

int32_t CPluginIvnPay::ValidPayCallbackSign(http::CHttpRespMaker &req, const CMyString &strMd5Valid, const CMyString &strOrderNo, char *szResp, uint32_t &cbResp)
{
    CSdkChannel *pChannel = CPayWrapper::on_get_order_sdkChannel(strOrderNo);
    if (NULL == pChannel)
    {
        Json::Value resp;
        resp["ret"] = -1005;
        resp["msg"] = CMyString("not found orderNo %s", strOrderNo.c_str());
        dealResp(resp, szResp, cbResp);
        return -1005;
    }

    CSdk *pSdk = pChannel->getSdk();
    if (NULL == pSdk)
    {
        Json::Value resp;
        resp["ret"] = -1007;
        resp["msg"] = CMyString("not found SDK %s", pChannel->getSdkCode().c_str());
        dealResp(resp, szResp, cbResp);
        return -1007;
    }

    CMyString str1 = req.http_str_manual('&', '=', false, false); // req_data.http_str();
    str1.Append("%s", pSdk->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str1.c_str());
    LogDebug("origin text[%s] sign[%s]", str1.c_str(), strMd5.c_str());

    strMd5.MakeUpper();
    if (strMd5 != strMd5Valid)
    {
        LogErr("ivnpay resp sign [%s], yet verify sign is [%s]",
                strMd5Valid.c_str(), strMd5.c_str());
        Json::Value resp;
        resp["ret"] = -1009;
        resp["msg"] = "ivnpay callback sign mismatch";
        dealResp(resp, szResp, cbResp);
        return -1009;
    }

    return 0;
}

/*
int32_t CPluginInvPay::ValidWithdrawResp(http::CHttpRespMaker &req, const CMyString &strOrder, const CMyString &strMD5, char *szResp, uint32_t &cbResp)
{
    CMyString withdrawKey("%s%s", getWithdrawKeyPrefix(), strOrder.c_str());
    std::string creditData;
    int32_t ret = m_redis.Get(withdrawKey, creditData);
    // 获取 gameCode, mechKey
    CMyString gameCode;
    CMyString mechKey;
    if (ret != 1)
    {
        // 从DB中查询出相关数据
        bool bFind (false);
        int32_t orderStatus(0);
        m_pDB->Cmd("SELECT t1.`gameCode`,t1.`status`,t2.`apiKey` ");
        m_pDB->Cmd("FROM `t_withdraw` t1, `t_sdk` t2 ");
        m_pDB->Cmd("WHERE t1.`order_id`='%s' AND t1.`sdkcode`=t2.`code`;", strOrder.c_str());
        while(m_pDB->More())
        {
            gameCode = m_pDB->GetString(0);
            orderStatus = m_pDB->GetInt(1);
            mechKey = m_pDB->GetString(2);
            bFind = true;
        }

        if (!bFind)
        {
            safe_strcpy(szResp, cbResp, "not found withdraw order.");
            cbResp = strlen(szResp);
            return -1;
        }

        // 检查 orderStatus 是否需要继续处理
        if (orderStatus <10)
        {
            safe_strcpy(szResp, cbResp, "withdraw order status not need pay again");
            cbResp = strlen(szResp);
            return -3;
        }
        else if (orderStatus==10)
        {
            safe_strcpy(szResp, cbResp, "withdraw order already paied.");
            cbResp = strlen(szResp);
            return 0;
        }
    }
    else
    {
        CJsonWrapper jr;
        jr.Init(creditData.c_str());
        gameCode = jr.getString("gameCode");
        mechKey = jr.getString("key");
    }

    // 检查回调IP
    
    // 校验回调签名


    return false;
}
*/

bool CPluginIvnPay::ValidIp(const CMyString &strIp, const CMyString &sect_enable, const CMyString &sect_ips)
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

void CPluginIvnPay::getSignInfo(const CMyString &sect, bool &bEnable, CMyString &field)
{
    CXmlFile &xml = CONFIG.getXml();
    bEnable = xml.GetBool(sect.c_str(), "enable");
    field = xml.GetString(sect.c_str(), "field");
}

int32_t CPluginIvnPay::valid_req(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::vector<CMyString> &vecParam, bool bSign, const CMyString &signField, bool bSignFullParam, const CMyString &gameCodeField, CGame *&pGame)
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

int32_t CPluginIvnPay::CheckCreditOrder(CGame *pGame, const CMyString &strUrl, const CMyString &pid, const CMyString &uin, const CMyString &orderNo, const CMyString &key, CMyString &strError, CMyString &strResp, int32_t &state)
{
    http::CHttpRespMaker req_credit;
    req_credit.add("mch_id", pid);
    req_credit.add("mch_order_id", orderNo);

    CMyString strSignSrc = req_credit.http_str();
    strSignSrc.Append("%s", key.c_str());
    CMyString strSign = CMD5Wrapper::encode(strSignSrc.c_str());
    req_credit.add("hash", strSign);

    CMyString str3 = req_credit.json_str(true);

    CMyString strContentType("Content-Type: application/json");
    LogDebug("[ivnpay credit key ] %s", key.c_str());
    LogDebug("[ivnpay credit md5 ] %s", strSign.c_str());
    LogDebug("[ivnpay credit sign] %s", str3.c_str());

    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    LogDebug("[ivnpay credit url] %s", strUrl.c_str());
    CMyString strHeader;
    // CMyString strResp;
    int32_t result(-1);
    do {
        int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, str3.c_str());
        if (ret != 0)
        {
            CMyString strMsg;
            m_http.getError(ret, strMsg);
            CMyString strNote("用户(uin=%s)调起ivnpay付款失败，错误码:%d, 错误说明:%s", uin.c_str(), ret, strMsg.c_str() );
            CPayWrapper::postAlert(pGame, strNote);

            strError.Format("%s, URL:%s", strMsg.c_str(), strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        LogWarn("[ivnpay CREDIT HEADER] %s", strHeader.c_str());
        LogWarn("[ivnpay CREDIT REP   ] %s", strResp.c_str());
        http::CHttpRespone credit_resp;
        credit_resp.read((void*)strHeader.c_str(), strHeader.size());
        credit_resp.read((void*)strResp.c_str(), strResp.size());
        result = dealCreditCheckResp(credit_resp, strResp, strError, state);
    }while(0);

    return result;
}

int32_t CPluginIvnPay::dealCreditCheckResp(http::CHttpRespone &resp, const CMyString &strResp, CMyString &strError, int32_t &result)
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
    int32_t code = jr.getInt("ret");

    if (0 != code)
    {
        strError = strMsg;
        return -3;
    }
    
    CJsonWrapper data;
    if (!jr.getReader("data", data))
    {
        // 数据格式不正确，没有data字段
        strError.Format("data field invalid, resp[%s]", strResp.c_str());
        return -4;
    }

#if 0
    // 处理成功，依据result来判断结果
    // 1:待处理;2:已付款;3:已驳回;4:订单异常;5:付款中;6:代付失败
    int32_t nStatus = data.getInt("status");
    if (0==nStatus)
    {
       result = 5;
    }
    else if (1==nStatus)
    {
        result = 2;
    }
    else if (2 == nStatus)
    {
        result = 6;
    }
    else
    {
        result = 4;
    }
#else
    // NOTE: 不需要对status进行转义，在enum中已做顺序处理
    result = data.getInt("status");
#endif
    return 0;
}


int32_t CPluginIvnPay::LogCreditStatus(const CMyString &orderNo, int32_t status, const CMyString &remark, int64_t creditTime, const CMyString &creditOp, char *szResp, uint32_t &cbResp)
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


int32_t CPluginIvnPay::dealCreditSyncResp(CMyString &strResp, CMyString &strError, CMyString &platformOrderNo)
{
    platformOrderNo= "N/A";
    CJsonWrapper jr;
    if (!jr.Init(strResp.c_str()))
    {
        strError = "invalid json string";
        return -1;
    }
    if (jr.getInt("ret") != 0)
    {
        strError = jr.getString("msg");
        return -2;
    }

    CJsonWrapper jrData;
    if (jr.getReader("data", jrData))
    {
        if (jrData.getInt("status") == 2)
        {
            return -3;
        }

        platformOrderNo = jrData.getString("mch_order_id");
    }
    return 0;
}


int32_t CPluginIvnPay::dealCreditOrder(const CMyString &creditData)
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
    CMyString creditKey("%s%s", getWithdrawKeyPrefix(), orderNo.c_str());
    CMyString strError;
    CMyString strOrderResp;
    int32_t state(-1);
    int32_t result = this->CheckCreditOrder(pGame, jr.getString("url"),
            jr.getString("pid"), uin, orderNo,
            jr.getString("key"), strError, strOrderResp, state);

    LogDebug("ivnpay-resp order:%s, state:%d", orderNo.c_str(), state);
    if (result != 0)
    {
        return -7;
    }

    int32_t status(-1);
    switch(state)
    {
    //case creditNotFound:
    //    m_redis.Del(creditKey);
    //    break;
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


int32_t CPluginIvnPay::NotifyGamesvr(const CMyString &gameCode, const CMyString &uin, const CMyString &orderNo, int64_t fee, int32_t step, int32_t result, CGame *pGame)
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


int32_t CPluginIvnPay::LogLoanStatus(const CMyString &orderNo, int32_t status, char *szResp, uint32_t &cbResp)
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

int32_t CPluginIvnPay::QueryPayOrder(const CMyString &orderNo, Json::Value &resp)
{
    // 通过orderNo 查询 pSdk

    int32_t sdkChannelId(-1);
    int32_t billpointId(-1);
    m_pDB->Cmd("SELECT `sdkChannelId`,`bpId` FROM `t_payorder` WHERE `orderNo`='%s';", orderNo.c_str());
    while(m_pDB->More())
    {
        sdkChannelId = m_pDB->GetInt(0);
        billpointId = m_pDB->GetInt(1);
    }
    char szError[128] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("[Q9Pay] query order info failure, orderNo:%s, error:%s", orderNo.c_str(), szError);
        return -101;
    }
    if (sdkChannelId <= 0)
    {
        LogWarn("[Q9Pay] not found ourderNo %s", orderNo.c_str());
        return -103;
    }

    CSdkChannel *pChannel = SDKCHANNEL_LIST.Find(sdkChannelId);
    if (pChannel == NULL)
    {
        LogWarn("[Q9Pay] not found sdkChannelId %d in memory", sdkChannelId);
        return -105;
    }
    CSdk *pSdk = pChannel->getSdk();
    if (pSdk == NULL)
    {
        LogFatal("[Q9Pay] not found CSdk for sdkChannelId %d, maybe corrupted.", sdkChannelId);
        return -107;
    }

    http::CHttpRespMaker req_;
    req_.add("mch_id", pSdk->getAccount());
    req_.add("mch_order_id", orderNo);
    CMyString str1 = req_.http_str();
    str1 += pSdk->getApiKey();
    CMyString str2 = CMD5Wrapper::encode(str1.c_str());
    req_.add("hash", str2);
    CMyString strReq = req_.json_str(true);

    int32_t result = -109;
    CMyString strMsg;
    CMyString strResp;
    CMyString strRespHeader;
    http::VEC_STRING headers;
    headers.push_back("Content-Type: application/json");
    do {
        int32_t ret = m_http.HttpPostWithHeaders(pSdk->getPayUrl().c_str(), 
                strResp, strRespHeader, headers,
                strReq.c_str()); 
        if (ret != 0)
        {
            if (ret > 0)
                m_http.getError(ret, strMsg);
            else
                strMsg.Format("CHttpClient error %d", ret);
            LogErr("[Q9Pay] query payorder result failure, error:%s", strMsg.c_str());
            result = -109;
            break;
        }
        LogDebug("[Q9Pay] query order %s succeed,\nresp[%s]\nheader[%s]", 
                orderNo.c_str(), strResp.c_str(), strRespHeader.c_str());
        // 解析结果
        CJsonWrapper jr;
        if (!jr.Init(strResp))
        {
            LogErr("[Q9Pay] query order %s resp [%s] not valid json.", orderNo.c_str(), strResp.c_str());
            result = -111;
            break;
        }
        if (jr.getInt("ret") != 0)
        {
            strMsg = jr.getString("msg");
            LogErr("[Q9Pay] query order %s failure, msg:%s", orderNo.c_str(), strMsg.c_str());
            result = -113;
            break;
        }
        CJsonWrapper jrData;
        if (!jr.getReader("data", jrData))
        {
            LogErr("[Q9Pay] query order %s resp [%s] has invalid \'data\' field.", orderNo.c_str(), strResp.c_str());
            result = -115;
            break;
        }
        if (jrData.getString("mch_order_id") != orderNo)
        {
            LogFatal("[Q9Pay] query order %s resp [%s] invalid, order not match.", orderNo.c_str(), strResp.c_str());
            result = -117;
            break;
            
        }
        resp["pay_type"] = jrData.getInt("pay_type");
        resp["amount"] = jrData.getInt("amount");
        resp["platformOrderNo"] = jrData.getString("svr_transaction_id");
        resp["status"] = jrData.getInt("status");
        result = 0;
    }while(0);

    return result;
}

