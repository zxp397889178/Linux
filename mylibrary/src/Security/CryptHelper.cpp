/*
 * CryptoHelper.cpp
 *
 *  Created on: 2014年6月5日
 *      Author: Administrator
 */

#include "CryptHelper.h"
#include <vector>

using std::vector;



namespace UTILS
{
unsigned char ToHex(const unsigned char &x)
{
	return x > 9 ? x - 10 + 'A' : x + '0';
}

unsigned char FromHex(unsigned char x)
{
	unsigned char y = 0;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	return y;
}


static unsigned char cbc_iv[8] = { '0', '1', 'A', 'B', 'a', 'b', '9', '8' };

EVP_PKEY* CryptHelper::getKeyByPKCS1(const std::string &key, const int32_t keyType)
{
    RSA* rsa = getRsaKey(key, keyType);
    if(!rsa)
    {
        printf("getRsaKey failed !\n");
        return NULL;
    }
    EVP_PKEY* pkey = EVP_PKEY_new();
    if(1 != EVP_PKEY_assign_RSA(pkey, rsa))
    {
        printf("EVP_PKEY_assign_RSA failed !\n");
        RSA_free(rsa);
        EVP_PKEY_free(pkey);
        return NULL;
    }
    return pkey;
}

RSA* CryptHelper::getRsaKey(const std::string &key, const int32_t keyType)
{
    uint8_t *keyBuf;
    uint8_t *p;

    keyBuf = (uint8_t *) alloca(key.length());

    size_t keyLen(0);
    keyLen = base64Decode(keyBuf, (const uint8_t *) key.c_str(), key.length());
    if (0 > keyLen)
    {
        printf("base64Decode key failed !\n");
        return NULL;
    }

    //d2i_RSA_PUBKEY
    p = keyBuf;
    RSA *rsa =
            (keyType == 0) ?
                    d2i_RSA_PUBKEY(NULL, (const uint8_t **) &p, keyLen) :
                    d2i_RSAPrivateKey(NULL, (const uint8_t **) &p, keyLen);
    return rsa;
}

void CryptHelper::freeKey(RSA* key)
{
    if (key)
        RSA_free(key);
}

void CryptHelper::freeKey(EVP_PKEY* key)
{
    if (key)
        EVP_PKEY_free(key);
}


int32_t CryptHelper::signWithRsa(const std::string &data, const EVP_MD *type, EVP_PKEY* priKey, std::string &sign)
{
    EVP_MD_CTX mdCtx;
    EVP_SignInit(&mdCtx, type);
    EVP_SignUpdate(&mdCtx, data.c_str(), data.length());

    uint32_t signLen(EVP_PKEY_size(priKey)), outLen(0);
    uint8_t* signBuf = (uint8_t *) OPENSSL_malloc(signLen);
    uint8_t* outBuf = (uint8_t *) OPENSSL_malloc(signLen * 2);

    int32_t ret = EVP_SignFinal(&mdCtx, signBuf, &signLen, priKey);
    if (1 != ret)
    {
        printf("EVP_SignFinal failed\n");
    }
    else
    {
        if (0 > (outLen = EVP_EncodeBlock(outBuf, signBuf, signLen)))
        {
            ret = -1;
            printf("EVP_EncodeBlock failed\n");
        }
        else
        {
            sign.assign((char*)outBuf, outLen);
        }
    }

    OPENSSL_free(signBuf);
    OPENSSL_free(outBuf);

    return 1 == ret ? 0 : -1;
}

int32_t CryptHelper::verifySignWithRsa(const std::string &data, const std::string &sign, const EVP_MD *type, EVP_PKEY* pubKey)
{
    EVP_MD_CTX mdCtx;
    uint8_t* signSrc = (uint8_t *) OPENSSL_malloc(sign.length());
    int32_t signSrcLen = base64Decode(signSrc, (const uint8_t *)sign.c_str(), sign.length());
    if(0 > signSrcLen)
    {
        printf("sign base64Decode failed\n");
        OPENSSL_free(signSrc);
        return -1;
    }

    EVP_VerifyInit(&mdCtx, type);
    EVP_VerifyUpdate(&mdCtx, data.c_str(), data.length());
    int32_t ret = EVP_VerifyFinal(&mdCtx, signSrc, signSrcLen, pubKey);

    OPENSSL_free(signSrc);
    return 1 == ret ? 0 : -2;
}


int32_t CryptHelper::md5WithRsa(const std::string &data, std::string &sign, EVP_PKEY* priKey)
{
    return signWithRsa(data, EVP_md5(), priKey, sign);
}


int32_t CryptHelper::verifyMd5WithRsa(const std::string &data, const std::string &sign, EVP_PKEY* pubKey)
{
    return verifySignWithRsa(data, sign, EVP_md5(), pubKey);
}

int32_t CryptHelper::verifySha1WithRsa(const std::string &data, const std::string &sign, EVP_PKEY* pubKey)
{
    return verifySignWithRsa(data, sign, EVP_sha1(), pubKey);
}

int32_t CryptHelper::verifySha256WithRsa(const std::string &data, const std::string &sign, EVP_PKEY* pubKey)
{
	return verifySignWithRsa(data, sign, EVP_sha256(), pubKey);
}

int32_t CryptHelper::base64Encode(uint8_t *out, const uint8_t *in, int32_t inl)
{
    int32_t outl(0);

    outl = EVP_EncodeBlock(out, in, inl);
    if (0 > outl)
    {
        return -1;
    }
    return outl;

}

int32_t CryptHelper::base64Decode(uint8_t *out, const uint8_t *in, int32_t inl)
{
    int32_t outl(0), ret(0);

    if ('=' == in[inl - 1])
    {
        ret++;
    }
    if ('=' == in[inl - 2])
    {
        ret++;
    }
    outl = EVP_DecodeBlock(out, in, inl);
    if (0 > outl)
    {
        printf("EVP_DecodeBlock failed\n");
        return -1;
    }
    out[outl - ret] = '\0';
    return outl - ret;
}

string CryptHelper::UrlEncode(const char* sIn, int32_t inLen) {
    std::string sOut = "";
	for (int32_t i = 0; i < inLen; i++)
    {
        if (isalnum((unsigned char)sIn[i]) ||
            (sIn[i] == '-') ||
            (sIn[i] == '_') ||
            (sIn[i] == '.') ||
            (sIn[i] == '~'))
            sOut += sIn[i];
        else if (sIn[i] == ' ')
            sOut += "+";
        else
        {
            sOut += '%';
            sOut += ToHex((unsigned char)sIn[i] >> 4);
            sOut += ToHex((unsigned char)sIn[i] % 16);
        }
    }
    return sOut;
}

string CryptHelper::UrlDecode(const char* sIn, int32_t inLen) {
    std::string strTemp = "";
    for (int32_t i = 0; i < inLen; i++)
    {
        if (sIn[i] == '+') strTemp += ' ';
        else if (sIn[i] == '%')
        {
            unsigned char high = FromHex((unsigned char)sIn[++i]);
            unsigned char low = FromHex((unsigned char)sIn[++i]);
            strTemp += high*16 + low;
        }
        else strTemp += sIn[i];
    }
    return strTemp;
}

string CryptHelper::HmacWithSha1(const char * key, const char * input)
{
	uint32_t output_length = 0;

	const EVP_MD * engine = NULL;
	engine = EVP_sha1();

	unsigned char *buffer = (unsigned char*)malloc(EVP_MAX_MD_SIZE);
	memset(buffer, 0, EVP_MAX_MD_SIZE);

	HMAC_CTX ctx;
	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(key), engine, NULL);
	HMAC_Update(&ctx, (unsigned char*)input, strlen(input));
	HMAC_Final(&ctx, buffer, &output_length);
	HMAC_CTX_cleanup(&ctx);

