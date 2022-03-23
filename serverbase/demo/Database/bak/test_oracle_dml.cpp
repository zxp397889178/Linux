/*
 * =====================================================================================
 *
 *       Filename:  test_oracle_dml.cpp
 *
 *    Description:  test insert/update/delete in oracle
 *
 *        Version:  1.0
 *        Created:  2019年09月02日 13时49分18秒
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

void insert_dml(ISimDB *pDB)
{
    int32_t uid;
    std::string name, birth;
    printf("input userId:");
    std::cin >> uid;
    printf("input name:");
    std::cin >> name;
    printf("input birth:");
    std::cin >> birth;

    pDB->Cmd("insert into test1(id,name,birthday) values(%d,'%s', to_date('%s','yyyy-mm-dd'))",
            uid, name.c_str(), birth.c_str());
    int32_t ret = pDB->Exec();
    if (ret>0)
    {
        display(pDB);
    }
    else
    {
        printf("insert data failure.\n");
    }
}

void delete_dml(ISimDB *pDB)
{
    printf("input userid to delete:");
    int32_t userid;
    std::cin >> userid;
    pDB->Cmd("delete from test1 where id=%d", userid);
    int32_t ret = pDB->Exec();
    if (ret>0)
    {
        display(pDB);
    }
    else
    {
        printf("delete data failure.\n");
    }
}

void update_dml(ISimDB *pDB)
{
    std::string name;
    int32_t id;
    printf("input userid to update:");
    std::cin >> id;
    printf("input the new name:");
    std::cin >> name;
    pDB->Cmd("update test1 set name='%s' where id=%d",
            name.c_str(), id);
    int32_t ret = pDB->Exec();
    if (ret>0)
    {
        display(pDB);
    }
    else
    {
        printf("update data failure.\n");
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

    display(pDB);
    insert_dml(pDB);
    update_dml(pDB);
    delete_dml(pDB);

    display(pDB);

    printf("\n\n test finished. \n");
    SAFE_RELEASE(pDB);

    return 0;
}

