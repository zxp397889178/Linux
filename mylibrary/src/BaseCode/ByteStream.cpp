/*
 * =====================================================================================
 *
 *       Filename:  ByteStream.cpp
 *
 *    Description:  byte stream base on std::vector
 *
 *        Version:  1.0
 *        Created:  2014/11/13 00时04分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */


#include "ByteStream.h"
#include <algorithm>
#include <iterator>
#include <sys/types.h>



CByteStream::CByteStream(int nCapacity)
	: m_stream()
	, m_bPeekMode(false)
	, m_iterPeek()
{
	// 对于 deque 容器，不支持预留容量
	// 对于 vector 容器，支持预留容量
}

CByteStream::~CByteStream()
{
	m_stream.clear();
}

void CByteStream::push_back(const char *buffer)
{
	//char *pc1 = const_cast<char*>(buffer);
	char *pc1 = (char*)buffer;
	size_t len = strlen(pc1);
	char *pc2 = pc1+len;
	std::copy(pc1, pc2, std::back_inserter(m_stream));
}

void CByteStream::push_back(const void *buffer, size_t cbLen)
{
	void *pc = (void*)buffer;
	char *pc1 = (char*)pc;
	//char *pc1 = (char*)(const_cast<void*>(buffer));
	char *pc2 = pc1 + cbLen;
	std::copy(pc1, pc2, std::back_inserter(m_stream));
}

void CByteStream::push_front(const char *buffer)
{
	//char *pc1 = const_cast<char*>(buffer);
	char *pc1 = (char*)buffer;
	size_t len = strlen(pc1);
	char *pc2 = pc1+len;
	std::copy(pc1, pc2, std::front_inserter(m_stream));
}

void CByteStream::push_front(const void *buffer, size_t cbLen)
{
	//char *pc1 = (char*)(const_cast<void*>(buffer));
	void *pc = (void*)buffer;
	char *pc1 = (char*)pc;
	char *pc2 = pc1 + cbLen;
	std::copy(pc1, pc2, std::front_inserter(m_stream));
}

void CByteStream::Write(const char *buffer)
{
	this->push_back(buffer);
}

void CByteStream::Write(const void *buffer, size_t cbLen)
{
	this->push_back(buffer, cbLen);
}

void CByteStream::Write(char val)
{
	m_stream.push_back(val);
}

void CByteStream::Write(unsigned char val)
{
	m_stream.push_back(val);
}

void CByteStream::Write(const CByteStream &rhs)
{
	std::copy(rhs.m_stream.begin(), rhs.m_stream.end(), std::back_inserter(m_stream));
}

void CByteStream::Write(const CMyString &str)
{
	this->push_back(str.c_str(), str.length());
}

void CByteStream::SetPeekMode(bool bPeekMode)
{
	m_bPeekMode = bPeekMode;
	m_iterPeek = m_stream.begin();
}

bool CByteStream::CanRead(size_t cbLen)
{
	if (m_bPeekMode)
	{
		int nDist = std::distance(m_iterPeek, m_stream.end());
		if (nDist<(int)cbLen)
		{
			return false;
		}
	}
	else
	{
		if(m_stream.size() < cbLen)
		{
			return false;
		}
	}

	return true;
}

bool CByteStream::Read(void *pBuf, size_t cbLen)
{
	if ( !this->CanRead(cbLen) || 0==cbLen )
	{
		return false;
	}

	BYTE_QUEUE::iterator iter1(m_stream.begin());
	if (m_bPeekMode)
	{
		iter1 = m_iterPeek;
	}
	BYTE_QUEUE::iterator iter2(iter1);
	std::advance(iter2, cbLen);
	char *pc = (char*)pBuf;
	// copy 是不能用的	
	// std::copy(iter1, iter2, buff );
	for(BYTE_QUEUE::iterator it(iter1); it != iter2; ++it)
	{
		*pc++ = *it;
	}

	if (m_bPeekMode)
	{
		m_iterPeek = iter2;
	}
	else
	{
		m_stream.erase(iter1, iter2);
	}

	return true;
}


