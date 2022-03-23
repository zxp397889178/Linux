/*
 * =====================================================================================
 *
 *       Filename:  IpTables.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年09月01日 00时39分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "IpTables.h"
#include "NetWork/NetFunc.h"

CIpTables::CIpTables()
	: m_allowEnable(false)
	, m_forbidEnable(false)  
	, m_setAllowIp()
	, m_setForbidIp()
	, m_setAllowUin()
	, m_setForbidUin()
	, m_setAllowDevice()
	, m_setForbidDevice()  
{
}


CIpTables::~CIpTables()
{
}

void CIpTables::Reset(void)
{
	m_allowEnable = false;
	m_forbidEnable = false;

	m_setAllowIp.clear();
	m_setAllowUin.clear();
	m_setAllowDevice.clear();

	m_setForbidUin.clear();
	m_setForbidIp.clear();
	m_setForbidDevice.clear();
}

bool CIpTables::Init(const CMyString &strFile)
{
	CXmlFile xml;

	if (!xml.Load(strFile.c_str()))
	{
		LogFatal("can not load xml %s", strFile.c_str());
		return false;
	}

	this->Reset();

	try
	{
		m_allowEnable = xml.GetBool("allow", "enable");
		m_forbidEnable = xml.GetBool("forbid", "enable");
		if (m_allowEnable)
		{
			this->getIps(xml, m_setAllowIp, "allow");
			this->getUin(xml, m_setAllowUin, "allow");
			this->getDev(xml, m_setAllowDevice, "allow");
		}
		if (m_forbidEnable)
		{
			this->getIps(xml, m_setForbidIp, "forbid");
			this->getUin(xml, m_setForbidUin, "forbid");
			this->getDev(xml, m_setForbidDevice, "forbid");
		}
	}
	catch(...)
	{
		LogFatal("can not load iptable config file.");
		return false;
	}

	return true;
}

int32_t CIpTables::getIps(CXmlFile &xml, UINT_SET &ipSet, const CMyString &strKey)
{
	CMyString strRoot("%s/ip", strKey.c_str());
	void *pElement = xml.GetElement(strRoot.c_str());
	if (NULL == pElement)
	{
		return 0;
	}
	while(pElement != NULL)
	{
		CMyString val = xml.GetValue(pElement, "val");
		if (!val.empty())
		{
			uint32_t ip = str2ip(val.c_str());
			if (ip != 0)
			{
				ipSet.insert(ip);
			}
		}
		pElement = xml.GetNextSibling(pElement, "ip");
	}
	return (int32_t)ipSet.size();
}

int32_t CIpTables::getUin(CXmlFile &xml, UINT_SET &uinSet, const CMyString &strKey)
{
	CMyString strRoot("%s/uin", strKey.c_str());
	void *pElement = xml.GetElement(strRoot.c_str());
	if (NULL == pElement)
	{
		return 0;
	}
	while(pElement != NULL)
	{
		CMyString val = xml.GetValue(pElement, "val");
		if (!val.empty())
		{
			uint32_t uin = StrToUint32(val.c_str());
			if (uin != 0)
			{
				uinSet.insert(uin);
			}
		}
		pElement = xml.GetNextSibling(pElement, "uin");
	}
	return (int32_t)uinSet.size();
}

int32_t CIpTables::getDev(CXmlFile &xml, STRING_SET &devSet, const CMyString &strKey)
{
	CMyString strRoot("%s/device", strKey.c_str());
	void *pElement = xml.GetElement(strRoot.c_str());
	if (NULL == pElement)
	{
		return 0;
	}
	while(pElement != NULL)
	{
		CMyString val = xml.GetValue(pElement, "val");
		if (!val.empty())
		{
			devSet.insert(val);
		}
		pElement = xml.GetNextSibling(pElement, "device");
	}
	return (int32_t)devSet.size();
}


bool CIpTables::IsIpAllow(uint32_t ip)
{
	if (m_forbidEnable)
	{
		if (m_setForbidIp.find(ip) != m_setForbidIp.end())
		{
			return false;
		}
	}

	if (m_allowEnable)
	{
		if (m_setAllowIp.find(ip) == m_setAllowIp.end())
		{
			return false;
		}
	}

	return true;
}

bool CIpTables::IsIpAllow(const CMyString &strIp)
{
	uint32_t ip = str2ip(strIp.c_str());
	if (ip == 0)
	{
		return true;
	}

	return this->IsIpAllow(ip);
}

bool CIpTables::IsUinAllow(uint32_t uin)
{
	if (m_forbidEnable)
	{
		if (m_setForbidUin.find(uin) != m_setForbidUin.end())
		{
			return false;
		}
	}

	if (m_allowEnable)
	{
		if (m_setAllowUin.find(uin) == m_setAllowUin.end())
		{
			return false;
		}
	}

	return true;
}

bool CIpTables::IsDeviceAllow(const CMyString &strDeviceId)
{
	if (m_forbidEnable)
	{
		if (m_setForbidDevice.find(strDeviceId) != m_setForbidDevice.end())
		{
			return false;
		}
	}

	if (m_allowEnable)
	{
		if (m_setAllowDevice.find(strDeviceId) != m_setAllowDevice.end())
		{
			return false;
		}
	}

	return true;
}

