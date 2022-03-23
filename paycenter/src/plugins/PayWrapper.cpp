/*
 * =====================================================================================
 *
 *       Filename:  PayWrapper.cpp
 *
 *    Description:  支付功能通用封装
 *
 *        Version:  1.0
 *        Created:  2020年02月01日 16时38分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "PayWrapper.h"
#include "Config.h"
#include "Security.h"
#include "Common.h"
#include "pay_error.h"

CPayWrapper::CPayWrapper(const CMyString &payFlag)
    : m_pDB(NULL)
    , m_payFlag(payFlag)
{
    http::CHttpClient::GlobalInit();
    m_timerRedo.SetInterval(5000);
    m_timerRedo.Startup();
}

CPayWrapper::~CPayWrapper()
{
    SAFE_RELEASE(m_pDB);
    http::CHttpClient::GlobalFini();
}

int32_t CPayWrapper::check_mysql_failure(ISimDB *pDB, const CMyString &strNote, const CMyString &strFunc, int32_t nLine)
{
    char szError[128] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("[%s:%s:%d]%s, errno:%d, error:%s", __FILE__, strFunc.c_str(), nLine, strNote.c_str(), err, szError);
        return PAYFRAME_ERROR(err_DbmsMysqlFault);
    }
    return err_Success;
}

CSdk* CPayWrapper::getSdk(const CMyString &gameCode)
{
    return SDK_LIST.FindByCode(gameCode, m_sdkCode);
}

CSdkChannel* CPayWrapper::getSdkChannel(int32_t channelId)
{
    return SDKCHANNEL_LIST.Find(channelId);
}

CGame* CPayWrapper::getGame(const CMyString &gameCode)
{
    return GAME_LIST.FindByCode(gameCode);
}

CGame* CPayWrapper::getGame(int32_t gameId)
{
    return GAME_LIST.Find(gameId);
}

#define HTTP_REQ_STR(str, param)   CMyString str=req.getParam(#param)
#define HTTP_REQ_INT(val, param)   int32_t val=req.getParamInt(#param)
#define HTTP_REQ_INT64(val,param)  int64_t val=req.getParamInt64(#param)
int32_t CPayWrapper::createNewOrder(http::CHttpRequest &req, int32_t channelId, int32_t &nOrderId, CMyString &strOrderNo, char *szResp, uint32_t &cbResp)
{
/*    
    int32_t configId = req.getParamInt("configId");
    int64_t payMoney = req.getParamInt64("amount");
    CMyString gameOrder = req.getParam("billNo");
    CMyString productId = req.getParam("productId");
    CMyString strRemark = req.getParam("Remark");
    CMyString strUin = req.getParam("uin");
    CMyString gameCode = req.getParam("gameCode");
    CMyString strIp = req.getParam("playerIp");
    CMyString clientType = req.getParam("clientType");  // 玩家前端类型，参见 dict_clientType
    CMyString userChannelId = req.getParam("channelId");  // 玩家渠道ID
    CMyString userChannelCode = req.getParam("channelCode"); // 玩家渠道编码
    CMyString userChannelName = req.getParam("channelName"); // 玩家渠道名称
    CMyString userName = req.getParam("userName");      // 玩家呢称
    int64_t userCreateTime = req.getParamInt64("createTime");  // 玩家创建时间
    
    int32_t bpId = req.getParamInt("paycenter_bpId");
    CMyString payStyle = req.getParam("paycenter_method");
*/

    HTTP_REQ_INT(configId, configId);
    HTTP_REQ_INT64(payMoney, amount);
    HTTP_REQ_STR(gameOrder, billNo);
    HTTP_REQ_STR(productId, productId);
    HTTP_REQ_STR(strRemark, Remark);
    HTTP_REQ_STR(strUin, uin);
    HTTP_REQ_STR(gameCode, gameCode);
    HTTP_REQ_STR(strIp, playerIp);
    HTTP_REQ_STR(clientType, clientType);
    HTTP_REQ_STR(userChannelId, channelId);
    HTTP_REQ_STR(userChannelCode, channelCode);
    HTTP_REQ_STR(userChannelName, channelName);
    HTTP_REQ_STR(userName, userName);
    HTTP_REQ_STR(payStyle, paycenter_method);
    HTTP_REQ_INT64(userCreateTime, createTime);
    HTTP_REQ_INT(bpId, paycenter_bpId);
    // 对呢称进行base64编码
    CMyString strName = CBase64Wrapper::Encode(userName.c_str(), userName.length(), false);

    CSdkChannel *pSdkChannel = SDKCHANNEL_LIST.Find(channelId);
    CSdk *pSdk = pSdkChannel->getSdk();
    CGame *pGame = pSdkChannel->getGame();

    //CMyString strReq = req.getFullUrl();
    //size_t cbReq = strReq.size();
    //CMyString strGameReq = CBase64Wrapper::Encode(strReq.c_str(), cbReq, false);

    int32_t code(0);
    CMyString strMsg;
    // 写入数据库
    m_pDB->Cmd("call pNewOrder_v3('%s','%s','%s', %d, "\
            " %d, %d, %d, %d," \
            " '%s', '%s','%s', '%s', "\
            " %ld,'%s',%ld, '%s', "\
            " %d, '%s', '%s', '%s' )" ,
            pGame->getCode().c_str(),       // 游服代码
            pSdk->getCode().c_str(),        // SDK 代码
            payStyle.c_str(),               // 支付方式
            pGame->getId(),                 // 游服ID
            configId,                       // t_channel 中的ID
            pSdk->getId(),                  // t_sdk 中的ID
            channelId,                      // t_sdkchannel 中的ID
            bpId,                           // t_billpoint 中的ID
            strUin.c_str(),                 // UIN
            strIp.c_str(),                  // 玩家外网IP
            strName.c_str(),               // 玩家呢称
            clientType.c_str(),             // 玩家前端类型，参见 dict_clientType
            userCreateTime,                 // 玩家创建时间
            gameOrder.c_str(),              // 游戏订单号
            payMoney,                       // 支付金额，以分计
            productId.c_str(),              // 产品ID，由游戏传过来
            userChannelId.c_str(),          // 玩家渠道ID
            userChannelCode.c_str(),        // 玩家渠道编码
            userChannelName.c_str(),        // 玩家渠道名称
            strRemark.c_str()               // 前端发过来的备注
         );
    while(m_pDB->More())
    {
        code = m_pDB->GetInt(0);
        strMsg = m_pDB->GetString(1);
        nOrderId = m_pDB->GetInt(2);
        strOrderNo = m_pDB->GetString(3);
    }

    int32_t result(err_Success);
    char szError[128] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        result = PAYFRAME_ERROR(err_CreateNewOrderFailure);
        CMyString strNote("save order data failure, errno:%d, error:%s", err, szError);
        this->OnPayFailure(result, strNote.c_str(), szResp, cbResp);
        LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strNote.c_str());
        return result;
    }

    if (code < 0)
    {
        result = code;
        this->OnPayFailure(result, "支付平台数据库错误", szResp, cbResp);
        return result;
    }
    if (strOrderNo.empty())
    {
        result = PAYFRAME_ERROR(err_GeneratePayOrderFailure);
        this->OnPayFailure(result, "支付平台数据存储失败", szResp, cbResp);
        return result;
    }

    // NOTE: 准备返回给游戏服务器的数据，先行写入到redis缓存中
    CJsonWrapper respone_data;
    respone_data.Write("code", 0);
    respone_data.Write("message", "数据在redis内存中");
    respone_data.Write("uin", strUin);
    respone_data.Write("gameCode", gameCode);
    respone_data.Write("orderNo", strOrderNo);
    respone_data.Write("billNo", gameOrder);
    respone_data.Write("fee", payMoney);
    respone_data.Write("realFee", 0);
    respone_data.Write("productId", productId);
    respone_data.Write("status", 0);
    // 以下两条数据为新增:2020-07-26
    respone_data.Write("payId", nOrderId);        // t_payorder中的ID , 唯一ID
    respone_data.Write("channelId", channelId);   // t_sdkchannel中的ID
    respone_data.Write("billpoint", bpId);        // 计费点ID
    respone_data.Write("method", payStyle);       // 支付方式

    CMyString strKey("paycenter_order_%s#data", strOrderNo.c_str());
    m_redis.Set(strKey.c_str(), respone_data.toString(), m_nOrderTimeout, 1);

    int64_t nOrderExpired = Now()+m_nOrderTimeout*1000;
    m_mapOrderExpired.insert(std::make_pair(strOrderNo, nOrderExpired));

    return err_Success;
}

