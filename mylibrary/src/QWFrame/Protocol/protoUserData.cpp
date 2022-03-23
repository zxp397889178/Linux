/*
* =====================================================================================
*
*       Filename:  protoUserData.cpp
*
*    Description:  �û�����Э��
*
*        Version:  1.0
*        Created:  2018��1��29��
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  none
*
* =====================================================================================
*/

#include "protoUserData.h"

/*�û�����*/
UserItemInfo::UserItemInfo()
: itemId(0), itemTypeId(0), num(0)
{
	isWeared = 0;
}

uint32_t UserItemInfo::getLength(void)
{
	return sizeof(itemId)
		+sizeof(itemTypeId)
		+sizeof(num)
		+ sizeof(isWeared);
}

int32_t UserItemInfo::Serialize(void *pBuf, size_t cbBuf, uint32_t &offset)
{
	int32_t ret(0);

	SERIALIZE_DATA(itemId, -11);
	SERIALIZE_DATA(itemTypeId, -12);
	SERIALIZE_DATA(num, -13);
	SERIALIZE_DATA(isWeared, -15);

	return (int32_t)offset;
}

int32_t UserItemInfo::Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset)
{
	int32_t ret(0);

	DESERIALIZE_DATA(itemId, -11);
	DESERIALIZE_DATA(itemTypeId, -12);
	DESERIALIZE_DATA(num, -13);
	DESERIALIZE_DATA(isWeared, -15);

	return (int32_t)offset;
}

//////////////////////////////////////////////////////////////////////////////////////////

/*�û�װ����Ϣ*/
UserWearItemInfo::UserWearItemInfo()
{
	itemTypeId = 0;
	status = 0;
	wearTime = 0;
	expireTime = 0;
}

uint32_t UserWearItemInfo::getLength(void)
{
	return sizeof(itemTypeId)
		+sizeof(status)
		+sizeof(wearTime)
		+sizeof(expireTime);
}

int32_t UserWearItemInfo::Serialize(void *pBuf, size_t cbBuf, uint32_t &offset)
{
	int32_t ret(0);

	SERIALIZE_DATA(itemTypeId, -11);
	SERIALIZE_DATA(status, -12);
	SERIALIZE_DATA(wearTime, -13);
	SERIALIZE_DATA(expireTime, -15);

	return (int32_t)offset;
}

int32_t UserWearItemInfo::Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset)
{
	int32_t ret(0);

	DESERIALIZE_DATA(itemTypeId, -11);
	DESERIALIZE_DATA(status, -12);
	DESERIALIZE_DATA(wearTime, -13);
	DESERIALIZE_DATA(expireTime, -15);

	return (int32_t)offset;
}

//////////////////////////////////////////////////////////////////////////////////////////

/*�û�������Ϣ*/
UserFishInfo::UserFishInfo()
{
	roomId = 0;
	totalPutScore = 0;
	totalGetScore = 0;
	lostScore = 0;
}

uint32_t UserFishInfo::getLength(void)
{
	return sizeof(roomId)
		+sizeof(totalPutScore)
		+sizeof(totalGetScore)
		+sizeof(lostScore);
}

int32_t UserFishInfo::Serialize(void *pBuf, size_t cbBuf, uint32_t &offset)
{
	int32_t ret(0);

	SERIALIZE_DATA(roomId, -11);
	SERIALIZE_DATA(totalPutScore, -12);
	SERIALIZE_DATA(totalGetScore, -13);
	SERIALIZE_DATA(lostScore, -15);

	return (int32_t)offset;
}

int32_t UserFishInfo::Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset)
{
	int32_t ret(0);

	DESERIALIZE_DATA(roomId, -11);
	DESERIALIZE_DATA(totalPutScore, -12);
	DESERIALIZE_DATA(totalGetScore, -13);
	DESERIALIZE_DATA(lostScore, -15);

	return (int32_t)offset;
}

//////////////////////////////////////////////////////////////////////////////////////////

