/*
 * =====================================================================================
 *
 *       Filename:  test_incrcache.cpp
 *
 *    Description:  test redis_cache::CIncrCache
 *
 *        Version:  1.0
 *        Created:  2021年07月22日 20时43分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "MyDB.h"
#include "MyUtility.h"

#pragma pack(push, 1)
struct tagBinData
{
    uint32_t    i1;
    int32_t     i2;
    int64_t     i3;
    char        i4;
    int8_t      i5;
    uint16_t    i6;

    void dump(void)
    {
        printf("i1:%u, i2:%d, i3:%ld, i4:%c, i5:%d, i6:%u\n",
                i1, i2, i3, i4, i5, i6);
    }
};
#pragma pack(pop)
typedef tagBinData  BinData;

int32_t main(int32_t argc, char *argv)
{
    CMyRedis redis;
    bool val = redis.Open("127.0.0.1", 6379, "", 600);
    if (!val)
    {
        printf("connect to redis failure.\n");
        return -1;
    }
    redis.Select(2);

    redis_cache::CIncrCache cache;
    cache.Init(redis, "incr#maxid", "incr#curid", "incr#id#%%016ld", "incr#id#0*");
    cache.SetTimeout(600);

    BinData data1 = {1, -123, 234567890123, 'A', 100, 120 };
    BinData data2 = {2, -234, 345678901234, 'B', 110, 119 };
    BinData data3 = {3, -345, 456789012356, 'C', 120, 114 };

    int32_t ret = cache.PushBinary((byte_t*)&data1, sizeof(data1));
    if (ret < 0)
    {
        printf("push increase binary data failure.\n");
        return -1;
    }
    ret = cache.PushBinary((byte_t*)&data2, sizeof(data2));
    if (ret < 0)
    {
        printf("push increase binary data failure.\n");
        return -3;
    }
    ret = cache.PushBinary((byte_t*)&data3, sizeof(data3));
    if (ret < 0)
    {
        printf("push increase binary data failure.\n");
        return -5;
    }

    printf("origin binary:\n\t");
    data1.dump(); printf("\t");
    data2.dump(); printf("\t");
    data3.dump();

    printf("\n");
    CByteStream buff;
    CMyString strKey;
    printf("read from redis:\n\t");
    do {
        ret = cache.PopBinary(strKey, buff);
        if (ret <0)
        {
            printf("pop binary data failure.\n");
            break;
        }
        if (ret == 0)
        {
            printf("not any binary data pending to pop.\n");
            break;
        }
        BinData data_1 = {0};
        buff.Read((void*)&data_1, sizeof(data_1));
        printf("key:[%s], data=>", strKey.c_str());
        data_1.dump();
        printf("\t");
    }while(true);
    if (ret < 0)
    {
        return -7;
    }

    printf("\n\n");
    printf("test finished.\n");


    return 0;
}


