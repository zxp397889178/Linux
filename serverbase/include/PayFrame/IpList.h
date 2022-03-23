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
class CIpList
{
public:
    CIpList();
    CIpList(const CMyString &ips, const char delimiter=',');
    ~CIpList();

    void setIps(const CMyString &ips, const char delimiter=',');
    bool Find(const CMyString &ip);

private:
    CStrSpliter                 m_ipList;
};

