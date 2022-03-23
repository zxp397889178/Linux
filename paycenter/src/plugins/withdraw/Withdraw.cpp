/*
 * =====================================================================================
 *
 *       Filename:  Withdraw.cpp
 *
 *    Description:  提现接口实现
 *
 *        Version:  1.0
 *        Created:  2021年01月09日 12时03分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Withdraw.h"
#include "Security.h"
#include <algorithm>
#include "Common.h"
#include "pay_error.h"
#include "../PayBase.h"
#include "UserCreditList.h"
#include "MyUtility.h"

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
static int32_t CheckMysqlErrorEx(ISimDB *pDB, Json::Value &resp, char *szResp, uint32_t &cbResp)
{
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        CMyString strMsg("mysql errno:%d, error:%s", err, szError);
        resp["code"] = -1;
        resp["message"] = strMsg;
        LogFatal(strMsg.c_str());
        return -1;
    }
    return 0;
}
#define CHECK_MYSQL_ERROR(err)      int32_t (err) = CheckMysqlError(m_pDB, szResp, cbResp); if (err!=0) { return -1; }
static int32_t getGamesvrResult(int32_t status)
{
    // status => t_withdraw.status 字段的状态序号
    // result => 返回给游戏服务器的状态: 
    //  0 表示状态未结束，需要后处理，不该返回给游服
    // -1 表示状态错误
    // 1/2/8/9 状态正确
    static int32_t s_result[] = {0, 9, 8, 0, 0, 1, 9, 8, -1, 2, 9, 8, 9};

    if (status<=0 || status>=(int32_t)(sizeof(s_result)/sizeof(int32_t)) )
    {
        return -1;
    }
    return s_result[status-1];
}

//////////////////////////////////////////////////////////////////////////
//
CPluginWithdraw::CPluginWithdraw()
    : m_pDB(NULL)
{
    http::CHttpClient::GlobalInit();
}

CPluginWithdraw::~CPluginWithdraw()
{
    SAFE_RELEASE(m_pDB);
    http::CHttpClient::GlobalFini();
}

int32_t CPluginWithdraw::Init(void)
{
    int32_t result(0);
    int32_t line(0);
    do {
        try {
            // 获得数据库指针
            const char *sectDB = "main/paycenter/db";
            SAFE_RELEASE(m_pDB);
            int32_t ret = open_mysql(sectDB, "main", m_pDB);
            if (ret != err_Success ) { result = ret; line=__LINE__; break; }
            
            m_redis.Close();
            ret = open_redis(sectDB, "redis", m_redis);
            if (ret != err_Success) { result = ret; line=__LINE__; break; }

        }
        catch(CException &e)
        {
            LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, e.what());
            return -1;
        }
        catch(...)
        {
            LogErr("[%s:%s:%d]call Init failure.", __FILE__, __func__, __LINE__);
            return -1;
        }
    }while(0);

    if (result != err_Success)
    {
        LogErr("[%s:%s:%d]init sdk pay channel data failure.", __FILE__, __func__, line);
        return result;
    }
    LogInit("init withdraw ok");

    CXmlFile &xml = CONFIG.getXml();
    if (!this->InitRedisCache(m_cacheCreditReq, m_redis, xml, "main/paycenter/credit/token"))
    {
        return -3;
    }

    //机器人告警日志初始化
    CAlertLog::Instance().Init(CONFIG.getXml());

    return 0;
}

int32_t CPluginWithdraw::Update(void)
{
    return this->Init();
}

void CPluginWithdraw::Maintance(void)
{
    if (m_pDB && m_pDB->Ping(300)>0)
    {
        LogDebug("withdraw main db ping.");
    }

    if (m_redis.Ping(600)>0)
    {
        LogDebug("withdraw main redis ping.");
    }
}

ENTRY_FUNC_DECLARE(CPluginWithdraw, doWithdrawRequest)
{
    ext_headers["Content-Type"] = "application/json";
    CGame*  pGame(NULL);
    CMyString   strMsg;

    bool signEnable(false);
    CMyString signField;
    this->getSignInfo("main/paycenter/sign/withdraw", signEnable, signField);
    // STEP1: 检查参数是否合法
    std::vector<CMyString> vecParam;
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    //vecParam.push_back("bankCode");
    //vecParam.push_back("bankName");
    vecParam.push_back("bankCard");
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
    CMyString strClientIp = getClientIpEx(getIpFromHeader, pClient, req, "X-Real-IP");
    LogDebug("client Ip:%s, server send ip:%s", strClientIp.c_str(), strIp.c_str());
    pClient->setIp(strIp);
    int64_t createTime = req.getParamInt64("createTime");
    CMyString gameOrderNo = req.getParam("gameOrderNo");
    int64_t tax = req.getParamInt64("tax");

    //根据提现配置判断是否可以提现
    code = this->checkCanWithdraw(gameCode, strUin, fee, tax, szResp, cbResp);
    if (code != 0)
    {
        return code;
    }

    // STEP2: 查询用户是否符合提现标准
    CMyString strWithdreaId;
    Json::Value resp;
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

    //进入待审核状态就要增加总提现额
    USERCREDIT_LIST.AddWithdraw(m_pDB, gameCode, strUin, (fee+tax)*100);

    // 写入提现日志数据库-创建订单
    int32_t result = 1;
    this->setWithdrawLog(strWithdreaId, result, strUin, "", resp["message"].asString(), createTime);

    // STEP3: 返回提现结果
    dealResp(resp, szResp, cbResp);

    return err_HttpOK;
}



ENTRY_FUNC_DECLARE(CPluginWithdraw, doWithdrawAudit)
{
    ext_headers["Content-Type"] = "application/json";
    CGame*  pGame(NULL);
    CMyString   strMsg;

    bool signEnable(false);
    CMyString signField;
    this->getSignInfo("main/paycenter/sign/report", signEnable, signField);
    // STEP1: 检查参数是否合法
    std::vector<CMyString> vecParam;
    vecParam.push_back("orderNo");
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    vecParam.push_back("fee");
    vecParam.push_back("result");
    vecParam.push_back("operator");
    int32_t code = this->valid_req(req, szResp, cbResp, vecParam, 
            signEnable, signField, true, "gameCode", pGame);
    if (code != 0)
    {
        return code;
    }

    // STEP2: 检查发起的IP是否合法
    CMyString strClientIp = getClientIpEx(getIpFromHeader, pClient, req, "X-Real-IP");
    if (!this->ValidIp(strClientIp, "main/paycenter/ip/report"))
    {
        Json::Value resp;
        resp["code"] = -1;
        resp["message"] = CMyString("ip %s 不合法", strClientIp.c_str());
        dealResp(resp, szResp, cbResp);
        return -1;
    }
    
    CMyString gameCode = req.getParam("gameCode");
    CMyString orderNo = req.getParam("orderNo");
    CMyString strUin = req.getParam("uin");
    int64_t fee = req.getParamInt64("fee");
    int32_t status = req.getParamInt("result");
    CMyString auditRemark = req.getParam("remark");
    CMyString auditOperator = req.getParam("operator");
    int64_t auditTime = req.getParamInt64("auditTime");
    int32_t gameResult = getGamesvrResult(status);
    if (gameResult<0)
    {
        Json::Value resp;
        resp["code"] = -3;
        resp["message"] = CMyString("status %d invalid", status);
        dealResp(resp, szResp, cbResp);
        return -3;
    }

    // STEP3: 通知游戏服务器
    int32_t nRet = err_HttpOK;
    CMyString strError;
    if (gameResult >0)
    {
        CMyString gameOrderNo;
        m_pDB->Cmd("SELECT `third_order` from `t_withdraw` where `order_id`='%s';", orderNo.c_str());
        while(m_pDB->More()) 
        {
            gameOrderNo = m_pDB->GetString(0);
        }
        nRet = this->notifyGamesvr(gameCode, strUin, orderNo, gameOrderNo, 
            fee, 1, gameResult, pGame, strError);
    }

    if (nRet != err_HttpOK)
    {
        Json::Value resp;
        resp["code"] = nRet;
        resp["message"] = strError;
        dealResp(resp, szResp, cbResp);

        // 写入提现日志数据库-审核订单
        this->setWithdrawLog(orderNo, status, auditOperator, auditRemark, strError, auditTime);
        return nRet;
    }

    // STEP4: 写入到数据库
    int32_t retval(-1);
    Json::Value resp;
    resp["code"] = -1000;
    resp["message"] = "store to database failure";
    m_pDB->Cmd("call pWithdrawAudit('%s', %d, '%s', %ld, '%s')",
            orderNo.c_str(), status, auditRemark.c_str(), auditTime, auditOperator.c_str());
    while(m_pDB->More())
    {
        retval= m_pDB->GetInt(0);
        resp["code"] = retval;
        resp["message"] = m_pDB->GetString(1);
    }
    CHECK_MYSQL_ERROR(err);

    //审核拒绝或冻结金币，退回总提现额
    if(status == 2 || status == 3)
    {
        USERCREDIT_LIST.AddWithdraw(m_pDB, gameCode, strUin, 0-fee);
    }

    dealResp(resp, szResp, cbResp);
    // 写入提现日志数据库-审核订单
    this->setWithdrawLog(orderNo, status, auditOperator, auditRemark, resp["message"].asString(), auditTime);

    if (retval != 0)
    {
        return -1;
    }

    return err_HttpOK;
}

ENTRY_FUNC_DECLARE(CPluginWithdraw, doWithdrawCredit)
{
    ext_headers["Content-Type"] = "application/json";
    CGame*  pGame(NULL);
    CMyString   strMsg;

    bool signEnable(false);
    CMyString signField;
    this->getSignInfo("main/paycenter/sign/withdraw", signEnable, signField);
    // STEP1: 检查参数是否合法
    std::vector<CMyString> vecParam;
    vecParam.push_back("orderNo");
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    vecParam.push_back("fee");
    vecParam.push_back("result");
    vecParam.push_back("operator");
    int32_t code = this->valid_req(req, szResp, cbResp, vecParam, 
            signEnable, signField, true, "gameCode", pGame);
    if (code != 0)
    {
        return code;
    }

    // STEP2: 检查发起的IP是否合法
    CMyString strClientIp = getClientIpEx(getIpFromHeader, pClient, req, "X-Real-IP");
    if (!this->ValidIp(strClientIp, "main/paycenter/ip/report"))
    {
        Json::Value resp;
        resp["code"] = -1;
        resp["message"] = CMyString("ip %s 不合法", strClientIp.c_str());
        dealResp(resp, szResp, cbResp);
        return -1;
    }
    
    CMyString gameCode = req.getParam("gameCode");
    CMyString orderNo = req.getParam("orderNo");
    CMyString strUin = req.getParam("uin");
    int64_t fee = req.getParamInt64("fee");
    int32_t status = req.getParamInt("result");
    CMyString sdkCode = req.getParam("sdkCode");
    CMyString creditRemark = req.getParam("remark");
    CMyString creditOperator = req.getParam("operator");
    int64_t creditTime = req.getParamInt64("creditTime");
    int32_t gameResult = getGamesvrResult(status);
    if (gameResult<0)
    {
        Json::Value resp;
        resp["code"] = -3;
        resp["message"] = CMyString("status %d invalid", status);
        dealResp(resp, szResp, cbResp);
        return -3;
    }

    if (5==status)
    {
        // 获得所需要的数据，通知后台管理再次操作
        this->postNextCredit(gameCode, sdkCode, orderNo, creditRemark, creditOperator, creditTime, szResp, cbResp);
        return err_HttpOK; 
    }

    // STEP3: 通知游戏服务器
    int32_t nRet = err_HttpOK;
    CMyString strError;
    if (gameResult > 0)
    {
        CMyString gameOrderNo;
        m_pDB->Cmd("SELECT `third_order` from `t_withdraw` where `order_id`='%s';", orderNo.c_str());
        while(m_pDB->More()) 
        {
            gameOrderNo = m_pDB->GetString(0);
        }
        nRet = this->notifyGamesvr(gameCode, strUin, orderNo, gameOrderNo, 
            fee, 2, gameResult, pGame, strError);
    }

    if (nRet != err_HttpOK)
    {
        Json::Value resp;
        resp["code"] = nRet;
        resp["message"] = strError;
        dealResp(resp, szResp, cbResp);

        // 写入提现日志数据库-审核订单
        this->setWithdrawLog(orderNo, status, creditOperator, creditRemark, strError, creditTime);
        return nRet;
    }

    // STEP4: 写入到数据库
    int32_t retval(-1);
    Json::Value resp;
    Json::Value resp_data;
    resp["code"] = -1000;
    resp["message"] = "store to database failure";
    m_pDB->Cmd("call pWithdrawCredit('%s', %d, '%s', %ld, '%s', '%s')",
            orderNo.c_str(), status, creditRemark.c_str(), creditTime, creditOperator.c_str(), sdkCode.c_str());
    while(m_pDB->More())
    {
        retval = m_pDB->GetInt(0);
        resp["code"] = retval;
        resp["message"] = m_pDB->GetString(1);
    }
    CHECK_MYSQL_ERROR(err);
    resp["data"] = resp_data;

    //放款审核拒绝原路返回或放款审核拒绝并冻结资金
    if(status == 7 || status == 8)
    {
        USERCREDIT_LIST.AddWithdraw(m_pDB, gameCode, strUin, 0-fee);
    }

    dealResp(resp, szResp, cbResp);
    // 写入提现日志数据库-审核订单
    this->setWithdrawLog(orderNo, status, creditOperator, creditRemark, resp["message"].asString(), creditTime);
    if (retval != 0)
    {
        return -1;
    }

    return err_HttpOK;
}

int32_t CPluginWithdraw::valid_req(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::vector<CMyString> &vecParam, bool bSign, const CMyString &signField, bool bSignFullParam, const CMyString &gameCodeField, CGame *&pGame)
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
    pGame = this->getGame(gameCode);
    if (NULL == pGame)
    {
        strMsg.Format("无此游服代码 %s", gameCode.c_str());
        build_error_msg(PAYFRAME_ERROR(err_InvalidGamesvr), strMsg, gameCode, szResp, cbResp);
        return err_HttpOK;
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

CGame* CPluginWithdraw::getGame(const CMyString &gameCode)
{
    return GAME_LIST.FindByCode(gameCode);
}


void CPluginWithdraw::build_error_msg(int32_t code, const CMyString &errMsg, const CMyString &gameCode, char *szResp, uint32_t &cbResp)
{
    Json::Value resp;
    resp["code"] = code;
    resp["message"] = errMsg;
    resp["gameCode"] = gameCode;
    Json::Value data_arr;
    data_arr.resize(0);
    resp["data"] = data_arr;

    CJsonWrapper writer;
    CMyString strResp = writer.write(resp);
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

int32_t CPluginWithdraw::postAlert(CGame *pGame, const CMyString &strText)
{
    if (NULL == pGame)
    {
        LogErr("pGame is NULL when postAlert msg:%s", strText.c_str());
        return -1;
    }
    
    // 获得警告的ID
    CJsonWrapper alert;
    pGame->getAlter(alert);
    int32_t alertId = alert.getInt("alertId");
    if (alertId <= 0)
    {
        return 0;
    }
    
    // 获得警告的URL
    CMyString alertUrl = alert.getString("url");
    
    // 发起警告
    http::CHttpClient net;
    CMyString strData("{\"chatId\":%d,\"text\":\"%s\"}", alertId, strText.c_str());
    http::VEC_STRING headers;
    headers.push_back("Content-Type: application/json");
    CMyString strHeader;
    CMyString strResp;
    int32_t ret = net.HttpPostWithHeaders(alertUrl.c_str(), strResp, strHeader, headers, strData.c_str());
    if (ret ==0)
    {
        // success
        return 1;
    }

    CMyString strError;
    net.getError(ret, strError);
    LogErr("call postAlert msg [%s] failure, errno:%d, error:%s", 
            strText.c_str(), ret, strError.c_str());
    return -1;
}

int32_t CPluginWithdraw::notifyGamesvr(const CMyString &gameCode, const CMyString &strUin, 
        const CMyString &orderNo, const CMyString &gameOrderNo, int64_t fee, 
        int32_t step, int32_t result, CGame *pGame, CMyString& strError)
{
    http::CHttpRespMaker resp_maker;
    resp_maker.add("gameCode", gameCode);
    resp_maker.add("uin", strUin);
    resp_maker.add("orderNo", orderNo);
    resp_maker.add("gameOrderNo", gameOrderNo);
    resp_maker.add("fee", fee);
    resp_maker.add("step", step);
    resp_maker.add("result", result);
    CMyString str1 = resp_maker.http_str();
    CMyString str2("%s&key=%s", str1.c_str(), pGame->getApiKey().c_str());
    CMyString strMd5 = CMD5Wrapper::encode(str2.c_str());
    strMd5.MakeLower();
    resp_maker.add("sign", strMd5);
    CMyString strSign = resp_maker.json_str();

    LogDebug("before key : [%s]", str1.c_str());
    LogDebug("add signkey: [%s]", str2.c_str());
    LogDebug("md5:         [%s]", strMd5.c_str());
    CMyString strUrl = pGame->getApiTranferUrl();
    LogDebug("url:         [%s]", strUrl.c_str());
    LogDebug("data:        [%s]", strSign.c_str());

    // 发送Http请求
    CMyString strNotifyResp;
    CMyString strContentType("Content-Type:application/json");
    std::vector<std::string> header_list;
    header_list.push_back(strContentType.c_str());
    CMyString strHeader;
    int32_t ret = m_http.HttpPostWithHeaders(strUrl.c_str(), strNotifyResp, strHeader, header_list, strSign.c_str() );
    if (ret != 0)
    {
        CMyString strErr;
        m_http.getError(ret, strErr);
        CMyString strNote("用户(uin=%s)提现审核%ld成功，通知游服失败，请安排人工操作，错误信息:%s", strUin.c_str(), fee/100, strErr.c_str()); 
        this->postAlert(pGame, strNote);

        // 远程连接失败
        LogFatal("[%s:%s:%d]uin %s withdraw-audit %ld succeed, but write to gamesvr failure, orderNo:%s, url:%s, error:%s",
                __FILE__, __func__, __LINE__, strUin.c_str(), fee/100,
                orderNo.c_str(), strUrl.c_str(), strErr.c_str());

        strError = strErr;
        return err_PayDataSendtoGameFailure;
    }
    return err_HttpOK;
}

void CPluginWithdraw::postNextCredit(const CMyString &gameCode, const CMyString &sdkCode, const CMyString &orderNo, const CMyString &remark, const CMyString &creditOp, int64_t creditTime, char *szResp, uint32_t &cbResp)
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
            resp_data["creditRemark"] = remark;
            resp_data["creditTime"] = (Json::Int64)creditTime;
            resp_data["operator"] = creditOp;
        }
    }
    resp["data"] = resp_data;
    int32_t err = CheckMysqlErrorEx(m_pDB, resp, szResp, cbResp);
    if (err == 0)
    {
        dealResp(resp, szResp, cbResp);
    }

    // 将TOKEN记录入redis
    m_cacheCreditReq.Push(orderNo, resp_data);
}

bool CPluginWithdraw::ValidIp(const CMyString &strIp, const CMyString &sect)
{
    CXmlFile &xml = CONFIG.getXml();
    bool bEnable = xml.GetBool("main/paycenter/ip", "enable");
    if (!bEnable)
    {
        // 禁用IP检查，所有IP都合法
        return true;
    }

    CMyString strIps = xml.GetString(sect.c_str(), "valid");
    CStrSpliter css;
    css.Split(strIps, ',');
    if (css.empty())
    {
        // 未配置允许IP，所有IP都合法
        return true;
    }

    bool bValid(false);
    for(int32_t i(0); i<css.size(); ++i)
    {
        CMyString strIp1 = css.asString(i);
        if (strIp1=="0.0.0.0")
        {
            // 放过所有IP
            bValid = true;
            break;
        }
        if (strIp1 == strIp)
        {
            // IP匹配
            bValid = true;
            break;
        }
    }
    return bValid;
}

void CPluginWithdraw::getSignInfo(const CMyString &sect, bool &bEnable, CMyString &field)
{
    CXmlFile &xml = CONFIG.getXml();
    bEnable = xml.GetBool(sect.c_str(), "enable");
    field = xml.GetString(sect.c_str(), "field");
}


bool CPluginWithdraw::InitRedisCache(redis_cache::CFmtCache &cache, CMyRedis &redis, CXmlFile &xml, const CMyString &sect)
{
    CMyString strFormat;
    CMyString strPatten;
    int32_t timeout(0);
    int32_t ret(0);
    CMyString strError;
    try
    {
        do {
            ret = xml.getString(sect, "keyFormat", strFormat);
            if (ret != 0)
            {
                strError.Format("get keyFormat value from sect [%s] failure, ret=%d", sect.c_str(), ret);
                break;
            }
            ret = xml.getString(sect, "keyPatten", strPatten);
            if (ret != 0)
            {
                strError.Format("get keyPattenvalue from sect [%s] failure, ret=%d", sect.c_str(), ret);
                break;
            }
            ret = xml.getInt32(sect, "timeout", timeout);
            if (ret != 0)
            {
                timeout = -1;
            }
        }while(false);
    }
    catch(std::exception &ex)
    {
        LogFatal("get redis_cache configure failure, error:%s", ex.what());
        return false;
    }

    if (ret != 0)
    {
        LogErr(strError.c_str());
        return false;
    }

    cache.Init(redis, strFormat, strPatten);
    cache.SetTimeout(timeout);

    return true;
}

bool CPluginWithdraw::InitRedisCache(redis_cache::CIncrCache &cache, CMyRedis &redis, CXmlFile &xml, const CMyString &sect)
{
    CMyString strFormat;
    CMyString strPatten;
    CMyString strMaxId;
    CMyString strCurrId;
    int32_t timeout(0);
    int32_t ret(0);
    CMyString strError;
    try
    {
        do {
            ret = xml.getString(sect, "currKey", strCurrId);
            if (ret != 0)
            {
                strError.Format("get currKey value from sect [%s] failure, ret=%d", sect.c_str(), ret);
                break;
            }
            ret = xml.getString(sect, "maxKey", strMaxId);
            if (ret != 0)
            {
                strError.Format("get maxKey value from sect [%s] failure, ret=%d", sect.c_str(), ret);
                break;
            }
            ret = xml.getString(sect, "keyFormat", strFormat);
            if (ret != 0)
            {
                strError.Format("get keyFormat value from sect [%s] failure, ret=%d", sect.c_str(), ret);
                break;
            }
            ret = xml.getString(sect, "keyPatten", strPatten);
            if (ret != 0)
            {
                strError.Format("get keyPattenvalue from sect [%s] failure, ret=%d", sect.c_str(), ret);
                break;
            }
            ret = xml.getInt32(sect, "timeout", timeout);
            if (ret != 0)
            {
                timeout = -1;
            }
        }while(false);
    }
    catch(std::exception &ex)
    {
        LogFatal("get redis_cache configure failure, error:%s", ex.what());
        return false;
    }

    if (ret != 0)
    {
        LogErr(strError.c_str());
        return false;
    }

    cache.Init(redis, strMaxId, strCurrId, strFormat, strPatten);
    cache.SetTimeout(timeout);

    return true;
}

void CPluginWithdraw::setWithdrawLog(const CMyString& orderNo, const int& result, CMyString oper, CMyString remark, CMyString message, int64_t createtime)
{
    int64_t creditTime = createtime;
    if (0 == creditTime)
    {
        creditTime = time(NULL);
    }
    CMyString strMessage = CBase64Wrapper::Encode(message.c_str(), message.length(), false);
    m_pDB->Cmd("call pWithdrawLog('%s', %d, '%s', '%s', '%s', %ld)", 
        orderNo.c_str(), result, oper.c_str(), remark.c_str(), strMessage.c_str(), creditTime);
	m_pDB->Exec();
}

int32_t CPluginWithdraw::checkCanWithdraw(const CMyString& gameCode, const CMyString& uin, const int64_t& fee, const int64_t& tax, char *szResp, uint32_t &cbResp)
{
    int64_t nTotalDeposit(0);
    int64_t nTotalWithdraw(0);
    int64_t nLimitWithdraw(-1);

    //获取已充值总金额
    nTotalDeposit = USERCREDIT_LIST.GetTotalDeposit(m_pDB, gameCode, uin);

    //获取充值提现配置
    nLimitWithdraw = USERCREDIT_LIST.GetLimitWithdraw(m_pDB, gameCode, uin);
    if (-1 == nLimitWithdraw)
    {
        CMyString strMsg("withdraw in limit, total deposit: %ld, limit withdraw: %ld", nTotalDeposit, nLimitWithdraw);
        LogMsg(strMsg.c_str());
        return 0;
    }

    //获取已提现总金额
    nTotalWithdraw = USERCREDIT_LIST.GetTotalWithdraw(m_pDB, gameCode, uin);

    int64_t nCurrentWithdraw = (fee + tax) * 100;
    if ( nTotalWithdraw + nCurrentWithdraw > nLimitWithdraw)
    {
        CMyString strMsg("withdraw over limit, total deposit: %ld, limit withdraw: %ld, total withdraw: %ld current withdraw: %ld", 
            nTotalDeposit, nLimitWithdraw, nTotalWithdraw, nCurrentWithdraw);

        Json::Value resp;
        resp["code"] = err_creditOverLimit;
        resp["message"] = strMsg;
        LogMsg(strMsg.c_str());
        dealResp(resp, szResp, cbResp);

        return err_creditOverLimit;
    }

    CMyString strMsg("withdraw in limit, total deposit: %ld, limit withdraw: %ld, total withdraw: %ld current withdraw: %ld", 
        nTotalDeposit, nLimitWithdraw, nTotalWithdraw, nCurrentWithdraw);
    LogMsg(strMsg.c_str());

    return 0;
}
