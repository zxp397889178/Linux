/*
 * =====================================================================================
 *
 *       Filename:  test_rsa2_1.cpp
 *
 *    Description:  origin rsaSign / rsaVerify
 *
 *        Version:  1.0
 *        Created:  09/22/2019 12:22:47 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "BaseCode.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <time.h>


using namespace std;

string base64Encode(const unsigned char *bytes, int len);
bool base64Decode(const string &str, unsigned char *bytes, int &len);
string rsaSign(const string &content, const string &key);
bool rsaVerify(const string &content, const string &sign, const string &key);

#define XRSA_KEY_BITS 2048

/** 此处替换为开发者使用openssl生成的rsa私钥 **/ 
std::string pKey = "-----BEGIN RSA PRIVATE KEY-----\n"
        "MIICXQIBAAKBgQDTI0/RaV/YKWDGbKkQGYpD9I/UljBCCf3rWm09sXiif8MN5rLA\n"
        "3TjC4gZ478n6Dys5yO23h1HGVTWu+mQ8071+pwLHGQ+dyDNrGWR89VLb9yanOeRf\n"
        "efOcN19ATZgGAzheM28E/iqaYkh8F2NlCjOiZAsBG6eVvxachwVAQUIWwwIDAQAB\n"
        "AoGAdVr8Q46JenHNW50L/2niw1DNHUF5g0tgeo+hhpf9UH0pIrHnC3Iq2Y+eP1ww\n"
        "7K+/u/elwcwSNOYp159PVcvvV9LwPwH29DdH6KEWIDiyFpjbXPcMMFwgakyLnFTL\n"
        "sxxa6DYznFokT+IPkF6esoypa7VQFU1RIal5Sgphq7CGCDECQQDqyL3QjYT6ffLd\n"
        "NRiMBB13+eIxvXGy5AEQcH4pNt6kYHWONCWeZ34miNp2UliIBvBHZ1uuGoO4F/Jx\n"
        "2sWwWlSpAkEA5jeQGFx/RDzzi0qPMpSOR50d2IC4NbbresY+hgJEBbI6n5hPR1ts\n"
        "MUuO1e3L5I5rzRKNzD1um1DdSgmqaqmHiwJBANLnRpNsPRMjRqHtS0Kjg7E9mDIk\n"
        "Qll3NXmGA96T+oXgXFlEgLJ9tzV4Y/471GlFClyp/RI1oTMi19fstP7I9hkCQBjr\n"
        "bseUS5phVqN/QJzjA7uwwChNVqNJ15eEmgP7fs13C213GS3KMZ3sZdu2T9m/qN+b\n"
        "4Il5JN3fFPUMssu06h0CQQCGBdmtRLi+9ws57qTPHR/BdHGUxdBRWllc9sGVVaRw\n"
        "+EOMGXus6/BssTRjwplx7w8uUR0U3s1KYDJMHMCjW25x\n"
        "-----END RSA PRIVATE KEY-----";
      
/** 支付宝公钥，用来验证支付宝返回请求的合法性 **/
std::string aliPubKey = "-----BEGIN PUBLIC KEY-----\n"
        "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDDI6d306Q8fIfCOaTXyiUeJHkr\n"
        "IvYISRcc73s3vF1ZT7XN8RNPwJxo8pWaJMmvyTn9N4HQ632qJBVHf8sxHi/fEsra\n"
        "prwCtzvzQETrNRwVxLO5jVmRGi60j8Ue1efIlzPXV9je9mkjzOmdssymZkh2QhUr\n"
        "CmZYI/FCEa3/cNMW0QIDAQAB\n"
        "-----END PUBLIC KEY-----";

std::string strUrl="timestamp=2019-09-22 00:00:08&method=alipay.trade.create&app_id=15782&sign_type=RSA2&version=1.0&charset=utf-8&biz_content={\"out_trade_no\":\"20150320010101001\",\"total_amount\":88.88,\"subject\":\"Iphone6 16G\",\"body\":\"Iphone6 16G\",\"buyer_id\":\"2088102146225135\",\"product_code\":\"FACE_TO_FACE_PAYMENT\",\"timeout_express\":\"90m\"}";

int main(int argc, char **argv)
{
    CMyString strSign1 = rsaSign(strUrl.c_str(), pKey);
    printf("rsa2 sign: [%s]\n", strSign1.c_str());

    return 0;
}

string rsaSign(const string &content, const string &key) {

    string signed_str;
    const char *key_cstr = key.c_str();
    int key_len = strlen(key_cstr);
    BIO *p_key_bio = BIO_new_mem_buf((void *)key_cstr, key_len);
    RSA *p_rsa = PEM_read_bio_RSAPrivateKey(p_key_bio, NULL, NULL, NULL);

    if (p_rsa != NULL) {

        const char *cstr = content.c_str();
        unsigned char hash[SHA256_DIGEST_LENGTH] = {0};
        SHA256((unsigned char *)cstr, strlen(cstr), hash);
        unsigned char sign[XRSA_KEY_BITS / 8] = {0};
        unsigned int sign_len = sizeof(sign);
        int r = RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, &sign_len, p_rsa);

        //if (0 != r && sizeof(sign) == sign_len) {
        if (r != 0) {
            signed_str = base64Encode(sign, sign_len);
        }
    }

    RSA_free(p_rsa);
    BIO_free(p_key_bio);
    return signed_str;
}

bool rsaVerify(const string &content, const string &sign, const string &key) {

    bool result = false;
    const char *key_cstr = key.c_str();
    int key_len = strlen(key_cstr);
    BIO *p_key_bio = BIO_new_mem_buf((void *)key_cstr, key_len);
    RSA *p_rsa = PEM_read_bio_RSA_PUBKEY(p_key_bio, NULL, NULL, NULL);

    if (p_rsa != NULL) {
        const char *cstr = content.c_str();
        unsigned char hash[SHA256_DIGEST_LENGTH] = {0};
        SHA256((unsigned char *)cstr, strlen(cstr), hash);
        unsigned char sign_cstr[XRSA_KEY_BITS / 8] = {0};
        int len = XRSA_KEY_BITS / 8;
        base64Decode(sign, sign_cstr, len);
        unsigned int sign_len = XRSA_KEY_BITS / 8;
        int r = RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH, (unsigned char *)sign_cstr, sign_len, p_rsa);

        if (r > 0) {
            result = true;
        }
    }

    RSA_free(p_rsa);
    BIO_free(p_key_bio);
    return result;
}

string base64Encode(const unsigned char *bytes, int len) {

    BIO *bmem = NULL;
    BIO *b64 = NULL;
    BUF_MEM *bptr = NULL;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, bytes, len);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    string str = string(bptr->data, bptr->length);
    BIO_free_all(b64);
    return str;
}

bool base64Decode(const string &str, unsigned char *bytes, int &len) {

    const char *cstr = str.c_str();
    BIO *bmem = NULL;
    BIO *b64 = NULL;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bmem = BIO_new_mem_buf((void *)cstr, strlen(cstr));
    b64 = BIO_push(b64, bmem);
    len = BIO_read(b64, bytes, len);

    BIO_free_all(b64);
    return len > 0;
}