/*�û�����������Ϣ*/
UserContiTaskInfo::UserContiTaskInfo()
{
	roomId = 0;
	taskId = 0;
	taskNum = 0;
	EMPTY_STRING(completeValue);
	status = 0;
	gainCoin = 0;
	costCoin = 0;
}

uint32_t UserContiTaskInfo::getLength(void)
{
	return sizeof(roomId)
		+sizeof(taskId)
		+sizeof(taskNum)
		+LENGTH_STR(completeValue)
		+sizeof(status)
		+sizeof(gainCoin)
		+sizeof(costCoin);
}

int32_t UserContiTaskInfo::Serialize(void *pBuf, size_t cbBuf, uint32_t &offset)
{
	int32_t ret(0);

	SERIALIZE_DATA(roomId, -11);
	SERIALIZE_DATA(taskId, -12);
	SERIALIZE_DATA(taskNum, -13);
	SERIALIZE_CHAR(completeValue, -15);
	SERIALIZE_DATA(status, -16);
	SERIALIZE_DATA(gainCoin, -17);
	SERIALIZE_DATA(costCoin, -18);

	return (int32_t)offset;
}

int32_t UserContiTaskInfo::Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset)
{
	int32_t ret(0);

	DESERIALIZE_DATA(roomId, -11);
	DESERIALIZE_DATA(taskId, -12);
	DESERIALIZE_DATA(taskNum, -13);
	DESERIALIZE_CHAR(completeValue, -15);
	DESERIALIZE_DATA(status, -16);
	DESERIALIZE_DATA(gainCoin, -17);
	DESERIALIZE_DATA(costCoin, -18);

	return (int32_t)offset;
}


//////////////////////////////////////////////////////////////////////////////////////////

protoUserInfoRequest::protoUserInfoRequest()
{
	protoId = IDD;
	protoLen = 0;
	checkCode = 0;

	uin = 0;
	roomId = 0;
}

int32_t protoUserInfoRequest::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(uin, -4);
	SERIALIZE_DATA(roomId, -5);

	GENERATE_CODE;

	return (int32_t)offset;
}

int32_t protoUserInfoRequest::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(uin, -4);
	DESERIALIZE_DATA(roomId, -5);

	return (int32_t)offset;
}

int32_t protoUserInfoRequest::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+sizeof(uin)
		+sizeof(roomId);
}


//////////////////////////////////////////////////////////////////////////////////////////

protoUserInfoResponse::protoUserInfoResponse()
{
	protoId = IDD;
	protoLen = 0;
	checkCode = 0;

	uin = 0;
	roomId = 0;
	EMPTY_STRING(nick);
	vip = 0;
	level = 0;
	exp = 0;
	coin = 0;
	ticket = 0;
	money = 0;
	trumpet = 0;
	bankCoin = 0;

	totalPutScore = 0;
	totalGetScore = 0;
	lostScore = 0;

	language = 0;
	isSimulator = false;
	
}

