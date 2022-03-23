/*
* =====================================================================================
*
*       Filename:  EncryptMgt.cpp
*
*    Description:  加密管理类
*
*        Version:  1.0
*        Created:  2015年7月2日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/
#include "EncryptMgt.h"
#include <stdint.h>
#include <string.h>

const int32_t aa = 0xAB;
const int32_t bb = 0xEC;
const int32_t cc = 0xF5;
const uint32_t key1 = 0x99A54B24;
const uint32_t key2 = 0x4258B586;

EncryptMgt *EncryptMgt::m_instance = NULL;

EncryptMgt::EncryptMgt()
{
	memset(m_key1, 0, sizeof(m_key1));
	memset(m_key2, 0, sizeof(m_key2));
}

EncryptMgt::~EncryptMgt()
{
}

EncryptMgt *EncryptMgt::getInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new EncryptMgt();
		m_instance->Init();
	}
	return m_instance;
}

bool EncryptMgt::Init()
{
	try
	{
		int	a1, b1, c1, fst1;
		a1 = ((key1 << 2) & 0xFFFF) ^ aa;
		b1 = ((key1 >> 5) & 0xFFFF) ^ bb;
		c1 = ((key1 >> 15) & 0xFFFF) ^ cc;
		fst1 = (key1 >> 16) & 0xFFFF;

		int	a2, b2, c2, fst2;
		a2 = ((key2 << 4) & 0xFFFF) ^ aa;
		b2 = ((key2 >> 12) & 0xFFFF) ^ bb;
		c2 = ((key2 >> 18) & 0xFFFF) ^ cc;
		fst2 = (key2 >> 21) & 0xFFFF;

		unsigned char nCode = (unsigned char)fst1;
		int i = 0;
		for (i = 0; i < 256; i++)
		{
			m_key1[i] = nCode;
			nCode = (unsigned char)((a1*nCode*i + b1*c1 + nCode*i) % 256);
		}

		nCode = (unsigned char)fst2;
		for (i = 0; i < 256; i++)
		{
			m_key2[i] = nCode;
			nCode = (unsigned char)((a2*nCode*i + b2*c2 + nCode*i) % 256);
		}

	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool EncryptMgt::Encrypt(unsigned char *buf, size_t nLen)
{
	return this->internal_encrypt(buf, nLen);
}

bool EncryptMgt::Decrypt(unsigned char *buf, size_t nLen)
{
	return this->internal_encrypt(buf, nLen);
}

bool EncryptMgt::internal_encrypt(unsigned char *buf, size_t nLen)
{
	try
	{
		for (size_t i = 0; i < nLen; i++)
		{
			buf[i] ^= m_key1[i % 256];
			buf[i] ^= m_key2[i % 256];
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}



