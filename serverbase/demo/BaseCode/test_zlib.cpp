/*
 * =====================================================================================
 *
 *       Filename:  test_zlib.cpp
 *
 *    Description:  test zlib compress / decompress
 *
 *        Version:  1.0
 *        Created:  01/27/2020 06:22:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"

int32_t main(int argc, char **argv)
{
    char *pData = "原标题：传奇坠落 | 篮球巨星科比因坠机去世！球迷泪崩：我的青春没了美国体育媒体ESPN、新闻媒体ABC及娱乐新闻媒体TMZ等多家媒体报道，NBA球星科比·布莱恩特在当地时间周日26号，于加利福尼亚州洛杉矶县卡拉巴萨斯的一场直升机坠机事故中身亡。据洛杉矶当地警方称，有五人在直升机坠毁中死亡。科比·布莱恩特（Kobe Bryant），1978年8月23日出生于美国宾夕法尼亚州费城，前美国职业篮球运动员，司职得分后卫/小前锋（锋卫摇摆人），绰号“黑曼巴”/“小飞侠”，是前NBA球员乔·布莱恩特的儿子。科比的最后一条推特更新，是祝贺詹姆斯得分超过自己。“继续让比赛前进@ 詹姆斯。对你充满敬意，我的兄弟 #33644#” ";
    uint32_t cbData = strlen(pData);
    byte_t z_data[1024] = {0,};
    uint32_t cbZData = sizeof(z_data);
    byte_t c_data[1024] = {0,};
    uint32_t cbCData = sizeof(c_data);

    printf("origin:[%s]\n\n", pData);
    printf("origin data length:%u\n", cbData);

    int32_t ret = z_compress((byte_t*)pData, cbData, z_data, cbZData);
    if (ret != 0)
    {
        printf("compress failure, ret=%d\n", ret);
        return -1;
    }
    printf("compress data length:%u\n", cbZData);

    ret = z_decompress(z_data, cbData, c_data, cbCData);
    if (ret != 0)
    {
        printf("decompress failure, ret=%d\n", ret);
        return -3;
    }
    printf("decompress data length:%u\n", cbCData);

    printf("decompress:[%s]\n\n", (const char*)c_data);

    return 0;}