bool CByteStream::Read(char &val)
{
	if (!this->CanRead(sizeof(val) ))
	{
		return false;
	}

	BYTE_QUEUE::iterator iter1(m_stream.begin());
	if (m_bPeekMode)
	{
		iter1 = m_iterPeek++;
	}
	val = (char)*iter1;

	if (!m_bPeekMode)
	{
		m_stream.pop_front();
	}
	return true;
}

bool CByteStream::Read(unsigned char &val)
{
	char c_val;
	bool ret = this->Read(c_val);
	if (!ret)
	{
		return false;
	}
	val = (unsigned char)c_val;
	return true;
}

bool CByteStream::Read(CByteStream &to, size_t cbLen)
{
	if ((size_t)-1==cbLen)
	{
		cbLen = m_stream.size();
	}

	if ( !this->CanRead(cbLen) )
	{
		return false;
	}

	BYTE_QUEUE::iterator iter1(m_stream.begin());
	if (m_bPeekMode)
	{
		iter1 = m_iterPeek;
	}
	BYTE_QUEUE::iterator iter2(iter1);
	std::advance(iter2, cbLen);

	std::copy(iter1, iter2, std::back_inserter(to.m_stream));
	if (m_bPeekMode)
	{
		m_iterPeek = iter2;
	}
	else
	{
		m_stream.erase(iter1, iter2);
	}
	return true;
}

bool CByteStream::Read(CMyString &to, size_t cbLen)
{
	if ((size_t)-1==cbLen)
	{
		cbLen = m_stream.size();
	}

	if ( !this->CanRead(cbLen) )
	{
		return false;
	}

	BYTE_QUEUE::iterator iter1(m_stream.begin());
	if (m_bPeekMode)
	{
		iter1 = m_iterPeek;
	}
	BYTE_QUEUE::iterator iter2(iter1);
	std::advance(iter2, cbLen);

	to.append(iter1, iter2);
	if (m_bPeekMode)
	{
		m_iterPeek = iter2;
	}
	else
	{
		m_stream.erase(iter1, iter2);
	}
	return true;
}

bool CByteStream::empty(void) const
{
	return m_stream.empty();
}

size_t CByteStream::size(void) const
{
	return m_stream.size();
}

void CByteStream::clear(void)
{
	m_stream.clear();
}

void CByteStream::skip(size_t cbLen)
{
	if (0==cbLen || m_stream.empty() )
	{
		return;
	}

	BYTE_QUEUE::iterator iter1(m_stream.begin());
	if (m_bPeekMode)
	{
		iter1 = m_iterPeek;
	}
	int nDistance = std::distance(iter1, m_stream.end());
	if (nDistance < (int)cbLen)
	{
		//printf("distance [%d], left size [%ld]\n", nDistance, cbLen);
		if (m_bPeekMode)
		{
			m_iterPeek = m_stream.end();
		}
		else
		{
			m_stream.erase(iter1, m_stream.end());
		}
		return;
	}

	BYTE_QUEUE::iterator iter2(iter1);
	std::advance(iter2, cbLen);
	if (m_bPeekMode)
	{
		m_iterPeek = iter2;
	}
	else
	{
		m_stream.erase(iter1, iter2);
	}
}

/////////////////////////////////////////////////////////////////////
//
CByteStreamGuard::CByteStreamGuard(CByteStream &buff)
    : m_buff(buff)
    , m_bGuard(true)
{
    m_buff.SetPeekMode(true);
}

CByteStreamGuard::~CByteStreamGuard()
{
    if (m_bGuard)
    {
        m_buff.SetPeekMode(false);
    }
}

void CByteStreamGuard::skip(uint32_t len)
{
    if (m_bGuard)
    {
        m_buff.SetPeekMode(false);
        m_bGuard = false;
    }
    m_buff.skip(len);
}