void CPayWrapper::OnPayFailure(int32_t err, const CMyString &strNote, char *szResp, uint32_t &cbResp)
{
    // TODO: 这儿需要处理一下err值

    Json::Value resp;
    resp["code"] = 1;
    resp["message"] = CMyString("%s, error:%d", strNote.c_str(), err).c_str();
    Json::Value data;
    data.resize(0);
    resp["data"] = data;
    
    this->OnPayResp(resp, szResp, cbResp);
}

void CPayWrapper::OnPayResp(Json::Value &resp, char *szResp, uint32_t &cbResp)
{
    CJsonWrapper wr;
    CMyString strResp = wr.write(resp);
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

void CPayWrapper::OnPayResp(int32_t code, const CMyString &strNote, const CMyString &strTradeNo, 
        const CMyString &strUrl, char *szResp, uint32_t &cbResp,
        const CMyString &strSdk, const CMyString &strChannel, const CMyString &strMethod)
{
    Json::Value resp;
    resp["code"] = code;
    resp["message"] = strNote;
    Json::Value data;
    data["tradeNo"] = strTradeNo;
    data["url"] = strUrl;
    data["sdk"] = strSdk;       //2020-07-30 
    data["channel"] = strChannel;
    data["method"] = strMethod;
    resp["data"] = data;
    
    CJsonWrapper writer;
    CMyString strResp = writer.write(resp);
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

void CPayWrapper::Maintance(void)
{
    if (m_pDB && m_pDB->Ping(300)>0)
    {
        LogDebug("paycenter main db ping.");
    }

    if (m_redis.Ping(600)>0)
    {
    }

    // 检查需要重新向gamesvr 发送的订单
    if (m_timerRedo.ToNextTime())
    {
        this->notify2gamesvr();
    }

    // 检查超时的订单
    this->checkOrderExpired();
}

int32_t CPayWrapper::Init(const CMyString &sect_)
{
    int32_t result(0);
    int32_t line(0);
    do {
        try{
        // 获得响应的URL
        bool val = getConfigStr("main/paycenter/apply", "url", m_applyUrl);

        if (!val) { result = PAYFRAME_ERROR(err_MissConfig_PaySDK); line=__LINE__; break; }

        // 获得数据库指针
        const char *sectDB = "main/paycenter/db";
        SAFE_RELEASE(m_pDB);
        int32_t ret = open_mysql(sectDB, "main", m_pDB);
        if (ret != err_Success ) { result = ret; line=__LINE__; break; }
        
        m_redis.Close();
        ret = open_redis(sectDB, "redis", m_redis);
        if (ret != err_Success) { result = ret; line=__LINE__; break; }

        if (!getConfigInt("main/paycenter/order", "timeout", m_nOrderTimeout))
        {
            m_nOrderTimeout = 600;
        }

        // 获得SDK相关参数
        val = getConfigStr(sect_, "sdk", m_sdkCode);
        if (!val) { result = PAYFRAME_ERROR(err_MissConfig_PaySDK); line=__LINE__; break; }
        
        }
        catch(CException &e)
        {
            LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, e.what());
            return -1;
        }
        catch(...)
        {
            LogErr("[%s:%s:%d]call Init failure.", __FILE__, __func__, __LINE__);
            return -1;
        }
    }while(0);

    if (result != err_Success)
    {
        LogErr("[%s:%s:%d]init sdk pay channel data failure.", __FILE__, __func__, line);
        return result;
    }
    LogInit("init pay channel ok, sdk %s", m_sdkCode.c_str());

    return err_Success;
}

int32_t CPayWrapper::_before_send_request(http::CHttpRequest &req, CSdkChannel *&pChannel, int32_t &nOrderId, CMyString &strOrderNo, char* szResp, uint32_t &cbResp)
{
    int32_t err(0);
    int32_t channelId = req.getParamInt("paycenter_channelId");
    if (channelId <= 0)
    {
        err = PAYFRAME_ERROR(err_MissPayChannelId);
        this->OnPayFailure(err, "not valid paycenter API", szResp, cbResp);
        LogFatal("[%s:%s:%d]not paycenter support.", __FILE__, __func__, __LINE__);
        return err;
    }
    pChannel = this->getSdkChannel(channelId);
    if (NULL == pChannel)
    {
        err = PAYFRAME_ERROR(err_InvalidChannelId);
        this->OnPayFailure(err, "not support pay channel", szResp, cbResp);
        LogFatal("[%s:%s:%d]unsupport channelId %d", __FILE__, __func__, __LINE__, channelId);
        return err;
    }

    int32_t ret = this->createNewOrder(req, channelId, nOrderId, strOrderNo, szResp, cbResp);
    if (ret != err_Success)
    {
        return ret;
    }

    return 0;
}

bool CPayWrapper::_after_send_request(int32_t nOrderId, int32_t code, const CMyString &strReq, const char *req_time, const CMyString &strRespHeader, const CMyString &strResp, const CMyString &strPlatformOrderNo)
{
    time_t t2 = time(0);
    char szRespTime[64] = "";
    TimeToStr(t2, szRespTime, sizeof(szRespTime));

    LogMsg("orderId:%d, code:%d, req[%s], resp header[%s] data[%s]", 
            nOrderId, code, strReq.c_str(), strRespHeader.c_str(), strResp.c_str());

    int32_t result(0);
    CMyString strMessage;
    m_pDB->Cmd("call pUpdateOrderStatus_v3(%u, %d, '%s')",
            nOrderId, code, strPlatformOrderNo.c_str());
    while(m_pDB->More())
    {
        result = m_pDB->GetInt(0);
        strMessage = m_pDB->GetString(1);
    }
    int32_t ret = this->check_mysql_failure(m_pDB, CMyString("update payorder failure, orderId[%d]", nOrderId), __func__, __LINE__);
    if (ret!=err_Success || result!=0)
    {
        return false;
    }

    return true;
}

void CPayWrapper::on_save_orderId(const CMyString &strOrderNo, int64_t nOrderId)
{
    CMyString strKey("paycenter_order_%s#id", strOrderNo.c_str());
    m_redis.Set(strKey.c_str(), nOrderId, 600, 1);
}

void CPayWrapper::on_save_order_channelId(const CMyString &strOrderNo, http::CHttpRequest &req)
{
    int32_t nId(0);
    req.getParam("paycenter_channelId", nId);
    CMyString strKey("paycenter_order_%s#channelId", strOrderNo.c_str());
    m_redis.Set(strKey.c_str(), nId, 600, 1);
}

CSdkChannel* CPayWrapper::on_get_order_sdkChannel(const CMyString strOrderNo)
{
    CMyString strKey("paycenter_order_%s#channelId", strOrderNo.c_str());
    int64_t sdkChannelId(0);
    int32_t ret = m_redis.Get(strKey.c_str(), sdkChannelId);
    if (ret <= 0)
    {
        //从DB中查询订单号的channelId
        bool bFind(false);
        m_pDB->Cmd("SELECT `sdkChannelId` FROM `t_payorder` WHERE `orderNo`='%s';", strOrderNo.c_str());
        while(m_pDB->More())
        {
            sdkChannelId = m_pDB->GetInt64(0);
            bFind = true;
        }
        char szError[256] = "";
        int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
        if (err != 0)
        {
            LogErr("query pay order %s's sdkChannelId failure, err:%d, msg:%s.", 
                    strOrderNo.c_str(), err, szError);
            return NULL;
        }
        if (!bFind)
        {
            LogWarn("not found order %s's sdkChannel in DB.", strOrderNo.c_str());
            return NULL;
        }
    }
    else
    {
        m_redis.Del(strKey.c_str());
    }

    return this->getSdkChannel(sdkChannelId);
}

int32_t CPayWrapper::on_read_orderId(const CMyString &strOrderNo, int32_t &nOrderId, bool freeCache)
{
    CMyString strKey("paycenter_order_%s#id", strOrderNo.c_str());
    int64_t orderId(0);
    int32_t ret = m_redis.Get(strKey.c_str(), orderId); 
    if (ret <=0 )
    {
        return PAYFRAME_ERROR(err_PayOrderIdNotFound);
    }

    nOrderId = (int32_t)orderId;
    if (freeCache)
    {
        m_redis.Del(strKey.c_str());
    }
    return 0;
}

int32_t CPayWrapper::on_read_orderData(const CMyString &strOrderNo, Json::Value &resp, bool freeCache)
{
    CMyString strKey("paycenter_order_%s#data", strOrderNo.c_str());
    std::string data;
    int32_t ret = m_redis.Get(strKey.c_str(), data);
    if (ret <= 0)
    {
        return PAYFRAME_ERROR(err_PayOrderDataNotFound);
    }
    CJsonWrapper reader;
    if (!reader.parse(data))
    {
        return PAYFRAME_ERROR(err_PayOrderDataNotFound);
    }
    resp = reader.json();
    if (freeCache)
    {
        m_redis.Del(strKey.c_str());
    }
    return 0;
}

int32_t CPayWrapper::storeCallbackData(int32_t nOrderId, int32_t code, const CMyString &strPlatformId, const CMyString &strResp, int32_t realFee, const CMyString &regionCode)
{
    time_t t3 = time(0);
    char szNotifyTime[64] = "";
    TimeToStr(t3, szNotifyTime, sizeof(szNotifyTime));

    LogMsg("orderId:%d, code:%d, reply notify:%s",
            nOrderId, code, strResp.c_str());

    int32_t result(0);
    CMyString strMessage;
    m_pDB->Cmd("call pUpdateOrderNotify_v3(%d, %d, '%s', %d, '%s')",
            nOrderId, code, strPlatformId.c_str(), realFee, regionCode.c_str()); 
    while(m_pDB->More())
    {
        result = m_pDB->GetInt(0);
        strMessage = m_pDB->GetString(1);
    }
    int32_t ret = this->check_mysql_failure(m_pDB, CMyString("update tPayOrder failure, orderId [%d]", nOrderId), __func__, __LINE__);
    if (ret!=err_Success || result<0)
    {
        return PAYFRAME_ERROR(err_SavePayCallbackDataFailure);
    }

    return 0;
}

int32_t CPayWrapper::queryTimeoutOrder(const CMyString &strOrderNo, int32_t &orderId, Json::Value &resp) 
{
    //Json::Value resp;
    bool bFind(false);
    resp["orderNo"] = strOrderNo;
    resp["code"] = 0;
    resp["billNo"] = "";
    int32_t status(0);
    m_pDB->Cmd("call pQueryOrderInfo_v3('%s')", strOrderNo.c_str());
    while(m_pDB->More())
    {
        // NOTE: 准备返回给游戏服务器的数据，从数据库中取出
        orderId = m_pDB->GetInt(0);
        resp["payId"] = orderId;
        resp["uin"] = m_pDB->GetString(1);
        resp["billNo"] = m_pDB->GetString(2);
        resp["orderNo"] = m_pDB->GetString(3);
        resp["fee"] = m_pDB->GetInt(4);
        resp["realFee"] = m_pDB->GetInt(5);
        status = m_pDB->GetInt(6);
        resp["status"] = status;
        resp["channelId"] = m_pDB->GetInt(7);
        resp["productId"] = m_pDB->GetString(8);
        resp["gameCode"] = m_pDB->GetString(9);
        resp["billpoint"] = m_pDB->GetInt(12);
        resp["message"] = "从数据库中获取超时订单数据";
        bFind = true;
    }

    int32_t err = this->check_mysql_failure(m_pDB, CMyString("query order from tPayOrder failure, orderNo:%s", strOrderNo.c_str()), __func__, __LINE__);
    if (err != err_Success)
    {
        return PAYFRAME_ERROR(err_PayOrderNoInvalid);
    }
    if (!bFind)
    {
        return PAYFRAME_ERROR(err_PayOrderNotFound);
    }

    if (status==payReqSucceed || status ==payCallbackOK)
    {
        LogWarn("[%s:%s:%d]replay timeout order %s, status %d",
                __FILE__, __func__,__LINE__, strOrderNo.c_str(), status);

        //CJsonWrapper writer;
        //strRespData = writer.write(resp);
        return err_Success;
    }

    // 其他不需要处理的状态
    LogWarn("[%s:%s:%d]order %s not need replay, status %d",
            __FILE__, __func__, __LINE__, strOrderNo.c_str(), status);
    return PAYFRAME_ERROR(err_PayOrderFinished);
}

int32_t CPayWrapper::logNotifyStatus(int32_t nOrderId, const CMyString &uin, int32_t bpId, int32_t code)
{
    int32_t ret(-1);
    CMyString strMsg;
    m_pDB->Cmd("call pFinishOrder_v3(%d, '%s', %d,  %d)", 
            nOrderId, uin.c_str(), bpId, code);
    while(m_pDB->More())
    {
        ret = m_pDB->GetInt(0);
        strMsg = m_pDB->GetString(1);
    }
    if (ret != 0)
    {
        LogFatal("[%s:%s:%d]update t_payorder status failure, orderId %d, status %d, msg %s",
                __FILE__, __func__, __LINE__, nOrderId, code, strMsg.c_str());
        return PAYFRAME_ERROR(err_UpdatePayOrderStatusFailure);
    }
    return err_Success;
}

int32_t CPayWrapper::getStoredOrderInfo(const CMyString &strOrderNo, int32_t &orderId, Json::Value &resp, bool freeCache)
{
    int32_t ret = this->on_read_orderId(strOrderNo, orderId, freeCache);
    if (ret != err_Success)
    {
        LogDebug("query orderno [%s]\'s orderId failure", strOrderNo.c_str());
        // 订单超时，从数据库中查找订单信息
        // 适用于重放订单回调
        ret = this->queryTimeoutOrder(strOrderNo, orderId, resp);
        if (ret != err_Success)
        {
            LogFatal("[%s:%s:%d] ret=%d", __FILE__, __func__, __LINE__, ret);
            return ret;
        }
        return err_Success;
    }

    ret = this->on_read_orderData(strOrderNo, resp, freeCache);
    if (ret != err_Success)
    {
        LogDebug("query orderno [%s]\'s data failure, orderId %d ", strOrderNo.c_str(), orderId);
        // 订单超时，从数据库中查找订单信息
        // 适用于重放订单回调
        ret = this->queryTimeoutOrder(strOrderNo, orderId, resp);
        if (ret != err_Success)
        {
            LogFatal("[%s:%s:%d] ret=%d", __FILE__, __func__, __LINE__,  ret);
            return ret;
        }
        return err_Success;
    }
    return err_Success;
}

int32_t CPayWrapper::validCallbackIp(const CMyString &strOrderNo, const CMyString &strIp, CGame* &pGame)
{
    // 获得保存的订单数据
    int32_t orderId(0);
    // CMyString strRespData;
    Json::Value resp;
    int32_t ret = this->getStoredOrderInfo(strOrderNo, orderId, resp, false);
    if (ret != err_Success)
    {
        LogFatal("order no [%s] not found when valid callback ip, ret=%d", 
                strOrderNo.c_str(), ret);
        return -1;
    }

    // 获得游戏指针
    CMyString gameCode = resp["gameCode"].asString();
    if (gameCode.empty())
    {
        LogErr("gamecode is empty when valid callback ip for orderNo %s", strOrderNo.c_str());
        return -2;
    }
    pGame = this->getGame(gameCode);
    if (NULL == pGame)
    {
        LogErr("gamecode [%s] not found when valid callback ip for orderNo %s",
                gameCode.c_str(), strOrderNo.c_str());
        return -3;
    }
    CSdk *pSdk = this->getSdk(gameCode);
    if (NULL == pSdk)
    {
        LogErr("sdk not found when valid callback ip, orderNo:%s, gameCode:%s", strOrderNo.c_str(), gameCode.c_str());
        return -4;
    }

    bool val = pSdk->IsRechargeIpValid(strIp);
    if (!val)
    {
        LogErr("callback ip [%s] not allowed, gameCode:%s, sdkCode:%s", gameCode.c_str(), m_sdkCode.c_str());

        CMyString strText("回调IP [%s] 不合法，游戏编码:%s, SDK编码:%s, 请联系开发人员", strIp.c_str(), gameCode.c_str(), m_sdkCode.c_str() );
        this->postAlert(pGame, strText);
        return -5;
    }
    return 0;
}

int32_t CPayWrapper::_deal_notify(const CMyString &strOrderNo, 
        const CMyString &strPlatformId, const CMyString &strCallbackResp, 
        int32_t code, int64_t realFee, const CMyString &regionCode)
{
    // 移除key相关数据, 2020-05-29
    CMyString strKey1("paycenter_order_%s#channelId", strOrderNo.c_str());
    m_redis.Del(strKey1.c_str());
    this->freeOrderExpired(strOrderNo);

    // 获得保存的订单数据
    int32_t orderId(0);
    // CMyString strRespData;
    Json::Value resp;
    int32_t ret = this->getStoredOrderInfo(strOrderNo, orderId, resp, true);
    if (ret != err_Success)
    {
        // 数据库中没有该订单号，出错
        LogFatal("[%s:%s:%d]query stored order info failure, orderNo=%s, ret=%d", 
                __FILE__, __func__, __LINE__, strOrderNo.c_str(), ret);
        return -1;
    }

    CMyString gameCode = resp["gameCode"].asString();
    CGame *pGame = this->getGame(gameCode);
    if (NULL == pGame)
    {
        // 内存中没有该游戏数据，出错
        LogFatal("[%s:%s:%d]invalid game code:%s, orderNo=%s", 
                __FILE__, __func__, __LINE__, gameCode.c_str(), strOrderNo.c_str());
        return -3;
    }

    LogMsg("store order %s(orderId:%d) resp to db, platformId:%s.",
            strOrderNo.c_str(), orderId, strPlatformId.c_str());

    CMyString uin = resp["uin"].asString();
    int32_t bpId = resp["billpoint"].asInt();
    int64_t coin(0);    // 金币数量
    int64_t extCoin(0); // 赠送金币数量
    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    CMyString strItemInfo;
    if (pBP != NULL)
    {
        coin = pBP->getCoin();
        extCoin = pBP->getExtCoin();
        strItemInfo = pBP->getBpItemData();
        LogMsg("orderNo:%s, billpoint Id:%d, coin:%ld, extCoin:%ld,itemInfo:[%s]",
                strOrderNo.c_str(),  bpId, coin, extCoin, strItemInfo.c_str());
    }
    else
    {
        int32_t exchangeRate = pGame->getExchangeRate();
        int64_t fee = resp["fee"].asInt64();
        coin = fee * exchangeRate / 100;
        extCoin = 0;
        LogMsg("orderNo:%s, not found billpoint Id:%d, exchangeRate:%d, fee:%ld, coin:%ld", strOrderNo.c_str(), bpId, exchangeRate, fee, coin);
    }

    int32_t loopLeft(3);
    do
    {
        // 写入数据库中
        ret = this->storeCallbackData(orderId, code, strPlatformId, strCallbackResp, 
            realFee, regionCode);
        if (ret != 0)
        {
            // 订单结束
            --loopLeft;
            nsleep(5);
        }
    }while(ret!=0 && loopLeft>0);
    if (loopLeft<=0)
    {
        // 写数据库失败
        LogWarn("orderNo %s fail, id %d, result %d, platform_orderNo %s", 
            strOrderNo.c_str(), orderId, ret, strPlatformId.c_str());
        return -5;
    }
    
    // NOTE: 生成Sign值
    if (code==payCallbackOK || code==payCallbackPending)
    {
        resp["code"] = 0;
    }
    else
    {
        resp["code"] = -1;
    }
    resp["status"] = code;
    resp["realFee"] = (Json::Int64)realFee;
    resp["coin"] = (Json::Int64)coin;
    resp["extraCoin"] = (Json::Int64)extCoin;
    resp["itemInfo"] = strItemInfo;
    http::CHttpRespMaker resp_maker;
    resp_maker.AddJson(resp);
    CMyString pending_sign_str = resp_maker.http_str();
    LogDebug("before key: [%s]", pending_sign_str.c_str());
    pending_sign_str.Append("&key=%s", pGame->getApiKey().c_str());
    CMyString strSign = CMD5Wrapper::encode(pending_sign_str.c_str());
    strSign.MakeLower();
    LogDebug("before sign:[%s]", pending_sign_str.c_str());
    LogDebug("md5:        [%s]", strSign.c_str());

    //填充返回结果数据
    resp["sign"] = strSign;
    CJsonWrapper writer;
    CMyString strRespDataWithSign = writer.write(resp);
    CMyString strUrl = pGame->getApiNotifyUrl();
    LogDebug("DBG>url [%s]", strUrl.c_str());
    LogDebug("DBG>data[%s]", strRespDataWithSign.c_str());

    // 发送Http请求
    CMyString strNotifyResp;
    CMyString strContentType("Content-Type:application/json");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    CMyString strHeader;
    ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strNotifyResp, strHeader, header_list, strRespDataWithSign.c_str() );
    if (ret != 0)
    {
        CMyString strUin = resp["uin"].asString();
        int32_t fee = resp["fee"].asInt();
        CMyString strErr;
        m_http.getError(ret, strErr);
        CMyString strNote("用户(uin=%s)充值%.2lf成功，发放金币失败，请安排人工补币，错误信息:%s", 
                strUin.c_str(), fee/100.0, strErr.c_str()); 
        this->postAlert(pGame, strNote);

        // 远程连接失败
        this->logNotifyStatus(orderId, uin, bpId, payNotifyFail);
        LogFatal("[%s:%s:%d]uin %s recharge %.2lf succeed, but write to gamesvr failure, orderId:%d, orderNo:%s, platformOrderNo:%s, url:%s, error:%s",
                __FILE__, __func__, __LINE__, strUin.c_str(), fee/100.0,
                orderId, strOrderNo.c_str(), strPlatformId.c_str(), strUrl.c_str(), strErr.c_str());

        // 自动向游服重发申请
        Json::Value redo;
        redo["gameCode"] = gameCode;
        redo["uin"] = strUin;
        redo["fee"] = (Json::Int64)fee;
        redo["orderNo"] = strOrderNo;
        redo["orderId"] = orderId;
        redo["bpId"] = bpId;
        redo["platformOrderId"] = strPlatformId;
        redo["url"] = strUrl;
        redo["data"] = strRespDataWithSign;
        CMyString redoMaxKey("%s#redo#maxId", m_payFlag.c_str());
        int64_t keyId(0);
        m_redis.IncBy(redoMaxKey, 1, keyId);
        CMyString redoKey("%s#redo#%08ld", m_payFlag.c_str(), keyId);
        CJsonWrapper wr;
        CMyString strRedo = wr.write(redo);
        m_redis.Set(redoKey, strRedo);

        return 1;
    }

    LogMsg("orderNo:%s, game recharge resp [%s]", 
            strOrderNo.c_str(), strNotifyResp.c_str());
    // 对响应结果进行处理
    strNotifyResp.MakeUpper();
    if (strNotifyResp != "SUCCESS")
    {
        CMyString strUin = resp["uin"].asString();
        int64_t fee = resp["fee"].asInt64();
        CMyString strNote("用户(uin=%s)充值%.2lf成功，发放金币失败，请安排人工补币", 
                strUin.c_str(), fee/100.0); 
        this->postAlert(pGame, strNote);

        this->logNotifyStatus(orderId, uin, bpId, payBalanceFail);
        LogFatal("[%s:%s:%d]uin %s recharge %.2lf succeed, but gamesvr notify client failure, orderId:%d, orderNo %s, platform_orderNo %s, result [%s]", 
                __FILE__, __func__, __LINE__, strUin.c_str(), fee/100.0,
                orderId, strOrderNo.c_str(), strPlatformId.c_str(), strNotifyResp.c_str());

        // 自动向游服重发申请
        Json::Value redo;
        redo["gameCode"] = gameCode;
        redo["uin"] = strUin;
        redo["fee"] = (Json::Int64)fee;
        redo["orderNo"] = strOrderNo;
        redo["orderId"] = orderId;
        redo["bpId"] = bpId;
        redo["platformOrderId"] = strPlatformId;
        redo["url"] = strUrl;
        redo["data"] = strRespDataWithSign;
        CMyString redoMaxKey("%s#redo#maxId", m_payFlag.c_str());
        int64_t keyId(0);
        m_redis.IncBy(redoMaxKey, 1, keyId);
        CMyString redoKey("%s#redo#%08ld", m_payFlag.c_str(), keyId);
        CJsonWrapper wr;
        CMyString strRedo = wr.write(redo);
        m_redis.Set(redoKey, strRedo);

        return 2;
    }
    this->logNotifyStatus(orderId, uin, bpId, payFinished);
    LogImpt("orderNo %s finished, id %d, platform_orderNo %s", 
            strOrderNo.c_str(), orderId, strPlatformId.c_str());

    return 0;
}

