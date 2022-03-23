/*
 * =====================================================================================
 *
 *       Filename:  ProtoSerialize.cpp
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

#include <stdlib.h>
#include <algorithm>
#include "protoSerialize.h"
#include "BaseCode.h"
#include "Security.h"

int32_t serialize_data(uint8_t data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(uint8_t)) > cbData)
	{
		return protoBufferOverflow;
	}
	char *pc = (char*)pData + offset;
	*(uint8_t*)pc = data;
	offset += sizeof(uint8_t);
	return protoSucceed;
}

int32_t serialize_data(uint16_t data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(uint16_t)) > cbData)
	{
		return protoBufferOverflow;
	}
	char *pc = (char*)pData + offset;
	*(uint16_t*)pc = CByteOrder::Cast(data);
	offset += sizeof(uint16_t);
	return protoSucceed;
}

int32_t serialize_data(int16_t data, void *pData, size_t cbData, uint32_t &offset)
{
	return serialize_data((uint16_t)data, pData, cbData, offset);
}

int32_t serialize_data(uint32_t data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(uint32_t)) > cbData)
	{
		return protoBufferOverflow;
	}

	char *pc = (char*)pData + offset;
	*(uint32_t*)pc = CByteOrder::Cast(data);
	offset += sizeof(uint32_t);
	return protoSucceed;
}

int32_t serialize_data(int32_t data, void *pData, size_t cbData, uint32_t &offset)
{
	return serialize_data((uint32_t)data, pData, cbData, offset);
}

int32_t serialize_data(uint64_t data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(uint64_t)) > cbData)
	{
		return protoBufferOverflow;
	}

	char *pc = (char*)pData + offset;
	*(uint64_t*)pc = CByteOrder::Cast(data);
	offset += sizeof(uint64_t);
	return protoSucceed;
}

int32_t serialize_data(int64_t data, void *pData, size_t cbData, uint32_t &offset)
{
	return serialize_data((uint64_t)data, pData, cbData, offset);
}

int32_t serialzie_data(float data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(float)) > cbData)
	{
		return protoBufferOverflow;
	}

	char *pc = (char*)pData + offset;
	*(float*)pc = data;
	offset += sizeof(float);
	return protoSucceed;
}

int32_t serialzie_data(double data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(double)) > cbData)
	{
		return protoBufferOverflow;
	}

	char *pc = (char*)pData + offset;
	*(double*)pc = data;
	offset += sizeof(double);
	return protoSucceed;
}

int32_t serialize_data(bool data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(bool)) > cbData)
	{
		return protoBufferOverflow;
	}

	char *pc = (char*)pData + offset;
	*(bool*)pc = data;
	offset += sizeof(bool);
	return protoSucceed;
}

int32_t serialize_data(std::string &data, void *pData, size_t cbData, uint32_t &offset)
{
	uint16_t len = (uint16_t)data.length();
	if ((offset + sizeof(uint16_t) + len) > cbData)
	{
		return protoBufferOverflow;
	}

	char *pc = (char*)pData + offset;
	*(uint16_t*)pc = CByteOrder::Cast(len);
	pc += sizeof(uint16_t);
	offset += sizeof(uint16_t);
	if (len > 0)
	{
		memcpy(pc, data.c_str(), len);
		offset += len;
	}
	return protoSucceed;
}

int32_t serialize_char(const char *data, uint16_t maxLen, void *pData, size_t cbData, uint32_t &offset)
{
	uint16_t len = (uint16_t)strlen(data);
	len = std::min(len, maxLen);
	if ((offset + sizeof(uint16_t) + len) > cbData)
	{
		return protoBufferOverflow;
	}

	char *pc = (char*)pData + offset;
	*(uint16_t*)pc = CByteOrder::Cast(len);
	pc += sizeof(uint16_t);
	offset += sizeof(uint16_t);
	if (len>0)
	{
		memcpy(pc, data, len);
		offset += len;
	}
	return protoSucceed;
}

int32_t serialize_data(void *data, uint32_t dataLen, void *pData, size_t cbData, uint32_t &offset)
{
	// LogTrace("offset %u, dataLen %u, bufferLen %u", offset, dataLen, cbData);
	if ((offset + dataLen) > cbData)
	{
		return protoBufferOverflow;
	}
	char *pc = (char*)pData+offset;
	// LogTrace("serialize void*, pc=%p, pData=%p, offset=%u, dataLen=%u", pc, pData, offset, dataLen);
	memcpy((void*)pc, data, dataLen);
	offset += dataLen;
	return protoSucceed;
}

int32_t serialize_data(std::string &data, uint16_t maxLen, void *pData, size_t cbData, uint32_t &offset)
{
	uint16_t len = std::max(maxLen, (uint16_t)data.length());
	if ((offset + sizeof(uint16_t) + len) > cbData)
	{
		return protoBufferOverflow;
	}

	char *pc = (char*)pData + offset;
	*(uint16_t*)pc = CByteOrder::Cast(len);
	pc += sizeof(uint16_t);
	offset += sizeof(uint16_t);
	if (len > 0)
	{
		memcpy(pc, data.c_str(), len);
		offset += len;
	}
	return protoSucceed;
}

int32_t serialize_data(struct MSG_HEAD &head, void *pData, size_t cbData, uint32_t &offset)
{
	uint16_t len = (uint16_t)sizeof(struct MSG_HEAD); 
	if ((offset + sizeof(uint16_t) + len) > cbData)
	{
		return protoBufferOverflow;
	}

	char *pc = (char*)pData + offset;
	struct MSG_HEAD *pHead = (struct MSG_HEAD*)pc;
	pHead->protoLen = CByteOrder::Cast(head.protoLen);
	pHead->protoId = CByteOrder::Cast(head.protoId);
	pHead->checkCode = CByteOrder::Cast(head.checkCode);
	offset += len;
	return protoSucceed;
}

int32_t serialize_file_data(FILE *fp, uint32_t maxLen, void *pData, size_t cbData, uint32_t &offset)
{
	fseek(fp, 0, SEEK_END);
	uint32_t fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	uint32_t fileLen = std::min(fileSize, maxLen);
	if (offset + fileLen > cbData)
	{
		printf("offset %u, fileLen %u, cbData %lu\n", offset, fileLen, cbData);
		return protoBufferOverflow;
	}

	char *pc = (char*)pData + offset;	
	fread(pc, 1, fileLen, fp);
	offset += fileLen;

	return protoSucceed;
}

///////////////////////////////////////////////////////////////////////////
//
int32_t deserialize_data(uint8_t &data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(uint8_t)) > cbData)
	{
		return protoBufferUnderflow;
	}

	char *pc = (char*)pData + offset;
	data = *(uint8_t*)pc;
	offset += sizeof(uint8_t);
	return protoSucceed;
}

int32_t deserialize_data(uint16_t &data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(uint16_t)) > cbData)
	{
		return protoBufferUnderflow;
	}

	char *pc = (char*)pData + offset;
	data = *(uint16_t*)pc;
	data = CByteOrder::Cast(data);
	offset += sizeof(uint16_t);
	return protoSucceed;
}

int32_t deserialize_data(uint32_t &data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(uint32_t)) > cbData)
	{
		return protoBufferUnderflow;
	}

	char *pc = (char*)pData + offset;
	data = *(uint32_t*)pc;
	data = CByteOrder::Cast(data);
	offset += sizeof(uint32_t);
	return protoSucceed;
}

int32_t deserialize_data(uint64_t &data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(uint64_t)) > cbData)
	{
		return protoBufferUnderflow;
	}

	char *pc = (char*)pData + offset;
	data = *(uint64_t*)pc;
	data = CByteOrder::Cast(data);
	offset += sizeof(uint64_t);
	return protoSucceed;
}

int32_t deserialize_data(int16_t &data, void *pData, size_t cbData, uint32_t &offset)
{
	uint16_t val(0);
	int32_t ret = deserialize_data(val, pData, cbData, offset);
	if (ret != protoSucceed)
	{
		return ret;
	}
	data = (int16_t)val;
	return protoSucceed;
}

int32_t deserialize_data(int32_t &data, void *pData, size_t cbData, uint32_t &offset)
{
	uint32_t val(0);
	int32_t ret = deserialize_data(val, pData, cbData, offset);
	if (ret != protoSucceed)
	{
		return ret;
	}
	data = (int32_t)val;
	return protoSucceed;
}

int32_t deserialize_data(int64_t &data, void *pData, size_t cbData, uint32_t &offset)
{
	uint64_t val(0);
	int32_t ret = deserialize_data(val, pData, cbData, offset);
	if (ret != protoSucceed)
	{
		return ret;
	}
	data = (int64_t)val;
	return protoSucceed;
}

int32_t deserialize_data(float &data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(float)) > cbData)
	{
		return protoBufferUnderflow;
	}

	char *pc = (char*)pData + offset;
	data = *(float*)pc;
	offset += sizeof(float);
	return protoSucceed;
}

int32_t deserialize_data(double &data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(double)) > cbData)
	{
		return protoBufferUnderflow;
	}

	char *pc = (char*)pData + offset;
	data = *(double*)pc;
	offset += sizeof(double);
	return protoSucceed;
}

int32_t deserialize_data(bool &data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(bool)) > cbData)
	{
		return protoBufferUnderflow;
	}

	char *pc = (char*)pData + offset;
	data = *(bool*)pc;
	offset += sizeof(bool);
	return protoSucceed;
}

int32_t deserialize_data(std::string &data, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(uint16_t)) > cbData)
	{
		return protoBufferUnderflow;
	}

	uint16_t len(0);
	char *pc = (char*)pData + offset;
	len = *(uint16_t*)pc;
	len = CByteOrder::Cast(len);
	if ((offset + sizeof(uint16_t) + len) > cbData)
	{
		return protoBufferUnderflow;
	}
	pc += sizeof(uint16_t);
	offset += sizeof(uint16_t);
	if (len > 0)
	{
		data.assign( pc, len);
		offset += len;
	}
	return protoSucceed;
}

int32_t deserialize_data(std::string &data, uint16_t maxLen, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + sizeof(uint16_t)) > cbData)
	{
		return protoBufferUnderflow;
	}

	uint16_t len(0);
	char *pc = (char*)pData + offset;
	len = *(uint16_t*)pc;
	len = CByteOrder::Cast(len);
	if ((offset + sizeof(uint16_t) + len) > cbData)
	{
		return protoBufferUnderflow;
	}
	pc += sizeof(uint16_t);
	offset += sizeof(uint16_t);
	
	uint16_t bufLen = std::min(len, maxLen);
	if (bufLen > 0)
	{
		data.assign( pc, bufLen);
		offset += len;
	}
	return protoSucceed;
}

int32_t deserialize_char(char *data, uint16_t maxLen, void *pData, size_t cbData, uint32_t &offset)
{
	memset(data, 0, maxLen);
	if ((offset + sizeof(uint16_t)) > cbData)
	{
		return protoBufferUnderflow;
	}

	uint16_t len(0);
	char *pc = (char*)pData + offset;
	len = *(uint16_t*)pc;
	len = CByteOrder::Cast(len);
	if ((offset + sizeof(uint16_t) + len) > cbData)
	{
		return protoBufferUnderflow;
	}
	pc += sizeof(uint16_t);
	offset += sizeof(uint16_t);
	uint16_t bufLen = std::min(len, maxLen);
	if (bufLen > 0)
	{
		memcpy(data, pc, bufLen);
		offset += len;
	}
	return protoSucceed;
}

int32_t deserialize_data(void *data, uint32_t dataLen, void *pData, size_t cbData, uint32_t &offset)
{
	if ((offset + dataLen) > cbData)
	{
		return protoBufferUnderflow;
	}

	char *pc = (char*)pData + offset;
	memcpy(data, pc, dataLen);
	offset += dataLen;
	return protoSucceed;
}

int32_t deserialize_data(struct MSG_HEAD &head, void *pData, size_t cbData, uint32_t &offset)
{
	int ret = deserialize_data((void*)&head, sizeof(struct MSG_HEAD), pData, cbData, offset);
	if (ret != protoSucceed)
	{
		return ret;
	}
	head.protoLen = CByteOrder::Cast(head.protoLen);
	head.protoId = CByteOrder::Cast(head.protoId);
	head.checkCode = CByteOrder::Cast(head.checkCode);
	return protoSucceed;
}

int32_t deserialize_file_data(FILE *fp, uint32_t maxLen, void *pData, size_t cbData, uint32_t &offset)
{
	char *pc = (char*)pData + offset;
	if (offset > cbData)
	{
		return protoBufferUnderflow;
	}
	else if (offset == cbData || maxLen==0)
	{
		return protoBufferEmpty;
	}
	uint32_t left = cbData - offset;
	uint32_t bufLen = std::min(left, maxLen);
	
	fwrite(pc, 1, bufLen, fp);
	fflush(fp);
	offset += bufLen;

	return protoSucceed;
}

/////////////////////////////////////////////////////////////////////////////
//CRC 算法
/* 
 * 计算：
 * 1. 对整个包按网络字节序转换后，将crcCode 置为预设值 pre_data
 * 2. 对整个包进行CRC64计算，得到值 val1
 * 3. 对 val1 转换成网络字节序，得到 val2
 * 4. 将 val2 填回到 crcCode
 *
 * 验证
 * 1. 取出 crcCode => val1
 * 2. 对 val1 的字节序按主机字节序取值 => val2
 * 3. 将 crcCode 置为预设值 pre_data
 * 4. 对整个包进行CRC64计算,获得 val3
 * 5. 比较 val2 和 val3，相等则说明匹配; 否则不匹配
 * */

