/*
 * =====================================================================================
 *
 *       Filename:  GMPay.cpp
 *
 *    Description:  客服放款
 *
 *        Version:  1.0
 *        Created:  2021年07月06日 17时40分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "GMPay.h"
#include "Config.h"
#include "SdkList.h"
#include "SdkChannelList.h"
#include "ChannelList.h"
#include "Common.h"
#include "Dict_PayStyle.h"
#include "UserCreditList.h"

CPluginGMPay::CPluginGMPay()
    : CPayBase("Gmpay", depositRequest, httpRequestSync)
{

}

CPluginGMPay::~CPluginGMPay()
{

}

int32_t CPluginGMPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CPluginGMPay::Update(void)
{
    CMyString strXml("main/qgmpay");
    int32_t ret = CPayBase::Init(strXml);
    return ret;
}

void CPluginGMPay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CPluginGMPay, doDeposit)
{   
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
            LogFatal("[%s]CSdkChannel object maybe corrupted when user %s request deposit.",
                m_sdkTag.c_str(), strUin.c_str());
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

    // 构建返回信息给游服
    this->doPayResp(pChannel, platformUrl, orderNo, szResp, cbResp);

    LogMsg("[%s]request deposit succeed, gameCode:%s, uin:%s, orderNo:%s",
            m_sdkTag.c_str(), gameCode.c_str(), strUin.c_str(), orderNo.c_str());
    return 0;
}

ENTRY_FUNC_DECLARE(CPluginGMPay, doDepositCB)
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
            LogFatal("[%s]call doParseDepositCallback failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(std::exception &ex)
        {
            LogFatal("[%s]call doParseDepositCallback failure, msg:%s", m_sdkTag.c_str(), ex.what());
            val = false;
        }
        catch(...)
        {
            LogFatal("[%s]call doParseDepositCallback failure, unknown error", m_sdkTag.c_str());
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
        if (!this->fetchDepositOrderData(req, orderNo, req_data))
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
            LogFatal("[%s]invalid gameCode %s when deposit order %s callback",
                m_sdkTag.c_str(), gameCode.c_str(), orderNo.c_str());
            break;
        }
        pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
        if (NULL == pSdk)
        {
            m_errCode = err_depositParameterFault;
            m_errNote.Format("[%s]invalid sdk %s (gameCode %s) when deposit callback.",
                m_sdkTag.c_str(), m_sdkCode.c_str(), gameCode.c_str());
            LogFatal("[%s]invalid sdk config when deposit order %s callback, gameCode:%s, sdkCode:%s",
                m_sdkTag.c_str(), orderNo.c_str(), gameCode.c_str(), m_sdkCode.c_str());
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

        //客服充值没有计费点，直接按比例获取金币
        // 获得充值对应的物品信息
        // 未找到计费点，按照兑换比例获得金币
        int32_t exchangeRate = pGame->getExchangeRate();
        coin = fee * exchangeRate / 100;
        ext_coin = 0;
        LogMsg("[%s]deposit callback not have billpoint, exchange_rate:%d, fee:%ld, coin:%ld", m_sdkTag.c_str(), exchangeRate, fee, coin);

        // 更新DB中的状态
        this->updateDepositNotify(gameCode, orderId, (depositedOK ? deposit_MoneyArrival : deposit_MoneyFail), 
            platformOrderNo, realFee, regionCode);

        //收到钱，更新用户总充值额
        if (depositedOK)
        {
            bool b = USERCREDIT_LIST.AddDeposit(m_pDB, gameCode, strUin, realFee);
            LogMsg("[%s] %s add deposit %ld, result: %s", m_sdkTag.c_str(), strUin.c_str(), realFee, b ? "true" : "false");
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

void CPluginGMPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    CMyString strIp = req.getParam("playerIp");
    // 金额转换成元
    int64_t payMoney = req.getParamInt64("amount");
    double money = payMoney / 100.0;

    CMyString uin = req.getParam("uin");
    CMyString productId = req.getParam("productId");
    
    // 时区校正
    CXmlFile &xml = CONFIG.getXml();
    int32_t adjust = xml.GetLong("main/paycenter/timezone", "adjust");
    int32_t nReqTime = time(NULL) + adjust; 

    CPayChannel *pPayChannel = pChannel->getPayChannel();

    // 构建发往支付SDK请求的URL
    http::CHttpRespMaker pay_req;
    pay_req.add("merchant", pSdk->getAccount());            // 商户号
    pay_req.add("uin", uin);                                // 玩家ID
    pay_req.add("userIp", strIp);                           // 玩家IP
    pay_req.add("productId", productId);                    // 产品ID
    pay_req.add("paystyle", pPayChannel->getPayStyle());    // 支付类型
    pay_req.add("bankcode", pChannel->getBankCode());       // 银行码
    // 转成 xx.xx 的格式
    CMyString reqMoney("%.2lf", money);
    pay_req.add("money", reqMoney);                         // 金额
    pay_req.add("tradeNo", orderNo);                     // 订单号
    pay_req.add("notify_url", pSdk->getPayCallbackUrl());                // 回调URL
    pay_req.add("return_url", pGame->getCallbackUrl());              // 支付结束返回URL
    pay_req.add("reqtime", nReqTime);                       // 请求时间

    CMyString str1 = pay_req.http_str_manual('&', '=', true, true, false);
    CMyString str2("%s&%s", str1.c_str(), pSdk->getApiKey().c_str());
    CMyString strMD5 = CMD5Wrapper::encode(str2.c_str());
    strMD5.MakeLower();
    pay_req.add("sign", strMD5);

    httpData = pay_req.http_str_manual('&', '=', false, true, true);
    url = pSdk->getPayUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}

bool CPluginGMPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    LogDebug("[%s]deposit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());

     CJsonWrapper jrRoot;
    if (!jrRoot.Load(respData))
    {
        errNote.Format("resp data not json string");
        return false;
    }

    int32_t nCode = jrRoot.getInt("code");
    if (nCode != 0)
    {
        errNote = jrRoot.getString("m");
        return false;
    }

    Json::Value data;
    if (!jrRoot.getJsonObject("data", data))
    {
        errNote.Format("resp data d not json string");
        return false;
    }

    depositUrlData = data["url"].asString();
    depositOrderNo = data["orderNo"].asString();
    LogDebug("[%s] doParseDepositResp : depositUrlData[%s]",  m_sdkTag.c_str(), depositUrlData.c_str());
    
    return true;
}

bool CPluginGMPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
{
    orderNo = req.getParam("tradeNo");
    platformOrderNo = req.getParam("orderNo");
    CMyString money = req.getParam("money");
    double realMoney = money.empty() ? 0 : (money.asDouble() * 100);
    fee = realMoney;  
    realFee = fee;
    tax = 0;
    depositedOK = true;

    LogDebug("[%s]pay param: orderNo:%s, platformOrderNo:%s, fee:%ld", m_sdkTag.c_str(), orderNo.c_str(), platformOrderNo.c_str(), fee);

    return true;
}

bool CPluginGMPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel* pChannel, CMyString& errNote)
{
    return true;
}

void CPluginGMPay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    safe_strcpy(szResp, cbResp, "ok");
    cbResp = strlen(szResp);
}

void CPluginGMPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CPluginGMPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CPluginGMPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CPluginGMPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CPluginGMPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}

bool CPluginGMPay::createDepositOrder(http::CHttpRequest& req, CSdk* pSdk, CGame* pGame, int32_t& orderId, CMyString& orderNo)
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
        LogFatal("[%s]save deposit order into mysql failure, user:%s, err:%d, msg:%s",
            m_sdkTag.c_str(), strUin.c_str(), err, szError);
        return false;
    }
    if (code < 0 || orderNo.empty())
    {
        m_errCode = err_depositCreateOrderFailure;
        m_errNote = strMsg;
        LogFatal("[%s]save deposit order into mysql failure, user:%s, unknown error.",
            m_sdkTag.c_str(), strUin.c_str());
        return false;
    }

    return true;
}

bool CPluginGMPay::createDepositRequest(http::CHttpRequest &req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, 
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
        LogFatal("[%s]call doMakeDepositRequest failure, msg:%s", m_sdkTag.c_str(), ex.what());
        return false;
    }
    catch(std::exception &ex)
    {
        LogFatal("[%s]call doMakeDepositRequest failure, msg:%s", m_sdkTag.c_str(), ex.what());
        return false;
    }
    catch(...)
    {
        LogFatal("[%s]call doMakeDepositRequest failure", m_sdkTag.c_str());
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

//    int64_t fee = req.getParamInt("amount");
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
            LogFatal("[%s]user %s deposit fail, orderNo:%s, url:%s, err:%d, msg:%s, cost:%.2lfms",
                m_sdkTag.c_str(), strUin.c_str(), orderNo.c_str(), strUrl.c_str(), ret, m_errNote.c_str(), cost/1000.0);
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
            LogErr("[%s]user %s deposit fail, orderNo:%s, resp header [%s], resp data [%s], error:%s, cost:%.2lfms",
                m_sdkTag.c_str(), strUin.c_str(), orderNo.c_str(), strRespHeader.c_str(), strResp.c_str(), m_errNote.c_str(), cost/1000.0);
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

bool CPluginGMPay::updateDepositOrder(const CMyString &gameCode, int32_t orderId, int32_t result, const CMyString &platformOrderNo)
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
        LogFatal("[%s]update deposit order failure, result:%d, err:%s, msg:%s",
            m_sdkTag.c_str(), result, err, szError);
        return false;
    }
    if (code < 0)
    {
        m_errCode = err_depositOrderUpdateFailure;
        m_errNote = strMsg;
        LogFatal("[%s]update deposit order failure, result:%d, msg:%s",
            m_sdkTag.c_str(), result, strMsg.c_str());
        return false;
    }
    return true;
}

void CPluginGMPay::doPayResp(CSdkChannel *pChannel, const CMyString& strUrl, const CMyString &orderNo, char* szResp, uint32_t& cbResp)
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

void CPluginGMPay::OnPayResp(Json::Value &resp, char *szResp, uint32_t& cbResp)
{
    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

bool CPluginGMPay::fetchDepositOrderData(http::CHttpRequest& req, const CMyString& orderNo, Json::Value& req_data)
{
    bool bFind = false;
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
        LogFatal("[%s]query deposit order %s failure, err:%s, msg:%s",
            m_sdkTag.c_str(), orderNo.c_str(), err, szError);
        return false;
    }
    if (!bFind)
    {
        m_errCode = err_depositOrderNotFound;
        m_errNote.Format("deposit order %s not found.", orderNo.c_str());
        LogFatal("[%s]deposit order %s not found.", m_sdkTag.c_str(), orderNo.c_str());
        return false;
    }

    int32_t orderStatus = req_data["status"].asInt();
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

bool CPluginGMPay::validDepositCallbackIp(CSdk *pSdk, const CMyString& strIp)
{
    if (!pSdk->IsRechargeIpValid(strIp))
    {
        m_errNote.Format("deposit callback ip %s invalid.", strIp.c_str());
        LogWarn("[%s]deposit callback ip %s invalid", m_sdkTag.c_str(), strIp.c_str());
        return false;
    }

    return true;
}

bool CPluginGMPay::updateDepositNotify(const CMyString &gameCode, int32_t orderId, int32_t result, const CMyString &platformOrderNo, int64_t realFee, const CMyString &regionCode)
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
        LogFatal("[%s]update deposit order failure, orderId:%d, result=>%d, err:%d, msg:%s",
            m_sdkTag.c_str(), orderId, result, err, szError);
        return false;
    }
    if (code < 0)
    {
        m_errCode = err_depositOrderUpdateFailure;
        m_errNote = strMsg;
        LogFatal("[%s]update deposit order failure, orderId:%d, result=>%d, msg:%s",
            m_sdkTag.c_str(), orderId, result, strMsg.c_str());
        return false;
    }
    return true;
}

bool CPluginGMPay::postDepositResultToGamesvr(CGame *pGame, CSdk* pSdk, Json::Value &req_data)
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
    enum GMPayDepositStatus depositResult;
    bool bNeedReDeliver(false);
    bool bSucceed = this->deliverGoods(pGame->getCode(), strUrl, strRespDataWithSign, depositResult, bNeedReDeliver);

    // 记录发货状态 
    this->finishDepositOrder(pGame->getCode(), orderId, depositResult, bpId, strUin);

    return bSucceed;
}

bool CPluginGMPay::deliverGoods(const CMyString &gameCode, const CMyString &strUrl, const CMyString &strData, enum GMPayDepositStatus &result, bool &bNeedReDeliver)
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
            net.getError(m_errCode, m_errNote);
            result = deposit_DeliverHostFail;
            m_errCode = err_depositDeliverHostFail; 
            LogFatal("[%s]deliver goods to server [%s] failure, data[%s], err:%d, msg:%s",
                m_sdkTag.c_str(), strUrl.c_str(), strData.c_str(), ret, m_errNote.c_str());
            bNeedReDeliver = true;
            break;
        }

        LogDebug("[%s]deliver goods resp data[%s], request data[%s]", 
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

void CPluginGMPay::doPayFailure(const CMyString &gameCode, const CMyString &strUin, const CMyString &orderNo, int64_t fee, char* szResp, uint32_t& cbResp)
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

bool CPluginGMPay::finishDepositOrder(const CMyString &gameCode, int32_t orderId, int32_t result, int32_t bpId, const CMyString &strUin)
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
        LogFatal("[%s]update deposit order failure, orderId:%d, result=>%d, err:%d, msg:%s",
            m_sdkTag.c_str(), orderId, result, err, szError);
        return false;
    }
    if (code < 0)
    {
        m_errCode = err_depositOrderUpdateFailure;
        m_errNote = strMsg;
        LogFatal("[%s]update deposit order failure, orderId:%d, result=>%d, msg:%s",
            m_sdkTag.c_str(), orderId, result, strMsg.c_str());
        return false;
    }
    return true;
}