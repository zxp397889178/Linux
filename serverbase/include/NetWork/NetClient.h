/*
 * =====================================================================================
 *
 *       Filename:  NetClient.h
 *
 *    Description:  net client class
 *
 *        Version:  1.0
 *        Created:  2014年11月12日 11时49分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_NETWORK_CHILD_NETCLIENT_H_
#define _MYLIB_NETWORK_CHILD_NETCLIENT_H_

#include "INetClient.h"
#include <string>
#include "BaseCode.h"
// #include "ByteStream.h"


class CNetClient : public IClientSocket
{
	enum
	{
		SOCKET_CHECK_WRITE = 1,
		SOCKET_CHECK_READ = 2,
		RCV_BUFFER_SIZE = 262144,	// 256KB
		SND_BUFFER_SIZE = 262144,	// 256KB
	};
	enum MY_SOCK_ERROR
	{
		ERR_OK = 0,			// not error
		ERR_NO_MORE_DATA,	// not more data
		ERR_CLOSED,			// socket is closed by peer
		ERR_SHUTDOWN,		// socket is closed by self
		ERR_SYSTEM,			// local system cause error
		ERR_OTHERS,			// other error, may check errno or SOL_ERROR
	};
public:
	explicit CNetClient(IClientSocketEvent *pEvent);
	~CNetClient();
	bool Open(const char *pszHost, uint16_t wPort, 
			uint32_t dwTimeout = 1000);	//连接超时，以毫秒计算

public:
	virtual void release(void) { delete this; }
	virtual int32_t SendMsg(const void *pBuf, size_t &cbLen);
	virtual int32_t RecvMsg(void *pBuf, size_t &cbLen);
	virtual bool Process(uint32_t dwSleepTime);
	virtual void Close(void);

	virtual bool IsActive(void) { return m_bIsActive; }
	virtual bool GetHost(char *szHost, size_t cbHost, uint16_t &wPort);

private:
	int32_t Connect(uint32_t dwIp, uint16_t wPort, 
			uint32_t dwConnectTimeout);

	bool IsConnected(uint32_t dwTimeout);
	bool CanWrite(uint32_t dwTimeout);
	int32_t CanRead(uint32_t dwTimeout);


	// flag: 1	check socket can be write
	// 		 2	check socket can be read
	// dwTimeout in milliseconds
	// result: -1 fail
	// 		    0 timeout
	// 		    1 socket is OK(can be write)
	int32_t CheckSocket_BySelect(int32_t sockfd, int32_t flag, uint32_t dwTimeout);
	int32_t CheckSocket_ByPoll(int32_t sockfd, int32_t flag, uint32_t dwTimeout);
	void internal_close(void);

	void PrintError(int32_t sockfd, const char *ext_msg);
	int32_t GetLastError(void);

	int32_t ProcessRecv(uint32_t dwTimeout);
	int32_t ProcessData(void);
private:
	IClientSocketEvent	*m_pEvent;
	std::string			m_strHost;
	uint16_t			m_wPort;
	bool 				m_bIsActive;

	int					m_fd;
    int					m_cbSendBuf;
	int32_t 			m_cbRecvBuf;
	
	CByteStream			m_buffer;
};

IClientSocket* OpenClient(IClientSocketEvent *pHandler, const char *pszHost, uint16_t wPort);


#endif //_MYLIB_NETWORK_CHILD_NETCLIENT_H_
