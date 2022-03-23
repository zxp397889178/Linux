/*
 * =====================================================================================
 *
 *       Filename:  SimSqlite.cpp
 *
 *    Description:  wrapper of sqlite3
 *
 *        Version:  1.0
 *        Created:  2015年05月31日 16时11分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *	// http://blog.csdn.net/zlhy_/article/details/8707694
 * =====================================================================================
 */

#include <stdlib.h>
#include "SimSqlite.h"
#include "Database.h"

CSimSqlite::CSimSqlite()
	: m_pDB(NULL)
	, m_pStmt(NULL)
	, m_strDB()
	, m_strSQL()
	, m_eResultStatus(ISimDB::statusDummy)
	, m_nErrno(0)
	, m_strError()
{

}

CSimSqlite::~CSimSqlite()
{
	this->Close();
}

void CSimSqlite::release(void)
{
	delete this;
}

int32_t CSimSqlite::Open(const char* user_name, const char* passwd,
		const char* host, const char* db, int32_t port,
		ISimDBEvent* pEvent, const char* appname, const char* charset)
{
	int32_t ret = this->Open(db);
	return ret;
}

int32_t CSimSqlite::Open(const char *pszFile)
{
	int32_t ret = sqlite3_open(pszFile, &m_pDB);
	if (ret != SQLITE_OK)
	{
		return -1;
	}
	m_strDB = pszFile;

	return 0;
}

void CSimSqlite::BindEvent(ISimDBEvent* pEvent)
{
	//TODO
}

void CSimSqlite::Close(void)
{
	if (NULL != m_pDB)
	{
		int32_t ret = sqlite3_close(m_pDB);
		if (ret != SQLITE_OK)
		{
			LogErr("close sqlite db failure.");
		}
		m_pDB = NULL;
	}
	m_eResultStatus = statusDummy;
}

ISimDB* CSimSqlite::Duplicate(ISimDBEvent* pEvent)
{
	if (NULL == m_pDB)
	{
		return NULL;
	}

	return CreateSqliteConn(m_strDB.c_str());
}

const char* CSimSqlite::Cmd(const char* fmt, ...)
{
	if (NULL==fmt)
	{
		return "";
	}

	char buffer[MAX_SQL_BUFFER] = "";
	va_list args;
	va_start(args, fmt);
	int32_t cnt = vsnprintf(buffer, sizeof(buffer)-1, fmt, args);
	va_end(args);
	// TODO: 需要检查cnt的值

	m_strSQL.append(buffer, strlen(buffer));

	m_eResultStatus = statusHasSql;

	return m_strSQL.c_str();
}

int32_t CSimSqlite::Exec(void)
{
	if (NULL == m_pDB)
	{
		m_strError = "database not open";
		m_nErrno = SQLITE_EMPTY;
		return -1;
	}
	if (m_strSQL.empty() || m_eResultStatus != statusHasSql)
	{
		m_strError = "empty sql";
		m_nErrno = SQLITE_EMPTY;
		return -1;
	}
	// 字符集处理
	//
	
	//判断否为SELECT语句
	CMyString strTest(m_strSQL);
	strTest.MakeUpper();
	if (strTest.find("SELECT") != std::string::npos)
	{
		return this->ExecQuery();
	}

	m_nErrno = 0;
	m_strError.clear();
	// 非SELECT语句，直接执行
	char* szError = NULL;
	int32_t ret = sqlite3_exec(m_pDB, m_strSQL.c_str(), 0, 0, &szError);
	if (ret == SQLITE_OK)
	{
		m_strSQL.clear();
		m_eResultStatus = statusDummy;
		return sqlite3_changes(m_pDB);
	}

	m_strSQL.clear();
	m_eResultStatus = statusHasError;
	m_strError = szError;
	m_nErrno = ret;
	return -1;
}


int32_t CSimSqlite::MoreExec(void)
{
	return this->Exec();
}

bool CSimSqlite::More(void)
{
	switch(m_eResultStatus)
	{
	case statusDummy:
		return false;
	case statusHasError:
		return false;
	case statusHasSql:
		if(this->ExecQuery()<0)
		{
			return false;
		}
		break;
	default:
		break;
	}

	int32_t ret = sqlite3_step(m_pStmt);
	if (ret == SQLITE_DONE)
	{
		// 到查询的未尾
		m_eResultStatus = statusDummy;
		sqlite3_finalize(m_pStmt);
		m_pStmt = NULL;
		return false;	
	}
	else if (ret == SQLITE_ROW)
	{
		// 有结果
		m_eResultStatus = statusHasResult;
		return true;
	}
	ret = sqlite3_finalize(m_pStmt);
	m_pStmt = NULL;
	m_eResultStatus = statusHasError;

	return false;
}

