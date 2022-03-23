/*
 * =====================================================================================
 *
 *       Filename:  Base64Wrapper.h
 *
 *    Description:  base64 with openssl
 *
 *        Version:  1.0
 *        Created:  2019年08月28日 23时58分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __SECURITY_LIBRARY_BASE64WRAPPER_H__
#define __SECURITY_LIBRARY_BASE64WRAPPER_H__

#include "ISecurity.h"

class CBase64Wrapper
{
public:
    static CMyString Encode(const char *buffer, int32_t len, bool newLine);
    static CMyString Decode(const char *buffer, int32_t len, bool newLine);

    static void Decode(const char *buffer, size_t buflen, bool newLine, 
            uint8_t *&outbuf, size_t &outlen);
};

#endif // __SECURITY_LIBRARY_BASE64WRAPPER_H__
