#pragma once

#include "BaseCode.h"
#include "IpList.h"
#include "NetWork/IPackage.h"

struct MSG_HEAD_EX
{
    uint16_t    protoLen;
    uint16_t    cmdId;
};

struct PortItem
{
    uint16_t            port;
    uint8_t             bSsl;           // 是否SSL，0-非ssl，1-SSL
    uint8_t             acceptNew;      // 是否接收后续的连接:0-不接收，1-接收
    enum NetMode        eMode;
    enum HeaderType     eHeader;
    CIpList             ipList;         // 允许的IP列表

    PortItem() 
        : port(0)
        , bSsl(0)
        , acceptNew(1)
        , eMode(net_Dummy)
        , eHeader(hdr_Dummy)
    {
    }
};

enum NetMode Str2Netmode(const CMyString &str);
CMyString Netmode2Str(enum NetMode eMode);
enum HeaderType Str2Headertype(const CMyString &str);
CMyString Headertype2Str(enum HeaderType eType);

// 解析config.xml中的端口信息
class CPortItemList
{
public:
    CPortItemList();
    ~CPortItemList();

    /* 解析config.xml中配置的端口数量
     * 返回值： <0  出错，请查看日志
     *          =0  未配置服务器
     *          >0  已配置服务的端口数量
     */
    int32_t Init(CXmlFile &xml);
    bool getInfo(uint16_t port, PortItem& portInfo);
    int32_t getPorts(std::vector<uint16_t> &vecPorts);

private:
    int32_t ParsePort(CXmlFile &xml, void *pItem);

private:
    typedef std::map<uint16_t, PortItem>    MAP_PORTINFO;
    MAP_PORTINFO    m_mapPortItem;
};


