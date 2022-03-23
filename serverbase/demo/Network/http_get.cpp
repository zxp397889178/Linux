/*
 * =====================================================================================
 *
 *       Filename:  http_get.cpp
 *
 *    Description:  test http_ex::CHttpClient
 *
 *        Version:  1.0
 *        Created:  2020年02月22日 11时13分22秒
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

void syntax(char *app)
{
    printf("syntax: %s <url>\n", app);
}


int main(int argc, char **argv)
{
    if (argc<2)
    {
        syntax(argv[1]);
        return -1;
    }

    Comm::OpenLog("http_test", 3, "/data/work/logs");
    httpex::CHttpClient http;
    http::CHttpRespone resp;
    int32_t ret = http.Get(argv[1], "", resp);
    printf("result:%s%d%s\n", _color_red, ret, _color_end);
    printf("headers:\n%s%s%s\n", _color_blue, resp.getFullHeader().c_str(), _color_end);
    
    CMyString strData;
    CByteStream &buff = resp.GetPost();
    buff.Read(strData);
    printf("respone:\n%s%s%s\n", _color_green, strData.c_str(), _color_end);

    printf("\ntest finished.\n");

    return 0;
}
