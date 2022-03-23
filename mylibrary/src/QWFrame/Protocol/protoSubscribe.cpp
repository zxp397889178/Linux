/*
* =====================================================================================
*
*   Filename:  protoSubscribe.cpp
*
*   Description:  订阅消息协议
*
*   Version:  1.0
*   Created:  2018-06-14
*   Revision:  none
*   Compiler:  gcc
*
*   Author:  meilinfeng
*   Organization:  quwan
*
* =====================================================================================
*/


#include "protoSubscribe.h"
#include "protoSerialize.h"
#include "Serialize.hpp"



protoSubscribeMsgRequest::protoSubscribeMsgRequest()
{
	protoId = IDD;
	protoLen = 0;
	checkCode = 0;

	svrId= 0;
	EMPTY_STRING(svrName);

}

int32_t protoSubscribeMsgRequest::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(svrId, -4);
	SERIALIZE_CHAR(svrName, -5);
    
    TSerializor1<uint32_t> objProtoId;
    ret = objProtoId.serialize_vector1(vecProtoId, -6, pBuf, cbBuf, offset);
    if (ret < 0) return ret;


	GENERATE_CODE;

	return (int32_t)offset;
}

int32_t protoSubscribeMsgRequest::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(svrId, -4);
	DESERIALIZE_CHAR(svrName, -5);

    TSerializor1<uint32_t> objProtoId;
    ret = objProtoId.deserialize_vector1(vecProtoId, -6, pBuf, cbBuf, offset);
    if (ret < 0) return ret;


	return (int32_t)offset;
}

int32_t protoSubscribeMsgRequest::getProtoLen(void)
{
    TSerializor1<uint32_t> objProtoId;
    
	return sizeof(MSG_HEAD)
		+ sizeof(svrId)
		+ LENGTH_STR(svrName)
		+ objProtoId.get_vector_length1(vecProtoId);

}





