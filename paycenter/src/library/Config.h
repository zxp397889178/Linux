/*
 * =====================================================================================
 *
 *       Filename:  Config.h
 *
 *    Description:  config for apigate
 *
 *        Version:  1.0
 *        Created:  2019年09月02日 19时02分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __WEBFRAME_CONFIG_H__
#define __WEBFRAME_CONFIG_H__

#include "BaseCode.h"
#include "IpList.h"

enum NET_TYPE 
{ 
    net_http, 
    net_tcp, 
    net_websocket 
};
enum HEADER_TYPE 
{ 
    header_msghead = 1,         // MSG_HEAD: uint16_t(pkgLen) + uint16_t(cmdId) + uint32_t(code)
    //header_cmd,                 // uint32_t(cmdId)
    header_protobuf             // uint16_t(pkgLen) + uint16_t(cmdId) 
};

struct NetCfg
{
    uint32_t                port;
    bool                    ssl;
    int32_t                 fd;
    
    NetCfg() : port(0), ssl(false), fd(-1) {}
};

struct AliaseCfg
{
    CMyString       cgi_aliase;
    CMyString       cgi;
};
typedef std::vector<AliaseCfg>  VEC_ALIASE;
struct PluginCfg
{
    CMyString       name;       // plugin name
    CMyString       dll[2];     // 两个slot
    CMyString       init_func;  // 初始化函数，[可选]
    CMyString       entry_func; // plugin 入口函数，函数集合定义处
    CMyString       run_func;   // plugin 定时执行函数，[可选]

    VEC_ALIASE      vecAliase;

};
enum DBMS_TYPE { dbms_oracle, dbms_mysql, dbms_sqlite, dbms_redis };
struct DbmsCfg
{
    int32_t         id;      // 索引1
    CMyString       name;    // 索引2
    DBMS_TYPE       dbms_type;
    CMyString       host;
    int32_t         port;
    CMyString       db;
    CMyString       user;
    CMyString       passwd;

    DbmsCfg() : id(0), dbms_type(dbms_mysql), port(0) {  }
};
struct RegexCfg
{
    CMyString       name;
    CMyString       value;
};
struct ReloadCfg
{
    CMyString       cgi;            // 对应的CGI
    CIpList         ips;            // 可用的IP列表
    CMyString       redis_name;     // 对应的redis名称
    CMyString       keyformat;      // key的名称格式
    int32_t         timeout;        // 超时时间，以秒计
    int32_t         loop_time;      // 循环检查的时间间隔，以秒计
};
struct TransferInfo
{
    CMyString       cgi;        // 初始的CGI
    CMyString       host;       // 要跳转到的主机
    CMyString       exec_cgi;   // 执行跳转的CGI指令
};
class CConfig
{
public:
    CConfig();
    ~CConfig();

    static CConfig& Instance(void);
    
public:

    bool Load(const CMyString &strFile);
    /* result: 0 - xml not modified, NOT need reload
     *         1 - xml has been modified, reload SUCCEED
     *        <0 - xml has been modified, reload FAILURE
     */
    int32_t Update(bool bForce = false);
    CMyString getConfigPath(void);

    int32_t getLogLevel(void);
    CMyString getLogModule(void);
    CMyString getLogPath(void);

    CMyString getPid(void);
    enum NET_TYPE getNetType(void) { return m_netType; }
    int32_t getHeaderType(void) { return m_headerType; }
    int32_t getWorkCount(void);

    int32_t getAcceptTimeout(void);
    int32_t getAcceptParallel(void);

    int32_t getClientActiveTimeout(void);
    int32_t getClientAuthTimeout(void);
    int32_t getClientHeartInterval(void);
    int32_t getClientHeartTimeout(void);
    int32_t getClientRespBufferSize(void);
    
    uint32_t getWorkerId(void);
    uint32_t getWorkerCount(void);
    uint32_t getWorkerClientId(void);
    uint32_t getWorkerMaxConn(void);

    int32_t getNetCount(void);
    int32_t getNet(int32_t index, NetCfg &net);

    int32_t getPluginCount(void);
    int32_t getPlugin(int32_t index, PluginCfg &plugin);

    int32_t getDbmsCount(void);
    int32_t getDbms(int32_t index, DbmsCfg &dbms);
    int32_t getDbmsById(int32_t id, DbmsCfg &dbms);
    int32_t getDbmsByName(const CMyString &strName, DbmsCfg &dbms);

    int32_t getRegex(const CMyString &name, CMyString &value);

    bool getAliaseCgi(const CMyString &strAliase, CMyString &strCgi);
    bool getTransferInfo(const CMyString &strCgi, CMyString &strHost, CMyString &strExecCgi);
    CMyString& getEmptyCgi(void);

    // <alert url="..." id="123" ids="123,345" note=""/>
    // 本函数获得id，单个通知
    bool getAlert(int32_t &alertId, CMyString &alertUrl);
    // 本函数获得多个id，每个id之前以','分隔，多个群通知
    bool getAlert(CMyString &alertUrl, CMyString &alertIds, CMyString &extraNote);

    /* <main>
     *      <service>
     *          <ip url="..." />
     *      </service>
     * </main>
     */
    const char* getIpUrl(CMyString &url);

    ReloadCfg& getReloadCfg(void) { return m_cfgReload; }
public:
    // 传出CXmlFile 对象，用于子类读取扩展的xml配置
    CXmlFile& getXml(void) { return m_xml; }

    uint32_t getProcessWorkerId(void) { return m_processWorkerId; }
    void setProcessWorkerId(uint32_t val) { m_processWorkerId = val; }

protected:
    int32_t readParam(void);

    int32_t parseNet(void);
    int32_t parseDbms(void);
    int32_t parsePlugin(void);
    int32_t parseRegex(void);
    int32_t parseAliase(void);
    int32_t parseTransfer(void);

    int32_t parseReloadCfg(void);

private:
    void initLog(void);
    int32_t readNetType(void);

private:
    CXmlFile        m_xml;
    CFileInfo       *m_pInfo;
    CMyTimer        m_timer;

    std::vector<NetCfg>     m_vecNet;
    std::vector<PluginCfg>  m_vecPlugin;
    std::vector<DbmsCfg>    m_vecDbms;
    std::vector<RegexCfg>   m_vecRegex;

    NET_TYPE                m_netType;
    int32_t                 m_headerType;

private:
    // 全局的别名系统，一收到别名立即替换实际cgi
    VEC_ALIASE              m_vecAliaseGlobal;

    // 跳转系统
    std::vector<TransferInfo>   m_vecTransfer;

    CMyString               m_strEmptyCgi;
    ReloadCfg               m_cfgReload;
    uint32_t                m_processWorkerId;
};

#define CONFIG      CConfig::Instance()

#endif // __WEBFRAME_CONFIG_H__


