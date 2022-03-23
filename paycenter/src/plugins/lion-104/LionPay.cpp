/*
 * =====================================================================================
 *
 *       Filename:  LionPay.cpp
 *
 *    Description:  狮子会支付系统
 *
 *        Version:  1.0
 *        Created:  12/07/2019 09:31:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "LionPay.h"
#include "Security.h"
#include "Config.h"
#include "IClient.h"
#include <math.h>


CLionPay::CLionPay()
    : m_pDB(NULL)
{
    http::CHttpClient::GlobalInit();    
}

CLionPay::~CLionPay()
{
    SAFE_RELEASE(m_pDB);
    http::CHttpClient::GlobalFini();
}

int32_t CLionPay::Init(void)
{
    CXmlFile &xml = CONFIG.getXml();
    const char *pszSection = "main/lion";
    int32_t dbcfgId = xml.GetLong(pszSection, "dbId");
    int32_t recfgId = xml.GetLong(pszSection, "redisId");
    CMyString strIds = xml.GetString(pszSection, "ids");
    m_strCallbackUrl = xml.GetString(pszSection, "callback");
    m_strNotifyUrl = xml.GetString(pszSection, "notify");

    DbmsCfg mycfg;
    int32_t ret = CONFIG.getDbmsById(dbcfgId, mycfg);
    if (ret < 0)
    {
        LogFatal("can not load lion config data, Id=%d", dbcfgId);
        return -1;
    }

    m_pDB = CreateMysqlConn(mycfg.user.c_str(), mycfg.passwd.c_str(),
            mycfg.host.c_str(), mycfg.db.c_str(), mycfg.port,
            0, 0, "utf8mb4");
    if (NULL == m_pDB)
    {
        LogFatal("can not connect to lion database, id=%d", dbcfgId);
        return -3;
    }

    m_pDB->Cmd("SELECT id, keydata FROM tPayChannel WHERE id in (%s)", strIds.c_str());
    while(m_pDB->More())
    {
        Keydata kd;
        kd.id = m_pDB->GetInt(0);
        kd.keydata = m_pDB->GetString(1);
        m_vecKeyData.push_back(kd);
    }

    DbmsCfg recfg;
    ret = CONFIG.getDbmsById(recfgId, recfg);
    if (ret < 0)
    {
        LogFatal("can not open redis config, id =%d", recfgId);
        return -5;
    }

    bool val = m_redis.Open(recfg.host.c_str(), recfg.port, recfg.passwd.c_str(), 200);
    if (!val)
    {
        LogFatal("can not connect to redis server.");
        return -7;
    }
    m_redis.Select(StrToInt32(recfg.db.c_str()));

    return 0;
}

void CLionPay::Maintance(void)
{
    int32_t ret = m_pDB->Ping(300);
    if (ret == 1)
    {
        LogWarn("lion paycenter ping dbms");
    }
}

void CLionPay::OnPayFailure(int32_t code, const char *msg, char *szResp, uint32_t &cbResp)
{
    Json::Value resp;
    resp["code"] = 1;
    resp["message"] = CMyString("%s, error:%d", msg, code).c_str();
    Json::Value data;
    data["tradeNo"] = "N/A";
    data["url"] = "N/A";
    resp["data"] = data;

    CJsonWrapper writer;
    CMyString strResp = writer.write(resp);
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

void CLionPay::Rollback(uint32_t dbId, const CMyString &strNote)
{
    // 数据库记录失败
    /* 订单状态：   0 - 创建订单
     *              1 - 支付成功(支付SDK回调)
     *              2 - 支付失败(支付SDK回调)
     *              3 - 支付取消(支付SDK回调)
     *              4 - 退款(支付SDK操作)
     *              5 - 发货成功
     *              6 - 下单成功(发送到支付SDK平台成功)
     *              7 - 下单失败(发送到支付SDK平台失败)
     *   0 -> 6 -> 1/2/3 -> 5
     *     |  |
     *     V  |
     *     7  |
     *        V
     *        7
     * */
    m_pDB->Cmd("call pOrderResult(%u, %d, 'N/A', '%s', 0 )", 
            dbId, 7, strNote.c_str());
    while(m_pDB->More());
    LogFatal("payorder %u failure, result: %s", dbId, strNote.c_str());
}

