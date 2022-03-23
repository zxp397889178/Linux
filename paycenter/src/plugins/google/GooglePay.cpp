/*
 * =====================================================================================
 *
 *       Filename:  GooglePay.cpp
 *
 *    Description:  谷歌支付定义
 *
 *        Version:  1.0
 *        Created:  2020年10月21日 15时15分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "GooglePay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPluginGooglePay::CPluginGooglePay()
    : CPayWrapper("GooglePay")
{
    //mkdir("./html/google", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

CPluginGooglePay::~CPluginGooglePay()
{
}

int32_t CPluginGooglePay::Init(void)
{
    CMyString strSect("main/google");
    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {   
        return ret;
    }
    
    CMyString pyHome;
    getConfigStr("main/google/python", "home", pyHome);
    // 获得 google 认证所需信息
    const char* sect_ = "main/google/certified";
    getConfigStr(sect_, "script", m_script);
    getConfigStr(sect_, "function", m_function);
    getConfigStr(sect_, "keyfile", m_secret);
    getConfigStr("main/google/package", "name", m_packageName);

    if (!m_python.Init(pyHome))
    {
        LogErr("init python loader failure.");
        return -21;
    }
    ret = m_python.LoadScript(m_script);
    if (ret != 0)
    {
        LogErr("load python script %s failure, ret=%d.", m_script.c_str(), ret);
        return -21;
    }
    LogInit("load script %s succeed.", m_script.c_str());

    // TODO: 装载所有历史订单数据


    m_ackTimer.SetInterval(1000);  // 1 秒等待
    m_ackTimer.Startup();
    return 0;
}

int32_t CPluginGooglePay::Update(void)
{
    return this->Init();
}

void CPluginGooglePay::Maintance()
{
    CPayWrapper::Maintance();

    // 定时处理待ack的订单
    if (m_ackTimer.ToNextTime())
    {
        this->dealPendingAck();
    }
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

ENTRY_FUNC_DECLARE(CPluginGooglePay, doPay)
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

    // 生成订单号并返回给前端即可
    time_t t1 = time(NULL);
    char szReqTime[64] = "";
    TimeToStr(t1, szReqTime, sizeof(szReqTime));
    CMyString strRequestInfo("google-inapp");
    CMyString strHeader("N/A");
    CMyString strResp("N/A");
    CMyString platformOrderNo("N/A");
    CMyString strTransferUrl("google/callback");
    CPayWrapper::_after_send_request(nOrderId, payReqSucceed, strRequestInfo, szReqTime, strHeader, strResp, platformOrderNo);

    // 记录支付的主键
    CPayWrapper::on_save_orderId(strOrderNo, nOrderId);

    // 返回给调用端
    // 判断是否为原生，若是原生的传其他数据
    CPayWrapper::OnPayResp(0, "call google InApp by client", strOrderNo,
            strTransferUrl, szResp, cbResp, 
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

int32_t CPluginGooglePay::doResp(int32_t code, int32_t result, const CMyString &errNote, 
        const CMyString &uin, const CMyString &orderNo, const CMyString &googleOrderNo, 
        char *szResp, uint32_t &cbResp)
{
    Json::Value resp;
    Json::Value data;
    resp["code"] = code;
    resp["message"] = errNote;
    data["result"] = result;
    data["uin"] = uin;
    data["orderNo"] = orderNo;
    data["googleOrderNo"] = googleOrderNo;
    resp["data"] = data;
    this->doResp(resp, szResp, cbResp);
    if (code == 0)
    {
        if (googleOrderNo.empty())
        {
            LogMsg("orderNo:%s, uin:%s, result:%s", orderNo.c_str(), uin.c_str(), errNote.c_str());
        }
        else
        {
            LogMsg("orderNo:%s, googleOrderNo:%s, uin:%s, result:%s", 
                    orderNo.c_str(), googleOrderNo.c_str(), uin.c_str(), errNote.c_str());
        }
    }
    else
    {
        LogErr("orderNo:%s, googleOrderNo:%s, uin:%s, error:%s",
                orderNo.c_str(), googleOrderNo.c_str(), uin.c_str(), errNote.c_str());
    }
    return code;
}

#define HTTP_REQ_STR(var)   CMyString var = req.getParam(#var)
#define HTTP_REQ_INT(var)   int32_t var = req.getParamInt(#var)
#define HTTP_REQ_INT64(var) int64_t var = req.getParamInt64(#var)
ENTRY_FUNC_DECLARE(CPluginGooglePay, doCallback)
{
    ext_headers["Content-Type"] = "Application/json";

    uint64_t t1 = Now();
    // 参数必须要用 productId, token 两个字段
    Json::Value resp;
    Json::Value resp_data;
    std::vector<CMyString> vecParam;
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    vecParam.push_back("orderNo");
    vecParam.push_back("productId");
    vecParam.push_back("token");
    vecParam.push_back("fee");
    CMyString strErr;
    HTTP_REQ_STR(uin);
    HTTP_REQ_STR(orderNo);
    bool ret = req.ValidParam(vecParam, strErr);
    if (!ret)
    {
        return this->doResp(-1, 400, strErr, uin, orderNo, "", szResp, cbResp);
    }

    uint64_t t2 = Now();
    HTTP_REQ_STR(gameCode);
    HTTP_REQ_STR(productId);
    HTTP_REQ_STR(token);
    HTTP_REQ_STR(sign);
    HTTP_REQ_INT64(fee);

    LogDebug("google cert=> gameCode:%s, uin:%s, orderNo:%s, productId:%s, token:%s, fee:%ld, sign:%s",
            gameCode.c_str(), uin.c_str(), orderNo.c_str(), productId.c_str(), token.c_str(), fee, sign.c_str());
    CGame *pGame = GAME_LIST.FindByCode(gameCode);
    if (NULL == pGame)
    {
        strErr.Format("unknown gameCode:%s", gameCode.c_str());
        return this->doResp(-3, 400, strErr, uin, orderNo, "", szResp, cbResp);
    }

    uint64_t t3 = Now();
    // 验证sign是否合法
    http::CHttpRespMaker reqVerify;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    req.BuildRespMaker_Except(vecExcept, reqVerify);
    CMyString str1 = reqVerify.http_str();
    str1.Append("&merchantKey=%s", pGame->getApiKey().c_str());
    CMyString strSign = CMD5Wrapper::encode(str1.c_str());
    strSign.MakeUpper();
    if (strcasecmp(strSign.c_str(), sign.c_str()))
    {
        strErr.Format("sign mismatch, it must be [%s]", strSign.c_str());
        return this->doResp(-5, 400, strErr, uin, orderNo, "", szResp, cbResp);
    }

    uint64_t t4 = Now();
    // 检查订单是否重复验证
    if (this->IsDuplicateOrder(gameCode, uin, productId, token))
    {
        strErr.Format("productId %s, token:%s already paied", productId.c_str(), token.c_str());
        return this->doResp(-7, 400, strErr, uin, orderNo, "", szResp, cbResp);
    }

    uint64_t t5 = Now();
    // 验证订单是否合法
    Json::Value orderInfo;
    int32_t orderId(0);
    ret = CPayWrapper::getStoredOrderInfo(orderNo, orderId, orderInfo, false);
    if (ret != err_Success)
    {
        return this->doResp(-9, 400, "orderNo invalid", uin, orderNo, "", szResp, cbResp);
    }
    CMyString strIp = getClientIp(pClient, req);

#if 0
    // 调用python脚本执行验证
    std::vector<PyParam>  params;
    params.push_back(PyParam("s", m_secret));
    params.push_back(PyParam("s", m_packageName));
    params.push_back(PyParam("s", productId));
    params.push_back(PyParam("s", token));
    PyObject *pResult(NULL);
    ret = m_python.CallFunc(m_function, params, pResult);
    int32_t code(payCallbackFail);
    int32_t google_result(-1);
    CMyString googleResp;
    if (ret==0 && pResult!=NULL)
    {
        // 解析 pResult的值
        code = payCallbackOK;
        int32_t google_ret(-1);
        char* google_resp;
        PyArg_ParseTuple(pResult, "is", &google_ret, &google_resp);
        google_result = google_ret;
        googleResp = google_resp;
        LogDebug("google resp=> ret:%d, resp:%s", google_ret, google_resp);
        //Py_DECREF(google_ret);
        Py_DECREF(google_resp);
    }
    //Py_DECREF(pResult);    
    LogDebug("google resp data=> [%s]", googleResp.c_str());

    // 处理google返回的结果
    CMyString strPlatformId("N/A");
    CMyString strError;
    googleResp.Replace('\'','\"');
    CMyString strCallbackResp(googleResp);
    CMyString regionCode("");
    if (google_result==0)
    {
        ret = this->dealGoogleResp(orderNo, googleResp, strPlatformId, regionCode);
        if (0==ret)
        {
            // 等待google确认
            code = payCallbackPending;
            CMyString strKey("pending-google-ack:%lu", Now());
            CMyString req_json = reqVerify.json_str();
            m_redis.Set(strKey, req_json);

            return this->doResp(0, 10, "Pending acknowledged", uin, orderNo, strPlatformId, szResp, cbResp);
        }
        else if (1==ret)
        {
            // google已确认
            code = payCallbackOK;
            this->doResp(0, 1, "OK", uin, orderNo, strPlatformId, szResp, cbResp);
        }
        else if (-3==ret)
        {
            code = payCancel;
            this->doResp(-11, 3, "cancelled", uin, orderNo, strPlatformId, szResp, cbResp);
        }
        else
        {
            code = payCallbackFail;
            this->doResp(-13, 2, strCallbackResp, uin, orderNo, strPlatformId, szResp, cbResp);
        }
    }
    else
    {
        this->dealGoogleError(orderNo, googleResp, strError);
        code = payCallbackFail;
        this->doResp(-99, 3, strCallbackResp, uin, orderNo, "", szResp, cbResp);
    }
    
    int64_t realFee(-1);
    // 将订单数据写入到redis内存
    if (code == payCallbackOK)
    {
        Json::Value order;
        order["gameCode"] = gameCode;
        order["uin"] = uin;
        order["orderNo"] = orderNo;
        order["productId"] = productId;
        order["token"] = token;
        order["googleOrderNo"] = strPlatformId;
        order["cert_time"] = (Json::Int64)Now();
        CJsonWrapper writer;
        CMyString orderData = writer.write(order);
        CMyString str1_1("productId:%s#token:%s", productId.c_str(), token.c_str());
        CMyString str2_1 = CMD5Wrapper::encode(str1_1.c_str());
        CMyString orderKey("%s:%s:%s", gameCode.c_str(), uin.c_str(), str2_1.c_str());
        m_redis.Set(orderKey, orderData);
        
        realFee = fee;
    }

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(orderNo, strPlatformId, 
            strCallbackResp, code, realFee, regionCode);

    return ret;
#else
    uint64_t t6 = Now();
    // 等待google确认
    CMyString strPlatformId("N/A");
    CMyString strKey("pending-google-ack:%lu", Now());
    CMyString req_json = reqVerify.json_str();
    m_redis.Set(strKey, req_json);

    this->doResp(0, 10, "Pending acknowledged", uin, orderNo, strPlatformId, szResp, cbResp);

    uint64_t t7 = Now();

    LogDebug("[STAT]google cert cost:%dms, "\
            "valid param:%dms, "\
            "query gamesvr:%dms, "\
            "verify sign:%dms, "\
            "check duplicate:%dms, "\
            "valid order:%dms, "\
            "save to redis & resp to server:%dms",
            (int32_t)(t7-t1),
            (int32_t)(t2-t1),
            (int32_t)(t3-t2),
            (int32_t)(t4-t3), 
            (int32_t)(t5-t4), 
            (int32_t)(t6-t5),
            (int32_t)(t7-t6));
    return 0;
#endif
}

void CPluginGooglePay::doResp(Json::Value &resp, char *szResp, uint32_t &cbResp)
{
    CJsonWrapper writer;
    CMyString strResp = writer.write(resp);
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

int32_t CPluginGooglePay::dealGoogleResp(const CMyString &orderNo, const CMyString &resp, CMyString &googleOrderNo, CMyString &regionCode)
{
    // {'purchaseTimeMillis': '1616688666443', 'purchaseState': 0, 'consumptionState': 1, 'developerPayload': '', 'orderId': 'GPA.3373-3973-3400-68421', 'acknowledgementState': 1, 'kind': 'androidpublisher#productPurchase', 'regionCode': 'EG'}
    
    // {"purchaseTimeMillis": "1619420112024", "purchaseState": 2, "consumptionState": 0, "developerPayload": "", "orderId": "GPA.3331-4582-0859-83361", "purchaseType": 0, "acknowledgementState": 0, "kind": "androidpublisher#productPurchase", "regionCode": "IN", "preordered": False}

    CJsonWrapper jr;
    bool ret = jr.Init(resp.c_str());
    if (!ret)
    {
        LogErr("orderNo %s google resp [%s] not valid json", orderNo.c_str(), resp.c_str());
        return -1;
    }

    int32_t purchaseState = jr.getInt("purchaseState");
    int32_t consumptionState = jr.getInt("consumptionState");
    int32_t acknowledgementState = jr.getInt("acknowledgementState");
    jr.Read("orderId", googleOrderNo);
    jr.Read("regionCode", regionCode);
    LogMsg("orderNo:%s, google OrderNo:%s, purchaseState:%d, consumptionState:%d, acknowledgementState:%d", orderNo.c_str(), googleOrderNo.c_str(), purchaseState, consumptionState, acknowledgementState);

    int32_t result(-3);
    if (purchaseState==0)
    {
        if (acknowledgementState==1)
        {
            // 已确认
            result = 1;
        }
        else
        {
            // 未确认，压入列表后续处理
            result = 0;
            LogWarn("orderNo:%s, google OrderNo:%s purchased, but not acknoledged.", orderNo.c_str(), googleOrderNo.c_str());
        }
    }
    else if (purchaseState==1)
    {
        result = -3;  //取消
        LogErr("orderNo:%s, google orderNo:%s purchase CANCELLED.", orderNo.c_str(), googleOrderNo.c_str());
    }
    else if (purchaseState==2)
    {
        result = 0;
        LogWarn("orderNo:%s, google OrderNo:%s pending purchased.", orderNo.c_str(), googleOrderNo.c_str());
    }
    else
    {
        result = -5;
        LogErr("orderNo:%s, google orderNo:%s purchase FAIL, invalid purchaseState %d.", 
                orderNo.c_str(), googleOrderNo.c_str(), purchaseState);
    }

    return result;
}


void CPluginGooglePay::dealGoogleError(const CMyString &orderNo, const CMyString &resp, CMyString &googleError)
{
    // <HttpError 400 when requesting https://androidpublisher.googleapis.com/androidpublisher/v3/applications/com.voicechat.game.royalludo/purchases/products/100/tokens/test-token-1234?alt=json returned "Invalid Value". Details: "Invalid Value">
   // => HttpError 400
   // => returned " "

    CStrSpliter ss1;
    ss1.Split(orderNo, ' ');
    int32_t code = ss1.asInt32(1);

    CStrSpliter ss2;
    ss2.Split(orderNo, '\"');
    googleError = ss2.at(1);

    LogWarn("orderNo:%s, google resp code:%d, msg:%s", orderNo.c_str(), code, googleError.c_str());
}

bool CPluginGooglePay::IsDuplicateOrder(const CMyString &gameCode, const CMyString &uin, const CMyString &productId, const CMyString &token)
{
    CMyString str1("productId:%s#token:%s", productId.c_str(), token.c_str());
    CMyString str2 = CMD5Wrapper::encode(str1.c_str());
    CMyString orderKey("%s:%s:%s", gameCode.c_str(), uin.c_str(), str2.c_str());

    int32_t ret = m_redis.IsExists(orderKey);
    if (ret != 1)
    {
        return false;
    }

    CMyString strData;
    ret = m_redis.GetData(orderKey, strData);
    if (ret == 1)
    {
        CJsonWrapper jr;
        jr.Init(strData.c_str());
        int64_t certTime = jr.getInt64("cert_time");
        CMyString orderNo = jr.getString("orderNo");
        char szTime[64] = "";
        time_t t1 = certTime/1000;
        TimeToStr(t1, szTime, sizeof(szTime));
        LogWarn("uin:%s, orderNo:%s, productId:%s, token:%s already PAIED at %s.",
                uin.c_str(), orderNo.c_str(), productId.c_str(), token.c_str(), szTime);
        return true;
    }

    return false;
}

void CPluginGooglePay::dealPendingAck(void)
{
    std::string keys = "pending-google-ack:*";
    std::vector<std::string> vecKey;
    m_redis.Keys(keys, vecKey);

    for(std::vector<std::string>::iterator it(vecKey.begin());
            it != vecKey.end(); ++it)
    {
        std::string key = *it;
        std::string val;
        int32_t ret = m_redis.Get(key, val);
        if (ret == 1)
        {
            ret = this->dealGoogleAck(val);
            if (ret==0)
            {
                m_redis.Del(key);
                continue;
            }

            // 判断是否超时，若超时则取消处理
            CStrSpliter css;
            css.Split(key.c_str(), ':');
            int64_t firstTime = css.asInt64(1);
            int64_t now = Now();
            if (now > firstTime + 600*1000)
            {
                time_t t1 = firstTime/1000;
                char szTime[64] = "";
                TimeToStr(t1, szTime, sizeof(szTime));
                LogErr("google cert data[%s] timeout, first cert time:%s", val.c_str(), szTime);
                m_redis.Del(key);
            }
        }
    }
}

int32_t CPluginGooglePay::dealGoogleAck(const CMyString &json_str)
{
    CJsonWrapper jr;
    jr.Init(json_str.c_str());

    CMyString productId = jr.getString("productId");
    CMyString token = jr.getString("token");
    CMyString orderNo = jr.getString("orderNo");
    CMyString gameCode = jr.getString("gameCode");
    CMyString uin = jr.getString("uin");

    // 调用python脚本执行验证
    std::vector<PyParam>  params;
    params.push_back(PyParam("s", m_secret));
    params.push_back(PyParam("s", m_packageName));
    params.push_back(PyParam("s", productId));
    params.push_back(PyParam("s", token));
    PyObject *pResult(NULL);
    int32_t ret = m_python.CallFunc(m_function, params, pResult);
    int32_t code(payCallbackFail);
    int32_t google_result(-1);
    CMyString googleResp;
    if (ret==0 && pResult!=NULL)
    {
        // 解析 pResult的值
        code = payCallbackOK;
        int32_t google_ret(-1);
        char* google_resp;
        PyArg_ParseTuple(pResult, "is", &google_ret, &google_resp);
        google_result = google_ret;
        googleResp = google_resp;
        LogDebug("google resp=> ret:%d, resp:%s", google_ret, google_resp);
        //Py_DECREF(google_ret);
        Py_DECREF(google_resp);
    }
    //Py_DECREF(pResult);    
    LogDebug("google resp data=> [%s]", googleResp.c_str());

    // 处理google返回的结果
    CMyString strPlatformId("N/A");
    CMyString strError;
    googleResp.Replace('\'','\"');
    CMyString strCallbackResp(googleResp);
    CMyString regionCode("");
    if (google_result==0)
    {
        ret = this->dealGoogleResp(orderNo, googleResp, strPlatformId, regionCode);
        if (0==ret)
        {
            // 等待google确认
            code = payCallbackPending;
        }
        else if (1==ret)
        {
            // google已确认
            code = payCallbackOK;
        }
        else if (-3==ret)
        {
            code = payCancel;
        }
        else
        {
            code = payCallbackFail;
        }
    }
    else
    {
        this->dealGoogleError(orderNo, googleResp, strError);
        code = payCallbackFail;
    }

    // 返回结果给前端
    if (code == payCallbackPending)
    {
        return 1;
    }

    int64_t realFee(-1);
    if (code == payCallbackOK)
    {
        // 将订单数据写入到redis内存
        Json::Value order;
        order["gameCode"] = jr.getString("gameCode");
        order["uin"] = jr.getString("uin");
        order["orderNo"] = jr.getString("orderNo");
        order["productId"] = productId;
        order["token"] = token;
        order["googleOrderNo"] = strPlatformId;
        order["cert_time"] = (Json::Int64)Now();
        CJsonWrapper wr;
        CMyString orderData = wr.write(order);
        orderData.trim_endl();
        CMyString str1_1("productId:%s#token:%s", productId.c_str(), token.c_str());
        CMyString str2_1 = CMD5Wrapper::encode(str1_1.c_str());
        CMyString orderKey("%s:%s:%s", gameCode.c_str(), uin.c_str(), str2_1.c_str());
        m_redis.SetData(orderKey, orderData);

        realFee = jr.getInt64("fee");
    }

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(orderNo, strPlatformId, 
            strCallbackResp, code, realFee, regionCode);

    return 0;
}

