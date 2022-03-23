#include "SimMysql.h"
#include "errmsg.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "SimMysqlEventWrapper.h"



inline bool IsConnectLost(uint32_t errid)
{
    return (CR_SERVER_GONE_ERROR==errid || CR_SERVER_LOST==errid || 
        CR_CONNECTION_ERROR==errid || CR_CONN_HOST_ERROR==errid ||
        CR_IPSOCK_ERROR==errid || CR_SOCKET_CREATE_ERROR==errid);
}

extern ISimDB* CreateMysqlConn(const char *user_name, const char *passwd, 
    const char *host, const char *db=0, int32_t port=0, ISimDBEvent *pEvent = 0,
    const char *appname=0, const char *charset="utf8");
//////////////////////////////////////////////////////////////////////////
//CSimMySql
CSimMySql::CSimMySql()
    : m_conn(0)
    , m_results(0)
    , m_row(0)
    , m_lengths(0)
    , m_field_num(0)
    , m_fields(0)
    , m_pEvent(NULL)
    , m_eResultStatus(ISimDB::statusDummy)
    , m_strSQL()
{
    m_lastHeartbeat = 0;
    m_nResultIndex = -1;
}

CSimMySql::~CSimMySql()
{
    this->Cancel();
    if (NULL != m_conn)
    {
        mysql_close(m_conn);
		delete m_conn;
        m_conn = NULL;
    }
}

void CSimMySql::release(void)
{
    delete this;
}

int32_t CSimMySql::Open(const char *user_name, const char *passwd, 
    const char *host, const char *db/* = NULL */, int32_t port/* =0 */, 
    ISimDBEvent *pEvent/* = NULL */, const char *appname/* =NULL */,
    const char *charset /*="utf8"*/ )
{
    UNUSED(appname);

    if (EMPTY_STR(user_name) || EMPTY_STR(passwd) || EMPTY_STR(host))
    {
        return -1;
    }

    if (port<=0) port=3306;
    char dbname[128] = "";
    safe_strcpy(dbname, sizeof(dbname), EMPTY_STR(db) ? "" : db);

	MYSQL *tmp_mysql = new MYSQL;
    mysql_init(tmp_mysql);

    int32_t ret=0;
    char reconnect = 1;
    ret = mysql_options(tmp_mysql, MYSQL_OPT_RECONNECT, &reconnect);

    uint32_t client_flags = CLIENT_MULTI_STATEMENTS;
    MYSQL* mysql_conn = mysql_real_connect(tmp_mysql, host, 
        user_name, passwd, dbname, port, NULL, client_flags);
    if (NULL == mysql_conn)
    {
        mysql_close(tmp_mysql);
		delete tmp_mysql;
        return -3;
    }

    //设置字符符为UTF8
    ret = mysql_set_character_set(mysql_conn, charset);
    DB_ASSERT(ret==0);
    m_strCurCharset = GetIconvCharset(charset);

    //连接生效
    m_conn = mysql_conn;
    m_pEvent = pEvent;
    if (NULL != m_pEvent)
    {
        pEvent->SetAppName(appname);
    }

    m_lastHeartbeat = Now();
    return 0;
}

void CSimMySql::BindEvent(ISimDBEvent *pEvent /* = NULL */)
{
    m_pEvent = pEvent;
}

void CSimMySql::Close(void)
{
    if (NULL != m_conn)
    {
        mysql_close(m_conn);
		delete m_conn;
        m_conn = NULL;
    }
}

ISimDB* CSimMySql::Duplicate(ISimDBEvent *pEvent)
{
    if (!m_conn)
        return NULL;

    return CreateMysqlConn(m_conn->user, m_conn->passwd, m_conn->host,
        m_conn->db, m_conn->port, pEvent, NULL, m_strCurCharset.c_str());
}

