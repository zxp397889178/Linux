/*
 * =====================================================================================
 *
 *       Filename:  WsWrapper.h
 *
 *    Description:  Websocket wrapper
 *
 *        Version:  1.0
 *        Created:  12/15/2019 01:22:23 PM
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
#include "http/HttpData.h"
#include "http/HttpRequest.h"
#include "http/HttpRespone.h"
#include "IPackage.h"

// http://tools.ietf.org/html/rfc6455#section-5.2  Base Framing Protocol
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-------+-+-------------+-------------------------------+
// |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
// |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
// |N|V|V|V|       |S|             |   (if payload len==126/127)   |
// | |1|2|3|       |K|             |                               |
// +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
// |     Extended payload length continued, if payload len == 127  |
// + - - - - - - - - - - - - - - - +-------------------------------+
// |                               |Masking-key, if MASK set to 1  |
// +-------------------------------+-------------------------------+
// | Masking-key (continued)       |          Payload Data         |
// +-------------------------------- - - - - - - - - - - - - - - - +
// :                     Payload Data continued ...                :
// + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
// |                     Payload Data continued ...                |
// +---------------------------------------------------------------+
class CWsWrapper
{
public:
    enum opcode_type
    {
       CONTINUATION = 0x0,                                                                                                                           
       TEXT_FRAME = 0x1,
       BINARY_FRAME = 0x2,
       CLOSE_FRAME = 8,
       PING_FRAME = 9,
       PONG_FRAME = 0xa,
    };

#pragma pack(push, 1)
    struct wsheader
    {
        uint8_t opcode:4;
        uint8_t rsv3:1;
        uint8_t rsv2:1;
        uint8_t rsv1:1;
        uint8_t fin:1;

        uint8_t pkgLen:7;
        uint8_t mask:1;
    };
#pragma pack(pop)

    CWsWrapper(int32_t fd, bool bUseMask);
    ~CWsWrapper();

public:
    // ?????????,??????????????? => outbuff
    // ???????????? -1  inbuff ???????????????
    //          <-1 ??????
    //          =0  ??????
    int32_t Ack(CByteStream &inbuff, CByteStream &outbuff);

    // ????????????????????????
    void Encode(enum opcode_type opType, void *pData, size_t cbData, CByteStream &outbuff);
    void Encode(enum opcode_type opType, CByteStream &inbuff, CByteStream &outbuff);
    // ???inbuff ??????????????????????????????(pkgLen + pData) => outbuff
    // ??????????????????PING/PONG/CLOSE???
    // headerType = 3 ?????????protobuf??????
    //            = 4 cmdId(uint32_t) + protobuf ?????????
    // ?????????: <0 ???????????????????????????socket
    //         =0 ?????????
    //         >0 outbuff ???????????????????????????
    int32_t Decode(CByteStream &inbuff, CByteStream &outbuff, enum HeaderType hdrType);

    // ???Ack?????????????????????????????????IP
    // way = 1 => ??? Header?????????
    // way = 2 => ??? Paramter?????????
    void SetRealIpParam(int32_t way, const CMyString &strParam);
    const char* getRealIp(void) { return m_strRealIp.c_str(); }

private:
    void httpRespone(http::CHttpRespone &resp, 
        uint32_t code, const CMyString &status, 
        const CMyString &data, CByteStream &outbuff);
    void add_header(enum opcode_type opType, 
            uint64_t data_size, CByteStream &buff);

    int32_t sendData(enum opcode_type opType, void *buffer, size_t buflen);
    void sendClose(void);
    void close(void);

    void DecodeRealIp(CMyString &strIp);

private:
    int32_t     m_fd;
    bool        m_bUseMask;

    int32_t     m_ipWay;     // ??????IP?????????
    CMyString   m_ipParam;
    CMyString   m_strRealIp;
};