	string output = string((const char*)buffer, output_length);
	free(buffer);

	return output;
}

string CryptHelper::HmacWithSha1ToHex(const char * key, const char * input)
{
	uint32_t output_length = 0;

	const EVP_MD * engine = NULL;
	engine = EVP_sha1();

	unsigned char *buffer = (unsigned char*)malloc(EVP_MAX_MD_SIZE);
	memset(buffer, 0, EVP_MAX_MD_SIZE);

	HMAC_CTX ctx;
	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key, strlen(key), engine, NULL);
	HMAC_Update(&ctx, (unsigned char*)input, strlen(input));
	HMAC_Final(&ctx, buffer, &output_length);
	HMAC_CTX_cleanup(&ctx);

	string output;
	char ch[10];
	char *pc = (char*)buffer;
	for (size_t i = 0; i < output_length; ++i)
	{
		uint8_t c = (uint8_t)pc[i];
		sprintf(ch, "%02x", c);
		output += ch;
	}

	free(buffer);
	return output;
}

string CryptHelper::DesEncrypt(const string cleartext, const string key, CRYPTO_MODE mode)
{
	string strCipherText;

	switch (mode) {
	case GENERAL:
	case ECB:
	{
		DES_cblock keyEncrypt;
		memset(keyEncrypt, 0, 8);

		if (key.length() <= 8)
			memcpy(keyEncrypt, key.c_str(), key.length());
		else
			memcpy(keyEncrypt, key.c_str(), 8);

		DES_key_schedule keySchedule;
		DES_set_key_unchecked(&keyEncrypt, &keySchedule);

		const_DES_cblock inputText;
		DES_cblock outputText;
		vector<unsigned char> vecCiphertext;
		unsigned char tmp[8];

		for (int i = 0; i <(int) cleartext.length() / 8; i++) {
			memcpy(inputText, cleartext.c_str() + i * 8, 8);
			DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
			memcpy(tmp, outputText, 8);

			for (int j = 0; j < 8; j++)
				vecCiphertext.push_back(tmp[j]);
		}

		if (cleartext.length() % 8 != 0) {
			int tmp1 = cleartext.length() / 8 * 8;
			int tmp2 = cleartext.length() - tmp1;
			memset(inputText, 0, 8);
			memcpy(inputText, cleartext.c_str() + tmp1, tmp2);

			DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
			memcpy(tmp, outputText, 8);

			for (int j = 0; j < 8; j++)
				vecCiphertext.push_back(tmp[j]);
		}

		strCipherText.clear();
		strCipherText.assign(vecCiphertext.begin(), vecCiphertext.end());
	}
	break;
	case CBC:
	{
		DES_cblock keyEncrypt, ivec;
		memset(keyEncrypt, 0, 8);

		if (key.length() <= 8)
			memcpy(keyEncrypt, key.c_str(), key.length());
		else
			memcpy(keyEncrypt, key.c_str(), 8);

		DES_key_schedule keySchedule;
		DES_set_key_unchecked(&keyEncrypt, &keySchedule);

		memcpy(ivec, cbc_iv, sizeof(cbc_iv));

		int iLength = cleartext.length() % 8 ? (cleartext.length() / 8 + 1) * 8 : cleartext.length();
		unsigned char* tmp = new unsigned char[iLength + 16];
		memset(tmp, 0, iLength);

		DES_ncbc_encrypt((const unsigned char*)cleartext.c_str(), tmp, cleartext.length() + 1, &keySchedule, &ivec, DES_ENCRYPT);

		strCipherText = (char*)tmp;

		delete[] tmp;
	}
	break;
	case CFB:
	{
		DES_cblock keyEncrypt, ivec;
		memset(keyEncrypt, 0, 8);

		if (key.length() <= 8)
			memcpy(keyEncrypt, key.c_str(), key.length());
		else
			memcpy(keyEncrypt, key.c_str(), 8);

		DES_key_schedule keySchedule;
		DES_set_key_unchecked(&keyEncrypt, &keySchedule);

		memcpy(ivec, cbc_iv, sizeof(cbc_iv));

		unsigned char* outputText = new unsigned char[cleartext.length()];
		memset(outputText, 0, cleartext.length());

		const unsigned char* tmp = (const unsigned char*)cleartext.c_str();

		DES_cfb_encrypt(tmp, outputText, 8, cleartext.length(), &keySchedule, &ivec, DES_ENCRYPT);

		strCipherText = (char*)outputText;

		delete[] outputText;
	}
	break;
	case TRIPLE_ECB:
	{
		DES_cblock ke1, ke2, ke3;
		memset(ke1, 0, 8);
		memset(ke2, 0, 8);
		memset(ke2, 0, 8);

		if (key.length() >= 24) {
			memcpy(ke1, key.c_str(), 8);
			memcpy(ke2, key.c_str() + 8, 8);
			memcpy(ke3, key.c_str() + 16, 8);
		}
		else if (key.length() >= 16) {
			memcpy(ke1, key.c_str(), 8);
			memcpy(ke2, key.c_str() + 8, 8);
			memcpy(ke3, key.c_str() + 16, key.length() - 16);
		}
		else if (key.length() >= 8) {
			memcpy(ke1, key.c_str(), 8);
			memcpy(ke2, key.c_str() + 8, key.length() - 8);
			memcpy(ke3, key.c_str(), 8);
		}
		else {
			memcpy(ke1, key.c_str(), key.length());
			memcpy(ke2, key.c_str(), key.length());
			memcpy(ke3, key.c_str(), key.length());
		}

		DES_key_schedule ks1, ks2, ks3;
		DES_set_key_unchecked(&ke1, &ks1);
		DES_set_key_unchecked(&ke2, &ks2);
		DES_set_key_unchecked(&ke3, &ks3);

		const_DES_cblock inputText;
		DES_cblock outputText;
		vector<unsigned char> vecCiphertext;
		unsigned char tmp[8];

		for (int i = 0; i < (int)cleartext.length() / 8; i++) {
			memcpy(inputText, cleartext.c_str() + i * 8, 8);
			DES_ecb3_encrypt(&inputText, &outputText, &ks1, &ks2, &ks3, DES_ENCRYPT);
			memcpy(tmp, outputText, 8);

			for (int j = 0; j < 8; j++)
				vecCiphertext.push_back(tmp[j]);
		}

		if (cleartext.length() % 8 != 0) {
			int tmp1 = cleartext.length() / 8 * 8;
			int tmp2 = cleartext.length() - tmp1;
			memset(inputText, 0, 8);
			memcpy(inputText, cleartext.c_str() + tmp1, tmp2);

			DES_ecb3_encrypt(&inputText, &outputText, &ks1, &ks2, &ks3, DES_ENCRYPT);
			memcpy(tmp, outputText, 8);

			for (int j = 0; j < 8; j++)
				vecCiphertext.push_back(tmp[j]);
		}

		strCipherText.clear();
		strCipherText.assign(vecCiphertext.begin(), vecCiphertext.end());
	}
	break;
	case TRIPLE_CBC:
	{
		DES_cblock ke1, ke2, ke3, ivec;
		memset(ke1, 0, 8);
		memset(ke2, 0, 8);
		memset(ke2, 0, 8);

		if (key.length() >= 24) {
			memcpy(ke1, key.c_str(), 8);
			memcpy(ke2, key.c_str() + 8, 8);
			memcpy(ke3, key.c_str() + 16, 8);
		}
		else if (key.length() >= 16) {
			memcpy(ke1, key.c_str(), 8);
			memcpy(ke2, key.c_str() + 8, 8);
			memcpy(ke3, key.c_str() + 16, key.length() - 16);
		}
		else if (key.length() >= 8) {
			memcpy(ke1, key.c_str(), 8);
			memcpy(ke2, key.c_str() + 8, key.length() - 8);
			memcpy(ke3, key.c_str(), 8);
		}
		else {
			memcpy(ke1, key.c_str(), key.length());
			memcpy(ke2, key.c_str(), key.length());
			memcpy(ke3, key.c_str(), key.length());
		}

		DES_key_schedule ks1, ks2, ks3;
		DES_set_key_unchecked(&ke1, &ks1);
		DES_set_key_unchecked(&ke2, &ks2);
		DES_set_key_unchecked(&ke3, &ks3);

		memcpy(ivec, cbc_iv, sizeof(cbc_iv));

		int iLength = cleartext.length() % 8 ? (cleartext.length() / 8 + 1) * 8 : cleartext.length();
		unsigned char* tmp = new unsigned char[iLength + 16];
		memset(tmp, 0, iLength);

		DES_ede3_cbc_encrypt((const unsigned char*)cleartext.c_str(), tmp, cleartext.length() + 1, &ks1, &ks2, &ks3, &ivec, DES_ENCRYPT);

		strCipherText = (char*)tmp;

		delete[] tmp;
	}
	break;
	default:
		break;
	}

	return strCipherText;
}

