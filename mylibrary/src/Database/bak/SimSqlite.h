/*
 * =====================================================================================
 *
 *       Filename:  SimSqlite.h
 *
 *    Description:  wrapper of sqlite3
 *
 *        Version:  1.0
 *        Created:  2015年05月31日 16时02分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _GFX_MYDB_SIMSQLITE_H_
#define _GFX_MYDB_SIMSQLITE_H_

#include "sqlite/sqlite3.h"
#include "ISimDB.h"
#include <assert.h>
#include "BaseCode.h"

class CSimSqlite : public ISimDB
{
public:
	CSimSqlite();
	virtual ~CSimSqlite();

public:
	virtual void release(void);

	//仅 db 有用，其他参数都忽略
	virtual int32_t Open(const char* user_name, const char* passwd,
			const char* host, const char* db, int32_t port, 
			ISimDBEvent* pEvent, const char* appname,
			const char* charset);
	virtual void BindEvent(ISimDBEvent* pEvent);
	virtual void Close(void);

	virtual ISimDB* Duplicate(ISimDBEvent* pEvent);
	virtual const char* Cmd(const char* fmt, ...);
	virtual int32_t Exec(void);
	virtual int32_t MoreExec(void);
	virtual bool More(void);
	virtual void Cancel(void);
	virtual int32_t GetData(const char* field_name, byte_t* &buffer, size_t &cbBuffer);
	virtual int32_t GetData(int32_t field_index, byte_t* &buffer, size_t &cbBuffer);
	virtual int32_t GetLastError(char *buffer, size_t cbBuffer);
	virtual void SetHandler(ISimDBEvent *pEvent);

public:
    //获取数据结果
    virtual CMyString GetString(const char *field_name);
    virtual CMyString GetString(int32_t field_index);
    virtual const char* GetString(const char *field_name, char *szText, size_t cbText);
    virtual const char* GetString(int32_t field_index, char *szText, size_t cbText);
    virtual time_t GetTime(const char *field_name);
    virtual time_t GetTime(int32_t field_index);
    virtual int32_t GetInt(const char *field_name);
    virtual int32_t GetInt(int32_t field_index);
    virtual uint32_t GetUInt(const char *field_name);
    virtual uint32_t GetUInt(int32_t field_index);
    virtual int64_t GetInt64(const char *field_name);
    virtual int64_t GetInt64(int32_t field_index);
    virtual uint64_t GetUInt64(const char *field_name);
    virtual uint64_t GetUInt64(int32_t field_index);
    virtual float GetFloat(const char *field_name);
    virtual float GetFloat(int32_t field_index);
    virtual double GetDouble(const char *field_name);
    virtual double GetDouble(int32_t field_index);

public:
	virtual void SetOutCharset(const char *pszCharset);
	virtual bool SetCharset(const char *pszCharset);

    virtual int32_t Ping(uint32_t timeout);
private:
	int32_t Open(const char *pszFile);
	int32_t ExecQuery(void);
	int32_t GetFieldIndex(const char *pszFieldName);

private:
	sqlite3*		m_pDB;
	sqlite3_stmt*	m_pStmt;
	CMyString		m_strDB;

	CMyString		m_strSQL;
	RESULT_STATUS	m_eResultStatus;

private:
	int32_t			m_nErrno;
	CMyString		m_strError;
};

#endif // _GFX_MYDB_SIMSQLITE_H_


