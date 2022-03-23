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

#ifndef _QW_DATABASEMGR_H_
#define _QW_DATABASEMGR_H_

#include "MyDB.h"
#include <map>
using std::map;
class CDatabaseMgr
{
	typedef map<CMyString, ISimDB* > DB_CONN_MAP;
	typedef map<CMyString, CMyRedis* > REDIS_CONN_MAP;
public:
	CDatabaseMgr();
	~CDatabaseMgr();


public:
	bool Init();

	ISimDB* GetDbConnector(const char *dbName);
	CMyRedis* GetRedis(const char *dbName);
private:
	bool ConnectMySql();
	bool ConnectRedis();
private:
	DB_CONN_MAP m_mapDbConnector;
	REDIS_CONN_MAP m_mapRedis;
};

#endif	// _QW_QWFRAME_H_
