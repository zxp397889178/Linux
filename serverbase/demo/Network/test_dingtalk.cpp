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
#if 0
    cb.setDingtalkToken("19bf2c0942754b172b449e13fcf5c45f6a7d24f85a7598417460600ef5c4827", "");
#else
    cb.setDingtalkToken("a19bf2c0942754b172b449e13fcf5c45f6a7d24f85a7598417460600ef5c4827", 
            "SECa6f61416adf65582c41cdec45c1143804f0ead762d137d03c43d0515b92b0fb5");
#endif
    uint64_t t1 = Now(true);
    int32_t ret(0);
    if (argc > 1)
    {
        ret = cb.dingtalk(argv[1]);
    }
    else
    {
        ret = cb.dingtalk(CMyString("syntax: %s <message>", argv[0]));
    }
    uint64_t t2 = Now(true);
    int32_t cost = (int)(t2-t1);

    printf("send dingtalk result=%d, cost %.2lfms\n", ret, cost/1000.0);
    return 0;
}

