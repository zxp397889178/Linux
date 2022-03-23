/*
 * =====================================================================================
 *
 *       Filename:  oradata.cpp
 *
 *    Description:  game api write to oracle
 *
 *        Version:  1.0
 *        Created:  2019年09月12日 18时20分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "oradata.h"
#include "../Config.h"

COraData::COraData()
    : m_pDB(NULL)
{

}

COraData::~COraData()
{
    SAFE_RELEASE(m_pDB);
}

bool COraData::Init(void)
{
    DbmsCfg dbcfg;
    int32_t ret = CONFIG.getDbmsById(1, dbcfg);
    if (ret < 0)
    {
        LogFatal("not found oracle config data, dbms Id 1");
        return false;
    }
    m_pDB = CreateOracleConn(dbcfg.user.c_str(), dbcfg.passwd.c_str(), 
            dbcfg.host.c_str(), dbcfg.db.c_str(), dbcfg.port);
    if (NULL== m_pDB)
    {
        LogFatal("connect to oracle database failure, dbms id=1");
        return false;
    }
    
    return true;
}

#define CHECK_JSON_PARAM(IN_param, OUT_value, RET_code) ret = jr.GetString(IN_param, OUT_value); \
          if (!ret) { \
              resp["ret"] = #RET_code; \
              CMyString temp("miss %s param.", IN_param); \
              resp["msg"] = temp.c_str(); \
              strResp = writer.write(resp); \
              safe_strcpy(szResp, cbResp, strResp.c_str()); \
              cbResp = strlen(szResp);\
              return RET_code; } 
#define CHECK_JSON_PARAM_I(IN_param, OUT_value, RET_code) ret = jr.GetInt(IN_param, OUT_value); \
          if (!ret) { \
              resp["ret"] = #RET_code; \
              CMyString temp("miss %s param.", IN_param); \
              resp["msg"] = temp.c_str(); \
              strResp = writer.write(resp); \
              safe_strcpy(szResp, cbResp, strResp.c_str()); \
              cbResp = strlen(szResp);\
              return RET_code; } 
#define RESP(RET_code, RET_msg) resp["ret"]=#RET_code; resp["msg"]=RET_msg; \
    strResp = writer.write(resp); \
    safe_strcpy(szResp, cbResp, strResp.c_str()); \
    cbResp = strlen(szResp); \
    return RET_code;

ENTRY_FUNC_DECLARE(COraData, doUserLogin)
{
    LogDebug("enter function %s", __func__);

    ext_headers["Content-Type"]="application/json";
    Json::Value resp;
    Json::FastWriter writer;
    CMyString strResp;

    LogDebug("try to read data");
    CByteStream buff;
    req.getPost(buff);
    LogDebug("post data size %u", buff.size());
    CTempBuffer tmpbuf(buff.size());
    bool ret = buff.Read((void*)tmpbuf.data(), buff.size());
    if (!ret)
    {
        LogWarn("[%s] invalid post data.", __func__);
        RESP(-1001, "post data invalid.");
    }
    LogDebug("post data:%s", tmpbuf.data());

    CJsonReader jr;
    if (!jr.Init(tmpbuf.data()))
    {
        LogWarn("[%s] json buffer invalid. %s", __func__, tmpbuf.data());
        RESP(-1003, "post data is not json-string.");
    }
    LogDebug("JSON decode succeed.");


    CMyString productId;
    CMyString channelId;
    int32_t   accountType(0);
    CMyString accountId;
    CMyString loginIp;
    CMyString phoneId;
    CMyString imeiId;
    CMyString deviceId;
    CMyString gameVersion;
    CMyString gameSource;
    CMyString languageVersion;
    int32_t sourceId;
    CMyString manufacturer;
    CMyString sourceVer;

    
    CHECK_JSON_PARAM("product", productId, -1005);
    CHECK_JSON_PARAM("channel", channelId, -1007);
    CHECK_JSON_PARAM_I("accountType", accountType, -1009);
    CHECK_JSON_PARAM("account", accountId, -1011);
    CHECK_JSON_PARAM("ip", loginIp, -1013);
    jr.GetString("mobile", phoneId);
    jr.GetString("imei", imeiId);
    jr.GetString("device", deviceId);
    CHECK_JSON_PARAM("gameVersion", gameVersion, -1021);
    CHECK_JSON_PARAM("gameSource", gameSource, -1023);
    jr.GetString("language", languageVersion);
    CHECK_JSON_PARAM_I("sourceId", sourceId, -1027);
    jr.GetString("manufacturer", manufacturer);
    jr.GetString("sourceVer", sourceVer);


#ifdef USE_COMMON_DB
    m_pDB->Cmd("BEGIN p_UserLogin('%s', '%s', %d, '%s', '%s', '%s', "\
            "'%s', '%s', '%s', '%s', '%s', %d, '%s', '%s'); END; ",
            productId.c_str(),
            channelId.c_str(),
            accountType,
            accountId.c_str(),
            loginIp.c_str(),
            phoneId.c_str(),
            imeiId.c_str(),
            deviceId.c_str(),
            gameVersion.c_str(),
            gameSource.c_str(),
            languageVersion.c_str(),
            sourceId,
            manufacturer.c_str(),
            sourceVer.c_str());
#else
    m_pDB->CmdProc("p_UserLogin");
    m_pDB->setParam("v_productId", productId);
    m_pDB->setParam("v_channelId", channelId);
    m_pDB->setParam("v_accountType", accountType);
    m_pDB->setParam("v_accountId", accountId);
    m_pDB->setParam("v_loginIp", loginIp);
    m_pDB->setParam("v_phone", phoneId);
    m_pDB->setParam("v_imeiId", imeiId);
    m_pDB->setParam("v_deviceId", deviceId);
    m_pDB->setParam("v_gameVersion", gameVersion);
    m_pDB->setParam("v_gameSource", gameSource);
    m_pDB->setParam("v_langVer", languageVersion);
    m_pDB->setParam("v_sourceId", sourceId);
    m_pDB->setParam("v_manufacturer", manufacturer);
    m_pDB->setParam("v_sourceVer", sourceVer);
#endif
    int32_t result = m_pDB->Exec();
    if (result < 0)
    {
        LogWarn("write user login data failure.");
        RESP(-1029, "save user login data failure.");
    }

    CMyString val("save user %s login data OK.", accountId.c_str());
    LogDebug(val.c_str());
    RESP(0, val.c_str());
}

/////////////////////////////////////////////////////////////////////
static COraData *g_pPlugin;
//PLUGIN_SO_INIT(COraData)
PLUGIN_SO_FINI

extern "C" int32_t doInit(void)
{
    g_pPlugin = new COraData();

    if (!g_pPlugin->Init())
    {
        return -1001;
    }

    return 0;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "userLogin", "doUserLogin");
}

ENTRY_FUNC_IMPLEMENT(doUserLogin)