int32_t CPayWrapper::postAlert(CGame *pGame, const CMyString &strText)
{
    if (NULL == pGame)
    {
        LogErr("pGame is NULL when postAlert msg:%s", strText.c_str());
        return -1;
    }
    
    // 获得警告的ID
    CJsonWrapper alert;
    pGame->getAlter(alert);
    int32_t alertId = alert.getInt("alertId");
    if (alertId <= 0)
    {
        return 0;
    }
    
    // 获得警告的URL
    CMyString alertUrl = alert.getString("url");
    
    // 发起警告
    http::CHttpClient net;
    CMyString strData("{\"chatId\":%d,\"text\":\"%s\"}", alertId, strText.c_str());
    http::VEC_STRING headers;
    headers.push_back("Content-Type: application/json");
    CMyString strHeader;
    CMyString strResp;
    int32_t ret = net.HttpPostWithHeaders(alertUrl.c_str(), strResp, strHeader, headers, strData.c_str());
    if (ret ==0)
    {
        // success
        return 1;
    }

    CMyString strError;
    net.getError(ret, strError);
    LogErr("call postAlert msg [%s] failure, errno:%d, error:%s", 
            strText.c_str(), ret, strError.c_str());
    return -1;
}

bool CPayWrapper::disableBillPoint(CSdkChannel *pChannel, int32_t payMoney)
{
    // TODO: 暂时与禁止SDK渠道相同的功能
    return this->disableSdkChannel(pChannel);
}

