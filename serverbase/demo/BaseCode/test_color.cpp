/*
 * =====================================================================================
 *
 *       Filename:  test_color.cpp
 *
 *    Description:  测试有颜色的输出
 *
 *        Version:  1.0
 *        Created:  02/07/2020 03:40:45 PM
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
    CMyString str[10];
    double val=10.0000231;

    printf("hello, world, my name is %s, age %s, and i live in %s, I have $%s,  thanks %s\n",
           color_text(_color_red, "David", str[0]),
           color_int(_color_blue, 10, str[1]),
           color_text(_color_cyan, "FuZhou", str[2]),
           color_double(_color_green, val, str[4]),
           color_text(_color_lightmagenta, "EveryOne",str[3])
           );

    return 0;
}
