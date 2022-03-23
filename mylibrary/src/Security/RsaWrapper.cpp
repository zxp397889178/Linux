/*
 * =====================================================================================
 *
 *       Filename:  RsaWrapper.cpp
 *
 *    Description:  ras base openeel
 *
 *        Version:  1.0
 *        Created:  08/29/2019 11:18:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "RsaWrapper.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <iostream>
#include <fstream>

#include "ShaWrapper.h"
#include "Base64Wrapper.h"

#define KEY_LENGTH      2048    // 密钥长度
#define PUB_KEY_FILE    "pubkey.pem"
#define PRI_KEY_FILE    "prikey.pem"

void CRsaWrapper::generateRSAKey(CMyString &strPubKey, CMyString &strPriKey)
{
    size_t pri_len;
    size_t pub_len;
    char* pri_key = NULL;
    char* pub_key = NULL;

    // 生成密钥对 
    RSA *keypair = RSA_generate_key(KEY_LENGTH, RSA_3, NULL, NULL);

    BIO *pri = BIO_new(BIO_s_mem());
    BIO *pub = BIO_new(BIO_s_mem());

    PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
    PEM_write_bio_RSAPublicKey(pub, keypair);

    // get length
    pri_len = BIO_pending(pri);
    pub_len = BIO_pending(pub);

    // keypair get string
    pri_key = (char*)malloc(pri_len+1);
    BIO_read(pri, pri_key, pri_len);
    pri_key[pri_len]='\0';
    strPriKey = pri_key;

    pub_key = (char*)malloc(pub_len+1);
    BIO_read(pub, pub_key, pub_len);
    pub_key[pub_len]='\0';
    strPubKey = pub_key;

    // free memory
    RSA_free(keypair);
    BIO_free_all(pub);
    BIO_free_all(pri);
    free(pri_key);
    free(pub_key);

    // save to disk
    std::fstream pubf(PUB_KEY_FILE, std::ios::out);
    if (!pubf.fail())
    {
        pubf << strPubKey.c_str();
        pubf.close();
    }

    std::fstream prif(PRI_KEY_FILE, std::ios::out);
    if (!prif.fail())
    {
        prif << strPriKey.c_str();
        prif.close();
    }
}

int32_t CRsaWrapper::Encrypt(const uint8_t *input, size_t cbInput, 
        const CMyString &strPubKey, 
        uint8_t *&cipher, size_t &cbCipher)
{
    if (input==NULL||cbInput==0)
    {
        return -1;
    }
    RSA *rsa = RSA_new();
    BIO *keybio = BIO_new_mem_buf((uint8_t*)strPubKey.c_str(), -1);
    // 此处有三种方法  
    // 1, 读取内存里生成的密钥对，再从内存生成rsa  
    // 2, 读取磁盘里生成的密钥对文本文件，在从内存生成rsa  
    // 3，直接从读取文件指针生成rsa  
    rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);

    cbCipher = 0;
    int32_t cipher_len = RSA_size(rsa);
    cipher = (uint8_t*)malloc(cipher_len+1);
    memset(cipher, 0, cipher_len+1);
    int32_t ret = RSA_public_encrypt(cbInput, input, cipher, rsa, RSA_PKCS1_PADDING);

    BIO_free_all(keybio);
    RSA_free(rsa);
    if (ret<0)
    {
        free(cipher);
        cipher = NULL;
        return -1000 + ret;
    }
    cbCipher = ret;
    return ret;
}

int32_t CRsaWrapper::Decrypt(const uint8_t *cipher, size_t cbCipher, 
        const CMyString &strPriKey,
        uint8_t *&output, size_t &cbOutput)
{
    if (cipher==NULL||cbCipher==0)
    {
        return -1;
    }

    RSA *rsa = RSA_new();
    BIO *keybio = BIO_new_mem_buf((uint8_t*)strPriKey.c_str(), -1);
    // 此处有三种方法  
    // 1, 读取内存里生成的密钥对，再从内存生成rsa  
    // 2, 读取磁盘里生成的密钥对文本文件，在从内存生成rsa  
    // 3，直接从读取文件指针生成rsa  
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

    cbOutput = 0;
    int32_t output_len = RSA_size(rsa);
    output = (uint8_t*)malloc(output_len+1);
    memset(output, 0, output_len+1);
    int32_t ret = RSA_private_decrypt(cbCipher, cipher, output, rsa, RSA_PKCS1_PADDING);
    printf("decrypt result=%d, output size=%u\n", ret, output_len);
    BIO_free_all(keybio);
    RSA_free(rsa);
    if (ret<0)
    {
        free(output);
        output = NULL;
        return -1000+ret;
    }
    cbOutput = ret;
    return ret;
}

int32_t CRsaWrapper::rsaSign(const uint8_t *input, size_t cbInput, 
        const CMyString &strPriKey, CMyString &strCipher)
{
    int32_t result(-1);
    const char* key_cstr = strPriKey.c_str();
    int32_t key_len = strPriKey.length();
    BIO *p_key_bio = BIO_new_mem_buf((void *)key_cstr, key_len);
    RSA *p_rsa = PEM_read_bio_RSAPrivateKey(p_key_bio, NULL, NULL, NULL);
    if (NULL != p_rsa)
    {
        //printf("\nrsa valid. \n");

        // 对 input 做 SHA256 加密
        uint8_t hash[SHA256_DIGEST_LENGTH+1] = {0};
        //size_t cbHash = sizeof(hash);
        SHA256(input, cbInput, hash);
        /*
        if (shaType == sha256)
            ShaWrapper::SHA256(input, cbInput, hash, cbHash);
        else
            ShaWrapper::SHA1(input, cbInput, hash, cbHash);
        */
        /*
        printf("hash result:\n");
        for(size_t i=0; i<sizeof(hash); ++i)
        {
            printf("%02x ", hash[i]);
        }
        printf("\n");
        */

        // 对 hash 做 RSA 加密
        uint8_t sign[XRSA_KEY_BITS / 8 + 1] = {0};
        uint32_t sign_len = sizeof(sign);
        int ret = RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, &sign_len, p_rsa);

        /*
        printf("\n\nsign:\n");
        for(size_t i=0; i<sign_len; ++i)
        {
            printf("%02x ", sign[i]);
        }
        printf("\nrsa sign result %d, sign_len=%d, sizeof(sign)=%u\n",
                ret, sign_len, sizeof(sign));
        //if (0 != ret && sizeof(sign) == sign_len) 
        */
        if (ret > 0 && sign_len>0)
        {
            //printf("base64 encode.\n");
            // 对 sign 做 Base64 加密
            strCipher = CBase64Wrapper::Encode((const char*)&sign[0], sign_len, false);
            result = 0;
        }

    }
    RSA_free(p_rsa);
    BIO_free(p_key_bio);

    return result;
}

