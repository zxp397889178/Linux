/*
 * =====================================================================================
 *
 *       Filename:  test_cast.cpp
 *
 *    Description:  test byte cast
 *
 *        Version:  1.0
 *        Created:  03/06/2020 07:04:54 PM
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
    int32_t val = 301998414;
    printf("val = [%d]\n", CByteOrder::Cast(val));
    return 0;
}
