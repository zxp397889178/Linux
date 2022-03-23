/*
 * =====================================================================================
 *
 *       Filename:  Pay77.cpp
 *
 *    Description:  pay77 接口实现
 *
 *        Version:  1.0
 *        Created:  2021年01月08日 15时14分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "vnpay.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"
#include "Common.h"

CPluginVnPay::CPluginVnPay()
    : CPayWrapper()
{
    
}

CPluginVnPay::~CPluginVnPay()
{
}

int32_t CPluginVnPay::Init(void)
{ 
    const CMyString strComm("main/vnpay");
    int32_t ret = CPayWrapper::Init(strComm);
    if (ret < 0)
    {   
        return ret;
    }   
      
    CXmlFile &xml = CONFIG.getXml();
    CMyString strFile = xml.GetString(strComm.c_str(), "privkey");
    ret = load_key_file(strFile, m_priKey);
    if (ret < 0)
    {
        LogFatal("invalid private key.");
        return -1003;
    }

    strFile = xml.GetString(strComm.c_str(), "pubkey");
    ret = load_key_file(strFile, m_pubKey);
    if (ret < 0)
    {
        LogFatal("invalid public key.");
        return -1005;
    }

    return 0;
}


int32_t CPluginVnPay::load_key_file(CMyString& strFile, CMyString &val)
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

int32_t CPluginVnPay::Update(void)
{
    return this->Init();
}

void CPluginVnPay::Maintance()
{
    CPayWrapper::Maintance();
}

// 以下为调用网银接口
ENTRY_FUNC_DECLARE(CPluginVnPay, doPay)
{

    LogWarn("---------THIS-----doInit--------%p----", this);


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

///////// 以下为宝丽平台专用调用
    int32_t payMoney(0);
    CMyString strProductId;
    CMyString strNick;
    req.getParam("productId", strProductId);
    req.getParam("amount", payMoney);
    req.getParam("userName", strNick);

    // 生成发送到支付平台的数据
    CJsonWrapper sdk_extdata;
    pSdk->getPayExtraData(sdk_extdata);
    //LogWarn("extra_data:[%s], appid[%s]", keydata.ext_data.c_str(), sdk_extdata.getString("appid").c_str());


    CMyDateTime tNow;
     CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    CMyString strCallbackUrl = pGame->getCallbackUrl();

    http::CHttpRespMaker req_data;


 /*   req_data.add("merchantCode", pSdk->getAccount());
    req_data.add("method", "ZALO");
    req_data.add("orderNum", strOrderNo);
    req_data.add("payMoney", payMoney);
    req_data.add("productDetail", strProductId);
    req_data.add("name", strNick);
    req_data.add("email", "abcd@google.com");   
    req_data.add("redirectUrl", strCallbackUrl);
    req_data.add("notifyUrl", strNotifyUrl);
    req_data.add("phone", "565849512");
    req_data.add("expiryPeriod", "100000");
    req_data.add("dateTime", tNow.c_str("%Y%m%d%H%M%S"));
*/

    // m_sortMap["tttt"] = "dsffsdfsdf";

    std::map<CMyString, CMyString> paraList;

    this->AddPara(req_data, paraList, "merchantCode", pSdk->getAccount());
    this->AddPara(req_data, paraList, "method", "MOMO");
    this->AddPara(req_data, paraList, "orderNum", strOrderNo);
    this->AddPara(req_data, paraList, "payMoney", payMoney);
    this->AddPara(req_data, paraList, "productDetail", strProductId);
    this->AddPara(req_data, paraList, "name", strNick);
    this->AddPara(req_data, paraList, "email", "abcd@google.com");
    this->AddPara(req_data, paraList, "redirectUrl", strCallbackUrl);
    this->AddPara(req_data, paraList, "notifyUrl", strNotifyUrl);
    this->AddPara(req_data, paraList, "phone", "565849512");
    this->AddPara(req_data, paraList, "expiryPeriod", "100000");
    this->AddPara(req_data, paraList, "dateTime", tNow.c_str("%Y%m%d%H%M%S"));
    this->AddPara(req_data, paraList, "bankCode", "");

    // 生成sign
    CMyString strSign;
    CMyString str1 = this->GetSignSrc(paraList);
    
    ret = CRsaWrapper::PrivEncrypt((uint8_t*)str1.c_str(), str1.length(), m_priKey, strSign);

    LogDebug("origin text[%s] sign[%s]", str1.c_str(), strSign.c_str());

    req_data.add("sign", strSign.c_str());
    CMyString strReqData = req_data.json_str(true);
    //CMyString strSign = req_data.http_str(true);
    //CMyString strReqData("params=%s", strSign1.c_str());
    LogDebug("need send data[%s]", strReqData.c_str());

    // 发送到第三方平台
    CMyString strContentType("Content-Type: application/json");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());

    CMyString strHeader;
    CMyString strResp;
    CMyString strError;
    CMyString strTransferUrl;
    int32_t result(0);
    CMyString platformOrderNo("N/A");
    do {
        CMyString strUrl = pSdk->getPayUrl();


        LogWarn("strurl [ %s ]", strUrl.c_str());

        ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strReqData.c_str());
        if (ret != 0)
        {
            strError.Format("host [%s] unreachable.", strUrl.c_str());
            // 远程连接失败
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strError.c_str());
            result = -1021;
            break;
        }
        strTransferUrl = strResp;

        LogWarn("header [ %s ]", strHeader.c_str());
        LogWarn("resp   [ %s ]", strResp.c_str());
        http::CHttpRespone http_resp;
        http_resp.read((void*)strHeader.c_str(), strHeader.size());
        http_resp.read((void*)strResp.c_str(), strResp.size());

    //    result = dealJsonResp(strResp, strTransferUrl, strError, platformOrderNo);
        if (result >=0)
        {
            strTransferUrl = strError;
        }
    }while(0);

    time_t t1 = time(NULL);
    char szReqTime[64] = "";
    TimeToStr(t1, szReqTime, sizeof(szReqTime));
    if (result < 0)
    {
        // 记录数据库，失败
        CPayWrapper::_after_send_request(nOrderId, payReqFail, strSign, szReqTime, strHeader, strResp, platformOrderNo);
        this->OnPayFailure(result, strError.c_str(), szResp, cbResp);
        // TODO: 订单数据让其超时
        return result;
    }
    else
    {
        CPayWrapper::_after_send_request(nOrderId, payReqSucceed, strSign, szReqTime, strHeader, strResp, platformOrderNo);
    }

    // 记录支付的主键
    CPayWrapper::on_save_orderId(strOrderNo, nOrderId);

    // 返回给调用端
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strTransferUrl, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginVnPay, doCallback)
{
    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString strCallbackResp;
    buff.Read(strCallbackResp);
    CMyString strIp = getClientIp(pClient, req);
    LogInfo("Pay77-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    LogImpt("Pay77-pay resp, ip:[%s], data:[%s]", strIp.c_str(), strCallbackResp.c_str());
    
    // 校验回调IP
    CMyString strOrderNo = req.getParam("out_trade_no");
    CGame *pGame(NULL);
    int32_t ret = CPayWrapper::validCallbackIp(strOrderNo, strIp, pGame);
    if (ret < 0)
    {
        int32_t result = PAYFRAME_ERROR(err_callbackIpInvalid); // 回调的IP不合法
        CMyString strNote("pay77 callback ip [%s] invalid", strIp.c_str());
        CPayWrapper::OnPayFailure(result, strNote, szResp, cbResp);
        return result;
    }

    // 验签
    CSdkChannel *pChannel = CPayWrapper::on_get_order_sdkChannel(strOrderNo);
    if (NULL != pChannel)
    {
        CSdk *pSdk = pChannel->getSdk();
        if (NULL != pSdk)
        {
            http::CHttpRespMaker resp_data;
            resp_data.add("callback_time", req.getParam("callback_time"));
            resp_data.add("pid", req.getParam("pid"));
            resp_data.add("trade_no", req.getParam("trade_no"));
            resp_data.add("out_trade_no", req.getParam("out_trade_no"));
            resp_data.add("money", req.getParam("money"));
            resp_data.add("type", req.getParam("type"));
            CMyString str1 = resp_data.http_str_manual('&', '=', false, false); // req_data.http_str();

            CMyString str2("%s&%s", str1.c_str(), pSdk->getApiKey().c_str());
            CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
            strMd5.MakeLower();
            CMyString strMd5Valid = req.getParam("sign");
            if (strMd5 != strMd5Valid)
            {
                LogErr("pay77 resp sign [%s], yet verify sign is [%s]",
                        strMd5Valid.c_str(), strMd5.c_str());
                int32_t result = PAYFRAME_ERROR(err_SignMismatch);    // 验签失败
                CMyString strNote("pay77 callback sign mismatch");
                CPayWrapper::OnPayFailure(result, strNote, szResp, cbResp);
                return result;
            }
        }
    }

    // 填充返回结果数据
    CMyString strPlatformId = req.getParam("trade_no");
    CMyString status = req.getParam("status");
    int32_t code(payCallbackFail);
    if (status != "success")
    {
        code = payCallbackFail;
    }
    else
    {
        code = payCallbackOK;
    }

    CMyString strFee = req.getParam("money");
    CMyString strRealFee = req.getParam("money_real");
    double realFee = strRealFee.asDouble();
    int32_t nRealFee = realFee * 100; // 转换成分

    // 调用封装的通知函数
    ret = CPayWrapper::_deal_notify(strOrderNo, strPlatformId, 
            strCallbackResp, code, nRealFee);

    // 回传到回调方
    safe_strcpy(szResp, cbResp, "success");
    cbResp = strlen(szResp);
    return ret;
}


/*
ENTRY_FUNC_DECLARE(CPluginVnPay, doWithdraw)
{
    return 0;
}

ENTRY_FUNC_DECLARE(CPluginVnPay, doWithdrawCallback)
{
    return 0;
}
*/


int32_t CPluginVnPay::dealResp(http::CHttpRespone &http_resp, 
        CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo, 
        CSdkChannel *pChannel, int32_t payMoney)
{
/*
 * HEADER:
 * HTTP/1.1 200 OK
 * Server: nginx
 * Date: Fri, 08 Jan 2021 10:56:45 GMT
 * Content-Type: text/html;charset=utf-8
 * Transfer-Encoding: chunked
 * Connection: keep-alive
 * Vary: Accept-Encoding
 * Set-Cookie: PHPSESSID=ddh37ie0q5kgoqtvfblap4j7e5; path=/
 * Expires: Thu, 19 Nov 1981 08:52:00 GMT
 * Cache-Control: no-store, no-cache, must-revalidate
 * Pragma: no-cache
 * Refresh: 0;url=http://vi.long77.net/gateway/pay/automaticBank.do?id=2750920210108100575
 *
 * <若没有Refresh字段，则表示出错>
 *
 * DATA:
 * 请稍后..
 *
 * */    

    
    return 0;
}

/*
int32_t CPluginVnPay::dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &platformOrderNo, CSdkChannel *pChannel, int32_t payMoney)
{
    CGame *pGame = pChannel->getGame();
    CJsonWrapper reader;
    if (!reader.Init(strResp.c_str()))
    {
        CMyString strNote("Pay77支付调起结果异常, %s", strResp.c_str());
        CPayWrapper::postAlert(pGame, strNote);

        strError.Format("Pay77支付调用异常,请联系客服");
        // 返回串不合法
        LogErr("[%s:%s:%d]Pay77 resp [%s] not valid json string", __FILE__, __func__, __LINE__, strResp.c_str());
        return -1023;
    }

    int32_t status = reader.getInt("code");
    if (status != 1)
    {
        CMyString strMsg = reader.getString("msg");
        // STEP1: 发给IM
        CMyString strNote("Pay77支付调起失败,错误描述:%s", strMsg.c_str());
        CPayWrapper::postAlert(pChannel->getGame(), strNote);

        // STEP2: 发给前端
        CMyString strClientMsg;
        int32_t deal = CPayWrapper::dealRespError(pChannel, payMoney,strMsg, strClientMsg);
        strError.Format("[Pay77支付]%s", strClientMsg.c_str());

        // STEP4: 记录日志
        LogErr("[%s:%s:%d]Pay77-Pay call failure, code:%d, error:%s", __FILE__, __func__, __LINE__, status, strMsg.c_str());
        return -1025;
    }

    strTransferUrl = reader.getString("pay_url");
    platformOrderNo = reader.getString("orderid");

    return 0;
}
*/


void CPluginVnPay::AddPara(http::CHttpRespMaker& resp, std::map<CMyString, CMyString>& sortMap, const char* key, const char* value)
{
    CMyString skey = key;
    CMyString svalue = value;
    sortMap[skey] = svalue;

    resp.add(key, value);
}

void CPluginVnPay::AddPara(http::CHttpRespMaker& resp, std::map<CMyString, CMyString>& sortMap,const char* key, int64_t val)
{
    CMyString skey = key;
    CMyString sTmp("%ld", val);
    sortMap[skey] = sTmp;

    resp.add(key, val);
}

CMyString CPluginVnPay::GetSignSrc(std::map<CMyString, CMyString>& sortMap)
{
    CMyString sTmp;
    map<CMyString, CMyString>::iterator it =  sortMap.begin();
    for (; it != sortMap.end(); ++it)
    {
        sTmp.Append("%s", it->second.c_str());
    }

   
    return sTmp;
}

void CPluginVnPay::AddPara(http::CHttpRespMaker& resp, std::map<CMyString, CMyString>& sortMap,const char* key, CMyString& val)
{
    this->AddPara(resp, sortMap, key, val.c_str());
}