void CLionPay::OnSendOK(uint32_t dbId, const CMyString &strNote)
{
    m_pDB->Cmd("call pOrderResult(%u, 6, 'N/A', '%s', 0 )", 
            dbId, strNote.c_str());
    while(m_pDB->More());
    LogImpt("payorder %u send succeed", dbId);
}

ENTRY_FUNC_DECLARE(CLionPay, doLionPay)
{
    ext_headers["Content-Type"] = "application/json";

    int32_t nId(0);
    bool ret = req.getParam("paycenter_Id", nId);
    if (!ret)
    {
        // 构建出错返回
        this->OnPayFailure(-1001, "不支持的支付渠道", szResp, cbResp);
        return -1001;
    }

    CMyString strKeyData;
    VEC_KEYDATA::iterator it(m_vecKeyData.begin());
    for(; it != m_vecKeyData.end(); ++it)
    {
        Keydata &kd = *it;
        if (kd.id == nId)
        {
            strKeyData = kd.keydata;
            break;
        }
    }
    if (strKeyData.empty())
    {
        m_pDB->Cmd("SELECT keydata FROM tPayChannel WHERE id=%d", nId);
        while(m_pDB->More())
        {
            strKeyData = m_pDB->GetString(0);
        }
    }
    if (strKeyData.empty())
    {
        // 没有配置渠道
        this->OnPayFailure(-1003, "未配置支付渠道的参数", szResp, cbResp);
        return -1003;
    }

    CJsonWrapper reader;
    if (!reader.Init(strKeyData.c_str()))
    {
        // keydata 数据不合法
        this->OnPayFailure(-1005, "支付渠道的参数错误", szResp, cbResp);
        return -1005;
    }
    
    CMyString strGameOrder;
    CMyString strProductId;
    CMyString strRemark;
    int32_t payMoney(0);
    req.getParam("billNo", strGameOrder);
    req.getParam("amount", payMoney);
    req.getParam("productId", strProductId);
    req.getParam("Remark", strRemark);
    CMyString strSDKId = CMyString("%d", nId);
    CMyString strPayStyle = req.getParam("paycenter_PayStyle");
    CMyString strUin = req.getParam("uin");
    // TODO: 此处不能为int_32, 必须转成CMyString
    int32_t nGameId = req.getParamInt("gameCode");

    // 写入数据库
    CMyString strOrderNo;
    int32_t nOrderId(0);
    char szError[512] = "";
    try
    {
        m_pDB->Cmd("call pNewOrder('%s', %d, %d, '%s', %d, '%s', '%s', '%s', %d, '%s', '%s', '%s')",
                strUin.c_str(),
                nGameId,
                StrToInt32(strSDKId.c_str()),
                strGameOrder.c_str(),
                payMoney,
                m_strNotifyUrl.c_str(),
                strProductId.c_str(),
                req.getParamStr("playerIp"),
                req.getParamInt("configId"),
                strPayStyle.c_str(),
                req.getIp(),
                req.getParamStr("clientType"));
        while(m_pDB->More())
        {
            nOrderId = m_pDB->GetInt(0);
            strOrderNo = m_pDB->GetString(1);
        }
    }
    catch(...)
    {
        int32_t errNo = m_pDB->GetLastError(szError, sizeof(szError));
        if (errNo == 0)
        {
            safe_strcpy(szError, sizeof(szError), "数据库系统错误");
        }
        this->OnPayFailure(-1007, szError, szResp, cbResp);
        return -1007;
    }
    int32_t errNo = m_pDB->GetLastError(szError, sizeof(szError));
    if (errNo != 0)
    {
        this->OnPayFailure(-1009, szError, szResp, cbResp);
        return -1009;
    }
    if (strOrderNo.empty())
    {
        this->OnPayFailure(-1011, "支付平台数据存储失败", szResp, cbResp);
        return -1011;
    }

    // 写入到redis缓存中
    CJsonWrapper respone_data;
    respone_data.Set("code", 0);
    respone_data.Set("uin", strUin);
    respone_data.Set("gameCode", nGameId);
    respone_data.Set("tradeNo", strOrderNo);
    respone_data.Set("outTradeNo", "");
    respone_data.Set("totalFee", payMoney);
    respone_data.Set("product", strProductId);
    respone_data.Set("tradeStatus", 0);
    CMyString strRand("%lu", RandGet());
    respone_data.Set("nonceStr", strRand);
    m_redis.Set(strOrderNo.c_str(), respone_data.c_str(), 600, 1);

    // 生成发到支付平台的数据
    http::CHttpRespMaker request_data;
    request_data.add("merchant", reader.getString("accountId"));
    request_data.add("qrtype", reader.getString("bank_code"));
    request_data.add("customno", strOrderNo); 
    request_data.add("money", CMyString("%.2f", payMoney/100.0));
    request_data.add("sendtime", CMyString("%lu", time(NULL)));
    request_data.add("notifyurl", m_strCallbackUrl); 
    request_data.add("backurl", reader.getString("return_url"));
    request_data.add("risklevel", "1");
    CMyString strSign = request_data.signWithMd5NoSort("sign", true, reader.getString("accountKey").c_str());
    LogDebug("DBGPAY>sign str [%s]", strSign.c_str());

    // 发送到支付平台
    CMyString strResp;
    CMyString strContentType("Content-Type:application/x-www-form-urlencoded");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    CMyString strUrl = reader.getString("url");
    header_list.push_back("Content-Type:application/x-www-form-urlencoded");
    LogDebug("DBGPAY>paycenter url [%s]", strUrl.c_str());
    CMyString strHeader;
    ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, header_list, strSign.c_str());
    if (ret < 0)
    {
        // 远程连接失败
        this->OnPayFailure(-1013, "Lion支付后台不可到达", szResp, cbResp);
        this->Rollback(nOrderId, "Lion支付后台不可到达");
        return -1013;
    }
    LogDebug("Lion resp [%s]", strResp.c_str());

