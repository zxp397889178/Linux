/*
 * =====================================================================================
 *
 *       Filename:  WsClient.h
 *
 *    Description:  wetsocket client
 *
 *        Version:  1.0
 *        Created:  12/13/2019 10:10:28 PM
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
#include "WsWrapper.h"

#define USE_WEBSOCKET_WRAPPER
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
class CWsClient 
{
    enum readyState
    {
        CLOSING,
        CLOSED,
        CONNECTING,
        OPEN
    };
/*    
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
*/
public:
    CWsClient();
    ~CWsClient();


    // ?????????ws??????????????????0???????????????<0 ????????????
    // ?????????<0??????
    // 2021-4-22: ????????????????????????????????????
    int32_t Open(const CMyString &url, bool bUseMask, const CMyString &origin, int32_t timeout=10000);

    // ????????????
    int32_t Ping(void);
    int32_t Send(const CMyString &str);
    int32_t Send(void *pData, size_t cbData);
    void sendClose(void);

    // ????????????
    // ?????????   <0 ??????????????????
    //          =0 ?????????????????????
    //          >0 ?????????????????????
    int32_t RecvPackage(void *&pData, size_t &cbData, enum HeaderType hdrType);

    void close();

    bool isActive(void);

private:
    void build_first_pkg(CMyString &str, const char* host, int32_t port, const char *cgi, const CMyString &origin);

/** syntax:
 *
 *  CWsClient ws;
 *  ws.Open("ws://...", "");
 *  ...
 *  void *pBuf = ...
 *  ...
 *  ws.Send(pBuf, cbBuf);
 *
 *  bool running=true;
 *  do {
 *      int32_t cnt = ws.Process();
 *      if (cnt > 0)
 *      {
 *          uint16_t cmdId;
 *          void *pData(NULL);
 *          size_t cbData;
 *          while(ws.getPackage(pData, cbData))
 *          {
 *              //TODO: deal with pData;
 *              SAFE_DELETE(pData);
 *              cbData = 0;
 *              cmdId = 0;
 *          }
 *      }
 *  } while(!running);
 * 
 */
    //????????????
    int32_t sendData(enum CWsWrapper::opcode_type opType, void *buffer, size_t buflen);

    // ??????????????????????????????????????????
    int32_t process_recv(enum HeaderType hdrType);
    bool getPackage(enum HeaderType hdrType, void *&pData, size_t &cbData);


private:
    int32_t         m_fd;
    bool            m_bUseMask;
    CMyString       m_strUrl;

    CMyString       m_strHost;
    CMyString       m_strCgi;
    int32_t         m_nPort;

    CByteStream     m_buffer;  // ????????????
    CByteStream     m_package; // ??????????????? 

    enum readyState m_eState; 
};


