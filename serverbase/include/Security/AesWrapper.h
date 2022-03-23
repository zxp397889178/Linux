/*
 * =====================================================================================
 *
 *       Filename:  AesWrapper.h
 *
 *    Description:  aes with openssl
 *
 *        Version:  1.0
 *        Created:  08/29/2019 09:21:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 *        ref : https://blog.csdn.net/qq839306215/article/details/73558728
 *        pkcs5 vs pkcs7:  http://www.sunnyu.com/?p=477
 *                         https://blog.csdn.net/chary8088/article/details/21185187
 * =====================================================================================
 */

#ifndef __SECURITY_LIBRARY_AESWRAPPER_H__
#define __SECURITY_LIBRARY_AESWRAPPER_H__

#include "ISecurity.h"

class CAesWrapper
{
public:
    // key length must be 16, 24, 32
    static int32_t Encode(const uint8_t *input, size_t cbInput, 
            const CMyString &key,
            uint8_t *&cipher, size_t &cbCipher,
            enum PaddingMode mode);
    static int32_t Decode(const uint8_t *cipher, size_t cbCipher,
            const CMyString &key,
            uint8_t *&output, size_t &cbOutput,
            enum PaddingMode mode);


    static int32_t EncodeCBC(const uint8_t *input, size_t cbInput,
            const CMyString &key, const CMyString &iv,
            uint8_t *&cipher, size_t &cbCipher,
            enum PaddingMode mode);
    static int32_t DecodeCBC(const uint8_t *cipher, size_t cbCipher,
            const CMyString &key, const CMyString &iv,
            uint8_t *&output, size_t &cbOutput,
            enum PaddingMode mode);
};

#endif // __SECURITY_LIBRARY_AESWRAPPER_H__

