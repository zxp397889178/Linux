/*
 * =====================================================================================
 *
 *       Filename:  PayBase.cpp
 *
 *    Description:  支付中心封装v2
 *
 *        Version:  1.0
 *        Created:  2021年06月09日 16时26分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "PayBase.h"
#include "Security.h"
#include "Common.h"
#include "Config.h"

#include "GameList.h"
#include "SdkList.h"
#include "SdkChannelList.h"
#include "ChannelList.h"
#include "BillPointList.h"
#include "BillpointDataList.h"
#include "pay_error.h"
#include "IClient.h"
#include "Dict_PayStyle.h"
#include "UserCreditList.h"

void StrReplace(CMyString &str, const char *from, const char* to);
///////////////////////////////////////////////////////////////////////
CPayBase::CPayBase(const CMyString&sdkFlag, 
        enum DepositStyle depositStyle,
        enum HttpRequestStyle httpRequestStyle)
    : m_pDB(NULL)
    , m_sdkTag(sdkFlag)
    , m_depositStyle(depositStyle)
    , m_httpRequestStyle(httpRequestStyle)
    , m_creditRequestMethod(httpUsePost)
{
}

CPayBase::~CPayBase()
{
    SAFE_RELEASE(m_pDB)
}

bool CPayBase::InitRedisCache(redis_cache::CFmtCache &cache, CMyRedis &redis, CXmlFile &xml, const CMyString &sect)
{
    CMyString strFormat;
    CMyString strPatten;
    int32_t timeout(0);
    int32_t ret(0);
    CMyString strError;
    try
    {
        do {
            ret = xml.getString(sect, "keyFormat", strFormat);
            if (ret != 0)
            {
                strError.Format("get keyFormat value from sect [%s] failure, ret=%d", sect.c_str(), ret);
                break;
            }
            ret = xml.getString(sect, "keyPatten", strPatten);
            if (ret != 0)
            {
                strError.Format("get keyPattenvalue from sect [%s] failure, ret=%d", sect.c_str(), ret);
                break;
            }
            int32_t ret2 = xml.getInt32(sect, "timeout", timeout);
            if (ret2 != 0)
            {
                timeout = -1;
            }
        }while(false);
    }
    catch(std::exception &ex)
    {
        LogFatal("get redis_cache configure failure, error:%s", ex.what());
        return false;
    }

    if (ret != 0)
    {
        LogErr(strError.c_str());
        return false;
    }

    cache.Init(redis, strFormat, strPatten);
    cache.SetTimeout(timeout);

    return true;
}

bool CPayBase::InitRedisCache(redis_cache::CIncrCache &cache, CMyRedis &redis, CXmlFile &xml, const CMyString &sect)
{
    CMyString strFormat;
    CMyString strPatten;
    CMyString strMaxId;
    CMyString strCurrId;
    int32_t timeout(0);
    int32_t ret(0);
    CMyString strError;
    try
    {
        do {
            ret = xml.getString(sect, "currKey", strCurrId);
            if (ret != 0)
            {
                strError.Format("get currKey value from sect [%s] failure, ret=%d", sect.c_str(), ret);
                break;
            }
            ret = xml.getString(sect, "maxKey", strMaxId);
            if (ret != 0)
            {
                strError.Format("get maxKey value from sect [%s] failure, ret=%d", sect.c_str(), ret);
                break;
            }
            ret = xml.getString(sect, "keyFormat", strFormat);
            if (ret != 0)
            {
                strError.Format("get keyFormat value from sect [%s] failure, ret=%d", sect.c_str(), ret);
                break;
            }
            ret = xml.getString(sect, "keyPatten", strPatten);
            if (ret != 0)
            {
                strError.Format("get keyPattenvalue from sect [%s] failure, ret=%d", sect.c_str(), ret);
                break;
            }
            int32_t ret2 = xml.getInt32(sect, "timeout", timeout);
            if (ret2 != 0)
            {
                timeout = -1;
            }
        }while(false);
    }
    catch(std::exception &ex)
    {
        LogFatal("get redis_cache configure failure, error:%s", ex.what());
        return false;
    }

    if (ret != 0)
    {
        LogErr(strError.c_str());
        return false;
    }

    cache.Init(redis, strMaxId, strCurrId, strFormat, strPatten);
    cache.SetTimeout(timeout);

    return true;
}

int32_t CPayBase::Init(const CMyString &xmlSect)
{
    // 初始化mysql
    // 初始化redis
    CMyString sect_db = "main/paycenter/db";
    int32_t ret = open_mysql(sect_db, "main", m_pDB);
    if (ret != 0)
    {
        LogErr("[%s]can not open mysql config.", m_sdkTag.c_str());
        return -101;
    }

    m_redis.Close();
    ret = open_redis(sect_db, "redis", m_redis);
    if (ret != 0)
    {
        LogErr("[%s]can not open redis config.", m_sdkTag.c_str());
        return -103;
    }

    // 装载各个参数配置
    CMyString sect1("main/paycenter/order");
    CXmlFile &xml = CONFIG.getXml();
    CMyString strDelivers;
    try
    {
        if (xml.getString(sect1, "re-deliver", strDelivers) != 0)
        {
            strDelivers = "3,5,10,15,30,60,90";
        }
        if (xml.getString(xmlSect, "sdk", m_sdkCode) != 0)
        {
            LogErr("[%s]not found \"sdk\" field in sect [%s].", m_sdkTag.c_str(), xmlSect.c_str());
            return -105;
        }
    }
    catch(CException &ex)
    {
        LogErr("[%s]open config failure, error:%s", m_sdkTag.c_str(), ex.what());
        return -107;
    }
    catch(std::exception &ex)
    {
        LogErr("[%s]open config failure, error:%s", m_sdkTag.c_str(), ex.what());
        return -107;
    }
    catch(...)
    {
        LogErr("[%s]read config failure, unknown error.", m_sdkTag.c_str());
        return -107;
    }

    // 充值订单数据，保存到缓存，控制是否超时
    CMyString sect2("%s/deposit/callback", xmlSect.c_str());
    if (!this->InitRedisCache(m_cacheDepositOrder, m_redis, xml, sect2))
    {
        return -109;
    }

    // 充值发货数据，保存到缓存，用于定时重发
    CMyString sect3("%s/deposit/redeliver", xmlSect.c_str());
    if (!this->InitRedisCache(m_cacheDeliverGoods, m_redis, xml, sect3))
    {
        return -111;
    }

    // 提现等三方平台回调，保存到缓存，用于提速
    CMyString sect4("%s/credit/callback", xmlSect.c_str());
    if (!this->InitRedisCache(m_cacheCreditCB, m_redis, xml, sect4))
    {
        return -113;
    }

    // plugin_withdraw 模块缓存，用于指示提现二次请求数据
    CMyString sect5("main/paycenter/credit/token");
    if (!this->InitRedisCache(m_cacheCreditReq, m_redis, xml, sect5))
    {
        return -115;
    }

    // 提现回调数据
    CMyString sect6("main/paycenter/credit/respback");
    if(!this->InitRedisCache(m_cacheCreditRespBack, m_redis, xml, sect6))
    {
        return -116;
    }

    //告警中心
    CMyString sect7("main/service/alarm");
    ret = xml.getString(sect7, "url", m_strAlarmUrl);
    if (ret != 0)
    {
        return -117;
    }
    ret = xml.getString(sect7, "nodename", m_strNodeName);
    if (ret != 0)
    {
        return -118;
    }
    ret = xml.getInt32(sect7, "group", m_nGroup);
    if (ret != 0)
    {
        return -119;
    }
    int32_t nOpenOldAlt = 0;
    ret = xml.getInt32(sect7, "openoldalt", nOpenOldAlt);
    m_bOldAlt = (nOpenOldAlt == 1);

    // 构建重新申请发货的时间序列
    if (!strDelivers.empty())
    {
        m_reDeliverTimes.clear();
        CStrSpliter css;
        if (css.Split(strDelivers, ','))
        {
            for(int32_t idx(0); idx<css.size(); ++idx)
            {
                m_reDeliverTimes.push_back(css.asInt32(idx));
            }
        }
    }

    // 启动定时器列表
    m_timerDeliverGoods.SetInterval(1000);
    m_timerDeliverGoods.Startup();
    m_timerDepositOrder.SetInterval(3000);  // 每3秒检查
    m_timerDepositOrder.Startup();

    //机器人告警日志初始化
    CAlertLog::Instance().Init(CONFIG.getXml());

    LogInit("[%s]init pay object succeed.", m_sdkTag.c_str());
    return 0;
}

void CPayBase::Maintance(void)
{
    this->PingMysql();
    this->PingRedis();

    // 订单支付超时处理
    this->dealExpiredOrder();

    // 订单发货失败，重新发货
    this->dealReDeliver();

    // 充值验证模式时，处理待验证的订单
    this->dealDepositPendingVerify();

    // 兑出验证模式时，处理待验证的订单
    this->dealCreditPendingVerify();

    // 发起充值请求
    this->dealDepositRequest();
    // 发起充值验证请求
    this->dealDepositCheck();
    // 向第三方发起代付请求
    this->dealCreditOrder();
    // 处理请求第三方兑出时，对方就返回了callback的情况
    this->dealRespBack();

}

int32_t CPayBase::doDepositRequest(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers)
{
    /* 1、设置pClient的IP
     * 2、检查是否支付中心的订单
     * 3、创建新的订单号（存入DB）
     * 4、向三方平台发起请求
     * 5、成功则回馈支付URL或数据
     * 6、把订单数据放入到redis
     */
    ext_headers["Content-Type"] = "application/json";
    // 重置客户端IP
    CMyString clientIp = req.getParam("playerIp");
    LogDebug("[%s]deposit player ip is [%s]", m_sdkTag.c_str(), clientIp.c_str());
    if (clientIp.empty() || clientIp=="0.0.0.0")
    {
        clientIp = getClientIp(pClient, req);
        LogDebug("[%s]get paycenter config ip %s", m_sdkTag.c_str(), clientIp.c_str());
    }
    pClient->setIp(clientIp);

    m_errCode = 0;
    HTTP_REQ_STR(gameCode, gameCode);
    HTTP_REQ_STR(strUin, uin);
    HTTP_REQ_INT32(bpId, bpId);

    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    if (NULL == pBP)
    {
        m_errCode = err_depositParameterFault;
        LogMsg("[%s]deposit request not found billpoint %d", m_sdkTag.c_str(), bpId);

        Json::Value resp;
        resp["code"] = m_errCode;
        resp["message"] = "deposit request not found billpoint";
        this->OnPayResp(resp, szResp, cbResp);
        return m_errCode;
    }

    if (!(pBP->CanBuy(strUin)))
    {
        m_errCode = err_depositBillPointCannotBuy;
        LogMsg("[%s]deposit request found billpoint %d, billpoint can't buy", m_sdkTag.c_str(), bpId);

        Json::Value resp;
        resp["code"] = m_errCode;
        resp["message"] = "purchase times of this billpoint up to the limit";
        this->OnPayResp(resp, szResp, cbResp);
        return m_errCode;
    }

    // 检查是否合法的paycenter请求
    int32_t channelId(0);
    CSdkChannel *pChannel(NULL);
    CSdk *pSdk(NULL);
    CGame *pGame(NULL);
    int32_t orderId(0);
    CMyString orderNo;
    bool ret = req.getParam("paycenter_channelId", channelId);
    do {
        if (!ret || channelId<=0)
        {
            m_errCode = err_depositMissParameter;
            m_errNote.Format("not valid paycenter api");
            LogWarn("[%s]not found field \"paycenter_channelId\" in http req parameter.",
                m_sdkTag.c_str());
            break;
        }
        pChannel = SDKCHANNEL_LIST.Find(channelId);
        if (NULL == pChannel)
        {
            m_errCode = err_depositParameterFault;
            m_errNote.Format("invalid sdk-channel %d", channelId);
            LogWarn("[%s]channelId %s not avaiable in system.", m_sdkTag.c_str(), channelId);
            break;
        }

        pSdk = pChannel->getSdk();
        pGame = pChannel->getGame();
        if (NULL == pSdk || NULL==pGame)
        {
            m_errCode = err_depositSystemCorrupted;
            m_errNote.Format("invalid sdk or game config for sdk-channel %d", channelId);
            LogFatal("[%s][%s:%s:%d]CSdkChannel object maybe corrupted when user %s request deposit.",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str());
            this->postAlert(gameCode, CMyString("系统错误=>用户 %s 发起充值请求时未找到CSdk及CGame对象", strUin.c_str()));
            break;
        }

        // 创建新的订单（写入DB）
        ret = this->createDepositOrder(req, pSdk, pGame, orderId, orderNo);
        if (!ret)
        {
            break;
        }
    }while(0);
    int64_t fee = req.getParamInt64("amount");
    if (m_errCode != 0)
    {
        this->doPayFailure(gameCode, strUin, orderNo, fee, szResp, cbResp);
        return m_errCode;
    }

    // 构建HTTP请求
    CMyString platformUrl;              // 三方平台返回的URL或其他数据
    CMyString platformOrderNo("N/A");   // 三方平台的订单号
    int32_t httpCost(0);                // 以微秒统计
    if (m_depositStyle == depositRequest)
    {
        ret = this->createDepositRequest(req, pGame, pSdk, pChannel, orderId, orderNo, 
            platformUrl, platformOrderNo, httpCost);
        if (!ret)
        {
            // 数据库中的订单出错，取消
            this->updateDepositOrder(pGame->getCode(), orderId, deposit_RequestFail, platformOrderNo);

            this->doPayFailure(gameCode, strUin, orderNo, fee, szResp, cbResp);
            return m_errCode;
        }
    }

    // 更新订单状态
    this->updateDepositOrder(pGame->getCode(), orderId, deposit_RequestSucceed, platformOrderNo);

    // 保存到redis中,用于回调检查之用
    this->storeDepositOrder(req, orderId, orderNo);

    // 构建返回信息给游服
    this->doPayResp(pChannel, platformUrl, orderNo, szResp, cbResp);

    LogMsg("[%s]request deposit succeed, gameCode:%s, uin:%s, orderNo:%s",
            m_sdkTag.c_str(), gameCode.c_str(), strUin.c_str(), orderNo.c_str());
    return 0;
}

