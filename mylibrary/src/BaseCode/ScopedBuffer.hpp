/*
 * =====================================================================================
 *
 *       Filename:  ScopedBuffer.hpp
 *
 *    Description:  范围内存块，尽量利用栈中的内存操作频繁的内存操作
 *
 *        Version:  1.0
 *        Created:  2015年09月13日 21时54分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */
#ifndef _QW_BASECODE_SCOPED_BUFFER_H_
#define _QW_BASECODE_SCOPED_BUFFER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

template<int N>
class TScopedBuffer
{
public:
	TScopedBuffer(uint32_t bufLen)
	{
		memset(m_buffer, 0, sizeof(m_buffer));
		if (bufLen > sizeof(m_buffer))
		{
			m_bNew = true;
			m_ptr = (char*)calloc(sizeof(char), bufLen+8);
		}
		else
		{
			m_bNew = false;
			m_ptr = &m_buffer[0];
		}
	}
	~TScopedBuffer()
	{
		if (m_bNew)
		{
			free(m_ptr);
		}
	}
	char* data(void)
	{ 
		return m_ptr; 
	}

private:
	char	m_buffer[N];
	bool	m_bNew;
	char*	m_ptr;
};

typedef TScopedBuffer<1024>		CTempBuffer;
typedef TScopedBuffer<4096>		C4KBuffer;
typedef TScopedBuffer<8192>		C8KBuffer;

#endif // _QW_BASECODE_SCOPED_BUFFER_H_


