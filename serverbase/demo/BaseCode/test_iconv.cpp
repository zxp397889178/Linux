/*
 * =====================================================================================
 *
 *       Filename:  test_iconv.cpp
 *
 *    Description:  测试字符转换
 *
 *        Version:  1.0
 *        Created:  02/07/2020 09:13:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"

int main(int argc, char **argv)
{
    char *szText = "\u5546\u6237\u9a8c\u7b7e\u5931\u8d25";

    char szOut[128] = "";
    size_t cbOut = sizeof(szOut);
    int32_t ret = utf16_to_gbk(szOut, cbOut, szText, strlen(szText));
    printf("ret=%d\n", ret);
    printf("origin=%s\n", szText);
    szOut[cbOut]='\0';
    printf("text=%s\n", szOut);
    printf("\n");


    return 0;
}