int32_t CPayBase::doDepositCallback(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers)
{
    CMyString strIp = getClientIp(pClient, req);
    if (!strIp.empty())
    {
        pClient->setIp(strIp);
    }
    // TODO: 查询IP对应的regionCode
    CMyString regionCode("N/A");

    CMyString orderNo;
    CMyString platformOrderNo;
    CMyString strUin;
    CMyString gameCode;
    CMyString itemInfo;
    CSdkChannel *pChannel(NULL);
    CGame *pGame(NULL);
    CSdk* pSdk(NULL);
    bool depositedOK(false);
    Json::Value req_data;
    int32_t orderId(0);
    int32_t bpId(0);
    int64_t fee(0);         // 充值金额
    int64_t realFee(0);     // 实际到账金额
    int64_t tax(0);         // 手续费
    int64_t coin(0);
    int64_t ext_coin(0);
    m_errCode = 0;
    m_errNote.clear();
    do {
        // 解析出订单号
        // 虚函数调用,用于解析充值回调的结果
        bool val(false);
        try
        {
            val = this->doParseDepositCallback(req, orderNo, platformOrderNo, fee, realFee, tax, depositedOK);
        }
        catch(CException &ex)
        {
            LogFatal("[%s][%s:%s:%d]call doParseDepositCallback failure, msg:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, ex.what());
            val = false;
        }
        catch(std::exception &ex)
        {
            LogFatal("[%s][%s:%s:%d]call doParseDepositCallback failure, msg:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, ex.what());
            val = false;
        }
        catch(...)
        {
            LogFatal("[%s][%s:%s:%d]call doParseDepositCallback failure, unknown error", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
            val = false;
        }
        if (!val)
        {
            // 回调数据解析失败,在虚函数中要记录错误信息
            m_errCode = err_depositCallbackDataCorrupted; 
            m_errNote.Format("[%s]parse deposit callback data failure", m_sdkTag.c_str());
            break;
        }
    
        // 查询订单数据
        int32_t orderStatus = -1;
        if (!this->fetchDepositOrderData(req, orderNo, req_data, orderStatus))
        {
            if (orderStatus == deposit_CreateOrder)
            {
                bool b = delayedDealDepositCallback(req, orderNo);
                if (b)
                {
                    this->doDepositCallbackResp(true, 0, ext_headers, szResp, cbResp);
                    return 200;
                }
            }
            m_errCode = err_depositDataLost;
            // 查无此订单号
            break;
        }
        orderId = req_data["payId"].asInt();
        strUin = req_data["uin"].asString();
        bpId = req_data["billpoint"].asInt();
        fee = req_data["fee"].asInt64();
        gameCode = req_data["gameCode"].asString();
        LogWarn("[%s]DELIVER-GOODS>bpId:%d, orderId:%d, uin:%s, fee:%ld, orderNo:%s", 
                m_sdkTag.c_str(), bpId, orderId, strUin.c_str(), fee, orderNo.c_str());

        // 删除缓存数据
        m_cacheDepositOrder.Del(orderNo);

        // 查询SDK数据
        int32_t channelId = req_data["channelId"].asInt();
        pChannel = SDKCHANNEL_LIST.Find(channelId);
        if (NULL == pChannel)
        {
            m_errCode = err_depositParameterFault;
            m_errNote.Format("invalid sdk-channel %d", channelId);
            LogWarn("[%s]channelId %s not avaiable in system.", m_sdkTag.c_str(), channelId);
            break;
        }
        pGame = GAME_LIST.FindByCode(gameCode);
        if (NULL == pGame)
        {
            m_errCode = err_depositParameterFault;
            m_errNote.Format("[%s]invalid gameCode %s when deposit callback.",
                m_sdkTag.c_str(), gameCode.c_str());
            LogFatal("[%s][%s:%s:%d]invalid gameCode %s when deposit order %s callback",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, gameCode.c_str(), orderNo.c_str());    
            break;
        }
        pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
        if (NULL == pSdk)
        {
            m_errCode = err_depositParameterFault;
            m_errNote.Format("[%s]invalid sdk %s (gameCode %s) when deposit callback.",
                m_sdkTag.c_str(), m_sdkCode.c_str(), gameCode.c_str());
            LogFatal("[%s][%s:%s:%d]invalid sdk config when deposit order %s callback, gameCode:%s, sdkCode:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), gameCode.c_str(), m_sdkCode.c_str());
            this->postAlert(gameCode, CMyString("系统错误=>用户 %s 充值订单 %s 回调时无法获得CSdk对象", strUin.c_str(), orderNo.c_str()));
            break;
        }

        // 校验回调IP是否合法
        if (!this->validDepositCallbackIp(pSdk, strIp))
        {
            m_errCode = err_depositCallbackIpInvalid;
            break;
        }

        try
        {
            // 虚函数调用，用于验签
            val = this->validDepositCallbackSign(req, pSdk, pChannel, m_errNote);
        }
        catch(CException &ex)
        {
            LogFatal("[%s]call validDepositCallbackSign failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(std::exception &ex)
        {
            LogFatal("[%s]call validDepositCallbackSign failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(...)
        {
            LogFatal("[%s]call validDepositCallbackSign failure, unknown error", m_sdkTag.c_str());
            val = false;
        }
        if (!val)
        {
            m_errCode = err_depositCallbackSignMismatch;
            break;
        }

        // 获得充值对应的物品信息
        this->getDepositWealth(pGame, bpId, fee, coin, ext_coin, itemInfo);

        // 更新DB中的状态
        this->updateDepositNotify(gameCode, orderId, (depositedOK ? deposit_MoneyArrival : deposit_MoneyFail), 
            platformOrderNo, realFee, regionCode);

        //收到钱，更新用户总充值额
        if (depositedOK)
        {
            bool b = USERCREDIT_LIST.AddDeposit(m_pDB, gameCode, strUin, realFee);
            LogMsg("[%s] %s add deposit %ld, result: %s", m_sdkTag.c_str(), strUin.c_str(), realFee, b ? "true" : "false");
        }

        // 不论是否支付成功，都向游服发送通知
        this->judgeBPInfo(pGame, bpId, fee, strUin, req_data, coin, ext_coin, itemInfo);

        //上一步判断完成后再更新计费点使用次数，否则最后一次就无法购买
        if (depositedOK)
        {
            BPDATA_LIST.IncBill(strUin, bpId);
            // 通知其他进程刷新数据
            Json::Value data;
            data["uin"] = strUin;
            data["bpId"] = bpId;
            Broadcast2Sibling(sc_increaseBillpoint, data);
        }

        req_data["code"] = depositedOK ? 0 : -1;
        req_data["realFee"] = (Json::Int64)realFee;
        req_data["coin"] = (Json::Int64)coin;
        req_data["extraCoin"] = (Json::Int64)ext_coin;
        req_data["itemInfo"] = itemInfo;
        this->postDepositResultToGamesvr(pGame, pSdk, req_data);
    }while(0);

    try
    {
        // 回调给第三方平台
        switch(m_errCode)
        {
        case err_depositCallbackDataCorrupted:
        case err_depositDataLost:
        case err_depositCallbackIpInvalid:
        case err_depositCallbackSignMismatch:
            // 失败, 虚函数调用
            this->doDepositCallbackResp(false, m_errCode, ext_headers, szResp, cbResp);
            break;
        case err_depositParameterFault:
        case err_depositOrderUpdateFailure:
        case err_depositDeliverHostFail:
            // 成功,虚函数调用
            this->doDepositCallbackResp(true, m_errCode, ext_headers, szResp, cbResp);
            break;
        default:
            // 其他状态，认为成功，虚函数调用
            this->doDepositCallbackResp(true, m_errCode, ext_headers, szResp, cbResp);
            break;
        }
    }
    catch(CException &ex)
    {
        LogFatal("[%s]call doDepositCallbackResp failure, m_errCode:%d, msg:%s", m_sdkTag.c_str(), m_errCode, ex.what());
    }
    catch(std::exception &ex)
    {
        LogFatal("[%s]call doDepositCallbackResp failure, m_errCode:%d, msg:%s", m_sdkTag.c_str(), m_errCode, ex.what());
    }
    catch(...)
    {
        LogFatal("[%s]call doDepositCallbackResp failure, m_errCode:%d, unknown error", m_sdkTag.c_str(), m_errCode);
    }

    return 200;
}

bool CPayBase::checkDepositVerifySign(http::CHttpRequest &req, const CMyString &gameApiKey, Json::Value &resp_data)
{
    HTTP_REQ_STR(sign, sign);

    http::CHttpRespMaker reqVerify;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    req.BuildRespMaker_Except(vecExcept, reqVerify);

    CMyString str1 = reqVerify.http_str();
    str1.Append("&merchantKey=%s", gameApiKey.c_str());
    CMyString strSign = CMD5Wrapper::encode(str1.c_str());
    if (0 != strcasecmp(strSign.c_str(), sign.c_str()))
    {
        LogErr("[%s][%s:%s:%d]deposit verify sign mismatch, origin[%s], sign[%s], sign from request[%s]", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, str1.c_str(), strSign.c_str(), sign.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]deposit verify sign mismatch, origin[%s], sign[%s], sign from request[%s]",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, str1.c_str(), strSign.c_str(), sign.c_str()));

        resp_data["origin"] = str1;
        resp_data["sign"] = strSign;
        resp_data["sign_from_req"] = sign;
        return false;
    }
    return true;
}
int32_t CPayBase::doDepositVerify(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers)
{
    CMyString strIp = getClientIp(pClient, req);
    if (!strIp.empty())
    {
        pClient->setIp(strIp);
    }

    std::vector<CMyString> vecParam;
    bool ret(false);
    CMyString gameCode;
    CMyString orderNo;
    CMyString strUin;
    Json::Value req_data;
    CGame* pGame(NULL);
    CSdk* pSdk(NULL);
    Json::Value resp_data;
    do{
        // 检查参数是否合法: gameCode, uin, orderNo, sign 必不可少
        // 调用虚函数：确认必须包含的参数
        vecParam.push_back("gameCode"); 
        vecParam.push_back("uin");
        // vecParam.push_back("token");
        vecParam.push_back("orderNo");
        vecParam.push_back("sign");
        this->addDepositVerifyNeededParam(vecParam);
        ret = req.ValidParam(vecParam, m_errNote);
        if (!ret)
        {
            LogErr("[%s][%s:%s:%d]missing parameter [%s] when deposit verify.", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_errNote.c_str());
            this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]missing parameter [%s] when deposit verify.",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_errNote.c_str()));
            m_errCode = err_depositMissParameter;
            break;
        }

        // 检查部分参数的合法性
        gameCode = req.getParam("gameCode");
        strUin = req.getParam("uin");
        pGame = GAME_LIST.FindByCode(gameCode);
        if (NULL == pGame)
        {
            m_errCode = err_depositParameterFault;
            m_errNote.Format("invalid gameCode %s", gameCode.c_str());
            LogFatal("[%s][%s:%s:%d]invalid gameCode %s when deposit order %s verify",
                m_sdkTag.c_str(),  __FILE__, __func__, __LINE__, gameCode.c_str(), orderNo.c_str());
            resp_data["gameCode"] = gameCode;
            break;
        }
        orderNo = req.getParam("orderNo");
        pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
        if (NULL == pSdk)
        {
            m_errCode = err_depositParameterFault;
            m_errNote.Format("invalid sdk %s", m_sdkCode.c_str());
            LogFatal("[%s][%s:%s:%d]invalid sdk config when deposit order %s verify, gameCode:%s, sdkCode:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), gameCode.c_str(), m_sdkCode.c_str());
            this->postAlert(gameCode, CMyString("系统错误=>用户 %s 充值订单 %s 验证时无法获得CSdk对象", strUin.c_str(), orderNo.c_str()));
            resp_data["gameCode"] = gameCode;
            resp_data["sdkCode"] = m_sdkCode;
            break;
        }
        
        // 验签
        ret = this->checkDepositVerifySign(req, pGame->getApiKey(), resp_data);
        if (!ret)
        {
            m_errCode = err_depositVerifySignMismatch;
            m_errNote = "deposit verify sign mismatch";
            resp_data["gameCode"] = gameCode;
            resp_data["sdkCode"] = m_sdkCode;
            resp_data["orderNo"] = orderNo;
            break;
        }

        // 获得得订订数据
        int32_t orderStatus = -1;
        if (!this->fetchDepositOrderData(req, orderNo, req_data, orderStatus))
        {
            m_errCode = err_depositDataLost;
            m_errNote.Format("not found deposit order %s", orderNo.c_str());
            LogErr("[%s][%s:%s:%d]invalid gameCode %s when deposit order %s verify",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, gameCode.c_str(), orderNo.c_str());
            resp_data["gameCode"] = gameCode;
            resp_data["orderNo"] = orderNo;
            break;
        }
        // 移除redis中保存的数据
        m_cacheDepositOrder.Del(orderNo);

        // 虚函数：检查订单是否重复发送
        ret = this->checkDepositVerifyDuplicate(req);
        if (!ret)
        {
            m_errCode = err_depositVerifyDuplicate;
            m_errNote.Format("duplicate deposit");
            LogErr("[%s][%s:%s:%d]duplicate deposit order %s when verified.", 
                    m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str());
            this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]duplicate deposit order %s when verified.",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str()));
            resp_data["gameCode"] = gameCode;
            resp_data["orderNo"] = orderNo;
            break;
        }
        
        // 虚函数：保存结果，以待循环中处理, 至少要有以下数据: gameCode, uin, bpId, fee
        this->storeDepositVerifyData(pGame, pSdk, req, req_data);
        m_errCode = 0;
        m_errNote = "pending result";
        resp_data["gameCode"] = gameCode;
        resp_data["orderNo"] = orderNo;
    }while(0);

    // 返回给游服, code=0表示成功，其他是失败
    Json::Value resp;
    resp["code"] = m_errCode;
    resp["message"] = m_errNote;
    resp["data"] = resp_data;
    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);

    return 0;
}

int32_t CPayBase::doDepositQuery(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers)
{
    CMyString strIp = getClientIp(pClient, req);
    if (!strIp.empty())
    {
        pClient->setIp(strIp);
    }

    bool val(false);
    CGame *pGame(NULL);
    CSdk *pSdk(NULL);
    Json::Value resp;
    CMyString url;
    CMyString contentType;
    CMyString httpData;
    int32_t httpCost(0);
    do {
        // 检查订单参数是否合法
        val = this->validQueryParam(req, resp, pGame);
        if (!val)
        {
            // 出错，返回
            break;
        }
        pSdk = SDK_LIST.FindByCode(pGame->getCode(), m_sdkCode);
        if (NULL == pSdk)
        {
            val = false;
            resp["code"] = m_errCode;
            resp["message"] = "system fault when get SDK";
            LogFatal("[%s]system data corrupted when query deposit order", m_sdkTag.c_str());
            this->postAlert(pGame->getCode(), CMyString("系统错误=>用户查询订单时无法获得CSdk对象"));
            break;
        }

        // 发起订单查询，返回结果
        val = this->createDepositQueryRequest(req, pGame, pSdk, resp, httpCost);
        if (val)
        {
            resp["code"] = 0;
            resp["message"] = "OK";
            // 额外的数据在 doParseDepositQueryResp 中设置
        }
    } while(0);

    // 返回给前端
    ext_headers["Content-Type"] = "application/json";
    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);

    return 200;
}

int32_t CPayBase::doCreditRequest(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers)
{
    CMyString strIp = getClientIp(pClient, req);
    if (!strIp.empty())
    {
        pClient->setIp(strIp);
    }

    HTTP_REQ_STR(strUin, uin);
    HTTP_REQ_STR(orderNo, orderNo);     //商户订单号
    HTTP_REQ_STR(creditToken, token);
    bool val(false);
    CGame *pGame(NULL);
    Json::Value resp;
    Json::Value req_data;   // credit请求的数据，plugin_withdraw保存的
    do
    {
        // 检查传入参数是否合法, 以及签名是否合法
        val = this->validCreditParam(req, resp, pGame);
        if (!val)
        {
            // 返回参数，说是提现参数不合法
            break;
        }

        // 检查发起IP是否合法
        val = this->validCreditIp(strIp);
        if (!val)
        {
            resp["code"] = err_creditIpInvalid;
            resp["message"] = CMyString("credit ip %s invalid", strIp.c_str());
            LogErr("[%s][%s:%s:%d]invalid credit ip %s, uin:%s, orderNo:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strIp.c_str(), strUin.c_str(), orderNo.c_str());
            this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]invalid credit ip %s, uin:%s, orderNo:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strIp.c_str(), strUin.c_str(), orderNo.c_str()));
            break;
        }

        // 校验TOKEN是否合法
        val = this->validCreditToken(orderNo, creditToken, req_data, resp);
        if (!val)
        {
            break;
        }
    } while (0);
    if (!val)
    {
        this->OnPayResp(resp, szResp, cbResp);
        return 400;
    }

    CSdk *pSdk(NULL);
    // 发送提现请求到三方平台
    CMyString creditOrderNo;
    int32_t creditCost(0);
    int32_t result(0);
    do{
        m_errNote.clear();
        if (!this->postCreditRequest(req_data, resp, pGame, pSdk, creditOrderNo, creditCost))
        {
            result = credit_SdkLoanFail;
            break;
        }

        resp["code"] = 0;
        resp["message"] = "OK";
        if (!m_errNote.empty())
        {
            CJsonWrapper jsResp;
            if (jsResp.Init(m_errNote))
            {
                resp["data"] = jsResp.getJson();
            }
            else
            {
                resp["data"] = m_errNote;
            }
            m_errNote.clear();
        }

        result = credit_LoanBySDK;
    } while (0);

    // 记录到数据库，等待回调
    CMyString creditRemark = req_data["creditRemark"].asString();
    int64_t creditTime = req_data["creditTime"].asInt64();
    CMyString creditOp = req_data["operator"].asString();
    this->updateCreditOrder(pGame->getCode(), creditRemark, creditTime, creditOp, orderNo, creditOrderNo, result);

    // 写redis需要放在写数据库之后，否则会造成其他进程callback回来后读取到redis数据直接继续执行，先把状态写成10后，这边本进程才写数据库，又把数据改成5
    if (result == credit_LoanBySDK)
    {
        int64_t fee = req_data["fee"].asInt64();
        // 写入redis内存表
        this->storeCreditData(pGame, pSdk, strUin, orderNo, fee, creditCost);
    }

    // 写入提现日志数据库-提现发放
    this->setWithdrawLog(orderNo, result, creditOp, creditRemark, resp["message"].asString(), creditTime);

    CMyString strParam;
    CMyString strKey;
    int32_t ret = m_cacheCreditRespBack.Pop(orderNo, strKey, strParam);
    if (ret > 0 && !strParam.empty())
    {
        m_qRespBackParam.push(strParam);
    }

    // 回传给web操作端
    this->OnPayResp(resp, szResp, cbResp);

    return 200;
}

int32_t CPayBase::doCreditCallback(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers)
{
    CMyString strIp = getClientIp(pClient, req);
    if (!strIp.empty())
    {
        pClient->setIp(strIp);
    }

    bool val(false);
    CMyString orderNo;
    CMyString creditOrderNo;
    CMyString gameCode;
    Json::Value req_data;
    CGame *pGame(NULL);
    CSdk *pSdk(NULL);
    int64_t fee(0);         // 提现金额, 以分显示
    int64_t realFee(0);     // 实际到账金额，以分显示
    int64_t tax(0);         // 手续费，以分显示
    bool loanOK(false);
    enum CreditStatus creditStatus = credit_SdkLoanFault;
    do {
        try
        {
            // 虚函数回调，获得回调结果返回的订单号、结果等
            val = this->doParseCreditCallback(req, orderNo, creditOrderNo, fee, realFee, tax, loanOK);
        }
        catch(CException &ex)
        {
            LogFatal("[%s][%s:%s:%d]call doParseCreditCallback failure, msg:%s", m_sdkTag.c_str(),  __FILE__, __func__, __LINE__, ex.what());
            val = false;
        }
        catch(std::exception &ex)
        {
            LogFatal("[%s][%s:%s:%d]call doParseCreditCallback failure, msg:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, ex.what());
            val = false;
        }
        catch(...)
        {
            LogFatal("[%s][%s:%s:%d]call doParseCreditCallback failure, unknown error", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
            val = false;
        }
        if (!val)
        {
            m_errCode = err_creditCallbackFault;
            break;
        }

        // 查询订单数据
        val = this->fetchCreditOrderData(orderNo, req_data);
        if (!val)
        {
            // 查无此订单号
            break;
        }

        gameCode = req_data["gameCode"].asString();
        pGame = GAME_LIST.FindByCode(gameCode);
        if (NULL == pGame)
        {
            val = false;
            m_errCode = err_creditParameterFault;

            CMyString params;
            req.getFullParamStr(params);
            LogMsg("[%s]credit callback, param[%s]", m_sdkTag.c_str(), params.c_str());
            m_cacheCreditRespBack.Push(orderNo, params);

            // 返回结果给三方平台
            this->doCreditCallbackResp(true, 0, ext_headers, szResp, cbResp);
            return 200;
            
            //break;
        }
        pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
        if (NULL == pSdk)
        {
            val = false;
            m_errCode = err_creditSystemFault;
            this->postAlert(gameCode, CMyString("系统错误=>提现订单 %s 在结果回调时无法获得CSdk对象", orderNo.c_str()));
            break;
        }
        // 校验回调IP
        val = this->validCreditCallbackIp(strIp, pSdk);
        if (!val)
        {
            m_errCode = err_creditCallbackIpInvalid;
            break;
        }

        try
        {
            // 虚函数调用，验签
            val = this->validCreditCallbackSign(req, pGame, pSdk, m_errNote );
        }
        catch(CException &ex)
        {
            LogFatal("[%s]call validCreditCallbackSign failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(std::exception &ex)
        {
            LogFatal("[%s]call validCreditCallbackSign failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(...)
        {
            LogFatal("[%s]call validCreditCallbackSign failure", m_sdkTag.c_str());
            val = false;
        }
        if (!val)
        {
            m_errCode = err_creditCallbackSignMismatch;
            break;
        }

    }while(0);
    if (!val)
    {
        try
        {
            // 虚函数调用，返回给三方平台
            this->doCreditCallbackResp(false, m_errCode, ext_headers, szResp, cbResp);
        }
        catch(CException &ex)
        {
            LogFatal("[%s]call validCreditCallbackSign failure, msg:%s", m_sdkTag.c_str(), ex.what());
        }
        catch(std::exception &ex)
        {
            LogFatal("[%s]call validCreditCallbackSign failure, msg:%s", m_sdkTag.c_str(), ex.what());
        }
        catch(...)
        {
            LogFatal("[%s]call validCreditCallbackSign failure", m_sdkTag.c_str());
        }
        return 500;
    }

    creditStatus = loanOK ? credit_SdkLoanSucceed : credit_SdkLoanReject;
    LogDebug("[%s]credit callback, status:%d", m_sdkTag.c_str(), creditStatus);
    //更新税款到数据库
    if (tax > 0)
    {
        this->updateCreditTax(gameCode, orderNo, fee, realFee, tax);
    }
    // 回传到到数据库
    val = this->finishCreditOrder(gameCode, orderNo, creditOrderNo, creditStatus);

    // 通知游服处理
    CMyString strMessage = "OK";
    bool bSuccess = this->postCreditResultToGamesvr(pGame, pSdk, orderNo, fee, realFee, creditStatus);
    if (!bSuccess)
    {
        strMessage = m_errNote;
    }
    // 更新提现日志数据库-游服处理结果
    this->updateWithdrawLog(orderNo, creditStatus, strMessage);

    // 返回结果给三方平台
    this->doCreditCallbackResp(true, 0, ext_headers, szResp, cbResp);

    return 200;
}

bool CPayBase::checkCreditVerifySign(http::CHttpRequest &req, const CMyString &gameApiKey, Json::Value &resp_data)
{
    HTTP_REQ_STR(sign, sign);

    http::CHttpRespMaker reqVerify;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    req.BuildRespMaker_Except(vecExcept, reqVerify);

    CMyString str1 = reqVerify.http_str();
    str1.Append("&merchantKey=%s", gameApiKey.c_str());
    CMyString strSign = CMD5Wrapper::encode(str1.c_str());
    if (0 != strcasecmp(strSign.c_str(), sign.c_str()))
    {
        LogErr("[%s][%s:%s:%d]deposit verify sign mismatch, origin[%s], sign[%s], sign from request[%s]", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, str1.c_str(), strSign.c_str(), sign.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]deposit verify sign mismatch, origin[%s], sign[%s], sign from request[%s]",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, str1.c_str(), strSign.c_str(), sign.c_str()));

        resp_data["origin"] = str1;
        resp_data["sign"] = strSign;
        resp_data["sign_from_req"] = sign;
        return false;
    }
    return true;
}

int32_t CPayBase::doCreditVerify(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers)
{
    CMyString strIp = getClientIp(pClient, req);
    if (!strIp.empty())
    {
        pClient->setIp(strIp);
    }

    std::vector<CMyString> vecParam;
    bool ret(false);
    CMyString gameCode;
    CMyString orderNo;
    CMyString strUin;
    Json::Value req_data;
    CGame* pGame(NULL);
    CSdk* pSdk(NULL);
    Json::Value resp_data;
    do{
        // 检查参数是否合法: gameCode, uin, orderNo, sign 必不可少
        // 调用虚函数：确认必须包含的参数
        vecParam.push_back("gameCode"); 
        vecParam.push_back("uin");
        vecParam.push_back("orderNo");
        vecParam.push_back("sign");
        this->addCreditVerifyNeededParam(vecParam);
        ret = req.ValidParam(vecParam, m_errNote);
        if (!ret)
        {
            LogErr("[%s][%s:%s:%d]missing parameter [%s] when credit verify.", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_errNote.c_str());
            this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]missing parameter [%s] when credit verify.",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_errNote.c_str()));
            m_errCode = err_creditMissParameter;
            break;
        }

        // 检查部分参数的合法性
        gameCode = req.getParam("gameCode");
        strUin = req.getParam("uin");
        pGame = GAME_LIST.FindByCode(gameCode);
        if (NULL == pGame)
        {
            m_errCode = err_creditParameterFault;
            m_errNote.Format("invalid gameCode %s", gameCode.c_str());
            LogFatal("[%s][%s:%s:%d]invalid gameCode %s when credit order %s verify",
                m_sdkTag.c_str(),  __FILE__, __func__, __LINE__, gameCode.c_str(), orderNo.c_str());
            resp_data["gameCode"] = gameCode;
            break;
        }
        orderNo = req.getParam("orderNo");
        pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
        if (NULL == pSdk)
        {
            m_errCode = err_creditParameterFault;
            m_errNote.Format("invalid sdk %s", m_sdkCode.c_str());
            LogFatal("[%s][%s:%s:%d]invalid sdk config when credit order %s verify, gameCode:%s, sdkCode:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), gameCode.c_str(), m_sdkCode.c_str());
            this->postAlert(gameCode, CMyString("系统错误=>用户 %s 充值订单 %s 验证时无法获得CSdk对象", strUin.c_str(), orderNo.c_str()));
            resp_data["gameCode"] = gameCode;
            resp_data["sdkCode"] = m_sdkCode;
            break;
        }
        
        // 验签
        ret = this->checkCreditVerifySign(req, pGame->getApiKey(), resp_data);
        if (!ret)
        {
            m_errCode = err_creditSignMismatch;
            m_errNote = "credit verify sign mismatch";
            resp_data["gameCode"] = gameCode;
            resp_data["sdkCode"] = m_sdkCode;
            resp_data["orderNo"] = orderNo;
            break;
        }
        
        // 虚函数：保存结果，以待循环中处理, 至少要有以下数据: gameCode, uin, bpId, fee
        this->storeCreditVerifyData(pGame, pSdk, req, req_data, resp_data);
        m_errCode = 0;
        m_errNote = "pending result";
        resp_data["gameCode"] = gameCode;
        resp_data["orderNo"] = orderNo;
    }while(0);

    // 返回给游服, code=0表示成功，其他是失败
    Json::Value resp;
    resp["code"] = m_errCode;
    resp["message"] = m_errNote;
    resp["data"] = resp_data;
    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);

    return 0;
}

int32_t CPayBase::doWithdrawQuery(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers)
{
    CMyString strIp = getClientIp(pClient, req);
    if (!strIp.empty())
    {
        pClient->setIp(strIp);
    }

    bool val(false);
    CGame *pGame(NULL);
    CSdk *pSdk(NULL);
    Json::Value resp;
    CMyString url;
    CMyString contentType;
    CMyString httpData;
    int32_t httpCost(0);
    do {
        // 检查订单参数是否合法
        val = this->validQueryParam(req, resp, pGame);
        if (!val)
        {
            // 出错，返回
            break;
        }
        pSdk = SDK_LIST.FindByCode(pGame->getCode(), m_sdkCode);
        if (NULL == pSdk)
        {
            val = false;
            resp["code"] = m_errCode;
            resp["message"] = "system fault when get SDK";
            LogFatal("[%s]system data corrupted when query credit order", m_sdkTag.c_str());
            this->postAlert(pGame->getCode(), CMyString("系统错误=>在查询订单状态时无法获得CSdk对象"));
            break;
        }

        // 发起订单查询，返回结果
        val = this->createCreditQueryRequest(req, pGame, pSdk, resp, httpCost);
        if (val)
        {
            resp["code"] = 0;
            resp["message"] = "OK";
            // 额外的数据在 doParseDepositQueryResp 中设置
        }
    } while(0);

    // 返回给前端
    ext_headers["Content-Type"] = "application/json";
    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);

    return 200;
}

bool CPayBase::createDepositOrder(http::CHttpRequest& req, CSdk* pSdk, CGame* pGame, int32_t& orderId, CMyString& orderNo)
{
    HTTP_REQ_INT32(configId, configId);
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
    HTTP_REQ_INT32(bpId, paycenter_bpId);
    HTTP_REQ_INT32(channelId, paycenter_channelId);
    // 对呢称进行base64编码
    CMyString strName = CBase64Wrapper::Encode(userName.c_str(), userName.length(), false);
    LogDebug("[%s]origin name[%s], base64 name[%s]", m_sdkTag.c_str(), userName.c_str(), strName.c_str());


    int32_t code(0);
    CMyString strMsg;
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
        orderId = m_pDB->GetInt(2);
        orderNo = m_pDB->GetString(3);
    }
    char szError[128] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        m_errCode = err_depositCreateOrderFailure;
        m_errNote.Format("save order data failure, errno:%d, error:%s", err, szError);
        LogFatal("[%s][%s:%s:%d]save deposit order into mysql failure, user:%s, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str(), err, szError);
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]save deposit order into mysql failure, user:%s, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str(), err, szError));
        this->postAlert(gameCode, CMyString("数据库错误=>创建用户 %s 充值订单错误[%s]", strUin.c_str(), szError));
        return false;
    }
    if (code < 0 || orderNo.empty())
    {
        m_errCode = err_depositCreateOrderFailure;
        m_errNote = strMsg;
        LogFatal("[%s][%s:%s:%d]save deposit order into mysql failure, user:%s, unknown error.",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str());
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]save deposit order into mysql failure, user:%s, unknown error.",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str()));
        this->postAlert(gameCode, CMyString("数据库错误=>创建用户 %s 充值订单错误[%s]", strUin.c_str(), strMsg.c_str()));
        return false;
    }

    return true;
}

int32_t CPayBase::doDepositHttpRequest(http::CHttpClient &net, const CMyString &strUrl, http::VEC_STRING &header_list, const CMyString &httpData, CMyString &strResp, CMyString &strRespHeader)
{
    //TODO: 此函数居然会发生 signal 11 的异常
    //http request error, reason: curl_easy_perform error :Timeout was reached
    //需要检查一下此段代码
        
    if (m_creditRequestMethod == httpUsePost)
    {
        return net.HttpPostWithHeaders(strUrl.c_str(),
                strResp, strRespHeader, header_list,
                httpData.c_str());
    }
    else
    {
        CMyString strFullUrl = strUrl + "?" + httpData;
        return net.HttpGetWithHeaders(strFullUrl.c_str(), 
                strResp, strRespHeader, header_list);
    }
}

bool CPayBase::createDepositRequest(http::CHttpRequest &req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, 
        int32_t orderId, const CMyString &orderNo, CMyString& platformUrl, CMyString& platformOrderNo, int32_t &cost)
{
    // 构建HTTP请求相关数据
    CMyString httpData;
    CMyString strUrl;
    CMyString contentType;
    HTTP_REQ_STR(strUin, uin);

    // 虚函数调用
    int64_t t1 = Now(true);
    try
    {
        this->doMakeDepositRequest(req, pGame, pSdk, pChannel, orderNo, strUrl, contentType, httpData);
    }
    catch(CException &ex)
    {
        LogFatal("[%s][%s:%s:%d]call doMakeDepositRequest failure, msg:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, ex.what());
        return false;
    }
    catch(std::exception &ex)
    {
        LogFatal("[%s][%s:%s:%d]call doMakeDepositRequest failure, msg:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, ex.what());
        return false;
    }
    catch(...)
    {
        LogFatal("[%s][%s:%s:%d]call doMakeDepositRequest failure", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        return false;
    }
    LogMsg("[%s]user %s deposit, orderNo:%s, url:%s, data:%s",
            m_sdkTag.c_str(), strUin.c_str(), orderNo.c_str(), strUrl.c_str(), httpData.c_str());

    // 2021-6-17 返回值中没有"Content-Type" 这几个字样,则直接返回url
    if (contentType.find("Content-Type") == std::string::npos)
    {
        platformUrl = strUrl;
        platformOrderNo = "N/A";
        cost = (int32_t)(Now(true) - t1);
        return true;
    }

    int64_t fee = req.getParamInt("amount");
    int32_t ret(0);
    // 发起HTTP POST请求
    http::VEC_STRING header_list;
    header_list.push_back(contentType);
    CMyString strResp;
    CMyString strRespHeader;
    int64_t httpBegin=Now(true);
    do {
        http::CHttpClient net;
        // 虚函数，子类可以重载解决
        ret = this->doDepositHttpRequest(net, strUrl, header_list, httpData, strResp, strRespHeader);
        cost = (int32_t)(Now(true) - httpBegin);
        if (ret != 0)
        {
            net.getError(ret, m_errNote);  
            m_errCode = err_depositHostFail;
            LogFatal("[%s][%s:%s:%d]user %s deposit fail, orderNo:%s, url:%s, err:%d, msg:%s, cost:%.2lfms",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str(), orderNo.c_str(), strUrl.c_str(), ret, m_errNote.c_str(), cost/1000.0);
            this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]user %s deposit fail, orderNo:%s, url:%s, err:%d, msg:%s, cost:%.2lfms",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str(), orderNo.c_str(), strUrl.c_str(), ret, m_errNote.c_str(), cost/1000.0));
            this->postAlert(pGame->getCode(), CMyString("平台错误=>订单 %s 申请充值时失败，金额:%.2lf, msg:%s", orderNo.c_str(), fee/100.0, m_errNote.c_str()));
            break;
        }

        // 虚函数调用，解析HTTP返回数据
        bool val(false);
        try
        {
            val = this->doParseDepositResp(pGame, strRespHeader, strResp, 
                platformUrl, platformOrderNo, m_errNote);
        }
        catch(CException &ex)
        {
            LogFatal("[%s]call doParseDepositResp failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(std::exception &ex)
        {
            LogFatal("[%s]call doParseDepositResp failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(...)
        {
            LogFatal("[%s]call doParseDepositResp failure", m_sdkTag.c_str());
            val = false;
        }
        if (!val)
        {
            /* TODO: 有可能的错误：
             * 1、充值范围越界，偏小、偏大
             * 2、通道已关闭
             */
            m_errCode = err_depositRequestDataCorrupted;
            LogErr("[%s][%s:%s:%d]user %s deposit fail, orderNo:%s, resp header [%s], resp data [%s], error:%s, cost:%.2lfms",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str(), orderNo.c_str(), strRespHeader.c_str(), strResp.c_str(), m_errNote.c_str(), cost/1000.0);
            this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]user %s deposit fail, orderNo:%s, resp header [%s], resp data [%s], error:%s, cost:%.2lfms",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str(), orderNo.c_str(), strRespHeader.c_str(), strResp.c_str(), m_errNote.c_str(), cost/1000.0));
            this->postAlert(pGame->getCode(), CMyString("平台错误=>订单%s请求充值时返回结果错误，msg:%s", orderNo.c_str(), m_errNote.c_str()));
            break;
        }
    }while(0);

    if(m_errCode != 0)
    {
        return false;
    }

    LogMsg("[%s]user %s deposit succeed, orderNo:%s, cost:%.2lfms",
            m_sdkTag.c_str(), strUin.c_str(), orderNo.c_str(), cost/1000.0);
    return true;
}

bool CPayBase::updateDepositOrder(const CMyString &gameCode, int32_t orderId, int32_t result, const CMyString &platformOrderNo)
{
    int32_t code(0);
    CMyString strMsg;
    m_pDB->Cmd("call pUpdateOrderStatus_v3(%u, %d, '%s')",
            orderId, result, platformOrderNo.c_str());
    while(m_pDB->More())
    {
        code = m_pDB->GetInt(0);
        strMsg = m_pDB->GetString(1);
    }
    char szError[128] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        m_errCode = err_depositOrderUpdateFailure;
        m_errNote.Format("update deposit order failure, errno:%d, error:%s", err, szError);
        LogFatal("[%s][%s:%s:%d]update deposit order failure, result:%d, err:%s, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, result, err, szError);
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]update deposit order failure, result:%d, err:%s, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, result, err, szError));
        this->postAlert(gameCode, CMyString("数据库错误=>更新订单 %d 状态至 %d 失败，msg:%s", orderId, result, szError));
        return false;
    }
    if (code < 0)
    {
        m_errCode = err_depositOrderUpdateFailure;
        m_errNote = strMsg;
        LogFatal("[%s][%s:%s:%d]update deposit order failure, result:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, result, strMsg.c_str());
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]update deposit order failure, result:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, result, strMsg.c_str()));
        this->postAlert(gameCode, CMyString("数据库错误=>更新订单 %d 状态至 %d失败，msg:%s", orderId, result, strMsg.c_str()));
        return false;
    }
    return true;
}

bool CPayBase::updateDepositNotify(const CMyString &gameCode, int32_t orderId, int32_t result, const CMyString &platformOrderNo, int64_t realFee, const CMyString &regionCode)
{
    int32_t code(0);
    CMyString strMsg;
    m_pDB->Cmd("call pUpdateOrderNotify_v3(%d, %d, '%s', %d, '%s')",
            orderId, result, platformOrderNo.c_str(), realFee, regionCode.c_str());
    while(m_pDB->More())
    {
        code = m_pDB->GetInt(0);
        strMsg = m_pDB->GetString(1);
    }
    char szError[128] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        m_errCode = err_depositOrderUpdateFailure;
        m_errNote.Format("update deposit order failure, errno:%d, error:%s", err, szError);
        LogFatal("[%s][%s:%s:%d]update deposit order failure, orderId:%d, result=>%d, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderId, result, err, szError);
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]update deposit order failure, orderId:%d, result=>%d, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderId, result, err, szError));
        this->postAlert(gameCode, CMyString("数据库错误=>更新充值订单 %d 至状态 %d 失败，msg:%s", orderId, result, szError));
        return false;
    }
    if (code < 0)
    {
        m_errCode = err_depositOrderUpdateFailure;
        m_errNote = strMsg;
        LogFatal("[%s][%s:%s:%d]update deposit order failure, orderId:%d, result=>%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderId, result, strMsg.c_str());
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]update deposit order failure, orderId:%d, result=>%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderId, result, strMsg.c_str()));
        this->postAlert(gameCode, CMyString("数据库错误=>更新充值订单 %d 至状态 %d 失败，msg:%s", orderId, result, strMsg.c_str()));
        return false;
    }
    return true;
}

