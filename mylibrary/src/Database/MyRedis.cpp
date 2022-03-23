/*
 * =====================================================================================
 *
 *       Filename:  MyRedis.cpp
 *
 *    Description:  wrapper of HiRedis
 *
 *        Version:  1.0
 *        Created:  04/24/2015 09:53:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <assert.h>
#include "BaseCode.h"
#include "MyRedis.h"

/////////////////////////////////////////////////////////////////////////////////////////
class CRedisReplyObjectGuard
{
public:
	CRedisReplyObjectGuard(redisReply *replyObj)
		: _reply(replyObj)
	{
	}
	~CRedisReplyObjectGuard()
	{
		if (NULL != _reply)
		{
			freeReplyObject(_reply);
            _reply = NULL;
		}
	}
private:
	redisReply*		_reply;
};


#define EXECUTE_REDIS_CMD(fmt, ...)		int32_t ret = this->ExecuteRedisCmd(pReply, fmt, __VA_ARGS__);\
										if (ret<0) { return ret; } \
										ret = 0; \
										CRedisReplyObjectGuard guard(pReply);

#define EXECUTE_REDIS_CMD_EX(fmt, ...)	int32_t ret = this->ExecuteRedisCmd(pReply, fmt, __VA_ARGS__);\
										if (ret<0) { return ret; }  ret = 0;

#define CHECK_RESULT(key,val)		switch(pReply->type) { \
									case REDIS_REPLY_STRING:\
									case REDIS_REPLY_STATUS:\
										if (strcasecmp(pReply->str, "OK")==0) { \
											LogTrace("SET %s=>%s OK", key, val); ret = 1; } \
										else { \
											LogErr("SET %s=>%s FAIL, error %s", key, val, pReply->str); ret=0; } \
										break;\
									case REDIS_REPLY_NIL: \
										LogTrace("NOT FOUND key %s", key); ret = 0; \
										break; \
									case REDIS_REPLY_INTEGER:\
										ret = (int32_t)pReply->integer;	\
										break; \
									default: \
										LogErr("FAIL TO SET %s=>%s, type=%d, str=[%s]", key, val, pReply->type, pReply->str); ret = -1; \
										break; \
									}

//////////////////////////////////////////////////////////////////////////////////////////

CMyRedis::CMyRedis()
	: m_pContext(NULL)
    , m_lastHeartbeat(0)
{
}

CMyRedis::~CMyRedis()
{
    this->Close();
}

void CMyRedis::Close(void)
{
	if (NULL != m_pContext)
	{
		redisFree(m_pContext);
		m_pContext = NULL;
	}
}

bool CMyRedis::Open(std::string const strHost, uint32_t port, 
        std::string const strPasswd, uint32_t timeout)
{
    this->Close();

	struct timeval tv = { timeout/1000, (timeout%1000)*1000 };
	m_pContext = redisConnectWithTimeout(strHost.c_str(), port, tv);
	if (NULL == m_pContext || m_pContext->err)
	{
		if (m_pContext != NULL)
		{
            printf("redis open failure, host:%s, port:%d err:%d, msg::%s\n", 
                    strHost.c_str(), port, m_pContext->err, m_pContext->errstr);
			LogErr("connect to redis-server [%s:%u] error: %s", 
					strHost.c_str(), port, m_pContext->errstr);
			redisFree(m_pContext);
            m_pContext = NULL;
		}
		else
		{
            printf("\n%sredis open failure, maybe redis not open.%s\n", _color_red, _color_end);
			LogErr("connect ro redis-server [%s:%u] error: can't allocate redis context.",
					strHost.c_str(), port);
		}
		return false;
	}

	LogInit("connect to redis-server [%s:%u] succeed.", strHost.c_str(), port);

    m_lastHeartbeat = Now();
    if (strPasswd.empty())
    {
        return true;
    }
    // URL: https://blog.csdn.net/grafx/article/details/54233047
    redisReply *pReply = (redisReply*)redisCommand(m_pContext, "AUTH %s", strPasswd.c_str());
    CRedisReplyObjectGuard guard(pReply);
    if (pReply->type == REDIS_REPLY_ERROR)
    {
        LogFatal("authorize redis-server [%s:%u] failure.", strHost.c_str(), port);
        return false;
    }
    LogInit("redis-server authoirize OK");
	return true;
}

bool CMyRedis::Select(int32_t dbIndex)
{
	redisReply *pReply = (redisReply*)redisCommand(m_pContext, "SELECT %d", dbIndex);
	if (NULL == pReply)
	{
		return false;
	}
    m_lastHeartbeat = Now();
	CRedisReplyObjectGuard guard(pReply);
	bool ret(false);
	if (pReply->type==REDIS_REPLY_STATUS && strcasecmp(pReply->str, "OK")==0)
	{
		LogTrace("select %d OK", dbIndex);
		ret = true;
	}
	else
	{
		LogErr("failed to select %d", dbIndex);
	}
	return ret;
}

int32_t CMyRedis::Ping(uint32_t timeout)
{
    uint64_t now_ = Now();
    if (timeout==0 || now_ >= (m_lastHeartbeat+timeout*1000))
    {
        ;  // 是需要ping的时间了
    }
    else
    {
        return 0;
    }

	redisReply *pReply = (redisReply*)redisCommand(m_pContext, "PING");
	if (NULL == pReply)
	{
		return -1;
	}
    m_lastHeartbeat = now_;
	CRedisReplyObjectGuard guard(pReply);

	bool ret(false);
	if (pReply->type==REDIS_REPLY_STATUS && strcasecmp(pReply->str, "PONG")==0)
	{
		ret = true;
	}
	return ret ? 1 : -1;
}


int32_t CMyRedis::Set(std::string const strKey, std::string const strValue, 
		uint32_t nTimeout, int32_t timeStyle)
{
#if 0
	redisReply *pReply(NULL);
	if (nTimeout == INFINITE)
	{
		EXECUTE_REDIS_CMD_EX("SET %s %s", strKey.c_str(), strValue.c_str());
	}
	else
	{
		if (timeStyle==0)
		{
			EXECUTE_REDIS_CMD_EX("SET %s %s PX %u", strKey.c_str(), strValue.c_str(), nTimeout);
		}
		else
		{
			EXECUTE_REDIS_CMD_EX("SET %s %s EX %u", strKey.c_str(), strValue.c_str(), nTimeout);
		}
	}
	CRedisReplyObjectGuard guard(pReply);

	int32_t ret(0);
	CHECK_RESULT(strKey.c_str(), strValue.c_str());
	return ret;
#else
    int32_t ret(0);
    if (nTimeout == INFINITE)
        ret = this->SetData(strKey, strValue);
    else
    {
        if (timeStyle==0)
        {
            ret = this->SetData(strKey, strValue, MICROSECOND, nTimeout);
        }
        else
        {
            ret = this->SetData(strKey, strValue, SECOND, nTimeout);
        }
    }
    return ret;
#endif
}

int32_t CMyRedis::Set(std::string const strKey, void *pData, size_t cbData, 
		uint32_t nTimeout, int32_t timeStyle)
{
#if 0
	redisReply *pReply(NULL);
	if (nTimeout == INFINITE)
	{
		pReply = (redisReply*)redisCommand(m_pContext, "SET %b %b", strKey.c_str(), strKey.length(), pData, cbData);
	}
	else
	{
		if (timeStyle==0)
		{
			pReply = (redisReply*)redisCommand(m_pContext, "SET %b %b PX %u", strKey.c_str(), strKey.length(), pData, cbData, nTimeout);
		}
		else
		{
			pReply = (redisReply*)redisCommand(m_pContext, "SET %b %b EX %u", strKey.c_str(), strKey.length(), pData, cbData, nTimeout);
		}
	}
    m_lastHeartbeat = Now();
	CRedisReplyObjectGuard guard(pReply);

	if (NULL == pReply)
	{
		return -2;
	}
	if (pReply->type == REDIS_REPLY_ERROR)
	{
		return -3;
	}

	int32_t ret(0);
	CHECK_RESULT(strKey.c_str(), "(binary data)");
	return ret;
#else
    int32_t ret(0);
    if (nTimeout == INFINITE)
        ret = this->SetBinary(strKey, (const byte_t*)pData, cbData);
    else
    {
        if (timeStyle==0)
        {
            ret = this->SetBinary(strKey, (const byte_t*)pData, cbData, MICROSECOND, nTimeout);
        }
        else
        {
            ret = this->SetBinary(strKey, (const byte_t*)pData, cbData, SECOND, nTimeout);
        }
    }
    return ret;
#endif
}

int32_t CMyRedis::Set(std::string const strKey, int64_t val, 
		uint32_t nTimeout, int32_t timeStyle)
{
#if 0
	redisReply *pReply(NULL);
	if (nTimeout == INFINITE)
	{
		EXECUTE_REDIS_CMD_EX("SET %s %lld", strKey.c_str(), val);
	}
	else
	{
		if (timeStyle==0)
		{
			EXECUTE_REDIS_CMD_EX("SET %s %lld PX %u", strKey.c_str(), val, nTimeout);
		}
		else
		{
			EXECUTE_REDIS_CMD_EX("SET %s %lld EX %u", strKey.c_str(), val, nTimeout);
		}
	}
    m_lastHeartbeat = Now();
	CRedisReplyObjectGuard guard(pReply);

	int32_t ret(0);
	CMyString strVal("%ld", val);
	CHECK_RESULT(strKey.c_str(), strVal.c_str());
	return ret;
#else
    CMyString strVal("%ld", val);
    size_t cbData = strVal.length();
    int32_t ret(0);
    if (nTimeout == INFINITE)
        ret = this->SetBinary(strKey, (const byte_t*)strVal.c_str(), cbData);
    else
    {
        if (timeStyle==0)
        {
            ret = this->SetBinary(strKey, (const byte_t*)strVal.c_str(), cbData, MICROSECOND, nTimeout);
        }
        else
        {
            ret = this->SetBinary(strKey, (const byte_t*)strVal.c_str(), cbData, SECOND, nTimeout);
        }
    }
    return ret;
#endif
}

int32_t CMyRedis::SetNumber(const CMyString &strKey, int64_t val, enum TimeStyle timeStyle, int32_t timeValue)
{
    CMyString strVal("%ld", val);
    return this->SetBinary(strKey, (const byte_t*)strVal.c_str(), strVal.length(), timeStyle, timeValue);
}

int32_t CMyRedis::SetNumber(const CMyString &strKey, int32_t val, enum TimeStyle timeStyle, int32_t timeValue)
{
    CMyString strVal("%d", val);
    return this->SetBinary(strKey, (const byte_t*)strVal.c_str(), strVal.length(), timeStyle, timeValue);
}

int32_t CMyRedis::SetNumber(const CMyString &strKey, uint32_t val, enum TimeStyle timeStyle, int32_t timeValue)
{
    CMyString strVal("%u", val);
    return this->SetBinary(strKey, (const byte_t*)strVal.c_str(), strVal.length(), timeStyle, timeValue);
}

int32_t CMyRedis::GetNumber(const CMyString &strKey, int32_t &val)
{
    int64_t val2(0);
    int32_t ret = this->GetNumber(strKey, val2);
    if (ret>0)
    {
        val = (int32_t)val2;
    }
    return ret;
}

int32_t CMyRedis::GetNumber(const CMyString &strKey, int64_t &val)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("GET %s", strKey.c_str());
	switch(pReply->type)
	{
	case REDIS_REPLY_STRING:
		if (NULL != pReply->str)
		{
			val = StrToInt64(pReply->str);
			ret = 1;
		}
		break;
	case REDIS_REPLY_NIL:
		LogTrace("not exists key [%s]", strKey.c_str());
		ret = 0;
		break;
	default:
		ret = -1;	
		LogErr("Failed to get %s.", strKey.c_str());
		break;
	}
	return ret;
}

int32_t CMyRedis::GetNumber(const CMyString &strKey, uint32_t &val)
{
    int64_t val2(0);
    int32_t ret = this->GetNumber(strKey, val2);
    if (ret>0)
    {
        val = (uint32_t)val2;
    }
    return ret;
}

int32_t CMyRedis::Get(std::string const strKey, std::string& strValue)
{
#if 0
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("GET %s", strKey.c_str());
	switch(pReply->type)
	{
	case REDIS_REPLY_STRING:
		if (NULL != pReply->str)
		{
			strValue.assign(pReply->str, pReply->len);
			ret = 1;
		}
		break;
	case REDIS_REPLY_NIL:
		LogTrace("not exists key [%s]", strKey.c_str());
		ret = 0;
		break;
	default:
		ret = -1;	
		LogErr("Failed to get %s.", strKey.c_str());
		break;
	}
	return ret;
#else
    CMyString str1;
    int32_t ret = this->GetData(strKey, str1);
    if (ret>0)
    {
        strValue = str1;
    }
    return ret;
#endif
}

#if 0
int32_t CMyRedis::Get(std::string const strKey, void* &pData, size_t &cbData)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("GET %s", strKey.c_str());
	switch(pReply->type)
	{
	case REDIS_REPLY_STRING:
		{
			cbData = pReply->len;
			pData = NULL;
			if (NULL != pReply->str)
			{
				pData = calloc(1, cbData+1);
				memcpy(pData, pReply->str, cbData);
				ret = 1;
			}
            else
            {
                cbData = 0;
                ret = 1;
            }
		}
		break;
	case REDIS_REPLY_NIL:
		LogTrace("not exists key [%s]", strKey.c_str());
		ret = 0;
		break;
	default:
		LogErr("Failed to get %s.", strKey.c_str());
		ret = -1;
		break;
	}
	return ret;
}


int32_t CMyRedis::getData(std::string const strKey, void *pData, size_t &cbData)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("GET %s", strKey.c_str());
	switch(pReply->type)
	{
	case REDIS_REPLY_STRING:
		{
            cbData = std::min(cbData, pReply->len);
			if (NULL != pReply->str)
			{
				memcpy(pData, pReply->str, cbData);
				ret = 1;
			}
            else
            {
                cbData = 0;
                ret = 1;
            }
		}
		break;
	case REDIS_REPLY_NIL:
		LogTrace("not exists key [%s]", strKey.c_str());
		ret = 0;
		break;
	default:
		LogErr("Failed to get %s.", strKey.c_str());
		ret = -1;
		break;
	}
	return ret;
}
#endif

int32_t CMyRedis::Get(std::string const strKey, int64_t &val)
{
    return this->GetNumber(strKey, val);
}

int32_t CMyRedis::SetData(const CMyString &strKey, const CMyString &strData, enum TimeStyle timeStyle, int32_t timeValue)
{
    return this->SetBinary(strKey, (const byte_t*)strData.c_str(), strData.length(), timeStyle, timeValue);
}

int32_t CMyRedis::GetData(const CMyString &strKey, CMyString &strData)
{
    CByteStream buff;
    int32_t ret = this->GetBinary(strKey, buff);
    if (ret == 1)
    {
        buff.Read(strData);
    }
    return ret;
}

int32_t CMyRedis::SetBinary(const CMyString &strKey, const byte_t *pData, size_t cbData, enum TimeStyle timeStyle, int32_t timeValue)
{
    const char *argv[5] = {0};
    size_t argvlen[5] = {0};

    if (NULL == m_pContext)
    {
        m_strLastError = "CMyRedis not initialized";
        return -1;
    }
    if (strKey.find(0x20) != std::string::npos)
    {
        m_strLastError.Format("key [%s] cannot contain spaces", strKey.c_str());
        return -1;
    }
    if (NULL==pData || cbData==0)
    {
        m_strLastError = "data can not be null";
        return -1;
    }

    argv[0] = "SET";
    argvlen[0] = strlen(argv[0]);

    argv[1] = strKey.c_str();
    argvlen[1] = strKey.length();

    argv[2] = (char*)pData;
    argvlen[2] = cbData;

    int32_t argv_cnt(3);
    // 设置超时时间
    if (timeStyle != INFINITE2)
    {
        argv[3] = (timeStyle == SECOND) ? "EX" : "PX";
        argvlen[3] = strlen(argv[3]);

        CMyString ts("%d", timeValue);
        argv[4] = ts.c_str();
        argvlen[4] = ts.length();

        argv_cnt = 5;
    }

    redisReply *pReply = (redisReply *)redisCommandArgv(m_pContext, argv_cnt, argv, argvlen);
    if (NULL==pReply)
    {
        m_strLastError.Format("save binary data failure");
        return -2;
    }
    m_lastHeartbeat = Now();
    CRedisReplyObjectGuard guard(pReply);

    int32_t ret(0);
    if (REDIS_REPLY_STATUS == pReply->type)
    {
        if (strcasecmp(pReply->str, "OK")==0)
        {
            m_strLastError.clear();
            ret = 1;
        }
        else
        {
            m_strLastError.Format("save binary data resp [%s]", pReply->str);
            ret = 0;
        }
    }
    return ret;
}

int32_t CMyRedis::GetBinary(const CMyString &strKey, CByteStream &buff)
{
    if (NULL == m_pContext)
    {
        m_strLastError = "CMyRedis not initialized";
        return -1;
    }
    if (strKey.find(0x20) != std::string::npos)
    {
        m_strLastError.Format("key [%s] cannot contain spaces", strKey.c_str());
        return -1;
    }

	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("GET %s", strKey.c_str());
	switch(pReply->type)
	{
	case REDIS_REPLY_STRING:
		{
			ret = 1;
            if (NULL != pReply->str)
            {
                buff.Write((const void*)pReply->str, (size_t)pReply->len);
			}
		}
		break;
	case REDIS_REPLY_NIL:
        m_strLastError.Format("not found key [%s]", strKey.c_str());
		LogTrace(m_strLastError.c_str());
		ret = 0;
		break;
	default:
        m_strLastError.Format("failed to get [%s]", strKey.c_str());
		LogErr(m_strLastError.c_str());
		ret = -1;
		break;
	}
	return ret;
}

int32_t CMyRedis::Del(std::string const strKey)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("DEL %s", strKey.c_str());
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::IsExists(std::string const strKey)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("EXISTS %s", strKey.c_str());
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::IncBy(std::string const strKey, int32_t incValue, int64_t &retVal)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("INCRBY %s %d",strKey.c_str(), incValue);
	retVal = (int64_t)pReply->integer;
	return 1;
}

int32_t CMyRedis::DecBy(std::string const strKey, int32_t decValue, int64_t &retVal)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("DECRBY %s %d",strKey.c_str(), decValue);
	retVal = (int64_t)pReply->integer;
	return 1;
}

int32_t CMyRedis::StrLen(std::string const strKey)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("STRLEN %s",strKey.c_str());
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::SetExpire(std::string const strKey, uint32_t timeout, int32_t timeStyle)
{
	redisReply *pReply(NULL);
	if (timeStyle == 0)
	{
	 	EXECUTE_REDIS_CMD_EX("PEXPIRE %s %u", strKey.c_str(), timeout);
	}
	else
	{
		EXECUTE_REDIS_CMD_EX("EXPIRE %s %u", strKey.c_str(), timeout);
	}
	CRedisReplyObjectGuard guard(pReply);
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::GetExpire(std::string const strKey, uint32_t &val)
{
	redisReply *pReply(NULL);

	EXECUTE_REDIS_CMD("TTL %s", strKey.c_str());

	val = (uint32_t)pReply->integer;
	return ret;
}

int32_t CMyRedis::Keys(std::string const strPatten, std::vector<std::string > &arKeys)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("KEYS %s", strPatten.c_str()); 
	ret = (int32_t)pReply->elements;
	if (ret > 0)
	{
		struct redisReply **pResults = pReply->element;
		for(int32_t index(0); index<ret; ++index)
		{
			std::string strKey = pResults[index]->str;
			arKeys.push_back(strKey);
		}
	}
	return ret;
}

int32_t CMyRedis::Scan(int32_t cursor, const CMyString &patten, int32_t count, std::vector<CMyString> &vecKey, int32_t &next_cursor)
{
    CMyString patten_str = patten.empty() ? "" : (" MATCH " + patten);
    CMyString count_str_1(" COUNT %d", count);
    CMyString count_str = count<=0 ? "" : count_str_1;
    CMyString cmd("SCAN %d", cursor);
    cmd += patten_str;
    cmd += count_str;
    int32_t ret = this->execScan(cmd, vecKey, next_cursor);
    return ret;
}

int32_t CMyRedis::Rename(const CMyString &fromKey, const CMyString &toKey, bool bOverrideNew)
{
    if (fromKey == toKey)
    {
        return 1;
    }

    int32_t result(-1);
    redisReply *pReply(NULL);
    if (bOverrideNew)
    {
        EXECUTE_REDIS_CMD("RENAME %s %s", fromKey.c_str(), toKey.c_str());
	    if (NULL != pReply->str)
        {
		    CMyString strMsg;
            strMsg.assign(pReply->str, pReply->len);
            if (strMsg =="OK")
            {
                result = 1;
            }
            else
            {
                result = 0;
            }
        }
        else
        {
            result = -1;
        }
    }  
    else
    {
        EXECUTE_REDIS_CMD("RENAMENX %s %s", fromKey.c_str(), toKey.c_str());
        result = pReply->integer;
    }
    return result;
}

int32_t CMyRedis::HashSet(std::string const strKey, std::string const strField, std::string const strValue)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HSET %s %s %s", strKey.c_str(), strField.c_str(), strValue.c_str());
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::HashSet(std::string const strKey, std::string const strField, void *pData, size_t cbData)
{
	redisReply *pReply(NULL);
	//EXECUTE_REDIS_CMD("HSET %s %s %b", strKey.c_str(), strField.c_str(), pData, cbData);
	pReply = (redisReply*)redisCommand(m_pContext, "HSET %s %s %b", strKey.c_str(), strField.c_str(), pData, cbData);
	if (NULL == pReply)
	{
		return -2;
	}
    m_lastHeartbeat = Now();
	CRedisReplyObjectGuard guard(pReply);

	if (pReply->type == REDIS_REPLY_ERROR)
	{
		pReply = NULL;
		return -3;
	}

	return (int32_t)pReply->integer;
}

int32_t CMyRedis::HashSet(std::string const strKey, std::string const strField, int64_t val)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HSET %s %s %lld", strKey.c_str(), strField.c_str(), val);
	return (int32_t)pReply->integer;
}


int32_t CMyRedis::GetHashFields(std::string const strKey, std::vector<CMyString> &allField)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HKEYS %s", strKey.c_str());
	ret = (int32_t)pReply->elements;
	if (ret > 0)
	{
		CMyString strKey;
		struct redisReply **pResults = pReply->element;
		for(int32_t index(0); index<ret; ++index)
		{
			strKey = pResults[index]->str;
			allField.push_back(strKey);
		}
	}
	return (int32_t)allField.size();
}

int32_t CMyRedis::GetHashValues(std::string const strKey, std::vector<CMyString> &allValue)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HVALS %s", strKey.c_str());
	ret = (int32_t)pReply->elements;
	if (ret > 0)
	{
		CMyString strValue;
		struct redisReply **pResults = pReply->element;
		for(int32_t index(0); index<ret; ++index)
		{
			strValue = pResults[index]->str;
			allValue.push_back(strValue);
		}
	}
	return (int32_t)allValue.size();
}

int32_t CMyRedis::HashSetAll(std::string const strKey, int64_t val)
{
	std::vector<CMyString> arField;
	int32_t ret2 = this->GetHashFields(strKey, arField);
	if (ret2 <= 0)
	{
		return ret2;
	}

	CMyString strCmd("HMSET %s", strKey.c_str());
	for(std::vector<CMyString>::iterator it(arField.begin()); it != arField.end(); ++it)
	{
		CMyString &field = *it;
		strCmd.Append(" %s %lld ", field.c_str(), val);  
	}
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("%s", strCmd.c_str());
	return (int32_t)arField.size();
}


int32_t CMyRedis::HashSetAll(std::string const strKey, std::string const strVal)
{
	std::vector<CMyString> arField;
	int32_t ret2 = this->GetHashFields(strKey, arField);
	if (ret2 <= 0)
	{
		return ret2;
	}

	CMyString strCmd("HMSET %s", strKey.c_str());
	for(std::vector<CMyString>::iterator it(arField.begin()); it != arField.end(); ++it)
	{
		CMyString &field = *it;
		strCmd.Append(" %s %s ", field.c_str(), strVal.c_str());  
	}
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("%s", strCmd.c_str());
	return (int32_t)arField.size();
}

int32_t CMyRedis::HashDelAll(std::string const strKey)
{
	return this->Del(strKey);
}

int32_t CMyRedis::HashGet(std::string const strKey, std::string const strField, std::string &strValue)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HGET %s %s", strKey.c_str(), strField.c_str());
	if (NULL != pReply->str)
	{
		strValue.assign(pReply->str, pReply->len);
		ret = 1;
	}
	return ret;
}

int32_t CMyRedis::HashGet(std::string const strKey, std::string const strField, void* &pData, size_t &cbData)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HGET %s %s", strKey.c_str(), strField.c_str());
	cbData = pReply->len;
	pData = NULL;
	if (0 != cbData && NULL != pReply->str)
	{
		pData = calloc(1, cbData+1);
		memcpy(pData, pReply->str, cbData);
		ret = 1;
	}
	return ret;
}

int32_t CMyRedis::HashGet(std::string const strKey, std::string const strField, int64_t &val)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HGET %s %s", strKey.c_str(), strField.c_str());
	if (NULL != pReply->str)
	{
		val = StrToUint64(pReply->str);
		ret = 1;
	}
	return ret;
}

int32_t CMyRedis::HashGetAll(std::string const strKey, std::map<CMyString, CMyString> &values)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HGETALL %s", strKey.c_str());
	ret = (int32_t)pReply->elements;
	if (ret > 0)
	{
		CMyString strKey;
		CMyString strValue;
		struct redisReply **pResults = pReply->element;
		for(int32_t index(0); index<ret; index+=2)
		{
			strKey = pResults[index]->str;
			strValue = pResults[index+1]->str;
			values.insert(std::make_pair(strKey, strValue));
		}
	}
	return ret/2;
}

int32_t CMyRedis::HashGetAll(std::string const strKey, std::map<CMyString, BinaryInfo> &values)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HGETALL %s", strKey.c_str());
	ret = (int32_t)pReply->elements;
	if (ret > 0)
	{
		CMyString strKey;
		struct redisReply **pResults = pReply->element;
		for (int32_t index(0); index<ret; index += 2)
		{
			strKey = pResults[index]->str;
			BinaryInfo binaryInfo;
			binaryInfo.cbData = pResults[index + 1]->len;
			binaryInfo.pData = calloc(1, binaryInfo.cbData + 1);
			memcpy(binaryInfo.pData, pResults[index + 1]->str, binaryInfo.cbData);

			values.insert(std::make_pair(strKey, binaryInfo));
		}
	}
	return ret / 2;
}

int32_t CMyRedis::HashDel(std::string const strKey, std::string const strField)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HDEL %s %s", strKey.c_str(), strField.c_str());
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::IsHashExists(std::string const strKey, std::string const strField)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HEXISTS %s %s", strKey.c_str(), strField.c_str());
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::IncHashValue(std::string const strKey, std::string const strField, 
		int32_t incValue, int64_t &retValue)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HINCRBY %s %s %d", strKey.c_str(), strField.c_str(), incValue);
	retValue = (int64_t)pReply->integer;
	return 1;
}

int32_t CMyRedis::GetFieldCount(std::string const strKey)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("HLEN %s", strKey.c_str());
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::SAdd(std::string const strKey, std::string const strMember)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("SADD %s %s", strKey.c_str(), strMember.c_str());
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::SAdd(std::string const strKey, int32_t value)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("SADD %s %d", strKey.c_str(), value);
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::SAdd(std::string const strKey, uint32_t value)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("SADD %s %u", strKey.c_str(), value);
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::SAdd(std::string const strKey, int64_t value)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("SADD %s %ld", strKey.c_str(), value);
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::SDel(std::string const strKey, std::string const strMember)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("SREM %s %s", strKey.c_str(), strMember.c_str());
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::SDel(std::string const strKey, int32_t value)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("SREM %s %d", strKey.c_str(), value);
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::SDel(std::string const strKey, uint32_t value)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("SREM %s %u", strKey.c_str(), value);
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::SDel(std::string const strKey, int64_t value)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("SREM %s %ld", strKey.c_str(), value);
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::SMembers(std::string const strKey, std::vector<std::string> &arMember)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("SMEMBERS %s", strKey.c_str());
	ret = (int32_t)pReply->elements;
	if (ret > 0)
	{
		struct redisReply **pResults = pReply->element;
		for(int32_t index(0); index<ret; ++index)
		{
			std::string strVal = pResults[index]->str;
			arMember.push_back(strVal);
		}
	}
	return ret;
}

int32_t CMyRedis::SIsExists(std::string const strKey, std::string const strMember)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("SISMEMBER %s %s", strKey.c_str(), strMember.c_str());
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::SCount(std::string const strKey)
{
	redisReply *pReply(NULL);
	EXECUTE_REDIS_CMD("SCARD %s", strKey.c_str());
	return (int32_t)pReply->integer;
}

int32_t CMyRedis::SDelKey(std::string const strKey)
{
	return this->Del(strKey);
}

int32_t CMyRedis::ExecuteRedisCmd(redisReply *&pReply, const char *fmt, ...)
{
	int32_t ret(0);

    m_strLastError.clear();
	uint32_t bufLen(8192);
	C8KBuffer tmpBuf(bufLen);
	va_list args;
	va_start(args, fmt);
	try
	{
		ret = vsnprintf(tmpBuf.data(), bufLen, fmt, args);
		if ( !(ret>=-1 && ret<(int32_t)bufLen) )
		{
            m_strLastError = "buffer oversize";
			return -1;
		}
	}
	catch(...)
	{
        m_strLastError = "invalid arguments";
		return -1;
	}

	pReply = (redisReply*)redisCommand(m_pContext, tmpBuf.data()); 
    m_lastHeartbeat = Now();
	if (NULL == pReply)
	{
        m_strLastError.Format("execute cmd [%s] failure", tmpBuf.data());
		return -2;
	}
	if (pReply->type == REDIS_REPLY_ERROR)
	{
        m_strLastError = pReply->str;
		freeReplyObject(pReply);
		pReply = NULL;
		return -3;
	}

	return 0;
}

int32_t CMyRedis::execScan(const CMyString &cmd, std::vector<CMyString> &vecKeys, int32_t &nCursor)
{
    if (NULL == m_pContext)
    {
        return -1;
    }
    
    redisReply* _reply = (redisReply*)::redisCommand(m_pContext, cmd.c_str());
    if (NULL == _reply)
    {
        return -3;
    }

    CRedisReplyObjectGuard guard(_reply);
    if (_reply->type != REDIS_REPLY_ARRAY)
    {
        return -5;
    }
    
    redisReply* sub_reply = (_reply->element[1]);
    int32_t len = sub_reply->elements;
    vecKeys.clear();
    vecKeys.reserve(len);
    for(int32_t i=0; i<len; ++i)
    {
        CMyString temp(sub_reply->element[i]->str, sub_reply->element[i]->len);
        vecKeys.push_back(temp);
    }

    CMyString strCursor(_reply->element[0]->str, _reply->element[0]->len);
    nCursor = strCursor.asInt();
    return 0;
}

int32_t CMyRedis::LPush(const CMyString &key_, const CMyString &value_, enum ContainDirection direct_)
{
    redisReply *pReply(NULL);
    int32_t ret(0);
    if (direct_ == CMyRedis::HEAD)
    {
        ret = this->ExecuteRedisCmd(pReply, "LPUSH %s %s", key_.c_str(), value_.c_str());
    }
    else
    {
        ret = this->ExecuteRedisCmd(pReply, "RPUSH %s %s", key_.c_str(), value_.c_str());
    }
    if (ret < 0)
    {
        // 出错
        return ret;
    }
    CRedisReplyObjectGuard guard(pReply);
    ret = (int32_t)pReply->integer;
    return ret;
}

int32_t CMyRedis::LPop(const CMyString &key_, CMyString &value_, enum ContainDirection direct_)
{
    redisReply *pReply(NULL);
    int32_t ret(0);
    if (direct_ == CMyRedis::HEAD)
    {
        ret = this->ExecuteRedisCmd(pReply, "LPOP %s", key_.c_str());
    }
    else
    {
        ret = this->ExecuteRedisCmd(pReply, "RPOP %s", key_.c_str());
    }
    if (ret < 0)
    {
        // 出错
        return ret;
    }
    CRedisReplyObjectGuard guard(pReply);
    switch(pReply->type)
    {
    case REDIS_REPLY_NIL:
        ret = 0;
        break;
    case REDIS_REPLY_STRING:
        value_ = pReply->str;
        ret = 1;
        break;
    case REDIS_REPLY_STATUS:
        if (strcasecmp(pReply->str, "OK")==0)
        {
            ret = 1;
        }
        else
        {
            ret = 0;
        }
        value_ = pReply->str;
        break;
    default:
        ret = 0;
        break;
    }
    return ret;

}

int32_t CMyRedis::LSize(const CMyString &key_)
{
    redisReply *pReply(NULL);
    int32_t ret(0);
    ret = this->ExecuteRedisCmd(pReply, "LLEN %s", key_.c_str());
    if (ret < 0)
    {
        // 出错
        return ret;
    }
    CRedisReplyObjectGuard guard(pReply);
    ret = (int32_t)pReply->integer;
    return ret;
}

int32_t CMyRedis::SetBinary(BinaryData &data, enum TimeStyle timeStyle, int32_t timeValue)
{
    return this->SetBinary(data.key, data.pData, data.cbData, timeStyle, timeValue);
}

class CMultiGuard
{
public:
    CMultiGuard() {}
    ~CMultiGuard()
    {
        for(std::set<char*>::iterator it(m_ptrs.begin());
                it != m_ptrs.end(); ++it)
        {
            char* pc = *it;
            delete [] pc;
        }
    }
    void push(char *ptr)
    {
        m_ptrs.insert(ptr);
    }

private:
    std::set<char*>  m_ptrs;
};

int32_t CMyRedis::SetBinary(VEC_BINARY &vecBinary)
{
    size_t total_col = vecBinary.size() * 2;
    std::vector<const char*> argv(total_col + 1);
    std::vector<size_t> argvlen(total_col +1);

    int32_t col(0);
    static char msetcmd[] = "MSET";
    argv[col] = msetcmd;
    argvlen[col] = sizeof(msetcmd)-1;

    CMultiGuard mguard;
    for(VEC_BINARY::iterator it(vecBinary.begin());
            it != vecBinary.end(); ++it)
    {
        BinaryData &data = *it;

        ++col;
        size_t keyLen = data.key.length();
        argvlen[col] = keyLen;
        char *pc = new char[keyLen+1];
        mguard.push(pc);
        argv[col] = pc;
        memset((void*)argv[col], 0, keyLen);
        memcpy((void*)argv[col], data.key.data(), keyLen);

        ++col;
        size_t dataLen = data.cbData;
        argvlen[col] = dataLen;
        pc = new char[dataLen+1];
        mguard.push(pc);
        argv[col] = pc;
        memset((void*)argv[col], 0, dataLen);
        memcpy((void*)argv[col], data.pData, dataLen);
    }

    redisReply *pReply = (redisReply *)redisCommandArgv(m_pContext, argv.size(), &(argv[0]), &(argvlen[0]) );
    if (NULL==pReply)
    {
        m_strLastError.Format("save binary data failure");
        return -2;
    }
    m_lastHeartbeat = Now();
    CRedisReplyObjectGuard guard(pReply);

    int32_t ret(0);
    if (REDIS_REPLY_STATUS == pReply->type)
    {
        if (strcasecmp(pReply->str, "OK")==0)
        {
            m_strLastError.clear();
            ret = 1;
        }
        else
        {
            m_strLastError.Format("save binary data resp [%s]", pReply->str);
            ret = 0;
        }
    }
    return ret;
}

