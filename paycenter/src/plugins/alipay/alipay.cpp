/*
 * =====================================================================================
 *
 *       Filename:  alipay.cpp
 *
 *    Description:  alipay implement
 *
 *        Version:  1.0
 *        Created:  2019年09月21日 14时06分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "alipay.h"
#include "Security.h"
#include "Config.h"
#include "SdkList.h"
#include "BillPointList.h"
#include "SdkChannelList.h"


extern void Json2Url(Json::Value &parent_value, CMyString &str);

CPluginAlipay::CPluginAlipay()
    : CPayBase("Alipay", depositRequest, httpRequestSync)
{

}

CPluginAlipay::~CPluginAlipay()
{

}

int32_t CPluginAlipay::Init(void)
{
    int32_t ret(0);
    ret = this->Update();
    return ret;
}

int32_t CPluginAlipay::Update(void)
{
    int32_t nRet = CPayBase::Init("main/alipay");

    CXmlFile &xml = CONFIG.getXml();
    // const CMyString strApp("main/alipay/crazyfish");
    // int32_t ret = load_key_file(xml, strApp, "key", m_priKey);
    // if (ret < 0)
    // {
    //     LogFatal("invalid private key.");
    // }

    // ret = load_key_file(xml, strApp, "pubkey", m_pubKey);
    // if (ret < 0)
    // {
    //     LogFatal("invalid public key.");
    // }
  
    const CMyString strCancelPay("main/alipay/deposit/request");
    xml.getString(strCancelPay, "cancelPay", m_strCancelPayUrl);

    void *pRoot(NULL);
    try
    {
        pRoot = xml.GetElement("main/alipay/crazyfish/item");
        int32_t val(0);
        while(pRoot != NULL)
        {
            CMyString strPriKey, strPubKey;
            CMyString strAppId = xml.GetValue(pRoot, "appid");
            CMyString strPriFile = xml.GetValue(pRoot, "key");
            val = load_key_file(strPriFile, strPriKey);
            if (val == 0)
            {
                LogDebug("[%s:%s:%d] appid:%s, prikey:%s", __FILE__, __func__, __LINE__, strAppId.c_str(), strPriKey.c_str());
                m_mapPriKey[strAppId] = strPriKey;
            }
            CMyString strPubFile = xml.GetValue(pRoot, "pubkey");
            val = load_key_file(strPubFile, strPubKey);
            if (val == 0)
            {
                LogDebug("[%s:%s:%d] appid:%s, pubkey:%s", __FILE__, __func__, __LINE__, strAppId.c_str(), strPubKey.c_str());
                m_mapPubKey[strAppId] = strPubKey;
            }
            if (val != 0)
            {
                // 出错了，中止操作
                nRet = -101;
                break;
            }
            pRoot = xml.GetNextSibling(pRoot, "item");
        }
    }
    catch(CException &ex)
    {
        LogErr("get alipay key failure, err:%s", ex.what());
        nRet = -103;
    }
    catch(std::exception &ex)
    {
        LogErr("get alipay key failure, err:%s", ex.what());
        nRet = -105;
    }
    catch(...)
    {
        LogErr("get alipay key failure, unknown err");
        nRet = -107;
    }

    return nRet;
}

void CPluginAlipay::Maintance(void)
{
    CPayBase::Maintance();
}

int32_t CPluginAlipay::load_key_file(CXmlFile &xml, const CMyString &strApp, const CMyString &strKey, CMyString &val)
{
    CMyString strFile = xml.GetString(strApp.c_str(), strKey.c_str());
    
    FILE *fp = fopen(strFile.c_str(), "r");
    if (NULL == fp)
    {
        return false;
    }
    CAutoFile f(fp);
    fseek(fp, 0, SEEK_END);
    uint32_t pos = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    CTempBuffer tmpbuf(pos+1);
    fread(tmpbuf.data(), 1, pos, fp);

    val = tmpbuf.data();

    return 0;
}

int32_t CPluginAlipay::load_key_file(const CMyString &strFile, CMyString &val)
{
    FILE *fp = fopen(strFile.c_str(), "r");
    if (NULL == fp)
    {
        return false;
    }
    CAutoFile f(fp);
    fseek(fp, 0, SEEK_END);
    uint32_t pos = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    CTempBuffer tmpbuf(pos+1);
    fread(tmpbuf.data(), 1, pos, fp);

    val = tmpbuf.data();

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginAlipay, doDeposit)
{
    setCreditRequestMethod(httpUseGet);
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CPluginAlipay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

// URL: https://opendocs.alipay.com/apis/api_1/alipay.trade.wap.pay
void CPluginAlipay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    CMyString strAppId, strMethod, strSignType;
    CMyString strProductCode, strSubject, strBody;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strAppId = extraData["appid"].asString();
        strMethod = extraData["method"].asString();
        strProductCode = extraData["product_code"].asString();
        strBody = extraData["body"].asString();
        strSignType = extraData["sign_type"].asString();
    }

    int32_t bpId = req.getParamInt("bpId");
    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    if (NULL != pBP)
    {
        strSubject = pBP->getBpCode();
    }
    else
    {
        LogErr("[%s]CPluginAlipay::doMakeDepositRequest >not found billpoint %d", m_sdkTag.c_str(), bpId);
        this->postAlarm(Alarm_Err, CMyString("[%s]CPluginAlipay::doMakeDepositRequest >not found billpoint %d", 
                m_sdkTag.c_str(), bpId));
    }

    Json::Value biz_content;
    biz_content["out_trade_no"] = orderNo.c_str();
    biz_content["total_amount"] = CMyString("%.2lf", req.getParamInt64("amount")/100.0).c_str();  //订单总金额,单位为元，精确到小数点后两位
    biz_content["subject"] = strSubject.c_str();
    biz_content["body"] = strBody.c_str();
    biz_content["product_code"] = strProductCode.c_str();
    biz_content["timeout_express"] = "5m";
    biz_content["quit_url"] = m_strCancelPayUrl;

    CJsonWrapper writer;
    Json::Value data;
    data["app_id"] = strAppId;
    data["method"] = strMethod;
    data["charset"] = "utf-8";
    data["sign_type"] = strSignType;
    CMyDateTime dt;
    data["timestamp"] = dt.c_str("%Y-%m-%d %H:%M:%S");
    data["versoin"] = "1.0";
    data["notify_url"] = pSdk->getPayCallbackUrl();
    data["return_url"] = pGame->getCallbackUrl();
    CMyString strBiz = writer.write(biz_content);
    data["biz_content"] = strBiz;

    http::CHttpRespMaker req_maker;
    req_maker.AddJson(data);
    CMyString str1 = req_maker.http_str_manual('&', '=', true, true, false);
    LogDebug("[%s] CPluginAlipay::doMakeDepositRequest sign data：%s", m_sdkTag.c_str(), str1.c_str());

    CMyString strSign;
    this->sign(str1, strSignType, strAppId, strSign);
    req_maker.add("sign", strSign);

    httpData = req_maker.http_str_manual('&', '=', false, false, true);
    url = pSdk->getPayUrl();
    contentType = "Content-Type: application/x-www-form-urlencoded";
}

bool CPluginAlipay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    LogDebug("[%s]deposit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());

    if (respHeader.empty())
    {
        errNote.Format("not any data respone");
        return false;
    }

    depositUrlData = "N/A";
    CMyString strHeader = respHeader;
    strHeader.MakeLower();
    CMyString strKey = "location: ";
    size_t nPos = strHeader.find(strKey);
    if (nPos != std::string::npos)
    {
        CMyString strTemp = strHeader.substr(nPos + strKey.length());
        nPos = strTemp.find("\n");
        if (nPos != std::string::npos)
        {
            depositUrlData = strTemp.substr(0, nPos);
            depositUrlData = depositUrlData.substr(0, depositUrlData.length() - 1); 
        }
    }
    else
    {
        return false;
    }

    //不可用splite，%2F会变成0.00000
    // CStrSpliter css;
    // css.Split(respHeader, '\n');
    // if (css.empty())
    // {
    //     LogDebug("[%s]deposit resp Split respHeader fail, ", m_sdkTag.c_str());
    //     return false;
    // }

    // CMyString strKey = "location: ";
    // CMyString strKey2 = "Location: ";
    // depositUrlData = "N/A";
    // for(int32_t i(0); i < css.size(); ++i)
    // {
    //     CMyString strHead = css.asString(i);
    //     LogDebug("[%s:%s:%d] strHead:%s ", __FILE__, __func__, __LINE__, strHead.c_str());
    //     size_t nPos = strHead.find(strKey);
    //     if (nPos != std::string::npos)
    //     {
    //         depositUrlData = strHead.substr(strKey.length());
    //         depositUrlData = depositUrlData.substr(0, depositUrlData.length() - 1); 
    //         LogDebug("[%s:%s:%d] depositUrlData:%s ", __FILE__, __func__, __LINE__, depositUrlData.c_str());
    //         break;
    //     }
    //     nPos = strHead.find(strKey2);
    //     if (nPos != std::string::npos)
    //     {
    //         depositUrlData = strHead.substr(strKey2.length());
    //         depositUrlData = depositUrlData.substr(0, depositUrlData.length() - 1); 
    //         LogDebug("[%s:%s:%d] depositUrlData:%s ", __FILE__, __func__, __LINE__, depositUrlData.c_str());
    //         break;
    //     }
    // }

    LogDebug("[%s]deposit resp depositUrlData:%s ", m_sdkTag.c_str(), depositUrlData.c_str());

    return true;
}

bool CPluginAlipay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
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

    CMyString strStatus = req.getParam("trade_status");
    if (strStatus == "TRADE_SUCCESS")
    {
        depositedOK = true;
    }
    else
    {
        depositedOK = false;
    }

    orderNo = req.getParam("out_trade_no");
    platformOrderNo = req.getParam("trade_no");
    fee = (int64_t)(req.getParam("total_amount").asDouble() * 100); //交易的订单金额，单位为元，两位小数
    realFee = (int64_t)(req.getParam("receipt_amount").asDouble() * 100);//实收金额，单位为元，两位小数。该金额为本笔交易，商户账户能够实际收到的金额
    tax = (int64_t)(req.getParam("point_amount").asDouble() * 100); //	积分支付的金额，单位为元，两位小数

    return true;
}

bool CPluginAlipay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign = req.getParam("sign");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("sign");
    vecExcept.push_back("sign_type");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString strSignType = req.getParam("sign_type");
    CMyString strAppId = req.getParam("app_id");

    CMyString str1 = resp_maker.http_str_manual('&', '=', true, true, false);
    str1.Replace("+", " ");
    LogDebug("[%s] CPluginAlipay::validDepositCallbackSign sign data：%s", m_sdkTag.c_str(), str1.c_str());
    bool bRet = this->signVerify(str1, strSignType, strAppId, strSign);

    //验证签名是否匹配，忽略大小写
    if (!bRet)
    {
        LogErr("[%s]deposit callback sign mismatch, origin text[%s], sign[%s]",
                m_sdkTag.c_str(), str1.c_str(), strSign.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]deposit callback sign mismatch, origin text[%s], sign[%s]", 
                m_sdkTag.c_str(), str1.c_str(), strSign.c_str()));
        return false;
    }

    return true;
}

void CPluginAlipay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    if (bSucceed)
    {
        safe_strcpy(szResp, cbResp, "success");   
    }
    else
    {
        safe_strcpy(szResp, cbResp, "fail");
    }
    cbResp = strlen(szResp);
}

void CPluginAlipay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CPluginAlipay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CPluginAlipay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CPluginAlipay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CPluginAlipay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}

int32_t CPluginAlipay::rsaSign(const uint8_t *input, size_t cbInput, const CMyString &strPriKey, const CMyString strType, CMyString &strCipher)
{
    int32_t result(-1);
    const char* key_cstr = strPriKey.c_str();
    int32_t key_len = strPriKey.length();
    BIO *p_key_bio = BIO_new_mem_buf((void*)key_cstr, key_len);
    RSA *p_rsa = PEM_read_bio_RSAPrivateKey(p_key_bio, NULL, NULL, NULL);
    if (NULL != p_rsa)
    {
        uint8_t sign[XRSA_KEY_BITS / 8 + 1] = {0};
        uint32_t sign_len = sizeof(sign);
        int ret = 0;

        if (strcasecmp(strType.c_str(), "sha1") == 0)
        {
            //对 input 做SHA1加密
            uint8_t hash[SHA_DIGEST_LENGTH+1] = {0};
            SHA1(input, cbInput, hash);
            ret = RSA_sign(NID_sha1, hash, SHA_DIGEST_LENGTH, sign, &sign_len, p_rsa);
        }
        else if (strcasecmp(strType.c_str(), "sha256") == 0)
        {
            //对 input 做SHA256加密
            uint8_t hash[SHA256_DIGEST_LENGTH+1] = {0};
            SHA256(input, cbInput, hash);
            ret = RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, &sign_len, p_rsa);
        }

        if (ret > 0 && sign_len > 0)
        {
            strCipher = CBase64Wrapper::Encode((const char*)&sign[0], sign_len, false);
            result = 0;
        }
    }
    RSA_free(p_rsa);
    BIO_free(p_key_bio);

    return result;
}

bool CPluginAlipay::rsaVerify(const uint8_t *input, size_t cbInput, const CMyString &strSign, const CMyString &strPubKey, const CMyString strType)
{
    bool result(false);
    const char *key_cstr = strPubKey.c_str();
    int key_len = strPubKey.length(); 
    BIO *p_key_bio = BIO_new_mem_buf((void *)key_cstr, key_len);
    RSA *p_rsa = PEM_read_bio_RSA_PUBKEY(p_key_bio, NULL, NULL, NULL);
    if (p_rsa != NULL)
    {   
        // Base64 解密
        CMyString strSign2 = CBase64Wrapper::Decode(strSign.c_str(), strSign.length(), false );
        int ret = 0;

        if (strcasecmp(strType.c_str(), "sha1") == 0)
        {
            // SHA 解密
            uint8_t hash[SHA_DIGEST_LENGTH] = {0};
            size_t cbHash = sizeof(hash);
            ShaWrapper::SHA1(input, cbInput, hash, cbHash);

            // RSA verify 
            ret = RSA_verify(NID_sha1, hash, SHA_DIGEST_LENGTH, 
                (uint8_t*)strSign2.c_str(), strSign2.size(), p_rsa);
        }
        else if (strcasecmp(strType.c_str(), "sha256") == 0)
        {
            // SHA 解密
            uint8_t hash[SHA256_DIGEST_LENGTH] = {0};
            size_t cbHash = sizeof(hash);
            uint32_t sign_len = XRSA_KEY_BITS / 8; 
            ShaWrapper::SHA256(input, cbInput, hash, cbHash);

            // RSA verify 
            ret = RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH, 
                (uint8_t*)strSign2.c_str(), sign_len, p_rsa);
        }

        if (ret > 0)
        {   
            result = true;
        }
    }   
    RSA_free(p_rsa);
    BIO_free(p_key_bio); 
    return result;
}

int32_t CPluginAlipay::sign(const CMyString &url_param, const CMyString &strType, const CMyString& strAppId, CMyString &sign)
{
    CMyString strShaType;
    if (strType == "RSA")
    {
        strShaType = "sha1";
    }
    else if (strType == "RSA2")
    {
        strShaType = "sha256";
    }

    CMyString strPriKey;
    auto iter = m_mapPriKey.find(strAppId);
    if (iter != m_mapPriKey.end())
    {
        strPriKey = iter->second;
    }
    
    int32_t nRet = this->rsaSign((const uint8_t*)url_param.c_str(), url_param.length(),  strPriKey, strShaType, sign); 
    LogDebug("[%s] CPluginAlipay::sign ret:%d sign data：%s", m_sdkTag.c_str(), nRet, sign.c_str());
    
    return nRet;
}

bool CPluginAlipay::signVerify(const CMyString &url_param, const CMyString &strType, const CMyString& strAppId, CMyString &sign)
{ 
    CMyString strShaType;
    if (strType == "RSA")
    {
        strShaType = "sha1";
    }
    else if (strType == "RSA2")
    {
        strShaType = "sha256";
    }

    CMyString strPubKey;
    auto iter = m_mapPubKey.find(strAppId);
    if (iter != m_mapPubKey.end())
    {
        strPubKey = iter->second;
    }

    bool bRet = this->rsaVerify((const uint8_t*)url_param.c_str(), url_param.length(), sign, strPubKey, strShaType); 
    LogDebug("[%s] CPluginAlipay::signVerify return ret: %s", m_sdkTag.c_str(), bRet ? "true" : "false");
    
    return bRet;
}
