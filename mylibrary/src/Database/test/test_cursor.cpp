/*
 * =====================================================================================
 *
 *       Filename:  test_cursor.cpp
 *
 *    Description:  test use oraquery.
 *
 *        Version:  1.0
 *        Created:  2019年11月17日 14时37分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "ocl.h"

using namespace ocl;

int main(int argc, char **argv)
{
    OraConnection m_conn;

    m_conn.setUsername("land_user");
    m_conn.setPassword("land_user");
    CMyString strTNS("%s:%d/%s", "192.168.2.67", 1521, "orcl");
    m_conn.setServer(strTNS.c_str());
    m_conn.setConnectMode(ocl::cmNormal);
    m_conn.open();


    OraCommand command( m_conn);
    command.setCommandText("PRO_GET_PVUV_DAILY_CHARS"); 
    command.setCommandType(ctStoredProc);
    command.params()["p_begin_date"].setDate(CRDate(2019,11,15));
    command.params()["p_end_date"].setDate(CRDate(2019,11,15));
    command.execute(); 

    OraQuery query( m_conn);
    query.setFetchRows( 10  );
    query.setCursor( command.params()["o_result"].getCursor()  );
    query.open();

    int recordCount = 0;
    while (!query.isEOF()) {

        printf("row#%d %s %d %d %d %d %d %.2lf \n", recordCount,
                query.field("STAT_TIME").getChars(),
                query.field("PV_CNT").getInt(),
                query.field("UV_CNT").getInt(),
                query.field("IP_CNT").getInt(),
                query.field("TEST_PLAY_CNT").getInt(),
                query.field("DOWNLOAD_CNT").getInt(),
                query.field("DOWNLOAD_RATE").getDouble());
        recordCount++;
        query.next();
    }
    query.close();
}
