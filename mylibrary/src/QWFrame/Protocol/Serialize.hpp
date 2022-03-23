/*
 * =====================================================================================
 *
 *       Filename:  Serialzie.hpp
 *
 *    Description:  一些常用的序列化宏
 *
 *        Version:  1.0
 *        Created:  2015年09月21日 17时57分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_PROTOCOL_SERIALIZE1_H_
#define _QW_PROTOCOL_SERIALIZE1_H_

#include "protoBase.h"
#include "protoSerialize.h"

template<typename T>
class TSerializor1
{
public:
	uint32_t get_vector_length1(std::vector<T> &vecData);
	int32_t serialize_vector1(std::vector<T> &vecData, int32_t err_result,
			void *pBuf, size_t cbBuf, uint32_t &offset);
	int32_t deserialize_vector1(std::vector<T> &vecData, int32_t err_result,
		void *pBuf, size_t cbBuf, uint32_t &offset);
};


#endif //  _QW_PROTOCOL_SERIALIZE_H_