bool CPayWrapper::disableSdk(CSdkChannel *pChannel)
{
    CSdk *pSdk = pChannel->getSdk();
    int32_t ret = pSdk->Disable(m_pDB);
    if (ret != 0)
    {
        return false;
    }

    CMyString gameCode = pChannel->getGame()->getCode();
    CMyString sdkCode = pChannel->getSdk()->getCode();
    CMyString strNote("自动禁止支付SDK%s", sdkCode.c_str());
    this->postAlert(pChannel->getGame(), strNote);

    LogImpt("auto disable paysdk %s succeed, gameCode:%s",
            sdkCode.c_str(), gameCode.c_str());
    this->applyChange();
    return true;
}

bool CPayWrapper::disableSdkChannel(CSdkChannel *pChannel)
{
    int32_t ret = pChannel->Disable(m_pDB);
    if (ret != 0)
    {
        return false;
    }

    CGame *pGame = pChannel->getGame();
    CSdk* pSdk = pChannel->getSdk();
    CPayChannel *pPayChannel = pChannel->getPayChannel();

    CMyString channelName = pChannel->getName();
    CMyString gameCode = pGame->getCode();
    CMyString sdkCode = pSdk->getCode();
    CMyString channelCode = pChannel->getBankCode();
    CMyString methodCode = pPayChannel->getPayStyle();
    CMyString strNote("自动禁止支付渠道%s => SDK:%s, 渠道:%s, 支付方式:%s", channelName.c_str(), sdkCode.c_str(), channelCode.c_str(), methodCode.c_str());
    this->postAlert(pChannel->getGame(), strNote);

    LogImpt("auto disable paychannel %s succeed, gameCode:%s, sdkCode:%s, channelCode:%s, methodCode:%s", channelName.c_str(), gameCode.c_str(), sdkCode.c_str(), channelCode.c_str(), methodCode.c_str());

    this->applyChange();

    return true;
}

