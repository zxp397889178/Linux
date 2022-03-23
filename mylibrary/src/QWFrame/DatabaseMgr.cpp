/*
 * =====================================================================================
 *
 *       Filename:  QWFrame.cpp
 *
 *    Description:  session manager, for accept callback
 *
 *        Version:  1.0
 *        Created:  2015年09月08日 16时44分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zengyh, zengyh@quwangame.com
 *   Organization:  www.quwangame.com
 *
 * =====================================================================================
 */


#include "DatabaseMgr.h"
#include "QWFrame.h"

CDatabaseMgr::CDatabaseMgr()
{

}


CDatabaseMgr::~CDatabaseMgr()
{

}


bool CDatabaseMgr::Init()
{
	if (!this->ConnectMySql())
	{
		return false;
	}

	if (!this->ConnectRedis())
	{
		return false;
	}


	return true;
}

ISimDB* CDatabaseMgr::GetDbConnector(const char *dbName)
{
	DB_CONN_MAP::iterator iter = m_mapDbConnector.find(dbName);
	if (iter == m_mapDbConnector.end())
	{
		return NULL;
	}

	return iter->second;
}

CMyRedis* CDatabaseMgr::GetRedis(const char *dbName)
{
	REDIS_CONN_MAP::iterator iter = m_mapRedis.find(dbName);
	if (iter == m_mapRedis.end())
	{
		return NULL;
	}

	return iter->second;
}


bool CDatabaseMgr::ConnectMySql()
{
	printf("CDatabaseMgr::Init begin connect to mysql db..\n");
	CQWFrameConfig::MYSQL_CFG_LIST& cfgMysqlList = QW_FRAME->GetFrameConfig().GetDatabaseCfg();
	CQWFrameConfig::MYSQL_CFG_LIST::iterator iter = cfgMysqlList.begin();
	for (; iter != cfgMysqlList.end(); ++iter)
	{
		MysqlCfg &mysqlCfg = iter->second;
		ISimDB* pDb = CreateMysqlConn(mysqlCfg.strUser.c_str(), mysqlCfg.strPasswd.c_str(), mysqlCfg.strHost.c_str(), mysqlCfg.strDb.c_str(), mysqlCfg.nPort, 0, 0, mysqlCfg.strCharset.c_str());
		if (pDb == NULL)
		{
			LogFatal("CDatabaseMgr::Init connect mysql %s host[%s] port[%d] db[%s] failure..", iter->first.c_str(), mysqlCfg.strHost.c_str(), mysqlCfg.nPort, mysqlCfg.strDb.c_str());
			printf("CDatabaseMgr::Init connect mysql %s host[%s] port[%d] db[%s] failure..\n", iter->first.c_str(), mysqlCfg.strHost.c_str(), mysqlCfg.nPort, mysqlCfg.strDb.c_str());
			return false;
		}
		printf("CDatabaseMgr::Init connect mysql %s host[%s] port[%d] db[%s] charset[%s] success..\n", iter->first.c_str(), mysqlCfg.strHost.c_str(), mysqlCfg.nPort, mysqlCfg.strDb.c_str(), mysqlCfg.strCharset.c_str());
		m_mapDbConnector.insert(make_pair(iter->first, pDb));
	}
	printf("CDatabaseMgr::Init end connect to mysql db..\n");
	return true;
}
bool CDatabaseMgr::ConnectRedis()
{
	printf("CDatabaseMgr::ConnectRedis, begin connect redis\n");
	CQWFrameConfig::REDIS_CFG_LIST& cfgRedisList = QW_FRAME->GetFrameConfig().GetRedisCfg();
	CQWFrameConfig::REDIS_CFG_LIST::iterator iter = cfgRedisList.begin();
	for (; iter != cfgRedisList.end(); ++iter)
	{
		RedisCfg &redisCfg = iter->second;
		LogInit("connect redis [%s:%u] db %u.", redisCfg.strHost.c_str(), redisCfg.port, redisCfg.dbIndex);

		CMyRedis *pRedis = new CMyRedis();
		if (!pRedis->Open(redisCfg.strHost, redisCfg.port, "", redisCfg.timeout))
		{
			LogFatal("CDatabaseMgr::ConnectRedis connect to server failure.");
			return false;
		}

		if (!pRedis->Ping(10))
		{
			LogFatal("CDatabaseMgr::ConnectRedis redis-server service FAILURE.");
			return false;
		}

		if (!pRedis->Select(redisCfg.dbIndex))
		{
			LogFatal("CDatabaseMgr::ConnectRedis redis select FAILURE.");
			return false;
		}
		
		printf("CDatabaseMgr::ConnectRedis connect redis [%s:%u] db [%u] success\n", redisCfg.strHost.c_str(), redisCfg.port, redisCfg.dbIndex);

		m_mapRedis.insert(make_pair(redisCfg.strName, pRedis));

	}

	printf("CDatabaseMgr::ConnectRedis end connect redis\n");
	return true;
}

