// MyDB.cpp : Defines the exported functions for the DLL application.
//

#include <stdio.h>
#include <stdarg.h>
#include "SimMysql.h"
#include "SimRecord.h"
// #include "SimSqlite.h"
// #include "SimOracle.h"

ISimDB* CreateMysqlConn(const char *user_name, const char *passwd, 
    const char *host, const char *db, int32_t port, ISimDBEvent *pEvent,
    const char *appname, const char *charset)
{
    CSimMySql *pMySql = new CSimMySql();
    if (NULL != pMySql)
    {
	
        int32_t ret = pMySql->Open(user_name, passwd, host, db, port, pEvent, appname, charset);
		if (ret < 0)
        {
			printf("open database failure, ret=%d\n", ret);
            SAFE_RELEASE(pMySql);
        }
    }

    return pMySql;
}

#if 0
ISimDB* CreateOracleConn(const char *user_name, const char *passwd, 
    const char *host, const char *db, int32_t port, ISimDBEvent *pEvent,
    const char *appname, const char *charset)
{
    //LogWarn("begin function %s\n", __func__);
    CSimOracle*pOracle = new CSimOracle();
    if (NULL != pOracle)
    {
        //LogWarn("new CSimOracle ok.\n");
        int32_t ret = pOracle->Open(user_name, passwd, host, db, port, pEvent, appname, charset);
		if (ret < 0)
        {
			printf("open database failure, ret=%d\n", ret);
            SAFE_RELEASE(pOracle);
        }
        //LogWarn("connect oracle succeed.\n");
    }

    return pOracle;
}

ISimDB* CreateSqliteConn(const char* db)
{
	CSimSqlite *pSqlite = new CSimSqlite();
	if (NULL != pSqlite)
	{
		if (0 != pSqlite->Open(0, 0, 0, db, 0, 0, 0, 0))
		{
			SAFE_RELEASE(pSqlite);
		}
	}
	return pSqlite;
}
#endif

ISimRecordSet* CreateRecordSet(ISimDB *pDB, const char *fmt, ...)
{
    CSimRecord *pMyRecord = new CSimRecord();
    if (NULL == pMyRecord)
    {
        return NULL;
    }

    //构建SQL语句
    char szSql[CSimMySql::MAX_SQL_BUFFER] = "";
    va_list args;
    va_start(args, fmt);
    int32_t cnt = vsnprintf(szSql, sizeof(szSql)-1, fmt, args);
    va_end(args);
    if (cnt<0)
    {
        SAFE_RELEASE(pMyRecord);
        return NULL;
    }

    //创建记录集
    if (!pMyRecord->Open(pDB, szSql))
    {
        SAFE_RELEASE(pMyRecord);
        return NULL;
    }
    return pMyRecord;
}

ISimRecordSet* LoadTable(ISimDB *pDB, const char *pszTable, const char*fmtCond, ...)
{
    CSimRecord *pMyRecord = new CSimRecord();
    if (NULL == pMyRecord)
    {
        return NULL;
    }

    //构建SQL语句
    char szSql[CSimMySql::MAX_SQL_BUFFER] = "";
    va_list args;
    va_start(args, fmtCond);
    int32_t cnt = vsnprintf(szSql, sizeof(szSql)-1, fmtCond, args);
    va_end(args);
    if (cnt<0)
    {
        SAFE_RELEASE(pMyRecord);
        return NULL;
    }

    //创建记录集
    if (!pMyRecord->LoadTable(pDB, pszTable, szSql))
    {
        SAFE_RELEASE(pMyRecord);
        return NULL;
    }
    return pMyRecord;
}



