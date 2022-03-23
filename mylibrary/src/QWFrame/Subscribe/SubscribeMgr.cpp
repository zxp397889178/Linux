/*
* =====================================================================================
*
*       Filename:  SubscribeMgr.cpp
*
*    Description:  ¶©ÔÄÏûÏ¢
*
*        Version:  1.0
*        Created:  2018-06-14
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  meilinfeng
*   Organization:  quwan
*
* =====================================================================================
*/
#include "SubscribeMgr.h"
#include "../QWFrame.h"
#include "../Protocol/protoSubscribe.h"
#include "protoBase.h"
static const char *DOMAIN="SUBSCRIBEMGR";

CSubscribe::CSubscribe()
{


}


CSubscribe::~CSubscribe()
{


}


bool CSubscribe::SubscribeMsg()
{
    protoSubscribeMsgRequest req;
    
    req.vecProtoId = m_vecProtoIdList;

    LogDebug2(DOMAIN, 0, "%s:vecProtoId size = %ld======",__func__,req.vecProtoId.size());
    
	Record record;
	return CONNECTION_MGR.SendMsgToSvr(actorCenterSvr, record, req);

}

void CSubscribe::AddProtoid(uint32_t protoId)
{

    m_vecProtoIdList.push_back(protoId);
}


CSubscribe& CSubscribe::Instance()
{
	static CSubscribe s_obj;

	return s_obj;
}