string CryptHelper::DesDecrypt(const string ciphertext, const string key, CRYPTO_MODE mode)
{
	string strClearText;

	switch (mode) {
	case GENERAL:
	case ECB:
	{
		DES_cblock keyEncrypt;
		memset(keyEncrypt, 0, 8);

		if (key.length() <= 8)
			memcpy(keyEncrypt, key.c_str(), key.length());
		else
			memcpy(keyEncrypt, key.c_str(), 8);

		DES_key_schedule keySchedule;
		DES_set_key_unchecked(&keyEncrypt, &keySchedule);

		const_DES_cblock inputText;
		DES_cblock outputText;
		vector<unsigned char> vecCleartext;
		unsigned char tmp[8];

		for (int i = 0; i < (int)ciphertext.length() / 8; i++) {
			memcpy(inputText, ciphertext.c_str() + i * 8, 8);
			DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
			memcpy(tmp, outputText, 8);

			for (int j = 0; j < 8; j++)
				vecCleartext.push_back(tmp[j]);
		}

		if (ciphertext.length() % 8 != 0) {
			int tmp1 = ciphertext.length() / 8 * 8;
			int tmp2 = ciphertext.length() - tmp1;
			memset(inputText, 0, 8);
			memcpy(inputText, ciphertext.c_str() + tmp1, tmp2);

			DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
			memcpy(tmp, outputText, 8);

			for (int j = 0; j < 8; j++)
				vecCleartext.push_back(tmp[j]);
		}

		strClearText.clear();
		strClearText.assign(vecCleartext.begin(), vecCleartext.end());
	}
	break;
	case CBC:
	{
		DES_cblock keyEncrypt, ivec;
		memset(keyEncrypt, 0, 8);

		if (key.length() <= 8)
			memcpy(keyEncrypt, key.c_str(), key.length());
		else
			memcpy(keyEncrypt, key.c_str(), 8);

		DES_key_schedule keySchedule;
		DES_set_key_unchecked(&keyEncrypt, &keySchedule);

		memcpy(ivec, cbc_iv, sizeof(cbc_iv));

		int iLength = ciphertext.length() % 8 ? (ciphertext.length() / 8 + 1) * 8 : ciphertext.length();
		unsigned char* tmp = new unsigned char[iLength];
		memset(tmp, 0, iLength);

		DES_ncbc_encrypt((const unsigned char*)ciphertext.c_str(), tmp, ciphertext.length() + 1, &keySchedule, &ivec, DES_DECRYPT);

		strClearText = (char*)tmp;

		delete[] tmp;
	}
	break;
	case CFB:
	{
		DES_cblock keyEncrypt, ivec;
		memset(keyEncrypt, 0, 8);

		if (key.length() <= 8)
			memcpy(keyEncrypt, key.c_str(), key.length());
		else
			memcpy(keyEncrypt, key.c_str(), 8);

		DES_key_schedule keySchedule;
		DES_set_key_unchecked(&keyEncrypt, &keySchedule);

		memcpy(ivec, cbc_iv, sizeof(cbc_iv));

		unsigned char* outputText = new unsigned char[ciphertext.length()];
		memset(outputText, 0, ciphertext.length());

		const unsigned char* tmp = (const unsigned char*)ciphertext.c_str();

		DES_cfb_encrypt(tmp, outputText, 8, 32/*ciphertext.length() - 16*/, &keySchedule, &ivec, DES_DECRYPT);

		strClearText = (char*)outputText;

		delete[] outputText;
	}
	break;
	case TRIPLE_ECB:
	{
		DES_cblock ke1, ke2, ke3;
		memset(ke1, 0, 8);
		memset(ke2, 0, 8);
		memset(ke2, 0, 8);

		if (key.length() >= 24) {
			memcpy(ke1, key.c_str(), 8);
			memcpy(ke2, key.c_str() + 8, 8);
			memcpy(ke3, key.c_str() + 16, 8);
		}
		else if (key.length() >= 16) {
			memcpy(ke1, key.c_str(), 8);
			memcpy(ke2, key.c_str() + 8, 8);
			memcpy(ke3, key.c_str() + 16, key.length() - 16);
		}
		else if (key.length() >= 8) {
			memcpy(ke1, key.c_str(), 8);
			memcpy(ke2, key.c_str() + 8, key.length() - 8);
			memcpy(ke3, key.c_str(), 8);
		}
		else {
			memcpy(ke1, key.c_str(), key.length());
			memcpy(ke2, key.c_str(), key.length());
			memcpy(ke3, key.c_str(), key.length());
		}

		DES_key_schedule ks1, ks2, ks3;
		DES_set_key_unchecked(&ke1, &ks1);
		DES_set_key_unchecked(&ke2, &ks2);
		DES_set_key_unchecked(&ke3, &ks3);

		const_DES_cblock inputText;
		DES_cblock outputText;
		vector<unsigned char> vecCleartext;
		unsigned char tmp[8];

		for (int i = 0; i < (int)ciphertext.length() / 8; i++) {
			memcpy(inputText, ciphertext.c_str() + i * 8, 8);
			DES_ecb3_encrypt(&inputText, &outputText, &ks1, &ks2, &ks3, DES_DECRYPT);
			memcpy(tmp, outputText, 8);

			for (int j = 0; j < 8; j++)
				vecCleartext.push_back(tmp[j]);
		}

		if (ciphertext.length() % 8 != 0) {
			int tmp1 = ciphertext.length() / 8 * 8;
			int tmp2 = ciphertext.length() - tmp1;
			memset(inputText, 0, 8);
			memcpy(inputText, ciphertext.c_str() + tmp1, tmp2);

			DES_ecb3_encrypt(&inputText, &outputText, &ks1, &ks2, &ks3, DES_DECRYPT);
			memcpy(tmp, outputText, 8);

			for (int j = 0; j < 8; j++)
				vecCleartext.push_back(tmp[j]);
		}

		strClearText.clear();
		strClearText.assign(vecCleartext.begin(), vecCleartext.end());
	}
	break;
	case TRIPLE_CBC:
	{
		DES_cblock ke1, ke2, ke3, ivec;
		memset(ke1, 0, 8);
		memset(ke2, 0, 8);
		memset(ke2, 0, 8);

		if (key.length() >= 24) {
			memcpy(ke1, key.c_str(), 8);
			memcpy(ke2, key.c_str() + 8, 8);
			memcpy(ke3, key.c_str() + 16, 8);
		}
		else if (key.length() >= 16) {
			memcpy(ke1, key.c_str(), 8);
			memcpy(ke2, key.c_str() + 8, 8);
			memcpy(ke3, key.c_str() + 16, key.length() - 16);
		}
		else if (key.length() >= 8) {
			memcpy(ke1, key.c_str(), 8);
			memcpy(ke2, key.c_str() + 8, key.length() - 8);
			memcpy(ke3, key.c_str(), 8);
		}
		else {
			memcpy(ke1, key.c_str(), key.length());
			memcpy(ke2, key.c_str(), key.length());
			memcpy(ke3, key.c_str(), key.length());
		}

		DES_key_schedule ks1, ks2, ks3;
		DES_set_key_unchecked(&ke1, &ks1);
		DES_set_key_unchecked(&ke2, &ks2);
		DES_set_key_unchecked(&ke3, &ks3);

		memcpy(ivec, cbc_iv, sizeof(cbc_iv));

		int iLength = ciphertext.length() % 8 ? (ciphertext.length() / 8 + 1) * 8 : ciphertext.length();
		unsigned char* tmp = new unsigned char[iLength];
		memset(tmp, 0, iLength);

		DES_ede3_cbc_encrypt((const unsigned char*)ciphertext.c_str(), tmp, ciphertext.length() + 1, &ks1, &ks2, &ks3, &ivec, DES_DECRYPT);

		strClearText = (char*)tmp;

		delete[] tmp;
	}
	break;
	default:
		break;
	}

	return strClearText;
}

}  //end namespace UTILS