bool CPayBase::finishDepositOrder(const CMyString &gameCode, int32_t orderId, int32_t result, int32_t bpId, const CMyString &strUin)
{
    int32_t code(0);
    CMyString strMsg;
    m_pDB->Cmd("call pFinishOrder_v3(%d, '%s', %d, %d)",
            orderId, strUin.c_str(), bpId, result);
    while(m_pDB->More())
    {
        code = m_pDB->GetInt(0);
        strMsg = m_pDB->GetString(1);
    }
    char szError[128] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        m_errCode = err_depositOrderUpdateFailure;
        m_errNote.Format("update deposit order failure, errno:%d, error:%s", err, szError);
        LogFatal("[%s][%s:%s:%d]update deposit order failure, orderId:%d, result=>%d, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderId, result, err, szError);
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]update deposit order failure, orderId:%d, result=>%d, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderId, result, err, szError));
        this->postAlert(gameCode, CMyString("数据库错误=>更新订单 %d 至状态 %d 失败，msg:%s", orderId, result, szError));
        return false;
    }
    if (code < 0)
    {
        m_errCode = err_depositOrderUpdateFailure;
        m_errNote = strMsg;
        LogFatal("[%s][%s:%s:%d]update deposit order failure, orderId:%d, result=>%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderId, result, strMsg.c_str());
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]update deposit order failure, orderId:%d, result=>%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderId, result, strMsg.c_str()));
        this->postAlert(gameCode, CMyString("数据库错误=>更新订单 %d 至状态 %d 失败，msg:%s", orderId, result, strMsg.c_str()));
        return false;
    }
    return true;
}

