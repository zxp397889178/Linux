/*
 * ==================================================================
 *
 *       Filename:  ProtoSerializeEx.cpp
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
 * 参考URL: https://blog.csdn.net/bookscape/article/details/84487810
 *
 * ==================================================================
 */

#include <stdlib.h>
#include <algorithm>
#include <typeinfo>

#include "protoSerializeEx.h"
#include "Security.h"

/////////////////////////////////////////////////////////////////////
//CSerializor

CSerializor::CSerializor(void *pData, size_t cbData)
    : m_pData(pData), m_cbData(cbData), m_offset(0)
{

}

CSerializor::~CSerializor()
{

}

////////////////////////////////////////////////////////////////////
//CSerializorEx
//

CSerializorEx::CSerializorEx(CByteStream &buff)
    : m_buffer(buff)
{

}

CSerializorEx::~CSerializorEx()
{

}


#include "protoSerializeEx.inl"

/////////////////////////////////////////////////////////////////////////////
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

/*
static void GenerateCheckCode(struct MSG_HEAD *pHead, uint64_t defCode)
{
	uint32_t bufLen = CByteOrder::Cast(pHead->protoLen);	
	pHead->checkCode = CByteOrder::Cast(defCode);
	//printf("buffer [%p] len [%u] [%u]\n", pHead, bufLen, pHead->protoLen);
	uint64_t val1 = CCrcWrapper::Encode64((byte_t*)pHead, bufLen);
	uint64_t val2 = CByteOrder::Cast(val1);
	pHead->checkCode = val2;
}

static bool IsCheckCodeValid(struct MSG_HEAD *pHead, uint64_t defCode)
{
	uint64_t val1 = pHead->checkCode;
	uint64_t val2 = CByteOrder::Cast(val1);
	pHead->checkCode = CByteOrder::Cast(defCode);
	uint32_t bufLen = CByteOrder::Cast(pHead->protoLen);
	uint64_t val3 = CCrcWrapper::Encode64((byte_t*)pHead, bufLen);
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
*/

