/*
 * =====================================================================================
 *
 *       Filename:  IpList.cpp
 *
 *    Description:  对IP的解析，主要用于黑白名单
 *
 *        Version:  1.0
 *        Created:  2021年03月02日 16时22分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "IpList.h"

CIpList::CIpList()
{

}

CIpList::CIpList(const CMyString &ips, const char delimiter)
{
    setIps(ips, delimiter);
}

CIpList::~CIpList()
{

}

void CIpList::setIps(const CMyString &ips, const char delimiter)
{
    bool val = m_ipList.Split(ips, delimiter);
    if (!val)
    {
        LogWarn("ips [%s] split failure.", ips.c_str());
        return;
    }
}

bool CIpList::Find(const CMyString &ip)
{
    bool bFind(false);
    for(int i=0; i<m_ipList.size(); ++i)
    {
        if (ip == m_ipList[i])
        {
            bFind = true;
            break;
        }
    }
    return bFind;
}
