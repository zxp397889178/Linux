/*
 * =====================================================================================
 *
 *       Filename:  test_aes_cbc.cpp
 *
 *    Description:  AES CBC
 *
 *        Version:  1.0
 *        Created:  12/11/2019 08:25:49 PM
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

void Dump(const CMyString &buf)
{
    uint8_t c;
    for(size_t i=0; i<buf.length(); ++i)
    {
        c = buf[i];
        printf("%02x ", c);
        if ((i+1)%32==0) printf("\n");
    }
    printf("\n\n");
}

void Dump(uint8_t *buf, size_t buflen)
{
    for(size_t i=0; i<buflen; ++i)
    {
        printf("%02x ", buf[i]);
        if ((i+1)%32==0) printf("\n");
    }
    printf("\n\n");
}

int32_t main(int argc, char **argv)
{
#if 0
    CMyString strData="s=6&startTime=1573183290659&endTime=1573183590659";
    CMyString strKey = "1f2d7e7d1dee82b3";
    CMyString strIV = "c961c1ee2792c864";
#else
    /*
    CMyString strData="{\"jackpotWin\":0.0,\"jackpotContribute\":-0.003,\"hasFreeGame\":0,\"hasGamble\":0,\"action\":8,\"transferId\":438139,\"gameSeqNo\":5250151628916,\"uid\":\"10211798626\",\"gType\":0,\"mType\":8047,\"reportDate\":\"18-12-2019\",\"gameDate\":\"18-12-2019 09:07:41\",\"bet\":-0.6,\"win\":0.0,\"netWin\":-0.6,\"currency\":\"RB\",\"denom\":0.01,\"lastModifyTime\":\"18-12-2019 09:07:41\",\"ipAddress\":\"120.36.188.16\",\"clientType\":\"MOBILE\",\"systemTakeWin\":0,\"ts\":1576674461580}";

    //CMyString strData="{\"uid\":\"10214519824\",\"currency\":\"RB\",\"ts\":1576671172263,\"act0ion\":6}";
    CMyString strKey = "4dce75d5af647183";
    CMyString strIV = "990fdfd744d25933";
    */
    CMyString strData = "uuid=13601780&nick=游客13601780&headUrl=http://119.3.59.200:18711/pic/head_15.png&sex=1&channel=fhylcl_hall_shanghai&agentId=0&ip=125.77.91.4&sessionKey=13601780-niuniuone-app-1584512959147606103&ext=diamond+70000";
    CMyString strKey = "ZXhhbXBsZS1hcHAtc4VjcmV0";
    CMyString strIV = "ZXhhbXBsZS1hcHAt";
#endif

    printf("Origin: [%s]\n", strData.c_str());
    uint8_t *cipher(NULL);
    size_t cipher_len(0);
    CAesWrapper::EncodeCBC((const uint8_t*)strData.c_str(), strData.length(), strKey, strIV, cipher, cipher_len, padPKCS5);
    printf("AES_CBC:Encrypt\n");
    Dump(cipher, cipher_len);
    CMyString b64e = CBase64Wrapper::Encode((const char*)cipher, cipher_len, false);
    printf("base64(aes_cbc): %s\n", b64e.c_str());

    uint8_t *b64d(NULL);
    size_t b64dlen(0);
    CBase64Wrapper::Decode(b64e.c_str(), b64e.length(), false, b64d, b64dlen);
    printf("base64: decrypt length:[%u]\n", b64dlen);
    Dump(b64d, b64dlen);

    CMyString b64d2 = CBase64Wrapper::Decode(b64e.c_str(), b64e.length(), false);
    printf("base64decode: length %d\n", b64d2.length());
    Dump(b64d2);

    uint8_t *clear(NULL);
    size_t clear_len(0);
    //CMyString b64d = CBase64Wrapper::Decode(b64e.c_str(), b64e.length(), false);
    //printf("base64decode: length %d\n", b64d.length());
    //Dump(b64d);
    CAesWrapper::DecodeCBC((const uint8_t*)b64d2.c_str(), b64d2.length(),strKey, strIV, clear, clear_len, padPKCS5);
    printf("AES_CBC:Decrypt:[%s]\n", (const char*)clear);
    //CAesWrapper::DecodeCBC(cipher, cipher_len,strKey, strIV, clear, clear_len, padZERO);

    printf("\n\n");
    printf("encrypt>base64(aes(%s))=[%s]\n", strData.c_str(), b64e.c_str());
    printf("decrypt>base64(aes(%s))=[%s]\n", (const char*)b64e.c_str(), (const char*)clear);

    CAutoBuffer gurad1((char*)cipher);
    CAutoBuffer guard2((char*)clear);
    CAutoBuffer guard3((char*)b64d);
    return 0;
}

