/*
 * =====================================================================================
 *
 *       Filename:  md5.cpp
 *
 *    Description:  md5 implement base OpenSSL
 *
 *        Version:  1.0
 *        Created:  2019年08月29日 06时58分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "MD5Wrapper.h"
#include <openssl/md5.h>
#include <iostream>
#include <fstream>

#define MAXDATABUFF 1024
#define MD5LENGTH 16

int CMD5Wrapper::encode(const char *text, uint8_t *md5)
{
    if (text == NULL || text[0]=='\0')
    {
        return -1;
    }

    MD5_CTX ctx;
    //md5 = new uint8_t[MD5LENGTH+1];
    memset(md5, 0, MD5LENGTH);
    MD5_Init(&ctx);
    MD5_Update(&ctx, text, strlen(text));
    MD5_Final(md5, &ctx);

    return 0;
}

int CMD5Wrapper::encode_file(const char *filename, uint8_t *md5)
{
    if (filename==NULL || filename[0]=='\0')
    {
        return -1;
    }

    std::ifstream ifile(filename, std::ios_base::in|std::ios_base::binary);
    if (ifile.fail())
    {
        return -2;
    }

    MD5_CTX ctx;
    //md5 = new uint8_t[MD5LENGTH+1];
    memset(md5, 0, MD5LENGTH);
    MD5_Init(&ctx);

    char buff[MAXDATABUFF+1] = "";
    while(!ifile.eof())
    {
        ifile.read(buff, MAXDATABUFF);
        int32_t len = ifile.gcount();
        if (len>0)
        {
            MD5_Update(&ctx, buff, len);
        }
    }
    MD5_Final(md5, &ctx);

    return 0;
}

CMyString CMD5Wrapper::encode(const char *text)
{
    uint8_t md5[MD5LENGTH+1];
    int32_t ret = CMD5Wrapper::encode(text, md5);
    if (ret < 0)
    {
        return CMyString();
    }
    char buffer[64] = "";
    for(int32_t i=0; i<MD5LENGTH; ++i)
    {
        sprintf(&(buffer[i*2]), "%02X", md5[i]);
    }
    CMyString val(buffer);
    return val;
}

CMyString CMD5Wrapper::encode_file(const char *filename)
{
    uint8_t md5[MD5LENGTH+1];
    int32_t ret = CMD5Wrapper::encode_file(filename, md5);
    if (ret < 0)
    {
        return CMyString();
    }
    char buffer[64] = "";
    for(int32_t i=0; i<MD5LENGTH; ++i)
    {
        sprintf(&(buffer[i*2]), "%02X", md5[i]);
    }
    CMyString val(buffer);
    return val;
}


