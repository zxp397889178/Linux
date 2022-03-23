/*
 * =====================================================================================
 *
 *       Filename:  richcw.cpp
 *
 *    Description:  极速支付的SDK
 *
 *        Version:  1.0
 *        Created:  12/30/2019 04:50:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "richcw.h"
#include "Config.h"
#include "IClient.h"
#include "Security.h"

CPluginRichcw::CPluginRichcw()
    : CPayWrapper()
{
}

CPluginRichcw::~CPluginRichcw()
{
}

int32_t CPluginRichcw::Init(void)
{
    CMyString strSect("main/richcw");

    int32_t ret = CPayWrapper::Init(strSect);
    if (ret < 0)
    {
        return ret;
    }

    return 0;
}

void CPluginRichcw::Maintance(void)
{
    CPayWrapper::Maintance(); 
}

ENTRY_FUNC_DECLARE(CPluginRichcw, doRichcwPay)
{
    int32_t nOrderId(0);
    CMyString strOrderNo;
    CSdkChannel *pChannel(NULL);
    int32_t ret = _before_send_request(req, pChannel, nOrderId, strOrderNo, szResp, cbResp);
    if (ret < 0)
    {
        return ret;
    }
    int32_t payMoney(0);
    req.getParam("amount", payMoney);

    CMyString strIp = req.getParam("playerIp");
    CSdk *pSdk = pChannel->getSdk();
    CGame *pGame = pChannel->getGame();
////////<<<<<<<<<<<<<<<<<<<< 以上为公共的调用 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    // 生成发送到支付平台的数据
    http::CHttpRespMaker request_data;
    request_data.add("version", "1.0");
    request_data.add("merId", pSdk->getAccount());
    request_data.add("orderId", strOrderNo);
    request_data.add("totalMoney", CMyString("%d", payMoney));
    request_data.add("tradeType", pChannel->getBankCode());
    request_data.add("descript", "");
    CMyString strNotifyUrl = pSdk->getPayCallbackUrl();
    request_data.add("notify", strNotifyUrl);
    request_data.add("redirectUrl", "");
    request_data.add("remark", "");
    request_data.add("formtype", "wap");
    request_data.add("cardNo", "");
    request_data.add("bankCode", "");
    //request_data.add("sign", "");
    request_data.add("user_ip", req.getIp());
    CMyString strData("%s&%s", 
            request_data.http_str().c_str(), 
            pSdk->getApiKey().c_str());
    CMyString md5 = CMD5Wrapper::encode(strData.c_str());
    md5.MakeUpper();
    request_data.add("sign", md5);
    strData = request_data.json_str();

    // TODO: 需要重签名
    CMyString strSign;

    // 发送到支付平台
    CMyString strResp;
    CMyString strContentType("Content-Type:application/json");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    CMyString strHeader;
    ret = m_http.HttpPostWithHeaders(pSdk->getPayUrl().c_str(), 
            strResp, strHeader, header_list, strSign.c_str());
    if (ret != 0)
    {
        // 远程连接失败
        this->OnPayFailure(-1007, "综合支付后台不可到达", szResp, cbResp);
        return -1007;
    }
    LogDebug("richcw resp [%s]", strResp.c_str());

    // 解析strResp
    CJsonWrapper reader_resp;
    if (!reader_resp.Init(strResp.c_str()))
    {
        // 返回串不合法
        this->OnPayFailure(-1009, "支付后台返回的结果不合法", szResp, cbResp);
        return -1009;
    }
    int32_t status = reader_resp.getInt("code");
    if (status != 0)
    {
        // 支付失败
        CMyString strErr = reader_resp.getString("errMsg");
        this->OnPayFailure(-1011, strErr.c_str(), szResp, cbResp);
        return -1011;
    }
    CJsonWrapper reader_data;
    ret = reader_resp.getReader("object", reader_data);
    if (!ret)
    {
        this->OnPayFailure(-1013, "支付调起的结果数据失败", szResp, cbResp);
        return -1013;
    }

    // TODO: 在失败的情况，如何响应到数据库
/*    
    ////////////// 以下为后续的调用
    // 记录支付的主键
    CMyString strOrderNoKey("key#%s", strOrderNo.c_str());
    int64_t keyValue = nOrderId;
    m_redis.Set(strOrderNoKey.c_str(), keyValue, 600, 1);

    // 返回给调用端
    Json::Value resp;
    resp["code"] = 0;
    resp["message"] = reader_resp.getString("errMsg").c_str();
    Json::Value data;
    data["tradeNo"] = ""; //reader_data.getString("order_no").c_str();
    data["url"] = reader_data.getString("payUrl").c_str();
    CJsonWrapper writer;
    strResp = writer.write(resp);
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
*/

    // 记录支付的主键
    CPayWrapper::on_save_orderId(strOrderNo, nOrderId);

    // 返回给调用端
    CPayWrapper::OnPayResp(0, "explain url manual", strOrderNo,
            strResp, szResp, cbResp,
            pChannel->getSdkCode(), pChannel->getBankCode(), pChannel->getMethodCode());
    return 0;
}



