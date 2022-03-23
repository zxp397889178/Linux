/*
* =====================================================================================
*
*       Filename:  ISessionMgr.h
*
*    Description:  session manager, for accept callback
*
*        Version:  1.0
*        Created:  2015年09月08日 16时44分33秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
*   Organization:  www.skyunion.net(IGG)
*
* =====================================================================================
*/

#ifndef _QW_FRAME_IMSGREGISTERFACTORY_H_
#define _QW_FRAME_IMSGREGISTERFACTORY_H_

#include <vector>
#include "IMessage.h"
//class IMessage;
class  IMsgRegisterFactory
{
public:
	 virtual ~IMsgRegisterFactory() {};

	 // 初始化（注册函数）
	 virtual void Init() = 0;

public:
	virtual std::vector<IMessage*> GetMessgeList() = 0;


};


#endif
