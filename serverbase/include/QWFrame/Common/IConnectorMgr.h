/*
 * =====================================================================================
 *
 *       Filename:  IConnectorMgr.h
 *
 *    Description:  连接器消息回调管理者，用于处理连接器断开，以及返回消息处理
 *
 *        Version:  1.0
 *        Created:  2015年09月11日 13时41分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_GAMESVR_CONNECTOR_MGR_H_
#define _QW_GAMESVR_CONNECTOR_MGR_H_



class CConnector;
class IConnectorMgr
{
public:
	virtual ~IConnectorMgr() {}

#if 0
	// 处理连接的身份验证
	virtual int32_t OnAuthorize(CConnection *pConn) = 0;
#endif

	// 处理connector返回的数据，pBuf是包含struct Record的数据
	// record 记录头(冗余，已转换)
	// pBuf   数据内容，包含record的原始值
	// cbBuf  数据区长度
	// 返回值：<0 出错，断开连接
	// 		   >=0 正常
	virtual int32_t OnConnectorRespone(CConnector *pConn, 
			struct Record &record, void *pBuf, uint32_t cbBuf) = 0;

	// 处理连接断开的情况
	virtual void OnConnectorShutdown(CConnector *pConn) = 0;


};

#endif // _QW_GAMESVR_CONNECTOR_MGR_H_

