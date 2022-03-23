/*
 * =====================================================================================
 *
 *       Filename:  http_sendfile.cpp
 *
 *    Description:  http 发送文件
 *
 *        Version:  1.0
 *        Created:  2020年02月14日 20时13分04秒
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
    printf("syntax: %s <url> <file-name>\n", argv[0]);
    if (argc < 3)
    {
        return -1;
    }
    CMyString strUrl = argv[1];
    CMyString strFile = argv[2];

    Comm::OpenLog("sendfile", 3, "/data/work/logs");

    http::CHttpRespone resp;
    int32_t ret = HttpSendFile(strUrl, strFile, resp);
    printf("send file result:%s%d%s\n", _color_red, ret, _color_end);

    return 0;
}
