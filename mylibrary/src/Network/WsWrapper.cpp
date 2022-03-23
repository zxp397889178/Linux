/*
 * =====================================================================================
 *
 *       Filename:  WsWrapper.cpp
 *
 *    Description:  Websocket wrapper
 *
 *        Version:  1.0
 *        Created:  12/15/2019 01:26:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "WsWrapper.h"
#include "Security.h"
#include "NetFunc.h"

const uint8_t masking_key[4] = { 0x12, 0x34, 0x56, 0x78 };

CWsWrapper::CWsWrapper(int32_t fd, bool bUseMask)
{
    m_fd = fd;
    m_bUseMask = bUseMask;

    m_ipWay = 0;
}

CWsWrapper::~CWsWrapper()
{

}

void CWsWrapper::httpRespone(http::CHttpRespone &resp, 
        uint32_t code, const CMyString &status, 
        const CMyString &data,
        CByteStream &outbuff)
{
    resp.setStatusCode(code);
    resp.setStatus(status.c_str());
	resp.setHeader("Data", getGMTDate(time(NULL))) ;
	resp.setHeader("Content-Type", "text/html");
	resp.setHeader("Server", "Webframe/1.0.11(Linux)");

	CByteStream &buff = resp.getBuff();
	if (data.empty())
	{
		resp.setContentLength( buff.size() );
	}
	else
	{
		resp.setContentLength(data.length());
		buff.clear();
		buff.Write((void*)data.c_str(), data.size());
	}

	resp.pack(outbuff, true);
}


int32_t CWsWrapper::Ack(CByteStream &inbuff, CByteStream &outbuff)
{
    size_t tmpbuf_len(4096);
    CTempBuffer tmpbuf(tmpbuf_len+1);
    size_t len = inbuff.size();
    if (len > tmpbuf_len)
    {
        LogFatal("websocket ack: msg length %u overflow", len);
        return -1;
    }

    CByteStreamGuard guard(inbuff);
    inbuff.Read(tmpbuf.data(), len);
    LogTrace("websocket data:[%s]", tmpbuf.data());

    http::CHttpRequest req;
    http::CHttpRespone resp;
    int32_t ret = req.read(tmpbuf.data(), len);
    resp.setVersion(req.getVersion());
    if (ret != http::CHttpData::stateOK)
    {
        httpRespone(resp, 404, "FAIL", "NOT FOUND", outbuff); 
        LogFatal("websocket request invalid.[%s]", tmpbuf.data());
        return -2;
    }

    CMyString strUpgrade = req.getHeader("Upgrade");
    CMyString strSecKey = req.getHeader("Sec-WebSocket-Key");
    strUpgrade.MakeLower();
    if (strSecKey.empty() || strUpgrade != "websocket")
    {
        LogFatal("websocket ack FAIL, invalid header value: Upgrade=>[%s], SecWebSocketKey=>[%s]",
                strUpgrade.c_str(), strSecKey.c_str());
        httpRespone(resp, 404, "FAIL", "NOT FOUND", outbuff); 
        return -3;
    }

    // 此处解析nginx转发的头，如真实IP，以及转发序列串等
    // https://www.cnblogs.com/heioray/p/9530650.html
    if (m_ipWay == 1)
    {
        m_strRealIp = req.getHeader(m_ipParam);
    }
    else if (m_ipWay == 2)
    {
        m_strRealIp = req.getParam(m_ipParam);
    }
    // TODO：在cookie中设置IP，暂时忽略
    
    LogDebug("websocket full header [%s]", req.getFullHeader().c_str());
    //LogDebug("query ip way [%d], real ip [%s]", m_ipWay, m_strRealIp.c_str());
    DecodeRealIp(m_strRealIp);
    //LogDebug("client realip [%s]", m_strRealIp.c_str());

    CMyString strAccept = ShaWrapper::Sha1WithBase64(strSecKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    LogDebug("websocket ack OK, Sec-Websocket-Accept:%s", strAccept.c_str());

	resp.setHeader("Access-Control-Allow-Credentials", "true");
	resp.setHeader("Access-Control-Allow-Headers", "content-type");
	resp.setHeader("Sec-WebSocket-Accept", strAccept.c_str());
	resp.setHeader("Upgrade", "WebSocket");
	resp.setHeader("Connection", "Upgrade");

    httpRespone(resp, 101, "Switching Protocol", "", outbuff);
    guard.skip(len);
    return 0;
}

void CWsWrapper::add_header(enum opcode_type opType, uint64_t data_size, CByteStream &buff)
{
    // base header
    CWsWrapper::wsheader base_hdr;
    memset(&base_hdr, 0, sizeof(base_hdr));
    base_hdr.fin = 1;
    base_hdr.opcode = opType;
    base_hdr.mask = (m_bUseMask ? 1 : 0);

    // extra header
    int32_t extra_hdrLen = (data_size>=126 ? 2: 0) + (data_size>=65536 ? 6 : 0) + (m_bUseMask ? 4 : 0);
    uint8_t extra_hdr[32] = {0, };

    int32_t mask_idx(0);
    if (data_size <= 0) {  }
    else if (data_size < 126)
    {
        base_hdr.pkgLen = data_size;
        mask_idx = 0;
    }
    else if (data_size < 65536)
    {
        base_hdr.pkgLen = 126;
        extra_hdr[0] = (data_size >> 8) & 0xFF;
        extra_hdr[1] = (data_size >> 0) & 0xFF;
        mask_idx = 2;
    }
    else 
    {
        base_hdr.pkgLen = 127;
        extra_hdr[0] = (data_size >> 56) & 0xFF;
        extra_hdr[1] = (data_size >> 48) & 0xFF;
        extra_hdr[2] = (data_size >> 40) & 0xFF;
        extra_hdr[3] = (data_size >> 32) & 0xFF;
        extra_hdr[4] = (data_size >> 24) & 0xFF;
        extra_hdr[5] = (data_size >> 16) & 0xFF;
        extra_hdr[6] = (data_size >> 8) & 0xFF;
        extra_hdr[7] = (data_size >> 0) & 0xFF;
        mask_idx = 8;
    }
    if (m_bUseMask)
    {
        extra_hdr[mask_idx+0] = masking_key[0];
        extra_hdr[mask_idx+1] = masking_key[1];
        extra_hdr[mask_idx+2] = masking_key[2];
        extra_hdr[mask_idx+3] = masking_key[3];
    }

    buff.Write((void*)&base_hdr, sizeof(base_hdr));
    if (extra_hdrLen > 0)
    {
        buff.Write((void*)&extra_hdr[0], extra_hdrLen);
    }
}

void CWsWrapper::Encode(enum opcode_type opType, CByteStream &inbuff, CByteStream &outbuff)
{
    if (m_fd <= 0)
    {
        return;
    }

    size_t cbData = inbuff.size();
    uint64_t data_size = cbData;
    add_header(opType, data_size, outbuff);

    byte_t *pData = new byte_t[cbData];
    CAutoArray guard(pData);
    inbuff.Read(pData, cbData);
    if (m_bUseMask && cbData>0)
    {
        uint8_t *pBuf = (uint8_t*)pData;
        for(size_t i=0; i<cbData; ++i)
        {
            pBuf[cbData-i-1] ^= masking_key[i&0x03];
        }
    }

    if (cbData > 0)
    {
        outbuff.Write(pData, cbData);
    }
}

void CWsWrapper::Encode(enum opcode_type opType, void *pData, size_t cbData, CByteStream &outbuff)
{
    if (m_fd <= 0)
    {
        return;
    }

    uint64_t data_size = cbData;
    add_header(opType, data_size, outbuff);

    if (m_bUseMask && data_size>0)
    {
        uint8_t *pBuf = (uint8_t*)pData;
        for(size_t i=0; i<cbData; ++i)
        {
            pBuf[cbData-i-1] ^= masking_key[i&0x03];
        }
    }

    if (data_size > 0)
    {
        outbuff.Write(pData, cbData);
    }
}

int32_t CWsWrapper::Decode(CByteStream &inbuff, CByteStream &outbuff, enum HeaderType hdrType)
{
    if (m_fd <= 0)
    {
        return -1;
    }

    // headerType = [1,4]
    if (hdrType == hdr_Dummy || hdrType == hdr_Plain)
    {
        // 消息头不对
        return -2;
    }

    int32_t pkgCount(0);
    bool bFinished(false);
    uint32_t cmdId(0);

    do {
        int32_t pkgLen(0);
        CByteStreamGuard guard2(inbuff);

        // 读取头数据
        wsheader base_hdr;
        bool ret = inbuff.Read((void*)&base_hdr, sizeof(base_hdr));
        if (!ret)
        {
            break;
        }
        pkgLen += sizeof(wsheader);
        LogDebug("base hdr: pkgLen %d, mask %s, opcode:%d, fin:%d", 
                base_hdr.pkgLen, (base_hdr.mask ? "true" : "false"),
                base_hdr.opcode, base_hdr.fin);

        // 读取头部扩展数据
        int32_t extra_hdrLen = (base_hdr.pkgLen==126 ? 2 : 0) +
            (base_hdr.pkgLen==127 ? 8 : 0) +
            (base_hdr.mask ? 4 : 0);
        uint8_t extra_hdr[32] = {0};
        if (extra_hdrLen > 0)
        {
            ret = inbuff.Read((void*)&extra_hdr, extra_hdrLen);
            if (!ret)
            {
                break;
            }
            pkgLen += extra_hdrLen;
            LogDebug("exten hdr: pkgLen %d ", extra_hdrLen);
        }


        uint64_t dataLen(0);
        int32_t mask_idx(0);
        if(base_hdr.pkgLen < 126)
        {
            dataLen = base_hdr.pkgLen;
            mask_idx = 0;
        }
        else if (base_hdr.pkgLen == 126)
        {
            dataLen |= ((uint64_t)extra_hdr[0]) << 8;
            dataLen |= ((uint64_t)extra_hdr[1]) << 0;
            mask_idx = 2;
        }
        else if (base_hdr.pkgLen == 127)
        {
            dataLen |= ((uint64_t) extra_hdr[0]) << 56;
            dataLen |= ((uint64_t) extra_hdr[1]) << 48;
            dataLen |= ((uint64_t) extra_hdr[2]) << 40;
            dataLen |= ((uint64_t) extra_hdr[3]) << 32;
            dataLen |= ((uint64_t) extra_hdr[4]) << 24;
            dataLen |= ((uint64_t) extra_hdr[5]) << 16;
            dataLen |= ((uint64_t) extra_hdr[6]) << 8;
            dataLen |= ((uint64_t) extra_hdr[7]) << 0;
            mask_idx = 8;
        }

        uint8_t wsbuf_mask[4]  = {0, };
        if (base_hdr.mask)
        {
            wsbuf_mask[0] = ((uint8_t) extra_hdr[mask_idx+0]) << 0;
            wsbuf_mask[1] = ((uint8_t) extra_hdr[mask_idx+1]) << 0;
            wsbuf_mask[2] = ((uint8_t) extra_hdr[mask_idx+2]) << 0;
            wsbuf_mask[3] = ((uint8_t) extra_hdr[mask_idx+3]) << 0;
        }

        // 读取数据区
        byte_t *pData(NULL);
        if (dataLen > 0)
        {
            pData = new byte_t[dataLen+1];
            ret = inbuff.Read((void*)&pData[0], dataLen);
            if (!ret)
            {
                SAFE_DELETE(pData);
                break;
            }
            pkgLen += dataLen;
        }

        // 解析数据区
        CAutoArray guard(pData);
        switch(base_hdr.opcode)
        {
        case TEXT_FRAME:
        case BINARY_FRAME:
        case CONTINUATION:
            if (base_hdr.mask)
            {
                for(size_t i(0); i<dataLen; ++i)
                {
                    pData[i] ^= wsbuf_mask[i&0x03];
                }
            }

            if (base_hdr.fin)
            {
                if (hdrType == hdr_MsgHead || hdrType==hdr_MsgHeadEx)
                {
                    outbuff.Write(pData, dataLen);
                    break;
                }
#if 0
                CMyString strOut;
                dumpbin(pData, dataLen, strOut);
                LogWarn("hdr :%d, data [%s]", (int32_t)hdrType, strOut.c_str());
#endif
                uint16_t dataLen2 = (uint16_t)dataLen;
                byte_t* pNewData(pData);
                if (hdrType==hdr_ProtobufEx)
                {
                    // 前四个字节是cmdId, dataLen 不含cmdId
                    memcpy(&cmdId, pData, sizeof(cmdId));
                    dataLen2 = dataLen - sizeof(cmdId);
                    pNewData = pData + sizeof(cmdId);
//                    LogDebug("WSDBG>protocol: cmdId %u, origin cmdId %u, dataLen:%u", 
//                            CByteOrder::Cast(cmdId), cmdId, dataLen2);
                }
                outbuff.Write(dataLen2);

                if (hdrType==hdr_ProtobufEx)
                {
                    // 回写cmdId
                    outbuff.Write(cmdId);
                }
                outbuff.Write(pNewData, dataLen2);
//                LogDebug("websocket: save %lu length to outbuff", dataLen+2);

                ++pkgCount;
            }
            else
            {
                // 非结束桢，继续接收数据
                LogFatal("[%s(%d):%s]get websocket buffer with multi frame.", __FILE__, __LINE__, __func__);
                outbuff.Write(pData, dataLen);
            }
            break;
        case PING_FRAME:
            if (base_hdr.mask)
            {
                for(size_t i(0); i<dataLen; ++i)
                {
                    pData[i] ^= wsbuf_mask[i&0x03];
                }
            }
            // 直接发PONG 回去
            if (this->sendData(PONG_FRAME, pData, dataLen) < 0)
            {
                bFinished = true;
            }
            break;
        case PONG_FRAME:
            {
                // 不处理
            }
            break;
        case CLOSE_FRAME:
            {
                //断开连接
                LogWarn("receive closeframe, client request closed.");
                this->close();
                bFinished = true;
            }
            break;
        default:
            {
                LogFatal("Got unexpected WebSocket message [%d]", base_hdr.opcode);
                this->sendClose();
                bFinished = true;
            }
            break;
        } // swithc(base_hdr.opcode)

        if (bFinished)
        {
            break;
        }

        guard2.skip(pkgLen);
    }while(true);

    if (bFinished)
    {
        return -2;
    }

    return pkgCount;
}

int32_t CWsWrapper::sendData(enum opcode_type opType, void *buffer, size_t buflen)
{
    if (m_fd <= 0)
    {
        return -1;
    }

    CByteStream outbuff;
    this->Encode(opType, buffer, buflen, outbuff);

    size_t wsbuf_size = outbuff.size();
    byte_t *wsbuf = new byte_t[wsbuf_size+1];
    CAutoArray guard(wsbuf);
    outbuff.Read(wsbuf, wsbuf_size);

    // 发送到服务端
    uint32_t totalSend(0);
    int32_t ret = send_data_ex(m_fd, wsbuf, wsbuf_size, totalSend, 1000);
    if (ret<0)
    {
        int32_t err = errno;
        LogFatal("send data failure, errno: %d, error: %s",
                err, strerror(errno));
        // 对应端出错，关闭
        this->close();

        return -1;
    }
    LogDebug("WSDBG>total send %u bytes.", totalSend);
    return 0;
}

void CWsWrapper::sendClose(void)
{
    uint8_t closeFrame[6] = {0x88, 0x80, 0x00, 0x00, 0x00, 0x00 };
    // last 4 bytes are a masking key
    
    uint32_t totalSend(0);
    send_data_ex(m_fd, (void*)&closeFrame[0], sizeof(closeFrame), totalSend, 1000);
    this->close();
}

void CWsWrapper::close(void)
{
/*    
    close_tcp_socket(m_fd, false);
*/    
    m_fd = -1;
}

void CWsWrapper::DecodeRealIp(CMyString &strIp)
{
    if (strIp.empty())
    {
        return;
    }

    size_t pos = strIp.find_first_of(',');
    if (pos != std::string::npos)
    {
        strIp = strIp.substr(0, pos);
    }
}

void CWsWrapper::SetRealIpParam(int32_t way, const CMyString &strParam)
{
    m_ipWay = way;
    m_ipParam = strParam;
}