void CPayWrapper::applyChange(void)
{
    // 通知服务端生效
    httpex::CHttpClient http;
    http::CHttpRespone resp;
    int32_t ret = http.Get(m_applyUrl, "", resp);
    int32_t code = resp.getStatusCode();
    CByteStream buff;
    resp.getPost(buff);
    CMyString strResp;
    buff.Read(strResp);
    LogImpt("url: [%s] code:%d, resp: %s", m_applyUrl.c_str(), code, strResp.c_str());
}

int32_t CPayWrapper::dealRespError(CSdkChannel *pChannel, int32_t payMoney, const CMyString &errorDesc, CMyString &errorResp)
{
    int32_t result(0);

    CSdk *pSdk = pChannel->getSdk();
    if (NULL == pSdk)
    {
        // ERROR
        return -1;
    }
    bool bFind = pSdk->GetErrorNote(errorDesc, errorResp, result);
    if (!bFind)
    {
        result = postNothing;
        errorResp = errorDesc;
    }

    switch(result)
    {
    case postDisableBillPoint:
        this->disableBillPoint(pChannel, payMoney);
        break;
    case postDisableChannel:
        this->disableSdkChannel(pChannel);
        break;
    case postDisableSDK:
        this->disableSdk(pChannel);
        break;
    default:
        break;
    }

    return result;
}

