/*
 * =====================================================================================
 *
 *       Filename:  ByteStreamEx.h
 *
 *    Description:  ByteStream base on manual buffer
 *
 *        Version:  1.0
 *        Created:  2014年11月15日 20时06分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_BYTE_STREAM_EX_H_
#define _MYLIB_BASECODE_BYTE_STREAM_EX_H_

#include <stdio.h>
#include <string.h>

class CByteStreamEx
{
public:
	explicit CByteStreamEx(size_t nCapacity = 1024);
	~CByteStreamEx();

	void Write(const char *buffer);
	void Write(const void *buffer, size_t cbLen);
	// char/unsigned char/short/unsigned short/int/
	// long/unsigned long/long long/unsigned long long/
	// float/double can use template Write
	// sam as thr Read
	void Write(const CByteStreamEx &rhs);

	void SetPeekMode(bool bPeekMode);
	bool Read(void *pBuf, size_t cbLen);
	bool Read(CByteStreamEx &to, size_t cbLen=-1);


	bool empty(void) const;
	size_t size(void) const;
	size_t capacity(void) const;
	void clear(void);
	void skip(size_t cbLen);
	char *data(void);
	const char* c_str(void);

public:
	template<typename T>
	void Write(T val)
	{
		unsigned char *pBuf = (unsigned char*)&val;
		this->Write(pBuf, sizeof(T));
	}

	template<typename T>
	bool Read(T &val)
	{
		void *pBuf = (void*)&val;
		return this->Read(pBuf, sizeof(T));
	}

private:
	bool CanRead(size_t cbLen);

	void push_back(const char *buffer);
	void push_back(const void *buffer, size_t cbLen);
	//void push_front(const char *buffer);
	//void push_front(const void *buffer, size_t cbLen);
	void DealWriteSpace(size_t len);
	
private:
	char*					m_pBuffer;
	size_t					m_cbCapacity;

	bool					m_bPeekMode;
	size_t					m_cbPeekPos;
	size_t					m_cbReadPos;
	size_t					m_cbWritePos;
};


#endif	// _MYLIB_BASECODE_BYTE_STREAM_EX_H_


