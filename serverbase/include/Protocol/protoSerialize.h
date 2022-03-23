/*
 * =====================================================================================
 *
 *       Filename:  ProtoSerialize.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/12/2015 02:58:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _GFX_PROTO_SERIALIZE_FUNC_H_
#define _GFX_PROTO_SERIALIZE_FUNC_H_

#include <stdint.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include "BaseCode/mydef.h"
#include "protoBase.h"

#define CRC32(buf, size)	CMyCrc::Calc32Crc((buf), (size))
#define CRC64(buf, size)	CMyCrc::Calc64Crc((buf), (size))

namespace crc_32 {

typedef 	 uint32_t CRC_DATA_TYPE;
extern const uint32_t crc32_table[256];


/* Initialize a CRC accumulator */
__inline void Init_crc(uint32_t & crc)
{
	crc = 0xffffffffUL;
}

/* Finish a CRC calculation */
__inline void Fin_crc(uint32_t & crc)
{
	crc ^= 0xffffffffUL;
}

/* Accumulate some (more) bytes into a CRC */
__inline void Calc_crc(uint32_t & crc, byte_t* pbData, size_t len)
{
#define DO1(buf) crc = crc32_table[(crc ^ (*pbData++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

	while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if (len) 
	{
		do 
		{
			DO1(buf);
		} while (--len);
	}
}


} // namespace crc32

namespace crc64 {

/*
 * If we have a 64-bit integer type, then a 64-bit CRC looks just like the
 * usual sort of implementation.  (See Ross Williams' excellent introduction
 * A PAINLESS GUIDE TO CRC ERROR DETECTION ALGORITHMS, available from
 * ftp://ftp.rocksoft.com/papers/crc_v3.txt or several other net sites.)
 * If we have no working 64-bit type, then fake it with two 32-bit registers.
 *
 * The present implementation is a normal (not "reflected", in Williams'
 * terms) 64-bit CRC, using initial all-ones register contents and a final
 * bit inversion.  The chosen polynomial is borrowed from the DLT1 spec
 * (ECMA-182, available from http://www.ecma.ch/ecma1/STAND/ECMA-182.HTM):
 *
 * x^64 + x^62 + x^57 + x^55 + x^54 + x^53 + x^52 + x^47 + x^46 + x^45 +
 * x^40 + x^39 + x^38 + x^37 + x^35 + x^33 + x^32 + x^31 + x^29 + x^27 +
 * x^24 + x^23 + x^22 + x^21 + x^19 + x^17 + x^13 + x^12 + x^10 + x^9 +
 * x^7 + x^4 + x + 1
 */

// Imported from c.h
/* Decide if we need to decorate 64-bit constants */
#define INT64CONST(x)  ((int64_t) x##LL)
#define UINT64CONST(x) ((uint64_t) x##ULL)

typedef uint64_t CRC_DATA_TYPE;

extern const uint64_t crc64_table[256];

/* Initialize a CRC accumulator */
__inline void Init_crc(uint64_t & crc)
{
	crc = UINT64CONST(0xffffffffffffffff);
}

/* Finish a CRC calculation */
__inline void Fin_crc(uint64_t & crc)
{
	crc ^= UINT64CONST(0xffffffffffffffff);
}

/* Accumulate some (more) bytes into a CRC */
__inline void Calc_crc(uint64_t & crc, byte_t* pbData, size_t len)
{
	/* Constant table for CRC calculation */

	byte_t*		__pbData = pbData;
	size_t		__len = len;

	while (__len-- > 0)
	{
		uint32_t __tab_index = ((uint32_t) (crc >> 56) ^ *__pbData++) & 0xFF;
		crc = crc64_table[__tab_index] ^ (crc << 8);
	}
}


} // namespace crc64

class CMyCrc
{
public:
	CMyCrc();
	~CMyCrc();

public:
	static uint32_t Calc32Crc(byte_t *pBuf, size_t cbBuf);
	static uint64_t Calc64Crc(byte_t *pBuf, size_t cbBuf);

	static int32_t Calc32Crc_File(const char *pszFile, uint32_t &crc);
	static int32_t Calc64Crc_File(const char *pszFile, uint64_t &crc64);
};

