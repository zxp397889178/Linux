/*
 * =====================================================================================
 *
 *       Filename:  test_aes.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2019年11月08日 22时52分35秒
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

int32_t main(int argc, char **argv)
{
    CMyString strData="s=6&startTime=1573183290659&endTime=1573183590659";
    CMyString strKey = "CA611AFB8BD370CA";

    uint8_t *cipher(NULL);
    size_t cipher_len(0);
    CAesWrapper::Encode((const uint8_t*)strData.c_str(), strData.length(), strKey, cipher, cipher_len, padPKCS5);
    CMyString b64e = CBase64Wrapper::Encode((const char*)cipher, cipher_len, false);

    uint8_t *clear(NULL);
    size_t clear_len(0);
    CMyString b64d = CBase64Wrapper::Decode(b64e.c_str(), b64e.length(), false);
    CAesWrapper::Decode((const uint8_t*)b64d.c_str(), b64d.length(),strKey, clear, clear_len, padPKCS5);

    printf("encrypt>base64(aes(%s))=[%s]\n", strData.c_str(), b64e.c_str());
    printf("decrypt>base64(aes(%s))=[%s]\n", (const char*)b64e.c_str(), (const char*)clear);

    CAutoBuffer gurad1((char*)cipher);
    CAutoBuffer guard2((char*)clear);
    return 0;
}
