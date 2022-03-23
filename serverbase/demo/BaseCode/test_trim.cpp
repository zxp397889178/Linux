/*
 * =====================================================================================
 *
 *       Filename:  test_trim.cpp
 *
 *    Description:  ltrim/rtrim/trim
 *
 *        Version:  1.0
 *        Created:  2020年02月24日 19时51分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#include "BaseCode.h"
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    std::string s1 = "   hello,world   i am  ";
    std::string s2 = "   hello,world   i am  ";
    std::string s3 = "   hello,world   i am  ";
    std::cout << "origin:   [" << s1 <<"]" << std::endl;
    ltrim(s1);
    std::cout << "ltrim(s1):[" << s1  << "]" << std::endl;
    rtrim(s2);
    std::cout << "rtrim(s1):[" << s2  << "]" << std::endl;
    trim(s3);
    std::cout << "trim(s1): [" << s3  << "]" << std::endl;

    return 0;
}
