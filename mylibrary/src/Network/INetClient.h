/*
 * =====================================================================================
 *
 *       Filename:  INetClient.h
 *
 *    Description:  net client interface
 *
 *        Version:  1.0
 *        Created:  2014年11月12日 11时30分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_NETWORK_NETCLIENT_H_
#define _MYLIB_NETWORK_NETCLIENT_H_

#include <stdint.h>
#include <sys/types.h>

class IClientSocketEvent
{
public:
	virtual bool OnEstablished(const char *pszHost, uint16_t wPort) = 0;
	// 接收并处理消息
	// 返回值: <0 出错，必须断开与远端的连接
	//		    0 数据包不完整，没有处理，继续接收数据
	//		   >0 接收并处理ret的数据
	virtual int32_t OnRcvMsg(const void *pBuf, size_t cbLen) = 0;
	virtual void OnClose(bool bPassive) = 0;

	virtual void release(void) = 0;

protected:
	virtual ~IClientSocketEvent() = 0;
};

class IClientSocket
{
public:
	virtual void release(void) = 0;

	// 发送消息，会自动依据发包缓冲区进行分包发送
	// 参数: cbLen [IN]  指示 pBuf 的长度
	// 			   [OUT] 指示发送成功的字节数 
	// 返回值：-1 出错
	// 		   =0 对方网络断开
	// 		    1 发送成功
	virtual int32_t SendMsg(const void *pBuf, size_t &cbLen) = 0;

	// 手动调用接收消息 ( 与Process互斥，两者只能调用其一 )
	// 参数: cbLen [IN]	 指示 pBuf 的长度
	// 			   [OUT] 指示接收成功的字节数
	// 返回值： -1 出错(对方网络断开也在此表达)
	// 			 0 对方断开网络
	// 		     1 接收数据完毕，且没有更多的数据需要接收
	// 		     2 接收数据完毕，可能存在更多的数据需要接收
	virtual int32_t RecvMsg(void *pBuf, size_t &cbLen) = 0;

	// 自动调用接收消息, 须与 IClientSocketEvent::OnRcvMsg() 配合使用
	// 返回值: false 出错了，可以中断后续的处理
	// 		   true  处理成功
	virtual bool Process(uint32_t dwSleepTime) = 0;
	
	//主动关闭网络
	virtual void Close(void) = 0;

public:
	// 网络连接是否可用
	virtual bool IsActive(void) = 0;
	// 获取远端网络 IP 和 端口
	virtual bool GetHost(char *szHost, size_t cbHost, uint16_t &wPort) = 0;

protected:
	virtual ~IClientSocket() = 0;
};


#endif	//_MYLIB_NETWORK_NETCLIENT_
