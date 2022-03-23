/*
* =====================================================================================
*
*       Filename:  ShaWrapper.h
*
*    Description:  sha base on openssl
*
*        Version:  1.0
*        Created:  2017年6月14日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi, modified by rossen yu
*   Organization:  quwan
*
*   ref: http://blog.chinaunix.net/uid-8877552-id-5728503.html
*   ref: https://blog.csdn.net/yasi_xi/article/details/9066003
*
* =====================================================================================
*/

#ifndef _QW_SECURITY_SHAWRAPPER_H_
#define _QW_SECURITY_SHAWRAPPER_H_

#include "BaseCode.h"

class ShaWrapper
{
public:
	static CMyString sha1(const CMyString &data);
	static CMyString sha224(const CMyString &data);
	static CMyString sha256(const CMyString &data);
	static CMyString sha384(const CMyString &data);
	static CMyString sha512(const CMyString &data);

    // cbCipher >= SHA_DIGEST_LENGTH
    static int32_t SHA1(const uint8_t *input, size_t cbInput, uint8_t *cipher, size_t &cbCipher);

    // cbCipher >= SHA256_DIGEST_LENGTH
    static int32_t SHA256(const uint8_t *input, size_t cbInput, uint8_t *cipher, size_t &cbCipher);


	/*
	*@type取值 sha512,sha256,sha1,md5,sha224,sha384,sha,md2
	*/
	static CMyString Hmac(const char *type, const char *key, const char *input);

    static int32_t Hmac(const char *type, const char *key, const char *input, uint8_t *cipher, size_t &cbCipher);

public:
    static CMyString Sha1WithBase64(const CMyString &data);

    static CMyString Sha256WithBase64(const CMyString &data);
};

#endif	// _QW_SECURITY_SHAWRAPPER_H_
