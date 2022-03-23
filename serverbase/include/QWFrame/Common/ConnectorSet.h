/*
 * =====================================================================================
 *
 *       Filename:  ConnectorSet.h
 *
 *    Description:  连接器管理者，负责创建，但不负责删除
 *
 *        Version:  1.0
 *        Created:  2015年09月13日 10时17分10秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */


#ifndef _QW_GAMESVR_CONNECTOR_SET_H_
#define _QW_GAMESVR_CONNECTOR_SET_H_

#include <vector>
#include <map>
#include "Connector.h"

/* 仅管理CConnector的映射关系，不涉及所有权处置，如删除对象
 * */
class IConnectorMgr;
class CConnectorSet
{
	typedef std::vector<CConnector*>		CONNECTOR_VEC;
	typedef std::map<uint32_t, CConnector*>	CONNECTOR_MAP;
public:
	CConnectorSet();
	~CConnectorSet();

	bool Init(uint32_t svrId, uint32_t count,
			enum Actor actorSelf, enum Actor actor, const CMyString &actorName,
			IConnectorMgr *pMgr, mm::Reactor *pReactor,
			const CMyString &strHost, uint32_t port);

	/* maintance connector count
	 * return: -1	create CConnector fail
	 * 			0	not need maintance
	 * 		   >0	maintance count	
	 * */
	int32_t Maintance();

	CConnector* Find(uint32_t channelId = 0);

	bool SendMsg(void *pBuf, size_t cbBuf);
	bool SendMsg(uint32_t channelId, void *pBuf, size_t cbBuf);

	void Remove(uint32_t channelId);
	void Remove(CConnector *pConn);
	uint32_t Add(CConnector *pConn);

private:
	CONNECTOR_VEC		m_vecConnector;
	CONNECTOR_MAP		m_mapConnector;

	ILock*				m_pLock;
	uint32_t			m_lastChannelId;

private:
	uint32_t			m_svrId;
	uint32_t			m_count;
	enum Actor			m_actor;
	CMyString			m_actorName;
	IConnectorMgr*		m_pMgr;
	mm::Reactor*		m_pReactor;
	CMyString			m_strHost;
	uint32_t			m_port;
	enum Actor			m_actorSelf;
};

#endif // _QW_GAMESVR_CONNECTOR_SET_H_

