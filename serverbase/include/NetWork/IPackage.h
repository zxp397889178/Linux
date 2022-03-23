/*
 * =====================================================================================
 *
 *       Filename:  IPackage.h
 *
 *    Description:  对网络消息包头进行定义
 *
 *        Version:  1.0
 *        Created:  2021年08月25日 20时09分50秒
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

//网络模式
// tcp, websocket, http
enum NetMode
{
    net_Dummy = 0,
    net_Tcp   = 1,      // tcp 端口
    net_Websocket,      // ws 端口
    net_Http,           // http端口
};

// 包头的类型，仅针对 Tcp/Websocket 网络方式有效
// msghead, protobuf, protobuf_ex, text/plain
enum HeaderType
{
    hdr_Dummy = 0,

    /* protocol:
     * 自定义的包头 MSG_HEAD:
     * MSG_HEAD: uint32_t(pkgLen) + uint32_t(pkgId) + uint64_t(code) + {... data ...}
     *
     * +---------+---------+---------+-----------------------------------------+
     * | protoLen| cmdId   | code    | data                                    |
     * +---------+---------+---------+-----------------------------------------+
     * | 4 bytes | 4 bytes | 8 bytes | protoLen-16 bytes                       |
     * +---------+---------+---------+-----------------------------------------+
     * |NetOrder |NetOrder |NetOrder | ...                                     |
     * +---------+---------+---------+-----------------------------------------+
     **/

    hdr_MsgHead,
    /* protocol:
     * 简化版的自定义包头 MSG_HEADER;
     * MSG_HEADER: uint16_t(pkgLen) + uint16_t(cmdId) + {... data ... }
     *
     * +---------+---------+--------------------------------------------------+
     * | protoLen| cmdId   | data                                             |
     * +---------+---------+--------------------------------------------------+
     * | 2 bytes | 2 bytes | protoLen-4  bytes                                |
     * +---------+---------+--------------------------------------------------+
     * |NetOrder |NetOrder | ...                                              |
     * +---------+---------+--------------------------------------------------+
     **/
    hdr_MsgHeadEx,

    /* protocol: 全部是用 protobuf,协议约定如下：
     * protobuf格式，仅包含cmdId(uint16_t), pkgLen从ws包头获得
     * protobuf( uint32_t[cmdId]+data[...]  )
     *
     *           |<------------ protobuf ------------------------------------>|
     * +---------+---------+--------------------------------------------------+
     * |protoLen | cmdId   | data                                             |
     * +---------++---------+-------------------------------------------------+
     * |2 bytes  |4 bytes  | protoLen-4 bytes                                 |
     * +---------+---------+--------------------------------------------------+
     * |HostOrder|HostOrder| ...                                              |
     * +---------+---------+--------------------------------------------------+
     * |<--------------------- tcp ------------------------------------------>|
     *           |<------------------------ websocket ----------------------->|
     **/
    hdr_Protobuf,

    /* protocol: 部份采用 protobuf,协议约定如下：
     * uint32_t[cmdId] + protobuf(data[...])
     *
     *                     |<------------ protobuf -------------------------->|
     * +---------+---------+--------------------------------------------------+
     * |protoLen | cmdId   | data                                             |
     * +---------++---------+-------------------------------------------------+
     * |2 bytes  |4 bytes  | protoLen bytes                                   |
     * +---------+---------+--------------------------------------------------+
     * |HostOrder|NetOrder | ...                                              |
     * +---------+---------+--------------------------------------------------+
     * |<-------------------- tcp ------------------------------------------->|
     *           |<------------------------ websocket ----------------------->|
     **/
    hdr_ProtobufEx,

    // text / plain, 纯文本
    hdr_Plain,
};

// 包头类型对应的中文字说明
// const char* g_szHdrType[] = { "N/A", "msghead", "msghead_ex", "protobuf", "protobuf_ex", "plain" };


