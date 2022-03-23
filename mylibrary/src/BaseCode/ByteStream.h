/*
 * =====================================================================================
 *
 *       Filename:  ByteStream.h
 *
 *    Description:  byte stream base on std::vector
 *
 *        Version:  1.0
 *        Created:  2014/11/13 00时01分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_BYTE_STREAM_H_
#define _MYLIB_BASECODE_BYTE_STREAM_H_

//#include <vector>
#include <deque>
#include <stdio.h>
#include <string.h>
#include "MyString.h"


class CByteStream
{
	typedef std::deque<unsigned char>	BYTE_QUEUE;
public:
	explicit CByteStream(int nCapacity = 1024);
	~CByteStream();

	void Write(const char *buffer);
	void Write(const void *buffer, size_t cbLen);
	void Write(char val);
	void Write(unsigned char val);
	// short/unsigned short/int/long/unsigned long/long long/unsigned long long/
	// float/double can use template Write
	// sam as thr Read
	void Write(const CByteStream &rhs);
	void Write(const CMyString &str);

	void SetPeekMode(bool bPeekMode);
	bool Read(void *pBuf, size_t cbLen);
	bool Read(char &val);
	bool Read(unsigned char &val);
	bool Read(CByteStream &to, size_t cbLen=-1);
	bool Read(CMyString &to, size_t cbLen=-1);

	bool empty(void) const;
	size_t size(void) const;
	void clear(void);
	void skip(size_t cbLen);

    // 读写可以用Push/Pop来处理
    // 2021-07-27
public:
    void Push(const byte_t *pData, size_t cbData) { this->push_back((const void*)pData, cbData); }
    void Push(const CMyString &str)               { this->push_back(str.c_str()); }
    void Push(const CByteStream &stream)          { this->Write(stream); }
    void Push(const char *str)                    { this->push_back(str); }
    void Push(char val)                           { this->Write(val); }
    void Push(byte_t val)                         { this->Write(val); }
    template<typename T> void Push(T val)         { this->Write<T>(val); }

    bool Pop(byte_t *pData, size_t cbData)        { return this->Read((void*)pData, cbData); }
    bool Pop(CByteStream &buffer, size_t cbBuffer){ return this->Read(buffer, cbBuffer); }
    bool Pop(CMyString &str, size_t cbLen=-1)     { return this->Read(str, cbLen); }
    bool Pop(char &val)                           { return this->Read(val); }
    bool Pop(byte_t &val)                         { return this->Read(val); }
    template<typename T> bool Pop(T &val)         { return this->Read<T>(val); }

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
	void push_front(const char *buffer);
	void push_front(const void *buffer, size_t cbLen);
	
private:
	BYTE_QUEUE				m_stream;

	bool					m_bPeekMode;
	BYTE_QUEUE::iterator	m_iterPeek;
};

class CByteStreamGuard
{
public:
    explicit CByteStreamGuard(CByteStream &buff);
    ~CByteStreamGuard();

    // 只要调用该函数，则为取消peek-mode
    // 后续对buff的操作均为真实操作
    void skip(uint32_t len);

private:
    CByteStream &m_buff;
    bool        m_bGuard;
};

#endif	// _MYLIB_BASECODE_BYTE_STREAM_H_


