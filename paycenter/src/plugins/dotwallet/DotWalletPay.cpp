/*
 * =====================================================================================
 *
 *       Filename:  DotWalletPay.cpp
 *
 *    Description:  数字钱包支付
 *
 *        Version:  1.0
 *        Created:  2021年11月30日 19时16分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "DotWalletPay.h"
#include "Security.h"
#include "SdkList.h"
#include "GameList.h"
#include "SdkChannelList.h"
#include "MyNet.h"
#include "Config.h"
#include "pay_error.h"
#include "BillPointList.h"
#include <algorithm>
#include "Security/CryptHelper.h"
#include "ecdsa_secp256k1.h"
#include "Common.h"
#include "BillpointDataList.h"

#define QueryOrder_Interval 30   //查询间隔30秒
#define QueryOrder_Num  5   //查询订单轮询次数
#define QueryOrderNumKey "DOTWALLET#ordernum#"
#define QueryDepositGameOrderExist "DOTWALLET#deposit#gameorder#"
#define QueryWithdrawGameOrderExist "DOTWALLET#withdraw#gameorder#"


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
        dealResp(resp, szResp, cbResp);
        return -1;
    }
    return 0;
}

#define CHECK_MYSQL_ERROR(err)      int32_t (err) = CheckMysqlError(m_pDB, szResp, cbResp); if (err!=0) { return -1; }

CDotWalletPay::CDotWalletPay()
    : CPayBase("DotWalletPay", depositRequest, httpRequestSync)
{
    m_nExpiresIn = 0;
    m_nTimeout = 0;
    m_bDepositAutoPay = false;
}

CDotWalletPay::~CDotWalletPay()
{

}

int32_t CDotWalletPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CDotWalletPay::Update(void)
{   
    m_mapAddress.clear();

    CMyString strAddress;
    CXmlFile &xml = CONFIG.getXml();
    CMyString sect = "main/dotwallet/config/address";
    int32_t nRet = xml.getString(sect, "BSV", strAddress);
    if (nRet == 0)
    {
        m_mapAddress["BSV"] = strAddress;
    }
    else
    {
        return -1;
    }
    strAddress.clear();
    nRet = xml.getString(sect, "BTC", strAddress);
    if (nRet == 0)
    {
        m_mapAddress["BTC"] = strAddress;
    }
    else
    {
        return -2;
    }
    strAddress.clear();
    nRet = xml.getString(sect, "ETH", strAddress);
    if (nRet == 0)
    {
        m_mapAddress["ETH"] = strAddress;
    }
    else
    {
        return -3;
    }

    CMyString sect2 = "main/dotwallet/config/auth_token";
    nRet = xml.getString(sect2, "url", m_strAuthUrl);
    if (nRet != 0)
    {
        return -4;
    }
    nRet = xml.getString(sect2, "client_id", m_strClientId);
    nRet = xml.getString(sect2, "client_secret", m_strClientSecret);


    CMyString sect3 = "main/dotwallet/config/payurl";
    nRet = xml.getString(sect3, "url", m_strPayUrl);
    if (nRet != 0)
    {
        return -5;
    }

    CMyString sect4 = "main/dotwallet/config/trans_url";
    nRet = xml.getString(sect4, "url", m_strTransUrl);
    if (nRet != 0)
    {
        return -6;
    }

    CMyString sect5 = "main/dotwallet/config/redis_key";
    nRet = xml.getString(sect5, "deposit_keyFormat", m_strDepositKeyFormat);
    if (nRet != 0)
    {
        return -7;
    }
    nRet = xml.getString(sect5, "credit_keyFormat", m_strCreditKeyFormat);
    if (nRet != 0)
    {
        return -8;
    }
    nRet = xml.getInt32(sect5, "timeout", m_nTimeout);

    CMyString sect6 = "main/dotwallet/config/user_receive_adress_url";
    nRet = xml.getString(sect6, "url", m_strUserAddressUrl);
    if (nRet != 0)
    {
        return -9;
    }
    
    if (!this->InitRedisCache(m_cacheCreditReq, m_redis, xml, "main/paycenter/credit/token"))
    {
        return -10;
    }

    CMyString sect7("main/dotwallet/credit/verifyorder");
    if (!CPayBase::InitRedisCache(m_cacheCreditOrder, m_redis, xml, sect7))
    {
        return -11;
    }

    CMyString sect8 = "main/dotwallet/config/autopay_balance_url";
    nRet = xml.getString(sect8, "url", m_strGetBalanceUrl);
    if (nRet != 0)
    {
        return -12;
    }

    int32_t nOpen = 0;
    CMyString sect9 = "main/dotwallet/config/deposit_autopay";
    nRet = xml.getInt32(sect9, "open", nOpen);
    if (nRet != 0)
    {
        return -13;
    }
    m_bDepositAutoPay = (nOpen == 1);

    //充值自动支付回调
    CMyString sect10("main/dotwallet/deposit/respback");
    if(!this->InitRedisCache(m_cacheDepositRespBack, m_redis, xml, sect10))
    {
        return -14;
    }

    //充值订单查询，处理没收到回调的情况
    CMyString sect11("main/dotwallet/deposit/queryorder/orderdata");
    if(!this->InitRedisCache(m_cacheQueryOrderData, m_redis, xml, sect11))
    {
        return -15;
    }

    CMyString sect12 = "main/dotwallet/config/queryorder_url";
    nRet = xml.getString(sect12, "url", m_strQueryOrderUrl);
    if (nRet != 0)
    {
        return -16;
    }

    CMyString sect13 = "main/dotwallet/config/autopay_transfer_url";
    nRet = xml.getString(sect13, "url", m_strAutoPayTransferUrl);
    if (nRet != 0)
    {
        return -17;
    }

    CMyString sect14 = "main/dotwallet/config/autopay_config_url";
    nRet = xml.getString(sect14, "url", m_strAutoPayConfigUrl);
    if (nRet != 0)
    {
        return -18;
    }

    GetRate();
    
    // 启动定时器列表
    m_timerRefershRate.SetInterval(60000); //一分钟轮询一次
    m_timerRefershRate.Startup();
    m_timerQueryOrder.SetInterval(QueryOrder_Interval * 1000);  // 每10秒检查
    m_timerQueryOrder.Startup();

    return CPayBase::Init("main/dotwallet");
}

void CDotWalletPay::Maintance(void)
{
    CPayBase::Maintance();

    this->dealDepositRespBack();
    if (m_timerRefershRate.ToNextTime())
    {
        this->GetRate();
    }

    if (m_timerQueryOrder.ToNextTime())
    {
        this->dealUnFinishOrder();
    }
}

ENTRY_FUNC_DECLARE(CDotWalletPay, doDeposit)
{
    CMyString strBillNo = req.getParam("billNo");
    CMyString strRedisKey("%s%s", QueryDepositGameOrderExist, strBillNo.c_str());
    CMyString strResp;
    if (m_redis.Get(strRedisKey, strResp) > 0)
    {
        LogWarn("[%s][%s:%s:%d] billNo=%s, has exist, strResp:%s", m_sdkTag.c_str(), 
            __FILE__, __func__, __LINE__, strBillNo.c_str(), strResp.c_str());
        safe_strcpy(szResp, cbResp, strResp.c_str());
        cbResp = strlen(szResp);
        return 0;
    }

    m_strCurrentOrderNo.clear();
    if (m_bDepositAutoPay)
    {  
        CMyString strCoinType = req.getParam("productId");  //传coin_type
        CMyString strAccount = req.getParam("account"); //钱包用户名
        int32_t amount = req.getParamInt("amount"); //分为单位
        int64_t nCoin = this->GetRMBTransCoin(amount, strCoinType);

        int64_t nBanlance = 0;
        bool bRet = this->GetUserAutoPayBalance(strCoinType, strAccount, nBanlance);
        if (bRet)
        {
            LogWarn("[%s] nBanlance=%d, nCoin=%d, auto pay", m_sdkTag.c_str(), nBanlance, nCoin);
            int32_t n = doDepositAutoPay(pClient, req, szResp, cbResp, ext_headers);
            m_redis.Set(strRedisKey, szResp, 30000);
            return n;
        }
        else
        {
            LogWarn("[%s] nBanlance=%d, nCoin=%d, banlance not enough", m_sdkTag.c_str(), nBanlance, nCoin);
            int32_t n = CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
            m_redis.Set(strRedisKey, szResp, 30000);
            return n;
        }
    }

    int32_t n = CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
    m_redis.Set(strRedisKey, szResp, 30000);
    return n;
}

ENTRY_FUNC_DECLARE(CDotWalletPay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CDotWalletPay, doWithdraw)
{
    CMyString strGameOrderNo = req.getParam("gameOrderNo");
    CMyString strRedisKey("%s%s", QueryWithdrawGameOrderExist, strGameOrderNo.c_str());
    CMyString strResp;
    if (m_redis.Get(strRedisKey, strResp) > 0)
    {
        LogWarn("[%s][%s:%s:%d] gameOrderNo=%s, has exist, strResp:%s", m_sdkTag.c_str(), 
            __FILE__, __func__, __LINE__, strGameOrderNo.c_str(), strResp.c_str());
        safe_strcpy(szResp, cbResp, strResp.c_str());
        cbResp = strlen(szResp);
        return 0;
    }

    int32_t nRet = this->doWithdrawRequest(pClient, req, szResp, cbResp, ext_headers);
    if (nRet != 0)
    {
        m_redis.Set(strRedisKey, szResp, 30000);
        return nRet;
    }
    nRet = this->doWithdrawAudit(pClient, req, szResp, cbResp, ext_headers);
    if (nRet != 0)
    {
        m_redis.Set(strRedisKey, szResp, 30000);
        return nRet;
    }
    this->doWithdrawCredit(pClient, req, szResp, cbResp, ext_headers);
    
    CMyString gameCode = req.getParam("gameCode");
    CGame* pGame = GAME_LIST.FindByCode(gameCode);
    if (pGame)
    {
        std::vector<CMyString> vecExcept;
        vecExcept.push_back("sign");
        http::CHttpRespMaker reqVerify;
        req.BuildRespMaker_Except(vecExcept, reqVerify);
        CMyString str1 = reqVerify.http_str();
        CMyString str2("%s&merchantKey=%s", str1.c_str(), pGame->getApiKey().c_str());
        CMyString strSign2 = CMD5Wrapper::encode(str2.c_str());
        req.setParam("sign", strSign2);
    }
    else
    {
        CMyString strMsg;
        strMsg.Format("无此游服代码 %s", gameCode.c_str());
        Json::Value resp;
        resp["code"] = PAYFRAME_ERROR(err_InvalidGamesvr);
        resp["message"] = strMsg;
        resp["gameCode"] = gameCode;

        LogErr("[%s][%s:%s:%d] %s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strMsg.c_str());
        dealResp(resp, szResp, cbResp);
        return 200;
    }
    
    int32_t n = CPayBase::doCreditVerify(pClient, req, szResp, cbResp, ext_headers);
    m_redis.Set(strRedisKey, szResp, 30000);
    return n;
}

ENTRY_FUNC_DECLARE(CDotWalletPay, doWithdrawCB)
{
    return CPayBase::doCreditCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CDotWalletPay, doGetRate)
{
    ext_headers["Content-Type"] = "application/json";
    
    CJsonWrapper js(m_jsRate);
    safe_strcpy(szResp, cbResp, js.toString().c_str());
    cbResp = strlen(szResp);
    return 200;
}

int32_t CDotWalletPay::doDepositAutoPay(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers)
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
    LogDebug("[%s]deposit autopay player ip is [%s]", m_sdkTag.c_str(), clientIp.c_str());
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
    
    CMyString platformOrderNo("N/A");   // 三方平台的订单号
    int64_t platformCode = 0;
    int nRet = this->doRechargeAutoPay(req, pGame, pSdk, orderNo, platformOrderNo, platformCode);
    if (nRet != 0)
    {
        // 数据库中的订单出错，取消
        this->updateDepositOrder(pGame->getCode(), orderId, deposit_RequestFail, platformOrderNo);

        if (platformCode == 10180007) //表示用户的自动支付余额不足 引导用户跳转到打点自动支付划转页面进行充值
        {
            CMyString strRedirectUrl = pGame->getCallbackUrl();
            strRedirectUrl = CHttpWrapper::urlEncode(strRedirectUrl);
            CMyString strPlatformUrl = m_strAutoPayTransferUrl + strRedirectUrl;
            this->doPayResp(pChannel, strPlatformUrl, orderNo, szResp, cbResp);
        }
        else if (platformCode == 10180029)//表示订单支付金额大于用户的授权金额 引导用户跳转到打点自动支付单笔限额设置页面进行限额设置
        {
            CMyString strPlatformUrl = m_strAutoPayConfigUrl + m_strClientId;
            this->doPayResp(pChannel, strPlatformUrl, orderNo, szResp, cbResp);
        }
        else
        {
            this->doPayFailure(gameCode, strUin, orderNo, fee, szResp, cbResp);
        }
        return m_errCode;
    }

    // 更新订单状态
    this->updateDepositOrder(pGame->getCode(), orderId, deposit_RequestSucceed, platformOrderNo);

    // 保存到redis中,用于回调检查之用
    this->storeDepositOrder(req, orderId, orderNo);

    CMyString strParam;
    CMyString strKey;
    nRet = m_cacheDepositRespBack.Pop(orderNo, strKey, strParam);
    if (nRet > 0 && !strParam.empty())
    {
        m_qDepositRespBackParam.push(strParam);
    }
    else
    {   
        //回调还没回来，设置轮询
        //设置查询订单轮询次数 
        CMyString strRealKey("%s%s", QueryOrderNumKey, orderNo.c_str());
        m_redis.Set(strRealKey, QueryOrder_Num);
        //设置查询订单轮询数据
        Json::Value order_data;
        order_data["orderNo"] = orderNo;
        order_data["platformOrderNo"] = platformOrderNo;
        order_data["deal_time"] = 0;
        m_cacheQueryOrderData.Push(order_data);
    }

    // 构建返回信息给游服
    this->doPayResp(pChannel, "auto", orderNo, szResp, cbResp);

    LogMsg("[%s]request deposit autopay succeed, gameCode:%s, uin:%s, orderNo:%s",
            m_sdkTag.c_str(), gameCode.c_str(), strUin.c_str(), orderNo.c_str());
    return 0;
}

int32_t CDotWalletPay::doRechargeAutoPay(http::CHttpRequest &req, CGame* pGame, CSdk *pSdk, const CMyString &orderNo, CMyString& platformOrderNo, int64_t& platformCode)
{
    Json::Value data;
    Json::Value to(Json::arrayValue);
    Json::Value product(Json::objectValue);

    CMyString strClientId, strClientSecret;
    Json::Value extraData = pSdk->getPayExtraData();
    if (!extraData.empty())
    {
        strClientId = extraData["appid"].asString();
        strClientSecret = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strClientId: %s, strClientSecret:%s", __FILE__, __func__, __LINE__, strClientId.c_str(), strClientSecret.c_str());
    }

    CMyString strCoinType = req.getParam("productId");  //传coin_type
    CMyString strUserId = req.getParam("account");
    int32_t nRMBAmount = req.getParamInt("amount"); //分为单位
    int64_t nCoin = this->GetRMBTransCoin(nRMBAmount, strCoinType);

    data["out_order_id"] = orderNo;
    data["coin_type"] = strCoinType;
    data["user_id"] = strUserId;  //兑出付款方userid，此时为用户userid
    data["notify_url"] = pSdk->getPayCallbackUrl();
    
    map<CMyString, CMyString>::iterator it_find = m_mapAddress.find(strCoinType);
    if (it_find != m_mapAddress.end())
    {
        Json::Value item;
        item["type"] = "address";
        item["content"] = it_find->second;
        item["amount"] = nCoin;
        to.append(item);
    }
    data["to"] = to;

    CMyString strProductTitle;
    int32_t bpId = req.getParamInt("bpId");
    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    if (NULL != pBP)
    {
        strProductTitle = pBP->getBpCode();

        product["id"] = CMyString("%d", pBP->getId());
        product["name"] = strProductTitle;
    }
    else
    {
        LogErr("[%s]CDotWalletPay::doMakeDepositRequest >not found billpoint %d", m_sdkTag.c_str(), bpId);
        this->postAlarm(Alarm_Err, CMyString("[%s]CDotWalletPay::doMakeDepositRequest >not found billpoint %d", 
                m_sdkTag.c_str(), bpId));
    }
    data["product"] = product;
    data["subject"] = strProductTitle;

    {
        Json::Value redisData;
        redisData["coin_type"] = strCoinType;
        redisData["amount"] = nRMBAmount;
        redisData["coin"] = nCoin;
        redisData["orderNo"] = orderNo;
        CJsonWrapper jwRedis(redisData);
        CMyString strRedisValue = jwRedis.toString();
        CMyString strRedisKey(m_strDepositKeyFormat.c_str(), orderNo.c_str());
        m_redis.Set(strRedisKey, strRedisValue, m_nTimeout, 1);
        LogMsg("[%s:%s:%d] strRedisKey: %s, strRedisValue:%s", __FILE__, __func__, __LINE__, strRedisKey.c_str(), strRedisValue.c_str());
    }

    CJsonWrapper jw(data);
    CMyString httpData = jw.toString();
    CMyString strUrl = pSdk->getWithdrawUrl();
    CMyString contentType = "Content-Type: application/json";
    CMyString strAuthorization = "Authorization: Bearer ";
    strAuthorization += this->GetAccessToken(strClientId, strClientSecret);

    http::VEC_STRING header_list;
    header_list.push_back(contentType);
    header_list.push_back(strAuthorization);
    CMyString strRespHeader;
    CMyString strResp;

    m_errCode = 0;
    int64_t httpBegin = Now(true);
    http::CHttpClient net;
    int32_t ret = net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list,  httpData.c_str());
    int32_t cost = Now(true) - httpBegin;
    if (ret != 0)
    {
        net.getError(ret, m_errNote);  
        m_errCode = err_depositHostFail;
        LogErr("[%s][%s:%s:%d]deposit autopay failure, url [%s], data [%s], strResp [%s], err:%d, msg:%s, cost:%.2lf",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), httpData.c_str(), strResp.c_str(), ret, m_errNote.c_str(), cost/1000.0f);
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d]deposit autopay failure, url [%s], data [%s], strResp [%s], err:%d, msg:%s, cost:%.2lf", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), httpData.c_str(), strResp.c_str(), ret, m_errNote.c_str(), cost/1000.0f));
    }
    else 
    {
        LogDebug("[%s][%s:%s:%d] deposit autopay success, url [%s], data [%s], strResp [%s], header [%s], cost:%.2lf", 
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str(), cost/1000.0f);

        do
        {
            m_errCode = err_depositRequestDataCorrupted;
            CJsonWrapper jrRoot;
            if (!jrRoot.Init(strResp))
            {
                LogErr("[%s:%s:%d] resp data [%s] not json string", __FILE__, __func__, __LINE__, strResp.c_str());
                m_errNote.Format("resp data [%s] not json string", strResp.c_str());
                break;
            }

            platformCode = jrRoot.getInt("code");
            if (platformCode != 0)
            {
                LogErr("[%s:%s:%d] resp code is %d", __FILE__, __func__, __LINE__, platformCode);
                CMyString strMsg = jrRoot.getString("msg");
                m_errNote.Format("resp code is %d, msg:%s", platformCode, strMsg.c_str());
                break;
            }

            Json::Value data;
            if (!jrRoot.getJsonObject("data", data))
            {
                LogErr("[%s:%s:%d] data is not object", __FILE__, __func__, __LINE__);
                m_errNote.Format("data is not object");
                break;
            }

            platformOrderNo = data["order_id"].asString();         
            m_errCode = 0;

        } while (false);   
    }

    return m_errCode;
}

bool CDotWalletPay::delayedDealDepositCallback(http::CHttpRequest& req, const CMyString& orderNo)
{
    CMyString params;
    req.getFullParamStr(params);
    LogMsg("[%s]delayedDealDepositCallback, param[%s]", m_sdkTag.c_str(), params.c_str());
    m_cacheDepositRespBack.Push(orderNo, params);

    return true;
}

void CDotWalletPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    CMyString strCoinType = req.getParam("productId");  //传coin_type
    int32_t amount = req.getParamInt("amount"); //分为单位
    int64_t nCoin = this->GetRMBTransCoin(amount, strCoinType);

    Json::Value data;
    Json::Value to(Json::arrayValue);
    Json::Value product(Json::objectValue);

    data["out_order_id"] = orderNo;
    data["coin_type"] = strCoinType;
    map<CMyString, CMyString>::iterator it_find = m_mapAddress.find(strCoinType);
    if (it_find != m_mapAddress.end())
    {
        Json::Value item;
        item["type"] = "address";
        item["content"] = it_find->second;
        item["amount"] = nCoin;
        to.append(item);
    }
    data["to"] = to;

    CMyString strProductTitle;
    int32_t bpId = req.getParamInt("bpId");
    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    if (NULL != pBP)
    {
        strProductTitle = pBP->getBpCode();

        product["id"] = CMyString("%d", pBP->getId());
        product["name"] = strProductTitle;
    }
    else
    {
        LogErr("[%s]CDotWalletPay::doMakeDepositRequest >not found billpoint %d", m_sdkTag.c_str(), bpId);
    }
    data["product"] = product;
    data["subject"] = strProductTitle;
    data["notify_url"] = pSdk->getPayCallbackUrl();
    data["redirect_uri"] = pGame->getCallbackUrl();

    CMyDateTime tNow;
    data["expires"] = tNow.time() + m_nTimeout;

    CMyString strClientId, strClientSecret;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strClientId = extraData["appid"].asString();
        strClientSecret = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strClientId: %s, strClientSecret:%s", __FILE__, __func__, __LINE__, strClientId.c_str(), strClientSecret.c_str());
    }

    {
        Json::Value redisData;
        redisData["coin_type"] = strCoinType;
        redisData["amount"] = amount;
        redisData["coin"] = nCoin;
        redisData["orderNo"] = orderNo;
        CJsonWrapper jwRedis(redisData);
        CMyString strRedisValue = jwRedis.toString();
        CMyString strRedisKey(m_strDepositKeyFormat.c_str(), orderNo.c_str());
        m_redis.Set(strRedisKey, strRedisValue, m_nTimeout, 1);
        LogMsg("[%s:%s:%d] strRedisKey: %s, strRedisValue:%s", __FILE__, __func__, __LINE__, strRedisKey.c_str(), strRedisValue.c_str());
    }

    CJsonWrapper jwData(data);
    httpData = jwData.toString();
    url = pSdk->getPayUrl();
    contentType = "Content-Type: application/json;Authorization: Bearer ";
    contentType += this->GetAccessToken(strClientId, strClientSecret);

    //设置查询订单轮询次数
    m_strCurrentOrderNo = orderNo;
    CMyString strRealKey("%s%s", QueryOrderNumKey, orderNo.c_str());
    m_redis.Set(strRealKey, QueryOrder_Num);
}

bool CDotWalletPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    LogDebug("[%s]deposit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        LogErr("[%s:%s:%d] resp data [%s] not json string", __FILE__, __func__, __LINE__, respData.c_str());
        errNote.Format("resp data [%s] not json string", respData.c_str());
        return false;
    }

    int32_t nCode = jrRoot.getInt("code");
    if (nCode != 0)
    {
        LogErr("[%s:%s:%d] resp code is %d", __FILE__, __func__, __LINE__, nCode);
        CMyString strMsg = jrRoot.getString("msg");
        errNote.Format("resp code is %d, msg:%s", nCode, strMsg.c_str());
        return false;
    }

    CMyString strData = jrRoot.getString("data");
    depositOrderNo = strData;
    depositUrlData = m_strPayUrl + strData;

    //设置查询订单轮询数据
    Json::Value order_data;
    order_data["orderNo"] = m_strCurrentOrderNo;
    order_data["platformOrderNo"] = depositOrderNo;
    order_data["deal_time"] = 0;
    m_cacheQueryOrderData.Push(order_data);

    return true;
}


bool CDotWalletPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &depositOK)
{
    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString params;
    req.getFullParamStr(params);
    LogDebug("[%s]deposit callback, header[%s], param[%s] post[%s]",
            m_sdkTag.c_str(), headers.c_str(), params.c_str(), strPost.c_str());

    CMyString str1 = req.getParam("payload");
    if (str1.empty())
    {
        LogWarn("[%s]CDotWalletPay::doParseDepositCallback payload is empty", m_sdkTag.c_str());
        return false;
    }

    CJsonWrapper jrParam;
    if (!jrParam.Init(str1))
    {
        LogErr("[%s]CDotWalletPay::doParseDepositCallback parse json err. payload: %s", m_sdkTag.c_str(), str1.c_str());
        return false;
    }

    orderNo = jrParam.getString("out_order_id");
    platformOrderNo = jrParam.getString("order_id");
    int64_t nCoinFee = (int64_t)(jrParam.getString("amount").asInt64()); //交易金额，单位：聪 satoshi(bsv)或者 gwei （eth)
    
    {
        //回调回来删除轮询数据
        CMyString strRealKey("%s%s", QueryOrderNumKey, orderNo.c_str());
        m_redis.Del(strRealKey);
    }

    CMyString strRedisValue;
    CMyString strRedisKey(m_strDepositKeyFormat.c_str(), orderNo.c_str());
    if (m_redis.Get(strRedisKey, strRedisValue) != 1)
    {
        LogErr("[%s]CDotWalletPay::doParseDepositCallback order err, redis not find, strRedisKey:%s", m_sdkTag.c_str(), strRedisKey.c_str());
        return false;
    }
    LogMsg("[%s:%s:%d] strRedisKey: %s, strRedisValue:%s", __FILE__, __func__, __LINE__, strRedisKey.c_str(), strRedisValue.c_str());
    CJsonWrapper JsRedis;
    if (!JsRedis.Init(strRedisValue))
    {
        LogErr("[%s]CDotWalletPay::doParseDepositCallback redisvalue parse json err. strRedisValue: %s", m_sdkTag.c_str(), strRedisValue.c_str());
        return false;
    }

    int64_t nRedisCoin = JsRedis.getUInt64("coin");
    LogDebug("[%s] [%s:%d] coin range: %u - %u", m_sdkTag.c_str(), __FILE__, __LINE__, uint32_t(nRedisCoin * 0.9), uint32_t(nRedisCoin * 1.1));
    if (nCoinFee < uint32_t(nRedisCoin * 0.9) || nCoinFee > uint32_t(nRedisCoin * 1.1))
    {
        LogErr("[%s][%s:%s:%d] coin error, nCoinFee: %d, order nRedisCoin: %u", m_sdkTag.c_str(), __FILE__, __func__, __LINE__,  nCoinFee, nRedisCoin);
        this->postAlarm(Alarm_Fatal, CMyString("[%s][%s:%s:%d] coin error, nCoinFee: %d, order nRedisCoin: %u", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__,  nCoinFee, nRedisCoin));
        return false;
    }
    
    //m_redis.Del(strRedisKey);
    fee = JsRedis.getUInt64("amount");
    realFee = fee;

    CMyString strCoinType = JsRedis.getString("coin_type");
    int64_t nTaxFee = jrParam.getString("fee").asInt64();//交易矿工费，单位：聪 satoshi(bsv)或者 gwei （eth)
    tax = this->GetCoinTransRMB(nTaxFee, strCoinType); 

    depositOK = true;
    return true;
}

bool CDotWalletPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("signature");
    CMyString strPayLoad = req.getParam("payload");
    CMyString strPubKey = req.getParam("pubkey");
    
    std::vector<uint8_t> vchSig;
    ECDSAWrapper::readhexEx(vchSig, strSign1.c_str());
    std::vector<uint8_t> vchPub;
    ECDSAWrapper::readhexEx(vchPub, strPubKey.c_str());

    int bRet = ECDSAWrapper::verify(vchSig, strPayLoad.c_str(), vchPub);
    // 验证签名是否匹配，忽略大小写
    if (bRet == 0)
    {
        LogErr("[%s]deposit callback sign mismatch, payload[%s], signature[%s], pubkey[%s]",
                m_sdkTag.c_str(), strPayLoad.c_str(), strSign1.c_str(), strPubKey.c_str());
        this->postAlarm(Alarm_Fatal, CMyString("[%s]deposit callback sign mismatch, payload[%s], signature[%s], pubkey[%s]", 
                m_sdkTag.c_str(), strPayLoad.c_str(), strSign1.c_str(), strPubKey.c_str()));
        return false;
    }
    
    CJsonWrapper jrParam;
    if (jrParam.Init(strPayLoad))
    {
        CMyString orderNo = jrParam.getString("out_order_id");
        CMyString strRedisValue;
        CMyString strRedisKey(m_strDepositKeyFormat.c_str(), orderNo.c_str());

        if (m_redis.Get(strRedisKey, strRedisValue) == 1)
        {
            m_redis.Del(strRedisKey);
            LogDebug("[%s] deposit del redis key, strRedisKey:%s strRedisValue:%s", m_sdkTag.c_str(), strRedisKey.c_str(), strRedisValue.c_str());
        }
    }
    return true;
}

void CDotWalletPay::doDepositCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    Json::Value resp;
    if (bSucceed)
    {
        resp["code"] = 0;
        resp["msg"] = "";
    }
    else
    {
        resp["code"] = errCode;
        resp["msg"] = "";
    }
    
    CJsonWrapper js(resp);
    safe_strcpy(szResp, cbResp, js.toString().c_str());
    cbResp = strlen(szResp);
}

int32_t CDotWalletPay::doDepositHttpRequest(http::CHttpClient &net, const CMyString &strUrl, http::VEC_STRING &header_list, const CMyString &httpData, CMyString &strResp, CMyString &strRespHeader)
{
    if (!header_list.empty())
    {
        CMyString strContentType = header_list[0];
        uint32_t nPos = strContentType.find(";");
        if (nPos != std::string::npos)
        {
            CMyString strContent1 = strContentType.substr(0, nPos);
            CMyString strContent2 = strContentType.substr(nPos + 1);
            header_list.clear();
            header_list.push_back(strContent1);
            header_list.push_back(strContent2);
        }
    }

    return net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list,  httpData.c_str());
}

void CDotWalletPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData) 
{
    Json::Value data;
    Json::Value to(Json::arrayValue);
    Json::Value product(Json::objectValue);

    CMyString strClientId, strClientSecret;
    Json::Value extraData = pSdk->getPayExtraData();
    if (!extraData.empty())
    {
        strClientId = extraData["appid"].asString();
        strClientSecret = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strClientId: %s, strClientSecret:%s", __FILE__, __func__, __LINE__, strClientId.c_str(), strClientSecret.c_str());
    }

    CMyString strCoinType = req["bankCode"].asString();
    CMyString strUserId = req["bankName"].asString();
    int64_t nRMBAmount = req["fee"].asInt64();  //单位分
    int64_t nCoin = GetRMBTransCoin(nRMBAmount, strCoinType);
    CMyString strAccessToken = this->GetAccessToken(strClientId, strClientSecret);

    data["out_order_id"] = req["orderNo"].asString();
    data["coin_type"] = strCoinType;
    data["user_id"] = pSdk->getAccount();  //兑出付款方userid
    CMyString strCallbackUrl = pSdk->getWithdrawCallbackUrl();
    LogWarn("[%s] strCallbackUrl:%s", m_sdkTag.c_str(), strCallbackUrl.c_str());
    data["notify_url"] = strCallbackUrl;
    
    Json::Value item;
    item["type"] = "address";
    item["content"] = this->GetUserReceiveAddress(strAccessToken, strCoinType, strUserId);
    item["amount"] = nCoin;
    to.append(item);
    data["to"] = to;

    product["id"] = req["orderNo"].asString();
    product["name"] = req["orderNo"].asString();
    data["product"] = product;

    {
        Json::Value redisData;
        redisData["coin_type"] = strCoinType;
        redisData["amount"] = nRMBAmount;
        redisData["coin"] = nCoin;
        redisData["orderNo"] = req["orderNo"].asString();
        CJsonWrapper jwRedis(redisData);
        CMyString strRedisValue = jwRedis.toString();
        CMyString strRedisKey(m_strCreditKeyFormat.c_str(), req["orderNo"].asString().c_str());
        m_redis.Set(strRedisKey, strRedisValue, m_nTimeout, 1);
        LogMsg("[%s:%s:%d] strRedisKey: %s, strRedisValue:%s", __FILE__, __func__, __LINE__, strRedisKey.c_str(), strRedisValue.c_str());
    }

    CJsonWrapper jw(data);
    httpData = jw.toString();
    url = pSdk->getWithdrawUrl();
    contentType = "Content-Type: application/json;Authorization: Bearer ";
    contentType += this->GetAccessToken(strClientId, strClientSecret);
}

int32_t CDotWalletPay::doCreditHttpRequest(http::CHttpClient &net, const CMyString &strUrl, http::VEC_STRING &header_list, const CMyString &httpData, CMyString &strResp, CMyString &strRespHeader)
{
    if (!header_list.empty())
    {
        CMyString strContentType = header_list[0];
        uint32_t nPos = strContentType.find(";");
        if (nPos != std::string::npos)
        {
            CMyString strContent1 = strContentType.substr(0, nPos);
            CMyString strContent2 = strContentType.substr(nPos + 1);
            header_list.clear();
            header_list.push_back(strContent1);
            header_list.push_back(strContent2);
        }
    }

    return net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list,  httpData.c_str());
}

bool CDotWalletPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    LogDebug("[%s]credit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());

    errNote.clear();
    bool bSuccess = false;
    do
    {
        CJsonWrapper jrRoot;
        if (!jrRoot.Init(respData))
        {
            LogErr("[%s:%s:%d] resp data [%s] not json string", __FILE__, __func__, __LINE__, respData.c_str());
            errNote.Format("resp data [%s] not json string", respData.c_str());
            break;
        }

        int32_t nCode = jrRoot.getInt("code");
        if (nCode != 0)
        {
            LogErr("[%s:%s:%d] resp code is %d", __FILE__, __func__, __LINE__, nCode);
            CMyString strMsg = jrRoot.getString("msg");
            errNote.Format("resp code is %d, msg:%s", nCode, strMsg.c_str());
            break;
        }

        Json::Value data;
        if (!jrRoot.getJsonObject("data", data))
        {
            LogErr("[%s:%s:%d] data is not object", __FILE__, __func__, __LINE__);
            errNote.Format("data is not object");
            break;
        }

        creditOrderNo = data["order_id"].asString();
        bSuccess = true;

        CMyString strOrderNo = data["out_order_id"].asString();
        CMyString strRedisValue;
        CMyString strRedisKey(m_strCreditKeyFormat.c_str(), strOrderNo.c_str());
        if (m_redis.Get(strRedisKey, strRedisValue) == 1)
        {
            CJsonWrapper JsRedis;
            if (JsRedis.Init(strRedisValue))
            {
                Json::Value respData(Json::objectValue);
                respData["fee"] = JsRedis.getUInt64("amount");
                respData["orderNo"] = strOrderNo;
                respData["bankCode"] = JsRedis.getString("coin_type");
                
                CJsonWrapper jsResp(respData);
                errNote = jsResp.toString();
                LogMsg("[%s:%s:%d] errNote:%s", __FILE__, __func__, __LINE__, errNote.c_str());
            }
        }

    } while (false);
    
    // if (!bSuccess)
    // {
    //     nRet = this->notifyGamesvr(gameCode, strUin, orderNo, gameOrderNo, 
    //         fee, 2, gameResult, pGame, strError);
    //     this->notifyGamesvr();
    // }

    return bSuccess;
}


void CDotWalletPay::addCreditVerifyNeededParam(std::vector<CMyString> &vecParam)
{
    vecParam.push_back("gameOrderNo");
    vecParam.push_back("fee");
    vecParam.push_back("bankName");
}

void CDotWalletPay::storeCreditVerifyData(CGame *pGame, CSdk *pSdk, http::CHttpRequest &req, Json::Value &data, Json::Value &resp_data)
{
    CMyString bankCode = req.getParam("bankCode");
    CMyString strUin =  req.getParam("uin");
    CMyString gameCode = req.getParam("gameCode");

    if (bankCode.empty())
    {
        m_pDB->Cmd("SELECT productId FROM `t_payorder` WHERE uin = %s AND `orderStatus` IN(5,8,9,10) AND `gameCode` = '%s' ORDER BY updatedAt DESC LIMIT 1;", strUin.c_str(), gameCode.c_str());
        while(m_pDB->More())
        {
            bankCode = m_pDB->GetString(0);
        }
    }

    data["gameCode"] = req.getParam("gameCode");
    data["gameOrderNo"] = req.getParam("gameOrderNo");
    data["fee"] = req.getParamInt64("fee");
    
    CMyString bankName_b64 = req.getParam("bankName");
    CMyString bankName = CBase64Wrapper::Decode(bankName_b64.c_str(), bankName_b64.length(), false);
    data["bankName"] = bankName;
    data["bankCard"] = req.getParam("bankCard");
    data["bankCode"] = bankCode;
    data["tax"] = req.getParam("tax");
    data["uin"] = req.getParam("uin");
    data["orderNo"] = req.getParam("orderNo");
    data["creditRemark"] = req.getParam("creditRemark");
    data["creditTime"] = time(NULL);
    data["operator"] = req.getParam("operator");

    // 保存到cache中，稍后发送
    int32_t nRet = m_cacheCreditOrder.Push(data);
    if (nRet < 0)
    {
        LogErr("[%s] SaveData error[%s]", m_sdkTag.c_str(), m_redis.getLastError());
        this->postAlarm(Alarm_Err, CMyString("[%s] SaveData error[%s]", 
                m_sdkTag.c_str(), m_redis.getLastError()));
    }
    else
    {
        LogMsg("[%s] SaveData success", m_sdkTag.c_str());
    }

    resp_data["bankCode"] = bankCode;
    resp_data["fee"] = req.getParamInt("fee");
}

int32_t CDotWalletPay::verifyCreditRequest(Json::Value &req_data, bool &creditOK, CMyString &creditOrderNo, CMyString& errNote)
{
    CMyString strKey, strData;
    int32_t nRet = 0;
    int32_t ret = m_cacheCreditOrder.Pop(strKey, strData);
    if (ret > 0)
    {
        LogDebug("[%s]getIncrData ret:[%d] strKey:[%s] strData:[%s]", m_sdkTag.c_str(), ret, strKey.c_str(), strData.c_str());
        
        CJsonWrapper jr;
        if (!jr.parse(strData))
        {
            LogDebug("[%s] dealCallBack parse fail", m_sdkTag.c_str());
            return 0;
        }
        Json::Value &root = jr.json();

        req_data = root;
        int32_t n = this->requestDotWalletAutoPay(req_data, creditOK, creditOrderNo, errNote);
        if (n == 0)
        {
            nRet = 1;
        }
        else
        {
            nRet = -1;
        }
    }

    return nRet;
}

int32_t CDotWalletPay::requestDotWalletAutoPay(Json::Value &req_data, bool &creditOK, CMyString &creditOrderNo, CMyString& errNote)
{
    errNote.clear();
    CMyString gameCode = req_data["gameCode"].asString();
    CGame *pGame = GAME_LIST.FindByCode(gameCode);
    if (NULL == pGame)
    {
        errNote.Format("not found gameCode %s when credit verify response", gameCode.c_str());
        LogFatal("[%s][%s:%s:%d]not found gameCode %s when credit verify response", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, gameCode.c_str());
        return -1;
    }
    CSdk* pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
    if (NULL == pSdk)
    {
        errNote.Format("invalid sdk when deposit verify response, gameCode:%s, sdkCode:%s", gameCode.c_str(), m_sdkCode.c_str());
        LogFatal("[%s][%s:%s:%d]invalid sdk when deposit verify response, gameCode:%s, sdkCode:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, gameCode.c_str(), m_sdkCode.c_str());
        return -2;
    }

    Json::Value data;
    Json::Value to(Json::arrayValue);
    Json::Value product(Json::objectValue);

    CMyString strClientId, strClientSecret;
    Json::Value extraData = pSdk->getPayExtraData();
    if (!extraData.empty())
    {
        strClientId = extraData["appid"].asString();
        strClientSecret = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strClientId: %s, strClientSecret:%s", __FILE__, __func__, __LINE__, strClientId.c_str(), strClientSecret.c_str());
    }

    CMyString strCoinType = req_data["bankCode"].asString();
    CMyString strUserId = req_data["bankName"].asString();
    int64_t nRMBAmount = req_data["fee"].asInt64();  //单位分
    int64_t nCoin = GetRMBTransCoin(nRMBAmount, strCoinType);
    CMyString strAccessToken = this->GetAccessToken(strClientId, strClientSecret);

    data["out_order_id"] = req_data["orderNo"].asString();
    data["coin_type"] = strCoinType;
    data["user_id"] = pSdk->getAccount();  //兑出付款方userid
    CMyString strCallbackUrl = pSdk->getWithdrawCallbackUrl();
    LogWarn("[%s] strCallbackUrl:%s", m_sdkTag.c_str(), strCallbackUrl.c_str());
    data["notify_url"] = strCallbackUrl;
    
    Json::Value item;
    item["type"] = "address";
    item["content"] = this->GetUserReceiveAddress(strAccessToken, strCoinType, strUserId);
    item["amount"] = nCoin;
    to.append(item);
    data["to"] = to;

    product["id"] = req_data["orderNo"].asString();
    product["name"] = req_data["orderNo"].asString();
    data["product"] = product;

    {
        Json::Value redisData;
        redisData["coin_type"] = strCoinType;
        redisData["amount"] = nRMBAmount;
        redisData["coin"] = nCoin;
        redisData["orderNo"] = req_data["orderNo"].asString();
        CJsonWrapper jwRedis(redisData);
        CMyString strRedisValue = jwRedis.toString();
        CMyString strRedisKey(m_strCreditKeyFormat.c_str(), req_data["orderNo"].asString().c_str());
        m_redis.Set(strRedisKey, strRedisValue, m_nTimeout, 1);
        LogMsg("[%s:%s:%d] strRedisKey: %s, strRedisValue:%s", __FILE__, __func__, __LINE__, strRedisKey.c_str(), strRedisValue.c_str());
    }

    CJsonWrapper jw(data);
    CMyString httpData = jw.toString();
    CMyString strUrl = pSdk->getWithdrawUrl();
    CMyString contentType = "Authorization: Bearer ";
    contentType += this->GetAccessToken(strClientId, strClientSecret);

    http::VEC_STRING header_list;
    header_list.push_back("Content-Type: application/json");
    header_list.push_back(contentType);
    CMyString strRespHeader;
    CMyString strResp;
    int64_t httpBegin = Now(true);

    http::CHttpClient net;
    int32_t ret = net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list,  httpData.c_str());
    int32_t cost = Now(true) - httpBegin;
    if (ret != 0)
    {
        net.getError(ret, errNote);  
        LogErr("[%s][%s:%s:%d]credit server failure, url [%s], data [%s], err:%d, msg:%s, cost:%.2lfms",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), httpData.c_str(), ret, errNote.c_str(), cost/1000.0f);
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]credit server failure, url [%s], data [%s], err:%d, msg:%s, cost:%.2lfms", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUrl.c_str(), httpData.c_str(), ret, errNote.c_str(), cost/1000.0f));
        return -3;
    }
    LogDebug("[%s]credit resp, url[%s], postdata[%s], header[%s], resp[%s], cost:%.2lfms", m_sdkTag.c_str(), 
            strUrl.c_str(), httpData.c_str(),strRespHeader.c_str(), strResp.c_str(), cost/1000.0f);

    creditOK = false;
    do
    {
        CJsonWrapper jrRoot;
        if (!jrRoot.Init(strResp))
        {
            LogErr("[%s:%s:%d] resp data [%s] not json string", __FILE__, __func__, __LINE__, strResp.c_str());
            errNote.Format("resp data [%s] not json string", strResp.c_str());
            break;
        }

        int32_t nCode = jrRoot.getInt("code");
        if (nCode != 0)
        {
            LogErr("[%s:%s:%d] resp code is %d", __FILE__, __func__, __LINE__, nCode);
            CMyString strMsg = jrRoot.getString("msg");
            errNote.Format("resp code is %d, msg:%s", nCode, strMsg.c_str());
            break;
        }

        Json::Value data;
        if (!jrRoot.getJsonObject("data", data))
        {
            LogErr("[%s:%s:%d] data is not object", __FILE__, __func__, __LINE__);
            errNote.Format("data is not object");
            break;
        }

        creditOrderNo = data["order_id"].asString();
        creditOK = true;

    } while (false);
    
    return 0;
}















bool CDotWalletPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString params;
    req.getFullParamStr(params);
    LogDebug("[%s]credit callback, header[%s], param[%s], post[%s]",
            m_sdkTag.c_str(), headers.c_str(), params.c_str(), strPost.c_str());


    CMyString str1 = req.getParam("payload");
    CJsonWrapper jrParam;
    if (!jrParam.Init(str1))
    {
        LogErr("[%s]CDotWalletPay::doParseCreditCallback parse json err. payload: %s", m_sdkTag.c_str(), str1.c_str());
        return false;
    }

    orderNo = jrParam.getString("out_order_id");
    creditOrderNo = jrParam.getString("order_id");
    int64_t nCoinFee = (int64_t)(jrParam.getString("amount").asInt64()); //交易金额，单位：聪 satoshi(bsv)或者 gwei （eth)

    CMyString strRedisValue;
    CMyString strRedisKey(m_strCreditKeyFormat.c_str(), orderNo.c_str());
    if (m_redis.Get(strRedisKey, strRedisValue) != 1)
    {
        LogErr("[%s]CDotWalletPay::doParseCreditCallback order err, redis not find, strRedisKey:%s", m_sdkTag.c_str(), strRedisKey.c_str());
        return false;
    }
    LogMsg("[%s:%s:%d] strRedisKey: %s, strRedisValue:%s", __FILE__, __func__, __LINE__, strRedisKey.c_str(), strRedisValue.c_str());
    CJsonWrapper JsRedis;
    if (!JsRedis.Init(strRedisValue))
    {
        LogErr("[%s]CDotWalletPay::doParseCreditCallback redisvalue parse json err. strRedisValue: %s", m_sdkTag.c_str(), strRedisValue.c_str());
        return false;
    }

    int64_t nRedisCoin = JsRedis.getUInt64("coin");
    LogDebug("[%s] [%s:%d] coin range: %u - %u", m_sdkTag.c_str(), __FILE__, __LINE__, uint32_t(nRedisCoin * 0.8), uint32_t(nRedisCoin * 1.2));
    if (nCoinFee < uint32_t(nRedisCoin * 0.8) || nCoinFee > uint32_t(nRedisCoin * 1.2))
    {
        LogErr("[%s][%s:%s:%d] coin error, nCoinFee: %d, order nRedisCoin: %u", m_sdkTag.c_str(), __FILE__, __func__, __LINE__,  nCoinFee, nRedisCoin);
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d] coin error, nCoinFee: %d, order nRedisCoin: %u", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__,  nCoinFee, nRedisCoin));
        return false;
    }
    
    //m_redis.Del(strRedisKey);
    fee = JsRedis.getUInt64("amount");
    realFee = nCoinFee; //数字钱包realFee字段表示实际到账数字货币数。

    CMyString strCoinType = JsRedis.getString("coin_type");
    int64_t nTaxFee = jrParam.getString("fee").asInt64();//交易矿工费，单位：聪 satoshi(bsv)或者 gwei （eth)
    tax = this->GetCoinTransRMB(nTaxFee, strCoinType); 

    creditOK = true;

    LogDebug("[%s]credit callback: orderNo:%s, creditOrderNo:%s, fee:%ld, realFee:%ld, tax:%ld, creditOK:%d",
            m_sdkTag.c_str(), orderNo.c_str(), creditOrderNo.c_str(), fee, realFee, tax, creditOK);
    return true;
}


bool CDotWalletPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("signature");
    CMyString strPayLoad = req.getParam("payload");
    CMyString strPubKey = req.getParam("pubkey");

    std::vector<uint8_t> vchSig;
    ECDSAWrapper::readhexEx(vchSig, strSign1.c_str());
    std::vector<uint8_t> vchPub;
    ECDSAWrapper::readhexEx(vchPub, strPubKey.c_str());

    int bRet = ECDSAWrapper::verify(vchSig, strPayLoad.c_str(), vchPub);
    // 验证签名是否匹配，忽略大小写
    if (bRet == 0)
    {
        LogErr("[%s]credit callback sign mismatch, payload[%s], signature[%s], pubkey[%s]",
                m_sdkTag.c_str(), strPayLoad.c_str(), strSign1.c_str(), strPubKey.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]credit callback sign mismatch, payload[%s], signature[%s], pubkey[%s]", 
                m_sdkTag.c_str(), strPayLoad.c_str(), strSign1.c_str(), strPubKey.c_str()));
        return false;
    }
    
    CJsonWrapper jrParam;
    if (jrParam.Init(strPayLoad))
    {
        CMyString orderNo = jrParam.getString("out_order_id");
        CMyString strRedisValue;
        CMyString strRedisKey(m_strCreditKeyFormat.c_str(), orderNo.c_str());

        if (m_redis.Get(strRedisKey, strRedisValue) == 1)
        {
            m_redis.Del(strRedisKey);
            LogDebug("[%s]del redis key, strRedisKey:%s strRedisValue:%s", m_sdkTag.c_str(), strRedisKey.c_str(), strRedisValue.c_str());
        }
    }

    return true;
}


void CDotWalletPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    Json::Value resp;
    if (bSucceed)
    {
        resp["code"] = 0;
        resp["msg"] = "";
    }
    else
    {
        resp["code"] = errCode;
        resp["msg"] = "";
    }
    
    CJsonWrapper js(resp);
    safe_strcpy(szResp, cbResp, js.toString().c_str());
    cbResp = strlen(szResp);
}


CMyString CDotWalletPay::GetAccessToken(CMyString& strClientId, CMyString& strClientSecret)
{
    CMyString strAccessToken;
    if (m_strAuthUrl.empty())
    {
        LogErr("[%s][%s:%s:%d]GetAccessToken m_strAuthUrl is empty", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        return strAccessToken;
    }
    strAccessToken = m_strAccessToken;
    CMyDateTime tNow;
    if (tNow.time() < m_nExpiresIn && !strAccessToken.empty())
    {
        LogMsg("[%s][%s:%s:%d]GetAccessToken accesstoken not expires. accesstoken:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strAccessToken.c_str());
        return strAccessToken;
    }

    if (strClientId.empty())
    {
        strClientId = m_strClientId;
    }
    if (strClientSecret.empty())
    {
        strClientSecret = m_strClientSecret;
    }

    Json::Value root;
    root["client_id"] = strClientId;
    root["client_secret"] = strClientSecret;
    root["grant_type"] = "client_credentials";

    CJsonWrapper jw(root);
    CMyString httpData = jw.toString();

    http::VEC_STRING header_list;
    CMyString strResp;
    CMyString strRespHeader;
    CMyString strContent = "Content-Type: application/json";
    header_list.push_back(strContent);

    http::CHttpClient net;
    int32_t nRet = net.HttpPostWithHeaders(m_strAuthUrl.c_str(), strResp, strRespHeader, header_list, httpData.c_str());
    if (nRet == 0)
    {
        LogMsg("[%s][%s:%s:%d]url:%s, httpData:%s, strResp:%s, strRespHeader:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strAuthUrl.c_str(), httpData.c_str(),  strResp.c_str(), strRespHeader.c_str());

        do
        {
            CJsonWrapper jrRoot;
            if (!jrRoot.Load(strResp))
            {
                LogErr("[%s][%s:%s:%d]resp data not json string", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            int32_t nCode = jrRoot.getInt("code");
            if (nCode != 0)
            {
                CMyString strMsg = jrRoot.getString("msg");
                LogErr("[%s][%s:%s:%d]code = %d, msg=%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, nCode, strMsg.c_str());
                break;
            }

            Json::Value data;
            if (!jrRoot.getJsonObject("data", data))
            {
                LogErr("[%s][%s:%s:%d]data is not object", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            m_strAccessToken = data["access_token"].asString();
            int64_t nExpires = data["expires_in"].asInt64();
            m_nExpiresIn = tNow.time() + nExpires - 10;
            strAccessToken = m_strAccessToken;

        } while (false);
    }
    else
    {
        CMyString strErrNote;
        net.getError(nRet, strErrNote);  
        LogErr("[%s][%s:%s:%d]url:%s, httpData:%s, strResp:%s, strRespHeader:%s, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strAuthUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str(), nRet, strErrNote.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]url:%s, httpData:%s, strResp:%s, strRespHeader:%s, err:%d, msg:%s", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strAuthUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str(), nRet, strErrNote.c_str()));
    }

    return strAccessToken;
}

CMyString CDotWalletPay::GetUserReceiveAddress(const CMyString& strAccessToken, const CMyString& strCoinType, const CMyString& strUserId)
{
    CMyString strUserAccess;
    if (m_strUserAddressUrl.empty())
    {
        LogErr("[%s][%s:%s:%d]GetUserReceiveAddress m_strUserAddressUrl is empty", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        return strUserAccess;
    }

    Json::Value root;
    root["user_id"] = strUserId;
    root["coin_type"] = strCoinType;

    CJsonWrapper jw(root);
    CMyString httpData = jw.toString();

    http::VEC_STRING header_list;
    CMyString strResp;
    CMyString strRespHeader;
    CMyString contentType = "Content-Type: application/json";
    CMyString strAuthorization = "Authorization: Bearer ";
    strAuthorization += strAccessToken;
    header_list.push_back(contentType);
    header_list.push_back(strAuthorization);

    http::CHttpClient net;
    int32_t nRet = net.HttpPostWithHeaders(m_strUserAddressUrl.c_str(), strResp, strRespHeader, header_list, httpData.c_str());
    if (nRet == 0)
    {
        LogMsg("[%s][%s:%s:%d] url:%s, httpData:%s,  strResp:%s, strRespHeader:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strUserAddressUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str());

        do
        {
            CJsonWrapper jrRoot;
            if (!jrRoot.Load(strResp))
            {
                LogErr("[%s][%s:%s:%d]resp data not json string", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }
            int32_t nCode = jrRoot.getInt("code");
            if (nCode != 0)
            {
                LogErr("[%s][%s:%s:%d]resp code = %d", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, nCode);
                break;
            }

            Json::Value data;
            if(!jrRoot.getJsonObject("data", data))
            {
                LogErr("[%s][%s:%s:%d]not find data object", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }
            strUserAccess = data["primary_wallet"]["address"].asString();

        } while (false);
    }
    else
    {
        CMyString strErrNote;
        net.getError(nRet, strErrNote);  
        LogErr("[%s][%s:%s:%d]url:%s, httpData:%s,  strResp:%s, strRespHeader:%s err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strUserAddressUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str(), nRet, strErrNote.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]url:%s, httpData:%s,  strResp:%s, strRespHeader:%s err:%d, msg:%s", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strUserAddressUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str(), nRet, strErrNote.c_str()));
    }

    return strUserAccess;
}

void CDotWalletPay::GetRate()
{
    if (m_strTransUrl.empty())
    {
        LogErr("[%s][%s:%s:%d]GetRate m_strUserAddressUrl is empty", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        return ;
    }

    CMyString strResp;
    CMyString strRespHeader;
    http::VEC_STRING header_list;
    CMyString strContentType = "Authorization: Bearer ";
    strContentType += GetAccessToken(m_strClientId, m_strClientSecret);
    header_list.push_back(strContentType);

    http::CHttpClient net;
    int32_t nRet = net.HttpGetWithHeaders(m_strTransUrl.c_str(), strResp, strRespHeader, header_list);
    if (nRet == 0)
    {
        LogMsg("[%s][%s:%s:%d] strResp:%s, strRespHeader:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strResp.c_str(), strRespHeader.c_str());

        do
        {
            CJsonWrapper jrRoot;
            if (!jrRoot.Load(strResp))
            {
                LogErr("[%s][%s:%s:%d]resp data not json string", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            int32_t nCode = jrRoot.getInt("code");
            if (nCode != 0)
            {
                LogErr("[%s][%s:%s:%d]code != 0, code :%d", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, nCode);
                break;
            }

            Json::Value data;
            if (!jrRoot.getJsonObject("data", data))
            {
                LogErr("[%s][%s:%s:%d]data not object", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            m_jsRate = jrRoot.getJson();

        } while (false);
    }
    else
    {
        CMyString strErrNote;
        net.getError(nRet, strErrNote);  
        LogErr("[%s][%s:%s:%d] url:%s, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strTransUrl.c_str(), nRet, strErrNote.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d] url:%s, err:%d, msg:%s", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strTransUrl.c_str(), nRet, strErrNote.c_str()));
    }
}

bool CDotWalletPay::GetUserAutoPayBalance(const CMyString& strCoinType, const CMyString& strUserId, int64_t& nBalance)
{
    nBalance = 0;
    if (m_strGetBalanceUrl.empty())
    {
        LogErr("[%s][%s:%s:%d]GetUserAutoPayBalance m_strGetBalanceUrl is empty", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        return false;
    }

    CMyString strResp;
    CMyString strRespHeader;
    http::VEC_STRING header_list;
    CMyString contentType = "Content-Type: application/json";
    CMyString strAuthorization = "Authorization: Bearer ";
    strAuthorization += GetAccessToken(m_strClientId, m_strClientSecret);
    header_list.push_back(contentType);
    header_list.push_back(strAuthorization);

    Json::Value root;
    root["coin_type"] = strCoinType;
    root["user_id"] = strUserId;
    CJsonWrapper jw(root);
    CMyString httpData = jw.toString();

    http::CHttpClient net;
    int32_t nRet = net.HttpPostWithHeaders(m_strGetBalanceUrl.c_str(), strResp, strRespHeader, header_list, httpData.c_str());
    if (nRet == 0)
    {
        LogMsg("[%s][%s:%s:%d] url:%s, httpData:%s, strResp:%s, strRespHeader:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strGetBalanceUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str());

        do
        {
            CJsonWrapper jrRoot;
            if (!jrRoot.Load(strResp))
            {
                LogErr("[%s][%s:%s:%d]resp data not json string", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            int32_t nCode = jrRoot.getInt("code");
            if (nCode != 0)
            {
                LogErr("[%s][%s:%s:%d]code != 0, code :%d", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, nCode);
                break;
            }

            Json::Value data;
            if (!jrRoot.getJsonObject("data", data))
            {
                LogErr("[%s][%s:%s:%d]data not object", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }
            
            //nBalance = data["confirm"].asInt64();
            int64_t a = data["confirm"].asInt64();
            int64_t b = data["unconfirm"].asInt64();
            nBalance = a + b;

            return true;

        } while (false);
    }
    else
    {
        CMyString strErrNote;
        net.getError(nRet, strErrNote);  
        LogErr("[%s][%s:%s:%d] url:%s, httpData:%s, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strGetBalanceUrl.c_str(), httpData.c_str(), nRet, strErrNote.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d] url:%s, httpData:%s, err:%d, msg:%s", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strGetBalanceUrl.c_str(), httpData.c_str(), nRet, strErrNote.c_str()));
    }

    return false;
}

bool CDotWalletPay::GetOrderIsPay(const CMyString& strPlatformOrder)
{
    if (m_strQueryOrderUrl.empty())
    {
        LogErr("[%s][%s:%s:%d]GetOrderIsPay m_strQueryOrderUrl is empty", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        return false;
    }

    CMyString strResp;
    CMyString strRespHeader;
    http::VEC_STRING header_list;
    CMyString contentType = "Content-Type: application/json";
    CMyString strAuthorization = "Authorization: Bearer ";
    strAuthorization += GetAccessToken(m_strClientId, m_strClientSecret);
    header_list.push_back(contentType);
    header_list.push_back(strAuthorization);

    Json::Value root;
    root["order_id"] = strPlatformOrder;
    CJsonWrapper jw(root);
    CMyString httpData = jw.toString();

    http::CHttpClient net;
    int32_t nRet = net.HttpPostWithHeaders(m_strQueryOrderUrl.c_str(), strResp, strRespHeader, header_list, httpData.c_str());
    if (nRet == 0)
    {
        LogMsg("[%s][%s:%s:%d] url:%s, httpData:%s, strResp:%s, strRespHeader:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strQueryOrderUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str());

        do
        {
            CJsonWrapper jrRoot;
            if (!jrRoot.Load(strResp))
            {
                LogErr("[%s][%s:%s:%d]resp data not json string", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            int32_t nCode = jrRoot.getInt("code");
            if (nCode != 0)
            {
                LogErr("[%s][%s:%s:%d]code != 0, code :%d", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, nCode);
                break;
            }

            Json::Value data;
            if (!jrRoot.getJsonObject("data", data))
            {
                LogErr("[%s][%s:%s:%d]data not object", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }
            
            int32_t nStatus = data["status"].asInt();
            if (nStatus == 2)
            {
                return true;
            }

        } while (false);
    }

    return false;
}

int64_t CDotWalletPay::GetRMBTransCoin(int64_t nRMB, const CMyString& strCoinType)
{
    int64_t nCoin = 0;
    if (!m_jsRate.empty())
    {
        LogMsg("[%s][%s:%s:%d] m_jsRate:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_jsRate.toStyledString().c_str());

        Json::Value data = m_jsRate["data"];
        int64_t nCNY = data["CNY"].asInt64();
        if (strCoinType == "BTC")
        {
            int64_t nBTC = data["BTC"].asInt64();
            //nCoin = ceil((nRMB / (nBTC * 100.0f)) * nCNY) * 100000000;

            LogWarn("[%s:%s:%d] nBTC=%ld, nCNY=%ld", __FILE__, __func__, __LINE__, nBTC, nCNY);
            double temp1 = nRMB / (nBTC * 100.0f);
            double temp2 = temp1 * nCNY;
            int64_t temp3 = ceil(temp2 * 100000000.0f);
            LogWarn("[%s:%s:%d] temp1=%lf, temp2=%lf, temp3=%ld", __FILE__, __func__, __LINE__, temp1, temp2, temp3);
            nCoin = temp3;
        }
        else if (strCoinType == "BSV")
        {
            int64_t nBSV = data["BSV"].asInt64();
            //nCoin = ceil(((nCNY * 100) / (nBSV * 1.0f)) * 100000000.0f) * nRMB;

            LogWarn("[%s:%s:%d] nBSV=%ld, nCNY=%ld", __FILE__, __func__, __LINE__, nBSV, nCNY);
            double temp1 = nRMB / (nBSV * 100.0f);
            double temp2 = temp1 * nCNY;
            int64_t temp3 = ceil(temp2 * 100000000.0f);
            LogWarn("[%s:%s:%d] temp1=%lf, temp2=%lf, temp3=%ld", __FILE__, __func__, __LINE__, temp1, temp2, temp3);
            nCoin = temp3;
        }
        else if (strCoinType == "ETH")
        {
            int64_t nETH = data["ETH"].asInt64();
            //nCoin = ceil(((nCNY * 100) / (nETH * 1.0f)) * 1000000000.0f) * nRMB;

            LogWarn("[%s:%s:%d] nBSV=%ld, nCNY=%ld", __FILE__, __func__, __LINE__, nETH, nCNY);
            double temp1 = nRMB / (nETH * 100.0f);
            double temp2 = temp1 * nCNY;
            int64_t temp3 = ceil(temp2 * 1000000000.0f);
            LogWarn("[%s:%s:%d] temp1=%lf, temp2=%lf, temp3=%ld", __FILE__, __func__, __LINE__, temp1, temp2, temp3);
            nCoin = temp3;
        }
    }
    else
    {
        LogErr("[%s][%s:%s:%d] m_jsRate is empty", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
    }

    return nCoin;
}

int64_t CDotWalletPay::GetCoinTransRMB(int64_t nCoin, const CMyString& strCoinType)
{
    int64_t nRMB = 0;
    if (!m_jsRate.empty())
    {
        LogMsg("[%s][%s:%s:%d] m_jsRate:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_jsRate.toStyledString().c_str());

        Json::Value data = m_jsRate["data"];
        int64_t nCNY = data["CNY"].asInt64();
        if (strCoinType == "BTC")
        {
            int64_t nBTC = data["BTC"].asInt64();

            LogWarn("[%s:%s:%d] BTC=%ld, nCNY=%ld", __FILE__, __func__, __LINE__, nBTC, nCNY);
            double temp1 = nBTC / nCNY;
            double temp2 = temp1 / 1000000.0f;
            int64_t temp3 = floor(temp2 * nCoin);
            LogWarn("[%s:%s:%d] temp1=%lf, temp2=%lf, temp3=%ld", __FILE__, __func__, __LINE__, temp1, temp2, temp3);
            nRMB = temp3;
        }
        else if (strCoinType == "BSV")
        {
            int64_t nBSV = data["BSV"].asInt64();

            LogWarn("[%s:%s:%d] nBSV=%ld, nCNY=%ld", __FILE__, __func__, __LINE__, nBSV, nCNY);
            double temp1 = nBSV / nCNY;
            double temp2 = temp1 / 1000000.0f;
            int64_t temp3 = floor(temp2 * nCoin);
            LogWarn("[%s:%s:%d] temp1=%lf, temp2=%lf, temp3=%ld", __FILE__, __func__, __LINE__, temp1, temp2, temp3);
            nRMB = temp3;
        }
        else if (strCoinType == "ETH")
        {
            int64_t nETH = data["ETH"].asInt64();

            LogWarn("[%s:%s:%d] nETH=%ld, nCNY=%ld", __FILE__, __func__, __LINE__, nETH, nCNY);
            double temp1 = nETH / nCNY;
            double temp2 = temp1 / 10000000.0f;
            int64_t temp3 = floor(temp2 * nCoin);
            LogWarn("[%s:%s:%d] temp1=%lf, temp2=%lf, temp3=%ld", __FILE__, __func__, __LINE__, temp1, temp2, temp3);
            nRMB = temp3;
        }
    }
    else
    {
        LogErr("[%s][%s:%s:%d] m_jsRate is empty", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
    }

    return nRMB;
}








int32_t CDotWalletPay::doWithdrawRequest(IClient* pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, STRING_MAP &ext_headers)
{
    CGame* pGame(NULL);
    bool signEnable(false);
    CMyString signField;
    this->getSignInfo("main/paycenter/sign/withdraw", signEnable, signField);
    // STEP1: 检查参数是否合法
    std::vector<CMyString> vecParam;
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    vecParam.push_back("fee");
    vecParam.push_back("gameOrderNo");
    int32_t code = this->valid_req(req, szResp, cbResp, vecParam, 
            signEnable, signField, true, "gameCode", pGame);
    if (code != 0)
    {
        return code;
    }
    
    CMyString gameCode = req.getParam("gameCode");
    CMyString strUin = req.getParam("uin");
    CMyString bankCode = req.getParam("bankCode");
    CMyString bankCard = req.getParam("bankCard");
    CMyString bankName_b64 = req.getParam("bankName");
    // CMyString bankName = CBase64Wrapper::Decode(bankName_b64.c_str(), bankName_b64.length(), false);
    int64_t fee = req.getParamInt64("fee");
    CMyString strIp = req.getParam("ip");
    pClient->setIp(strIp);
    int64_t createTime = req.getParamInt64("createTime");
    CMyString gameOrderNo = req.getParam("gameOrderNo");
    int64_t tax = req.getParamInt64("tax");
    
    Json::Value resp;
    if (bankCode.empty())
    {
        m_pDB->Cmd("SELECT productId FROM `t_payorder` WHERE uin = %s AND `orderStatus` IN(5,8,9,10) AND `gameCode` = '%s' ORDER BY updatedAt DESC LIMIT 1;", strUin.c_str(), gameCode.c_str());
        while(m_pDB->More())
        {
            bankCode = m_pDB->GetString(0);
        }
    }
    if (bankCode.empty())
    {
        resp["code"] = -1;
        resp["message"] = CMyString("The user %s has not recharged yet.", strUin.c_str());
        dealResp(resp, szResp, cbResp);
        LogErr("[%s][%s:%s:%d] The user %s has not recharged yet.", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d] The user %s has not recharged yet.", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strUin.c_str()));
        return -1;
    }

    // STEP2: 查询用户是否符合提现标准
    CMyString strWithdreaId;
    resp["code"] = 0;
    resp["message"] = "OK";
    m_pDB->Cmd("call pWithdrawReq('%s', '%s', '%s', '%s', '%s', %ld, %ld, '%s', %ld, '%s')",
            gameCode.c_str(), strUin.c_str(), bankCode.c_str(), bankCard.c_str(), 
            bankName_b64.c_str(), fee, tax, strIp.c_str(), createTime, gameOrderNo.c_str());
    while(m_pDB->More()) 
    {
        resp["code"] = m_pDB->GetInt(0);
        resp["message"] = m_pDB->GetString(1);

        Json::Value data;
        data["uin"] = strUin;
        data["fee"] = (Json::Int64)fee;
        data["orderNo"] = m_pDB->GetString(2);
        resp["data"] = data;

        strWithdreaId = m_pDB->GetString(2);
    }
    CHECK_MYSQL_ERROR(err); 

    req.setParam("orderNo", strWithdreaId);
    return 0;
}

int32_t CDotWalletPay::doWithdrawAudit(IClient* pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, STRING_MAP &ext_headers)
{
    CMyString gameCode = req.getParam("gameCode");
    CMyString orderNo = req.getParam("orderNo");
    CMyString strUin = req.getParam("uin");
    int32_t status = 4;
    CMyString auditRemark;
    CMyString auditOperator;
    int64_t auditTime = time(NULL);

    // STEP4: 写入到数据库
    int32_t retval(-1);
    Json::Value resp;
    CMyString strMsg = "store to database failure";
    m_pDB->Cmd("call pWithdrawAudit('%s', %d, '%s', %ld, '%s')",
            orderNo.c_str(), status, auditRemark.c_str(), auditTime, auditOperator.c_str());
    while(m_pDB->More())
    {
        retval= m_pDB->GetInt(0);
        strMsg = m_pDB->GetString(1);
    }
    CHECK_MYSQL_ERROR(err);

    if (retval != 0)
    {
        resp["code"] = retval;
        resp["message"] = strMsg;
        dealResp(resp, szResp, cbResp);
        LogErr("[%s][%s:%s:%d] message: %s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strMsg.c_str());
        this->postAlarm(Alarm_General, CMyString("[%s][%s:%s:%d] message: %s", 
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strMsg.c_str()));
        return -1;
    }

    return 0;
}

int32_t CDotWalletPay::doWithdrawCredit(IClient* pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, STRING_MAP &ext_headers)
{
    CMyString gameCode = req.getParam("gameCode");
    CMyString orderNo = req.getParam("orderNo");
    CMyString sdkCode = "DOTWALLET";

    this->postNextCredit(gameCode, sdkCode, orderNo, req, szResp, cbResp);
    return 0; 
}

void CDotWalletPay::getSignInfo(const CMyString &sect, bool &bEnable, CMyString &field)
{
    CXmlFile &xml = CONFIG.getXml();
    bEnable = xml.GetBool(sect.c_str(), "enable");
    field = xml.GetString(sect.c_str(), "field");
}

int32_t CDotWalletPay::valid_req(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::vector<CMyString> &vecParam, bool bSign, const CMyString &signField, bool bSignFullParam, const CMyString &gameCodeField, CGame *&pGame)
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
    pGame = GAME_LIST.FindByCode(gameCode);;
    if (NULL == pGame)
    {
        strMsg.Format("无此游服代码 %s", gameCode.c_str());
        Json::Value resp;
        resp["code"] = PAYFRAME_ERROR(err_InvalidGamesvr);
        resp["message"] = strMsg;
        resp["gameCode"] = gameCode;
        Json::Value data_arr;
        data_arr.resize(0);
        resp["data"] = data_arr;

        dealResp(resp, szResp, cbResp);
        return 200;
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

void CDotWalletPay::postNextCredit(const CMyString &gameCode, const CMyString &sdkCode, const CMyString &orderNo, http::CHttpRequest &req, char *szResp, uint32_t &cbResp)
{
    CMyString creditToken;
    Json::Value resp;
    Json::Value resp_data;
    resp["code"] = -1;
    resp["message"] = "withdraw orderno invalid"; 
    m_pDB->Cmd("call pWithdrawAudit_Query('%s', '%s', '%s')",
            gameCode.c_str(), sdkCode.c_str(), orderNo.c_str());
    while(m_pDB->More())
    {
        CMyString strCgi = m_pDB->GetString(5);
        if (strCgi.empty())
        {
            resp["code"] = -2;
            resp["message"] = CMyString("sdk %s not config withdraw cgi.", sdkCode.c_str());
        }
        else
        {
            resp["code"] = 0;
            resp["message"] = "need deal withdraw audit data.";

            creditToken = m_pDB->GetString(6);
            resp_data["token"] = creditToken;
            resp_data["uri"] = strCgi;
            resp_data["gameCode"] = gameCode;
            resp_data["uin"] = m_pDB->GetString(0);
            resp_data["orderNo"] = orderNo;
            resp_data["bankCode"] = m_pDB->GetString(1);
            resp_data["bankCard"] = m_pDB->GetString(2);
            resp_data["bankName"] = m_pDB->GetString(3);
            int64_t fee = m_pDB->GetInt64(4);
            resp_data["fee"] = (Json::Int64)fee;
            resp_data["creditRemark"] = "";
            resp_data["creditTime"] = time(NULL);
            resp_data["operator"] = "";
        }
    }
    resp["data"] = resp_data;

    // 将TOKEN记录入redis
    m_cacheCreditReq.Push(orderNo, resp_data);
    req.setParam("token", creditToken);
}

void CDotWalletPay::dealDepositRespBack(void)
{
    if (m_qDepositRespBackParam.empty())
    {
        return;
    }

    CMyString strParam = m_qDepositRespBackParam.front();
    LogMsg("[%s] dealDepositRespBack: %s", m_sdkTag.c_str(), strParam.c_str());
    m_qDepositRespBackParam.pop();

    vector<KvstrItem> vecItem;
    safe_strtok(strParam.c_str(), vecItem, "&", "=");

    http::CHttpRequest req;
    for (size_t i = 0; i < vecItem.size(); ++i)
    {
        KvstrItem& item = vecItem[i];
        req.setParam(item.key, item.value);
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
}

void CDotWalletPay::dealUnFinishOrder()
{
    CMyDateTime now;
    vector<CMyString> vecCacheOrder;
    CMyString strKey, strData;
    int32_t nRet = m_cacheQueryOrderData.Pop(strKey, strData);
    while (nRet > 0)
    {
        CJsonWrapper jw;
        if (jw.Load(strData))
        {
            LogDebug("[%s][%s:%s:%d] key:%s Value:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strKey.c_str(), strData.c_str());
            CMyString strOrderNo = jw.getString("orderNo");
            CMyString strPlatformOrder = jw.getString("platformOrderNo");
            int64_t nDealTime = jw.getInt64("deal_time");
            if (now.time() - nDealTime < QueryOrder_Interval)
            {
                vecCacheOrder.push_back(strData);
                strKey.clear();
                strData.clear();
                nRet = m_cacheQueryOrderData.Pop(strKey, strData);
                continue;
            }

            int64_t nValue = 0;
            CMyString strNumKey("%s%s", QueryOrderNumKey, strOrderNo.c_str());
            int32_t ret = m_redis.Get(strNumKey, nValue);
            if (ret > 0)
            {
                if (nValue > 0)
                {
                    LogWarn("[%s][%s:%s:%d] query order %s nValue %d" , m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strOrderNo.c_str(), nValue);
                    //查询
                    bool bPay = this->GetOrderIsPay(strPlatformOrder);
                    if (bPay)
                    {
                        LogWarn("[%s][%s:%s:%d] query order %s has pay", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strOrderNo.c_str());
                        m_redis.Del(strNumKey);
                        //发货
                        this->deliveGoods(strOrderNo, strPlatformOrder);
                    }
                    else
                    {
                        m_redis.DecBy(strNumKey, 1, nValue);
                        if (nValue > 0)
                        {
                            //计数减一，继续轮询
                            //m_cacheQueryOrderData.Push(strData);
                            jw["deal_time"] = now.time();
                            vecCacheOrder.push_back(jw.toString());
                        }
                        else
                        {
                            m_redis.Del(strNumKey);
                        }
                    }
                    break;
                }
                else
                {
                    m_redis.Del(strNumKey);
                }
            }
        }
        
        strKey.clear();
        strData.clear();
        nRet = m_cacheQueryOrderData.Pop(strKey, strData);
    };

    for (size_t i = 0; i < vecCacheOrder.size(); ++i)
    {
        CMyString& strCacheData = vecCacheOrder[i];
        m_cacheQueryOrderData.Push(strCacheData);
    }
}

void CDotWalletPay::deliveGoods(const CMyString& orderNo, const CMyString& platformOrderNo)
{
    CMyString regionCode("N/A");
    CMyString strUin;
    CMyString gameCode;
    CMyString itemInfo;
    CSdkChannel *pChannel(NULL);
    CGame *pGame(NULL);
    CSdk* pSdk(NULL);
    Json::Value req_data;
    int32_t orderId(0);
    int32_t bpId(0);
    int64_t fee(0);         // 充值金额
    int64_t realFee(0);     // 实际到账金额
    int64_t coin(0);
    int64_t ext_coin(0);
    m_errCode = 0;
    m_errNote.clear();
    http::CHttpRequest req;

    do
    {
        // 查询订单数据
        int32_t orderStatus = -1;
        if (!this->fetchDepositOrderData(req, orderNo, req_data, orderStatus))
        {
            m_errCode = err_depositDataLost;
            // 查无此订单号
            break;
        }
        orderId = req_data["payId"].asInt();
        strUin = req_data["uin"].asString();
        bpId = req_data["billpoint"].asInt();
        fee = req_data["fee"].asInt64();
        gameCode = req_data["gameCode"].asString();
        realFee = fee;
        LogWarn("[%s]DELIVER-GOODS>bpId:%d, orderId:%d, uin:%s, fee:%ld, orderNo:%s", 
                m_sdkTag.c_str(), bpId, orderId, strUin.c_str(), fee, orderNo.c_str());

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
            break;
        }

        // 获得充值对应的物品信息
        this->getDepositWealth(pGame, bpId, fee, coin, ext_coin, itemInfo);

        // 更新DB中的状态
        this->updateDepositNotify(gameCode, orderId, deposit_MoneyArrival, 
            platformOrderNo, realFee, regionCode);

        // 不论是否支付成功，都向游服发送通知
        this->judgeBPInfo(pGame, bpId, fee, strUin, req_data, coin, ext_coin, itemInfo);

        //上一步判断完成后再更新计费点使用次数，否则最后一次就无法购买
        {
            BPDATA_LIST.IncBill(strUin, bpId);
            // 通知其他进程刷新数据
            Json::Value data;
            data["uin"] = strUin;
            data["bpId"] = bpId;
            Broadcast2Sibling(sc_increaseBillpoint, data);
        }

        req_data["code"] =  0;
        req_data["realFee"] = (Json::Int64)realFee;
        req_data["coin"] = (Json::Int64)coin;
        req_data["extraCoin"] = (Json::Int64)ext_coin;
        req_data["itemInfo"] = itemInfo;
        this->postDepositResultToGamesvr(pGame, pSdk, req_data);
    } while (false);
}