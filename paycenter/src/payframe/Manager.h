/*
 * =====================================================================================
 *
 *       Filename:  Manager.h
 *
 *    Description:  client manager.h
 *
 *        Version:  1.0
 *        Created:  09/08/2019 11:39:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __WEBFRAME_MANAGER_H__
#define __WEBFRAME_MANAGER_H__

#include "BaseCode.h"
#include "MyNet.h"
#include "MyDB.h"
#include "IWebClientMgr.h"
#include "PluginMgr.h"
#include "PayManager.h"

class CClient;
class CManager : public mm::EventHandler
               , public IWebClientMgr
{
public:
    CManager();
    virtual ~CManager();

    bool Init(int32_t fd, uint32_t workerId, uint32_t startId, uint32_t maxId);
    void Run(void);

	// mm::EventHandler
public:
	// = 0 表示处理成功
	virtual bool handleRead(void);
	virtual bool handleWrite(void);
	virtual bool handleTimeout(uint32_t timerId);
	virtual bool handleClose(void);
	virtual int32_t getSocket(void) const { return m_fd; }
	virtual bool handleError(int32_t events, mm::EventHandler *pHandler);
	virtual bool confirmDispose(void);

	// IWebClientMgr
public:
    virtual int32_t ExecuteCGI(const CMyString& strCgi, 
            http::CHttpRequest& req, char *szResp, uint32_t &cbResp,
            STRING_MAP& ext_header, 
            IClient* pClient);
//    virtual int32_t Broadcast2Sibling(int32_t cmd, Json::Value &data);

private:
    uint32_t getNextClientId(void);
    void closeClient(CClient *pClient);

    void OnPayFailure(int32_t code, const char *msg, char *szResp, uint32_t &cbResp);
    void OnCgiFailure(int32_t code, const char *msg, char *szResp, uint32_t &cbResp);

    // 装载相关的配置信息
    int32_t load_config(void);

    int32_t ReloadConfig(http::CHttpRequest &req, IClient* pClient, CMyString &strNote);

    // 调用支付插件
    int32_t doGetPayFunc(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, PCGIFUNC &cgiFunc, CMyString &strCgiNew);
    // 调用支付插件
    int32_t doGetSignFunc(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, PCGIFUNC &cgiFunc, CMyString &strCgiNew);
    // 调用支付插件
    int32_t doGetVerifyFunc(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, PCGIFUNC &cgiFunc, CMyString &strCgiNew);
    // 重载配置信息
    void doReload(http::CHttpRequest& req, char *szResp, uint32_t &cbResp, IClient* pClient);

private:
    int32_t DealSiblingMsg(void);
    int32_t ExecuteSiblingReq(const CMyString &req_);

    bool decodeSiblingRequest(const CMyString &reqstr, Json::Value &data);
    int32_t reloadConfig(void);
    int32_t reloadPlugin(void);
    int32_t reloadPayManager(void);
    void modifyBpData(int32_t cmd, Json::Value &data);

private:
    int32_t         m_fd;
    mm::Reactor     m_reactor;
    uint32_t        m_workerId;
    uint32_t        m_startId;
    uint32_t        m_maxId;
    uint32_t        m_nextId;
    uint32_t        m_timerId;

    typedef std::map<uint32_t, CClient*> CLIENT_MAP;
    CLIENT_MAP      m_mapClient;      // active client

    CPluginMgr      m_plugin;

    CPayManager     m_payMgr;

private:
    CMyString       m_cgiPay;           // 支付的CGI
    CMyString       m_cgiReload;        // 重载数据的CGI
    CMyString       m_cgiGetChannel;    // 获得充值渠道列表的CGI
    CMyString       m_cgiGetWealthStat; // 获得财富统计
    // CMyString       m_ipReload;     // 允许调用reload功能的IP , 0.0.0.0 表示不限
    uint32_t        m_procReqId;       // 进程请求
    CMyString       m_cgiSign;           // 支付的CGI
    CMyString       m_cgiVerify;           // 支付的CGI
};

#endif // __WEBFRAME_MANAGER_H__
