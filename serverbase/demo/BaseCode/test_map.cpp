/*
 * =====================================================================================
 *
 *       Filename:  test_map.cpp
 *
 *    Description:  test map
 *
 *        Version:  1.0
 *        Created:  2021年04月25日 20时02分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include <map>
#include <stdio.h>

int main(int argc, char **argv)
{
    std::map<int, int>  mapTest;
    mapTest[10]=100;
    mapTest[20]=200;
    mapTest[30]=300;

    printf("origin:\n");
    for(std::map<int,int>::iterator it1(mapTest.begin()); it1 != mapTest.end(); ++it1)
    {
        printf("key: %d, val:%d\n", it1->first, it1->second);
    }

    std::map<int, int>::iterator it(mapTest.find(20));
    if (it != mapTest.end())
    {
        it->second = it->second + 50;
    }

    printf("after modified:\n");
    for(std::map<int,int>::iterator it2(mapTest.begin()); it2 != mapTest.end(); ++it2)
    {
        printf("key: %d, val:%d\n", it2->first, it2->second);
    }

    return 0;
}
