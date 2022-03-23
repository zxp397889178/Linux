/*
 * =====================================================================================
 *
 *       Filename:  ClientHttp.h
 *
 *    Description:  Http Client
 *
 *        Version:  1.0
 *        Created:  2021年08月24日 00时21分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "Client.h"

class CClientHttp : public CClient
{
public:
    CClientHttp(mm::Reactor &reactor, IClientMgr* pClientMgr, uint32_t workerId, int64_t clientId, int32_t fd, uint32_t ip, uint16_t serverPort, bool bSsl);
    virtual ~CClientHttp();

public:
	virtual bool handleRead(void);

    virtual int32_t SendMsg(CByteStream &buff);
    virtual int32_t SendMsg(void *pData, size_t cbData);

protected:
    int32_t dealRequest(void);

private:
    void getRealCgi(CMyString &strUrl);

    // 若cgi为空，则取出缺省的cgi
    void getEmptyCgi(void);
    // 重定向到另一个服务
    bool getTransfer(CMyString &strUrl);
    // 检查是否为别名
    bool getAliase(CMyString &strUrl);
    // 最后解析URL
    void decodeUrl(CMyString &strUrl);

    int32_t decodeHttpRet(int32_t ret, CMyString &state);

    void httpResponse(uint32_t code, const CMyString &status, 
        STRING_MAP &header, const char *pData, uint32_t cbData);

private:
    http::CHttpRequest      m_httpReq;
    http::CHttpRespone      m_httpResp;

    CMyString               m_cgiName;
    int64_t                 m_cgiBegin;
    int64_t                 m_cgiEnd;

    int32_t                 m_respBufferSize;
    int32_t                 m_expireTimeout;
};

