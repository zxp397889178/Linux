/*
 * =====================================================================================
 *
 *       Filename:  test_str.cpp
 *
 *    Description:  test for CMyString
 *
 *        Version:  1.0
 *        Created:  03/19/2020 08:29:24 PM
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
    int32_t i1=1226;
    int32_t i2=123456;
    CMyString str1("%s%s%s%ld%d%s", "abc","345", "def",i1,i2,"ABC");
    printf("i1=>%s\n", str1.c_str());

    CMyString str2("{\"product_id\":\"4022\",\"ori_price\":0,\"unit_price\":0,\"ware_id\":4022}");
    printf("str2:[%s]\n", str2.c_str());
    str2.Replace("\"", "\\\"");
    printf("str2 new:[%s]\n", str2.c_str());

    return 0;
}
