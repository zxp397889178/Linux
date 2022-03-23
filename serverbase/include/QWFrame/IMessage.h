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

#ifndef _QW_FRAME_IMESSAGE_H_
#define _QW_FRAME_IMESSAGE_H_


struct Context
{
	uint32_t uin;
};


class IMessage
{
public:
	virtual ~IMessage() {};

	//virtual uint32_t		GetMsgId() = 0;
	//virtual int32_t		OnMessage(const Context& context, void *pData, uint32_t cbData) = 0;
	virtual bool Init() = 0;
	virtual void Run() = 0;
	virtual void Finish() = 0;
};


#endif // _QW_FRAME_IMESSAGE_H_ 