int32_t protoUserInfoResponse::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(uin, -4);
	SERIALIZE_DATA(roomId, -5);
	SERIALIZE_CHAR(nick, -6);
	SERIALIZE_DATA(vip, -7);
	SERIALIZE_DATA(level, -8);
	SERIALIZE_DATA(exp, -9);
	SERIALIZE_DATA(coin, -10);
	SERIALIZE_DATA(ticket, -11);
	SERIALIZE_DATA(money, -12);
	SERIALIZE_DATA(trumpet, -13);
	SERIALIZE_DATA(bankCoin, -14);

	TSerializor1<UserItemInfo> obj;

	ret = obj.serialize_vector1(vecUserItemInfo, -15, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	SERIALIZE_DATA(totalPutScore, -16);
	SERIALIZE_DATA(totalGetScore, -17);
	SERIALIZE_DATA(lostScore, -18);

	TSerializor1<UserWearItemInfo> obj1;

	ret = obj1.serialize_vector1(vecUserWearItemInfo, -19, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	SERIALIZE_DATA(language, -19);
	SERIALIZE_DATA(isSimulator, -20);


	GENERATE_CODE;

	return (int32_t)offset;
}

int32_t protoUserInfoResponse::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(uin, -4);
	DESERIALIZE_DATA(roomId, -5);
	DESERIALIZE_CHAR(nick, -6);
	DESERIALIZE_DATA(vip, -7);
	DESERIALIZE_DATA(level, -8);
	DESERIALIZE_DATA(exp, -9);
	DESERIALIZE_DATA(coin, -10);
	DESERIALIZE_DATA(ticket, -11);
	DESERIALIZE_DATA(money, -12);
	DESERIALIZE_DATA(trumpet, -13);
	DESERIALIZE_DATA(bankCoin, -14);

	TSerializor1<UserItemInfo> obj;

	ret = obj.deserialize_vector1(vecUserItemInfo, -15, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	DESERIALIZE_DATA(totalPutScore, -16);
	DESERIALIZE_DATA(totalGetScore, -17);
	DESERIALIZE_DATA(lostScore, -18);

	TSerializor1<UserWearItemInfo> obj1;

	ret = obj1.deserialize_vector1(vecUserWearItemInfo, -19, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	DESERIALIZE_DATA(language, -19);
	DESERIALIZE_DATA(isSimulator, -20);

	return (int32_t)offset;
}

int32_t protoUserInfoResponse::getProtoLen(void)
{
	TSerializor1<UserItemInfo> obj;
	TSerializor1<UserWearItemInfo> obj1;
	return sizeof(MSG_HEAD)
		+sizeof(uin)
		+sizeof(roomId)
		+LENGTH_STR(nick)
		+ sizeof(vip)
		+sizeof(level)
		+sizeof(exp)
		+sizeof(coin)
		+sizeof(ticket)
		+sizeof(money)
		+sizeof(trumpet)
		+sizeof(bankCoin)
		+obj.get_vector_length1(vecUserItemInfo)
		+ sizeof(totalPutScore)
		+sizeof(totalGetScore)
		+sizeof(lostScore)
		+obj1.get_vector_length1(vecUserWearItemInfo)
		+ sizeof(language)
		+sizeof(isSimulator);
}


//////////////////////////////////////////////////////////////////////////////////////////

protoSvrUserLeaveRoomRequest::protoSvrUserLeaveRoomRequest()
{
	protoId = IDD;
	protoLen = 0;
	checkCode = 0;

	uin = 0;
	roomId = 0;
	level = 0;
	exp = 0;
	coin = 0;
	ticket = 0;
	money = 0;

	totalPutScore = 0;
	totalGetScore = 0;
	lostScore = 0;
}

int32_t protoSvrUserLeaveRoomRequest::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(uin, -4);
	SERIALIZE_DATA(roomId, -5);
	SERIALIZE_DATA(level, -8);
	SERIALIZE_DATA(exp, -9);
	SERIALIZE_DATA(coin, -10);
	SERIALIZE_DATA(ticket, -11);
	SERIALIZE_DATA(money, -12);

	TSerializor1<UserItemInfo> obj;

	ret = obj.serialize_vector1(vecUserItemInfo, -14, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	SERIALIZE_DATA(totalPutScore, -15);
	SERIALIZE_DATA(totalGetScore, -16);
	SERIALIZE_DATA(lostScore, -17);

	TSerializor1<UserWearItemInfo> obj1;

	ret = obj1.serialize_vector1(vecUserWearItemInfo, -19, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	GENERATE_CODE;

	return (int32_t)offset;
}

int32_t protoSvrUserLeaveRoomRequest::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(uin, -4);
	DESERIALIZE_DATA(roomId, -5);
	DESERIALIZE_DATA(level, -8);
	DESERIALIZE_DATA(exp, -9);
	DESERIALIZE_DATA(coin, -10);
	DESERIALIZE_DATA(ticket, -11);
	DESERIALIZE_DATA(money, -12);

	TSerializor1<UserItemInfo> obj;

	ret = obj.deserialize_vector1(vecUserItemInfo, -14, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	DESERIALIZE_DATA(totalPutScore, -15);
	DESERIALIZE_DATA(totalGetScore, -16);
	DESERIALIZE_DATA(lostScore, -17);

	TSerializor1<UserWearItemInfo> obj1;

	ret = obj1.deserialize_vector1(vecUserWearItemInfo, -19, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	return (int32_t)offset;
}

int32_t protoSvrUserLeaveRoomRequest::getProtoLen(void)
{
	TSerializor1<UserItemInfo> obj;
	TSerializor1<UserWearItemInfo> obj1;
	return sizeof(MSG_HEAD)
		+sizeof(uin)
		+sizeof(roomId)
		+sizeof(level)
		+sizeof(exp)
		+sizeof(coin)
		+sizeof(ticket)
		+sizeof(money)
		+obj.get_vector_length1(vecUserItemInfo)
		+ sizeof(totalPutScore)
		+sizeof(totalGetScore)
		+sizeof(lostScore)
		+obj1.get_vector_length1(vecUserWearItemInfo);
}


//////////////////////////////////////////////////////////////////////////////////////////

protoSvrUserLeaveRoomResponse::protoSvrUserLeaveRoomResponse()
{
	protoId = IDD;
	protoLen = 0;
	checkCode = 0;

	result = 0;
}

int32_t protoSvrUserLeaveRoomResponse::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(result, -4);

	GENERATE_CODE;

	return (int32_t)offset;
}

int32_t protoSvrUserLeaveRoomResponse::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(result, -4);

	return (int32_t)offset;
}

int32_t protoSvrUserLeaveRoomResponse::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+sizeof(result);
}

//////////////////////////////////////////////////////////////////////////////////////////
CurrencyInfo::CurrencyInfo()
{
	type = 0;
	itemTypeId = 0;
	num = 0;
}

uint32_t CurrencyInfo::getLength(void)
{
	return sizeof(type)
		+sizeof(itemTypeId)
		+sizeof(num);
}

int32_t CurrencyInfo::Serialize(void *pBuf, size_t cbBuf, uint32_t &offset)
{
	int32_t ret(0);

	SERIALIZE_DATA(type, -11);
	SERIALIZE_DATA(itemTypeId, -12);
	SERIALIZE_DATA(num, -13);

	return (int32_t)offset;
}

int32_t CurrencyInfo::Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset)
{
	int32_t ret(0);

	DESERIALIZE_DATA(type, -11);
	DESERIALIZE_DATA(itemTypeId, -12);
	DESERIALIZE_DATA(num, -13);

	return (int32_t)offset;
}

