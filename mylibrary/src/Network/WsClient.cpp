/*
 * =====================================================================================
 *
 *       Filename:  WsClient.cpp
 *
 *    Description:  websocket client implement
 *
 *        Version:  1.0
 *        Created:  12/13/2019 10:15:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include <string.h>
#include "WsClient.h"
#include "http/HttpRespone.h"
#include "Security.h"
#include "NetFunc.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "WsWrapper.h"

///////////////////////////////////////////////////////////////////////////////////
//
CWsClient::CWsClient()
    : m_fd(0)
    , m_bUseMask(false)
    , m_strUrl()
{

}

CWsClient::~CWsClient()
{
    this->close();
}

int32_t CWsClient::Open(const CMyString &url, bool bUseMask, const CMyString &origin, int32_t timeout)
{
    char host[256] = "";
    int32_t port(0);
    char cgi[256] = "";

    m_eState = CONNECTING;

    // 解析出host, port, cgi
    if (false) { }
    else if (sscanf(url.c_str(), "ws://%[^:/]:%d/%s", host, &port, cgi) == 3) {
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]/%s", host, cgi) == 2) {
        port = 80;
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]:%d", host, &port) == 2) {
        cgi[0] = '\0';
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]", host) == 1) {
        port = 80;
        cgi[0] = '\0';
    }
    else {
        LogFatal("ERROR: Could not parse WebSocket url: %s", url.c_str());
        return -1;
    }
    LogDebug("WSDBG>connect to host [%s] port [%d], cgi [%s]", host, port, cgi);

    // 连接到远端,阻塞模式
    m_fd = create_tcp_socket(true, true, false, -1, -1);
    if (-1 == m_fd)
    {
        m_eState = CLOSED;
        LogFatal("Unable to create socket.");
        return -2;
    }
    bool ret = open_tcp_connect(m_fd, host, port);
    if (!ret)
    {
        m_eState = CLOSED;
        LogFatal("Unable to connect %s:%d", host, port);
        return -3;
    }

    // 发送第一个包文到服务端
    CMyString strData;
    build_first_pkg(strData, host, port, cgi, origin);
    LogDebug("send req [%s]", strData.c_str());
    set_sck_nonblock(m_fd);
    ssize_t result = ::send(m_fd, strData.c_str(), strData.length(), 0);
    if (-1 == result)
    {
        int32_t err = errno;
        LogFatal("send first pkg to %s:%d failure, errno %d, error: %s",
                host, port, err, strerror(errno));
        this->close();
        return -4;
    }

    // 接收第一个回应消息
    // NOTE: 2021-4-22 要设置一个超时
    http::CHttpRespone httpResp;
    CMyString strResp;
    bool bReadOK(false);
    uint64_t begin = Now();
    uint64_t expire = begin + timeout;
    do {
        char buffer[512] = "";
        result = ::recv(m_fd, buffer, sizeof(buffer), 0);
        if (result == 0)
        {
            // remote shutdown
            LogFatal("remote host %s:%d has shutdown connection.", host, port);
            this->close();
            return -9;
        }
        else if (result < 0)
        {
            int32_t err = errno;
            if (err == EWOULDBLOCK || err == EAGAIN)
            {
#if 0
                // 无数据
                LogWarn("WSDBG>recv data from host %s:%d errno %d, error %s.", 
                        host, port, err, strerror(errno));
                break;
#else
                // 改成了非阻塞模式，循环下一次
#endif
            }
            else
            {
                LogFatal("recv first pkg from %s:%d failure, errno %d, error: %s",
                    host, port, err, strerror(errno));
                this->close();
                return -11;
            }
        }
        else
        {
            // 获得数据
            strResp.Append(buffer);
            int32_t state = httpResp.read(buffer, result);
            if (state == http::CHttpData::stateOK)
            {
                // 收完毕
                LogDebug("recv data succeed, %ld bytes", result);
                bReadOK = true;
                break;
            }
        }
        nsleep(1);
    }while(Now() < expire);
    // LogDebug("=>begin %lu, now:%lu, delta:%lu", begin, Now(), Now()-begin);
    if (!bReadOK)
    {
        LogFatal("wait host %s:%u respone timeout, timeout is %dms", host, port, (int32_t)(Now()-begin));
        return -13;
    }

    // 对数据进行分析处理
    LogDebug("WSDBG>recv resp [%s]", strResp.c_str());
    int32_t status = httpResp.getStatusCode();
    if (status != 101)
    {
        LogFatal("Got bad status [%d] when connect to %s:%d", status, host, port);
        this->close();
        return -21;
    }

    set_sck_nonblock(m_fd);
    LogDebug("WSDBG>connect %s:%d ok.", host, port);
    // 连接OK
    m_bUseMask = bUseMask;
    m_strHost = host;
    m_strCgi = cgi;
    m_nPort = port;
    m_eState = OPEN;

    return 0;
}

void CWsClient::build_first_pkg(CMyString &str, const char* host, int32_t port, const char *cgi, const CMyString &origin)
{
    str.Append("GET /%s HTTP/1.1\r\n", cgi);
    if (port==80)
    {
        str.Append("Host: %s\r\n", host);
    }
    else
    {
        str.Append("Host: %s:%d\r\n", host, port);
    }
    str.Append("Upgrade: websocket\r\n");
    str.Append("Connection: Upgrade\r\n");
    if (!origin.empty())
    {
        str.Append("Origin: %s\r\n", origin.c_str());
    }

    // 随机生成key
    uint8_t buf[17] = {0, };
    for(int i=0; i<16; ++i)
    {
        buf[i] = (uint8_t)(RandGet(true) % 0xFF);
    }
    CMyString strKey = CBase64Wrapper::Encode((char*)&buf[0], 16, false);
    str.Append("Sec-WebSocket-Key: %s\r\n", strKey.c_str());
    //str.Append("Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n");
    str.Append("Sec-WebSocket-Version: 13\r\n");
    str.Append("\r\n");
}

int32_t CWsClient::Ping(void)
{
    return this->sendData(CWsWrapper::PING_FRAME, NULL, 0);
}

int32_t CWsClient::Send(const CMyString &str)
{
    size_t cbLen = str.length();
    CTempBuffer tmpbuf(cbLen+1);
    memcpy(tmpbuf.data(), str.c_str(), cbLen);
    return this->sendData(CWsWrapper::TEXT_FRAME, (void*)tmpbuf.data(), cbLen); 
}

int32_t CWsClient::Send(void *pData, size_t cbData)
{
    return this->sendData(CWsWrapper::BINARY_FRAME, pData, cbData);
}

int32_t CWsClient::sendData(enum CWsWrapper::opcode_type opType, void *buffer, size_t buflen)
{
    if (m_eState == CLOSING || m_eState == CLOSED) 
    {
        return -1;
    }

    CByteStream buff;
    CWsWrapper wsobj(m_fd, m_bUseMask);
    wsobj.Encode(opType, buffer, buflen, buff);

    size_t wsbuf_size = buff.size();
    byte_t *wsbuf = new byte_t[wsbuf_size+1];
    CAutoArray guard(wsbuf);
    buff.Read(wsbuf, wsbuf_size);

    // 发送到服务端
    uint32_t totalSend(0);
    int32_t ret = send_data_ex(m_fd, wsbuf, wsbuf_size, totalSend, 1000);
    if (ret<0)
    {
        int32_t err = errno;
        LogFatal("send data to %s:%d failure, errno: %d, error: %s",
                m_strHost.c_str(), m_nPort, err, strerror(errno));
        // 对应端出错，关闭
        this->close();

        return -1;
    }
    LogDebug("WSDBG>total send %u bytes.", totalSend);
    return 0;
}

int32_t CWsClient::RecvPackage(void* &pData, size_t &cbData, enum HeaderType hdrType)
{
    if (!this->isActive())
    {
        return -1;
    }

    int32_t ret = this->process_recv(hdrType);
    if (ret < 0)
    {
        return ret;
    }
    if (!this->getPackage(hdrType, pData, cbData))
    {
        return 0;
    }
    return 1;
}

int32_t CWsClient::process_recv(enum HeaderType hdrType)
{
    byte_t buffer[1024] = {0, };

    ssize_t ret = ::recv(m_fd, (void*)&buffer[0], sizeof(buffer), 0);
    if (ret == 0)
    {
        // the peer has performed an orderly shutdown.
        this->close();
        return -1;
    }
    else if (ret < 0)
    {
        //  an error occurred 
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // The  socket  is  marked  non-blocking  and  
            // the receive operation would block, 
            // or a receive timeout had been set and 
            // the timeout expired before data was received. 
            return 0;
        }
        else
        {
            this->close();
            return -2;

        }
    }
    else
    {
        //LogDebug("recv websocket data %ld bytes", ret);
        m_buffer.Write((void*)&buffer[0], ret);
    }

    /* websocket 的消息序列为
     * 
     * cmdId (uint16_t) + 
     * base_hdr(2bytes) + extra_hdr(0..14bytes) + 
     * pData (dataLen)
     *
     * */
    if (m_buffer.size() < sizeof(uint16_t) )
    {
        // 不足包头长度
        return 0;
    }

    //解析 m_buffer 中的数据，把整包发到另一个buffer中
    CWsWrapper wsobj(m_fd, m_bUseMask);
    int32_t pkgCount = wsobj.Decode(m_buffer, m_package, hdrType);
    if (pkgCount < 0)
    {
        this->close();
    }
    return pkgCount;
}

