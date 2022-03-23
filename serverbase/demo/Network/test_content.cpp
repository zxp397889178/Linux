/*
 * =====================================================================================
 *
 *       Filename:  test_content.cpp
 *
 *    Description:  test content type
 *
 *        Version:  1.0
 *        Created:  2020年02月17日 00时45分31秒
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
    CMyString strFile("earth1.jpg");
    CMyString str1;
    if (!GetContentType(strFile, str1))
    {
        printf("not found content.\n");
    }
    printf("content-type: %s\n", str1.c_str());

    return 0;
}
