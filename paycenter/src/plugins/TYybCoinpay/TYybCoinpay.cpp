/*
 * =====================================================================================
 *
 *       Filename:  TYybCoinpay.cpp
 *
 *    Description:  应用宝游戏币模式
 *
 *        Version:  1.0
 *        Created:  2021年07月19日 16时27分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "TYybCoinpay.h"
#include "Config.h"
#include "Security.h"
#include "SdkList.h"
#include "GameList.h"
#include "SdkChannelList.h"
#include "MyNet.h"
#include "UserCreditList.h"
#include "BillPointList.h"
#include "BillpointDataList.h"
#include "Common.h"
#include "Dict_PayStyle.h" 


string escapeURL(const string &URL);

CPluginTCPay::CPluginTCPay()
    : CPayBase("TCPay", depositRequest, httpRequestSync)
{

}

CPluginTCPay::~CPluginTCPay()
{

}

int32_t CPluginTCPay::Init(void)
{
    int32_t ret = this->Update();

    return ret;
}

int32_t CPluginTCPay::Update(void)
{ 
    CMyString strXmlSect = "main/tcpay";
    int32_t ret = CPayBase::Init(strXmlSect);
    if (ret < 0)
    {
        return ret;
    }

    CXmlFile &xml = CONFIG.getXml();
    CMyString sect("%s/verify", strXmlSect.c_str());
    if (!CPayBase::InitRedisCache(m_cacheOrder, m_redis, xml, sect))
    {
        return -1;
    }

    const CMyString strApp("%s/banlance", strXmlSect.c_str());
    xml.getString(strApp, "host", m_strGetBanlanceHost);
    xml.getString(strApp, "url", m_strGetBanlanceUrl);
    
    const CMyString strCoin("%s/consume", strXmlSect.c_str());
    xml.getString(strCoin, "host", m_strConsumeCoinHost);
    xml.getString(strCoin, "url", m_strConsumeCoinUrl);
    
    const CMyString strCancel("%s/cancelpay", strXmlSect.c_str());
    xml.getString(strCancel, "host", m_strCancelPayHost);
    xml.getString(strCancel, "url", m_strCancelPayUrl);

     // 装载各个参数配置
    CMyString sect1("main/paycenter/order");
    CMyString strDelivers;
    if (xml.getString(sect1, "re-deliver", strDelivers) != 0)
    {
        strDelivers = "3,5,10,15,30,60,90";
    }

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

    // 充值发货数据，保存到缓存，用于定时重发
    CMyString sect2("%s/deposit/redeliver", strXmlSect.c_str());
    if (!this->InitRedisCache(m_cacheDeliverGoods, m_redis, xml, sect2))
    {
        return -111;
    }

    CMyString sect3("%s/tcpaysdk/callback", strXmlSect.c_str());
    if (!CPayBase::InitRedisCache(m_cacheYybConsumeOrder, m_redis, xml, sect3))
    {
        return -112;
    }

    // 启动定时器列表
    m_timerDeliverGoods.SetInterval(1000);
    m_timerDeliverGoods.Startup();
    m_timerOrder.SetInterval(3000);  // 每3秒检查
    m_timerOrder.Startup();

    return ret;
}

void CPluginTCPay::Maintance(void)
{
    CPayBase::Maintance();

    //轮询订单，防止掉单 5分钟，每分钟一次
    this->dealUnFinishOrder();

    // 订单发货失败，重新发货
    this->dealReDeliver();
}

ENTRY_FUNC_DECLARE(CPluginTCPay, doDeposit)
{ 
    // 检查是否合法的paycenter请求
    int32_t channelId(0);
    CSdkChannel *pChannel(NULL);
    m_errCode = 0;
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

    }while(false);

    if (m_errCode != 0)
    {
        this->doPayFailure(szResp, cbResp);
        return m_errCode;
    }

    CMyString strAppId, strAppKey;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strAppId = extraData["appid"].asString();
        strAppKey = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strAppKey: %s", __FILE__, __func__, __LINE__, strAppKey.c_str());
    }

    CMyString strExtData = req.getParam("extData");
    this->StrReplace(strExtData, "\\\"", "\"");
    CJsonWrapper json;
    if (!json.Init(strExtData.c_str()))
    {
        LogErr("[%s][%s:%d] parse extData error",m_sdkTag.c_str(),  __FILE__, __LINE__);  
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%d] parse extData error", 
                m_sdkTag.c_str(),  __FILE__, __LINE__));

        m_errCode = -1;
        m_errNote.Format("parse extData error");
        this->doPayFailure(szResp, cbResp);
        return m_errCode;
    }

    CMyString strOpenId = json.getString("openid");
    CMyString strOpenKey = json.getString("openkey");
    CMyString strPf = json.getString("pf");
    CMyString strPfKey = json.getString("pf_key");
    int32_t nPlatForm = json.getInt("platform");

    bool bGetBalance = false;
    for (int32_t i = 0; i < 3; ++i)
    {
        int32_t amt = 0;
        int32_t nRet = getBalance(strOpenId, strOpenKey, strPf, strPfKey, strAppId, strAppKey, nPlatForm, amt);
        if (nRet == 0)
        {
            bGetBalance = true;
            break;
        }
        else
        {
            m_errCode = nRet;
            m_errNote.Format("get Balance error");
        }
    }

    if (bGetBalance)
    {
        return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
    }
    else
    {
        LogErr("[%s][%s:%d]doGetBanlance 3 times failed, ret=[%d]",m_sdkTag.c_str(),  __FILE__, __LINE__, m_errCode);
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%d]doGetBanlance 3 times failed, ret=[%d]", 
                m_sdkTag.c_str(),  __FILE__, __LINE__, m_errCode));
    }

    this->doPayFailure(szResp, cbResp);
    return m_errCode;
}

ENTRY_FUNC_DECLARE(CPluginTCPay, doConsumeCoin)
{
    return CPayBase::doDepositVerify(pClient, req, szResp, cbResp, ext_headers);

    // int32_t amount = req.getParamInt("amount") / 10.0;
    // CMyString strOrderNo = req.getParam("orderNo");
    // CMyString strOpenId = req.getParam("openid");
    // CMyString strOpenKey = req.getParam("openkey");
    // CMyString strPf = req.getParam("pf");
    // CMyString strPfKey = req.getParam("pf_key");
    // int32_t nPlatForm = req.getParamInt("platform");
    // CMyString strUin = req.getParam("uin");

    // if (strOrderNo.empty() || strOpenId.empty() || strOpenKey.empty() || strPf.empty() || strPfKey.empty())
    // {
    //     LogErrAlt("[%s:%d]do parse deposit callback paramater error", __FILE__, __LINE__);
    //     m_errCode = err_depositParameterFault;
    //     m_errNote = "do parse deposit callback paramater error";
    //     this->doPayFailure(szResp, cbResp);
    //     return m_errCode;
    // }

    // CMyString platformOrderNo;
    // //consume里面不调游服发货接口，发货数据直接返回给游服data，游服自己发货
    // Json::Value data;
    // int nRet = doConsumeAndDeliverGoodsEx(strOrderNo, strOpenId, strOpenKey, strPf, strPfKey, strUin, nPlatForm, amount, platformOrderNo, data);
    // if (nRet != 0)
    // {
    //     this->doPayFailure(szResp, cbResp);
    // }
    // else
    // {
    //     //成功扣款后删除redis，停止轮询
    //     CMyString strRealKey, strData;
    //     m_cacheOrder.Pop(strOrderNo, strRealKey, strData);

    //     Json::Value resp;
    //     resp["code"] = 0;
    //     resp["message"] = "OK";
    //     resp["billno"] = platformOrderNo;
    //     resp["data"] = data;
    //     this->OnPayResp(resp, szResp, cbResp);
    // }

    // return nRet;
}

//https://wiki.open.qq.com/index.php?title=%E7%B1%B3%E5%A4%A7%E5%B8%88%E6%9C%8D%E5%8A%A1%E7%AB%AF%E6%8E%A5%E5%85%A5
void CPluginTCPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    url = "";
    httpData = "";
    contentType = "application/x-www-form-urlencoded";

    CMyString strAppId, strAppKey;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strAppId = extraData["appid"].asString();
        strAppKey = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strAppKey: %s", __FILE__, __func__, __LINE__, strAppKey.c_str());
    }
    if (strAppKey.empty())
    {
        strAppKey = pSdk->getApiKey();
    }

    CMyString strExtData = req.getParam("extData");
    this->StrReplace(strExtData, "\\\"", "\"");
    CJsonWrapper json;
    if (!json.Init(strExtData.c_str()))
    {
        LogErr("[%s][%s:%d] parse extData error",m_sdkTag.c_str(),  __FILE__, __LINE__);  
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%d] parse extData error", 
                m_sdkTag.c_str(),  __FILE__, __LINE__));
        return;
    }

    CMyString strOpenId = json.getString("openid");
    CMyString strOpenKey = json.getString("openkey");
    CMyString strPf = json.getString("pf");
    CMyString strPfKey = json.getString("pf_key");
    int32_t nPlatForm = json.getInt("platform");

    int32_t amt = 0;
    int32_t nRet = getBalance(strOpenId, strOpenKey, strPf, strPfKey, strAppId, strAppKey, nPlatForm, amt);
    if (nRet == 0)
    {
        Json::Value data;
        data["uin"] = req.getParam("uin");
        data["openid"] = strOpenId;
        data["openkey"] = strOpenKey;
        data["pf"] = strPf;
        data["pf_key"] = strPfKey;
        data["platform"] = nPlatForm;
        data["orderno"] = orderNo;
        data["amount"] = req.getParamInt64("amount") / 100; //游戏币以元为单位
        data["save_amt"] = amt;

        CJsonWrapper wr(data);
        CMyString strData = wr.toString();
        LogDebug("[%s][%s:%d]strData: %s", m_sdkTag.c_str(), __FILE__, __LINE__, strData.c_str());

        // 存入到redis, 用于回调之用
        m_cacheOrder.Push(orderNo, strData);
        if (nRet < 0)
        {
            LogErr("[%s] SaveData error[%s]", m_sdkTag.c_str(), m_redis.getLastError());
        }
        else
        {
            LogMsg("[%s] SaveData success", m_sdkTag.c_str());
        }
    }
}

bool CPluginTCPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

//验证
void CPluginTCPay::addDepositVerifyNeededParam(std::vector<CMyString> &vecParam)
{
    vecParam.push_back("amount");
    vecParam.push_back("openid");
    vecParam.push_back("openkey");
    vecParam.push_back("pf");
    vecParam.push_back("pf_key");
    vecParam.push_back("platform");
}

void CPluginTCPay::storeDepositVerifyData(CGame *pGame, CSdk *pSdk, http::CHttpRequest &req, Json::Value &data)
{
    data["amount"] = req.getParamInt("amount") / 10.0;
    data["openid"] = req.getParam("openid");
    data["openkey"] = req.getParam("openkey");
    data["pf"] = req.getParam("pf");
    data["pf_key"] = req.getParam("pf_key");
    data["platform"] = req.getParamInt("platform");

    // 保存到cache中，稍后发送
    int32_t nRet = m_cacheYybConsumeOrder.Push(data);
    if (nRet < 0)
    {
        LogErr("[%s] SaveData error[%s]", m_sdkTag.c_str(), m_redis.getLastError());
    }
    else
    {
        LogMsg("[%s] SaveData success", m_sdkTag.c_str());
    }
}

int32_t CPluginTCPay::verifyDepositRequest(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode)
{ 
    CMyString strKey, strData;
    int32_t nRet = 0;
    int32_t ret = m_cacheYybConsumeOrder.Pop(strKey, strData);
    if (ret > 0)
    {
        LogMsg("[%s]getIncrData ret:[%d] strKey:[%s] strData:[%s]", m_sdkTag.c_str(), ret, strKey.c_str(), strData.c_str());
        CJsonWrapper jr;
        if (!jr.parse(strData))
        {
            LogDebug("[%s] dealCallBack parse fail", m_sdkTag.c_str());
            return 0;
        }
        Json::Value &root = jr.json();
        CSdkChannel *pChannel(NULL);

        CMyString strOpenId = jr.getString("openid");
        CMyString strOpenKey = jr.getString("openkey");
        CMyString strPf = jr.getString("pf");
        CMyString strPfKey = jr.getString("pf_key");
        int32_t nPlatForm = jr.getInt("platform");
        CMyString strOrderNo = jr.getString("orderNo");
        int32_t amount = jr.getInt64("amount");

        int32_t channelId = root["channelId"].asInt();
        pChannel = SDKCHANNEL_LIST.Find(channelId);
        if (NULL == pChannel)
        {
            m_errCode = err_depositParameterFault;
            m_errNote.Format("invalid sdk-channel %d", channelId);
            LogWarn("[%s]channelId %s not avaiable in system.", m_sdkTag.c_str(), channelId);
            return 0;
        }
        CMyString strAppId, strAppKey;
        Json::Value extraData = pChannel->getKeyData();
        if (!extraData.empty())
        {
            strAppId = extraData["appid"].asString();
            strAppKey = extraData["appkey"].asString();
            LogMsg("[%s][%s:%s:%d] strAppId：%s strAppKey: %s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strAppId.c_str(), strAppKey.c_str());
        }
        
        req_data = root;
        ret = consumeCoin(strOpenId, strOpenKey, strPf, strPfKey, strAppId, strAppKey, nPlatForm, amount, platformOrderNo);
        if (ret != 0)
        {
            nRet = -1;
            depositOK = false;
            LogMsg("[%s][%s:%s:%d] consumeCoin fail", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        }
        else
        {    
            //成功扣款后删除redis，停止轮询
            CMyString strRealKey, strDataTemp;
            m_cacheOrder.Pop(strOrderNo, strRealKey, strDataTemp);

            nRet = 1;
            realFee = amount * 100;
            depositOK = true;
            LogMsg("[%s][%s:%s:%d] consumeCoin success", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        }
    }

    return nRet;
}



void CPluginTCPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData) 
{

}


bool CPluginTCPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return false;
}


bool CPluginTCPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}


bool CPluginTCPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}


void CPluginTCPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    
}

char dec2hexChar(short int n)
{
    if (0 <= n && n <= 9)
    {
        return char(short('0') + n);
    }
    else if (10 <= n && n <= 15)
    {
        return char(short('A') + n - 10);
    }
    else
    {
        return char(0);
    }
}

string escapeURL(const string &URL)
{
    string result = "";
    for (unsigned int i = 0; i < URL.size(); i++)
    {
        char c = URL[i];
        if (
            ('0' <= c && c <= '9') ||
            ('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            c == '-' || c == '.' || c == '_')
        {
            result += c;
        }
        else
        {
            int j = (short int)c;
            if (j < 0)
            {
                j += 256;
            }
            int i1, i0;
            i1 = j / 16;
            i0 = j - i1 * 16;
            result += '%';
            result += dec2hexChar(i1);
            result += dec2hexChar(i0);
        }
    }
    return result;
}

const string Base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
string EncodeWithOut0A(const char * cRawData, unsigned int iLength) 
{
  string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (iLength--) {
    char_array_3[i++] = *(cRawData++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; (i < 4); i++)
        ret += Base64Chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += Base64Chars[char_array_4[j]];

    while ((i++ < 3))
      ret += '=';
  }

  return ret;
}

string HmacWithSha1(const char * key, const char * input)
{
  uint32_t output_length = 0;

  const EVP_MD * engine = NULL;
  engine = EVP_sha1();

  unsigned char *buffer = (unsigned char*)malloc(EVP_MAX_MD_SIZE);
  memset(buffer, 0, EVP_MAX_MD_SIZE);

  HMAC_CTX ctx;
  HMAC_CTX_init(&ctx);
  HMAC_Init_ex(&ctx, key, strlen(key), engine, NULL);
  HMAC_Update(&ctx, (unsigned char*)input, strlen(input));
  HMAC_Final(&ctx, buffer, &output_length);
  HMAC_CTX_cleanup(&ctx);

  string output = string((const char*)buffer, output_length);
  free(buffer);

  return output;
}

CMyString CPluginTCPay::GetSign(const CMyString& strAppKey, const CMyString& strParam, const CMyString& strUrl)
{
    CMyString strEncodeUrl = escapeURL(strUrl);
    CMyString strEncodeParam = escapeURL(strParam);

    CMyString strSignData("GET&%s&%s", strEncodeUrl.c_str(), strEncodeParam.c_str());
    CMyString strKey = strAppKey + "&";

    LogDebug("[%s] sign key: %s data：%s", m_sdkTag.c_str(), strKey.c_str(), strSignData.c_str());

    string str = HmacWithSha1(strKey.c_str(), strSignData.c_str());
    string strSign = EncodeWithOut0A(str.c_str(), str.length());

    LogDebug("[%s] sign sha1: %s Encode: %s", m_sdkTag.c_str(), str.c_str(), strSign.c_str());
    
    return strSign;
}

int32_t CPluginTCPay::doConsumeAndDeliverGoods(const CMyString& strOrderNo, const CMyString& strOpenId, const CMyString& strOpenKey, const CMyString& strPf, 
                                                const CMyString& strPfKey, const CMyString& strUserId, int32_t nPlatform, int64_t amount, CMyString& billNo, int64_t save_amt)
{
    // TODO: 查询IP对应的regionCode
    CMyString regionCode("N/A");

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

        if (strUin != strUserId)
        {
            m_errCode = err_depositDataLost;
            m_errNote.Format("orderNo %s is initiated by user:%s, but now user:%s.", strOrderNo.c_str(), strUin.c_str(), strUserId.c_str());
            LogMsg("[%s][%s:%d]orderNo %s is initiated by user:%s, but now user:%s.", m_sdkTag.c_str(), __FILE__, __LINE__, strOrderNo.c_str(), strUin.c_str(), strUserId.c_str());
            break;
        }

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

    CMyString strAppId, strAppKey;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strAppId = extraData["appid"].asString();
        strAppKey = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strAppKey: %s", __FILE__, __func__, __LINE__, strAppKey.c_str());
    }
    if (strAppId.empty())
    {
        Json::Value extraData = pSdk->getPayExtraData();
        if (!extraData.empty())
        {
            strAppId = extraData["appid"].asString();
        }
    }
    if (strAppKey.empty())
    {
        strAppKey = pSdk->getApiKey();
    }

    //轮询时需要检查账户余额
    int32_t saveAmt = 0;
    int32_t nRet = getBalance(strOpenId, strOpenKey, strPf, strPfKey, strAppId, strAppKey, nPlatform, saveAmt);
    if (nRet == 0)
    {
        if (saveAmt == save_amt || saveAmt != (save_amt + amount))
        {
            LogWarn("[%s:%d] query balance, balance=[%d] == save_amt=[%d], balance error, req amount=[%d]", __FILE__, __LINE__, saveAmt, save_amt, amount);
            return -1;
        }
    }
    else
    {
        return nRet;
    }

    nRet = consumeCoin(strOpenId, strOpenKey, strPf, strPfKey, strAppId, strAppKey, nPlatform, amount, billNo);
    if (nRet != 0)
    {
        return nRet;
    }
    depositedOK = true;
    realFee = amount * 100; //支付中心用分为单位

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
    this->judgeBPInfo(pGame, bpId, fee, strUin, req_data, coin, ext_coin, itemInfo, true);

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

    // 发货失败，有重发机制，不取消
    // if (!bSuccess)
    // {
    //     cancelPayCoin(strOpenId, strOpenKey, strPf, strPfKey, strAppId, pSdk->getApiKey(), nPlatform, billNo);
    // }

    return 0;
}

int32_t  CPluginTCPay::doConsumeAndDeliverGoodsEx(const CMyString& strOrderNo, const CMyString& strOpenId, const CMyString& strOpenKey, const CMyString& strPf, const CMyString& strPfKey, 
                                        const CMyString& strUserId, int32_t nPlatform, int64_t amount, CMyString& billNo, Json::Value& data)
{
     // TODO: 查询IP对应的regionCode
    CMyString regionCode("N/A");

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

        if (strUin != strUserId)
        {
            m_errCode = err_depositDataLost;
            m_errNote.Format("orderNo %s is initiated by user:%s, but now user:%s.", strOrderNo.c_str(), strUin.c_str(), strUserId.c_str());
            LogMsg("[%s][%s:%d]orderNo %s is initiated by user:%s, but now user:%s.", m_sdkTag.c_str(), __FILE__, __LINE__, strOrderNo.c_str(), strUin.c_str(), strUserId.c_str());
            break;
        }

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

    CMyString strAppId, strAppKey;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strAppId = extraData["appid"].asString();
        strAppKey = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strAppKey: %s", __FILE__, __func__, __LINE__, strAppKey.c_str());
    }
    if (strAppId.empty())
    {
        Json::Value extraData = pSdk->getPayExtraData();
        if (!extraData.empty())
        {
            strAppId = extraData["appid"].asString();
        }
    }
    if (strAppKey.empty())
    {
        strAppKey = pSdk->getApiKey();
    }

    int32_t nRet = consumeCoin(strOpenId, strOpenKey, strPf, strPfKey, strAppId, strAppKey, nPlatform, amount, billNo);
    if (nRet != 0)
    {
        return nRet;
    }
    depositedOK = true;
    realFee = amount * 100; //支付中心用分为单位

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
    this->judgeBPInfo(pGame, bpId, fee, strUin, req_data, coin, ext_coin, itemInfo, false);

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

int32_t CPluginTCPay::getBalance(const CMyString& strOpenId, const CMyString& strOpenKey, const CMyString& strPf, const CMyString& strPfKey, const CMyString& strAppId, const CMyString& strAppKey, int32_t nPlatform, int32_t& amt)
{
    Json::Value data;
    data["openid"] = strOpenId;
    data["openkey"] = strOpenKey;
    data["appid"] = strAppId;
    data["ts"] = (Json::Int64)time(NULL);
    data["pf"] = strPf;
    data["pfkey"] = strPfKey;
    data["zoneid"] = 1;

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('&', '=', false, true, false);
    LogDebug("[%s] getBalance sign param: %s ", m_sdkTag.c_str(), str1.c_str());

    CMyString strSign = GetSign(strAppKey, str1, m_strGetBanlanceUrl);
    CMyString httpData = req_maker.http_str_manual('&', '=', false, false, true);
    httpData.Replace("*", "%2A");
    httpData += "&sig=";
    httpData += escapeURL(strSign);

    CMyString strUrl = m_strGetBanlanceHost + m_strGetBanlanceUrl;
    
    CMyString sessionid, sessiontype;
    if (nPlatform == 1)
    {
        sessionid = "openid";
        sessiontype = "kp_actoken";
    }
    else if (nPlatform == 2)
    {
        sessionid = "hy_gameid";
        sessiontype = "wc_actoken";
    }
    else if (nPlatform == 3)
    {
        sessionid = "openid";
        sessiontype = "openkey";
    }

    CMyString strOrgloc, strSessionId, strSessionType;
    strSessionId = sessionid; 
    strSessionType = sessiontype;
    strOrgloc = m_strGetBanlanceUrl;
    CMyString strCookie("session_id=%s;session_type=%s;org_loc=%s", strSessionId.c_str(), strSessionType.c_str(), strOrgloc.c_str());
    LogDebug("[%s] getBalance cookie[%s]", m_sdkTag.c_str(), strCookie.c_str());

    CMyString strFullUrl = strUrl + "?" + httpData;
    LogDebug("[%s] getBalance fullurl[%s]", m_sdkTag.c_str(), strFullUrl.c_str());

    int32_t nRet = 0;
    Json::Value resp;
    do
    {
        http::VEC_STRING header_list;
        header_list.push_back("Content-Type: application/x-www-form-urlencoded");
        CMyString strResp;
        CMyString strRespHeader;
        CMyString strErr;
        http::CHttpClient net;
        nRet = net.HttpGetWithHeaderCookie(strFullUrl.c_str(), strResp, strRespHeader, header_list, strCookie.c_str());
        if (nRet != 0)
        {
            net.getError(nRet, strErr);
            LogFatal("[%s]doGetBanlance fail,  url:%s, err:%d, msg:%s",
                     m_sdkTag.c_str(), strFullUrl.c_str(), nRet, strErr.c_str());
            break;
        }
        else
        {
            LogDebug("[%s]doGetBanlance resp, header[%s], data[%s]", m_sdkTag.c_str(), strRespHeader.c_str(), strResp.c_str());
        }

        //解析transdata
        CJsonWrapper json;
        if (!json.Init(strResp.c_str()))
        {
            LogErr("[%s][%s:%d]doGetBanlance parse tencent_yyb transdata error",m_sdkTag.c_str(),  __FILE__, __LINE__);  
            nRet = -4;
            break;
        }

        bool val = json.Read("ret", nRet);
        if (!val)
        {
            LogErr("[%s][%s:%d]doGetBanlance failed, ret field not exist", m_sdkTag.c_str(), __FILE__, __LINE__);
            nRet = -5;
            break;
        }

        if (nRet != 0)
        {
            LogErr("[%s][%s:%d]doGetBanlance failed, ret=[%d]",m_sdkTag.c_str(),  __FILE__, __LINE__, nRet);
            break;
        }

        int32_t save_amt = 0;
        val = json.Read("save_amt", save_amt);
        if (!val)
        {
            LogErr("[%s][%s:%d]doGetBanlance failed, save_amt field not exist",m_sdkTag.c_str(),  __FILE__, __LINE__);
            nRet = -7;
            break;
        }

        amt = save_amt;

    } while (false);

    return nRet;
}

int32_t CPluginTCPay::consumeCoin(const CMyString& strOpenId, const CMyString& strOpenKey, const CMyString& strPf, const CMyString& strPfKey, const CMyString& strAppId, const CMyString& strAppKey, int32_t nPlatform, int64_t amount, CMyString& billNo)
{
    Json::Value data;
    data["openid"] = strOpenId;
    data["openkey"] = strOpenKey;
    data["appid"] = strAppId;
    data["ts"] = (Json::Int64)time(NULL);
    data["pf"] = strPf;
    data["pfkey"] = strPfKey;
    data["zoneid"] = 1;
    data["amt"] = amount;

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('&', '=', false, true, false);
    LogDebug("[%s] consumeCoin sign param: %s ", m_sdkTag.c_str(), str1.c_str());

    CMyString strSign = GetSign(strAppKey, str1, m_strConsumeCoinUrl);
    CMyString httpData = req_maker.http_str_manual('&', '=', false, false, true);
    httpData.Replace("*", "%2A");
    httpData += "&sig=";
    httpData += escapeURL(strSign);

    CMyString strUrl = m_strConsumeCoinHost + m_strConsumeCoinUrl;
    
    CMyString sessionid, sessiontype;
    if (nPlatform == 1)
    {
        sessionid = "openid";
        sessiontype = "kp_actoken";
    }
    else if (nPlatform == 2)
    {
        sessionid = "hy_gameid";
        sessiontype = "wc_actoken";
    }
    else if (nPlatform == 3)
    {
        sessionid = "openid";
        sessiontype = "openkey";
    }

    CMyString strOrgloc, strSessionId, strSessionType;
    strSessionId = sessionid; 
    strSessionType = sessiontype;
    strOrgloc = m_strConsumeCoinUrl;
    CMyString strCookie("session_id=%s;session_type=%s;org_loc=%s", strSessionId.c_str(), strSessionType.c_str(), strOrgloc.c_str());
    LogDebug("[%s] consumeCoin cookie[%s]", m_sdkTag.c_str(), strCookie.c_str());

    CMyString strFullUrl = strUrl + "?" + httpData;
    LogDebug("[%s] consumeCoin fullurl[%s]", m_sdkTag.c_str(), strFullUrl.c_str());

    m_errCode = 0;
    m_errNote.clear();
    Json::Value resp;
    do
    {
        http::VEC_STRING header_list;
        header_list.push_back("Content-Type: application/x-www-form-urlencoded");
        CMyString strResp;
        CMyString strRespHeader;
        http::CHttpClient net;
        m_errCode = net.HttpGetWithHeaderCookie(strFullUrl.c_str(), strResp, strRespHeader, header_list, strCookie.c_str());
        if (m_errCode != 0)
        {
            net.getError(m_errCode, m_errNote);
            LogFatal("[%s]consumeCoin fail,  url:%s, err:%d, msg:%s",
                     m_sdkTag.c_str(), strFullUrl.c_str(), m_errCode, m_errNote.c_str());
            this->postAlarm(Alarm_Fatal, CMyString("[%s]consumeCoin fail,  url:%s, err:%d, msg:%s", 
                    m_sdkTag.c_str(), strFullUrl.c_str(), m_errCode, m_errNote.c_str()));
            break;
        }
        else
        {
            LogDebug("[%s]consumeCoin resp, header[%s], data[%s]", m_sdkTag.c_str(), strRespHeader.c_str(), strResp.c_str());
        }

        //解析transdata
        CJsonWrapper json;
        if (!json.Init(strResp.c_str()))
        {
            LogErr("[%s][%s:%d]consumeCoin parse tencent_yyb transdata error",m_sdkTag.c_str(),  __FILE__, __LINE__);  
            m_errCode = -4;
            break;
        }

        bool val = json.Read("ret", m_errCode);
        if (!val)
        {
            LogErr("[%s][%s:%d]consumeCoin failed, ret field not exist", m_sdkTag.c_str(), __FILE__, __LINE__);
            m_errCode = -5;
            break;
        }

        if (m_errCode != 0)
        {
            LogErr("[%s][%s:%d]consumeCoin failed, ret=[%d]",m_sdkTag.c_str(),  __FILE__, __LINE__, m_errCode);
            m_errNote = json.getString("msg");
            break;
        }

        val = json.Read("billno", billNo);
        if (!val)
        {
            LogErr("[%s][%s:%d]consumeCoin failed, billno field not exist",m_sdkTag.c_str(),  __FILE__, __LINE__);
            m_errCode = -7;
            break;
        }

    } while (false);

    return m_errCode;
}

int32_t CPluginTCPay::cancelPayCoin(const CMyString& strOpenId, const CMyString& strOpenKey, const CMyString& strPf, const CMyString& strPfKey, const CMyString& strAppId, const CMyString& strAppKey, int32_t nPlatform, CMyString& billNo)
{
    Json::Value data;
    data["openid"] = strOpenId;
    data["openkey"] = strOpenKey;
    data["appid"] = strAppId;
    data["ts"] = (Json::Int64)time(NULL);
    data["pf"] = strPf;
    data["pfkey"] = strPfKey;
    data["zoneid"] = 1;
    data["billno"] = billNo;

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('&', '=', false, true, false);
    LogDebug("[%s] cancelPayCoin sign param: %s ", m_sdkTag.c_str(), str1.c_str());

    CMyString strSign = GetSign(strAppKey, str1, m_strCancelPayUrl);
    CMyString httpData = req_maker.http_str_manual('&', '=', false, false, true);
    httpData.Replace("*", "%2A");
    httpData += "&sig=";
    httpData += escapeURL(strSign);

    CMyString strUrl = m_strCancelPayHost + m_strCancelPayUrl;
    
    CMyString sessionid, sessiontype;
    if (nPlatform == 1)
    {
        sessionid = "openid";
        sessiontype = "kp_actoken";
    }
    else if (nPlatform == 2)
    {
        sessionid = "hy_gameid";
        sessiontype = "wc_actoken";
    }
    else if (nPlatform == 3)
    {
        sessionid = "openid";
        sessiontype = "openkey";
    }

    CMyString strOrgloc, strSessionId, strSessionType;
    strSessionId = sessionid; 
    strSessionType = sessiontype;
    strOrgloc = m_strCancelPayUrl;
    CMyString strCookie("session_id=%s;session_type=%s;org_loc=%s", strSessionId.c_str(), strSessionType.c_str(), strOrgloc.c_str());
    LogDebug("[%s] cancelPayCoin cookie[%s]", m_sdkTag.c_str(), strCookie.c_str());

    CMyString strFullUrl = strUrl + "?" + httpData;
    LogDebug("[%s] cancelPayCoin fullurl[%s]", m_sdkTag.c_str(), strFullUrl.c_str());

    m_errCode = 0;
    m_errNote.clear();
    Json::Value resp;
    do
    {
        http::VEC_STRING header_list;
        header_list.push_back("Content-Type: application/x-www-form-urlencoded");
        CMyString strResp;
        CMyString strRespHeader;
        http::CHttpClient net;
        m_errCode = net.HttpGetWithHeaderCookie(strFullUrl.c_str(), strResp, strRespHeader, header_list, strCookie.c_str());
        if (m_errCode != 0)
        {
            net.getError(m_errCode, m_errNote);
            LogFatal("[%s]cancelPayCoin fail,  url:%s, err:%d, msg:%s",
                     m_sdkTag.c_str(), strFullUrl.c_str(), m_errCode, m_errNote.c_str());
            this->postAlarm(Alarm_Fatal, CMyString("[%s]cancelPayCoin fail,  url:%s, err:%d, msg:%s", 
                    m_sdkTag.c_str(), strFullUrl.c_str(), m_errCode, m_errNote.c_str()));
            break;
        }
        else
        {
            LogDebug("[%s]cancelPayCoin resp, header[%s], data[%s]", m_sdkTag.c_str(), strRespHeader.c_str(), strResp.c_str());
        }

        //解析transdata
        CJsonWrapper json;
        if (!json.Init(strResp.c_str()))
        {
            LogErr("[%s][%s:%d]cancelPayCoin parse tencent_yyb transdata error",m_sdkTag.c_str(),  __FILE__, __LINE__);  
            m_errCode = -4;
            break;
        }

        bool val = json.Read("ret", m_errCode);
        if (!val)
        {
            LogErr("[%s][%s:%d]cancelPayCoin failed, ret field not exist", m_sdkTag.c_str(), __FILE__, __LINE__);
            m_errCode = -5;
            break;
        }

        if (m_errCode != 0)
        {
            LogErr("[%s][%s:%d]cancelPayCoin failed, ret=[%d]",m_sdkTag.c_str(),  __FILE__, __LINE__, m_errCode);
            m_errNote = json.getString("msg");
            break;
        }

    } while (false);

    return m_errCode;
}

void CPluginTCPay::OnPayResp(Json::Value &resp, char *szResp, uint32_t& cbResp)
{
    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

bool CPluginTCPay::fetchDepositOrderData(const CMyString& orderNo, Json::Value& req_data)
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
        this->postAlarm(Alarm_Fatal, CMyString("[%s]query deposit order %s failure, err:%s, msg:%s", 
                    m_sdkTag.c_str(), orderNo.c_str(), err, szError));
        return false;
    }
    if (!bFind)
    {
        m_errCode = err_depositOrderNotFound;
        m_errNote.Format("deposit order %s not found.", orderNo.c_str());
        LogFatal("[%s]deposit order %s not found.", m_sdkTag.c_str(), orderNo.c_str());
        this->postAlarm(Alarm_Fatal, CMyString("[%s]deposit order %s not found.", 
                    m_sdkTag.c_str(), orderNo.c_str()));
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

bool CPluginTCPay::updateDepositNotify(const CMyString &gameCode, int32_t orderId, int32_t result, const CMyString &platformOrderNo, int64_t realFee, const CMyString &regionCode)
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
        this->postAlarm(Alarm_Fatal, CMyString("[%s]update deposit order failure, orderId:%d, result=>%d, err:%d, msg:%s", 
                    m_sdkTag.c_str(), orderId, result, err, szError));
        return false;
    }
    if (code < 0)
    {
        m_errCode = err_depositOrderUpdateFailure;
        m_errNote = strMsg;
        LogFatal("[%s]update deposit order failure, orderId:%d, result=>%d, msg:%s",
            m_sdkTag.c_str(), orderId, result, strMsg.c_str());
        this->postAlarm(Alarm_Fatal, CMyString("[%s]update deposit order failure, orderId:%d, result=>%d, msg:%s", 
                    m_sdkTag.c_str(), orderId, result, strMsg.c_str()));
        return false;
    }
    return true;
}

bool CPluginTCPay::postDepositResultToGamesvr(CGame *pGame, CSdk* pSdk, Json::Value &req_data)
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
    enum TCPayDepositStatus depositResult;
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

bool CPluginTCPay::deliverGoods(const CMyString &gameCode, const CMyString &strUrl, const CMyString &strData, enum TCPayDepositStatus &result, bool &bNeedReDeliver)
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
            this->postAlert(gameCode, CMyString("游服错误=>申请充值订单发放失败, msg:%s", m_errNote.c_str()));
            bNeedReDeliver = true;
            LogFatal("[%s]deliver goods to server [%s] failure, data[%s], err:%d, msg:%s",
                m_sdkTag.c_str(), strUrl.c_str(), strData.c_str(), ret, m_errNote.c_str());
            this->postAlarm(Alarm_Fatal, CMyString("[%s]deliver goods to server [%s] failure, data[%s], err:%d, msg:%s", 
                    m_sdkTag.c_str(), strUrl.c_str(), strData.c_str(), ret, m_errNote.c_str()));
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

void CPluginTCPay::postAlert(const CMyString &gameCode, const CMyString &note)
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

void CPluginTCPay::doPayFailure(char* szResp, uint32_t& cbResp)
{
    Json::Value resp;
    resp["code"] = m_errCode;
    resp["message"] = m_errNote;
    this->OnPayResp(resp, szResp, cbResp);
}

bool CPluginTCPay::finishDepositOrder(const CMyString &gameCode, int32_t orderId, int32_t result, int32_t bpId, const CMyString &strUin)
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
        this->postAlarm(Alarm_Fatal, CMyString("[%s]update deposit order failure, orderId:%d, result=>%d, err:%d, msg:%s", 
                    m_sdkTag.c_str(), orderId, result, err, szError));
        return false;
    }
    if (code < 0)
    {
        m_errCode = err_depositOrderUpdateFailure;
        m_errNote = strMsg;
        LogFatal("[%s]update deposit order failure, orderId:%d, result=>%d, msg:%s",
            m_sdkTag.c_str(), orderId, result, strMsg.c_str());
        this->postAlarm(Alarm_Fatal, CMyString("[%s]update deposit order failure, orderId:%d, result=>%d, msg:%s", 
                    m_sdkTag.c_str(), orderId, result, strMsg.c_str()));
        return false;
    }
    return true;
}

void CPluginTCPay::getDepositWealth(CGame *pGame, int32_t bpId, int64_t fee, int64_t& coin, int64_t& ext_coin, CMyString& itemInfo)
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

void CPluginTCPay::StrReplace(CMyString &str, const char *from, const char* to)
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

void CPluginTCPay::judgeBPInfo(CGame *pGame, const int32_t& bpId, const int64_t& fee, const CMyString& strUin, Json::Value& req_data, int64_t& coin, int64_t& ext_coin, CMyString& itemInfo, bool bNeedEscape)
{
    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    if (NULL == pBP)
    {
        LogErr("[%s]DELIVER-GOODS>not found billpoint %d", m_sdkTag.c_str(), bpId);
        this->postAlarm(Alarm_Err, CMyString("[%s]DELIVER-GOODS>not found billpoint %d", 
                m_sdkTag.c_str(), bpId));  
        return;
    }

    if (pBP->CanBuy(strUin))
    {
        req_data["category"] = pBP->getCategory();
        CMyString str1 = pBP->getExtraInfo();
        if (bNeedEscape)
        {
            StrReplace(str1, "\"", "\\\"");
        }
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

void CPluginTCPay::dealUnFinishOrder()
{
    if (!m_timerOrder.ToNextTime())
    {
        return;
    }

    CMyString patten = m_cacheOrder.getPatten();
    CMyString prefix = patten;
    prefix.Erase("*");

    int32_t cursor(0);
    int32_t next_cursor(0);
    int32_t ret(-1);
    std::vector<CMyString> vecKeys;
    do {
        ret = m_cacheOrder.Scan(10, vecKeys, next_cursor, cursor);
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
            
            if (val < 0)
            {
                continue;
            }
            if (timeout == INFINITE)
            {
                continue;
            }
            if (timeout == (uint32_t)(m_cacheOrder.getTimeout()))
            {
                continue;
            }
            if (timeout % 60 >= 3 ) //1分钟查询一次
            {
                continue;
            }

            LogDebug("[%s][%s:%d] val:%d, strkey:%s, timeout:%d",m_sdkTag.c_str(),  __FILE__, __LINE__, val, strKey.c_str(), timeout);  
            CMyString orderNo = strKey;
            orderNo.Erase(prefix);
            CMyString strRealKey, strData;
            m_cacheOrder.Get(orderNo, strRealKey, strData);
            
            CJsonWrapper data;
            if (data.Init(strData.c_str()))
            {
                CMyString strOpenId = data.getString("openid");
                CMyString strOpenKey = data.getString("openkey");
                CMyString strPf = data.getString("pf");
                CMyString strPfKey = data.getString("pf_key");
                int32_t nPlatForm = data.getInt("platform");
                CMyString strOrderNo = data.getString("orderno");
                int32_t amount = data.getInt64("amount");
                int32_t save_amt = data.getInt64("save_amt");
                CMyString strUin = data.getString("uin");
                
                CMyString billNo;
                int nRet = doConsumeAndDeliverGoods(strOrderNo, strOpenId, strOpenKey, strPf, strPfKey, strUin, nPlatForm, amount, billNo, save_amt);
                if (nRet == 0)
                {
                    LogDebug("[%s][%s:%d] del strRealKey:%s",m_sdkTag.c_str(),  __FILE__, __LINE__,  strRealKey.c_str()); 
                    m_cacheOrder.Pop(orderNo, strRealKey, strData);
                }
            }

            if (timeout <= 3)
            {
                LogDebug("[%s][%s:%d] del strRealKey:%s",m_sdkTag.c_str(),  __FILE__, __LINE__,  strRealKey.c_str());
                m_cacheOrder.Pop(orderNo, strRealKey, strData);
            }
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

void CPluginTCPay::dealReDeliver(void)
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

void CPluginTCPay::dealReDeliver(const CMyString &key)
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
    enum TCPayDepositStatus depositResult;
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
            LogFatal("[%s]deliver goods more than %d times, orderNo:%s url:%s, data:%s",
                    m_sdkTag.c_str(), deliverIdx+1, orderNo.c_str(), strUrl.c_str(), strData.c_str());
            this->postAlarm(Alarm_Fatal, CMyString("[%s]deliver goods more than %d times, orderNo:%s url:%s, data:%s", 
                    m_sdkTag.c_str(), deliverIdx+1, orderNo.c_str(), strUrl.c_str(), strData.c_str()));
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