const char* CSimMySql::Cmd(const char *fmt, ...)
{
    if (NULL==fmt)
    {
        DB_ASSERT(0);
        return NULL;
    }

    char buffer[MAX_SQL_BUFFER] = "";
    va_list args;
    va_start(args, fmt);
    int32_t cnt = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

	if (cnt>0 && cnt<(int32_t)sizeof(buffer))
	{
		// 正常
	}
	else
	{
		m_eResultStatus = statusDummy;
		return NULL;
	}
#if 0
    //对字符集进行转义
    char buf2[MAX_SQL_BUFFER]="";
    mysql_real_escape_string(m_conn, buf2, buffer, cnt);
	printf("sql:[%s]\n", buf2);
    //m_strSQL.Append(buf2);
    m_strSQL.append(buf2, strlen(buf2));
#else
    UNUSED(cnt);
    //m_strSQL.Append(buffer);
	//printf("sql:[%s]\n", buffer);
    m_strSQL.append(buffer, strlen(buffer));
#endif

    m_eResultStatus = statusHasSql;
	return m_strSQL.c_str();
}

static void char_replace(std::string &buffer, uint8_t c, const std::string &dest)
{
	do {
		std::string::size_type pos = buffer.find_first_of(c);
		if (pos == std::string::npos)
		{
			break;
		}
		buffer.replace(pos, 1, dest);
	}while(true);
}
#if 0
static void str_replace(std::string &buffer, const std::string &src, const std::string &dest)
{
	do {
		std::string::size_type pos = buffer.find_first_of(src);
		if (pos == std::string::npos)
		{
			break;
		}
		buffer.replace(pos, src.length(), dest);
	}while(true);
}
#endif

const char* CSimMySql::SafeString(std::string &src)
{
	static const char s_szChar1[] = {(char)(0xef), (char)(0xbc), (char)(0x85), (char)(0x00) };  // '%'
	static const char s_szChar2[] = {(char)(0xef), (char)(0xbc), (char)(0x9b), (char)(0x00) };  // ';'
	static const char s_szChar3[] = {(char)0xe2, (char)0x80, (char)0x98, (char)0x00 };  // '
	static const char s_szChar4[] = {(char)0xe2, (char)0x80, (char)0x99, (char)0x00 };  // '

	static const char s_szOrgChar1 = 0x27;   //'
	static const char s_szOrgChar2 = 0x22;	 // "
	static const char s_szOrgChar3 = 0x5c;	 // 

#if 1
	uint32_t srcLen = src.length();
	bool bNewBuf(false);
	char buffer[2048] = "";
	char *pDest = &buffer[0];
	if (srcLen > sizeof(buffer)/2)
	{
		pDest = (char*)calloc(sizeof(char), srcLen*2);
		bNewBuf = true;
	}

	char_replace(src, s_szOrgChar1, s_szChar4);
	char_replace(src, s_szOrgChar2, s_szChar4);
	char_replace(src, s_szOrgChar3, "");
	while(true)
	{
		std::string::size_type pos = src.find_first_of('%');
		if (pos == std::string::npos)
		{
			break;
		}
		src.replace(pos, 1, "");
	}
	uint32_t ret = mysql_real_escape_string(m_conn, pDest, src.c_str(), srcLen);
	src.clear();
	src.assign(pDest, ret);
	if (bNewBuf)
	{
		free(pDest);
	}
	char_replace(src, '%', s_szChar1);
	char_replace(src, ';', s_szChar2);
	//char_replace(src, '_', "");
#else
	printf("replace 1\n");
	char_replace(src, 0x25, "％");  // %
	printf("replace 2\n");
	//char_replace(src, '\\', "");  // '\'    
	printf("replace 2.0\n");
	char_replace(src, 0x27, "\\'");  // '
	printf("replace 3\n");
	char_replace(src, 0x22, "\\'");  // "
	printf("replace 4\n");
	char_replace(src, 0x20, "");	// ' '
	printf("replace 5\n");
	printf("src=[%s]\n", src.c_str());
#endif
	return src.c_str();
}

