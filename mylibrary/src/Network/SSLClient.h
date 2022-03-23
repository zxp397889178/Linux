/*
 * =====================================================================================
 *
 *       Filename:  SSLClient.h
 *
 *    Description:  SSL前端
 *
 *        Version:  1.0
 *        Created:  2020年02月23日 22时30分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include "BaseCode.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

namespace net {

class SSLGuard
{
public:
    SSLGuard(SSL_CTX* ctx, SSL *ssl)
        : _ctx(ctx), _ssl(ssl)
    {
    }
    explicit SSLGuard(SSL_CTX *ctx)
        : _ctx(ctx), _ssl(NULL)
    {
    }
    void setSSL(SSL *ssl)
    {
        _ssl = ssl;
    }
    void giveup(void)
    {
        _ctx = NULL;
        _ssl = NULL;
    }
    ~SSLGuard()
    {
	    if (NULL != _ssl)
	    {
		    SSL_shutdown(_ssl);
		    SSL_free(_ssl);
	    }
	    if (NULL != _ctx)
	    {
		    SSL_CTX_free(_ctx);
	    }
    }
private:
    SSL_CTX*    _ctx;
    SSL*        _ssl;
};

class CSSLClient
{
public:
    CSSLClient();
    ~CSSLClient();

    /** 初始化并绑定socket
     */
    int32_t Init(int32_t fd);

    /* 发送数据
     * 返回值： 0  发送数据失败，但SSL无错
     *          -1 发送数据失败，SSL出错
     *          >0 发送的字节数
     */
    int32_t SendMsg(void* pData, size_t cbData);

    /* 接收SSL数据
     * 返回值： 0  接收数据失败，但SSL无错
     *          -1 接收数据失败，且SSL出错
     *          >0 接收数据的字节数
     */
    int32_t RecvMsg(void* pData, size_t &cbData);

    void Close(void);

    bool IsActive(void) { return m_bActive; }

protected:
    void getLastError(int32_t &err, CMyString &errNote);
    void getLastError(SSL *ssl, int32_t ret, int32_t &err, CMyString &errNote);

private:
    SSL_CTX*        m_ctx;
    SSL*            m_ssl;

    bool            m_bActive;
};


}