//////////////////////////////////////////////////////////////////////////////////////////

protoSvrAddCurrencyRequest::protoSvrAddCurrencyRequest()
{
	protoId = IDD;
	protoLen = 0;
	checkCode = 0;

	uin = 0;
	type = 0;
	EMPTY_STRING(szInfo);
	timestamp = Now();
    opIndex = 0;
}

int32_t protoSvrAddCurrencyRequest::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(uin, -4);
	SERIALIZE_DATA(type, -5);
	

	TSerializor1<CurrencyInfo> obj;

	ret = obj.serialize_vector1(vecCurrencyInfo, -14, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	SERIALIZE_CHAR(szInfo, -7);
	SERIALIZE_DATA(timestamp, -8);
    SERIALIZE_DATA(opIndex, -9);

	GENERATE_CODE;

	return (int32_t)offset;
}

int32_t protoSvrAddCurrencyRequest::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(uin, -4);
	DESERIALIZE_DATA(type, -5);

	TSerializor1<CurrencyInfo> obj;

	ret = obj.deserialize_vector1(vecCurrencyInfo, -14, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	DESERIALIZE_CHAR(szInfo, -7);
	DESERIALIZE_DATA(timestamp, -8);
    DESERIALIZE_DATA(opIndex, -9);

	return (int32_t)offset;
}