int32_t CSimSqlite::ExecQuery(void)
{
	if (NULL == m_pDB || m_strSQL.empty() || m_eResultStatus != statusHasSql)
	{
		return -1;
	}

	m_nErrno = 0;
	m_strError.clear();
	const char* szTail(0);
	int32_t ret = sqlite3_prepare(m_pDB, m_strSQL.c_str(), -1, &m_pStmt, &szTail);
	if (ret != SQLITE_OK)
	{
		m_strSQL.clear();
		m_eResultStatus = statusDummy;
		return -1;
	}
	m_strSQL.clear();
	m_eResultStatus = statusQuery;
	return 0;
}

void CSimSqlite::Cancel(void)
{
	if (NULL != m_pStmt)
	{
		sqlite3_finalize(m_pStmt);
		m_pStmt = NULL;
	}
	m_nErrno = 0;
	m_strError.clear();
	m_eResultStatus = statusDummy;
}

int32_t CSimSqlite::GetFieldIndex(const char *pszFieldName)
{
	if (m_eResultStatus != statusHasResult || NULL == m_pStmt 
			|| NULL==pszFieldName || '\0'==pszFieldName[0])
	{
		return -1;
	}
	for(int32_t idx(0); idx<sqlite3_column_count(m_pStmt); ++idx)
	{
		CMyString fname = sqlite3_column_name(m_pStmt, idx);
		if (!strcasecmp(fname.c_str(), pszFieldName))
		{
			return idx;
		}
	}
	return -2;
}

int32_t CSimSqlite::GetData(const char* field_name, byte_t* &buffer, size_t &cbBuffer)
{
	int32_t index = this->GetFieldIndex(field_name);
	if (index<0)
	{
		return -1;
	}
	return this->GetData(index, buffer, cbBuffer);
}

int32_t CSimSqlite::GetData(int32_t field_index, byte_t* &buffer, size_t &cbBuffer)
{
	if (NULL == m_pStmt || m_eResultStatus != statusHasResult || 
			field_index<0 || field_index>=sqlite3_column_count(m_pStmt))
	{
		return -1;
	}
	const byte_t* pc = sqlite3_column_text(m_pStmt, field_index);
	cbBuffer = sqlite3_column_bytes(m_pStmt, field_index);
	if (cbBuffer > 0 && NULL != pc)
	{
		buffer = (byte_t*)calloc(1, cbBuffer+10);
		memcpy(buffer, pc, cbBuffer);
	}
	return 0;
}

int32_t CSimSqlite::GetLastError(char *buffer, size_t cbBuffer)
{
	if (!m_strError.empty())
	{
		if (m_nErrno == SQLITE_OK)
		{
			buffer[0]='\0';
			return 0;
		}
	}
	else
	{
		m_nErrno = sqlite3_errcode(m_pDB);
		m_strError = sqlite3_errmsg(m_pDB);
	}
	safe_strcpy(buffer, cbBuffer, m_strError.c_str());
	return m_nErrno;
}

void CSimSqlite::SetHandler(ISimDBEvent *pEvent)
{
	// TODO
}

CMyString CSimSqlite::GetString(const char *field_name)
{
	int32_t index = this->GetFieldIndex(field_name);
	if (index<0)
	{
		return "";
	}
	return this->GetString(index);
}

CMyString CSimSqlite::GetString(int32_t field_index)
{
	if (NULL == m_pStmt || m_eResultStatus != statusHasResult || 
			field_index<0 || field_index>=sqlite3_column_count(m_pStmt))
	{
		return "";
	}
	const byte_t* pc = sqlite3_column_text(m_pStmt, field_index);
	int32_t dataLen = sqlite3_column_bytes(m_pStmt, field_index);
	CMyString result;
	if (NULL != pc)
	{
		result.assign((const char*)pc, dataLen);
	}
	return result;
}

const char* CSimSqlite::GetString(const char *field_name, char *szText, size_t cbText)
{
	int32_t index = this->GetFieldIndex(field_name);
	if (index<0)
	{
		return NULL;
	}
	return this->GetString(index, szText, cbText);
}

