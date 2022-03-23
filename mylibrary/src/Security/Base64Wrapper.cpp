/*
 * =====================================================================================
 *
 *       Filename:  Base64Wrapper.cpp
 *
 *    Description:  base64 implement on openssl
 *
 *        Version:  1.0
 *        Created:  2019年08月29日 00时00分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Base64Wrapper.h"
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <string>
#include <iostream>

CMyString CBase64Wrapper::Encode(const char *buffer, int32_t len, bool newLine)
{
    BIO *bmem = NULL;
    BIO *b64 = NULL;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    if (!newLine) {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, buffer, len);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);
    BIO_set_close(b64, BIO_NOCLOSE);

    CTempBuffer tmpbuf(bptr->length+1);
    memcpy(tmpbuf.data(), bptr->data, bptr->length);
    BIO_free_all(b64);

    return CMyString(tmpbuf.data());
}

CMyString CBase64Wrapper::Decode(const char *input, int32_t len, bool newLine)
{
    uint8_t *outbuf(NULL);
    size_t outlen(0);
    Decode(input, len, newLine, outbuf, outlen);
    CMyString str;
    str.assign((const char*)outbuf, outlen);
    SAFE_DELETE(outbuf);
    return str;
}

void CBase64Wrapper::Decode(const char *buffer, size_t buflen, bool newLine, 
            uint8_t *&outbuf, size_t &outlen)
{
    BIO *b64 = BIO_new(BIO_f_base64());
    if (!newLine) 
    {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    BIO *bmem = BIO_new_mem_buf(const_cast<char *>(buffer), buflen);
    bmem = BIO_push(b64, bmem);
    outlen = buflen;
    outbuf = new uint8_t[outlen+1];
    memset(outbuf, 0, outlen+1);
    outlen = BIO_read(bmem, outbuf, outlen);
    BIO_free_all(bmem);
}