bool CRsaWrapper::rsaVerify(const uint8_t *input, size_t cbInput,
            const CMyString &strSign, const CMyString &strPubKey)
{
    bool result(false);
    const char *key_cstr = strPubKey.c_str();
    int key_len = strPubKey.length(); 
    BIO *p_key_bio = BIO_new_mem_buf((void *)key_cstr, key_len);
    RSA *p_rsa = PEM_read_bio_RSA_PUBKEY(p_key_bio, NULL, NULL, NULL);
    if (p_rsa != NULL)
    {
        // SHA 解密
        uint8_t hash[SHA256_DIGEST_LENGTH] = {0};
        size_t cbHash = sizeof(hash);
        ShaWrapper::SHA256(input, cbInput, hash, cbHash);

        // Base64 解密
        //uint8_t sign_cstr[XRSA_KEY_BITS / 8] = {0};
        //int sign_len = XRSA_KEY_BITS / 8;
        CMyString strSign2 = CBase64Wrapper::Decode(strSign.c_str(), strSign.length(), false );

        // RSA verify
        uint32_t sign_len = XRSA_KEY_BITS / 8; 
        int ret = RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH, 
                (uint8_t*)strSign2.c_str(), sign_len, p_rsa);
        if (ret > 0)
        {
            result = true;
        }
    }
    RSA_free(p_rsa);
    BIO_free(p_key_bio); 
    return result;
}

