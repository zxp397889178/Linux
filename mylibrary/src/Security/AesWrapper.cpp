/*
 * =====================================================================================
 *
 *       Filename:  AesWrapper.cpp
 *
 *    Description:  aes with openssl
 *
 *        Version:  1.0
 *        Created:  08/29/2019 09:26:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "AesWrapper.h"
#include <openssl/aes.h>

int32_t CAesWrapper::Encode(const uint8_t *input, size_t cbInput, 
        const CMyString &key,
        uint8_t *&cipher, size_t &cbCipher,
        enum PaddingMode mode)
{
    if (input==NULL || cbInput==0)
    {
        return -1;
    }
    AES_KEY     aesKey;
    if (AES_set_encrypt_key((const uint8_t*)key.c_str(), key.length()*8, &aesKey) != 0)
    {
        return -2;
    }

    uint8_t inputText[AES_BLOCK_SIZE+1] = {0, };
    uint8_t outputText[AES_BLOCK_SIZE+1] = {0, };
    std::vector<uint8_t> vecCipher;
    for(int32_t i=0; i<(int)(cbInput/AES_BLOCK_SIZE); ++i)
    {
        memcpy(inputText, &input[i*AES_BLOCK_SIZE], AES_BLOCK_SIZE);
        AES_encrypt(inputText, outputText, &aesKey);
        for(int32_t j=0; j<AES_BLOCK_SIZE; ++j)
        {
            vecCipher.push_back(outputText[j]);
        }
    }

    int32_t modval = cbInput%AES_BLOCK_SIZE;
    if (modval != 0)
    {
        int32_t tmp1 = cbInput/AES_BLOCK_SIZE * AES_BLOCK_SIZE;
        int32_t tmp2 = cbInput - tmp1;

        int32_t padding = 0;
        if (mode == padPKCS5 || mode == padPKCS7)
        {
            padding = AES_BLOCK_SIZE - tmp2;
        }
        memset(inputText, padding, AES_BLOCK_SIZE);
        memcpy(inputText, &input[tmp1], tmp2);

        AES_encrypt(inputText, outputText, &aesKey);
        for(int32_t j=0; j<AES_BLOCK_SIZE; ++j)
        {
            vecCipher.push_back(outputText[j]);
        }
    }
    if (modval == 0 && mode==padPKCS5 )
    {
        int32_t padding = 8;
        memset(inputText, padding, AES_BLOCK_SIZE);
        AES_encrypt(inputText, outputText, &aesKey);
        for(int32_t j=0; j<AES_BLOCK_SIZE; ++j)
        {
            vecCipher.push_back(outputText[j]);
        }
    }

    cbCipher = vecCipher.size();
    cipher = new uint8_t[cbCipher+1];
    for(size_t k=0; k<cbCipher; ++k)
    {
        cipher[k] = vecCipher[k];
    }
    return 0;
}

int32_t CAesWrapper::Decode(const uint8_t *cipher, size_t cbCipher, 
        const CMyString &key,
        uint8_t *&output, size_t &cbOutput,
        enum PaddingMode mode)
{
    if (cipher==NULL || cbCipher==0 )
    {
        return -1;
    }
    if ((cbCipher % AES_BLOCK_SIZE) != 0)
    {
        return -2;
    }
    AES_KEY aesKey;
    if (AES_set_decrypt_key((const uint8_t*)key.c_str(), key.length()*8, &aesKey) != 0)
    {
        return -3;
    }

    uint8_t inputText[AES_BLOCK_SIZE+1] = {0, };
    uint8_t outputText[AES_BLOCK_SIZE+1] = {0, };
    std::vector<uint8_t> vecClear;
    for(int32_t i=0; i<(int32_t)(cbCipher/AES_BLOCK_SIZE); ++i)
    {
        memcpy(inputText, &cipher[i*AES_BLOCK_SIZE], AES_BLOCK_SIZE);
        AES_decrypt(inputText, outputText, &aesKey);
        for(int32_t j=0; j<AES_BLOCK_SIZE; ++j)
        {
            vecClear.push_back(outputText[j]);
        }
    }

    cbOutput= vecClear.size();
    //printf("output length=%d\n", cbOutput);
    output = new uint8_t[cbOutput+1];
    for(size_t k=0; k<cbOutput; ++k)
    {
        output[k] = vecClear[k];
    }

    if (mode == padPKCS5)
    {
        uint8_t unpadding = output[cbOutput-1];
        cbOutput = cbOutput - unpadding;
        output[cbOutput]='\0';
    }
    else if (mode == padPKCS7)
    {
        // TODO: 不知如何移除尾部的数据
    }

    return 0;
}

int32_t CAesWrapper::EncodeCBC(const uint8_t *input, size_t cbInput,
            const CMyString &key, const CMyString &iv,
            uint8_t *&cipher, size_t &cbCipher,
            enum PaddingMode mode)
{
    if (input==NULL || cbInput==0)
    {
        return -1;
    }
    AES_KEY aesKey;
    if (AES_set_encrypt_key((const uint8_t*)key.c_str(), key.length()*8, &aesKey) != 0)
    {
        return -2;
    }

    uint8_t ivbuf[AES_BLOCK_SIZE+1] = {0, };
    for(int32_t i=0; i<AES_BLOCK_SIZE; ++i)
    {
        ivbuf[i] = iv[i];
    }

    uint8_t inputText[AES_BLOCK_SIZE+1] = {0, };
    uint8_t outputText[AES_BLOCK_SIZE+1] = {0, };
    std::vector<uint8_t> vecCipher;
    for(int32_t i=0; i<(int)(cbInput/AES_BLOCK_SIZE); ++i)
    {
        memcpy(inputText, &input[i*AES_BLOCK_SIZE], AES_BLOCK_SIZE);
        AES_cbc_encrypt(inputText, outputText, AES_BLOCK_SIZE, &aesKey, ivbuf, AES_ENCRYPT);
        for(int32_t j=0; j<AES_BLOCK_SIZE; ++j)
        {
            vecCipher.push_back(outputText[j]);
        }
    }
    
    int32_t modval = cbInput%AES_BLOCK_SIZE;
    if (modval != 0)
    {
        // 有没对齐的内容
        int32_t tmp1 = cbInput/AES_BLOCK_SIZE * AES_BLOCK_SIZE;
        int32_t tmp2 = cbInput - tmp1;

        int32_t padding = 0;
        if (mode == padPKCS5 || mode == padPKCS7)
        {
            padding = AES_BLOCK_SIZE - tmp2;
        }
        memset(inputText, padding, AES_BLOCK_SIZE);
        memcpy(inputText, &input[tmp1], tmp2);

        AES_cbc_encrypt(inputText, outputText, AES_BLOCK_SIZE, &aesKey, ivbuf, AES_ENCRYPT);
        for(int32_t j=0; j<AES_BLOCK_SIZE; ++j)
        {
            vecCipher.push_back(outputText[j]);
        }
    }
    if (modval == 0 && mode==padPKCS5 )
    {
        int32_t padding = 8;
        memset(inputText, padding, AES_BLOCK_SIZE);
        AES_cbc_encrypt(inputText, outputText, AES_BLOCK_SIZE, &aesKey, ivbuf, AES_ENCRYPT);
        for(int32_t j=0; j<AES_BLOCK_SIZE; ++j)
        {
            vecCipher.push_back(outputText[j]);
        }
    }

    cbCipher = vecCipher.size();
    cipher = new uint8_t[cbCipher+1];
    for(size_t k=0; k<cbCipher; ++k)
    {
        cipher[k] = vecCipher[k];
    }

    return 0;
}

int32_t CAesWrapper::DecodeCBC(const uint8_t *cipher, size_t cbCipher,
            const CMyString &key, const CMyString &iv,
            uint8_t *&output, size_t &cbOutput,
            enum PaddingMode mode)
{
    if (cipher==NULL || cbCipher==0 )
    {
        return -1;
    }
    if ((cbCipher % AES_BLOCK_SIZE) != 0)
    {
        return -2;
    }
    AES_KEY aesKey;
    if (AES_set_decrypt_key((const uint8_t*)key.c_str(), key.length()*8, &aesKey) != 0)
    {
        return -3;
    }

    uint8_t ivbuf[AES_BLOCK_SIZE+1] = {0, };
    for(int32_t i=0; i<AES_BLOCK_SIZE; ++i)
    {
        ivbuf[i] = iv[i];
    }

    uint8_t inputText[AES_BLOCK_SIZE+1] = {0, };
    uint8_t outputText[AES_BLOCK_SIZE+1] = {0, };
    std::vector<uint8_t> vecClear;
    for(int32_t i=0; i<(int32_t)(cbCipher/AES_BLOCK_SIZE); ++i)
    {
        memcpy(inputText, &cipher[i*AES_BLOCK_SIZE], AES_BLOCK_SIZE);
        AES_cbc_encrypt(inputText, outputText, AES_BLOCK_SIZE, &aesKey, ivbuf, AES_DECRYPT);
        for(int32_t j=0; j<AES_BLOCK_SIZE; ++j)
        {
            vecClear.push_back(outputText[j]);
        }
    }

    cbOutput= vecClear.size();
    //printf("output length=%d\n", cbOutput);
    output = new uint8_t[cbOutput+1];
    for(size_t k=0; k<cbOutput; ++k)
    {
        output[k] = vecClear[k];
    }

    if (mode == padPKCS5)
    {
        uint8_t unpadding = output[cbOutput-1];
        cbOutput = cbOutput - unpadding;
        output[cbOutput]='\0';
    }
    else if (mode == padPKCS7)
    {
        // TODO: 不知如何移除尾部的数据
    }

    return 0;
}


