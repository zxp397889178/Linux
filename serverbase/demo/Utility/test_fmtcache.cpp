/*
 * =====================================================================================
 *
 *       Filename:  test_fmtcache.cpp
 *
 *    Description:  test CFmtCache
 *
 *        Version:  1.0
 *        Created:  2021年07月17日 17时50分35秒
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

int main(int argc, char **argv)
{
    CMyRedis redis;
    bool val = redis.Open("127.0.0.1", 6379, "", 600);
    if (!val)
    {
        printf("connect to redis failure.\n");
        return -1;
    }
    redis.Select(2);

    redis_cache::CFmtCache cache;
    cache.Init(redis, "test#username#%%s", "test#username#*");
    cache.SetTimeout(600);

    // 测试1
    printf("begin to write data.\n");
    CMyString strText("{\"source\":  2,\n\"cha\\nnel\":\"huawei\",\"data\":{\"nick\":\"alibaba\",\"uin\":\"100001\"},\"ext\":{\"EN\":\"dsdsad\",\"ARA\":\"سلوكسلوك\"}}");
    printf("origin text: [%s]\n", strText.c_str());

    CMyString key("yuzp");
    int32_t ret = cache.Push(key, strText);
    if (ret<0)
    {
        printf("push format cache data failure.\n");
        return -3;
    }
    printf("push format data succeed.\n");
    
    CMyString key2;
    CMyString data2;
    ret = cache.Pop(key, key2, data2);
    if (ret < 0)
    {
        printf("pop format cache data failure.\n");
        return -5;
    }
    printf("pop data succeed.\n");
    printf("key [%s]\n", key2.c_str());
    printf("data[%s]\n", data2.c_str());

    // 测试2
    printf("\n\n");
    Json::Value data1;
    data1["name"] = "hello";
    data1["age"] = 100;
    Json::Value ext1;
    ext1["Region"]="CN";
    ext1["ARA"] = "ﺲﻟﻮﻜﺴﻟﻮﻛ";
    data1["ext1"] = ext1;
    key = "fqx";
    ret = cache.Push(key, data1);
    if (ret<0)
    {
        printf("push json cache data failure.\n");
        return -3;
    }
    printf("push json data succeed.\n");
    
    key2.clear();
    data2.clear();
    ret = cache.Pop(key, key2, data2);
    if (ret < 0)
    {
        printf("pop json cache data failure.\n");
        return -5;
    }
    printf("pop data succeed.\n");
    printf("key [%s]\n", key2.c_str());
    printf("data[%s]\n", data2.c_str());

    // 测试3, binary
    printf("\n\n");
    tagBinData bin;
    bin.i1 = 1234;
    bin.i2 = 2345;
    bin.i3 = 2345678901234;
    bin.i4 = 'A';
    bin.i5 = -110;
    bin.i6 = 65534;
    printf("origin binary:");
    bin.dump();
    key = "aaabbb";
    ret = cache.PushBinary(key, (byte_t*)&bin, sizeof(bin));
    if (ret <0)
    {
        printf("push binary data failure\n");
        return -7;
    }
    printf("\n");

    tagBinData data3 = {0};
    key2.clear();
    CByteStream buff;
    ret = cache.PopBinary(key, key2, buff);
    if (ret <0)
    {
        printf("pop binary data failure\n");
        return -9;
    }
    buff.Read((void*)&data3, sizeof(data3));
    printf("transfer binary:");
    data3.dump();
    printf("\n");
    
    
    printf("test finished.\n");

    return 0;
}
