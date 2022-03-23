/*
 * =====================================================================================
 *
 *       Filename:  PasswdWrapper.h
 *
 *    Description:  encode & decode password
 *
 *        Version:  1.0
 *        Created:  2019年09月03日 11时34分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __SECURITY_LIBRARY_PASSWDWRAPPER_H__
#define __SECURITY_LIBRARY_PASSWDWRAPPER_H__

#include "ISecurity.h"
class CPasswdWrapper
{
public:
    static int32_t Encode(const CMyString &input, CMyString &cipher);
    static int32_t Decode(const CMyString &cipher, CMyString &output);
};

#endif // __SECURITY_LIBRARY_PASSWDWRAPPER_H__
