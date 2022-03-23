/*
* =====================================================================================
*
*       Filename:  SubscribeMgr.h
*
*    Description:  订阅消息
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

#ifndef _QW_FRAME_SUBSCRIBEMGR_H_
#define _QW_FRAME_SUBSCRIBEMGR_H_


#include <stdint.h>
#include <vector>
//#include "Protocol/protoBase.h"

class CSubscribe
{
public:
    ~CSubscribe();

	// 添加需要订阅的消息
    void AddProtoid(uint32_t protoId);


	//订阅消息
	bool SubscribeMsg();

private:

    std::vector<uint32_t> m_vecProtoIdList;

public:
    static CSubscribe& Instance();
    
private:
	CSubscribe();

};



#define SUBSCRIBE   CSubscribe::Instance()



#endif







