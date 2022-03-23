/*
* =====================================================================================
*
*       Filename:  SubscribeMgr.h
*
*    Description:  ������Ϣ
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

	// �����Ҫ���ĵ���Ϣ
    void AddProtoid(uint32_t protoId);


	//������Ϣ
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







