/*
 * =====================================================================================
 *
 *       Filename:  test_md5.cpp
 *
 *    Description:  test use md5.h
 *
 *        Version:  1.0
 *        Created:  2019年08月29日 07时34分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "Security.h"
#include <iostream>

#pragma comment(lib, "libBaseCode.a")
#pragma comment(lib, "libSecurity.a")

int main(int argc, char **argv)
{
    char text[256]="hello,world!";

    CMyString ret = CMD5Wrapper::encode(text);
    std::cout << ret << std::endl;
    
    // CMD5Wrapper::md
    // CMyString ret = 

    return 0;
}
