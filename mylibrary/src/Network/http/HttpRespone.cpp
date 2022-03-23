/*
 * =====================================================================================
 *
 *       Filename:  HttpRespone.cpp
 *
 *    Description:  http response
 *
 *        Version:  1.0
 *        Created:  05/22/2015 09:41:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#include "HttpRespone.h"

namespace http {

CHttpRespone::CHttpRespone()
	: CHttpData()
	, m_nStatus(0)
	, m_strStatus()
{

}

CHttpRespone::~CHttpRespone()
{

}

int32_t CHttpRespone::onHeaderBegin(const CMyString &strLine)
{
	
	CMyString::size_type pos = strLine.find_first_of(" \t");
	m_version = strLine.substr(0, pos);
	m_version.trim();

	
	while( (pos < strLine.length()) && isspace(strLine[pos]) )
	{
		++pos;
	}

	
	if (pos < strLine.length()) 
	{
		CMyString::size_type end = strLine.find_first_of(" \t", pos);
		if (end == CMyString::npos)
		{
			CMyString code = strLine.substr(pos);
			m_nStatus = StrToUint32(code.c_str());
		}
		else
		{
			CMyString code = strLine.substr(pos, end-pos);
			m_nStatus = StrToUint32(code.c_str());
			m_strStatus = strLine.substr(end+1);
			m_strStatus.trim();
		}
	}

	return 0;
}

void CHttpRespone::onHeaderEnd(void)
{
	CHttpData::onHeaderEnd();
#if 0
	LogDebug("Resp>status: %u, %s", m_nStatus, m_strStatus.c_str());
#endif
}

void CHttpRespone::clear(void)
{
	CHttpData::clear();
	m_nStatus = 0;
	m_strStatus.clear();
}

void CHttpRespone::pack(CByteStream &data, bool bSetContentLength)
{
	CMyString header;
	if (m_version.empty())
	{
		header.Format("HTTP/1.0 %u %s\r\n", m_nStatus, m_strStatus.c_str());
	}	
	else
	{
		header.Format("%s %u %s\r\n", m_version.c_str(), 
			m_nStatus, m_strStatus.c_str());
	}

	data.Write((void*)header.c_str(), header.size());

	CHttpData::pack(data, bSetContentLength);
}

void CHttpRespone::getPost(CByteStream &buff)
{
	buff = m_buffer;
}


} 


