/*
 * CryptoHelper.h
 *
 *  Created on: 2014年6月5日
 *      Author: Administrator
 */

#ifndef CRYPTOHELPER_H_
#define CRYPTOHELPER_H_

#include <openssl/ossl_typ.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/hmac.h>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <string>
#include <openssl/des.h>  

using namespace std;



namespace UTILS
{

	unsigned char ToHex(const unsigned char &x);
	unsigned char FromHex(unsigned char x);

enum EkeyType
{
    kPubKey = 0,
    kPrivateKey = 1,
};

typedef enum {
	GENERAL = 0,
	ECB,
	CBC,
	CFB,
	OFB,
	TRIPLE_ECB,
	TRIPLE_CBC
}CRYPTO_MODE;

class CryptHelper
{
public:
    CryptHelper()
    {
    }
    ~CryptHelper()
    {
    }

    static EVP_PKEY* getKeyByPKCS1(const std::string &key, const int32_t keyType);

    static RSA* getRsaKey(const std::string &key, const int32_t keyType);
    static void freeKey(RSA* key);
    static void freeKey(EVP_PKEY* key);

    static int32_t signWithRsa(const std::string &data, const EVP_MD *type, EVP_PKEY* key, std::string &sign);
    static int32_t verifySignWithRsa(const std::string &data, const std::string &sign, const EVP_MD *type, EVP_PKEY* key);

    static int32_t md5WithRsa(const std::string &data, std::string &sign, EVP_PKEY* priKey);
    static int32_t verifyMd5WithRsa(const std::string &data, const std::string &sign, EVP_PKEY* pubKey);
	static int32_t verifySha1WithRsa(const std::string &data, const std::string &sign, EVP_PKEY* pubKey);
	static int32_t verifySha256WithRsa(const std::string &data, const std::string &sign, EVP_PKEY* pubKey);


    static int32_t base64Encode(uint8_t *out, const uint8_t *in, int32_t inl);
    static int32_t base64Decode(uint8_t *out, const uint8_t *in, int32_t inl);

	static string UrlEncode(const char* in, int32_t inLen);
	static string UrlDecode(const char* in, int32_t inLen);

	static string HmacWithSha1(const char * key, const char * input);
	static string HmacWithSha1ToHex(const char * key, const char * input);

	static string DesEncrypt(const string cleartext, const string key, CRYPTO_MODE mode);
	static string DesDecrypt(const string ciphertext, const string key, CRYPTO_MODE mode);


};

}  //end namespace UTILS

#endif /* CRYPTOHELPER_H_ */
