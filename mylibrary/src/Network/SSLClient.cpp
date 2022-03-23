/*
 * =====================================================================================
 *
 *       Filename:  SSLClient.cpp
 *
 *    Description:  SSL 客户端
 *
 *        Version:  1.0
 *        Created:  2020年02月23日 22时31分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "SSLClient.h"

namespace net {

CSSLClient::CSSLClient()
    : m_ctx(NULL)
    , m_ssl(NULL)
    , m_bActive(false)
{

}

CSSLClient::~CSSLClient()
{
    this->Close();
}

void CSSLClient::getLastError(int32_t &err, CMyString &errNote)
{
    do{
        err = ERR_get_error();
        if (err == 0)
        {
            break;
        }
        char *str = ERR_error_string(err, 0);
        if (!str)
        {
            break;
        }
        errNote.append(str);
    }while(true);
}

void CSSLClient::getLastError(SSL *ssl, int32_t ret, int32_t &err, CMyString &errNote)
{
    err = SSL_get_error(ssl, ret);
    char *str = ERR_error_string(err, 0);
    if (!str)
    {
        errNote = str;
    }
}


int32_t CSSLClient::Init(int32_t fd)
{
    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD *method = TLSv1_2_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (NULL == ctx)
    {
        int32_t err;
        CMyString errNote;
        this->getLastError(err, errNote);
        LogErr("[%s:%s:%d]create ssl context failure, errno:%d, error:%s",
                __FILE__, __func__, __LINE__, err, errNote.c_str());
        return -1;
    }
    SSLGuard guard(ctx);

    SSL* ssl = SSL_new(ctx);
    if (NULL == ssl)
    {
        int32_t err;
        CMyString errNote;
        this->getLastError(err, errNote);
        LogErr("[%s:%s:%d]create ssl failure, errno:%d, error:%s",
                __FILE__, __func__, __LINE__, err, errNote.c_str());
        return -3;
    }
    guard.setSSL(ssl);

    int32_t sock = SSL_get_fd(ssl);
    LogDebug("ssl fd is [%d]", sock);
    SSL_set_fd(ssl, fd);
    int32_t ret = SSL_connect(ssl);
    if (ret <=0)
    {
        int32_t err;
        CMyString errNote;
        this->getLastError(ssl, ret, err, errNote);
        LogErr("[%s:%s:%d]create ssl connection, failure, errno:%d, error:%s",
                __FILE__, __func__, __LINE__, err, errNote.c_str());
        return -5;
    }

    guard.giveup();
    m_ctx = ctx;
    m_ssl = ssl;
    m_bActive = true;
    return 0;
}

int32_t CSSLClient::SendMsg(void* pData, size_t cbData)
{
    if (!m_bActive)
    {
        return -1;
    }

    int32_t result(1);
    int32_t ret = SSL_write(m_ssl, pData, cbData);
    if (ret < 0)
    {
        int32_t err(0);
        CMyString errNote;
        getLastError(m_ssl, ret, err, errNote);
        switch(err)
        {
        case SSL_ERROR_WANT_WRITE:
            result = 0;
            break;
        case SSL_ERROR_WANT_READ:
            result = 0;
            break;
        case SSL_ERROR_ZERO_RETURN:
        case SSL_ERROR_SYSCALL:
        case SSL_ERROR_SSL:
            LogErr("[%s:%s:%d]write ssl data failure, errno:%d, error:%s", 
                    __FILE__, __func__, __LINE__, err, errNote.c_str());
            result = -1;
            break;
        default:
            LogErr("[%s:%s:%d]write ssl data failure, errno:%d", 
                    __FILE__, __func__, __LINE__, err);
            result = -1;
            break;
        }
    }
    else
    {
        result = ret;
    }
    return result;    
}

int32_t CSSLClient::RecvMsg(void* pData, size_t &cbData)
{
    if (!m_bActive)
    {
        return -1;
    }

    int32_t result(1);
    int32_t ret = SSL_read(m_ssl, pData, cbData);
    if (ret < 0)
    {
        int32_t err(0);
        CMyString errNote;
        getLastError(m_ssl, ret, err, errNote);
        switch(err)
        {
        case SSL_ERROR_WANT_READ:
            result = 0;
            break;
        case SSL_ERROR_WANT_WRITE:
            result = 0;
            break;
        case SSL_ERROR_ZERO_RETURN:
        case SSL_ERROR_SYSCALL:
        case SSL_ERROR_SSL:
            LogErr("[%s:%s:%d]read ssl data failure, errno:%d, error:%s", 
                    __FILE__, __func__, __LINE__, err, errNote.c_str());
            result = -1;
            break;
        default:
            LogErr("[%s:%s:%d]read ssl data failure, errno:%d",
                    __FILE__, __func__, __LINE__, err);
            result = -1;
            break;
        }
    }
    else
    {
        cbData = ret;
        result = ret;
    }
    return result;
}

void CSSLClient::Close(void)
{
    m_bActive = false;

    if (NULL != m_ssl)
    {
	    SSL_shutdown(m_ssl);
	    SSL_free(m_ssl);
        m_ssl = NULL;
    }

    if (NULL != m_ctx)
    {
	    SSL_CTX_free(m_ctx);
        m_ctx = NULL;
    }

}

}