int32_t CSimMySql::Exec(void)
{
    if (!m_conn)
        return -1;

    if (m_strSQL.empty())
    {
        return -1;
    }

    //将原始语句转换成utf8的字符集
    if (!m_strOutCharset.empty() && (m_strOutCharset != m_strCurCharset ))
    {
        char szTextTo[40960] = "";
        size_t cbTextTo = sizeof(szTextTo);
        char szTextFrom[40960] = "";
        size_t cbTextFrom = sizeof(szTextFrom);
        safe_strcpy(szTextFrom, cbTextFrom, m_strSQL.c_str());
        int32_t ret = Convert(m_strOutCharset.c_str(), m_strCurCharset.c_str(), 
            szTextTo, cbTextTo, szTextFrom, strlen(szTextFrom));
        if (0 == ret)
        {
            m_strSQL = szTextTo;
        }
    }

    CSimMySqlEventWrapper eventWrapper(m_pEvent, m_strSQL);
    // Zero if the statement was successful. Nonzero if an error occurred.
    int32_t ret = mysql_real_query(m_conn, m_strSQL.c_str(), m_strSQL.length());
	LogDebug("EXECUTE SQL [%s], result[%d]", m_strSQL.c_str(), ret);
    if (0 != ret)
    {
        //eventWrapper.SetError();
        m_eResultStatus = statusHasError;
//#if (defined(DEBUG) || defined(_DEBUG))
        int32_t nError(0);
        char szError[256] = "";
        nError = this->GetLastError(szError, sizeof(szError));
		LogErr("occur error[%d] %s when execute SQL [%s].", nError, szError, m_strSQL.c_str());
        //MyLogMsg(levelLogErr, "./log/sql_trace.log", "%s {error #%d: %s}", m_strSQL.c_str(), nError, szError);
//#endif
        m_strSQL.clear();
        return -2;
    }
    
    m_lastHeartbeat = Now();
    //获得返回的结果集行数
    my_ulonglong nCount = mysql_affected_rows(m_conn);
    if (nCount==(my_ulonglong)-1)
    {
        switch(eventWrapper.GetStyle())
        {
        case ISimDBEvent::eventDelete:
        case ISimDBEvent::eventInsert:
        case ISimDBEvent::eventUpdate:
            {
                eventWrapper.SetError();
                m_eResultStatus = statusHasError;
//#if (defined(DEBUG) || defined(_DEBUG))
                int32_t nError(0);
                char szError[256] = "";
                nError = this->GetLastError(szError, sizeof(szError));
				LogErr("occur error [%d] %s when execute SQL [%s].", nError, szError, m_strSQL.c_str());
//#endif
                m_strSQL.clear();
                return -2;
            }
            break;
        default:
            break;
        }
        nCount = 0;
    }
    m_eResultStatus = statusQuery;
	LogTrace("EXECUTE SQL [%s] count [%d]", m_strSQL.c_str(), (int32_t)nCount);
    m_strSQL.clear();
    return (int32_t)nCount;
}

int32_t CSimMySql::MoreExec(void)
{
//#if (defined(DEBUG) || defined(_DEBUG) )
    int32_t nLength = (int)m_strSQL.length();
    char * buf = new char[nLength+1];
    safe_strcpy(buf, nLength+1, m_strSQL.c_str());
    int32_t nTotalCount(0);
    bool bError(false);

	char *saved_ptr;
    char *token = strtok_r(buf, ";", &saved_ptr);
    while(token != NULL)
    {
        m_strSQL = token;
        int32_t nCount = this->Exec();
        if (nCount < 0)
        {
            bError = true;
            break;
        }
        else
        {
            nTotalCount += nCount;
        }
        token = strtok_r(NULL, ";", &saved_ptr);
    }
    SAFE_DELETE_ARRAY(buf);
    if (bError)
    {
        return -1;
    }
    return nTotalCount;
//#else
#if 0
//!!! 由于多条语句执行时存在BUG（出现错误时不一定会警告），暂时禁用多条语句执行的功能
    int32_t ret = this->Exec();
    if (ret<0)
    {
        return ret;
    }

    int32_t nTotalCount(0);
    int32_t nError(0);
    int32_t nIndex(-1);
    //取出多条结果
    do{
        ++nIndex;
        my_ulongint32_t nCount = mysql_affected_rows(m_conn);
        if (nCount == (my_ulonglong)-1)
        {
            --nError;
        }
        else
        {
            nTotalCount += nCount;
            MYSQL_RES *pRes = mysql_store_result(m_conn);
            if (pRes)
            {
                mysql_free_result(pRes);
            }
        }
    }while(!mysql_next_result(m_conn));
    if (nError<0)
    {
        return nError;
    }
    return nTotalCount;
#endif
}