void CPayBase::storeDepositOrder(http::CHttpRequest &req, int32_t orderId, const CMyString &orderNo)
{
    Json::Value data;
    data["payId"] = orderId;
    data["orderNo"] = orderNo;
    data["realFee"] = (Json::Int64)0;
    data["status"] = 0;
    HTTP_JSON_STR(uin,          uin);
    HTTP_JSON_STR(gameCode,     gameCode);
    HTTP_JSON_STR(billNo,       billNo);
    HTTP_JSON_STR(productId,    productId);
    HTTP_JSON_INT32(channelId,    paycenter_channelId);
    HTTP_JSON_STR(method,       paycenter_method);
    HTTP_JSON_INT32(billpoint,  paycenter_bpId);
    HTTP_JSON_INT64(fee,        amount);

    // 存入到redis, 用于回调之用
    m_cacheDepositOrder.Push(orderNo, data);
}

void CPayBase::doPayResp(CSdkChannel *pChannel, const CMyString& strUrl, const CMyString &orderNo, char* szResp, uint32_t& cbResp)
{
    Json::Value resp;
    resp["code"] = 0;
    resp["message"] = "explain url manual";
    Json::Value data;
    data["tradeNo"] = orderNo;
    data["url"] = strUrl;
    data["sdk"] = pChannel->getSdkCode();
    data["channel"] = pChannel->getBankCode();
    CMyString methodCode = pChannel->getMethodCode();
    data["method"] = methodCode ;
    CGame *pGame = pChannel->getGame();
    int32_t gameId(0);
    if (pGame != NULL)
    {
        gameId = pGame->getId();
    } 
    int32_t methodType(0);
    DICT_PAYSTYLE.getPayStyle(gameId, methodCode, methodType);
    data["methodType"] = methodType;
    resp["data"] = data;
    this->OnPayResp(resp, szResp, cbResp);
}

void CPayBase::doPayFailure(const CMyString &gameCode, const CMyString &strUin, const CMyString &orderNo, int64_t fee, char* szResp, uint32_t& cbResp)
{
    Json::Value resp;
    resp["code"] = m_errCode;
    resp["message"] = m_errNote;
    resp["fee"] = (Json::Int64)fee;
    this->OnPayResp(resp, szResp, cbResp);

    if (orderNo.empty())
    {
        LogFatal("[%s]user %s request deposit failure, orderNo:%s, fee:%ld, err:%d, msg:%s", 
            m_sdkTag.c_str(), strUin.c_str(), orderNo.c_str(), fee, m_errCode, m_errNote.c_str());
    }
    else
    {
        LogFatal("[%s]user %s request deposit failure, orderNo:%s, fee:%ld, err:%d, msg:%s", 
            m_sdkTag.c_str(), strUin.c_str(), orderNo.c_str(), fee, m_errCode, m_errNote.c_str());
    }
    // this->postAlert(gameCode, strUin, orderNo);
}

