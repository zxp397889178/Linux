/*
 * =====================================================================================
 *
 *       Filename:  test_des.cpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  01/16/2020 03:10:56 PM
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

int main(int argc, char **argv)
{
    const char szText[128] = "ac=10&startTime=1579072036&endTime=1579072336";
    CMyString strKey = "Ujd3Qldz";
    
    printf("origin:[%s]\nkey[%s]\n", szText, strKey.c_str());
    uint8_t * cipher(NULL);
    size_t cipher_len(0);
    CDesWrapper::Encrypt_ECB((const uint8_t*)szText, strlen(szText),
            strKey, cipher, cipher_len, padPKCS5);
    printf("cipher_len=%u\n", cipher_len);
    CMyString b64e = CBase64Wrapper::Encode((const char*)cipher, cipher_len, false);
    printf("\n");
    printf("base64: [%s]\n", b64e.c_str());

    CMyString str1 = CHttpWrapper::urlEncode(b64e);
    printf("url_encode:[%s]\n", str1.c_str());

    uint8_t* clear_text(NULL);
    size_t clear_len(0);
    CDesWrapper::Decrypt_ECB(cipher, cipher_len, strKey, clear_text, clear_len, padPKCS5);
    printf("decrypt:[%s][%u]\n", (const char*)clear_text, clear_len);
    SAFE_DELETE(cipher);
    SAFE_DELETE(clear_text);

    return 0;
}
