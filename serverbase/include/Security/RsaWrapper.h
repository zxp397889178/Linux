/*
 * =====================================================================================
 *
 *       Filename:  RsaWrapper.h
 *
 *    Description:  ras base openssl
 *
 *        Version:  1.0
 *        Created:  08/29/2019 11:04:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __SECURITY_LIBRARY_RASWRAPPER_H__
#define __SECURITY_LIBRARY_RASWRAPPER_H__

#include "ISecurity.h"

#ifndef XRSA_KEY_BITS 
#   define XRSA_KEY_BITS (2048)
#endif

class CRsaWrapper
{
public:
    static void generateRSAKey(CMyString &strPubKey, CMyString &strPriKey);

    static int32_t Encrypt(const uint8_t *input, size_t cbInput, 
            const CMyString &strPubKey, 
            uint8_t *&cipher, size_t &cbCipher);
    static int32_t Decrypt(const uint8_t *cipher, size_t cbCihper, 
            const CMyString &strPriKey,
            uint8_t *&output, size_t &cbOutput);

    // SHA256withRSA
    // SHA1withRSA
    static int32_t rsaSign(const uint8_t *input, size_t cbInput, 
            const CMyString &strPriKey, CMyString &strCipher);

    // SHA1withRSA
    //
    static bool rsaVerify(const uint8_t *input, size_t cbInput,
            const CMyString &strSign, const CMyString &strPubKey);
};

#endif // __SECURITY_LIBRARY_RASWRAPPER_H__

