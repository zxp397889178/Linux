/*
 * =====================================================================================
 *
 *       Filename:  RedisCache.h
 *
 *    Description:  redis缓存键值
 *
 *        Version:  1.0
 *        Created:  2021年07月17日 11时52分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "BaseCode.h"
#include "MyDB.h"

namespace redis_cache {

// 通用型缓存管理器，key必须唯一
class CFmtCache
{
public:
    CFmtCache();
    CFmtCache(CMyRedis &redis, const CMyString &keyFormat, const CMyString &keyPatten);
    ~CFmtCache();

    void Init(CMyRedis &redis, const CMyString &keyFormat, const CMyString &keyPatten);
    int32_t Init(CMyRedis &redis, CXmlFile &xml, const CMyString &sect);
    void SetTimeout(int32_t val) 
    { 
        if(val<=0) m_timeout=(int32_t)INFINITE; else m_timeout=val; 
    }


    int32_t Push(const CMyString &key, const CMyString &data);
    int32_t Push(const CMyString &key, Json::Value &data);
    int32_t Pop(const CMyString &key, CMyString &realKey, CMyString &data);

    int32_t PushBinary(const CMyString &key, byte_t *pData, size_t cbData);
    int32_t PopBinary(const CMyString &key, CMyString &realKey, CByteStream &buff);

    int32_t Get(const CMyString &key, CMyString &realKey, CMyString &data);
    int32_t Del(const CMyString &key);

    int32_t Scan(int32_t count, std::vector<CMyString> &vecKey 
            ,int32_t &next_cursor, int32_t cursor=0);

public:
    CMyString GetRealKey(const CMyString &key);
    CMyString& getPatten(void) { return m_keyPatten; }

    CMyRedis* getRedis(void) { return m_pRedis; }
    int32_t   getTimeout(void) { return m_timeout; }

private:
    CMyRedis*       m_pRedis;
    CMyString       m_keyFormat;
    CMyString       m_keyPatten;
    int32_t         m_timeout;      // 超时的时间，以秒计算,-1表示不限
};

// 自增型缓存管理器，key以数值形式递增
class CIncrCache
{
    enum { REDO_LOOP_COUNT = 10, };  // 轮空时检查的最大次数 
    struct RedoItem
    {
        CMyString   key;
        int32_t     count_left;
    };
    typedef std::vector<RedoItem>   REDO_VECTOR;
public:
    CIncrCache();
    CIncrCache(CMyRedis &redis, const CMyString &maxKey, const CMyString &currKey, 
            const CMyString &keyFormat, const CMyString &keyPatten);
    ~CIncrCache();

    void Init(CMyRedis &redis, const CMyString &maxKey, const CMyString &currKey, 
            const CMyString &keyFormat, const CMyString &keyPatten);
    int32_t Init(CMyRedis &redis, CXmlFile &xml, const CMyString &sect);
    void SetTimeout(int32_t val)
    {
        if (val<=0) m_timeout = (int32_t)INFINITE; else m_timeout = val;
    }

    int32_t Push(const CMyString &data);
    int32_t Push(Json::Value &data);
    int32_t Pop(CMyString &realKey, CMyString &data);

    int32_t PushBinary(byte_t* pData, size_t cbData);
    int32_t PopBinary(CMyString &realKey, CByteStream &buff);

    int32_t Scan(int32_t count, std::vector<CMyString> &vecKey,
            int32_t &next_cursor, int32_t cursor = 0);

    // 检查有遗漏的KEY，定时获得KEY，然后重新获取
    int32_t QueryRedoKeys(std::vector<CMyString> &vecKey);

public:
    CMyString& getPatten(void)  { return m_keyPatten; }
    CMyString& getMaxKey(void)  { return m_maxKey; }
    CMyString& getCurrKey(void) { return m_currKey; }

    // 获得增量的key的名称,NULL表示出错
    const char* getIncrKey(CMyString &key);

    CMyRedis* getRedis(void)    { return m_pRedis; }
    int32_t   getTimeout(void)  { return m_timeout; }

private:
    CMyRedis*       m_pRedis;
    CMyString       m_maxKey;
    CMyString       m_currKey;
    CMyString       m_keyFormat;
    CMyString       m_keyPatten;
    int32_t         m_timeout;

private:
    REDO_VECTOR     m_vecRedo;
};

};