void CPayBase::OnPayResp(Json::Value &resp, char *szResp, uint32_t& cbResp)
{
    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

void CPayBase::PingMysql(void)
{
    if (m_pDB == NULL)
    {
        return;
    }
    if (m_pDB->Ping(300) <= 0)
    {
        return;
    }
    LogDebug("[%s]maintance mysql.", m_sdkTag.c_str());
}

void CPayBase::PingRedis(void)
{
    if (m_redis.Ping(600)<=0)
    {
        return;
    }
    LogDebug("[%s]maintance redis.", m_sdkTag.c_str());
}

void CPayBase::dealReDeliver(void)
{
    // 从redis中查询出缓存，并判断是否需要重新发货
    if (!m_timerDeliverGoods.ToNextTime())
    {
        return;
    }

    int32_t once_count(10);
    int32_t cursor(0);
    int32_t next_cursor(0);
    std::vector<CMyString> vecKeys;
    do {
        vecKeys.clear();
        int32_t ret = m_cacheDeliverGoods.Scan(once_count, vecKeys, next_cursor, cursor);
        if (ret < 0)
        {
            break;
        }
        
        // 循环处理keys
        for(std::vector<CMyString>::iterator it(vecKeys.begin());
                it != vecKeys.end(); ++it)
        {
            CMyString &key = *it;
            this->dealReDeliver(key);
        }

        // 下一次循环
        if (next_cursor == 0)
        {
            break;
        }
        cursor = next_cursor;
    }while(true);
}

void CPayBase::dealExpiredOrder(void)
{
    if (!m_timerDepositOrder.ToNextTime())
    {
        return;
    }

    CMyString patten = m_cacheDepositOrder.getPatten();
    CMyString prefix = patten;
    prefix.Erase("*");

    int32_t cursor(0);
    int32_t next_cursor(0);
    int32_t ret(-1);
    std::vector<CMyString> vecKeys;
    do {
        ret = m_cacheDepositOrder.Scan(10, vecKeys, next_cursor, cursor);
        if (ret != 0)
        {
            break;
        }
        // 逐个判断是否超时，若时则更新超时列表
        for(std::vector<CMyString>::iterator it(vecKeys.begin());
                it != vecKeys.end(); ++it)
        {
            CMyString strKey = *it;
            uint32_t timeout(0);
            int32_t val = m_redis.GetExpire(strKey, timeout);
            if (val <= 0)
            {
                continue;
            }
            if (timeout == INFINITE)
            {
                continue;
            }
            if (timeout >= 6)  // 添加key时，有额外加6秒，用于记录超时
            {
                continue;
            }

            CMyString orderNo = strKey;
            orderNo.Erase(prefix);

            // 更新数据库状态
            m_pDB->Cmd("UPDATE `t_payorder` SET `orderStatus`=%d,`updatedAt`=NOW() WHERE `orderNo`='%s' and `orderStatus` in (0,6,7)", 
                    (int32_t)deposit_UserPayTimeout, orderNo.c_str());
            m_pDB->Exec();

            // 删除缓存数据
            m_redis.Del(strKey);
        }

        // 查询所有的key
        if (next_cursor==0)
        {
            // finished.
            break;
        }
        cursor = next_cursor;
    }while(true);
}

void StrReplace(CMyString &str, const char *from, const char* to)
{
    int32_t cnt(0);
    size_t len_old = strlen(from);
    size_t len_new = strlen(to);
    size_t len = str.length();                                                                                                          
    std::string::size_type pos = str.find(from);
    while(pos != std::string::npos && cnt<(int32_t)len)
    {      
        str.erase(pos, len_old);
        str.insert(pos, to); 
        pos = str.find(from, pos+len_new);
        ++cnt;
    }
}

void CPayBase::dealDepositPendingVerify(void)
{
    //CMyString gameCode;
    //CMyString orderNo;
    Json::Value req_data;   
    int64_t realFee(0);
    bool depositedOK(false);
    CMyString platformOrderNo("N/A");
    CMyString regionCode("N/A");
    // 查询所有待验证的订单
    /* req_data 必须返回以下数据
     *      gameCode
     *      uin
     *      payId       => orderId
     *      orderNo
     *      billpoint   => bpId
     *      fee
     */
    int32_t ret = this->verifyDepositRequest(req_data, depositedOK, platformOrderNo, realFee, regionCode);
    if (ret < 0)
    {
        LogErr("[%s][%s:%s:%d]deposit order verify failure, result:%d", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, ret);
        return;
    }
    else if (ret == 0)
    {
        // 没有任何订单在等待
        return;
    }
    CMyString gameCode = req_data["gameCode"].asString();
    CMyString strUin = req_data["uin"].asString();
    CMyString orderNo = req_data["orderNo"].asString();
    int32_t orderId = req_data["payId"].asInt();
    int32_t bpId = req_data["billpoint"].asInt();
    int64_t fee = req_data["fee"].asInt64();

    CGame *pGame = GAME_LIST.FindByCode(gameCode);
    if (NULL == pGame)
    {
        LogFatal("[%s][%s:%s:%d]not found gameCode %s when deposit verify response, uin:%s, orderNo:%s, fee:%ld, result:%s", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, gameCode.c_str(), strUin.c_str(), orderNo.c_str(), fee, depositedOK ? "OK" : "Failure");
        return;
    }
    CSdk* pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
    if (NULL == pSdk)
    {
        LogFatal("[%s][%s:%s:%d]invalid sdk when deposit verify response, gameCode:%s, sdkCode:%s, uin:%s, orderNo:%s, fee:%ld, result:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, gameCode.c_str(), m_sdkCode.c_str(), strUin.c_str(), orderNo.c_str(), fee, depositedOK ? "OK" : "Failure");
        this->postAlert(gameCode, CMyString("系统错误=>申请提现订单 %s (UIN:%s, 金额:%.2lf) 时未发现CSdk对象", orderNo.c_str(), strUin.c_str(), fee/100.0));
        return;
    }

    int64_t coin(0);
    int64_t ext_coin(0);
    CMyString itemInfo;
    // 获得充值对应的物品信息
    this->getDepositWealth(pGame, bpId, fee, coin, ext_coin, itemInfo);

    // 更新DB中的状态
    this->updateDepositNotify(gameCode, orderId, 
            (depositedOK ? deposit_MoneyArrival : deposit_MoneyFail), 
            platformOrderNo, realFee, regionCode);

    // 不论是否支付成功，都向游服发送通知
    this->judgeBPInfo(pGame, bpId, fee, strUin, req_data, coin, ext_coin, itemInfo);

    //上一步判断完成后再更新计费点使用次数，否则最后一次就无法购买
    if (depositedOK)
    {
        BPDATA_LIST.IncBill(strUin, bpId);
        // 通知其他进程刷新数据
        Json::Value data;
        data["uin"] = strUin;
        data["bpId"] = bpId;
        Broadcast2Sibling(sc_increaseBillpoint, data);
    }

    req_data["code"] = depositedOK ? 0 : -1;
    req_data["realFee"] = (Json::Int64)realFee;
    req_data["coin"] = (Json::Int64)coin;
    req_data["extraCoin"] = (Json::Int64)ext_coin;
    req_data["itemInfo"] = itemInfo;
    this->postDepositResultToGamesvr(pGame, pSdk, req_data);
    LogMsg("[%s]deposit verify response, uin:%s, orderNo:%s, depositedOK:%s, fee:%ld, realFee:%ld, coin:%ld, ext_coin:%ld, itemInfo [%s]",
            m_sdkTag.c_str(), strUin.c_str(), orderNo.c_str(), (depositedOK ? "true" : "false"), fee, realFee, coin, ext_coin, itemInfo.c_str());
}

void CPayBase::dealCreditPendingVerify(void)
{
    // 发送提现请求到三方平台
    Json::Value req_data;   
    bool creditOK(false);
    CMyString creditOrderNo;
    int32_t creditCost(0);
    int32_t result(0);

    int32_t ret = this->verifyCreditRequest(req_data, creditOK, creditOrderNo, m_errNote);
    if (ret == 0)
    {
        // 没有任何订单在等待
        return;
    }
    CMyString gameCode = req_data["gameCode"].asString();
    CMyString strUin = req_data["uin"].asString();
    CMyString orderNo = req_data["orderNo"].asString();
    int64_t fee = req_data["fee"].asInt64();

    CGame *pGame = GAME_LIST.FindByCode(gameCode);
    if (NULL == pGame)
    {
        LogFatal("[%s][%s:%s:%d]not found gameCode %s when credit verify response, uin:%s, orderNo:%s, fee:%ld, result:%s", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, gameCode.c_str(), strUin.c_str(), orderNo.c_str(), fee, creditOK ? "OK" : "Failure");
        return;
    }
    CSdk* pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
    if (NULL == pSdk)
    {
        LogFatal("[%s][%s:%s:%d]invalid sdk when deposit verify response, gameCode:%s, sdkCode:%s, uin:%s, orderNo:%s, fee:%ld, result:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, gameCode.c_str(), m_sdkCode.c_str(), strUin.c_str(), orderNo.c_str(), fee, creditOK ? "OK" : "Failure");
        this->postAlert(gameCode, CMyString("系统错误=>申请提现订单 %s (UIN:%s, 金额:%.2lf) 时未发现CSdk对象", orderNo.c_str(), strUin.c_str(), fee/100.0));
        return;
    }

    if (ret == 1)
    {
        if (creditOK)
        {
            result = credit_LoanBySDK;
        }
        else
        {
            result = credit_SdkLoanFail;
        }
    }
    else if (ret == -1)
    {
        result = credit_SdkLoanFail;
    }

    // 记录到数据库，等待回调
    CMyString creditRemark = req_data["creditRemark"].asString();
    int64_t creditTime = req_data["creditTime"].asInt64();
    CMyString creditOp = req_data["operator"].asString();
    this->updateCreditOrder(pGame->getCode(), creditRemark, creditTime, creditOp, orderNo, creditOrderNo, result);

    // 写redis需要放在写数据库之后，否则会造成其他进程callback回来后读取到redis数据直接继续执行，先把状态写成10后，这边本进程才写数据库，又把数据改成5
    if (result == credit_LoanBySDK)
    {
        int64_t fee = req_data["fee"].asInt64();
        // 写入redis内存表
        this->storeCreditData(pGame, pSdk, strUin, orderNo, fee, creditCost);
    }

    // 写入提现日志数据库-提现发放
    this->setWithdrawLog(orderNo, result, creditOp, creditRemark, m_errNote, creditTime);

    CMyString strParam;
    CMyString strKey;
    int32_t nRet = m_cacheCreditRespBack.Pop(orderNo, strKey, strParam);
    if (nRet > 0 && !strParam.empty())
    {
        m_qRespBackParam.push(strParam);
    }
    
    LogMsg("[%s]credit verify response, uin:%s, orderNo:%s, ret:%d creditOK:%s, fee:%ld, result:%d",
            m_sdkTag.c_str(), strUin.c_str(), orderNo.c_str(), ret, (creditOK ? "true" : "false"), fee, result);
}

void CPayBase::dealCreditOrder(void)
{
    // TODO: 代付模式下，不再由报表发起二次支持请求
}

void CPayBase::dealDepositRequest(void)
{
    // TODO: 分离模式下，发起充值请求
}

void CPayBase::dealDepositCheck(void)
{
    // TODO: 分离模式下，校验充值验证的订单
}

bool CPayBase::fetchDepositOrderData(http::CHttpRequest& req, const CMyString& orderNo, Json::Value& req_data, int32_t& orderStatus)
{
    // 从REDIS内存中获得数据
    CMyString orderData;
    CMyString strKey;
    int32_t ret = m_cacheDepositOrder.Pop(orderNo, strKey, orderData);
    if (ret == 1)
    {
        m_redis.Del(strKey);
        CJsonWrapper jr;
        if (jr.Load(orderData))
        {
            LogDebug("redis get data: key:[%s], data:[%s]", strKey.c_str(), orderData.c_str());
            req_data = jr.getJson();
            return true;
        }
        // json 解析失败，从DB中获得
    }

    bool bFind(false);
    // 从DB中查询出订单
    m_pDB->Cmd("call pQueryOrderInfo_v3('%s')", orderNo.c_str());
    while(m_pDB->More())
    {
        bFind = true;
        MYSQL_JSON_INT32(payId, 0);
        MYSQL_JSON_STR(uin, 1);
        MYSQL_JSON_STR(billNo, 2);
        MYSQL_JSON_STR(orderNo, 3);
        MYSQL_JSON_INT64(fee, 4);
        MYSQL_JSON_INT64(realFee, 5);
        MYSQL_JSON_INT32(status, 6);
        MYSQL_JSON_INT32(channelId, 7);
        MYSQL_JSON_STR(productId, 8);
        MYSQL_JSON_STR(gameCode, 9);
        MYSQL_JSON_INT32(billpoint, 12);
    }
    char szError[128] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        m_errCode = err_depositOrderQueryFail;
        m_errNote = szError;
        LogFatal("[%s][%s:%s:%d]query deposit order %s failure, err:%s, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), err, szError);
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]query deposit order %s failure, err:%s, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), err, szError));
        //TODO: postAlert => 无法获得gameCode，不知该往哪儿发错误
        return false;
    }
    if (!bFind)
    {
        m_errCode = err_depositOrderNotFound;
        m_errNote.Format("deposit order %s not found.", orderNo.c_str());
        LogFatal("[%s][%s:%s:%d]deposit order %s not found.", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str());
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]deposit order %s not found.",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str()));
        // TODO: postAlert => 无法获得gameCode，不知该往哪儿发错误
        return false;
    }

    orderStatus = req_data["status"].asInt();
    bool result(false);
    switch(orderStatus)
    {
    case deposit_CreateOrder:           // 0
        m_errNote.Format("orderNo %s has created, platform callback maybe fault.",
                orderNo.c_str());
        break;
    case deposit_UserCancel:            // 3
        m_errNote.Format("orderNo %s has cancelled by user, platform callback maybe fault.", orderNo.c_str());
        break;
    case deposit_RequestFail:           // 7
        m_errNote.Format("orderNo %s request deposit failed, platform callback maybe fault.", orderNo.c_str());
        break;
    case deposit_Refund:                // 4
        m_errNote.Format("orderNo %s has refund, platform callback maybe fault.", orderNo.c_str());
        break;
    case deposit_MoneyArrival:          // 1
    case deposit_DeliverSucceed:        // 5
    case deposit_DeliverHostFail:     // 8
    case deposit_DeliverFail:           // 9
    case deposit_DeliverManual:         // 10
        m_errNote.Format("orderNo %s has paid, can not deliver goods again.", orderNo.c_str());
        break;
    case deposit_MoneyFail:             // 2
    case deposit_RequestSucceed:        // 6
    case deposit_UserPayTimeout:        // 11
        LogDebug("[%s]orderNo %s callback again.", m_sdkTag.c_str(), orderNo.c_str());
        result = true;
        break;
    default:
        m_errNote.Format("orderNo %s has invalid status %d", orderNo.c_str(), orderStatus);
        break;
    }
    return result;
}

bool CPayBase::validDepositCallbackIp(CSdk *pSdk, const CMyString& strIp)
{
    if (!pSdk->IsRechargeIpValid(strIp))
    {
        m_errNote.Format("deposit callback ip %s invalid.", strIp.c_str());
        LogWarn("[%s]deposit callback ip %s invalid", m_sdkTag.c_str(), strIp.c_str());
        this->postAlarm(Alarm_General, CMyString("[%s]deposit callback ip %s invalid", m_sdkTag.c_str(), strIp.c_str()));
        return false;
    }

    return true;
}

void CPayBase::getDepositWealth(CGame *pGame, int32_t bpId, int64_t fee, int64_t& coin, int64_t& ext_coin, CMyString& itemInfo)
{
    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    if (pBP!=NULL)
    {
        coin = pBP->getCoin();
        ext_coin = pBP->getExtCoin();
        itemInfo = pBP->getBpItemData();
        return;
    }

    // 未找到计费点，按照兑换比例获得金币
    int32_t exchangeRate = pGame->getExchangeRate();
    coin = fee * exchangeRate / 100;
    ext_coin = 0;
    LogMsg("[%s]deposit callback not found billpoint %d, exchange_rate:%d, fee:%ld, coin:%ld", m_sdkTag.c_str(), bpId, exchangeRate, fee, coin);
}

bool CPayBase::postDepositResultToGamesvr(CGame *pGame, CSdk* pSdk, Json::Value &req_data)
{
    CMyString strUin = req_data["uin"].asString();
    int32_t orderId = req_data["payId"].asInt();
    int32_t bpId = req_data["billpoint"].asInt();
    CMyString orderNo = req_data["orderNo"].asString();

    // 与 gamesvr 的签名
    http::CHttpRespMaker resp_maker;
    resp_maker.AddJson(req_data);
    CMyString pending_sign_str = resp_maker.http_str();
    pending_sign_str.Append("&key=%s", pGame->getApiKey().c_str());
    CMyString strSign = CMD5Wrapper::encode(pending_sign_str.c_str());
    strSign.MakeLower();

    //填充返回结果数据
    resp_maker.add("sign", strSign);
    CMyString strRespDataWithSign = resp_maker.json_str();
    CMyString strUrl = pGame->getApiNotifyUrl();
    LogMsg("[%s]post deposit result [%s] to gamesvr %s",
            m_sdkTag.c_str(), strRespDataWithSign.c_str(), strUrl.c_str());

    // 发送Http请求
    enum DepositStatus depositResult;
    bool bNeedReDeliver(false);
    bool bSucceed = this->deliverGoods(pGame->getCode(), strUrl, strRespDataWithSign, depositResult, bNeedReDeliver);
    if (bNeedReDeliver)
    {
        int32_t pendingSec = m_reDeliverTimes.at(0);
        Json::Value data;
        data["orderNo"] = orderNo;
        data["url"] = strUrl;
        data["data"] = strRespDataWithSign;
        data["idx"] = 1;
        data["expired"] = (Json::Int64)(Now() + pendingSec*1000);
        data["uin"] = strUin;
        data["billpoint"] = bpId;
        data["payId"] = orderId;
        data["gameCode"] = pGame->getCode();

        // 保存到cache中，稍后发送
        m_cacheDeliverGoods.Push(data);
    }

    // 记录发货状态 
    this->finishDepositOrder(pGame->getCode(), orderId, depositResult, bpId, strUin);

    return bSucceed;
}

void CPayBase::dealReDeliver(const CMyString &key)
{
    CMyString data;
    int32_t ret = m_redis.Get(key, data);
    if (ret <= 0)
    {
        m_redis.Del(key);
        return;
    }

    Json::Value root;
    CJsonWrapper jr;
    if (!jr.Load(data))
    {
        m_redis.Del(key);
        return;
    }
    root = jr.getJson();
    if (!root.isObject())
    {
        return;
    }
    int64_t expired = root["expired"].asInt64();
    if (expired > (int64_t)Now())
    {
        // 时间未到
        return;
    }

    CMyString strUrl = root["url"].asString();
    CMyString strData = root["data"].asString();
    CMyString orderNo = root["orderNo"].asString();
    CMyString gameCode = root["gameCode"].asString();
    enum DepositStatus depositResult;
    bool bNeedReDeliver(false);
    bool bSucceed = this->deliverGoods(gameCode, strUrl, strData, depositResult, bNeedReDeliver);
    if (!bSucceed)
    {
        // 发货失败，函数内有提示
    }
    if (bNeedReDeliver)
    {
        // 不成功，下一次继续处理
        int32_t deliverIdx = root["idx"].asInt();
        if (deliverIdx >= (int32_t)m_reDeliverTimes.size() - 1)
        {
            // 超过次数了，出错
            CMyString newKey = "fail#"+key;
            m_redis.Set(newKey, data);
            m_redis.Del(key);
            LogFatal("[%s][%s:%s:%d]deliver goods more than %d times, orderNo:%s url:%s, data:%s",
                    m_sdkTag.c_str(), __FILE__, __func__, __LINE__, deliverIdx+1, orderNo.c_str(), strUrl.c_str(), strData.c_str());
            this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]deliver goods more than %d times, orderNo:%s url:%s, data:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, deliverIdx+1, orderNo.c_str(), strUrl.c_str(), strData.c_str()));
            this->postAlert(gameCode, CMyString("游服错误=>订单%s发货失败,请人工介入", orderNo.c_str()));
            return;
        }

        root["idx"] = deliverIdx+1;
        int32_t expired_msec = m_reDeliverTimes.at(deliverIdx) * 1000;
        root["expired"] = (Json::Int64)(Now() + expired_msec);

        CJsonWrapper wr(root);
        data = wr.toString();
        m_redis.Set(key, data);
        LogWarn("[%s]deliver goods failure, times:%d, orderNo:%s, url:%s, data:%s, pending next time",
                m_sdkTag.c_str(), deliverIdx, orderNo.c_str(), strUrl.c_str(), data.c_str());
        return;
    }

    // 成功，结束订单
    int32_t orderId = root["payId"].asInt();
    int32_t bpId = root["billpoint"].asInt();
    CMyString strUin = root["uin"].asString();
    m_redis.Del(key);
    this->finishDepositOrder(gameCode, orderId, deposit_DeliverSucceed, bpId, strUin);
}

