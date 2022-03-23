/*
 * =====================================================================================
 *
 *       Filename:  md5.h
 *
 *    Description:  md5 use openssl
 *
 *        Version:  1.0
 *        Created:  2019年08月29日 06时39分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __SECURITY_LIBRARY_MD5WRAPPER_H__
#define __SECURITY_LIBRARY_MD5WRAPPER_H__

#include "ISecurity.h"

class CMD5Wrapper
{
public:
    static CMyString encode(const char *text);
    static CMyString encode_file(const char *filename);
    static int32_t encode(const char *text, uint8_t *md5);
    static int32_t encode_file(const char *filename, uint8_t *md5);
};

#endif // __SECURITY_LIBRARY_MD5WRAPPER_H__
