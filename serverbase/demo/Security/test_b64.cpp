/*
 * =====================================================================================
 *
 *       Filename:  test_b64.cpp
 *
 *    Description:  test base64 
 *
 *        Version:  1.0
 *        Created:  2019年08月29日 00时18分31秒
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
#include <string>

using namespace std;

int main(int argc, char **argv)
{
    bool newLine = false;
    string input = "Hello World!";

    CMyString encode = CBase64Wrapper::Encode(input.c_str(), input.length(), newLine);
    CMyString decode = CBase64Wrapper::Decode(encode.c_str(), encode.length(), newLine);

    cout << "Base64 Encoded : " << encode << endl;
    cout << "Base64 Decoded : " << decode << endl;

    return 0;
}
