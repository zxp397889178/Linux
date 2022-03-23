/*
 * =====================================================================================
 *
 *       Filename:  Decoder.h
 *
 *    Description:  tcp/ws 收到的数据进行解包
 *
 *        Version:  1.0
 *        Created:  01/15/2020 03:09:59 PM
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
#include "PortItemList.h"

class CDecoder
{
public:
    CDecoder();
    ~CDecoder();

    void Init(enum HeaderType headerType);
    int32_t Decode(CByteStream &buff, uint32_t &cmdId, byte_t* &pData, size_t &cbData);

private:
    /* protocol:
     * +---------+---------+---------+-----------------------------------------+
     * | protoLen| cmdId   | code    | data                                    |
     * +---------+---------+---------+-----------------------------------------+
     * | 4 bytes | 4 bytes | 8 bytes | protoLen-16 bytes                       |
     * +---------+---------+---------+-----------------------------------------+
     * protoLen 是包文长度，包含了自身的数据，采用网络字节序传输，传输或使用前要转码
     * cmdId 是命令ID，采用网络字节序传输，传输或使用前要转码
     * 返回值:  <0 失败 0-不需要处理; 1-需要处理 (下同)
     * */
    int32_t msghead_decode(CByteStream &buff, uint32_t &cmdId, byte_t* &pData, size_t &cbData);

    /* protocol:
     * +---------+---------+--------------------------------------------------+
     * | protoLen| cmdId   | data                                             |
     * +---------+---------+--------------------------------------------------+
     * | 2 bytes | 2 bytes | protoLen-4  bytes                                |
     * +---------+---------+--------------------------------------------------+
     * protoLen 是包文长度，包含了自身的数据，采用网络字节序传输，传输或使用前要转码
     * cmdId 是命令ID，采用网络字节序传输，传输或使用前要转码
     * 返回值:  <0 失败 0-不需要处理; 1-需要处理 (下同)
     * */
    int32_t msghead_ex_decode(CByteStream &buff, uint32_t &cmdId, byte_t* &pData, size_t &cbData);

    /* protocol: 全部是用 protobuf,协议约定如下：
     *           |<------------ protobuf ------------------------------------>|
     * +---------+---------+--------------------------------------------------+
     * |protoLen | cmdId   | data                                             |
     * +---------++---------+-------------------------------------------------+
     * |2 bytes  |4 bytes  | protoLen-2 bytes                                 |
     * +---------+---------+---------+----------------------------------------+
     * cmdId 采用protobuf的加解密，不需要特殊处理
     * */
    int32_t protobuf_decode(CByteStream &buff, uint32_t &cmdId, byte_t* &pData, size_t &cbData);

    /* protocol: 部份采用 protobuf,协议约定如下：
     *                     |<------------ protobuf -------------------------->|
     * +---------+---------+--------------------------------------------------+
     * |protoLen | cmdId   | data                                             |
     * +---------++---------+-------------------------------------------------+
     * |2 bytes  |4 bytes  | protoLen-6 bytes                                 |
     * +---------+---------+---------+----------------------------------------+
     * cmdId 采用网络字节序， 传输前要转码，收到数据后要解码
     * */
    int32_t protobuf_ex_decode(CByteStream &buff, uint32_t &cmdId, byte_t* &pData, size_t &cbData);

    /* 无格式的通讯协议，不做任何的转换，直接从buff取出数据
     */
    int32_t http_decode(CByteStream &buff, byte_t* &pData, size_t &cbData);
private:
    enum HeaderType     m_headerType;
};

