/*
 * =====================================================================================
 *
 *       Filename:  XiaomiPay.cpp
 *
 *    Description:  小米支付
 *
 *        Version:  1.0
 *        Created:  2021年09月26日 19时42分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "XiaomiPay.h"
#include "SdkList.h"
#include "Security.h"
#include "SdkChannelList.h"
#include "Security/CryptHelper.h"

CXiaomiPay::CXiaomiPay()
    : CPayBase("XiaomiPay", depositVerify, httpRequestSync)
{

}

CXiaomiPay::~CXiaomiPay()
{

}

int32_t CXiaomiPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CXiaomiPay::Update(void)
{
    return CPayBase::Init("main/xiaomipay");
}

void CXiaomiPay::Maintance(void)
{
    CPayBase::Maintance();
}

ENTRY_FUNC_DECLARE(CXiaomiPay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CXiaomiPay, doDepositCB)
{
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

void CXiaomiPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    
}

bool CXiaomiPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    return true;
}

bool CXiaomiPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK)
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

    //0等待支付，1支付成功，2支付失败，3支付取消
    CMyString strStatus = req.getParam("orderStatus");
    if (strStatus == "TRADE_SUCCESS")
    {
        depositedOK = true;
    }
    else
    {
        depositedOK = false;
    }

    orderNo = req.getParam("cpOrderId");
    platformOrderNo = req.getParam("orderId");
    fee = (int64_t)(req.getParamInt64("payFee")); //支付金额,单位为分,即0.01 米币。
    realFee = fee; 
    tax = 0;

    return true;
}

string byteToHexStr(unsigned char byte_arr[], int arr_len)
{
	string hexstr;
	for (int i=0;i<arr_len;i++)
	{
		char hex1;
		char hex2;
		int value=byte_arr[i]; //直接将unsigned char赋值给整型的值，强制转换
		int v1=value/16;
		int v2=value % 16;
 
		//将商转成字母
		if (v1>=0&&v1<=9)
			hex1=(char)(48+v1);
		else
			hex1=(char)(55+v1);
 
		//将余数转成字母
		if (v2>=0&&v2<=9)
			hex2=(char)(48+v2);
		else
			hex2=(char)(55+v2);
 
		//将字母连接成串
		hexstr=hexstr+hex1+hex2;
	}
	return hexstr;
}

bool CXiaomiPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("signature");

    http::CHttpRespMaker resp_maker;
    std::vector<CMyString> vecExcept;
    vecExcept.push_back("signature");
    req.BuildRespMaker_Except(vecExcept, resp_maker);

    CMyString strAppKey;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strAppKey = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strAppKey: %s", __FILE__, __func__, __LINE__, strAppKey.c_str());
    }
    if (strAppKey.empty())
    {
        strAppKey = pSdk->getApiKey();
    }

    CMyString str1 = resp_maker.http_str_manual('&', '=', true, true, false);
    CMyString strSignData = decodeParam(str1);
    LogDebug("[%s:%s:%d] singData:%s", __FILE__, __func__, __LINE__, strSignData.c_str());
    string strSign2 = UTILS::CryptHelper::HmacWithSha1ToHex(strAppKey.c_str(), strSignData.c_str());

    // 验证签名是否匹配，忽略大小写
    if (strcasecmp(strSign1.c_str(), strSign2.c_str()) != 0)
    {
        LogErr("[%s]deposit callback sign mismatch, origin text[%s], sign[%s], sign from platform[%s]",
                m_sdkTag.c_str(), strSignData.c_str(), strSign2.c_str(), strSign1.c_str());
        this->postAlarm(Alarm_Err, CMyString("[%s]deposit callback sign mismatch, origin text [%s], sign[%s], sign from platform[%s]", 
                m_sdkTag.c_str(), strSignData.c_str(), strSign2.c_str(), strSign1.c_str()));
        return false;
    }
    return true;
}

void CXiaomiPay::doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    Json::Value resp;
    if (bSucceed)
    {
        resp["errcode"] = 200;
    }
    else
    {
        switch(errCode)
        {
            case err_depositCallbackDataCorrupted:
            case err_depositDataLost:
                resp["errcode"] = 3515;
                break;
            case err_depositCallbackSignMismatch:
                resp["errcode"] = 1525;
                break;
            default:
                resp["errcode"] = errCode;
                break;
        }  
    }

    CJsonWrapper jw(resp);
    CMyString strResp = jw.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

void CXiaomiPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData)
{

}

bool CXiaomiPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    return true;
}

bool CXiaomiPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    return true;
}

bool CXiaomiPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    return true;
}

void CXiaomiPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{

}

CMyString CXiaomiPay::decodeParam(const CMyString& strUrl)
{
    CMyString strReturnUrl; 
    CStrSpliter splite;
    splite.Split(strUrl, '&');
    if (splite.empty())
    {
        return strUrl;
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
                    CMyString strParam = data.asString(1);
                    strReturnUrl += UTILS::CryptHelper::UrlDecode(strParam.c_str(), strParam.length());
                }
            }
            else
            {
                strReturnUrl += "&";
                strReturnUrl += data.asString(0);
                strReturnUrl += "=";
                if (data.size() == 2)
                {
                    CMyString strParam = data.asString(1);
                    strReturnUrl += UTILS::CryptHelper::UrlDecode(strParam.c_str(), strParam.length());
                }
            }
        }
    }

    return strReturnUrl;
}