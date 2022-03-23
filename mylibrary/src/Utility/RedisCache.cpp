/*
 * =====================================================================================
 *
 *       Filename:  RedisCache.cpp
 *
 *    Description:  redis缓存管理器
 *
 *        Version:  1.0
 *        Created:  2021年07月17日 13时52分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "RedisCache.h"

namespace redis_cache{

////////////////////////////////////////////////////////////////////////
CFmtCache::CFmtCache()
    : m_pRedis(NULL)
    , m_timeout((int32_t)INFINITE)
{

}

CFmtCache::~CFmtCache()
{

}

CFmtCache::CFmtCache(CMyRedis &redis, const CMyString &keyFormat, const CMyString &keyPatten)
    : m_pRedis(&redis)
    , m_keyFormat(keyFormat)
    , m_keyPatten(keyPatten)
    , m_timeout((int32_t)INFINITE)
{

}

void CFmtCache::Init(CMyRedis &redis, const CMyString &keyFormat, const CMyString &keyPatten)
{
    m_pRedis = &redis;
    m_keyFormat = keyFormat;
    m_keyPatten = keyPatten;
}

int32_t CFmtCache::Init(CMyRedis &redis, CXmlFile &xml, const CMyString &sect)
{
    m_pRedis = &redis;
    int32_t ret(0);
    try
    {
        ret = xml.getString(sect, "keyFormat", m_keyFormat);
        if (ret < 0)
        {
            return ret;
        }
        ret = xml.getString(sect, "keyPatten", m_keyPatten);
        if (ret < 0)
        {
            return ret;
        }
        ret = xml.getInt32(sect, "timeout", m_timeout);
        if (ret<0)
        {
            m_timeout = (int32_t)INFINITE;
        }
    }
    catch(std::exception &ex)
    {
        LogErr("init CFmtCache failure, sect:%s, error:%s", sect.c_str(), ex.what());
        return -101;
    }
    catch(...)
    {
        LogErr("init CFmtCache failure, sect:%s", sect.c_str());
        return -103;
    }
    return 0;
}

int32_t CFmtCache::Push(const CMyString &key, const CMyString &data)
{
    if (NULL == m_pRedis)
    {
        return -1;
    }

    CMyString realKey(m_keyFormat.c_str(), key.c_str());
    int32_t ret(0);
    if (m_timeout==(int32_t)INFINITE)
    {
        ret = m_pRedis->SetData(realKey, data);
    }
    else
    {
        ret = m_pRedis->SetData(realKey, data, CMyRedis::SECOND, m_timeout);
    }
    return ret;
}

int32_t CFmtCache::Push(const CMyString &key, Json::Value &data)
{
    CJsonWrapper jr;
    jr.Load(data);
    CMyString str = jr.toString();
    return this->Push(key, str);
}

CMyString CFmtCache::GetRealKey(const CMyString &key)
{
    CMyString realKey(m_keyFormat.c_str(), key.c_str());
    return realKey;
}

int32_t CFmtCache::Get(const CMyString &key, CMyString &realKey, CMyString &data)
{
    if (NULL == m_pRedis)
    {
        return -1;
    }
    realKey.Format(m_keyFormat.c_str(), key.c_str());
    return m_pRedis->GetData(realKey, data);
}

int32_t CFmtCache::Pop(const CMyString &key, CMyString &realKey, CMyString &data)
{
    int32_t ret = this->Get(key, realKey, data);
    if (ret==1)
    {
        m_pRedis->Del(realKey);
    }
    return ret;
}

int32_t CFmtCache::Scan(int32_t count, std::vector<CMyString> &vecKey,
        int32_t &next_cursor, int32_t cursor)
{
    if (NULL == m_pRedis)
    {
        return -1;
    }
    next_cursor = 0;
    int32_t ret = m_pRedis->Scan(cursor, m_keyPatten, count, vecKey, next_cursor);
    return ret;
}

int32_t CFmtCache::Del(const CMyString &key)
{
    if (NULL == m_pRedis)
    {
        return -1;
    }
    CMyString realKey(m_keyFormat.c_str(), key.c_str());
    return m_pRedis->Del(realKey);
}

int32_t CFmtCache::PushBinary(const CMyString &key, byte_t *pData, size_t cbData)
{
    if (NULL == m_pRedis)
    {
        return -1;
    }

    CMyString realKey(m_keyFormat.c_str(), key.c_str());
    int32_t ret(0);
    if (m_timeout==(int32_t)INFINITE)
    {
        ret = m_pRedis->SetBinary(realKey, pData, cbData);
    }
    else
    {
        ret = m_pRedis->SetBinary(realKey, pData, cbData, CMyRedis::SECOND, m_timeout);
    }
    return ret;
}

int32_t CFmtCache::PopBinary(const CMyString &key, CMyString &realKey, CByteStream &buff)
{
    if (NULL == m_pRedis)
    {
        return -1;
    }
    realKey.Format(m_keyFormat.c_str(), key.c_str());
    return m_pRedis->GetBinary(realKey, buff);
}

////////////////////////////////////////////////////////////////////////

CIncrCache::CIncrCache()
    : m_pRedis(NULL)
    , m_timeout((int32_t)INFINITE)
{

}

CIncrCache::~CIncrCache()
{
}

CIncrCache::CIncrCache(CMyRedis &redis, const CMyString &maxKey, const CMyString &currKey, 
        const CMyString &keyFormat, const CMyString &keyPatten)
    : m_pRedis(&redis)
    , m_maxKey(maxKey)
    , m_currKey(currKey)
    , m_keyFormat(keyFormat)
    , m_keyPatten(keyPatten)
    , m_timeout((int32_t)INFINITE)
{

}
void CIncrCache::Init(CMyRedis &redis, const CMyString &maxKey, const CMyString &currKey, 
        const CMyString &keyFormat, const CMyString &keyPatten)
{
    m_pRedis = &redis;
    m_maxKey = maxKey;
    m_currKey = currKey;
    m_keyFormat = keyFormat;
    m_keyPatten = keyPatten;
}

int32_t CIncrCache::Init(CMyRedis &redis, CXmlFile &xml, const CMyString &sect)
{
    m_pRedis = &redis;
    int32_t ret(0);
    try
    {
        ret = xml.getString(sect, "maxKey", m_maxKey);
        if (ret < 0)
        {
            return ret;
        }
        ret = xml.getString(sect, "currKey", m_currKey);
        if (ret < 0)
        {
            return ret;
        }
        ret = xml.getString(sect, "keyFormat", m_keyFormat);
        if (ret < 0)
        {
            return ret;
        }
        ret = xml.getString(sect, "keyPatten", m_keyPatten);
        if (ret < 0)
        {
            return ret;
        }
        ret = xml.getInt32(sect, "timeout", m_timeout);
        if (ret < 0)
        {
            m_timeout = (int32_t)INFINITE;
        }
    }
    catch(std::exception &ex)
    {
        LogErr("init CIncrCache failure, sect:%s, error:%s", sect.c_str(), ex.what());
        return -101;
    }
    catch(...)
    {
        LogErr("init CIncrCache failure, sect:%s", sect.c_str());
        return -103;
    }
    return 0;
}

int32_t CIncrCache::Push(const CMyString &data)
{
    return this->PushBinary((byte_t*)data.c_str(), data.length());   
}


int32_t CIncrCache::Push(Json::Value &data)
{
    CJsonWrapper jr;
    jr.Load(data);
    CMyString str = jr.toString();
    return this->Push(str);
}

int32_t CIncrCache::Pop(CMyString &realKey, CMyString &data)
{
    CByteStream buff;
    int32_t ret = this->PopBinary(realKey, buff);
    if (ret < 0)
    {
        return ret;
    }
    if (!buff.Read(data))
    {
        return -100;
    }
    return ret; 
}

int32_t CIncrCache::PushBinary(byte_t* pData, size_t cbData)
{
    if (NULL == m_pRedis)
    {
        return -101;
    }

    int64_t maxId(0);
    int32_t ret = m_pRedis->IncBy(m_maxKey.c_str(), 1, maxId);
    if (ret <= 0)
    {
        // 出错
        return -103;
    }

    CMyString keyName(m_keyFormat.c_str(), maxId);
    if (m_timeout == (int32_t)INFINITE)
    {
        ret = m_pRedis->SetBinary(keyName, pData, cbData);
    }
    else
    {
        ret = m_pRedis->SetBinary(keyName, pData, cbData, CMyRedis::SECOND, m_timeout);
    }
    return ret;
}

int32_t CIncrCache::PopBinary(CMyString &realKey, CByteStream &buff)
{
    if (NULL == m_pRedis)
    {
        return -101;
    }

    int64_t maxId(0);
    int32_t ret = m_pRedis->Get(m_maxKey, maxId);
    if (ret <= 0)
    {
        // 无此最大key
        return -103;
    }

    int32_t result(0);
    int64_t currId(0);
    int32_t loop_count(64);
    do {
        // 获得当前消费者序号
        ret = m_pRedis->IncBy(m_currKey.c_str(), 1, currId);
        if (ret < 0)
        {
            // 系统出错
            result = -105;
            break;
        }
        else if (ret == 0)
        {
            currId = 1;
        }

        if (currId > maxId)
        {
            // 消费超前了，回退到生产者序号
            m_pRedis->Set(m_currKey, maxId);
            break;
        }

        // 消费者获得数据
        realKey.Format(m_keyFormat.c_str(), currId);
        ret = m_pRedis->GetBinary(realKey, buff);
        if (ret<0)
        {
            // 系统出错
            result = -107;
        }
        else if (ret==0)
        {
            // 当前消费者序号有可能被其他进程获得了
            RedoItem redo;
            redo.key = realKey;
            redo.count_left = REDO_LOOP_COUNT;
            m_vecRedo.push_back(redo);
        }
        else
        {
            m_pRedis->Del(realKey);
            result = 1;
        }
        break;
    }while(--loop_count>0);
    return result;
}

int32_t CIncrCache::Scan(int32_t count, std::vector<CMyString> &vecKey,
        int32_t &next_cursor, int32_t cursor)
{
    if (NULL == m_pRedis)
    {
        return -1;
    }
    next_cursor = 0;
    int32_t ret = m_pRedis->Scan(cursor, m_keyPatten, count, vecKey, next_cursor);
    return ret;
}

const char* CIncrCache::getIncrKey(CMyString &key)
{
    if (NULL == m_pRedis)
    {
        return NULL;
    }
    int64_t maxId(0);
    int32_t ret = m_pRedis->IncBy(m_maxKey.c_str(), 1, maxId);
    if (ret <=0)
    {
        return NULL;
    }
    key.Format(m_keyFormat.c_str(), maxId);
    return key.c_str();
}

int32_t CIncrCache::QueryRedoKeys(std::vector<CMyString> &vecKey)
{
    if (m_vecRedo.empty())
    {
        return 0;
    }

    for(REDO_VECTOR::iterator it(m_vecRedo.begin());
            it != m_vecRedo.end(); )
    {
        RedoItem &item = *it; 
        
        int32_t ret = m_pRedis->IsExists(item.key);
        if (ret == 1)
        {
            vecKey.push_back(item.key);
            it = m_vecRedo.erase(it);
            continue;
        }
        else if(-1==ret)
        {
            // 获得KEY出错，移除重做
            it = m_vecRedo.erase(it);
            continue;
        }

        --item.count_left;
        if (item.count_left<=0)
        {
            // 达到处理上限，移除重做
            it = m_vecRedo.erase(it);
            continue;
        }

        ++it;
    }
    return (int32_t)vecKey.size();
}

};

