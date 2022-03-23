/*
 * =====================================================================================
 *
 *       Filename:  DesWrapper.h
 *
 *    Description:  des base openssl
 *
 *        Version:  1.0
 *        Created:  2019年08月29日 00时41分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *        ref: https://blog.csdn.net/mao834099514/article/details/54946322
 *        ref: https://www.cnblogs.com/findumars/p/7252839.html
 *
 * =====================================================================================
 */

#ifndef __SECURITY_LIBRARY_DESWRAPPER_H__
#define __SECURITY_LIBRARY_DESWRAPPER_H__

#include "ISecurity.h"

class CDesWrapper
{
public:
    static void Encrypt_ECB(const uint8_t *input, size_t cbInput, const CMyString &key,
            uint8_t *&cipher, size_t &cbCipher, enum PaddingMode pad = padZERO);
    static void Decrypt_ECB(const uint8_t *cipher, size_t cbCipher, 
            const CMyString &key, uint8_t *&output, size_t &cbOutput, 
            enum PaddingMode pad = padZERO);
};

#endif  // __SECURITY_LIBRARY_DESWRAPPER_H__