enum protoSerializeError
{
	protoSucceed 			= 0,
	protoBufferOverflow 	= -1,	// 内存上溢出
	protoBufferUnderflow 	= -2,	// 内存下溢出
	protoBufferEmpty 		= -3
};

// 序列化函数
int32_t serialize_data(uint8_t data,          void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_data(uint16_t data,         void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_data(uint32_t data,         void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_data(uint64_t data,         void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_data(int16_t data,          void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_data(int32_t data,          void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_data(int64_t data,          void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_data(float data, void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_data(double data, void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_data(bool data, void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_data(struct MSG_HEAD &head, void *pData, size_t cbData, uint32_t &offset);

int32_t serialize_data(std::string &data,     				void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_data(std::string &data, uint16_t maxLen,  void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_data(void *data,        uint32_t dataLen, void *pData, size_t cbData, uint32_t &offset);

int32_t serialize_char(const char *data,  uint16_t maxLen, void *pData, size_t cbData, uint32_t &offset);
int32_t serialize_file_data(FILE *fp,     uint32_t maxLen,  void *pData, size_t cbData, uint32_t &offset);

// 反序列化函数
int32_t deserialize_data(uint8_t &data, 		void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_data(uint16_t &data, 		void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_data(uint32_t &data, 		void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_data(uint64_t &data, 		void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_data(int16_t &data, 		void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_data(int32_t &data, 		void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_data(int64_t &data, 		void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_data(float &data, 			void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_data(double &data, 			void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_data(bool &data, void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_data(struct MSG_HEAD &head, void *pData, size_t cbData, uint32_t &offset);

int32_t deserialize_data(std::string &data, 	void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_data(std::string &data, uint16_t maxLen,   void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_data(void *data, 	    uint32_t dataLen,  void *pData, size_t cbData, uint32_t &offset);

int32_t deserialize_char(char *data, 	    uint16_t maxLen, void *pData, size_t cbData, uint32_t &offset);
int32_t deserialize_file_data(FILE *fp, 	uint32_t maxLen,    void *pData, size_t cbData, uint32_t &offset);

// 消息编码结束后做校验码生成
void GenerateCheckCode(void *pBuf);
// 收到消息后立即做校验码检查
bool IsCheckCodeValid(void *pBuf);

//////////////////////////////////////////////////////////////////////////////
//常用宏定义
#define SERIALIZE_DATA(element, result) 	{ret=serialize_data((element), pBuf, cbBuf, offset); if (ret!=0) return (result); }
#define DESERIALIZE_DATA(element, result) 	{ ret=deserialize_data((element), pBuf, cbBuf, offset); if (ret!=0) return (result); }

#define GENERATE_CODE	GenerateCheckCode(pBuf)

#define SERIALIZE_CHAR(element, result) 	{ret=serialize_char((element),sizeof((element)), pBuf, cbBuf, offset); if (ret!=0) return (result); }
#define DESERIALIZE_CHAR(element, result) 	{ret=deserialize_char((element),sizeof((element)), pBuf, cbBuf, offset); if (ret!=0) return (result); }

#define SERIALIZE_BUF(element, len, result) 	{ret=serialize_data((void*)(element),len, pBuf, cbBuf, offset); if (ret!=0) return (result); }
#define DESERIALIZE_BUF(element, len, result) 	{ret=deserialize_data((void*)(element),len, pBuf, cbBuf, offset); if (ret!=0) return (result); }

#define EMPTY_STRING(str)		memset((void*)&str[0], 0, sizeof(str))
#define LENGTH_STR(str)			sizeof(uint16_t)+strlen((str))

#define INIT_SERIALIZE_ENV		uint32_t offset(0);\
								int32_t	ret(0);\
								protoLen = this->getProtoLen();\
								SERIALIZE_DATA(protoLen,-1);\
								SERIALIZE_DATA(protoId,-2); \
								SERIALIZE_DATA(checkCode,-3);

#define INIT_DESERIALIZE_ENV 	uint32_t offset(0);\
								int32_t ret(0);\
								DESERIALIZE_DATA(protoLen,-1); \
								DESERIALIZE_DATA(protoId,-2); \
								DESERIALIZE_DATA(checkCode,-3);

#endif // _GFX_PROTO_SERIALIZE_FUNC_H_
