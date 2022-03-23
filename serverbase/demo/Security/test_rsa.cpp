/*
 * =====================================================================================
 *
 *       Filename:  test_rsa.cpp
 *
 *    Description:  test CRsaWrapper
 *
 *        Version:  1.0
 *        Created:  09/22/2019 01:07:18 PM
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

void PrintBuffer(uint8_t *buff, size_t cbBuff)
{
    printf("======== buffer, size %lu ========\n", cbBuff);
    int32_t max_col = 32;
    int32_t max_row = cbBuff/max_col + (((cbBuff%max_col)==0) ? 0 : 1);
    int32_t cnt(0);
    for(int row=0; row<max_row; ++row)
    {
        void *p = (void*)(buff + cnt);
        printf("%p  ", p);
        for(int col=0; col<max_col; ++col)
        {
            if (cnt >= cbBuff) break;
            printf("%02X ", buff[cnt]);
            ++cnt;
        }
        printf("\n");
    }
    printf("\n");
}
int main(int argc, char **argv)
{
    CMyString strPriKey, strPubKey;
    CRsaWrapper::generateRSAKey(strPubKey, strPriKey);

    CMyString strText("Hello, this is a test, 余志鹏同学在测试");
    uint8_t input[256] = {0};
    size_t cbInput = sizeof(input);

    printf("text [%s], len[%u]\n", strText.c_str(), strText.length());
    printf("pubkey:\n%s\n\n", strPubKey.c_str());
    printf("prikey:\n%s\n\n", strPriKey.c_str());
    cbInput = strText.length();
    memcpy(input, strText.c_str(), strText.length());
    PrintBuffer(input, cbInput);

    uint8_t *cipher(NULL);
    size_t cbCipher(0);
    int32_t ret = CRsaWrapper::Encrypt(input, cbInput, strPubKey, cipher, cbCipher);
    if (ret >=0)
    {
        PrintBuffer(cipher, cbCipher);
        printf("result=%d\n", ret);
    }
    else
    {
        printf("RSA Encrypt failure, ret=%d\n", ret);
    }

    uint8_t *output(NULL);
    size_t cbOutput(0);
    ret = CRsaWrapper::Decrypt(cipher, cbCipher, strPriKey, output, cbOutput);
    if (ret >=0)
    {
        PrintBuffer(output, cbOutput);
        printf("result=%d\n", ret);
    }
    else
    {
        printf("RSA Decrypt failure, ret=%d\n", ret);
    }
    printf("\ndecrypt: [%s]\n", (const char*)output);
    
    free(cipher);
    free(output);

    return 0;
}
