/*
 * =====================================================================================
 *
 *       Filename:  HuaweiPay.cpp
 *
 *    Description:  华为支付接口实现
 *
 *        Version:  1.0
 *        Created:  2021年04月28日 19时42分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "HuaweiPayEx.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"
#include "SdkList.h"
#include "GameList.h"
#include "SdkChannelList.h"
#include "MyNet.h"
#include "UserCreditList.h"
#include "BillPointList.h"
#include "BillpointDataList.h"
#include "Dict_PayStyle.h" 


CPluginHuaweiPayEx::CPluginHuaweiPayEx()
    : CPayBase("HuaweiPay", depositVerify, httpRequestSync)
{
}

CPluginHuaweiPayEx::~CPluginHuaweiPayEx()
{
}

int32_t CPluginHuaweiPayEx::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CPluginHuaweiPayEx::Update(void)
{
    CMyString strXmlSect = "main/huawei";
    int32_t ret = CPayBase::Init("main/huawei");
    if (ret < 0)
    {
        return ret;
    }

    CXmlFile &xml = CONFIG.getXml();
    CMyString sect("%s/huaweisdk/callback", strXmlSect.c_str());
    if (!CPayBase::InitRedisCache(m_cacheHuaweiOrder, m_redis, xml, sect))
    {
        return -1;
    }

    return 0;
}

void CPluginHuaweiPayEx::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CPluginHuaweiPayEx, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginHuaweiPayEx, doVerify)
{    
    return CPayBase::doDepositVerify(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginHuaweiPayEx, doVerifyV2)
{   
    CMyString strOrderNo = req.getParam("orderNo");
    CMyString platformOrderNo;
    //不调游服发货接口，发货数据直接返回给游服data，游服自己发货
    Json::Value data;  
    int nRet = doCerifyAndDeliverGoods(req, strOrderNo, platformOrderNo, data);
    if (nRet != 0)
    {
        this->doPayFailure(szResp, cbResp);
    }
    else
    {
        Json::Value resp;
        resp["code"] = 0;
        resp["message"] = "OK";
        resp["billno"] = platformOrderNo;
        resp["data"] = data;
        this->OnPayResp(resp, szResp, cbResp);
    }

    return nRet;
}

//充值
void CPluginHuaweiPayEx::doMakeDepositRequest(http::CHttpRequest &req, CGame *pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString &url, CMyString &contentType, CMyString &httpData)
{

}

bool CPluginHuaweiPayEx::doParseDepositResp(CGame *pGame, const CMyString &respHeader, const CMyString &respData, CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

//验证
void CPluginHuaweiPayEx::addDepositVerifyNeededParam(std::vector<CMyString> &vecParam)
{
    vecParam.push_back("productId");
    vecParam.push_back("token");
    vecParam.push_back("fee");
}

void CPluginHuaweiPayEx::storeDepositVerifyData(CGame *pGame, CSdk *pSdk, http::CHttpRequest &req, Json::Value &data)
{
    data["payurl"] = pSdk->getPayUrl();
    data["paycheckurl"] = pSdk->getPayCheckUrl();
    data["productId"] = req.getParam("productId");
    data["token"] = req.getParam("token");

    // 保存到cache中，稍后发送
    int32_t nRet = m_cacheHuaweiOrder.Push(data);
    if (nRet < 0)
    {
        LogErr("[%s] SaveData error[%s]", m_sdkTag.c_str(), m_redis.getLastError());
        this->postAlarm(Alarm_General, CMyString("[%s] SaveData error[%s]", 
                m_sdkTag.c_str(), m_redis.getLastError()));
    }
    else
    {
        LogMsg("[%s] SaveData success", m_sdkTag.c_str());
    }
}

int32_t CPluginHuaweiPayEx::verifyDepositRequest(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode)
{ 
    CMyString strKey, strData;
    int32_t nRet = 0;
    int32_t ret = m_cacheHuaweiOrder.Pop(strKey, strData);
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
        bool bRet = validHuaWeiDepositSign(req_data, depositOK, platformOrderNo, realFee, regionCode);
        if (bRet)
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

int32_t CPluginHuaweiPayEx::doCerifyAndDeliverGoods(http::CHttpRequest &req, const CMyString& strOrderNo, CMyString& billNo, Json::Value& data)
{
    // TODO: 查询IP对应的regionCode
    CMyString regionCode("N/A");

    CMyString strUin;
    CMyString gameCode;
    CMyString itemInfo;
    CGame *pGame(NULL);
    CSdk* pSdk(NULL);
    bool depositedOK(false);
    Json::Value req_data;
    int32_t orderId(0);
    int32_t bpId(0);
    int64_t fee(0);         // 充值金额
    int64_t realFee(0);     // 实际到账金额
    int64_t coin(0);
    int64_t ext_coin(0);
    m_errCode = 0;
    m_errNote.clear();
    do {
        // 查询订单数据
        if (!this->fetchDepositOrderData(strOrderNo, req_data))
        {
            if (m_errCode == 0)
            {
                m_errCode = err_depositDataLost;
            }
            // 查无此订单号
            break;
        }
        orderId = req_data["payId"].asInt();
        strUin = req_data["uin"].asString();
        bpId = req_data["billpoint"].asInt();
        fee = req_data["fee"].asInt64();
        gameCode = req_data["gameCode"].asString();
        LogWarn("[%s]DELIVER-GOODS>bpId:%d, orderId:%d, uin:%s, fee:%ld, orderNo:%s", 
                m_sdkTag.c_str(), bpId, orderId, strUin.c_str(), fee, strOrderNo.c_str());

        // 查询SDK数据
        pGame = GAME_LIST.FindByCode(gameCode);
        if (NULL == pGame)
        {
            m_errCode = err_depositParameterFault;
            m_errNote.Format("[%s]invalid gameCode %s when deposit callback.",
                m_sdkTag.c_str(), gameCode.c_str());
            LogFatal("[%s]invalid gameCode %s when deposit order %s callback",
                m_sdkTag.c_str(), gameCode.c_str(), strOrderNo.c_str());
            break;
        }
        pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
        if (NULL == pSdk)
        {
            m_errCode = err_depositParameterFault;
            m_errNote.Format("[%s]invalid sdk %s (gameCode %s) when deposit callback.",
                m_sdkTag.c_str(), m_sdkCode.c_str(), gameCode.c_str());
            LogFatal("[%s]invalid sdk config when deposit order %s callback, gameCode:%s, sdkCode:%s",
                m_sdkTag.c_str(), strOrderNo.c_str(), gameCode.c_str(), m_sdkCode.c_str());
            break;
        }
    }while(0);

    if (m_errCode != 0)
    {
        return m_errCode;
    }

    Json::Value valid_data = req_data;
    valid_data["payurl"] = pSdk->getPayUrl();
    valid_data["paycheckurl"] = pSdk->getPayCheckUrl();
    valid_data["productId"] = req.getParam("productId");
    valid_data["token"] = req.getParam("token");
    bool bRet = validHuaWeiDepositSign(valid_data, depositedOK, billNo, realFee, regionCode);
    if (!bRet)
    {
        LogErr("[%s]deposit order verify failure", m_sdkTag.c_str());
        return -1;
    }

    depositedOK = true;

    // 获得充值对应的物品信息
    this->getDepositWealth(pGame, bpId, fee, coin, ext_coin, itemInfo);

    // 更新DB中的状态
    this->updateDepositNotify(gameCode, orderId, (depositedOK ? deposit_MoneyArrival : deposit_MoneyFail), 
        billNo, realFee, regionCode);

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

    data = req_data;

    // 记录发货状态 
    this->finishDepositOrder(gameCode, orderId, deposit_DeliverSucceed, bpId, strUin);

    return 0;
}

CMyString CPluginHuaweiPayEx::getAppAT(const CMyString &gameCode, int32_t channelId, bool bForce)
{
    if (bForce)
    {
        m_mapGameAppAT.erase(gameCode);
    }

    MAP_GAME_APPAT::iterator it(m_mapGameAppAT.find(gameCode));
    if (it == m_mapGameAppAT.end())
    {
        int32_t ret = this->buildAppAT(gameCode, channelId);
        if (ret != 0)
        {
            return "";
        }
    }

    it = m_mapGameAppAT.find(gameCode);
    if (it == m_mapGameAppAT.end())
    {
        return "";
    }

    return it->second;
}

int32_t CPluginHuaweiPayEx::buildAppAT(const CMyString &gameCode, int32_t channelId)
{
    static CMyString s_tokenUrl = "https://oauth-login.cloud.huawei.com/oauth2/v3/token";

    int32_t result(-1);
    CSdk *pSdk = SDK_LIST.FindByCode(gameCode, m_sdkCode);
    if (NULL == pSdk)
    {
        LogFatal("[%s] not found sdk for gameCode:%s, sdkCode:%s",m_sdkTag.c_str(), gameCode.c_str(), m_sdkCode.c_str());
        return result;
    }

    CSdkChannel* pChannel = SDKCHANNEL_LIST.Find(channelId);
    if (NULL == pChannel)
    {
        LogFatal("[%s]channelId %d not avaiable in system.", m_sdkTag.c_str(), channelId);
        return result;
    }

    CMyString strAccount, strAppKey;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strAccount = extraData["account"].asString();
        strAppKey = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strAppKey: %s", __FILE__, __func__, __LINE__, strAppKey.c_str());
    }
    if (strAppKey.empty())
    {
        strAppKey = pSdk->getApiKey();
    }
    if (strAccount.empty())
    {
        strAccount = pSdk->getAccount();
    }

    CMyString grantType("client_credentials");
    CMyString msgBody("grant_type=%s&client_secret=%s&client_id=%s",
            grantType.c_str(), strAppKey.c_str(), 
            strAccount.c_str());
    CMyString respMsg;
    CMyString respHeader;
    http::VEC_STRING header_list;
    header_list.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
    do
    {
        http::CHttpClient net;
        int32_t ret = net.HttpPostWithHeaders(s_tokenUrl.c_str(), respMsg, respHeader, header_list, msgBody.c_str());
        if (ret == 0)
        {
            // 处理成功
            LogDebug("[%s] resp from huawei oauth2, header[%s], data[%s]",m_sdkTag.c_str(), respHeader.c_str(), respMsg.c_str());
            CJsonWrapper jr;
            if (!jr.Init(respMsg))
            {
                LogFatal("[%s] huawei oauth2 resp %s not json string",m_sdkTag.c_str(), respMsg.c_str());
                result = -101;
                break;
            }
            result = 0;
            CMyString token = jr.getString("access_token");
            CMyString str1("APPAT:%s", token.c_str());
            CMyString str2 = CBase64Wrapper::Encode(str1.c_str(), str1.length(), false);
            CMyString str3("Basic %s", str2.c_str());
            m_mapGameAppAT[gameCode] = str3;
            LogDebug("[%s] gameCode:%s, accessToken:%s, appAT:%s",m_sdkTag.c_str(), gameCode.c_str(), token.c_str(), str3.c_str());
        }
        else
        {
            CMyString strErr;
            if (ret > 0)
            {
                net.getError(ret, strErr);
            }
            else
            {
                strErr = "CHttpClient error.";
            }
            LogErr("[%s] request appAT from huawei oauth2 failure, url:%s, result:%d, msg:%s",m_sdkTag.c_str(), s_tokenUrl.c_str(), ret, strErr.c_str());
            result = -103;
        }
    }while(0);

    return 0;
}

bool CPluginHuaweiPayEx::validHuaWeiDepositSign(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode)
{
    CMyString gameCode = req_data["gameCode"].asString();
    CMyString huaweiToken = req_data["token"].asString();
    CMyString productId = req_data["productId"].asString();
    int32_t channelId = req_data["channelId"].asInt();
    CMyString appAt = getAppAT(gameCode, channelId, true);
    if (appAt.empty())
    {
        m_errCode = -1;
        m_errNote = "build huawei appAT failure.";
        LogErr("[%s]getAppAT fail  error:%s", m_sdkTag.c_str(), m_errNote.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]getAppAT fail  error:%s", 
                m_sdkTag.c_str(), m_errNote.c_str()));
        return false;
    }

    Json::Value huaweiReq;
    huaweiReq["purchaseToken"] = huaweiToken;
    huaweiReq["productId"] = productId;
    CJsonWrapper wr;
    CMyString reqData = wr.write(huaweiReq);

    //Order服务验证购买token
    //https://developer.huawei.com/consumer/cn/doc/development/HMSCore-References-V5/api-order-verify-purchase-token-0000001050746113-V5
    CMyString strUrl = req_data["payurl"].asString();
    CMyString respMsg;
    CMyString respHeader;
    http::VEC_STRING header_list;
    header_list.push_back("Content-Type: application/json; charset=UTF-8");
    CMyString strAuth("Authorization: %s", appAt.c_str());
    header_list.push_back(strAuth);
    int32_t result(-1);
    CJsonWrapper jr;
    do {
        http::CHttpClient net;
        int ret = net.HttpPostWithHeaders(strUrl.c_str(), respMsg, respHeader, header_list, reqData.c_str());       
        if (ret == 0)
        {
            LogDebug("[%s]resp from huawei verify, header[%s], data:[%s]",m_sdkTag.c_str(), respHeader.c_str(), respMsg.c_str());
            if (!jr.Init(respMsg))
            {
                LogFatal("[%s]huawei verify resp %s not json string",m_sdkTag.c_str(), respMsg.c_str());
                result = -101;
                break;
            }
            result = 0;
        }
        else
        {
            if (ret > 0)
            {
                net.getError(ret, m_errNote);
            }
            else
            {
                m_errNote = "CHttpClient error.";
            }
            LogErr("[%s]request huawei verify order failure, url:%s, result:%d, msg:%s", m_sdkTag.c_str(), strUrl.c_str(), ret, m_errNote.c_str());
            this->postAlarm(Alarm_General, CMyString("[%s]request huawei verify order failure, url:%s, result:%d, msg:%s", 
                m_sdkTag.c_str(), strUrl.c_str(), ret, m_errNote.c_str()));
            result = -103;
        }

    }while(0);

    if (result != 0)
    {
        // 出错，返回错误信息
        m_errCode = result;
        LogErr("[%s] request huawei verify order failure error:%s",m_sdkTag.c_str(), m_errNote.c_str());
        return false;
    }

    CMyString strCode = jr.getString("responseCode");
    CMyString strMsg = jr.getString("responseMessage");
    CMyString sign = jr.getString("dataSignature");
    CMyString huaweiOrderNo;

    CMyString strTokenData = jr.getString("purchaseTokenData");
    CJsonWrapper jrToken;
    if (jrToken.Init(strTokenData))
    {
        huaweiOrderNo = jrToken.getString("orderId");
        regionCode = jrToken.getString("country");
        realFee = jrToken.getInt("price");  //商品实际价格*100以后的值。商品实际价格精确到小数点后2位，例如此参数值为501，则表示商品实际价格为5.01。
    }
    platformOrderNo = huaweiOrderNo;

    if (strCode != "0")
    {
        LogErr("[%s] request huawei verify order get data error: huaweiOrderNo:%s,  error:%s", m_sdkTag.c_str(), huaweiOrderNo.c_str(),  strMsg.c_str());
        return false;
    }
    
    // Order服务确认购买
    //https://developer.huawei.com/consumer/cn/doc/development/HMSCore-References-V5/api-purchase-confirm-for-order-service-0000001051066054-V5
    CMyString strUrl2 = req_data["paycheckurl"].asString();
    do {
        http::CHttpClient net;
        respMsg.clear();
        int ret = net.HttpPostWithHeaders(strUrl2.c_str(), respMsg, respHeader, header_list, reqData.c_str());
        if (ret == 0)
        {
            LogDebug("[%s]resp from huawei confirm, header[%s], data:[%s]", m_sdkTag.c_str(), respHeader.c_str(), respMsg.c_str());
            if (!jr.Init(respMsg))
            {
                LogFatal("[%s]huawei confirm resp %s not json string", m_sdkTag.c_str(), respMsg.c_str());
                result = -201;
                break;
            }
            result = 0;
        }
        else
        {
            if (ret > 0)
            {
                net.getError(ret, m_errNote);
            }
            else
            {
                m_errNote = "CHttpClient error.";
            }
            LogErr("[%s] request huawei confirm order failure, url:%s, result:%d, msg:%s", m_sdkTag.c_str(), strUrl2.c_str(), ret, m_errNote.c_str());
            this->postAlarm(Alarm_General, CMyString("[%s] request huawei confirm order failure, url:%s, result:%d, msg:%s", 
                m_sdkTag.c_str(), strUrl2.c_str(), ret, m_errNote.c_str()));
            result = -203;
        }
    }while(0);
    
    if (result < 0)
    {
        m_errCode = result;
        LogErr("[%s] request huawei confirm order failure: huaweiOrderNo:%s, uin:%s, error:%s", m_sdkTag.c_str(), huaweiOrderNo.c_str(), strMsg.c_str());
        return false;

    }
    strCode = jr.getString("responseCode");
    strMsg = jr.getString("responseMessage");
    if (strCode != "0")
    {
        m_errCode = -1;
        m_errNote = strMsg;
        LogErr("[%s] huawei confirm fail  huaweiOrderNo:%s, error:%s", m_sdkTag.c_str(), huaweiOrderNo.c_str(), strMsg.c_str());
        this->postAlarm(Alarm_General, CMyString("[%s] huawei confirm fail  huaweiOrderNo:%s, error:%s", 
                m_sdkTag.c_str(), huaweiOrderNo.c_str(), strMsg.c_str()));
        return false;
    }

    depositOK = true;
    return true;
}

//提现
void CPluginHuaweiPayEx::doMakeCreditRequest(Json::Value &req, CGame *pGame, CSdk *pSdk, CMyString &url, CMyString &contentType, CMyString &httpData)
{
   
}

bool CPluginHuaweiPayEx::doParseCreditResp(CGame *pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return false;
}

bool CPluginHuaweiPayEx::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CPluginHuaweiPayEx::validCreditCallbackSign(http::CHttpRequest &req, CGame *pGame, CSdk *pSdk, CMyString &errNote)
{
    return true;
}

void CPluginHuaweiPayEx::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t &cbResp)
{
 
}

bool CPluginHuaweiPayEx::fetchDepositOrderData(const CMyString& orderNo, Json::Value& req_data)
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
    case deposit_DeliverSucceed:        // 5
        m_errNote.Format("orderNo %s has paid, can not deliver goods again.", orderNo.c_str());      
        m_errCode = err_depositDeliverGoodsSuccess;
        break;
    case deposit_MoneyArrival:          // 1
    case deposit_DeliverHostFail:     // 8
    case deposit_DeliverFail:           // 9
    case deposit_DeliverManual:         // 10
        m_errNote.Format("orderNo %s has paid, can not deliver goods again.", orderNo.c_str());      
        m_errCode = err_depositDeliverGoodsFail;
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

void CPluginHuaweiPayEx::OnPayResp(Json::Value &resp, char *szResp, uint32_t& cbResp)
{
    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

void CPluginHuaweiPayEx::doPayFailure(char* szResp, uint32_t& cbResp)
{
    Json::Value resp;
    resp["code"] = m_errCode;
    resp["message"] = m_errNote;
    this->OnPayResp(resp, szResp, cbResp);
}

bool CPluginHuaweiPayEx::finishDepositOrder(const CMyString &gameCode, int32_t orderId, int32_t result, int32_t bpId, const CMyString &strUin)
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

void CPluginHuaweiPayEx::getDepositWealth(CGame *pGame, int32_t bpId, int64_t fee, int64_t& coin, int64_t& ext_coin, CMyString& itemInfo)
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

void CPluginHuaweiPayEx::StrReplace(CMyString &str, const char *from, const char* to)
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

void CPluginHuaweiPayEx::judgeBPInfo(CGame *pGame, const int32_t& bpId, const int64_t& fee, const CMyString& strUin, Json::Value& req_data, int64_t& coin, int64_t& ext_coin, CMyString& itemInfo)
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

bool CPluginHuaweiPayEx::updateDepositNotify(const CMyString &gameCode, int32_t orderId, int32_t result, const CMyString &platformOrderNo, int64_t realFee, const CMyString &regionCode)
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