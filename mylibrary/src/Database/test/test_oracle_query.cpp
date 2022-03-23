/*
 * =====================================================================================
 *
 *       Filename:  test_oracle_query.cpp
 *
 *    Description:  test oracle query statement
 *
 *        Version:  1.0
 *        Created:  2019年09月01日 17时43分02秒
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

CMyString time2str(time_t t)
{
    struct tm t2;
    localtime_r(&t, &t2);
    CMyString str("%04d-%02d-%02d", t2.tm_year+1900, t2.tm_mon+1, t2.tm_mday);
    return str;
}

int main(int argc, char **argv)
{
    ISimDB *pDB = CreateOracleConn("yuzp", "zpking", "oracle.opk2.com", "orcl");
    if (NULL == pDB)
    {
        printf("connect to oracle failure.\n");
        return 0;
    }
    printf("connect to database succeed.\n");

    pDB->Cmd("select * from test1");
    printf("call sql query statement.\n");
    int row(0);
    while(pDB->More())
    {
        long id = pDB->GetInt(0);
        CMyString name = pDB->GetString(1);
        // long age = pDB->GetInt(2);
        time_t birth = pDB->GetTime(2);
        CMyString birth_str = time2str(birth);
        // double salary = pDB->GetDouble(3);
        printf("row:[%d] %d %s %s\n", ++row, id, name.c_str(), birth_str.c_str());
    }

    printf("query finished.\n");
    SAFE_RELEASE(pDB);

    return 0;
}
