/*
 * =====================================================================================
 *
 *       Filename:  test_geoip.cpp
 *
 *    Description:  test GeoIP data
 *
 *        Version:  1.0
 *        Created:  2021年07月19日 19时23分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "MyUtility.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("syntax: %s geoip_file ip \n", argv[0]);
        return -1;
    }

    int64_t t1 = Now(true);
    CGeoIPWrapper gip;
    if (!gip.Open(argv[1]))
    {
        printf("open geoip file [%s] failure.\n", argv[1]);
        return -3;
    }
    int64_t t2 = Now(true);

    Json::Value data;
    if (!gip.Lookup(argv[2], data, "zh-CN"))
    {
        printf("ip [%s] invalid.\n", argv[2]);
        return -5;
    }
    int64_t t3 = Now(true);

    Json::FastWriter wr;
    CMyString strTemp = wr.write(data);
    printf("\n=============================================================\n%s\n", strTemp.c_str());

    printf("\nopen GeoIP file [%s] cost %.2lfms,\nlookup ip [%s] cost %dus.\n",
            argv[1], (t2-t1)/1000.0, argv[2], (int32_t)(t3-t2));
    return 0;
}
