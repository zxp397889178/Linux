/*
 * =====================================================================================
 *
 *       Filename:  decode_url.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/07/2020 01:33:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "MyNet.h"

int test1(const CMyString &url, bool &ret)
{
    uint64_t t1 = Now(true);
    bool ssl(false);
    CMyString host;
    CMyString cgi;
    CMyString param;
    int32_t port;
    ret = decode_url(url, host, port, cgi, param, ssl);
    uint64_t t2 = Now(true);
    //printf("host[%s], port[%d], cgi[%s], param[%s], ssl[%s]\n",
    //        host.c_str(), port, cgi.c_str(), param.c_str(), ssl?"true":"false");

    return (t2-t1);
}

int test2(const CMyString &url, bool &ret)
{
    uint64_t t1 = Now(true);
    bool ssl(false);
    CMyString host;
    CMyString cgi;
    CMyString param;
    int32_t port;
    ret = decode_url_ex(url, host, port, cgi, param, ssl);
    uint64_t t2 = Now(true);

    //printf("host[%s], port[%d] cgi[%s], param[%s], ssl[%s]\n",
    //        host.c_str(), port, cgi.c_str(), param.c_str(), ssl?"true":"false");
    return (t2-t1);
}


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("syntax: %s <url>\n", argv[0]);
        return -1;
    }
    CMyString strUrl = argv[1];

#if 0
    bool ret1(false), ret2(false);
    int32_t t1 = test1(strUrl, ret1);
    int32_t t2 = test2(strUrl, ret2);

    printf("\nexec test1 %s, cost:%dus\n", ret1?"ok":"fail", t1);
    printf("\nexec test2 %s, cost:%dus\n", ret2?"ok":"fail", t2);
#else
    int32_t t1(0);
    int32_t t2(0);
    int32_t cnt(1000);
    bool ret(false);
    for(int i=0; i<cnt; ++i)
    {
        t1 += test1(strUrl, ret);
        t2 += test2(strUrl, ret);
    }
    printf("exec test1 cost:%.2lfus\n", t1*1.00/cnt);
    printf("exec test2 cost:%.2lfus\n", t2*1.00/cnt);
#endif
    return 0;
}