void CPayWrapper::notify2gamesvr(void)
{
    CMyString strMaxKey("%s#redo#maxId", m_payFlag.c_str());
    int64_t maxId(0);
    int32_t ret = m_redis.Get(strMaxKey, maxId);
    if (ret <= 0)
    {
        // 无需要重发的数据
        return;
    }

    CMyString strCurrKey("%s#redo#currId", m_payFlag.c_str());
    int64_t currId(0);
    ret = m_redis.Get(strCurrKey, currId);
    if (ret< 0)
    {
        // 出错
        return;
    }
    if (ret==0)
    {
        currId = 1;
    }

    CMyString strKey;
    for(int64_t row(currId); row<maxId; ++row)
    {
        strKey.Format("%s#redo#%08ld", m_payFlag.c_str(), row);
        std::string str1;
        ret = m_redis.Get(strKey, str1);
        if (ret <= 0)
        {
            continue;
        }

        this->redoNotifyGamesvr(str1);
        m_redis.Del(strKey);
    }
    m_redis.Set(strCurrKey, maxId);
}

void CPayWrapper::redoNotifyGamesvr(const CMyString &notify_data)
{
    CJsonWrapper jr;
    if (!jr.Init(notify_data))
    {
        // 非法Json
        return;
    }

    CMyString strUrl = jr.getString("url");
    CMyString strData = jr.getString("data");
    CMyString strOrderNo = jr.getString("orderNo");
    CMyString strPlatformId = jr.getString("platformOrderId");
    CMyString strUin = jr.getString("uin");
    CMyString gameCode = jr.getString("gameCode");
    int64_t fee = jr.getInt64("fee");
    int32_t orderId = jr.getInt("orderId");
    int32_t bpId = jr.getInt("bpId");

    // 发送Http请求
    CMyString strContentType("Content-Type:application/json");
    http::VEC_STRING header_list;
    header_list.push_back(strContentType.c_str());
    CMyString strResp;
    CMyString strHeader;
    int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strData.c_str() );
    if (ret != 0)
    {
        CMyString strErr;
        m_http.getError(ret, strErr);
        CMyString strNote("用户(uin=%s)充值%.2lf成功，发放金币失败，请安排人工补币，错误信息:%s", 
                strUin.c_str(), fee/100.0, strErr.c_str()); 

        // 通过机器人告警
        CGame *pGame = this->getGame(gameCode);
        if (pGame != NULL)
        {
            this->postAlert(pGame, strNote);
        }

        // 远程连接失败
        this->logNotifyStatus(orderId, strUin, bpId, payNotifyFail);
        LogFatal("[%s:%s:%d]uin %s recharge %.2lf succeed, but write to gamesvr failure, orderId:%d, orderNo:%s, platformOrderNo:%s, url:%s, error:%s",
                __FILE__, __func__, __LINE__, strUin.c_str(), fee/100.0,
                orderId, strOrderNo.c_str(), strPlatformId.c_str(), strUrl.c_str(), strErr.c_str());

        // 自动向游服重发申请
        CMyString redoMaxKey("%s#redo#maxId", m_payFlag.c_str());
        int64_t keyId(0);
        m_redis.IncBy(redoMaxKey, 1, keyId);
        CMyString redoKey("%s#redo#%08ld", m_payFlag.c_str(), keyId);
        m_redis.Set(redoKey, notify_data);

        return;
    }

    LogMsg("[%s] orderNo:%s, game recharge resp [%s]", 
            m_payFlag.c_str(), strOrderNo.c_str(), strResp.c_str());
    // 对响应结果进行处理
    strResp.MakeUpper();
    if (strResp != "SUCCESS")
    {
        CMyString strNote("用户(uin=%s)充值%.2lf成功，发放金币失败，请安排人工补币", 
                strUin.c_str(), fee/100.0); 
        CGame *pGame = this->getGame(gameCode);
        if (pGame != NULL)
        {
            this->postAlert(pGame, strNote);
        }

        this->logNotifyStatus(orderId, strUin, bpId, payBalanceFail);
        LogFatal("[%s:%s:%d]uin %s recharge %.2lf succeed, but gamesvr notify client failure, orderId:%d, orderNo %s, platform_orderNo %s, result [%s]", 
                __FILE__, __func__, __LINE__, strUin.c_str(), fee/100.0,
                orderId, strOrderNo.c_str(), strPlatformId.c_str(), strResp.c_str());

        // 自动向游服重发申请
        CMyString redoMaxKey("%s#redo#maxId", m_payFlag.c_str());
        int64_t keyId(0);
        m_redis.IncBy(redoMaxKey, 1, keyId);
        CMyString redoKey("%s#redo#%08ld", m_payFlag.c_str(), keyId);
        m_redis.Set(redoKey, notify_data);

        return; 
    }
    this->logNotifyStatus(orderId, strUin, bpId, payFinished);
    LogImpt("[%s] orderNo %s finished, id %d, platform_orderNo %s", 
            m_payFlag.c_str(), strOrderNo.c_str(), orderId, strPlatformId.c_str());

}

void CPayWrapper::checkOrderExpired(void)
{
    int64_t now = Now();
    ORDER_EXPIRED_MAP::iterator it(m_mapOrderExpired.begin());
    for(; it != m_mapOrderExpired.end(); )
    {
        CMyString strOrder = it->first;
        int64_t expired = it->second;
        if (expired <= now)
        {
            m_pDB->Cmd("UPDATE `t_payorder` SET `orderStatus`=11,`updatedAt`=NOW() WHERE `orderNo`='%s' AND `orderStatus` in (0,6,7) ", strOrder.c_str());
            m_pDB->Exec();
            m_mapOrderExpired.erase(it++);
            LogMsg("[%s]order %s has expired.", m_payFlag.c_str(), strOrder.c_str());
        }
        else
        {
            ++it;
        }
    }
}

void CPayWrapper::freeOrderExpired(const CMyString &orderNo)
{
    m_mapOrderExpired.erase(orderNo);
}