const char* CSimSqlite::GetString(int32_t field_index, char *szText, size_t cbText)
{
	if (NULL == m_pStmt || m_eResultStatus != statusHasResult || 
			field_index<0 || field_index>=sqlite3_column_count(m_pStmt))
	{
		return NULL;
	}
	const byte_t* pc = sqlite3_column_text(m_pStmt, field_index);
	int32_t dataLen = sqlite3_column_bytes(m_pStmt, field_index);
	dataLen = std::min((int32_t)(cbText-1), dataLen);
	if (NULL != pc)
	{
		memcpy(szText, pc, dataLen);
	}
	return szText;
}

time_t CSimSqlite::GetTime(const char *field_name)
{
	int32_t index = this->GetFieldIndex(field_name);
	if (index<0)
	{
		return time_t(0);
	}
	return this->GetTime(index);
}

time_t CSimSqlite::GetTime(int32_t field_index)
{
	if (NULL == m_pStmt || m_eResultStatus != statusHasResult || 
			field_index<0 || field_index>=sqlite3_column_count(m_pStmt))
	{
		return time_t(0);
	}
	sqlite3_int64 val = sqlite3_column_int64(m_pStmt, field_index);
	time_t tv = (time_t)val;
	return tv;
}

int32_t CSimSqlite::GetInt(const char *field_name)
{
	int32_t index = this->GetFieldIndex(field_name);
	if (index<0)
	{
		return time_t(0);
	}
	return this->GetInt(index);
}

int32_t CSimSqlite::GetInt(int32_t field_index)
{
	if (NULL == m_pStmt || m_eResultStatus != statusHasResult || 
			field_index<0 || field_index>=sqlite3_column_count(m_pStmt))
	{
		return -1;
	}
	return sqlite3_column_int(m_pStmt, field_index);
}

uint32_t CSimSqlite::GetUInt(const char *field_name)
{
	int32_t index = this->GetFieldIndex(field_name);
	if (index<0)
	{
		return 0;
	}
	return this->GetUInt(index);
}

uint32_t CSimSqlite::GetUInt(int32_t field_index)
{
	if (NULL == m_pStmt || m_eResultStatus != statusHasResult || 
			field_index<0 || field_index>=sqlite3_column_count(m_pStmt))
	{
		return 0;
	}
	return (uint32_t)sqlite3_column_int(m_pStmt, field_index);
}

int64_t CSimSqlite::GetInt64(const char *field_name)
{
	int32_t index = this->GetFieldIndex(field_name);
	if (index<0)
	{
		return 0;
	}
	return this->GetInt64(index);
}

int64_t CSimSqlite::GetInt64(int32_t field_index)
{
	if (NULL == m_pStmt || m_eResultStatus != statusHasResult || 
			field_index<0 || field_index>=sqlite3_column_count(m_pStmt))
	{
		return 0;
	}
	return (int64_t)sqlite3_column_int64(m_pStmt, field_index);
}

uint64_t CSimSqlite::GetUInt64(const char *field_name)
{
	int32_t index = this->GetFieldIndex(field_name);
	if (index<0)
	{
		return 0;
	}
	return this->GetUInt64(index);
}

uint64_t CSimSqlite::GetUInt64(int32_t field_index)
{
	if (NULL == m_pStmt || m_eResultStatus != statusHasResult || 
			field_index<0 || field_index>=sqlite3_column_count(m_pStmt))
	{
		return 0;
	}
	return (uint64_t)sqlite3_column_int64(m_pStmt, field_index);
}

float CSimSqlite::GetFloat(const char *field_name)
{
	int32_t index = this->GetFieldIndex(field_name);
	if (index<0)
	{
		return 0.0;
	}
	return this->GetFloat(index);
}

float CSimSqlite::GetFloat(int32_t field_index)
{
	if (NULL == m_pStmt || m_eResultStatus != statusHasResult || 
			field_index<0 || field_index>=sqlite3_column_count(m_pStmt))
	{
		return 0.0;
	}
	return (float)sqlite3_column_double(m_pStmt, field_index);
}

double CSimSqlite::GetDouble(const char *field_name)
{
	int32_t index = this->GetFieldIndex(field_name);
	if (index<0)
	{
		return 0.0;
	}
	return this->GetDouble(index);
}

double CSimSqlite::GetDouble(int32_t field_index)
{
	if (NULL == m_pStmt || m_eResultStatus != statusHasResult || 
			field_index<0 || field_index>=sqlite3_column_count(m_pStmt))
	{
		return 0.0;
	}
	return sqlite3_column_double(m_pStmt, field_index);
}

void CSimSqlite::SetOutCharset(const char *pszCharset)
{
	// TODO
}

bool CSimSqlite::SetCharset(const char *pszCharset)
{
	// TODO
	return false;
}

int32_t CSimSqlite::Ping(uint32_t timeout)
{
    return 0;
}

