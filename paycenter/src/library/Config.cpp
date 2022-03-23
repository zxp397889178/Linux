/*
 * =====================================================================================
 *
 *       Filename:  Config.cpp
 *
 *    Description:  config for apigate
 *
 *        Version:  1.0
 *        Created:  2019年09月02日 19时06分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Config.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include "Security.h"

CConfig::CConfig()
    : m_xml()
    , m_pInfo(NULL)
    , m_timer()
    , m_netType(net_tcp)
    , m_headerType(1)
    , m_processWorkerId(0)
{

}

CConfig::~CConfig()
{
    SAFE_DELETE(m_pInfo);
}

CConfig& CConfig::Instance(void)
{
    static CConfig s_obj;

    return s_obj;
}

bool CConfig::Load(const CMyString &strFile)
{
    m_pInfo = new CFileInfo;
    if (NULL == m_pInfo)
    {
        std::cout << "memory insufficient for create CFileInfo object. " << std::endl;
        return false;
    }
    if (!m_pInfo->Open(strFile.c_str()))
    {
        std::cout << "not found config file:" << strFile << std::endl;
        return false;
    }

    if (!m_xml.Load(strFile.c_str()))
    {
        std::cout << "load config file [" << strFile << "] failure, maybe corrupted." << std::endl;
        return false;
    }

    int32_t ret = this->readParam();
    if (ret<0)
    {
        std::cout << "read config failure, result: " << ret << std::endl;
        return false;
    }

    std::cout << "load config [" << strFile << "] succeed." << std::endl;
    return true;
}

int32_t CConfig::readNetType(void)
{
    CMyString nettype = m_xml.GetString("main/service", "type");
    nettype.MakeLower();
    if (nettype=="http") 
    {
        m_netType = net_http;
    }
    else if (nettype=="tcp")
    {
        m_netType = net_tcp;
    }
    else if (nettype=="websocket")
    {
        m_netType = net_websocket;
    }
    else
    {
        LogFatal("unknown net type %s", nettype.c_str());
        return -1;
    }

    m_headerType = m_xml.GetLong("main/service", "header");
    if (m_headerType <=0 || m_headerType>3)
    {
        LogFatal("unknown header type %d", m_headerType);
        return -1003;
    }

    return 0;
}

int32_t CConfig::parseNet(void)
{
    void *pRoot = m_xml.GetElement("main/service/net");
    if (NULL == pRoot)
    {
        return -1001;
    }

    m_vecNet.clear();
    do {
        std::vector<uint32_t> vec_ports;
        CMyString ports = m_xml.GetValue(pRoot, "ports");
        safe_strtok(ports.c_str(), vec_ports, ",");
        int32_t netssl(0);
        m_xml.GetValue(pRoot, "ssl", netssl);
        bool ssl = (netssl==0) ? false : true;

        for(std::vector<uint32_t>::iterator it(vec_ports.begin());
                it != vec_ports.end(); ++it)
        {
            NetCfg net;
            net.port = *it;
            net.ssl = ssl;
            m_vecNet.push_back(net);
        }

        pRoot = m_xml.GetNextSibling(pRoot, "net");       
    }while(pRoot != NULL);

    return 0;
}

int32_t CConfig::parseDbms(void)
{
    void *pRoot = m_xml.GetElement("main/dbms/db");
    if (pRoot == NULL)
    {
        return -1003;
    }

    m_vecDbms.clear();
    do {
        DbmsCfg dbms;
        m_xml.GetValue(pRoot, "id", dbms.id);
        m_xml.GetValue(pRoot, "name", dbms.name);
        CMyString dbms_type;
        m_xml.GetValue(pRoot, "type", dbms_type);
        dbms_type.MakeLower();
        if (dbms_type=="oracle")
        {
            dbms.dbms_type = dbms_oracle;
        }
        else if (dbms_type=="mysql")
        {
            dbms.dbms_type = dbms_mysql;
        }
        else if (dbms_type == "sqlite")
        {
            dbms.dbms_type = dbms_sqlite;
        }
        else if (dbms_type == "redis")
        {
            dbms.dbms_type = dbms_redis;
        }
        else
        {
            std::cout << "unknown dbms type " << dbms_type << std::endl;
            pRoot = m_xml.GetNextSibling(pRoot, "dbms");
            continue;
        }
        m_xml.GetValue(pRoot, "host", dbms.host);
        m_xml.GetValue(pRoot, "port", dbms.port);
        m_xml.GetValue(pRoot, "db", dbms.db);
        m_xml.GetValue(pRoot, "user", dbms.user);
        CMyString strPasswd;
        m_xml.GetValue(pRoot, "passwd", strPasswd);
        CPasswdWrapper::Decode(strPasswd, dbms.passwd);

        m_vecDbms.push_back(dbms);

        pRoot = m_xml.GetNextSibling(pRoot, "db");
    }while(pRoot != NULL);

    return 0;
}

int32_t CConfig::parsePlugin(void)
{
    m_strEmptyCgi = m_xml.GetString("main/plugin", "empty_cgi");
    void *pRoot = m_xml.GetElement("main/plugin/cgi");
    if (NULL == pRoot)
    {
        return -1005;
    }

    m_vecPlugin.clear();
    do {
        PluginCfg plugin;
        m_xml.GetValue(pRoot, "name", plugin.name);
        m_xml.GetValue(pRoot, "slot1", plugin.dll[0]);
        m_xml.GetValue(pRoot, "slot2", plugin.dll[1]);
        m_xml.GetValue(pRoot, "init", plugin.init_func);
        m_xml.GetValue(pRoot, "entry", plugin.entry_func);
        m_xml.GetValue(pRoot, "maintance", plugin.run_func);

        // 获得子项目的别名
        void *pAliase = m_xml.GetChild(pRoot, "aliase");
        if (pAliase != NULL)
        {
            do {
                struct AliaseCfg aliase;
                m_xml.GetValue(pAliase, "aliase",  aliase.cgi_aliase);
                m_xml.GetValue(pAliase, "cgi", aliase.cgi);
                plugin.vecAliase.push_back(aliase);
                pAliase = m_xml.GetNextSibling(pAliase, "aliase");
            }while(pAliase != NULL);
        }

        m_vecPlugin.push_back(plugin);

        pRoot = m_xml.GetNextSibling(pRoot, "cgi");
    }while(pRoot != NULL);

    return 0;
}

int32_t CConfig::parseAliase(void)
{
    void *pRoot = m_xml.GetElement("main/plugin/aliase");
    if (NULL == pRoot)
    {
        return -1005;
    }

    m_vecAliaseGlobal.clear();
    do {
        AliaseCfg aliase;
        m_xml.GetValue(pRoot, "aliase", aliase.cgi_aliase);
        m_xml.GetValue(pRoot, "cgi", aliase.cgi);

        m_vecAliaseGlobal.push_back(aliase);

        pRoot = m_xml.GetNextSibling(pRoot, "aliase");
    }while(pRoot != NULL);

    return 0;
}

int32_t CConfig::parseTransfer(void)
{
    void *pRoot = m_xml.GetElement("main/plugin/transfer");
    if (NULL == pRoot)
    {
        return -1005;
    }

    m_vecTransfer.clear();
    do {
        struct TransferInfo ti;
        m_xml.GetValue(pRoot, "cgi", ti.cgi);
        m_xml.GetValue(pRoot, "host", ti.host);
        m_xml.GetValue(pRoot, "exec_cgi", ti.exec_cgi);

        m_vecTransfer.push_back(ti);

        pRoot = m_xml.GetNextSibling(pRoot, "transfer");
    }while(pRoot != NULL);

    return 0;
}

int32_t CConfig::parseReloadCfg(void)
{
    static const CMyString sect1="main/service/reload/uri";
    static const CMyString sect2="main/service/reload/cache";

    int32_t ret = m_xml.getString(sect1, "cgi", m_cfgReload.cgi);
    if (ret != 0)
    {
        LogErr("not found config in section[%s] key[%s]", sect1.c_str(), "cgi");
        return -1;
    }
    CMyString strIps;
    ret = m_xml.getString(sect1, "validip", strIps);
    if (ret == 0)
    {
        m_cfgReload.ips.setIps(strIps, ',');
    }
    else
    {
        m_cfgReload.ips.setIps("0.0.0.0", ',');
    }

    ret = m_xml.getString(sect2, "cache", m_cfgReload.redis_name);
    if (ret != 0)
    {
        LogErr("not found config in section[%s] key[%s]", sect2.c_str(), "cache");
        return -3;
    }
    ret = m_xml.getString(sect2, "keyfmt", m_cfgReload.keyformat);
    if (ret != 0)
    {
        LogErr("not found config in section[%s] key[%s]", sect2.c_str(), "keyfmt");
        return -5;
    }
    ret = m_xml.getInt32(sect2, "timeout", m_cfgReload.timeout);
    if (ret != 0)
    {
        m_cfgReload.timeout = 60;
    }
    ret = m_xml.getInt32(sect2, "looptime", m_cfgReload.loop_time);
    if (ret != 0)
    {
        m_cfgReload.loop_time = 10;
    }
    return 0;
}

int32_t CConfig::parseRegex(void)
{
    void *pRoot = m_xml.GetElement("main/regex/item");
    if (NULL == pRoot)
    {
        return -1007;
    }

    m_vecRegex.clear();
    do {
        RegexCfg regex1;
        m_xml.GetValue(pRoot, "name", regex1.name);
        m_xml.GetValue(pRoot, "value", regex1.value);
        m_vecRegex.push_back(regex1);
        
        pRoot = m_xml.GetNextSibling(pRoot, "item");
    }while(pRoot != NULL);
    return 0;
}

void CConfig::initLog(void)
{
    CMyString strPath = this->getLogPath();
    CreateDir(strPath.c_str());
    Comm::OpenLog(this->getLogModule().c_str(),
            this->getLogLevel(),
            strPath.c_str());
}

int32_t CConfig::getLogLevel(void)
{
    return m_xml.GetLong("main/log", "level");
}

CMyString CConfig::getLogModule(void)
{
    return m_xml.GetString("main/log", "module");
}

CMyString CConfig::getLogPath(void)
{
    return m_xml.GetString("main/log", "path");
}

int32_t CConfig::readParam(void)
{
    initLog();

    readNetType();

    if (this->parseNet() != 0)
    {
        return -101;
    }

    if (this->parseDbms() != 0)
    {
        return -102;
    }

    if (this->parsePlugin() != 0)
    {
        return -103;
    }

    this->parseRegex();

    this->parseAliase();

    this->parseTransfer();

    if (this->parseReloadCfg() != 0)
    {
        return -109;
    }

    return 0;
}

CMyString CConfig::getPid(void)
{
    return m_xml.GetString("main/service", "pid");
}

int32_t CConfig::getWorkCount(void)
{
    return m_xml.GetLong("main/service", "parallel");
}

int32_t CConfig::getAcceptTimeout(void)
{
    return m_xml.GetLong("main/service/accept", "timeout");
}

int32_t CConfig::getAcceptParallel(void)
{
    return m_xml.GetLong("main/service/accept", "parallel");
}

int32_t CConfig::getClientActiveTimeout(void)
{
    return m_xml.GetLong("main/service/client/active", "timeout");
}

int32_t CConfig::getClientAuthTimeout(void)
{
    return m_xml.GetLong("main/service/client/authorize", "timeout");
}

int32_t CConfig::getClientHeartInterval(void)
{
    return m_xml.GetLong("main/service/client/heartbeat", "interval");
}

int32_t CConfig::getClientHeartTimeout(void)
{
    int32_t times = m_xml.GetLong("main/service/client/heartbeat", "timeout");
    return times * this->getClientHeartInterval();
}

int32_t CConfig::getClientRespBufferSize(void)
{
    return m_xml.GetLong("main/service/client/resp", "buffer_size");
}

int32_t CConfig::getNetCount(void)
{
    return static_cast<int32_t>(m_vecNet.size());
}

int32_t CConfig::getNet(int32_t index, NetCfg &net)
{
   if (index <0 || index >= this->getNetCount())
   {
       return -1101;
   }
   net = m_vecNet[index];
   return 0;
}

int32_t CConfig::getPluginCount(void)
{
    return static_cast<int32_t>(m_vecPlugin.size());
}

int32_t CConfig::getPlugin(int32_t index, PluginCfg &plugin)
{
    if (index<0 || index>=this->getPluginCount())
    {
        return -1103;
    }
    plugin = m_vecPlugin[index];
    return 0;
}

int32_t CConfig::getDbmsCount(void)
{
    return static_cast<int32_t>(m_vecDbms.size());
}

int32_t CConfig::getDbms(int32_t index, DbmsCfg &dbms)
{
    if (index<0 || index>=this->getDbmsCount())
    {
        return -1105;
    }
    dbms = m_vecDbms[index];
    return 0;
}

int32_t CConfig::getDbmsById(int32_t id, DbmsCfg &dbms)
{
    int32_t ret(-1107);

    std::vector<DbmsCfg>::iterator it(m_vecDbms.begin());
    for(; it != m_vecDbms.end(); ++it)
    {
        DbmsCfg &dbms1 = *it;
        if (dbms1.id == id)
        {
            dbms = dbms1;   
            ret = 0;
            break;
        }
    }
    return ret;
}

int32_t CConfig::getDbmsByName(const CMyString &strName, DbmsCfg &dbms)
{
    int32_t ret(-1108);

    std::vector<DbmsCfg>::iterator it(m_vecDbms.begin());
    for(; it != m_vecDbms.end(); ++it)
    {
        DbmsCfg &dbms1 = *it;
        if (!strcasecmp(dbms1.name.c_str(), strName.c_str()))
        {
            dbms = dbms1;   
            ret = 0;
            break;
        }
    }
    return ret;
}

int32_t CConfig::getRegex(const CMyString &name, CMyString &value)
{
    int32_t ret(-1109);

    std::vector<RegexCfg>::iterator it(m_vecRegex.begin());
    for(; it != m_vecRegex.end(); ++it)
    {
        RegexCfg &regex1 = *it;
        if (regex1.name == name)
        {
            value = regex1.value;
            ret = 0;
            break;
        }
    }

    return ret;
}

CMyString CConfig::getConfigPath(void)
{
    CMyString strPath;
    if (NULL == m_pInfo)
    {
        return strPath;
    }

    strPath = m_pInfo->GetFullPath();
    return strPath;
}

int32_t CConfig::Update(bool bForce)
{
    if (!bForce)
    {
        if (!m_pInfo->IsModified())
        {
            return 0;
        }
    }

    CMyString strFile = m_pInfo->GetFullPath();
    if (!m_xml.Load(strFile.c_str()))
    {
        LogFatal("reload config file %s FAILURE, maybe corrupted.",
                strFile.c_str());
        return -1;
    }

    int32_t ret = this->readParam();
    if (ret<0)
    {
        LogFatal("reload config file %s FAILURE, result %d",
                strFile.c_str(), ret);
        return -3;
    }
    LogImpt("reload config file %s SUCCEED.", m_pInfo->GetFullPath());
    return 1;
}

uint32_t CConfig::getWorkerId(void)
{
    return m_xml.GetLong("main/service/worker", "startId");
}

uint32_t CConfig::getWorkerCount(void)
{
    return m_xml.GetLong("main/service/worker", "count");
}

uint32_t CConfig::getWorkerClientId(void)
{
    return m_xml.GetLong("main/service/worker/client", "start");
}

uint32_t CConfig::getWorkerMaxConn(void)
{
    return m_xml.GetLong("main/service/worker/client", "maxconn");
}

bool CConfig::getAliaseCgi(const CMyString &strAliase, CMyString &strCgi)
{
    bool bFind(false);
    VEC_ALIASE::iterator it(m_vecAliaseGlobal.begin());
    for (; it != m_vecAliaseGlobal.end(); ++it)
    {
        struct AliaseCfg &cfg = *it;
        if (cfg.cgi_aliase == strAliase)
        {
            bFind = true;
            strCgi = cfg.cgi;
            break;
        }
    }
    return bFind;
}

bool CConfig::getTransferInfo(const CMyString &strCgi, CMyString &strHost, CMyString &strExecCgi)
{
    bool bFind(false);
    std::vector<TransferInfo>::iterator it(m_vecTransfer.begin());
    for (; it != m_vecTransfer.end(); ++it)
    {
        struct TransferInfo &cfg = *it;
        if (cfg.cgi == strCgi)
        {
            bFind = true;
            strHost = cfg.host;
            strExecCgi = cfg.exec_cgi;
            break;
        }
    }
    return bFind;
}

CMyString& CConfig::getEmptyCgi(void)
{
    return m_strEmptyCgi;
}

bool CConfig::getAlert(int32_t &alertId, CMyString &alertUrl)
{
    const char *sect = "main/service/alert";
    alertId = m_xml.GetLong(sect, "id");
    alertUrl = m_xml.GetString(sect, "url");

    if (alertId<=0 || alertUrl.empty())
    {
        return false;
    }
    return true;
}

bool CConfig::getAlert(CMyString &alertUrl, CMyString &alertIds, CMyString &extraNote)
{
    const char *sect = "main/service/alert";
    alertUrl = m_xml.GetString(sect, "url");
    alertIds = m_xml.GetString(sect, "ids");
    extraNote = m_xml.GetString(sect, "note");
    if (alertUrl.empty() || alertIds.empty())
    {
        return false;
    }
    return true;
}

const char* CConfig::getIpUrl(CMyString &url)
{
    static const char* sect="main/service/ip";

    url = m_xml.GetString(sect, "url");
    return url.c_str();
}
