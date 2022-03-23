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

#include "protoBase.h"

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
int32_t serialzie_data(float data,            void *pData, size_t cbData, uint32_t &offset);
int32_t serialzie_data(double data,           void *pData, size_t cbData, uint32_t &offset);
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
