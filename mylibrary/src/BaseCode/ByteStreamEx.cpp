/*
 * =====================================================================================
 *
 *       Filename:  ByteStreamEx.cpp
 *
 *    Description:  bytestream base on std::vector
 *
 *        Version:  1.0
 *        Created:  2014年11月15日 20时06分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#include "ByteStreamEx.h"
#include <stdlib.h>
#include <sys/types.h>
#include <algorithm>



CByteStreamEx::CByteStreamEx(size_t nCapacity)
	: m_pBuffer(NULL) 
	, m_cbCapacity(nCapacity)
	, m_bPeekMode(false)
	, m_cbPeekPos(0)
	, m_cbReadPos(0)
	, m_cbWritePos(0)
{
	if (m_cbCapacity == 0)
	{
		m_cbCapacity = 1024;	
	}
	m_pBuffer = (char*)calloc(sizeof(char), m_cbCapacity);
}

CByteStreamEx::~CByteStreamEx()
{
	if (NULL != m_pBuffer)
	{
		free(m_pBuffer);
		m_pBuffer = 0;
	}
}

void CByteStreamEx::push_back(const char *buffer)
{
	if (NULL==buffer)
	{
		return;
	}

	size_t len = strlen(buffer);
	this->push_back((void*)buffer, len);
}

void CByteStreamEx::push_back(const void *buffer, size_t cbLen)
{
	if (NULL == buffer || cbLen == 0)
	{
		return;
	}

	this->DealWriteSpace(cbLen);

	char *pc = m_pBuffer + m_cbWritePos;
	memcpy(pc, buffer, cbLen);
	m_cbWritePos += cbLen;
}

void CByteStreamEx::DealWriteSpace(size_t cbLen)
{
	// if write space is enough
	if (m_cbWritePos + cbLen < m_cbCapacity)
	{
		return;
	}

	// if the space is enough
	size_t cbDataLen = this->size() + cbLen;
	if ( cbDataLen < m_cbCapacity)
	{
		//erase readed data
		memmove(m_pBuffer, this->data(), this->size());
		if (m_bPeekMode)
		{
			m_cbPeekPos = (m_cbPeekPos - m_cbReadPos);
		}
		m_cbWritePos = this->size();
		m_cbReadPos = 0;
		return;
	}

	// re-allocate buffer
	size_t nCapacity = std::max(cbDataLen, m_cbCapacity) * 2;
	char *pc = (char *)calloc(sizeof(char), nCapacity);
	memcpy(pc, this->data(), this->size());
	free(m_pBuffer);
	m_pBuffer = pc;
	m_cbCapacity = nCapacity;
	if (m_bPeekMode)
	{
		m_cbPeekPos = (m_cbPeekPos - m_cbReadPos);
	}
	m_cbWritePos = this->size();
	m_cbReadPos = 0;
}

void CByteStreamEx::Write(const char *buffer)
{
	this->push_back(buffer);
}

void CByteStreamEx::Write(const void *buffer, size_t cbLen)
{
	this->push_back(buffer, cbLen);
}

void CByteStreamEx::Write(const CByteStreamEx &rhs)
{
	// const char *pc = rhs.c_str();
	// size_t len = rhs.size();
	//
	char *pc = rhs.m_pBuffer + rhs.m_cbReadPos;
	size_t len = rhs.m_cbWritePos - rhs.m_cbReadPos;
	this->Write(pc, len);
}

void CByteStreamEx::SetPeekMode(bool bPeekMode)
{
	m_bPeekMode = bPeekMode;
	m_cbPeekPos = m_cbReadPos;
}

bool CByteStreamEx::CanRead(size_t cbLen)
{
	if (m_bPeekMode)
	{
		if (m_cbWritePos < m_cbPeekPos + cbLen)
		{
			return false;
		}
	}
	else
	{
		if (this->size() < cbLen)
		{
			return false;
		}
	}

	return true;
}

bool CByteStreamEx::Read(void *pBuf, size_t cbLen)
{
	if ( !this->CanRead(cbLen) || 0==cbLen )
	{
		return false;
	}

	//memcpy(pBuf, this->data(), cbLen);
	char *ptr(NULL);
	if (m_bPeekMode)
	{
		ptr = m_pBuffer + m_cbPeekPos;
		m_cbPeekPos += cbLen;
	}
	else
	{
		ptr = m_pBuffer + m_cbReadPos;
		m_cbReadPos += cbLen;
	}
	memcpy(pBuf, ptr, cbLen);

	return true;
}

bool CByteStreamEx::Read(CByteStreamEx &to, size_t cbLen)
{
	if ((size_t)-1==cbLen)
	{
		cbLen = this->size();
	}

	if ( !this->CanRead(cbLen) )
	{
		return false;
	}


	// to.Write(this->data(), cbLen);
	char *ptr(NULL);
	if (m_bPeekMode)
	{
		ptr = m_pBuffer + m_cbPeekPos;
		m_cbPeekPos += cbLen;
	}
	else
	{
		ptr = m_pBuffer + m_cbReadPos;
		m_cbReadPos += cbLen;
	}
	to.Write(ptr, cbLen);
	
	return true;
}

bool CByteStreamEx::empty(void) const
{
	return (this->size()==0);
}

size_t CByteStreamEx::size(void) const
{
	return (m_cbWritePos - m_cbReadPos);
}

size_t CByteStreamEx::capacity(void) const
{
	return m_cbCapacity;
}

char* CByteStreamEx::data(void)
{
	return (m_pBuffer + m_cbReadPos);
}

const char* CByteStreamEx::c_str(void)
{
	return (m_pBuffer + m_cbReadPos);
}

void CByteStreamEx::clear(void)
{
	memset(m_pBuffer, 0, m_cbCapacity);
	m_cbReadPos = 0;
	m_cbWritePos = 0;
	m_cbPeekPos = 0;
}

void CByteStreamEx::skip(size_t cbLen)
{
	if (0==cbLen || this->empty() )
	{
		return;
	}

	if (cbLen > this->size())
	{
		cbLen = this->size();
	}

	if (m_bPeekMode)
	{
		m_cbPeekPos += cbLen;
	}
	else
	{
		m_cbReadPos += cbLen;
	}
}

