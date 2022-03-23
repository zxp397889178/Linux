/*
 * =====================================================================================
 *
 *       Filename:  test_chat.cpp
 *
 *    Description:  测试聊天机器人
 *
 *        Version:  1.0
 *        Created:  2020年07月21日 16时43分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "BaseCode.h"
#include "MyNet.h"

int main(int argc, char **argv)
{
    Comm::OpenLog("test", 3, "/data/work/logs");

    CChatBot cb;
    cb.setTelegramToken("1262721496:AAE4JkUFr54S20aM46Ilt17NC2GV3iANjHs");
    int32_t ret(0);
    uint64_t t1 = Now(false);
    if (argc > 2)
    {
        int32_t chatId = atoi(argv[1]);
        ret = cb.telegram(chatId, argv[2]);
    }
    else
    {
        printf("syntax: %s <chatId> <msg>\n", argv[0]);
        return -1;
    }
    uint64_t t2 = Now(false);
    int32_t cost = (int32_t)(t2-t1);

    printf("send dingtalk result=%d, cost %.2lfs\n", ret, cost/1000.0);
    return 0;
}

