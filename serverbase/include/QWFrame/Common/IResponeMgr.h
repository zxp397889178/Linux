/*
 * =====================================================================================
 *
 *       Filename:  IResponeMgr.h
 *
 *    Description:  respone callback
 *
 *        Version:  1.0
 *        Created:  2015年09月10日 10时54分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_AUTHSVR_RESPONE_MGR_H_
#define _QW_AUTHSVR_RESPONE_MGR_H_

#include "BaseCode.h"


class IResponeMgr 
{
public:
	virtual ~IResponeMgr() {}

	// 向指定客户端下发消息, pData仅包含消息内容
	virtual bool OnRespMsg(enum Actor actor, uint32_t svrId, uint32_t channelId,
			struct Record &record, void *pData, uint32_t cbData) = 0;

	// 向指定客户端下发消息，pBuf是已序列化过的，无法重新组装
	virtual bool OnRespBuffer(enum Actor actor, uint32_t svrId, uint32_t channelId,
			void *pBuf, uint32_t cbBuf) = 0;

	// 向所有客户端广播消息
	virtual bool OnBroadcastMsg(uint32_t extraId, void *pData, uint32_t cbData) = 0;

	// 向一组用户发送消息
	virtual bool OnMulticastMsg(std::vector<uint32_t> &arUin, void *pData, uint32_t cbData) = 0;	
};

#endif // _QW_AUTHSVR_RESPONE_MGR_H_

