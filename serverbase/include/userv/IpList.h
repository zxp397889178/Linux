/*
 * =====================================================================================
 *
 *       Filename:  IpList.h
 *
 *    Description:  对IP的解析，主要用于黑白名单
 *
 *        Version:  1.0
 *        Created:  2021年03月02日 16时18分21秒
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
#include <set>

class CIpList
{
public:
    CIpList();
    CIpList(const CMyString &ips, const char delimiter=',');
    ~CIpList();

    void setIps(const CMyString &ips, const char delimiter=',');
    // 判断IP是否在列表中，若传进去的IP为空，或0.0.0.0一律为false
    bool Find(const CMyString &ip);
    bool Find(uint32_t ip);

public:
    static CMyString Ip2Str(uint32_t uIp);
    static uint32_t Str2Ip(const CMyString &strIp);

private:
    CStrSpliter                 m_ipList;
    std::set<uint32_t>          m_setIp;
};

