/*
* =====================================================================================
*
*       Filename:  ShaWrapper.cpp
*
*    Description:  对SHA加密算法封装
*
*        Version:  1.0
*        Created:  2017年6月14日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/
#include "ShaWrapper.h"
#include <openssl/crypto.h>  // OPENSSL_cleanse  
#include <openssl/sha.h> 
#include <openssl/hmac.h> 
#include <openssl/evp.h>
#include <stdint.h>
#include <memory.h>
#include <vector>  
#include "Base64Wrapper.h"

using std::vector;

int32_t ShaWrapper::SHA1(const uint8_t *input, size_t cbInput, uint8_t *cipher, size_t &cbCipher)
{
    if (input==NULL || cbInput==0 || 
            cipher==NULL || cbCipher<SHA_DIGEST_LENGTH)
    {
        return -1;
    }

	SHA_CTX sha1;
	SHA1_Init(&sha1);
	SHA1_Update(&sha1, input, cbInput);
	SHA1_Final(cipher, &sha1);
	OPENSSL_cleanse(&sha1, sizeof(sha1));
    
    return 0;
}

CMyString ShaWrapper::sha1(const CMyString &data)
{
	uint8_t hash[SHA_DIGEST_LENGTH] = {0};
    size_t cbHash = sizeof(hash);
    int32_t ret = ShaWrapper::SHA1((const uint8_t*)data.c_str(), 
            data.length(), hash, cbHash);
    if (ret < 0)
    {
        return "";
    }
    
    CTempBuffer tmpbuf(cbHash*2);
    HexToStr(hash, cbHash, tmpbuf.data(), cbHash*2);
    return CMyString(tmpbuf.data());
}

CMyString ShaWrapper::Sha1WithBase64(const CMyString &data)
{
    uint8_t hash[SHA_DIGEST_LENGTH+1] = {0};
    size_t cbHash(SHA_DIGEST_LENGTH);
    int32_t ret = ShaWrapper::SHA1((const uint8_t*)data.c_str(),
            data.length(), hash, cbHash);
    if (ret < 0)
    {
        return "";
    }
    
    return CBase64Wrapper::Encode((const char*)hash, cbHash, false);
}

CMyString ShaWrapper::Sha256WithBase64(const CMyString &data)
{
    uint8_t hash[SHA256_DIGEST_LENGTH+1] = {0};
    size_t cbHash(SHA256_DIGEST_LENGTH);
    int32_t ret = ShaWrapper::SHA256((const uint8_t*)data.c_str(),
            data.length(), hash, cbHash);
    if (ret < 0)
    {
        return "";
    }
    
    return CBase64Wrapper::Encode((const char*)hash, cbHash, false);
}

CMyString ShaWrapper::sha224(const CMyString &data)
{
	uint8_t hash[SHA224_DIGEST_LENGTH] = {0};
    size_t cbHash = sizeof(hash);
	SHA256_CTX sha224;
	SHA224_Init(&sha224);
	SHA224_Update(&sha224, data.c_str(), data.size());
	SHA224_Final(hash, &sha224);
	OPENSSL_cleanse(&sha224, sizeof(sha224));

    CTempBuffer tmpbuf(cbHash*2);
    HexToStr(hash, cbHash, tmpbuf.data(), cbHash*2);
    return CMyString(tmpbuf.data());
}

int32_t ShaWrapper::SHA256(const uint8_t *input, size_t cbInput,
        uint8_t *cipher, size_t &cbCipher)
{
    if (input==NULL || cbInput==0 || 
            cipher==NULL || cbCipher<SHA256_DIGEST_LENGTH)
    {
        return -1;
    }

	//uint8_t hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, input, cbInput);
	SHA256_Final(cipher, &sha256);
	OPENSSL_cleanse(&sha256, sizeof(sha256));

    return 0;
}

CMyString ShaWrapper::sha256(const CMyString &data)
{
	uint8_t hash[SHA256_DIGEST_LENGTH] = {0};
    size_t cbHash = sizeof(hash);
    int32_t ret = ShaWrapper::SHA256((const uint8_t*)data.c_str(),
            data.length(), hash, cbHash);
    if (ret < 0)
    {
        return "";
    }

    CTempBuffer tmpbuf(cbHash*2);
    HexToStr(hash, cbHash, tmpbuf.data(), cbHash*2);
    return CMyString(tmpbuf.data());
}

CMyString ShaWrapper::sha384(const CMyString &data)
{
	uint8_t hash[SHA384_DIGEST_LENGTH] = {0};
    size_t cbHash=sizeof(hash);
	SHA512_CTX sha384;
	SHA384_Init(&sha384);
	SHA384_Update(&sha384, data.c_str(), data.size());
	SHA384_Final(hash, &sha384);
	OPENSSL_cleanse(&sha384, sizeof(sha384));

    CTempBuffer tmpbuf(cbHash*2);
    HexToStr(hash, cbHash, tmpbuf.data(), cbHash*2);
    return CMyString(tmpbuf.data());
}

CMyString ShaWrapper::sha512(const CMyString &data)
{
	uint8_t hash[SHA512_DIGEST_LENGTH] = {0};
    size_t cbHash=sizeof(hash);
	SHA512_CTX sha512;
	SHA512_Init(&sha512);
	SHA512_Update(&sha512, data.c_str(), data.size());
	SHA512_Final(hash, &sha512);
	OPENSSL_cleanse(&sha512, sizeof(sha512));

    CTempBuffer tmpbuf(cbHash*2);
    HexToStr(hash, cbHash, tmpbuf.data(), cbHash*2);
    return CMyString(tmpbuf.data());
}

int32_t ShaWrapper::Hmac(const char *type, const char *key, const char *input, uint8_t *cipher, size_t &cbCipher)
{
	const EVP_MD * engine = NULL;
	if (strcasecmp("sha512", type) == 0)
	{
		engine = EVP_sha512();
	}
	else if (strcasecmp("sha256", type) == 0)
	{
		engine = EVP_sha256();
	}
	else if (strcasecmp("sha1", type) == 0)
	{
		engine = EVP_sha1();
	}
	else if (strcasecmp("md5", type) == 0)
	{
		engine = EVP_md5();
	}
	else if (strcasecmp("sha224", type) == 0)
	{
		engine = EVP_sha224();
	}
	else if (strcasecmp("sha384", type) == 0)
	{
		engine = EVP_sha384();
	}
	else if (strcasecmp("sha", type) == 0)
	{
		engine = EVP_sha();
	}
	else if (strcasecmp("md2", type) == 0)
	{
		engine = EVP_md2();
	}
	else 
	{
		return -1;
	}

    uint8_t buffer[EVP_MAX_MD_SIZE] = {0};
    uint32_t output_length = EVP_MAX_MD_SIZE;

	HMAC_CTX ctx;
	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(key), engine, NULL);
	HMAC_Update(&ctx, (uint8_t*)input, strlen(input));
	HMAC_Final(&ctx, buffer, &output_length);
	HMAC_CTX_cleanup(&ctx);

    int32_t result(-3); // 空间不足
    if (cbCipher >= output_length)
    {
        memcpy(cipher, buffer, output_length);
        result = 0; // 正常
    }
    cbCipher = output_length;
    return result;
}

CMyString ShaWrapper::Hmac(const char *type, const char *key, const char *input)
{
/*    
	const EVP_MD * engine = NULL;
	if (strcasecmp("sha512", type) == 0)
	{
		engine = EVP_sha512();
	}
	else if (strcasecmp("sha256", type) == 0)
	{
		engine = EVP_sha256();
	}
	else if (strcasecmp("sha1", type) == 0)
	{
		engine = EVP_sha1();
	}
	else if (strcasecmp("md5", type) == 0)
	{
		engine = EVP_md5();
	}
	else if (strcasecmp("sha224", type) == 0)
	{
		engine = EVP_sha224();
	}
	else if (strcasecmp("sha384", type) == 0)
	{
		engine = EVP_sha384();
	}
	else if (strcasecmp("sha", type) == 0)
	{
		engine = EVP_sha();
	}
	else if (strcasecmp("md2", type) == 0)
	{
		engine = EVP_md2();
	}
	else 
	{
		return CMyString();
	}

    uint8_t buffer[EVP_MAX_MD_SIZE] = {0};
    uint32_t output_length = EVP_MAX_MD_SIZE;

	HMAC_CTX ctx;
	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(key), engine, NULL);
	HMAC_Update(&ctx, (uint8_t*)input, strlen(input));
	HMAC_Final(&ctx, buffer, &output_length);
	HMAC_CTX_cleanup(&ctx);
*/
    uint8_t cipher[EVP_MAX_MD_SIZE] = {0};
    size_t cbCipher(EVP_MAX_MD_SIZE);
    int32_t ret = ShaWrapper::Hmac(type, key, input, cipher, cbCipher);
    if (ret != 0)
    {
        return "";
    }

    CTempBuffer tmpbuf(cbCipher*2+1);
    HexToStr(cipher, cbCipher, tmpbuf.data(), cbCipher*2);
    return CMyString(tmpbuf.data());
}

