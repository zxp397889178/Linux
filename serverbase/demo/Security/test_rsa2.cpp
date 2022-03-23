/*
 * =====================================================================================
 *
 *       Filename:  test_rsa2.cpp
 *
 *    Description:  test rsaSign / rsaVerify
 *
 *        Version:  1.0
 *        Created:  09/21/2019 11:41:18 PM
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

using namespace std;
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
    CMyString strSign1;
    int32_t ret = CRsaWrapper::rsaSign((uint8_t*)strUrl.c_str(), strUrl.length(),
        pKey.c_str(), strSign1);
    printf("rsa2 sign: [%s], ret=%d\n", strSign1.c_str(), ret);

    return 0;
}