bool CSimMySql::GetResult(void)
{
    if (!m_conn)
        return false;

#if (MYSQL_RESULT == RESULT_STORED)
    m_results = mysql_store_result(m_conn);
#else
    m_results = mysql_use_result(m_conn);
#endif

    if (NULL == m_results)
    {
		LogTrace("query has not result.");
        m_nResultIndex = -1;
        return false;
    }

    m_field_num = mysql_num_fields(m_results);
    m_fields = mysql_fetch_fields(m_results);

    //m_lengths = mysql_fetch_lengths(m_results);
    //必须先fetch_row之后才能获取每行的长度

    m_eResultStatus = statusHasResult;
    if (NULL != m_pEvent)
    {
        m_pEvent->OnNewResult(m_results, m_fields, m_field_num);
    }

    // 结果集序号指向下一个
    ++m_nResultIndex;

    return true;
}

bool CSimMySql::More(void)
{
    switch(m_eResultStatus)
    {
	case statusDummy:
		LogErr("ERR: sql is dummy.");
		return false;
    case statusHasSql:
        m_nResultIndex = -1;
        if (this->Exec() < 0)
        {
			LogErr("ERR: execute sql failure.");
            return false;
        }
        ; //继续执行下一条指令
    case statusQuery:
        if (!this->GetResult())
        {
            return false;
        }
        break;
    default:
        break;
    }

    int32_t more_result(0);
    do {
        m_row = mysql_fetch_row(m_results);
        if (m_row != NULL)
        {
            m_lengths = mysql_fetch_lengths(m_results);
            if (NULL != m_pEvent)
            {
                m_pEvent->OnNewRow(m_row, (uint32_t*)m_lengths);
            }
            return true;
        }
        else
        {
            ///NULL if there are no more rows to retrieve or if an error occurred
            uint32_t errid = mysql_errno(m_conn);
            if (IsConnectLost(errid) || CR_UNKNOWN_ERROR==errid)
            {
                m_eResultStatus = statusHasError;
                if (NULL != m_pEvent)
                {
                    m_pEvent->OnError(ISimDBEvent::eventFetchResult, "");
                }
                return false;
            }
            //continue;
        }

        // if there are no more rows to retrieve
        // 释放已分配的内存
        mysql_free_result(m_results);
        m_results = 0;
        m_fields = 0;
        m_field_num = 0;
        m_row = 0;

        my_bool more_results_set = mysql_more_results(m_conn);
        if (more_results_set)
        {
            more_result = mysql_next_result(m_conn);
            if (0 == more_result)
            {
                //Successful and there are more results
                if (!this->GetResult())
                {
                    //无后续结果集，主要是针对存储过程的优化
                    more_result = -1;
                }
            }
            else if (-1 == more_result)
            {
                //Successful and there are no more results
                ;
            }
            else
            {
                //An error occurred
                m_eResultStatus = statusHasError;
                if (NULL != m_pEvent)
                {
                    m_pEvent->OnError(ISimDBEvent::eventFetchResult, "");
                }
                return false;
            }
        }
        else
        {
            more_result = -1;
        }
        //more_result = -1;
    }while(0==more_result);
	m_eResultStatus = statusDummy;

    return false;
}