bool CPayBase::deliverGoods(const CMyString &gameCode, const CMyString &strUrl, const CMyString &strData, enum DepositStatus &result, bool &bNeedReDeliver)
{
    bNeedReDeliver = false;

    // 发送Http请求
    CMyString strResp;
    CMyString strRespHeader;
    std::vector<std::string> header_list;
    header_list.push_back("Content-Type:application/json");
    http::CHttpClient net;
    do {
        int32_t ret = net.HttpPostWithHeaders(strUrl.c_str(), 
            strResp, strRespHeader, header_list, strData.c_str() );
        if (ret != 0)
        {
            // 网络失败,压缩到后台继续处理    
            net.getError(ret, m_errNote);
            result = deposit_DeliverHostFail;
            m_errCode = err_depositDeliverHostFail; 
            LogFatal("[%s][%s:%s:%d]deliver goods to server [%s] failure, data[%s], err:%d, msg:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), strData.c_str(), ret, m_errNote.c_str());
            this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]deliver goods to server [%s] failure, data[%s], err:%d, msg:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), strData.c_str(), ret, m_errNote.c_str()));
            this->postAlert(gameCode, CMyString("游服错误=>申请充值订单发放失败, msg:%s", m_errNote.c_str()));
            bNeedReDeliver = true;
            break;
        }

        LogMsg("[%s]deliver goods resp data[%s], request data[%s]", 
                m_sdkTag.c_str(), strResp.c_str(), strData.c_str());
        CJsonWrapper jr;
        if (!jr.Load(strResp))
        {
            // 返回结果不是json串
            if (strcasecmp(strResp.c_str(), "success") != 0)
            {
                // 需要重发
                result = deposit_DeliverFail;
                m_errCode = err_depositDeliverGoodsFail;
                bNeedReDeliver = true;

                LogFatal("[%s][%s:%s:%d]deliver goods to server [%s] failure, data[%s], resp:%s",
                    m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), strData.c_str(), strResp.c_str());
                this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]deliver goods to server [%s] failure, data[%s],  resp:%s",
                    m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), strData.c_str(), strResp.c_str()));
            }
            else
            {
                result = deposit_DeliverSucceed;
            }
            break;
        }
        int32_t code = jr.getInt("code");
        switch(code)
        {
        case deliver_Success:
            result = deposit_DeliverSucceed;
            break;
        case deliver_OrderFinished:
            // TODO: 更新订单状态为完成，不重发
            result = deposit_DeliverSucceed;
            break;
        case deliver_OrderNotExist:
        case deliver_FeeMotMatch:
            // TODO: 更新订单状态为失败，不重发
            result = deposit_DeliverFail;
            break;
        case deliver_Failure:
        case deliver_SystemFault:
            // 需要重发
            result = deposit_DeliverFail;
            bNeedReDeliver = true;
            break;
        }
    }while(0);

    return (result == deposit_DeliverSucceed);
}

void CPayBase::judgeBPInfo(CGame *pGame, const int32_t& bpId, const int64_t& fee, const CMyString& strUin, Json::Value& req_data, int64_t& coin, int64_t& ext_coin, CMyString& itemInfo)
{
    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    if (NULL == pBP)
    {
        LogErr("[%s]DELIVER-GOODS>not found billpoint %d", m_sdkTag.c_str(), bpId);
        return;
    }

    if (pBP->CanBuy(strUin))
    {
        req_data["category"] = pBP->getCategory();
        CMyString str1 = pBP->getExtraInfo();
        StrReplace(str1, "\"", "\\\"");
        req_data["extraInfo"] = str1;
        LogMsg("[%s]DELIVER-GOODS>found billpoint %d, billpoint can buy", m_sdkTag.c_str(), bpId);
        return;
    }

    bool bFindBP = false;
    std::vector<CBillPoint *> vecBillPoint;
    int32_t nchannelId = req_data["channelId"].asInt();
    CSdkChannel *pChannel = SDKCHANNEL_LIST.Find(nchannelId);

    do
    {
        if (!pChannel)
        {
            break;
        }

        int32_t nSize = BILLPOINT_LIST.Find(pChannel->getConfigId(), fee, vecBillPoint);
        if (nSize <= 0)
        {
            break;
        }

        std::vector<CBillPoint *>::iterator it(vecBillPoint.begin());
        for (; it != vecBillPoint.end(); ++it)
        {
            CBillPoint &bp = *(*it);
            if (bp.CanBuy(strUin))
            {
                bFindBP = true;
                coin = bp.getCoin();
                ext_coin = bp.getExtCoin();
                itemInfo = bp.getBpItemData();

                req_data["category"] = bp.getCategory();
                CMyString str1 = bp.getExtraInfo();
                StrReplace(str1, "\"", "\\\"");
                req_data["extraInfo"] = str1;

                LogMsg("[%s]deposit callback billpoint %d can't buy, fee:%ld, find same fee billpoint: %d coin:%ld", m_sdkTag.c_str(), bpId, fee, bp.getId(), coin);
                break;
            }
        }

    } while (false);

    if (!bFindBP)
    {
        // 未找到计费点，按照兑换比例获得金币
        int32_t exchangeRate = pGame->getExchangeRate();
        coin = fee * exchangeRate / 100;
        ext_coin = 0;
        itemInfo.clear();
        LogMsg("[%s]deposit callback billpoint %d can't buy, not find same fee billpoint, exchange_rate:%d, fee:%ld, coin:%ld", m_sdkTag.c_str(), bpId, exchangeRate, fee, coin);
    }
    
}

void CPayBase::postAlert(const CMyString &gameCode, const CMyString &note)
{
    CGame* pGame = GAME_LIST.FindByCode(gameCode);
    if (NULL == pGame)
    {
        LogErr("[%s]not found gameCode %s when postAlert", m_sdkTag.c_str(), gameCode.c_str());
        return;
    }
    
    // 获得警告的ID
    CJsonWrapper alert;
    pGame->getAlter(alert);
    int32_t alertId = alert.getInt("alertId");
    if (alertId <= 0)
    {
        return;
    }
    
    // 获得警告的URL
    CMyString alertUrl = alert.getString("url");
    char szTime[128] = "";
    TimeToStr(time(NULL), szTime, sizeof(szTime));
    CMyString alertMsg("[%s]%s %s", szTime, m_sdkTag.c_str(), note.c_str());
    Json::Value msg;
    msg["chatId"] = alertId;
    msg["text"] = alertMsg;
    CJsonWrapper wr(msg);
    CMyString strData = wr.toString();

    // 发起警告
    http::CHttpClient net;
    http::VEC_STRING headers;
    headers.push_back("Content-Type: application/json");
    CMyString strHeader;
    CMyString strResp;
    int32_t ret = net.HttpPostWithHeaders(alertUrl.c_str(), strResp, strHeader, headers, strData.c_str());
    if (ret==0)
    {
        // success
        return;
    }

    CMyString strError;
    net.getError(ret, strError);
    LogErr("[%s]call postAlert msg [%s] failure, errno:%d, error:%s", 
            m_sdkTag.c_str(), note.c_str(), ret, strError.c_str());
}

bool CPayBase::validCreditParam(http::CHttpRequest &req, Json::Value &resp, CGame *&pGame)
{
    HTTP_REQ_STR(strUin, uin);
    std::vector<CMyString> vecParam;
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    vecParam.push_back("token");
    vecParam.push_back("orderNo");
    //vecParam.push_back("fee");
    //vecParam.push_back("bankName");
    //vecParam.push_back("bankCard");
    //vecParam.push_back("bankCode");
    vecParam.push_back("sign");
    if (!req.ValidParam(vecParam, m_errNote))
    {
        m_errCode = err_creditMissParameter;
        resp["code"] = m_errCode;
        resp["message"] = m_errNote;
        LogErr("[%s][%s:%s:%d]missint param %s when user %s request credit.", 
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_errNote.c_str(), strUin.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]missint param %s when user %s request credit.",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_errNote.c_str(), strUin.c_str()));
        return false;
    }

    HTTP_REQ_STR(gameCode, gameCode);
    pGame = GAME_LIST.FindByCode(gameCode);
    if (NULL == pGame)
    {
        m_errCode = err_creditParameterFault;
        resp["code"] = m_errCode;
        resp["message"] = CMyString("not valid gameCode %s", gameCode.c_str());
        LogWarn("[%s]invalid gameCode %s when user %s request credit.", 
            m_sdkTag.c_str(), gameCode.c_str(), strUin.c_str());
        return false;
    }   

    HTTP_REQ_STR(strSign1, sign);
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    http::CHttpRespMaker reqVerify;
    req.BuildRespMaker_Except(vecExcept, reqVerify);
    CMyString str1 = reqVerify.http_str();
    CMyString str2("%s&merchantKey=%s", str1.c_str(), pGame->getApiKey().c_str());
    CMyString strSign2 = CMD5Wrapper::encode(str2.c_str());
    if (strcasecmp(strSign1.c_str(), strSign2.c_str())!=0)
    {
        m_errCode = err_creditSignMismatch;
        resp["code"] = m_errCode;
        resp["message"] = "credit sign mismatch";
        LogErr("[%s][%s:%s:%d]sign mismatch when user %s request credit, origin text[%s], sign[%s], sign from request[%s]",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str(), str2.c_str(), strSign2.c_str(), strSign1.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]sign mismatch when user %s request credit, origin text[%s], sign[%s], sign from request[%s]",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str(), str2.c_str(), strSign2.c_str(), strSign1.c_str()));
        return false;
    }
    return true;
}

bool CPayBase::validCreditIp(const CMyString &strIp)
{
    /* 从config.xml中查询出配置信息
    <main>
        <paycenter>
            <credit>
                <ip enable="true" valid="0.0.0.0" />
            </credit>
        </paycenter>
    </main>
    */

    CXmlFile &xml = CONFIG.getXml();
    CMyString sect="main/paycenter/credit/ip";
    bool bEnable(false);
    int32_t ret = xml.getBoolean(sect.c_str(), "enable", bEnable);
    if (ret != 0 || !bEnable)
    {
        // 未配置，默认是不校验IP
        // bEnable = false, 不校验IP
        return true;
    }

    CMyString strIpList;
    ret = xml.getString(sect.c_str(), "valid", strIpList);
    if (ret != 0 || strIpList.empty())
    {
        // 未配置，默认是不校验IP
        // ip 列表为空，不校验IP
        return true;
    }

    CStrSpliter css;
    css.Split(strIpList, ',');
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

bool CPayBase::validCreditToken(const CMyString &orderNo, const CMyString &creditToken, Json::Value &req_data, Json::Value &resp)
{
    // 从redis中获得TOKEN
    CMyString strKey;
    CMyString creditData;
    int32_t ret = m_cacheCreditReq.Pop(orderNo, strKey, creditData);
    if (ret <= 0)
    {
        // 订单号失效，或者订单号过期
        resp["code"] = err_creditTokenExpired;
        resp["message"] = "invalid orderNo or time expired.";
        LogErr("[%s][%s:%s:%d]credit token expired, orderNo:%s, key:%s.", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), strKey.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]credit token expired, orderNo:%s, key:%s.",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), strKey.c_str()));
        return false;
    }

    //CMyString strData = CBase64Wrapper::Decode(creditData.c_str(), creditData.length(), false);
    CMyString strData = creditData;
    CJsonWrapper jr;
    if (!jr.Load(strData))
    {
        resp["code"] = err_creditRequestCorrupted;
        resp["message"] = "credit request data corrupted";
        LogErr("[%s][%s:%s:%d]credit data corrupted, orderNo:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]credit data corrupted, orderNo:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str()));
        return false;
    }
    req_data = jr.getJson();

    CMyString creditTokenSaved = req_data["token"].asString();
    // 检查token是否匹配
    if (strcasecmp(creditTokenSaved.c_str(), creditToken.c_str()) != 0)
    {
        resp["code"] = err_creditTokenMismatch;
        resp["message"] = CMyString("orderNo %s token mismatch.", orderNo.c_str());
        LogErr("[%s][%s:%s:%d]credit token mismatch, orderNo:%s, save in redis is [%s], web transfer is [%s]",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), creditTokenSaved.c_str(), creditToken.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]credit token mismatch, orderNo:%s, save in redis is [%s], web transfer is [%s]",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), creditTokenSaved.c_str(), creditToken.c_str()));
        return false;
    }
    return true;
}

