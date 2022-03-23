/*
 * =====================================================================================
 *
 *       Filename:  IpTables.h
 *
 *    Description:  iptable, for ip,uin,device policy
 *
 *        Version:  1.0
 *        Created:  2015年09月01日 00时31分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_GAMESVR_IPTABLES_H_
#define _QW_GAMESVR_IPTABLES_H_

#include "BaseCode.h"
#include <set>

class CIpTables
{
	typedef std::set<uint32_t>		UINT_SET;
	typedef std::set<CMyString>		STRING_SET;
public:
	CIpTables();
	~CIpTables();

	bool Init(const CMyString &strFile);
	void Reset(void);

	bool IsIpAllow(uint32_t ip);
	bool IsIpAllow(const CMyString &strIp);
	bool IsUinAllow(uint32_t uin);
	bool IsDeviceAllow(const CMyString &strDeviceId);

private:
	int32_t getIps(CXmlFile &xml, UINT_SET &ipSet, const CMyString &strKey);
	int32_t getUin(CXmlFile &xml, UINT_SET &uinSet, const CMyString &strKey);
	int32_t getDev(CXmlFile &xml, STRING_SET &devSet, const CMyString &strKey);

private:
	bool			m_allowEnable;
	bool			m_forbidEnable;
	UINT_SET		m_setAllowIp;
	UINT_SET		m_setForbidIp;
	UINT_SET		m_setAllowUin;
	UINT_SET		m_setForbidUin;
	STRING_SET		m_setAllowDevice;
	STRING_SET		m_setForbidDevice;
};

#endif // _QW_GAMESVR_IPTABLES_H_