int32_t CSimMySql::GetLastError(char *buffer/* =NULL */, size_t cbBuffer/* =0 */)
{
    if (NULL == m_conn)
    {
        return -1;
    }

    uint32_t ret = mysql_errno(m_conn);
    if (NULL != buffer && cbBuffer != 0)
    {
        safe_strcpy(buffer, cbBuffer, mysql_error(m_conn));
    }

    this->Cancel();
    return (int)ret;
}

int32_t CSimMySql::GetData(int32_t field_index, byte_t *&buffer, size_t &cbBuffer)
{
    if (field_index>=(int)m_field_num || field_index<0)
    {
        return -1;
    }

    if (NULL == m_lengths)
        return -1;

    cbBuffer = m_lengths[field_index];
    buffer = (byte_t*)m_row[field_index];
    return 0;
}

int32_t CSimMySql::GetData(const char* field_name, byte_t *&buffer, size_t &cbBuffer)
{
    int32_t index = this->GetFieldIndex(field_name);
    if (-1 == index)
    {
        return -1;
    }

    return this->GetData((uint8_t)index, buffer, cbBuffer);
}

int32_t CSimMySql::GetFieldIndex(const char *field_name)
{
    if (m_eResultStatus != statusHasResult || NULL==m_fields || EMPTY_STR(field_name))
    {
        return -1;
    }

    for(uint8_t i=0; i<m_field_num; ++i)
    {
        if (!stricmp(m_fields[i].name, field_name)
            || !stricmp(m_fields[i].org_name, field_name)
            )
        {
            return i;
        }
    }
    return -1;
}

void CSimMySql::Cancel()
{
    if (m_eResultStatus != statusHasResult)
    {
        return;
    }

    if (0 != m_results)
    {
        mysql_free_result(m_results);
        m_results = 0;

        //多条语句的取消
        while(mysql_more_results(m_conn))
        {
            if (0 == mysql_next_result(m_conn))
            {
                m_results = mysql_store_result(m_conn);
                if (0 != m_results)
                {
                    mysql_free_result(m_results);
                    m_results = 0;
                }
            }
        }

    }
    //if (0 != m_results)
    //{
    //    mysql_free_result(m_results);
    //    m_results = 0;
    //}
    m_row = 0;
    m_fields = 0;
    m_lengths = 0;
    m_field_num = 0;
}

bool CSimMySql::IsDeleteSql(CMyString &strSql)
{
    CMyString strTemp = strSql.left(7 /*"DELETE "*/);
    strTemp.MakeUpper();
    return (strTemp == "DELETE ");
}

bool CSimMySql::IsUpdateSql(CMyString &strSql)
{
    CMyString strTemp = strSql.left(7 /*"UPDATE "*/);
    strTemp.MakeUpper();
    return (strTemp == "UPDATE ");
}

bool CSimMySql::IsInsertSql(CMyString &strSql)
{
    CMyString strTemp = strSql.left(7 /*"INSERT "*/);
    strTemp.MakeUpper();
    return (strTemp == "INSERT ");
}

void CSimMySql::SetHandler(ISimDBEvent *pEvent)
{
    m_pEvent = pEvent;
}

CMyString CSimMySql::GetString(const char *field_name)
{
    int32_t field_index = this->GetFieldIndex(field_name);
    if (field_index<0)
    {
        DB_ASSERT(0);
        return "";
    }
    return this->GetString(field_index);
}

CMyString CSimMySql::GetString(int32_t field_index)
{
	byte_t *buffer(NULL);
	size_t cbBuffer(0);
	int32_t ret = this->GetData(field_index, buffer, cbBuffer);
	if (ret != 0)
	{
		DB_ASSERT(0);
		return "";
	}
	CMyString result;
	result.assign((const char*)buffer, cbBuffer);
	return result;
}

const char* CSimMySql::GetString(const char *field_name, char *szText, size_t cbText)
{
    int32_t field_index = this->GetFieldIndex(field_name);
    if (field_index<0)
    {
        DB_ASSERT(0);
        return "";
    }
    return this->GetString(field_index, szText, cbText);
}

