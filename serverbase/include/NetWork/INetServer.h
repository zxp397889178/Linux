/*
 * =====================================================================================
 *
 *       Filename:  INetServer.h
 *
 *    Description:  net server interface
 *
 *        Version:  1.0
 *        Created:  2014/11/17 21时56分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *
 * =====================================================================================
 */

#ifndef _MYLIB_NETWORK_NETSERVER_H_
#define _MYLIB_NETWORK_NETSERVER_H_

#include <stdint.h>
#include <sys/types.h>


class IClientObj
{
public:
	virtual void release(void) = 0;
	virtual uint32_t getClientId(void) = 0;
	virtual uint32_t getIpRemote(void) = 0;
	virtual int32_t  getSocket(void) = 0;

protected:
	virtual ~IClientObj() {}

};

class IServerSocket;
class IServerSocketEvent
{
public:
	virtual void release(void) = 0;

	// 判断是否可以接受ipRemote(网络字节序)的接入
	virtual bool OnAcceptSck(uint32_t ipRemote) = 0;
	// 远端ipRemote连接成功，并分配一个nId标识用户身份
	virtual void OnEstablishSck(uint32_t nId, uint32_t ipRemote) = 0;
	// 接收到nId客户的数据，并返回处理完毕的字节数
	// -1表示断开网络连接
	virtual int32_t OnRcvMsg(uint32_t nId, const void *pBuf, size_t cbBuf) = 0;
	// 关闭客户端的连接，bPassive指示是否主动关闭
	virtual void OnCloseSck(uint32_t nId, uint32_t ipRemote, bool bPassive) = 0;

	virtual void BindSocket(IServerSocket *pSocket) = 0;
protected:
	virtual ~IServerSocketEvent() {};
};

class IServerSocket
{
public:

	virtual void release(void) = 0;

	virtual bool SendMsg(uint32_t nId, const void *pBuf, size_t cbBuf) = 0;

	virtual int32_t Process(void) = 0;

	virtual void CloseClient(int32_t nFd) = 0;

	virtual void RefuseNewConnect(bool bRefuse) = 0;

protected:
	virtual ~IServerSocket() { }
};


#endif //_MYLIB_NETWORK_NETSERVER_H_  


