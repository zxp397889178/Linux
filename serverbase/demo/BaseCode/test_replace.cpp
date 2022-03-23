/*
 * =====================================================================================
 *
 *       Filename:  test_replace.cpp
 *
 *    Description:  test CMyString Replace
 *
 *        Version:  1.0
 *        Created:  2021年03月27日 18时01分03秒
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
    CMyString test1("{'abc':1, 'def':2,'name':'david', 'age':23, 'home':'HK', 'chinese': True, \"Result\":\"TruetrUetrUE\"}");
    printf("origin: [%s]\n", test1.c_str());
    int32_t count(0);

    // 测试1
    count = test1.Replace('\'', '\"');
    printf("after : [%s], count:%d\n", test1.c_str(), count);

    // 测试2
    count = test1.Replace("TRUE", "true", true); 
    printf("after replace TRUE(1): [%s], count:%d\n", test1.c_str(),count);
    count = test1.Replace("TRUE", "true", false);
    printf("after replace TRUE(2): [%s], count:%d\n", test1.c_str(),count);

    return 0;
}
