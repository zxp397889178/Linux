/*
 * =====================================================================================
 *
 *       Filename:  protoBase.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年09月18日 11时26分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "protoBase.h"

CServerMsgBuffer::CServerMsgBuffer(uint32_t protoLen)
{
	m_pBuf 		= &m_buffer[0];
	m_cbBuf 	= protoLen + sizeof(struct Record);
	m_bNewAlloc = false;
	if (m_cbBuf > sizeof(m_buffer))
	{
		m_bNewAlloc = true;
		m_pBuf = (char*)calloc(sizeof(char), m_cbBuf+8);
	}
}

CServerMsgBuffer::~CServerMsgBuffer()
{
	if (m_bNewAlloc)
	{
		free(m_pBuf);
		m_pBuf = NULL;
	}
}

struct Record* CServerMsgBuffer::getRecord(void)
{
	return (struct Record*)m_pBuf;
}

char* CServerMsgBuffer::getProtoData(void)
{
	return m_pBuf + sizeof(struct Record);
}

uint32_t CServerMsgBuffer::getProtoLen(void)
{
	return m_cbBuf - sizeof(struct Record);
}


uint32_t CServerMsgBuffer::getBufferLen(void)
{
	return m_cbBuf;
}

void* CServerMsgBuffer::getBuffer(void)
{
	return m_pBuf;
}

void CServerMsgBuffer::BuildRecord(uint32_t _protoLen, uint32_t _protoId, 
			uint32_t _uin, uint32_t _sessionId, uint32_t _clientIp)
{
	struct Record *pRecord = this->getRecord();
	pRecord->Build(_protoLen, _protoId, _uin, _sessionId, _clientIp);
	pRecord->Cast();
}


//////////////////////////////////////////////////////////////////////////////////////
//
CBufferGuard::CBufferGuard(void *ptr)
	: m_ptr(ptr)
	, m_autoRelease(true)
{
}

CBufferGuard::~CBufferGuard()
{
	if (m_autoRelease && m_ptr != NULL)
	{
		free(m_ptr);
	}
}

void CBufferGuard::Keep(bool val)
{
	m_autoRelease = !val;
}


