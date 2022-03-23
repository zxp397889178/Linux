/*
 * =====================================================================================
 *
 *       Filename:  TYybDirectpay.cpp
 *
 *    Description:  腾讯应用宝直购模式
 *
 *        Version:  1.0
 *        Created:  2021年07月15日 17时57分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "TYybDirectpay.h"
#include "Security.h"
#include "SdkList.h"
#include "GameList.h"
#include "SdkChannelList.h"
#include "MyNet.h"


string escapeURL(const string &URL);
string validParam(const string& strUrl);

CPluginTDPay::CPluginTDPay()
    : CPayBase("TDPay", depositVerify, httpRequestSync)
{

}

CPluginTDPay::~CPluginTDPay()
{

}

int32_t CPluginTDPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CPluginTDPay::Update(void)
{
    return CPayBase::Init("main/tdpay");
}

void CPluginTDPay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CPluginTDPay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginTDPay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

//https://wiki.open.qq.com/index.php?title=%E7%B1%B3%E5%A4%A7%E5%B8%88%E6%9C%8D%E5%8A%A1%E7%AB%AF%E6%8E%A5%E5%85%A5
void CPluginTDPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    
}

bool CPluginTDPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}


bool CPluginTDPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &depositOK)
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

    CMyString appmeta = req.getParam("appmeta");
    int pos = appmeta.find('*');
    orderNo = appmeta.substr(0, pos);

    platformOrderNo = req.getParam("billno");

    //发货
    Json::Value req_data;
    if (!this->fetchDepositOrderData(req, orderNo, req_data))
    {
        LogErr("[%s][%s:%s:%d]not found deposit order %s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, orderNo.c_str());
        return false;
    }

    //https://wiki.open.qq.com/wiki/%E5%9B%9E%E8%B0%83%E5%8F%91%E8%B4%A7URL%E7%9A%84%E5%8D%8F%E8%AE%AE%E8%AF%B4%E6%98%8E_V3
    uint32_t amt = req.getParamInt("amt"); //这里以0.1Q点为单位。即如果总金额为18Q点，则这里显示的数字是180   1Q币 = 10Q点 = 100 0.1Q点 ，所以这里返回的是分为单位
    uint32_t money = req_data["fee"].asInt();
    LogDebug("[%s] [%s:%d] amount range: %u - %u", m_sdkTag.c_str(), __FILE__, __LINE__, uint32_t(money * 0.9), uint32_t(money * 1.1));
    if (amt < uint32_t(money * 0.9) || amt > uint32_t(money * 1.1))
    {
        LogErr("[%s][%s:%s:%d]dealYybResult amount error, amt: %d, order amount: %u", m_sdkTag.c_str(), __FILE__, __func__, __LINE__,  amt, money);
        this->postAlarm(Alarm_Err, CMyString("[%s][%s:%s:%d]dealYybResult amount error, amt: %d, order amount: %u",
                m_sdkTag.c_str(), __FILE__, __func__, __LINE__,  amt, money));
        return  false;
    }

    fee = amt;
    realFee = money;
    tax = 0;
    depositOK = true;

    return true;
}

bool CPluginTDPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("sig");

    //构造签名数据
    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sig");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString str1 = resp_maker.http_str_manual('&', '=', false, true, false);
    str1 = validParam(str1);
    LogDebug("[%s] sign param: %s ", m_sdkTag.c_str(), str1.c_str());

    CMyString strAppKey, strUrl;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strAppKey = extraData["appkey"].asString();
        strUrl = extraData["yyb_sign_url"].asString();
        LogDebug("[%s:%s:%d] strAppKey: %s, yyb_sign_url:%s", __FILE__, __func__, __LINE__, strAppKey.c_str(), strUrl.c_str());
    }
    if (strAppKey.empty())
    {
        strAppKey = pSdk->getApiKey();
    }
    if (strUrl.empty())
    {
        strUrl = pSdk->getPayCallbackUrl();
    }
    
    CMyString strSign2 = GetSign(strUrl, strAppKey, str1);

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), str1.c_str(), strSign2.c_str(), strSign1.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), str1.c_str(), strSign2.c_str(), strSign1.c_str()));
        return false;
    }

    return true;
}

void CPluginTDPay::doDepositCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    LogDebug("[%s] CPluginTDPay::doDepositCallbackResp ", m_sdkTag.c_str());
    Json::Value resp_data;

    ext_headers["Content-Type"] = "text/plain";
    if (bSucceed)
    {
        resp_data["ret"] = 0;
        resp_data["msg"] = "OK"; 
    }
    else
    {
        resp_data["ret"] = errCode;
        resp_data["msg"] = "FAIL";
    }

    CJsonWrapper wr;
    CMyString strResp = wr.write(resp_data);
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}


void CPluginTDPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData) 
{

}


bool CPluginTDPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return false;
}


bool CPluginTDPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}


bool CPluginTDPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}


void CPluginTDPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
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

string escapeValueValid(const string &value)
{
    string result = "";
    for (unsigned int i = 0; i < value.size(); i++)
    {
        char c = value[i];
        if (
            ('0' <= c && c <= '9') ||
            ('a' <= c && c <= 'z') ||
            ('A' <= c && c <= 'Z') ||
            c == '!' || c == '*' || c == '('|| c == ')')
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

string validParam(const string& strUrl)
{
    string strReturnUrl; 
    CStrSpliter splite;
    splite.Split(strUrl, '&');
    if (splite.empty())
    {
        return strReturnUrl;
    }
    for(int32_t i(0); i < splite.size(); ++i)
    {
        CMyString strData = splite.asString(i);
        CStrSpliter data;
        data.Split(strData, '=');
        if (!data.empty())
        {
            if (strReturnUrl.empty())
            {
                strReturnUrl += data.asString(0);
                strReturnUrl += "=";
                if (data.size() == 2)
                {
                    strReturnUrl += escapeValueValid(data.asString(1));
                }
            }
            else
            {
                strReturnUrl += "&";
                strReturnUrl += data.asString(0);
                strReturnUrl += "=";
                if (data.size() == 2)
                {
                    strReturnUrl += escapeValueValid(data.asString(1));
                }
            }
        }
    }

    return strReturnUrl;
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

CMyString CPluginTDPay::GetSign(CMyString& strUrl, CMyString& strAppKey, CMyString& strParam)
{
    size_t nPos = strUrl.find("://");
    if (nPos != string::npos)
    {
        strUrl = strUrl.substr(nPos + 3);
        nPos = strUrl.find("/");
        if (nPos != string::npos)
        {
            strUrl = strUrl.substr(nPos);
        }
    }
    CMyString strEncodeUrl = escapeURL(strUrl);
    CMyString strEncodeParam = escapeURL(strParam);

    CMyString strSignData("GET&%s&%s", strEncodeUrl.c_str(), strEncodeParam.c_str());
    CMyString strKey = strAppKey + "&";

    LogDebug("[%s] sign key: %s data：%s", m_sdkTag.c_str(), strKey.c_str(), strSignData.c_str());

    //CMyString str = ShaWrapper::Hmac("sha1", strKey.c_str(), strSignData.c_str());
    string str = HmacWithSha1(strKey.c_str(), strSignData.c_str());
    string strSign = EncodeWithOut0A(str.c_str(), str.length());

    LogDebug("[%s] sign sha1: %s Encode: %s", m_sdkTag.c_str(), str.c_str(), strSign.c_str());
    
    return strSign;
}

bool CPluginTDPay::fetchDepositOrderData(http::CHttpRequest& req, const CMyString& orderNo, Json::Value& req_data)
{
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
        LogFatal("[%s]query deposit order %s failure, err:%s, msg:%s",
            m_sdkTag.c_str(), orderNo.c_str(), err, szError);
        this->postAlarm(Alarm_Fatal, CMyString("[%s]query deposit order %s failure, err:%s, msg:%s", 
                    m_sdkTag.c_str(), orderNo.c_str(), err, szError));
        return false;
    }
    if (!bFind)
    {
        LogFatal("[%s]deposit order %s not found.", m_sdkTag.c_str(), orderNo.c_str());
        this->postAlarm(Alarm_Fatal, CMyString("[%s]deposit order %s not found.", 
                    m_sdkTag.c_str(), orderNo.c_str()));
        return false;
    }

    int32_t orderStatus = req_data["status"].asInt();
    bool result(false);
    switch(orderStatus)
    {
    case 0:           // 0
        LogErr("orderNo %s has created, platform callback maybe fault.", orderNo.c_str());
        break;
    case 3:            // 3
        LogErr("orderNo %s has cancelled by user, platform callback maybe fault.", orderNo.c_str());
        break;
    case 7:           // 7
        LogErr("orderNo %s request deposit failed, platform callback maybe fault.", orderNo.c_str());
        break;
    case 4:                // 4
        LogErr("orderNo %s has refund, platform callback maybe fault.", orderNo.c_str());
        break;
    case 1:          // 1
    case 5:        // 5
    case 8:     // 8
    case 9:           // 9
    case 10:         // 10
        LogErr("orderNo %s has paid, can not deliver goods again.", orderNo.c_str());
        break;
    case 2:             // 2
    case 6:        // 6
    case 11:        // 11
        LogDebug("[%s]orderNo %s callback again.", m_sdkTag.c_str(), orderNo.c_str());
        result = true;
        break;
    default:
        LogDebug("orderNo %s has invalid status %d", orderNo.c_str(), orderStatus);
        break;
    }
    return result;
}