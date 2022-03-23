/*
 * =====================================================================================
 *
 *       Filename:  test_time.cpp
 *
 *    Description:  测试decodetime & strtotime
 *
 *        Version:  1.0
 *        Created:  02/06/2020 07:01:49 PM
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

    const char *str1 = "00:00:00";
    const char *str2 = "23:59:59";

    int year(0), month(0), day(0), hour(0), minute(0), second(0);
    bool ret = DecodeTimeStr(str1, year, month, day, hour, minute, second, "HH:MM:SS");
    printf("[%s] => %d %d %d %d %d %d, result:%s\n", str1, year, month, day, hour, minute, second, ret ? "true" : "false");

    ret = DecodeTimeStr(str2, year, month, day, hour, minute, second, "HH:MM:SS");
    printf("[%s] => %d %d %d %d %d %d, result:%s\n", str2, year, month, day, hour, minute, second, ret ? "true" : "false");


    int32_t t1 = Time2Seconds(str1, "HH:MM:SS");
    printf("[%s] => %d\n", str1, t1);
    int32_t t2 = Time2Seconds(str2, "HH:MM:SS");
    printf("[%s] => %d\n", str2, t2);

    time_t t3 = 0;
    struct tm *t3m = localtime(&t3);
    printf("time [%ld] %d %d %d %d %d %d\n", t3, t3m->tm_year, t3m->tm_mon, t3m->tm_mday, t3m->tm_hour, t3m->tm_min, t3m->tm_sec);

    return 0;
}
