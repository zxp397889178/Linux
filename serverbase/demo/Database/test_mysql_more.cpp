/*
 * =====================================================================================
 *
 *       Filename:  test_mysql_more.cpp
 *
 *    Description:  测试多个结果集的返回
 *
 *        Version:  1.0
 *        Created:  2020年09月22日 10时20分19秒
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

int main(int argc, char **argv)
{
    ISimDB *pDB = CreateMysqlConn("test", "test123456", "192.168.2.163", "chicken_farm", 3306, NULL, NULL, "utf8mb4");
    if (NULL == pDB)
    {
        printf("connect database failure.\n");
    }
    printf("connect database succeed.\n");

    pDB->Cmd("call pQueryUserAll(17)");
    while(pDB->More())
    {
        int32_t res = pDB->getResultIndex();
        printf("result index : %d\n", res);
    }
    SAFE_RELEASE(pDB);
    return 0;
}



