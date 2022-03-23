/*
 * =====================================================================================
 *
 *       Filename:  Serialize.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年09月26日 21时03分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "Serialize.hpp"
#include "protoUserData.h"

template<typename T>
uint32_t TSerializor1<T>::get_vector_length1(std::vector<T> &vecData)
{
	typedef typename std::vector<T>::iterator	ITERATOR;

	uint32_t dataLen(0);
	ITERATOR it(vecData.begin());
	for(; it != vecData.end(); ++it)
	{
		T &item = *it;
		dataLen += item.getLength();
	}
	return dataLen + sizeof(uint32_t);
}
template<typename T>
int32_t TSerializor1<T>::serialize_vector1(std::vector<T> &vecData, int32_t err_result,
		void *pBuf, size_t cbBuf, uint32_t &offset)
{
	typedef typename std::vector<T>::iterator	ITERATOR;

	uint32_t vecCount = (uint32_t)vecData.size();
	int32_t ret(0);
	SERIALIZE_DATA(vecCount, err_result);
	ITERATOR it(vecData.begin());
	for(; it != vecData.end(); ++it)
	{
		T &item = *it;
		int32_t ret =item.Serialize(pBuf, cbBuf, offset);
		if (ret < 0) return ret;
	}
	return (int32_t)offset;
}
template<typename T>
int32_t TSerializor1<T>::deserialize_vector1(std::vector<T> &vecData, int32_t err_result,
		void *pBuf, size_t cbBuf, uint32_t &offset)
{
	uint32_t vecCount(0);
	int32_t ret(0);
	DESERIALIZE_DATA(vecCount, err_result);
	for(uint32_t index(0); index<vecCount; ++index)
	{
		T item;
		int32_t ret = item.Deserialize(pBuf, cbBuf, offset);
		if (ret < 0) return ret;
		vecData.push_back(item);
	}
	return (int32_t)offset;
}

///////////////////////////////////////////////////////////////////////////
template<>
uint32_t TSerializor1<uint32_t>::get_vector_length1(std::vector<uint32_t> &vecData)
{
	return sizeof(uint32_t) + vecData.size()*sizeof(uint32_t);
}
template<>
int32_t TSerializor1<uint32_t>::serialize_vector1(std::vector<uint32_t> &vecData, int32_t err_result,
		void *pBuf, size_t cbBuf, uint32_t &offset)
{
	uint32_t vecCount = (uint32_t)vecData.size();
	int32_t ret(0);
	SERIALIZE_DATA(vecCount, err_result);
	std::vector<uint32_t>::iterator it(vecData.begin());
	for(; it != vecData.end(); ++it)
	{
		uint32_t val = *it;
		SERIALIZE_DATA(val, err_result-1);
	}
	return (int32_t)offset;
}
template<>
int32_t TSerializor1<uint32_t>::deserialize_vector1(std::vector<uint32_t> &vecData, int32_t err_result,
		void *pBuf, size_t cbBuf, uint32_t &offset)
{
	uint32_t vecCount(0);
	int32_t ret(0);
	DESERIALIZE_DATA(vecCount, err_result);
	for(uint32_t index(0); index<vecCount; ++index)
	{
		uint32_t val(0);
		DESERIALIZE_DATA(val, err_result-1);
		vecData.push_back(val);
	}
	return (int32_t)offset;
}

///////////////////////////////////////////////////////////////////////////
template<>
uint32_t TSerializor1<string>::get_vector_length1(std::vector<string> &vecData)
{
	uint32_t len = sizeof(uint32_t);
	for (std::vector<string>::iterator iter = vecData.begin(); iter != vecData.end(); ++iter)
	{
		len += sizeof(uint16_t);
		len += iter->length();
	}
	return len;
}
template<>
int32_t TSerializor1<string>::serialize_vector1(std::vector<string> &vecData, int32_t err_result,
	void *pBuf, size_t cbBuf, uint32_t &offset)
{
	uint32_t vecCount = (uint32_t)vecData.size();
	int32_t ret(0);
	SERIALIZE_DATA(vecCount, err_result);
	std::vector<string>::iterator it(vecData.begin());
	for (; it != vecData.end(); ++it)
	{
		string &val = *it;
		SERIALIZE_DATA(val, err_result - 1);
	}
	return (int32_t)offset;
}
template<>
int32_t TSerializor1<string>::deserialize_vector1(std::vector<string> &vecData, int32_t err_result,
	void *pBuf, size_t cbBuf, uint32_t &offset)
{
	uint32_t vecCount(0);
	int32_t ret(0);
	DESERIALIZE_DATA(vecCount, err_result);
	for (uint32_t index(0); index<vecCount; ++index)
	{
		string val;
		DESERIALIZE_DATA(val, err_result - 1);
		vecData.push_back(val);
	}
	return (int32_t)offset;
}


///////////////////////////////////////////////////////////////////////////
//http://my.oschina.net/shewa/blog/206124
//必须显式的实例化类型，否则会链接出错

template class TSerializor1<uint32_t>;
template class TSerializor1<string>;
template class TSerializor1<CurrencyInfo>;
template class TSerializor1<UserItemInfo>;
template class TSerializor1<UserWearItemInfo>;
template class TSerializor1<UserFishInfo>;
template class TSerializor1<UserContiTaskInfo>;