/*
 * TODO: 判断是否接收成功
    // 解析strResp
    CJsonWrapper reader_resp;
    if (!reader_resp.Init(strResp.c_str()))
    {
        // 返回串不合法
        this->OnPayFailure(-1009, "支付后台返回的结果不合法", szResp, cbResp);
        return -1009;
    }
*/
    std::string::size_type pos = strResp.find("alert(\'");
    if (pos != std::string::npos)
    {
        CMyString strTemp = strResp.substr(pos+7);
        std::string::size_type pos2 = strTemp.find("\'");
        CMyString strNote = strTemp.substr(0, pos2);
        LogDebug("DBGPAY>pay resp note [%s]", strNote.c_str());
        this->OnPayFailure(-1015, strNote.c_str(), szResp, cbResp);
        this->Rollback(nOrderId, strNote);
        return -1015;
    }

    this->OnSendOK(nOrderId, "向Lion支付后台请求成功");

    // 记录支付的主键
    CMyString strOrderNoKey("key#%s", strOrderNo.c_str());
    int64_t keyValue = nOrderId;
    m_redis.Set(strOrderNoKey.c_str(), keyValue, 600, 1);

    // 返回给调用端
    Json::Value resp;
    resp["code"] = 0;
    //resp["message"] = reader_resp.getString("message").c_str();
    resp["message"] = "Send OK";
    Json::Value data;
    data["tradeNo"] = "";//reader_data.getString("order_no").c_str();
    data["url"] = ""; //reader_data.getString("redirect_url").c_str();
    CJsonWrapper writer;
    strResp = writer.write(resp);
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);

    return 0;
}