bool CPayBase::postCreditRequest(Json::Value &req, Json::Value &resp, CGame *pGame, CSdk *&pSdk, CMyString &creditOrderNo, int32_t &cost)
{
    // 获得相关参数
    CMyString gameCode = req["gameCode"].asString();
    CMyString orderNo = req["orderNo"].asString();
    CMyString strUin = req["uin"].asString();

    pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
    if (NULL == pSdk)
    {
        resp["code"] = err_creditSystemFault;
        resp["message"] = CMyString("not found sdk %s for game %s", m_sdkCode.c_str(), gameCode.c_str());
        LogFatal("[%s][%s:%s:%d]invalid sdk when credit, gameCode:%s, sdkCode:%s, user:%s, orderNo:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, gameCode.c_str(), m_sdkCode.c_str(), strUin.c_str(), orderNo.c_str());
        this->postAlert(gameCode, CMyString("系统错误=>申请提现订单 %s 时未发现CSdk对象", orderNo.c_str()));
        return false;
    }

    //判断是否开启提现
    if (!pSdk->getEnableWithdraw())
    {
        resp["code"] = err_creditEnabled;
        resp["message"] = CMyString("%s for game %s not enable the withdrawal function", m_sdkCode.c_str(), gameCode.c_str());
        LogErr("[%s][%s:%s:%d]not enable the withdrawal function when credit, gameCode:%s, sdkCode:%s, user:%s, orderNo:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, gameCode.c_str(), m_sdkCode.c_str(), strUin.c_str(), orderNo.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]not enable the withdrawal function when credit, gameCode:%s, sdkCode:%s, user:%s, orderNo:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, gameCode.c_str(), m_sdkCode.c_str(), strUin.c_str(), orderNo.c_str()));
        return false;
    }

    // 虚函数回调，用于获得代付请求相关参数
    CMyString creditUrl;
    CMyString contentType;
    CMyString httpData;
    try
    {
        this->doMakeCreditRequest(req, pGame, pSdk, creditUrl, contentType, httpData);
    }
    catch(CException &ex)
    {
        LogFatal("[%s][%s:%s:%d]call doMakeCreditRequest failure, msg:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, ex.what());
        return false;
    }
    catch(std::exception &ex)
    {
        LogFatal("[%s][%s:%s:%d]call doMakeCreditRequest failure, msg:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, ex.what());
        return false;
    }
    catch(...)
    {
        LogFatal("[%s][%s:%s:%d]call doMakeCreditRequest failure", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        return false;
    }
    LogMsg("[%s]user %s credit, orderNo:%s, url[%s], data[%s]",
            m_sdkTag.c_str(), strUin.c_str(), orderNo.c_str(), creditUrl.c_str(), httpData.c_str());

    // 向三方平台发起代付请求
    http::VEC_STRING header_list;
    header_list.push_back(contentType);
    CMyString strRespHeader;
    CMyString strResp;
    m_errCode = 0;
    int64_t httpBegin = Now(true);
    do {
        http::CHttpClient net;
        // 虚函数，子类可以重载解决
        int32_t ret = this->doCreditHttpRequest(net, creditUrl, header_list, httpData, strResp, strRespHeader);
        cost = Now(true) - httpBegin;
        if (ret != 0)
        {
            net.getError(ret, m_errNote);  
            m_errCode = err_creditHostFail;
            LogErr("[%s][%s:%s:%d]credit server failure, url [%s], data [%s], err:%d, msg:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, creditUrl.c_str(), httpData.c_str(), ret, m_errNote.c_str());
            this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]credit server failure, url [%s], data [%s], err:%d, msg:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, creditUrl.c_str(), httpData.c_str(), ret, m_errNote.c_str()));
            this->postAlert(gameCode, CMyString("平台错误=>申请提现时出错，订单号:%s, msg:%s", orderNo.c_str(), m_errNote.c_str()) );
            break;
        }

        // 虚函数调用，解析提现回调的结果
        bool val(false);
        try
        {
            val = this->doParseCreditResp(pGame, strRespHeader, strResp, creditOrderNo, m_errNote);
        }
        catch(CException &ex)
        {
            LogFatal("[%s]call doParseCreditResp failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(std::exception &ex)
        {
            LogFatal("[%s]call doParseCreditResp failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(...)
        {
            LogFatal("[%s]call doParseCreditResp failure", m_sdkTag.c_str());
            val = false;
        }
        if (!val)
        {
            m_errCode = err_creditRequestFail;
            LogWarn("[%s]credit server respone error, url [%s], data [%s], resp data[%s], resp header [%s], error:%s",
                m_sdkTag.c_str(), creditUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str(), m_errNote.c_str());
            this->postAlarm(Alarm_General, CMyString("[%s]credit server respone error, url [%s], data [%s], resp data[%s], resp header [%s], error:%s",
                m_sdkTag.c_str(), creditUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str(), m_errNote.c_str()));
            //this->postAlert(gameCode, CMyString("平台错误=>提现结时返回时出错，订单号:%s, msg:%s", orderNo.c_str(), m_errNote.c_str()) );
            break;
        }
    }while(0);

    if (m_errCode != 0)
    {
        resp["code"] = m_errCode;
        resp["message"] = m_errNote;
        return false;
    }
    return true;
}

int32_t CPayBase::doCreditHttpRequest(http::CHttpClient &net, const CMyString &strUrl, http::VEC_STRING &header_list, const CMyString &httpData, CMyString &strResp, CMyString &strRespHeader)
{
    //TODO: 此函数居然会发生 signal 11 的异常
    //http request error, reason: curl_easy_perform error :Timeout was reached
    //需要检查一下此段代码
        
    return net.HttpPostWithHeaders(strUrl.c_str(),
                strResp, strRespHeader, header_list,
                httpData.c_str());
}

void CPayBase::storeCreditData(CGame *pGame, CSdk *pSdk, const CMyString &strUin, const CMyString &orderNo, int64_t fee, int32_t creditCost)
{
    Json::Value credit_data;
    credit_data["url"] = pSdk->getWithdrawCheckUrl();
    credit_data["pid"] = pSdk->getAccount();
    credit_data["uin"] = strUin;
    credit_data["orderNo"] = orderNo;
    credit_data["key"] = pSdk->getApiKey();
    credit_data["gameCode"] = pGame->getCode();
    credit_data["fee"] = (Json::Int64)fee;
    credit_data["3rdcost"] = creditCost; // 第三方调用的耗时，ms
    m_cacheCreditCB.Push(orderNo, credit_data);
}

bool CPayBase::updateCreditOrder(const CMyString &gameCode, const CMyString &creditRemark, int64_t creditTime, 
        const CMyString &creditOperator, const CMyString &orderNo, const CMyString &creditOrderNo, int32_t result)
{
    int32_t code(0);
    m_pDB->Cmd("call pWithdrawCredit('%s', %d, '%s', %ld, '%s', '%s')",
            orderNo.c_str(), result, creditRemark.c_str(), creditTime, creditOperator.c_str(), 
            m_sdkCode.c_str());
    while(m_pDB->More())
    {
        code = m_pDB->GetInt(0);
        m_errNote = m_pDB->GetString(1);
    }
    char szError[128] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        m_errCode = err_creditOrderUpdateFailure;
        m_errNote.Format("update credit order failure, errno:%d, error:%s", err, szError);
        LogFatal("[%s][%s:%s:%d]update credit order failure, orderNo:%s, result:%d, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), result, err, szError);
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]update credit order failure, orderNo:%s, result:%d, err:%d, msg:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), result, err, szError));
        this->postAlert(gameCode, CMyString("数据库错误=>更新提现订单 %s 至状态 %d 失败, msg:%s", orderNo.c_str(), result, szError));
        return false;
    }
    if (code != 0)
    {
        m_errCode = err_creditOrderUpdateFailure;
        // m_errNote.Format("update credit order failure, errno:%d, error:%s", err, szError);
        LogErr("[%s][%s:%s:%d]update credit order failure, orderNo:%s, result:%d, code:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), result, code, m_errNote.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]update credit order failure, orderNo:%s, result:%d, code:%d, msg:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), result, code, m_errNote.c_str()));
        this->postAlert(gameCode, CMyString("数据库错误=>更新提现订单 %s 至状态 %d 失败, msg:%s", orderNo.c_str(), result, m_errNote.c_str()));
        return false;
    }
    return true;
}

bool CPayBase::updateCreditTax(const CMyString &gameCode, const CMyString &orderNo, int64_t &fee, int64_t &realFee, int64_t &tax)
{
    int32_t code(0);
    m_pDB->Cmd("UPDATE t_withdraw SET `duty_money` = %ld WHERE order_id = '%s';", tax, orderNo.c_str());
    while(m_pDB->More())
    {
        code = m_pDB->GetInt(0);
        m_errNote = m_pDB->GetString(1);
    }
    char szError[128] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        m_errCode = err_creditOrderUpdateFailure;
        m_errNote.Format("update credit order failure, errno:%d, error:%s", err, szError);
        LogWarn("[%s][%s:%s:%d]update credit order tax failure, orderNo:%s, tax:%d, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), tax, err, szError);
        return false;
    }
    if (code != 0)
    {
        m_errCode = err_creditOrderUpdateFailure;
        // m_errNote.Format("update credit order failure, errno:%d, error:%s", err, szError);
        LogWarn("[%s][%s:%s:%d]update credit order tax failure, orderNo:%s, tax:%d, code:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), tax, code, m_errNote.c_str());
        return false;
    }
    return true;
}

bool CPayBase::finishCreditOrder(const CMyString &gameCode, const CMyString &orderNo, const CMyString &creditOrderNo, int32_t result)
{
    int32_t code(0);
    m_pDB->Cmd("call pWithdrawSDKCallback('%s', %d, %ld, '%s')",
            orderNo.c_str(), result, ::time(NULL), m_sdkCode.c_str());
    while(m_pDB->More())
    {
        code = m_pDB->GetInt(0);
        m_errNote = m_pDB->GetString(1);
    }
    char szError[128] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        m_errCode = err_creditOrderUpdateFailure;
        m_errNote.Format("update credit order failure, errno:%d, error:%s", err, szError);
        LogFatal("[%s][%s:%s:%d]update credit order failure, orderNo:%s, result:%d, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), result, err, szError);
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]update credit order failure, orderNo:%s, result:%d, err:%d, msg:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), result, err, szError));
        this->postAlert(gameCode, CMyString("数据库错误=>更新提现订单 %s 至状态 %d 失败, msg:%s", orderNo.c_str(), result, szError));
        return false;
    }
    if (code != 0)
    {
        m_errCode = err_creditOrderUpdateFailure;
        // m_errNote.Format("update credit order failure, errno:%d, error:%s", err, szError);
        LogErr("[%s][%s:%s:%d]update credit order failure, orderNo:%s, result:%d, code:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), result, code, m_errNote.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]update credit order failure, orderNo:%s, result:%d, code:%d, msg:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str(), result, code, m_errNote.c_str()));
        this->postAlert(gameCode, CMyString("数据库错误=>更新提现订单 %s 至状态 %d 失败, msg:%s", orderNo.c_str(), result, m_errNote.c_str()));
        return false;
    }
    return true;
}

bool CPayBase::fetchCreditOrderData(const CMyString &orderNo, Json::Value &req_data)
{
    CMyString strData;
    CMyString strKey;
    int32_t ret = m_cacheCreditCB.Pop(orderNo, strKey, strData);
    if (ret <= 0)
    {
        // 从数据库中查询相关数据
        enum CreditStatus creditStatus(credit_SdkLoanFail);
        m_pDB->Cmd("CALL pQueryWithdrawOrderData('%s');", orderNo.c_str());
        while(m_pDB->More())
        {
            req_data["url"] = m_pDB->GetString(0);
            req_data["pid"] = m_pDB->GetString(1);
            req_data["uin"] = m_pDB->GetString(2);
            req_data["orderNo"] = orderNo;
            req_data["key"] = m_pDB->GetString(3);
            req_data["gameCode"] = m_pDB->GetString(4);
            req_data["fee"] = (Json::Int64)m_pDB->GetInt64(5);
            req_data["3rdcost"] = 0;
            creditStatus = (enum CreditStatus)m_pDB->GetInt(6);
        }
        char szError[128] = "";
        int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
        if (err != 0)
        {
            m_errCode = err_creditOrderExpired;
            LogWarn("[%s]credit order %s not found when callback, err:%d, msg:%s",
                m_sdkTag.c_str(), orderNo.c_str(), err, szError);
            this->postAlarm(Alarm_Err, CMyString("[%s]credit order %s not found when callback, err:%d, msg:%s",
                m_sdkTag.c_str(), orderNo.c_str(), err, szError));
            return false;
        }
        LogWarn("[%s]credit order %s time expired, query from db, uin:%s, status:%d",
                m_sdkTag.c_str(), orderNo.c_str(), 
                req_data["uin"].asString().c_str(), (int32_t)creditStatus);

        return true;
    }

    CJsonWrapper jr;
    if (!jr.Load(strData))
    {
        m_errCode = err_creditOrderCorrupted;
        LogErr("[%s][%s:%s:%d]json string [%s] invalid when callback credit, order %s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strData.c_str(), orderNo.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]json string [%s] invalid when callback credit, order %s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strData.c_str(), orderNo.c_str()));
        return false;
    }
    req_data = jr.getJson();

    return true;
}

bool CPayBase::validCreditCallbackIp(const CMyString &strIp, CSdk *pSdk)
{
    if (!pSdk->IsWithdrawIpValid(strIp))
    {
        LogFatal("[%s][%s:%s:%d]credit callback ip %s invalid", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strIp.c_str());
        return false;
    }
    return true;
}

// 支付中心的订单状态转换游服认可的状态
static int32_t transferGamesvrResult(int32_t status)
{
    // status => t_withdraw.status 字段的状态序号
    // result => 返回给游戏服务器的状态: 
    //  0 表示状态未结束，需要后处理，不该返回给游服
    // -1 表示状态错误
    // 1/2/8/9 状态正确
    // status=>                  1  2  3  4  5  6  7  8   9  10  11  12  13
    static int32_t s_result[] = {0, 9, 8, 0, 0, 1, 9, 8, -1,  2,  9,  9,  9};

    if (status<=0 || status>=(int32_t)(sizeof(s_result)/sizeof(int32_t)) )
    {
        return -1;
    }
    return s_result[status-1];
}

bool CPayBase::postCreditResultToGamesvr(CGame *pGame, CSdk *pSdk, const CMyString &orderNo, int64_t fee, int64_t realFee, enum CreditStatus creditResult)
{
    CMyString gameCode = pGame->getCode();
    // 查询出游服的订单号
    CMyString gameOrderNo;
    CMyString strUin;
    m_pDB->Cmd("SELECT `third_order`,`uuid` FROM `t_withdraw` WHERE `order_id`='%s';", orderNo.c_str());
    while(m_pDB->More())
    {
        gameOrderNo = m_pDB->GetString(0);
        strUin = m_pDB->GetString(1);
    }

    // 构建向游服发送的数据
    http::CHttpRespMaker resp_maker;
    resp_maker.add("gameCode", gameCode);
    resp_maker.add("uin", strUin);
    resp_maker.add("orderNo", orderNo);
    resp_maker.add("gameOrderNo", gameOrderNo);
    resp_maker.add("fee", fee);
    resp_maker.add("result", transferGamesvrResult((int32_t)creditResult));
    resp_maker.add("step", 4);
    resp_maker.add("digiccy", realFee); //数字钱包新增字段，通知游服实际到账多少数字货币。该字段仅对数字钱包有效，其他支付可忽略
    CMyString str1 = resp_maker.http_str();
    CMyString str2("%s&key=%s", str1.c_str(), pGame->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeLower();
    resp_maker.add("sign", strMd5);
    CMyString strSign = resp_maker.json_str();

    CMyString strUrl = pGame->getApiTranferUrl();
    bool bSucceed(false);
    // 发送Http请求
    CMyString strResp;
    CMyString strRespHeader;
    CMyString strContentType = "Content-Type: application/json";
    http::VEC_STRING header_list;
    header_list.push_back(strContentType.c_str());
    do {
        http::CHttpClient net;
        int32_t ret = net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list, strSign.c_str() );
        LogMsg("[%s:%s:%d] strUrl :%s, strSign: %s, strResp:%s, strRespHeader:%s", __FILE__, __func__, __LINE__, strUrl.c_str(), strSign.c_str(), strResp.c_str(), strRespHeader.c_str());
        if (ret != 0)
        {
            net.getError(ret, m_errNote);
            LogFatal("[%s][%s:%s:%d]notify credit result [%s] to server [%s] failure, err:%d, msg:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strSign.c_str(), strUrl.c_str(), ret, m_errNote.c_str());
            this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]notify credit result [%s] to server [%s] failure, err:%d, msg:%s",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strSign.c_str(), strUrl.c_str(), ret, m_errNote.c_str()));
            this->postAlert(gameCode, CMyString("游服错误=>提现结果通知游服失败，订单号:%s, msg:%s", orderNo.c_str(), m_errNote.c_str()));
            break;
        }
        // 处理返回结果
        LogMsg("[%s]uin %s credit order [%s] has been finished by gamesvr, resp [%s]",
            m_sdkTag.c_str(), strUin.c_str(), orderNo.c_str(), strResp.c_str());
        bSucceed = true;
    }while(0);

    return bSucceed;
}

bool CPayBase::validQueryParam(http::CHttpRequest &req, Json::Value &resp, CGame *&pGame)
{
    std::vector<CMyString> vecParam;
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    vecParam.push_back("orderNo");
    if (!req.ValidParam(vecParam, m_errNote))
    {
        m_errCode = err_queryorderMissParameter;
        resp["code"] = m_errCode;
        resp["message"] = m_errNote;
        return false;
    }

    HTTP_REQ_STR(strUin, uin);
    HTTP_REQ_STR(gameCode, gameCode);
    HTTP_REQ_STR(orderNo, orderNo);
    pGame = GAME_LIST.FindByCode(gameCode);
    if (NULL == pGame)
    {
        m_errCode = err_queryorderParameterFault;
        resp["code"] = m_errCode;
        resp["message"] = CMyString("not valid gameCode %s", gameCode.c_str());
        LogWarn("[%s]invalid gameCode %s when user %s query order %s.", 
            m_sdkTag.c_str(), gameCode.c_str(), strUin.c_str(), orderNo.c_str());
        return false;
    }   

    HTTP_REQ_STR(strSign1, sign);
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    http::CHttpRespMaker reqVerify;
    req.BuildRespMaker_Except(vecExcept, reqVerify);
    CMyString str1 = reqVerify.http_str();
    CMyString str2("%s&merchantKey=%s", str1.c_str(), pGame->getApiKey().c_str());
    CMyString strSign2 = CMD5Wrapper::encode(str2.c_str());
    if (strcasecmp(strSign1.c_str(), strSign2.c_str())!=0)
    {
        m_errCode = err_queryorderSignMismatch;
        resp["code"] = m_errCode;
        resp["message"] = "sign mismatch";
        LogErr("[%s]sign mismatch when user %s query order %s, origin text[%s], sign[%s], sign from request[%s]",
            m_sdkTag.c_str(), strUin.c_str(), orderNo.c_str(), str2.c_str(), strSign2.c_str(), strSign1.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]sign mismatch when user %s query order %s, origin text[%s], sign[%s], sign from request[%s]",
                m_sdkTag.c_str(), strUin.c_str(), orderNo.c_str(), str2.c_str(), strSign2.c_str(), strSign1.c_str()));
        return false;
    }
    return true;
}

bool CPayBase::createDepositQueryRequest(http::CHttpRequest &req, CGame* pGame, CSdk *pSdk, Json::Value &resp, int32_t &cost)
{
    // 构建HTTP请求相关数据
    CMyString httpData;
    CMyString strUrl;
    CMyString contentType;

    HTTP_REQ_STR(orderNo, orderNo);
    HTTP_REQ_STR(strUin, uin);

    // 虚函数调用，生成订单查询的数据
    try
    {
        this->doMakeDepositQueryRequest(req, pGame, pSdk, orderNo, strUrl, contentType, httpData);
    }
    catch(CException &ex)
    {
        LogFatal("[%s]call doMakeDepositQueryRequest failure, msg:%s", m_sdkTag.c_str(), ex.what());
        return false;
    }
    catch(std::exception &ex)
    {
        LogFatal("[%s]call doMakeDepositQueryRequest failure, msg:%s", m_sdkTag.c_str(), ex.what());
        return false;
    }
    catch(...)
    {
        LogFatal("[%s]call doMakeDepositQueryRequest failure", m_sdkTag.c_str());
        return false;
    }
 
    int32_t ret(0);
    // 发起HTTP POST请求
    http::VEC_STRING header_list;
    header_list.push_back(contentType);
    CMyString strResp;
    CMyString strRespHeader;
    int64_t httpBegin=Now(true);
    do {
        http::CHttpClient net;
        ret = net.HttpPostWithHeaders(strUrl.c_str(), 
                    strResp, strRespHeader, header_list,
                    httpData.c_str());
        cost = (int32_t)(Now(true) - httpBegin);
        if (ret != 0)
        {
            net.getError(ret, m_errNote);  
            m_errCode = err_queryorderHostFail;
            resp["code"] = m_errCode;
            resp["message"] = m_errNote;
            LogFatal("[%s]user %s query deposit failure, server[%s], data[%s], err:%d, msg:%s, cost:%.2lfms",
                m_sdkTag.c_str(), strUin.c_str(), strUrl.c_str(), httpData.c_str(), ret, m_errNote.c_str(), cost/1000.0);
            this->postAlert(pGame->getCode(), CMyString("平台错误=>查询充值订单状态时失败，订单号:%s, error:%s", orderNo.c_str(),m_errNote.c_str()));
            break;
        }

        // 虚函数调用，解析HTTP返回数据
        bool val(false);
        try
        {
            val = this->doParseDepositQueryResp(pGame, strRespHeader, strResp, resp, m_errNote);
        }
        catch(CException &ex)
        {
            LogFatal("[%s]call doParseDepositQueryResp failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false; 
        }
        catch(std::exception &ex)
        {
            LogFatal("[%s]call doParseDepositQueryResp failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false; 
        }
        catch(...)
        {
            LogFatal("[%s]call doParseDepositQueryResp failure", m_sdkTag.c_str());
            val = false;
        }
        if (!val)
        {
            m_errCode = err_queryorderResponeDataCorrupted;
            resp["code"] = m_errCode;
            resp["message"] = "respone data of deposit order corrupted.";
            LogErr("[%s]user %s request deposit failure, request data [%s], resp header [%s], resp data [%s], error:%s, cost:%.2lfms",
                m_sdkTag.c_str(), strUin.c_str(), httpData.c_str(), strRespHeader.c_str(), strResp.c_str(), m_errNote.c_str(), cost/1000.0);
            this->postAlert(pGame->getCode(), CMyString("平台错误=>查询充值订单状态返回数据错误，订单号:%s, error:%s", orderNo.c_str(),m_errNote.c_str()));
            break;
        }
    }while(0);

    if(m_errCode != 0)
    {
        return false;
    }

    LogMsg("[%s]user %s query deposit succeed, url=[%s], data=[%s], cost:%.2lfms",
            m_sdkTag.c_str(), strUin.c_str(), strUrl.c_str(), httpData.c_str(), cost/1000.0);
    return true;
}

bool CPayBase::createCreditQueryRequest(http::CHttpRequest &req, CGame* pGame, CSdk *pSdk, Json::Value &resp, int32_t &cost)
{
    // 构建HTTP请求相关数据
    CMyString httpData;
    CMyString strUrl;
    CMyString contentType;

    HTTP_REQ_STR(orderNo, orderNo);
    HTTP_REQ_STR(strUin, uin);

    // 虚函数调用，生成订单查询的数据
    try
    {
        this->doMakeCreditQueryRequest(req, pGame, pSdk, orderNo, strUrl, contentType, httpData);
    }
    catch(CException &ex)
    {
        LogFatal("[%s]call doMakeCreditQueryRequest failure, msg:%s", m_sdkTag.c_str(), ex.what());
        return false;
    }
    catch(std::exception &ex)
    {
        LogFatal("[%s]call doMakeCreditQueryRequest failure, msg:%s", m_sdkTag.c_str(), ex.what());
        return false;
    }
    catch(...)
    {
        LogFatal("[%s]call doMakeCreditQueryRequest failure", m_sdkTag.c_str());
        return false;
    }
 
    int32_t ret(0);
    // 发起HTTP POST请求
    http::VEC_STRING header_list;
    header_list.push_back(contentType);
    CMyString strResp;
    CMyString strRespHeader;
    int64_t httpBegin=Now(true);
    do {
        http::CHttpClient net;
        ret = net.HttpPostWithHeaders(strUrl.c_str(), 
                    strResp, strRespHeader, header_list,
                    httpData.c_str());
        cost = (int32_t)(Now(true) - httpBegin);
        if (ret != 0)
        {
            net.getError(ret, m_errNote);  
            m_errCode = err_queryorderHostFail;
            resp["code"] = m_errCode;
            resp["message"] = m_errNote;
            LogFatal("[%s]user %s query credit failure, server[%s], data[%s], err:%d, msg:%s, cost:%.2lfms",
                m_sdkTag.c_str(), strUin.c_str(), strUrl.c_str(), httpData.c_str(), ret, m_errNote.c_str(), cost/1000.0);
            this->postAlert(pGame->getCode(), CMyString("平台错误=>查询提现订单状态失败，订单号:%s, error:%s", orderNo.c_str(), m_errNote.c_str()));
            break;
        }

        // 虚函数调用，解析HTTP返回数据
        bool val(false);
        try
        {
            val = this->doParseCreditQueryResp(pGame, strRespHeader, strResp, resp, m_errNote);
        }
        catch(CException &ex)
        {
            LogFatal("[%s]call doParseCreditQueryResp failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(std::exception &ex)
        {
            LogFatal("[%s]call doParseCreditQueryResp failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(...)
        {
            LogFatal("[%s]call doParseCreditQueryResp failure", m_sdkTag.c_str());
            val = false;
        }
        if (!val)
        {
            /* TODO: 有可能的错误：
             * 1、充值范围越界，偏小、偏大
             * 2、通道已关闭
             */
            m_errCode = err_queryorderResponeDataCorrupted;
            resp["code"] = m_errCode;
            resp["message"] = "respone data of credit order corrupted.";
            LogErr("[%s]user %s request credit failure, request data [%s], resp header [%s], resp data [%s], error:%s, cost:%.2lfms",
                m_sdkTag.c_str(), strUin.c_str(), httpData.c_str(), strRespHeader.c_str(), strResp.c_str(), m_errNote.c_str(), cost/1000.0);
            this->postAlert(pGame->getCode(), CMyString("平台错误=>查询提现订单状态返回结果数据错误，订单号:%s, error:%s", orderNo.c_str(), m_errNote.c_str()));
            break;
        }
    }while(0);

    if(m_errCode != 0)
    {
        return false;
    }

    LogMsg("[%s]user %s query credit succeed, url=[%s], data=[%s], cost:%.2lfms",
            m_sdkTag.c_str(), strUin.c_str(), strUrl.c_str(), httpData.c_str(), cost/1000.0);
    return true;
}

void CPayBase::setWithdrawLog(const CMyString& orderNo, const int& result, CMyString oper, CMyString remark, CMyString message, int64_t createtime)
{
    int64_t creditTime = createtime;
    if (0 == creditTime)
    {
        creditTime = time(NULL);
    }
    CMyString strMessage = CBase64Wrapper::Encode(message.c_str(), message.length(), false);
    m_pDB->Cmd("call pWithdrawLog('%s', %d, '%s', '%s', '%s', %ld)", 
        orderNo.c_str(), result, oper.c_str(), remark.c_str(), strMessage.c_str(), creditTime);
	m_pDB->Exec();
}

void CPayBase::updateWithdrawLog(const CMyString& orderNo, const int& result, CMyString message)
{
    CMyString strMessage = CBase64Wrapper::Encode(message.c_str(), message.length(), false);
    m_pDB->Cmd("UPDATE `log_withdraw_operate` SET `status`=%d,`message`=FROM_BASE64('%s'),`update_time`=NOW() WHERE `withdraw_id`='%s' and `status`=5", 
                    result, strMessage.c_str(), orderNo.c_str());
    m_pDB->Exec();
}

void CPayBase::dealRespBack(void)
{
    if (m_qRespBackParam.empty())
    {
        return;
    }
    CMyString strParam = m_qRespBackParam.front();
    LogMsg("[%s] dealRespBack: %s", m_sdkTag.c_str(), strParam.c_str());
    m_qRespBackParam.pop();

    vector<KvstrItem> vecItem;
    safe_strtok(strParam.c_str(), vecItem, "&", "=");

    http::CHttpRequest req;
    for (size_t i = 0; i < vecItem.size(); ++i)
    {
        KvstrItem& item = vecItem[i];
        req.setParam(item.key, item.value);
    }

    bool val(false);
    CMyString orderNo;
    CMyString creditOrderNo;
    CMyString gameCode;
    Json::Value req_data;
    CGame *pGame(NULL);
    CSdk *pSdk(NULL);
    int64_t fee(0);         // 提现金额, 以分显示
    int64_t realFee(0);     // 实际到账金额，以分显示
    int64_t tax(0);         // 手续费，以分显示
    bool loanOK(false);
    enum CreditStatus creditStatus = credit_SdkLoanFault;
    do {
        try
        {
            // 虚函数回调，获得回调结果返回的订单号、结果等
            val = this->doParseCreditCallback(req, orderNo, creditOrderNo, fee, realFee, tax, loanOK);
        }
        catch(CException &ex)
        {
            LogFatal("[%s][%s:%s:%d]call doParseCreditCallback failure, msg:%s", m_sdkTag.c_str(),  __FILE__, __func__, __LINE__, ex.what());
            val = false;
        }
        catch(std::exception &ex)
        {
            LogFatal("[%s][%s:%s:%d]call doParseCreditCallback failure, msg:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, ex.what());
            val = false;
        }
        catch(...)
        {
            LogFatal("[%s][%s:%s:%d]call doParseCreditCallback failure, unknown error", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
            val = false;
        }
        if (!val)
        {
            m_errCode = err_creditCallbackFault;
            break;
        }

        // 查询订单数据
        val = this->fetchCreditOrderData(orderNo, req_data);
        if (!val)
        {
            // 查无此订单号
            break;
        }

        gameCode = req_data["gameCode"].asString();
        pGame = GAME_LIST.FindByCode(gameCode);
        if (NULL == pGame)
        {
            val = false;
            m_errCode = err_creditParameterFault;
            break;
        }
        pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
        if (NULL == pSdk)
        {
            val = false;
            m_errCode = err_creditSystemFault;
            this->postAlert(gameCode, CMyString("系统错误=>提现订单 %s 在结果回调时无法获得CSdk对象", orderNo.c_str()));
            break;
        }

        try
        {
            // 虚函数调用，验签
            val = this->validCreditCallbackSign(req, pGame, pSdk, m_errNote );
        }
        catch(CException &ex)
        {
            LogFatal("[%s]call validCreditCallbackSign failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(std::exception &ex)
        {
            LogFatal("[%s]call validCreditCallbackSign failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(...)
        {
            LogFatal("[%s]call validCreditCallbackSign failure", m_sdkTag.c_str());
            val = false;
        }
        if (!val)
        {
            m_errCode = err_creditCallbackSignMismatch;
            break;
        }

    }while(0);

    if (!val)
    {
        LogFatal("[%s] failure m_errCode:%d", m_sdkTag.c_str(), m_errCode);
        return;
    }

    creditStatus = loanOK ? credit_SdkLoanSucceed : credit_SdkLoanReject;
    LogDebug("[%s]credit callback, status:%d", m_sdkTag.c_str(), creditStatus);
    //更新税款到数据库
    if (tax > 0)
    {
        this->updateCreditTax(gameCode, orderNo, fee, realFee, tax);
    }
    // 回传到到数据库
    val = this->finishCreditOrder(gameCode, orderNo, creditOrderNo, creditStatus);

    // 通知游服处理
    CMyString strMessage = "OK";
    bool bSuccess = this->postCreditResultToGamesvr(pGame, pSdk, orderNo, fee, realFee, creditStatus);
    if (!bSuccess)
    {
        strMessage = m_errNote;
    }
    // 更新提现日志数据库-游服处理结果
    this->updateWithdrawLog(orderNo, creditStatus, strMessage);
}

void CPayBase::postAlarm(int32_t level, const CMyString &content)
{
    if (m_bOldAlt)
    {
        if (level == Alarm_Fatal)
        {
            LogFatalAlt(content.c_str());
        }
        else if (level == Alarm_Err)
        {
            LogErrAlt(content.c_str());
        }
        else
        {
            LogWarnAlt(content.c_str());
        }

        return;
    }

    if (m_strAlarmUrl.empty())
    {
        return;
    }

    CMyString strResp;
    CMyString strRespHeader;
    http::VEC_STRING header_list;
    CMyString contentType = "Content-Type: application/json";
    header_list.push_back(contentType);
    CMyDateTime now;

    Json::Value root;
    root["nodename"] = m_strNodeName;
    root["node"] = 0;
    root["level"] = level;
    root["group"] = m_nGroup;
    root["content"] = content;
    root["ts"] = now.time();
    CJsonWrapper jw(root);
    CMyString httpData = jw.toString();

    http::CHttpClient net;
    int32_t nRet = net.HttpPostWithHeaders(m_strAlarmUrl.c_str(), strResp, strRespHeader, header_list, httpData.c_str());
    if (nRet == 0)
    {
        LogDebug("[%s][%s:%s:%d] url:%s, httpData:%s, strResp:%s, strRespHeader:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strAlarmUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str());
    }
    else
    {
        CMyString strErrNote;
        net.getError(nRet, strErrNote);  
        LogErr("[%s][%s:%s:%d] url:%s, httpData:%s, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strAlarmUrl.c_str(), httpData.c_str(), nRet, strErrNote.c_str());
    }
}