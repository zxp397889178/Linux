/*
 * =====================================================================================
 *
 *       Filename:  test_oracle_conn.cpp
 *
 *    Description:  test connect to oracle
 *
 *        Version:  1.0
 *        Created:  2019年09月01日 16时59分48秒
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
    ISimDB *pDB = CreateOracleConn("yuzp", "zpking", "oracle.opk2.com", "orcl");
    if (NULL == pDB)
    {
        printf("connect to oracle failure.\n");
        return 0;
    }
    printf("connect to database succeed.\n");
    SAFE_RELEASE(pDB);
    printf("\n\n === test finished === \n");
    return 0;
}