ENTRY_FUNC_DECLARE(CLionPay, doLionPayCallback)
{
    // 保存post数据
    CByteStream buff;
    req.getPost(buff);
    CMyString reqbuff;
    buff.Read(reqbuff);
    LogInfo("lion resp: [%s]", reqbuff.c_str());

    // 获取预先存储的内容
    CMyString strOrderNo = req.getParam("customno");
    CMyString strNotify;
    int32_t ret = m_redis.Get(strOrderNo, strNotify);
    if (ret < 0)
    {
        // 出错，有可能是超时
        LogFatal("lion orderNo %s invalid or time expired.", strOrderNo.c_str());
        safe_strcpy(szResp, cbResp, "{\"code\":-2001, \"message\":\"time expired\"}");
        cbResp = strlen(szResp);
        return -2001;
    }
    m_redis.Del(strOrderNo);

    CMyString strKey("key#%s", strOrderNo.c_str());
    int64_t keyId(0);
    ret = m_redis.Get(strKey, keyId);
    if (ret < 0)
    {
        LogFatal("lion orderNo %s invalid or time expired.", strOrderNo.c_str());
        safe_strcpy(szResp, cbResp, "{\"code\":-2003, \"message\":\"time expired\"}");
        cbResp = strlen(szResp);
        return -2003;
    }
    m_redis.Del(strKey);

    // TODO: 校验sign 是否合法

    // 返回给支付SDK方
    safe_strcpy(szResp, cbResp, "OK");
    cbResp = strlen(szResp);

    int32_t nResult = 1;
    // TODO: 判断nResult 的值： 1/2/3

    // 写入到数据库中
    CMyString strPlatformId = req.getParam("orderno");
    double money(0.00);
    req.getParam("money", money);
    int32_t realFee = (int32_t)ceil(money*100);
    try
    {
        m_pDB->Cmd("call pOrderResult(%u, %d, '%s', '%s', %d)",
                keyId,
                nResult,
                strPlatformId.c_str(),
                reqbuff.c_str(),
                realFee);
        while(m_pDB->More());
    }
    catch(...)
    {

    }
    // TODO:
    if (ret < 0)
    {
        LogFatal("save respone result to db failure.");
    }
   
    // 填充返回结果数据
    CJsonWrapper resp_data;
    resp_data.AddJsonStr(strNotify.c_str());
    resp_data.Set("outTradeNo", strPlatformId );
    int32_t result = req.getParamInt("state");
    if (result != 1)
    {
        resp_data.Set("code", -1);
        resp_data.Set("tradeStatus", 2);
    }
    else
    {
        resp_data.Set("code", 0);
        resp_data.Set("tradeStatus", 1);
    }

    // 生成Sign值
    http::CHttpRespMaker resp_maker;
    resp_maker.AddJsonStr(strNotify.c_str());
    CMyString pending_sign_str = resp_maker.http_str();
    CMyString strSign = CMD5Wrapper::encode(pending_sign_str.c_str());
    strSign.MakeLower();
    resp_data.Set("sign", strSign);
    CMyString strNotifyWithSign = resp_data.c_str();

    // 发送Http请求
    CMyString strResp;
    CMyString strContentType("Content-Type:application/json");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    CMyString strHeader;
    ret = m_http.HttpPostWithHeaders(m_strNotifyUrl.c_str(), strResp, strHeader, header_list, strNotifyWithSign.c_str() );
    if (ret < 0)
    {
        // 远程连接失败
        LogFatal("notify pay result failure, maybe notify url invalid");
        return -1007;
    }

    LogDebug("orderNo %s notify succeed.", strOrderNo.c_str());

    return 0;
}

ENTRY_FUNC_DECLARE(CLionPay, doLionPayQuery)
{
    return 0;
}

ENTRY_FUNC_DECLARE(CLionPay, doLionPayEcho)
{
    safe_strcpy(szResp, cbResp, "lion pay plugin echo.");
    cbResp = strlen(szResp);

    return 0;
}

