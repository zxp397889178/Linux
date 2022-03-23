/*
 * =====================================================================================
 *
 *       Filename:  test_oracle_sp.cpp
 *
 *    Description:  call store procedure in oracle
 *
 *        Version:  1.0
 *        Created:  2019年09月02日 14时53分24秒
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

typedef TObjectPtrGuard<ISimDB, policy_ptr_release<ISimDB> >   CSimDB;
CMyString time2str(time_t t)
{
    struct tm t2;
    localtime_r(&t, &t2);
    CMyString str("%04d-%02d-%02d", t2.tm_year+1900, t2.tm_mon+1, t2.tm_mday);
    return str;
}

void display(ISimDB *pDB)
{
    pDB->Cmd("select * from test1");
    int32_t row(0);
    printf("\n\n");
    printf("row  id       name             birth\n");
    printf("==== ======== ================ ==============\n");
    while(pDB->More())
    {
        long id = pDB->GetInt(0);
        CMyString name = pDB->GetString(1);
        time_t birth = pDB->GetTime(2);
        CMyString birth_str = time2str(birth);
        printf("%4d %8ld %16s %s\n", ++row, id, name.c_str(), birth_str.c_str());
    }
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

    CSimDB db(pDB);
    display(db.get());

    std::string name;
    std::string birth;
    std::cout << "input username:";
    std::cin >> name;
    std::cout << "input birth(yyyy-mm-dd):";
    std::cin >> birth;
    //db->Cmd("execute pTestProc('%s', '%s')", name.c_str(), birth.c_str());
    db->Cmd("BEGIN pTestProc('%s', '%s'); END;", name.c_str(), birth.c_str());
    int32_t ret = db->Exec();
    if (ret > 0)
    {
        display(db.get());
    }
    else
    {
        printf("\nERROR: call store proecedure failure.\n");
    }

    return 0;
}