int32_t protoSvrAddCurrencyRequest::getProtoLen(void)
{
	TSerializor1<CurrencyInfo> obj;
	return sizeof(MSG_HEAD)
		+sizeof(uin)
		+sizeof(type)
		+obj.get_vector_length1(vecCurrencyInfo)
		+LENGTH_STR(szInfo)
		+sizeof(timestamp)
        +sizeof(opIndex);
}


//////////////////////////////////////////////////////////////////////////////////////////

protoSvrAddCurrencyResponse::protoSvrAddCurrencyResponse()
{
	protoId = IDD;
	protoLen = 0;
	checkCode = 0;

	result = 0;
    opIndex = 0;
}

int32_t protoSvrAddCurrencyResponse::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(result, -4);
    SERIALIZE_DATA(opIndex, -5);

	GENERATE_CODE;

	return (int32_t)offset;
}

int32_t protoSvrAddCurrencyResponse::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(result, -4);
    DESERIALIZE_DATA(opIndex, -5);

	return (int32_t)offset;
}

int32_t protoSvrAddCurrencyResponse::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+sizeof(result)
        +sizeof(opIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////

protoSvrDecCurrencyRequest::protoSvrDecCurrencyRequest()
{
	protoId = IDD;
	protoLen = 0;
	checkCode = 0;

	uin = 0;
	type = 0;
	EMPTY_STRING(szInfo);
	timestamp = Now();
    opIndex = 0;
}

int32_t protoSvrDecCurrencyRequest::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(uin, -4);

	TSerializor1<CurrencyInfo> obj;

	ret = obj.serialize_vector1(vecCurrencyInfo, -14, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	SERIALIZE_DATA(type, -6);
	SERIALIZE_CHAR(szInfo, -7);
	SERIALIZE_DATA(timestamp, -8);
    SERIALIZE_DATA(opIndex, -9);

	GENERATE_CODE;

	return (int32_t)offset;
}

int32_t protoSvrDecCurrencyRequest::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(uin, -4);

	TSerializor1<CurrencyInfo> obj;

	ret = obj.deserialize_vector1(vecCurrencyInfo, -14, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	DESERIALIZE_DATA(type, -6);
	DESERIALIZE_CHAR(szInfo, -7);
	DESERIALIZE_DATA(timestamp, -8);
    DESERIALIZE_DATA(opIndex, -9);

	return (int32_t)offset;
}

int32_t protoSvrDecCurrencyRequest::getProtoLen(void)
{
	TSerializor1<CurrencyInfo> obj;
	return sizeof(MSG_HEAD)
		+sizeof(uin)
		+obj.get_vector_length1(vecCurrencyInfo)
		+sizeof(type)
		+ LENGTH_STR(szInfo)
		+sizeof(timestamp)
        +sizeof(opIndex);
}


//////////////////////////////////////////////////////////////////////////////////////////

protoSvrDecCurrencyResponse::protoSvrDecCurrencyResponse()
{
	protoId = IDD;
	protoLen = 0;
	checkCode = 0;

	result = 0;
    opIndex = 0;
}

int32_t protoSvrDecCurrencyResponse::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(result, -4);
    SERIALIZE_DATA(opIndex, -5);

	GENERATE_CODE;

	return (int32_t)offset;
}

int32_t protoSvrDecCurrencyResponse::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(result, -4);
    DESERIALIZE_DATA(opIndex, -5);

	return (int32_t)offset;
}

int32_t protoSvrDecCurrencyResponse::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+sizeof(result)
        +sizeof(opIndex);
}


//////////////////////////////////////////////////////////////////////////////////////////

protoDCSvrLoadUserDataResponse::protoDCSvrLoadUserDataResponse()
{
	protoId = IDD;
	protoLen = 0;
	checkCode = 0;

	result = 0;

	EMPTY_STRING(szDevice);
	EMPTY_STRING(szChannel);

	source = 0;
	version = 0;
	sessionId = 0;
	clientIp = 0;
	new_user = 0;
	proxyId = 0;
	channelId = 0;

	EMPTY_STRING(nick);
	EMPTY_STRING(account);
	vip = 0;
	level = 0;
	exp = 0;
	coin = 0;
	ticket = 0;
	money = 0;
	trumpet = 0;
	bankCoin = 0;
	createTime = 0;
}

