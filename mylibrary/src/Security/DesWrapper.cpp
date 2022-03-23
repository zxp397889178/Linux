/*
 * =====================================================================================
 *
 *       Filename:  DesWrapper.cpp
 *
 *    Description:  des base openssl
 *
 *        Version:  1.0
 *        Created:  2019年08月29日 00时49分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "DesWrapper.h"
#include <openssl/des.h>

#define DES_BLOCK_SIZE  8

void CDesWrapper::Encrypt_ECB(const uint8_t *input, size_t cbInput, 
        const CMyString &key, uint8_t *&cipher, size_t &cbCipher,
        enum PaddingMode pad)
{
    DES_cblock keyEncrypt;
    memset(keyEncrypt, 0, DES_BLOCK_SIZE);
    // 构造补齐后的密钥
    if (key.length() < DES_BLOCK_SIZE)
    {
        memcpy(keyEncrypt, key.c_str(), key.length());
    }
    else
    {
        memcpy(keyEncrypt, key.c_str(), DES_BLOCK_SIZE);
    }

    // 密钥置换
    DES_key_schedule keySchedule;
    DES_set_key_unchecked(&keyEncrypt, &keySchedule);

    //循环加密，每8字节一次  
    const_DES_cblock inputText;
    DES_cblock outputText;
    std::vector<uint8_t> vecCipherText;
    uint8_t tmp[DES_BLOCK_SIZE+1] = {0};
    for(size_t i=0; i<cbInput/DES_BLOCK_SIZE; ++i)
    {
        memcpy(inputText, &input[i*DES_BLOCK_SIZE], DES_BLOCK_SIZE);
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
        memcpy(tmp, outputText, DES_BLOCK_SIZE);
        for(int32_t j=0; j<DES_BLOCK_SIZE; ++j)
        {
            vecCipherText.push_back(tmp[j]);
        }
    }
    if (cbInput%DES_BLOCK_SIZE !=0)
    {
        int32_t tmp1 = cbInput/DES_BLOCK_SIZE*DES_BLOCK_SIZE;
        int32_t tmp2 = cbInput - tmp1;
        int32_t padding = 0;
        if (pad == padPKCS5 || pad == padPKCS7)
        {
            padding = DES_BLOCK_SIZE - tmp2;
        }
        memset(inputText, padding, DES_BLOCK_SIZE);
        memcpy(inputText, &input[tmp1], tmp2);
        // 加密函数
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
        memcpy(tmp, outputText, DES_BLOCK_SIZE);
        for(int32_t j=0; j<DES_BLOCK_SIZE; ++j)
        {
            vecCipherText.push_back(tmp[j]);
        }
    }
    if ((cbInput%DES_BLOCK_SIZE == 0) && (pad==padPKCS5))
    {
        int32_t padding = DES_BLOCK_SIZE;
        memset(inputText, padding, DES_BLOCK_SIZE);
        // 加密函数
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
        memcpy(tmp, outputText, DES_BLOCK_SIZE);
        for(int32_t j=0; j<DES_BLOCK_SIZE; ++j)
        {
            vecCipherText.push_back(tmp[j]);
        }

    }

    cbCipher = vecCipherText.size();
    cipher = new uint8_t[cbCipher+1];
    for(size_t k=0; k<cbCipher; ++k)
    {
        cipher[k] = vecCipherText[k];
    }
}


void CDesWrapper::Decrypt_ECB(const uint8_t *cipher, size_t cbCipher, 
        const CMyString &key, uint8_t *&output, size_t &cbOutput,
        enum PaddingMode pad)
{
    DES_cblock keyEncrypt;
    memset(keyEncrypt, 0, DES_BLOCK_SIZE);
    if (key.length()<DES_BLOCK_SIZE)
    {
        memcpy(keyEncrypt, key.c_str(), key.length());
    }
    else
    {
        memcpy(keyEncrypt, key.c_str(), DES_BLOCK_SIZE);
    }

    // 密钥置换
    DES_key_schedule keySchedule;
    DES_set_key_unchecked(&keyEncrypt, &keySchedule);

    //循环加密，每8字节一次  
    const_DES_cblock inputText;
    DES_cblock outputText;
    std::vector<uint8_t> vecClearText;
    uint8_t tmp[DES_BLOCK_SIZE];
    for(size_t i=0; i<cbCipher/DES_BLOCK_SIZE; ++i)
    {
        memcpy(inputText, &cipher[i*DES_BLOCK_SIZE], DES_BLOCK_SIZE);
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
        memcpy(tmp, outputText, DES_BLOCK_SIZE);
        for(int32_t j=0; j<DES_BLOCK_SIZE; ++j)
        {
            vecClearText.push_back(tmp[j]);
        }
    }
    /*
     * 此部份代码不会跳进来
    if (cbCipher%DES_BLOCK_SIZE != 0)
    {
        int32_t tmp1 = cbCipher/DES_BLOCK_SIZE*DES_BLOCK_SIZE;
        int32_t tmp2 = cbCipher - tmp1;
        memset(inputText, 0, DES_BLOCK_SIZE);
        memcpy(inputText, &cipher[tmp1], tmp2);
        DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
        memcpy(tmp, outputText, DES_BLOCK_SIZE);
        for(int32_t j=0; j<DES_BLOCK_SIZE; ++j)
        {
            vecClearText.push_back(tmp[j]);
        }
    }
    */

    cbOutput = vecClearText.size();
    output = new uint8_t[cbOutput+1];
    for(size_t k=0; k<cbOutput; ++k)
    {
        output[k] = vecClearText[k];
    }

    if (pad == padPKCS5)
    {
        uint8_t unpadding = output[cbOutput-1];
        cbOutput = cbOutput - unpadding;
        output[cbOutput] = '\0';
    }
    else if (pad == padPKCS7)
    {
        // TODO: 不知如何移除尾部的数据
    }

}
