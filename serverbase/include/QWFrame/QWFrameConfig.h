/*
 * =====================================================================================
 *
 *       Filename:  QWFrame.h
 *
 *    Description:  QWFrame
 *
 *        Version:  1.0
 *        Created:  2017年06月01日 16时44分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zengyh, zengyh@quwangame.com
 *   Organization:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef _QW_QWFRAMECONFIG_H_
#define _QW_QWFRAMECONFIG_H_

#include "BaseCode.h"
#include "IpTables.h"
#include <map>
#include <stdint.h>
#include "Security.h"
using std::map;

struct MysqlCfg
{
	CMyString	strName;
	CMyString	strHost;
	uint32_t		nPort;
	CMyString	strDb;
	CMyString	strUser;
	CMyString	strPasswd;
	CMyString    strCharset;
	uint32_t nId;
};

struct RedisCfg
{
	CMyString	strName;
	CMyString	strHost;
	uint32_t	port;
	uint32_t	dbIndex;
	uint32_t	timeout;
	uint32_t	nId;
};

struct ListenCfg
{
	CMyString	strName;

	uint32_t		nId;
	uint32_t		nActorId;	
	uint32_t		nPort;
	CMyString	strProto;
};

struct ConnectionCfg
{
	CMyString	strName;

	uint32_t		nId;
	uint32_t		nActorId;

	CMyString	strHost;
	uint32_t		nPort;
	uint32_t		nCount;
};


class CQWFrameConfig
{
public:
	struct LogCfg
	{
		enum AiHit
		{
			LOG_IGNORE = 0,				//不记日志
			LOG_ALL = 1,				//记录所有玩家日志
			LOG_SPECIFY = 2,			//指定玩家,m_vecLogUin中的账号
		};

		CMyString 	strModule;
		CMyString	strPath;
		uint8_t		level;
	};

	struct IpTableCfg
	{
		bool		enable;
		CMyString	strFile;
	};

	typedef map<CMyString, MysqlCfg>		MYSQL_CFG_LIST;
	typedef map<uint32_t, ListenCfg>			LISTEN_CFG_LIST;
	typedef map<uint32_t, ConnectionCfg>	CONNECTION_CFG_LIST;
	typedef map<CMyString, RedisCfg>		REDIS_CFG_LIST;

public:
	CFileInfo* GetXmlInfo()		{ return m_pXmlInfo; }


public:
	bool Init(const CMyString &strFile);

	//if xml file is modifyed, reload it. the interval default is 5s
	void Update(int32_t interval = 5000);

	uint32_t	GetSvrId(void) { return m_nSvrId; }
	uint32_t	GetActorId(void) { return m_nActorId; }

	LogCfg& 					GetLogCfg(void) 		{ return m_cfgLog; }
	IpTableCfg& 			GetIpTableCfg(void) 	{ return m_cfgIpTable; };

	MYSQL_CFG_LIST& 	GetDatabaseCfg(void) 	{ return m_cfgMysqlList; }
	REDIS_CFG_LIST& 	GetRedisCfg(void) 	{ return m_cfgRedisList; }
	LISTEN_CFG_LIST&	GetListenConfig(void) { return m_cfgListenList; };

	CONNECTION_CFG_LIST&	GetConnectionConfig(void) { return m_cfgConnectionList; };

	bool IsIpValid(uint32_t ip);
	bool IsIpValid(const CMyString &strIp);
	bool IsDevValid(const CMyString &strDev);
	bool IsUinValid(uint32_t uin);

	MysqlCfg *GetMysqlCfg(const char *dbName);
	RedisCfg *GetRedisCfg(const char *dbName);
	ConnectionCfg *GetConnectionCfg(uint32_t actorId);
public:
	ListenCfg *GetListenCfg(uint32_t port);
public:
	CQWFrameConfig();
	~CQWFrameConfig();

	bool ReadAllConfig(void);

	int32_t ReadSvrCfg(CXmlFile &xml);
	int32_t ReadLogCfg(CXmlFile &xml);
	int32_t ReadIpTableCfg(CXmlFile &xml);
	int32_t ReadDatabaseCfg(CXmlFile &xml);
	int32_t ReadListenCfg(CXmlFile &xml);
	int32_t ReadConnectionCfg(CXmlFile &xml);

private:

	//void DecryptMysqlCfg(CMyString &strValue);

	int32_t ReadDbXmlContent(CMyString &strFile, CMyString &strContent);
private:
	CFileInfo*		m_pFrameXml;										//框架配置
	CFileInfo*		m_pXmlInfo;											//应用配置
	CFileInfo*		m_pIptable;											//IP限制配置
	CMyTimer		m_timerXmlCheck;

private:
	LogCfg								m_cfgLog;							// 日志相关配置
	IpTableCfg							m_cfgIpTable;						// IP相关配置
	MYSQL_CFG_LIST						m_cfgMysqlList;						// MYSQL相关配置
	REDIS_CFG_LIST						m_cfgRedisList;						// redis相关配置
	LISTEN_CFG_LIST						m_cfgListenList;					// SOCKET帧听相关配置
	CONNECTION_CFG_LIST					m_cfgConnectionList;				// 连接服务相关

	CIpTables		m_iptables;												//IP限制表


	uint32_t	m_nSvrId;
	uint32_t	m_nActorId;
};

#endif	// _QW_QWFRAME_H_