void DumpByteStream(CByteStream &buff)
{
    size_t cbData = buff.size();
    void *pData = calloc(cbData+1, 1);
    buff.Read(pData, cbData);
    CMyString strBin;
    dumpbin(pData, cbData, strBin);
    LogErr("package length:%u, dump: %s", cbData, strBin.c_str());
    free(pData);
}

bool CWsClient::getPackage(enum HeaderType hdrType, void *&pData, size_t &cbData)
{
    if (m_package.empty())
    {
        return false;
    }

    if (hdrType == hdr_MsgHead)
    {
        uint32_t protoLen(0);
        m_package.SetPeekMode(true);
        m_package.Read(protoLen);
        m_package.SetPeekMode(false);
        protoLen = CByteOrder::Cast(protoLen);
        if (protoLen>0)
        {
            cbData = protoLen;
            pData = calloc(cbData+1, 1);
            m_package.Read(pData, cbData);
            return true;
        }
        return false;
    }
    else if (hdrType==hdr_MsgHeadEx)
    {
        uint16_t protoLen(0);
        m_package.SetPeekMode(true);
        m_package.Read(protoLen);
        m_package.SetPeekMode(false);
        protoLen = CByteOrder::Cast(protoLen);
        if (protoLen>0)
        {
            cbData = protoLen;
            pData = calloc(cbData+1, 1);
            m_package.Read(pData, cbData);
            return true;
        }
        return false;
    }

    uint16_t pkgLen(0);
    m_package.Read(pkgLen);
    if (0==pkgLen)
    {
        DumpByteStream(m_package);
        cbData = 0;
        return false;
    }

    if (hdrType == hdr_ProtobufEx)
    {
        pkgLen += sizeof(uint32_t);
    }
    cbData = pkgLen;
    pData = calloc(cbData+1, 1);
    // pData = new byte_t[cbData+1];
    m_package.Read(pData, cbData);

    return true;
}

void CWsClient::sendClose(void)
{
    if (m_eState == CLOSING || m_eState == CLOSED)
    {
        return;
    }

    m_eState = CLOSING;
    uint8_t closeFrame[6] = {0x88, 0x80, 0x00, 0x00, 0x00, 0x00 };
    // last 4 bytes are a masking key
    
    uint32_t totalSend(0);
    send_data_ex(m_fd, (void*)&closeFrame[0], sizeof(closeFrame), totalSend, 1000);
    this->close();
}

void CWsClient::close(void)
{
    close_tcp_socket(m_fd, false);
    m_fd = -1;
    m_buffer.clear();
    m_package.clear();
    m_eState = CLOSED;
}

bool CWsClient::isActive(void)
{
    return ((m_fd != -1) && (m_eState == OPEN));
}


