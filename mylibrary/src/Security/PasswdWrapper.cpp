/*
 * =====================================================================================
 *
 *       Filename:  PasswdWrapper.cpp
 *
 *    Description:  password encode & decode
 *
 *        Version:  1.0
 *        Created:  2019年09月03日 11时37分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "PasswdWrapper.h"
#include "Base64Wrapper.h"

int32_t CPasswdWrapper::Encode(const CMyString &input, CMyString &cipher)
{
    if (input.empty() || input.length() < 6)
    {
        return -1;
    }
    CMyString strOrigin("qw%syq%s", input.substr(0,3).c_str(),input.substr(3).c_str());
    bool newLine(false);
    cipher = CBase64Wrapper::Encode(strOrigin.c_str(), strOrigin.length(), newLine);
    return 0;
}

int32_t CPasswdWrapper::Decode(const CMyString &cipher, CMyString &output)
{
    if (cipher.empty() || cipher.length() < 6)
    {
        return -1;
    }
    bool newLine(false);
    CMyString strClear = CBase64Wrapper::Decode(cipher.c_str(), cipher.length(), newLine);
    output.Format("%s%s", strClear.substr(2,3).c_str(), strClear.substr(7).c_str());
    return 0;
}
