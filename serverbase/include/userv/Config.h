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
#pragma once

#include "BaseCode.h"

#include "PortItemList.h"
#include "DbItemList.h"
#include "PluginItemList.h"

struct AliaseCfg
{
    CMyString       cgi_aliase;
    CMyString       cgi;
};
typedef std::vector<AliaseCfg>  VEC_ALIASE;

struct RegexCfg
{
    CMyString       name;
    CMyString       value;
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
    struct Stat
    {
        CMyString       procTag;
        CMyString       redisName;
        uint32_t        interval;
    };
    struct HttpParam
    {
        int32_t         buffer_size;
        int32_t         expire_time;    // 超时时间，-1表示不超时
    };
    struct WebsocketParam
    {
        int32_t         active_timeout; // 激活超时，以毫秒计，-1不超时
    };
    struct HttpServerInfo
    {
        CMyString       serverInfo;
        //...
    };

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
    int32_t Update(void);
    void Save(void);

    int32_t getLogLevel(void);
    CMyString getLogModule(void);
    CMyString getLogPath(void);

    CMyString getPid(void);
    CMyString getName(void);
    int32_t getWorkCount(void);

    int32_t getAcceptTimeout(void);
    int32_t getAcceptParallel(void);

    int32_t getClientActiveTimeout(void);
    //int32_t getWsConnectTimeout(void);
    int32_t getClientAuthTimeout(void);
    int32_t getClientHeartInterval(void);
    int32_t getClientHeartTimeout(void);

    // int32_t getClientRespBufferSize(void);
    void getHttpParam(struct HttpParam &param);
    void getWebsocketParam(struct WebsocketParam &param);
    
    uint32_t getWorkerId(void);
    uint32_t getWorkerCount(void);
    uint32_t getWorkerClientId(void);
    uint32_t getWorkerMaxConn(void);

    CPortItemList& getPortList(void) { return m_portList; }
    CDbItemList&   getDbList(void)   { return m_dbList; }
    CPluginItemList& getPluginList(void) { return m_pluginList; }
    bool getStatConf(struct Stat &stat);
    bool getServerInfo(struct HttpServerInfo &info);

    int32_t UpdatePluginCfg(const CMyString &soName, int32_t slotId, const CMyString &soFile);

    int32_t getRegex(const CMyString &name, CMyString &value);

    bool getAliaseCgi(const CMyString &strAliase, CMyString &strCgi);
    bool getTransferInfo(const CMyString &strCgi, CMyString &strHost, CMyString &strExecCgi);
    CMyString& getEmptyCgi(void);

    // 本函数获得多个id，每个id之前以','分隔，多个群通知
    bool getAlert(CMyString &alertUrl, CMyString &alertIds, CMyString &extraNote);
public:
    // 传出CXmlFile 对象，用于子类读取扩展的xml配置
    CXmlFile& getXml(void) { return m_xml; }

    uint32_t getProcessWorkerId(void) { return m_processWorkerId; }
    void setProcessWorkerId(uint32_t val) { m_processWorkerId = val; }

    void initLog(void);
protected:
    int32_t readParam(void);

    int32_t parseRegex(void);
    int32_t parseAliase(void);
    int32_t parseTransfer(void);


private:
    CXmlFile        m_xml;
    CFileInfo       *m_pInfo;
    CMyTimer        m_timer;

    CPortItemList       m_portList;
    CDbItemList         m_dbList;
    CPluginItemList     m_pluginList;
    int32_t             m_modifyCnt;

private:
    std::vector<RegexCfg>   m_vecRegex;
    // 全局的别名系统，一收到别名立即替换实际cgi
    VEC_ALIASE              m_vecAliaseGlobal;

    // 跳转系统
    std::vector<TransferInfo>   m_vecTransfer;

    CMyString               m_strEmptyCgi;

    uint32_t                m_processWorkerId;
};

#define CONFIG      CConfig::Instance()