int32_t protoDCSvrLoadUserDataResponse::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(result, -40);
	SERIALIZE_DATA(gamesvrId, -4);
	SERIALIZE_DATA(uin, -5);
	SERIALIZE_DATA(new_user, -6);
	SERIALIZE_DATA(source, -7);
	SERIALIZE_DATA(version, -8);
	SERIALIZE_DATA(sessionId, -9);
	SERIALIZE_DATA(clientIp, -10);
	SERIALIZE_CHAR(szDevice, -11);
	SERIALIZE_CHAR(szChannel, -13);
	SERIALIZE_DATA(proxyId, -15);
	SERIALIZE_DATA(channelId, -16);

	
	SERIALIZE_CHAR(account, -25);
	SERIALIZE_CHAR(nick, -26);
	SERIALIZE_DATA(vip, -27);
	SERIALIZE_DATA(level, -28);
	SERIALIZE_DATA(exp, -29);
	SERIALIZE_DATA(coin, -30);
	SERIALIZE_DATA(ticket, -31);
	SERIALIZE_DATA(money, -32);
	SERIALIZE_DATA(trumpet, -33);
	SERIALIZE_DATA(bankCoin, -34);
	SERIALIZE_DATA(verified, -35);
	SERIALIZE_DATA(createTime, -36);
	
	SERIALIZE_CHAR(registerChannel, -37);
	SERIALIZE_CHAR(reserve1, -38);
	SERIALIZE_CHAR(reserve2, -39);
	
	TSerializor1<UserItemInfo> obj;
	ret = obj.serialize_vector1(vecUserItemInfo, -36, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	TSerializor1<UserWearItemInfo> obj1;
	ret = obj1.serialize_vector1(vecUserWearItemInfo, -37, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	TSerializor1<UserFishInfo> obj2;
	ret = obj2.serialize_vector1(vecUserFishInfo, -38, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	TSerializor1<UserContiTaskInfo> obj3;
	ret = obj3.serialize_vector1(vecUserContiTaskInfo, -39, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	SERIALIZE_DATA(totalRecharge, -40);

	GENERATE_CODE;

	return (int32_t)offset;
}

int32_t protoDCSvrLoadUserDataResponse::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(result, -40);
	DESERIALIZE_DATA(gamesvrId, -4);
	DESERIALIZE_DATA(uin, -5);
	DESERIALIZE_DATA(new_user, -6);
	DESERIALIZE_DATA(source, -7);
	DESERIALIZE_DATA(version, -8);
	DESERIALIZE_DATA(sessionId, -9);
	DESERIALIZE_DATA(clientIp, -10);
	DESERIALIZE_CHAR(szDevice, -11);
	DESERIALIZE_CHAR(szChannel, -13);
	DESERIALIZE_DATA(proxyId, -15);
	DESERIALIZE_DATA(channelId, -16);

	DESERIALIZE_CHAR(account, -25);
	DESERIALIZE_CHAR(nick, -26);
	DESERIALIZE_DATA(vip, -27);
	DESERIALIZE_DATA(level, -28);
	DESERIALIZE_DATA(exp, -29);
	DESERIALIZE_DATA(coin, -30);
	DESERIALIZE_DATA(ticket, -31);
	DESERIALIZE_DATA(money, -32);
	DESERIALIZE_DATA(trumpet, -33);
	DESERIALIZE_DATA(bankCoin, -34);
	DESERIALIZE_DATA(verified, -35);
	DESERIALIZE_DATA(createTime, -36);
	
	DESERIALIZE_CHAR(registerChannel, -37);
	DESERIALIZE_CHAR(reserve1, -38);
	DESERIALIZE_CHAR(reserve2, -39);

	TSerializor1<UserItemInfo> obj;
	ret = obj.deserialize_vector1(vecUserItemInfo, -36, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	TSerializor1<UserWearItemInfo> obj1;
	ret = obj1.deserialize_vector1(vecUserWearItemInfo, -37, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	TSerializor1<UserFishInfo> obj2;
	ret = obj2.deserialize_vector1(vecUserFishInfo, -38, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	TSerializor1<UserContiTaskInfo> obj3;
	ret = obj3.deserialize_vector1(vecUserContiTaskInfo, -39, pBuf, cbBuf, offset);
	if (ret < 0) return ret;
	DESERIALIZE_DATA(totalRecharge, -40);
	return (int32_t)offset;
}

int32_t protoDCSvrLoadUserDataResponse::getProtoLen(void)
{
	TSerializor1<UserItemInfo> obj;
	TSerializor1<UserWearItemInfo> obj1;
	TSerializor1<UserFishInfo> obj2;
	TSerializor1<UserContiTaskInfo> obj3;

	return sizeof(MSG_HEAD)
		+sizeof(result)
		+sizeof(gamesvrId)
		+sizeof(uin)
		+sizeof(new_user)
		+sizeof(source)
		+sizeof(version)
		+sizeof(sessionId)
		+sizeof(clientIp)
		+sizeof(proxyId)
		+sizeof(channelId)
		+LENGTH_STR(szDevice)
		+ LENGTH_STR(szChannel)
		+ LENGTH_STR(account)
		+ LENGTH_STR(nick)
		+ sizeof(vip)
		+sizeof(level)
		+sizeof(exp)
		+sizeof(coin)
		+sizeof(ticket)
		+sizeof(money)
		+sizeof(trumpet)
		+sizeof(bankCoin)
		+sizeof(verified)
		+sizeof(createTime)
		+sizeof(totalRecharge)
		+LENGTH_STR(registerChannel)
		+ LENGTH_STR(reserve1)
		+ LENGTH_STR(reserve2)
		+obj.get_vector_length1(vecUserItemInfo)
		+obj1.get_vector_length1(vecUserWearItemInfo)
		+ obj2.get_vector_length1(vecUserFishInfo)
		+ obj3.get_vector_length1(vecUserContiTaskInfo);
}


//////////////////////////////////////////////////////////////////////////////////////////

protoDCSvrUserDataSyncRequest::protoDCSvrUserDataSyncRequest()
{
	protoId = IDD;
	protoLen = 0;
	checkCode = 0;

	uin = 0;
	index = 0;

	EMPTY_STRING(nick);
	vip = 0;
	level = 0;
	exp = 0;
	coin = 0;
	ticket = 0;
	money = 0;
	trumpet = 0;
	bankCoin = 0;
	verified = 0;
	nTotalRecharge = 0;
}

int32_t protoDCSvrUserDataSyncRequest::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(index, -24);
	SERIALIZE_DATA(uin, -25);
	SERIALIZE_CHAR(nick, -26);
	SERIALIZE_DATA(vip, -27);
	SERIALIZE_DATA(level, -28);
	SERIALIZE_DATA(exp, -29);
	SERIALIZE_DATA(coin, -30);
	SERIALIZE_DATA(ticket, -31);
	SERIALIZE_DATA(money, -32);
	SERIALIZE_DATA(trumpet, -33);
	SERIALIZE_DATA(bankCoin, -34);
	SERIALIZE_DATA(verified, -35);

	TSerializor1<UserItemInfo> obj;
	ret = obj.serialize_vector1(vecUserItemInfo, -36, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	TSerializor1<UserWearItemInfo> obj1;
	ret = obj1.serialize_vector1(vecUserWearItemInfo, -37, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	TSerializor1<UserFishInfo> obj2;
	ret = obj2.serialize_vector1(vecUserFishInfo, -38, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	TSerializor1<UserContiTaskInfo> obj3;
	ret = obj3.serialize_vector1(vecUserContiTaskInfo, -39, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	SERIALIZE_DATA(nTotalRecharge, -36);

	GENERATE_CODE;

	return (int32_t)offset;
}

int32_t protoDCSvrUserDataSyncRequest::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(index, -24);
	DESERIALIZE_DATA(uin, -25);
	DESERIALIZE_CHAR(nick, -26);
	DESERIALIZE_DATA(vip, -27);
	DESERIALIZE_DATA(level, -28);
	DESERIALIZE_DATA(exp, -29);
	DESERIALIZE_DATA(coin, -30);
	DESERIALIZE_DATA(ticket, -31);
	DESERIALIZE_DATA(money, -32);
	DESERIALIZE_DATA(trumpet, -33);
	DESERIALIZE_DATA(bankCoin, -34);
	DESERIALIZE_DATA(verified, -35);

	TSerializor1<UserItemInfo> obj;
	ret = obj.deserialize_vector1(vecUserItemInfo, -36, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	TSerializor1<UserWearItemInfo> obj1;
	ret = obj1.deserialize_vector1(vecUserWearItemInfo, -37, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	TSerializor1<UserFishInfo> obj2;
	ret = obj2.deserialize_vector1(vecUserFishInfo, -38, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	TSerializor1<UserContiTaskInfo> obj3;
	ret = obj3.deserialize_vector1(vecUserContiTaskInfo, -39, pBuf, cbBuf, offset);
	if (ret < 0) return ret;

	DESERIALIZE_DATA(nTotalRecharge, -36);

	return (int32_t)offset;
}

int32_t protoDCSvrUserDataSyncRequest::getProtoLen(void)
{
	TSerializor1<UserItemInfo> obj;
	TSerializor1<UserWearItemInfo> obj1;
	TSerializor1<UserFishInfo> obj2;
	TSerializor1<UserContiTaskInfo> obj3;

	return sizeof(MSG_HEAD)
		+sizeof(index)
		+sizeof(uin)
		+ LENGTH_STR(nick)
		+ sizeof(vip)
		+sizeof(level)
		+sizeof(exp)
		+sizeof(coin)
		+sizeof(ticket)
		+sizeof(money)
		+sizeof(trumpet)
		+sizeof(bankCoin)
		+sizeof(verified)
		+obj.get_vector_length1(vecUserItemInfo)
		+ obj1.get_vector_length1(vecUserWearItemInfo)
		+ obj2.get_vector_length1(vecUserFishInfo)
		+ obj3.get_vector_length1(vecUserContiTaskInfo)
		+ sizeof(nTotalRecharge);
}


//////////////////////////////////////////////////////////////////////////
protoRepostMsgRequest::protoRepostMsgRequest()
{
	protoId = IDD;
	protoLen = 0;

	uin = 0;
	cbData = 0;
	pData = 0;
	bBroadcast = false;
}

protoRepostMsgRequest::~protoRepostMsgRequest()
{
	if (NULL != pData)
	{
		free(pData);
		pData = NULL;
	}
}

int32_t protoRepostMsgRequest::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(uin, -4);
	SERIALIZE_DATA(cbData, -9);
	SERIALIZE_BUF(pData, cbData, -10);
	SERIALIZE_DATA(bBroadcast, -11);
	

	GENERATE_CODE;
	return (int32_t)offset;
}

int32_t protoRepostMsgRequest::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(uin, -4);
	DESERIALIZE_DATA(cbData, -9);
	pData = calloc(1, cbData);
	DESERIALIZE_BUF(pData, cbData, -10);

	DESERIALIZE_DATA(bBroadcast, -11);

	return (int32_t)offset;
}

int32_t protoRepostMsgRequest::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+sizeof(uin)
		+sizeof(cbData)
		+cbData
		+ sizeof(bBroadcast);
}


//////////////////////////////////////////////////////////////////////////
protoRepostMsgResponse::protoRepostMsgResponse()
{
	protoId = IDD;
	protoLen = 0;

	result = 0;
}

int32_t protoRepostMsgResponse::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(result, -4);

	GENERATE_CODE;
	return (int32_t)offset;
}

int32_t protoRepostMsgResponse::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(result, -4);

	return (int32_t)offset;
}

int32_t protoRepostMsgResponse::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+sizeof(result);
}

