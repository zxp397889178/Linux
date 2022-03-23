/*
 * =====================================================================================
 *
 *       Filename:  test_alert.cpp
 *
 *    Description:  测试LogWarnAlt
 *
 *        Version:  1.0
 *        Created:  2021年09月01日 17时18分57秒
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
#include "MyUtility.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("syntax: %s config.xml\n", argv[0]);
        return -1;
    }

    Comm::OpenLog("testAlert", 3, "/data/work/log");

    CXmlFile xml;
    if (!xml.Load(argv[1]))
    {
        printf("open config %s failure.\n", argv[1]);
        return -3;
    }

    if (!CAlertLog::Instance().Init(xml))
    {
        printf("open alert info failure.\n");
        return -5;
    }

    LogWarnAlt("this is a Warn test.!");

    LogErrAlt("this is a Error test.");

    LogFatalAlt("this is a FatalError test.");

    printf("test finished.\n");
    return 0;
}