const char* CSimMySql::GetString(int32_t field_index, char *szText, size_t cbText)
{
    byte_t *buffer(NULL);
    size_t cbBuffer(0);
    int32_t ret = this->GetData(field_index, buffer, cbBuffer);
    if (ret != 0)
    {
        DB_ASSERT(0);
        return "";
    }
    if (cbText < cbBuffer)
    {
        DB_ASSERT(0);
        return "";
    }

    memset(szText, 0, cbText);
    memcpy(szText, buffer, cbBuffer);
    return szText;
}

time_t CSimMySql::GetTime(const char *field_name)
{
    int32_t field_index = this->GetFieldIndex(field_name);
    if (field_index<0)
    {
        DB_ASSERT(0);
        return (time_t)0;
    }
    return this->GetTime(field_index);
}

time_t CSimMySql::GetTime(int32_t field_index)
{
    byte_t *buffer(NULL);
    size_t cbBuffer(0);
    int32_t ret = this->GetData(field_index, buffer, cbBuffer);
    if (ret != 0)
    {
        DB_ASSERT(0);
        return (time_t)0;
    }

    time_t val = StrToTime((const char*)buffer);
    return val;
}

void CSimMySql::SetOutCharset(const char *pszCharset)
{
    if (EMPTY_STR(pszCharset))
    {
        return;
    }
    m_strOutCharset = GetIconvCharset(pszCharset);
}

bool CSimMySql::SetCharset(const char *pszCharset)
{
    if (EMPTY_STR(pszCharset))
    {
        return false;
    }

    int32_t ret = mysql_set_character_set(m_conn, pszCharset);
    if (ret == 0)
    {
        m_strCurCharset = GetIconvCharset(pszCharset);
        return true;
    }
    DB_ASSERT(0);
    return false;
}


int32_t CSimMySql::GetInt(const char *field_name)
{ 
	return this->GetValue<int32_t>(field_name, StrToLong); 
}

int32_t CSimMySql::GetInt(int32_t field_index)
{ 
	return this->GetValue<int32_t>(field_index, StrToLong); 
}

uint32_t CSimMySql::GetUInt(const char *field_name)
{
	return this->GetValue<uint32_t>(field_name, StrToUint32); 
}

uint32_t CSimMySql::GetUInt(int32_t field_index)
{ 
	return this->GetValue<uint32_t>(field_index, StrToUint32); 
}

int64_t CSimMySql::GetInt64(const char* field_name)
{ 
	return this->GetValue<int64_t>(field_name, StrToInt64); 
}

int64_t CSimMySql::GetInt64(int32_t field_index)
{ 
	return this->GetValue<int64_t>(field_index, StrToInt64); 
}

uint64_t CSimMySql::GetUInt64(const char* field_name)
{ 
	return this->GetValue<uint64_t>(field_name, StrToUint64); 
}

uint64_t CSimMySql::GetUInt64(int32_t field_index)
{ 
	return this->GetValue<uint64_t>(field_index, StrToUint64); 
}

float CSimMySql::GetFloat(const char *field_name)
{ 
	return (float)this->GetValue<double>(field_name, StrToFloat); 
}

float CSimMySql::GetFloat(int32_t field_index)
{ 
	return (float)this->GetValue<double>(field_index, StrToFloat); 
}

double CSimMySql::GetDouble(const char *field_name)
{ 
	return this->GetValue<double>(field_name, StrToDouble);
}

double CSimMySql::GetDouble(int32_t field_index)
{
	return this->GetValue<double>(field_index, StrToDouble); 
}

bool CSimMySql::GetFields(CMyFieldSet &fields)
{
    if (0 == m_fields)
    {
        return false;
    }
    fields.Init(m_fields, m_field_num);
    return true;
}

int32_t CSimMySql::Ping(uint32_t timeout)
{
    if (NULL == m_conn) return -1;
    uint64_t now_ = Now();
    if (timeout==0 || (now_ > m_lastHeartbeat + timeout*1000) )
    {
        mysql_ping(m_conn);
        m_lastHeartbeat = now_;
        return 1;
    }
    return 0;
}
