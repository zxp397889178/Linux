// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MYDB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MYDB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef _GFX_DB_DATABASE_H_
#define _GFX_DB_DATABASE_H_

#include "BaseCode.h"
#include "ISimDB.h"
#include "IMyField.h"
#include "IMyRecord.h"


//创建数据库连接
ISimDB* CreateMysqlConn(const char *user_name, const char *passwd, 
    const char *host, const char *db=0, int32_t port=3306, ISimDBEvent *pEvent = 0,
    const char *appname=0, const char *charset="utf8");

#if 0
//创建数据库连接
ISimDB* CreateOracleConn(const char *user_name, const char *passwd, 
    const char *host, const char *db="orcl", int32_t port=1521, ISimDBEvent *pEvent = 0,
    const char *appname=0, const char *charset="utf8");

// 创建sqlite连接
ISimDB* CreateSqliteConn(const char* db);
#endif

//创建可读写的记录集（若存在主键则主键字段不可写），但不回写到数据库
ISimRecordSet* CreateRecordSet(ISimDB *pDB, const char *fmt, ...);
//创建可读写的记录集（必须存在主键，否则系统会失败），可以回写到数据库看
ISimRecordSet* LoadTable(ISimDB *pDB, const char *pszTable, const char*fmtCond, ...);

#endif // _GFX_DB_DATABASE_H_