static void GenerateCheckCode(struct MSG_HEAD *pHead, uint64_t defCode)
{
	uint32_t bufLen = CByteOrder::Cast(pHead->protoLen);	
	pHead->checkCode = CByteOrder::Cast(defCode);
	//printf("buffer [%p] len [%u] [%u]\n", pHead, bufLen, pHead->protoLen);
	//uint64_t val1 = CRC64((byte_t*)pHead, bufLen);
    uint64_t val1 = CCrcWrapper::Encode64((uint8_t*)pHead, bufLen);
	uint64_t val2 = CByteOrder::Cast(val1);
	pHead->checkCode = val2;
}

static bool IsCheckCodeValid(struct MSG_HEAD *pHead, uint64_t defCode)
{
	uint64_t val1 = pHead->checkCode;
	uint64_t val2 = CByteOrder::Cast(val1);
	pHead->checkCode = CByteOrder::Cast(defCode);
	uint32_t bufLen = CByteOrder::Cast(pHead->protoLen);
	//uint64_t val3 = CRC64((byte_t*)pHead, bufLen);
    uint64_t val3 = CCrcWrapper::Encode64((uint8_t*)pHead, bufLen);
	pHead->checkCode = val1;
	return (val2 == val3);
}

#define DEFAULT_CHECK_CODE 	0x012D39BB01337825
void GenerateCheckCode(void *pBuf)
{
	GenerateCheckCode((struct MSG_HEAD*)pBuf, DEFAULT_CHECK_CODE);
}

bool IsCheckCodeValid(void *pBuf)
{
	return IsCheckCodeValid((struct MSG_HEAD*)pBuf, DEFAULT_CHECK_CODE);
}


