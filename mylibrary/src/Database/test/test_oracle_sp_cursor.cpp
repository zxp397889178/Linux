/*
 * =====================================================================================
 *
 *       Filename:  test_oracle_sp_cursor.cpp
 *
 *    Description:  测试游标的获得
 *
 *        Version:  1.0
 *        Created:  2019年11月17日 13时06分17秒
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
#include <iostream>
#include "../SimOracle.h"

CMyString time2str(time_t t)
{
    struct tm t2;
    localtime_r(&t, &t2);
    CMyString str("%04d-%02d-%02d", t2.tm_year+1900, t2.tm_mon+1, t2.tm_mday);
    return str;
}

int main(int argc, char **argv)
{
    CSimOracle *pDB = new CSimOracle();
    int32_t ret = pDB->Open("land_user", "land_user", "192.168.2.67", "orcl", 1521);
    if (ret < 0)
    {
        printf("connect to oracle failure.\n");
        return 0;
    }
    printf("connect to database succeed.\n");

    pDB->CmdProc("PRO_GET_PVUV_DAILY_CHARS");
    pDB->setParam("p_begin_date", 2019, 11, 15, 0, 0, 0);
    pDB->setParam("p_end_date", 2019, 11, 15, 0, 0, 0);
    printf("query results:\n");
    while(pDB->MoreProc("o_result"))
    {
        time_t t1 = pDB->getTime(0);
        char szTime[64] = "";
        TimeToStr(t1, szTime, sizeof(szTime));
        printf("time: %s\n", szTime);
/*
        printf("%s  %d %d %d %d %d %.2lf\n",
                szTime,
                pDB->getInt(1),
                pDB->getInt(2),
                pDB->getInt(3),
                pDB->getInt(4),
                pDB->getInt(5),
                pDB->getDouble(6));
                */
    }
    printf("\nquery finished.\n");
    SAFE_DELETE(pDB);
    return 0;
}
