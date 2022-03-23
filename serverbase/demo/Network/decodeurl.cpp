/*
 * =====================================================================================
 *
 *       Filename:  decodeurl.cpp
 *
 *    Description:  解析url
 *
 *        Version:  1.0
 *        Created:  12/12/2019 03:07:54 PM
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

int main(int argc, char **argv)
{
    CMyString url, host, cgi, param;
    if (argc < 2)
    {
        printf("syntax: %s <http-url>\n", argv[0]);
        return -1;
    }

    url = argv[1];
    bool ssl(false);
    int32_t port(0);
    bool ret = decode_url(url, host, port, cgi, param, ssl);
    printf("decode url [%s]\n", ret ? "succeed" : "fail");
    printf("url : [%s]\n", url.c_str());
    printf("host: [%s]\n", host.c_str());
    printf("port: [%d]\n", port);
    printf("cgi:  [%s]\n", cgi.c_str());
    printf("param:[%s]\n", param.c_str());

    printf("\n\ntest ok.\n");

    return 0;
}
