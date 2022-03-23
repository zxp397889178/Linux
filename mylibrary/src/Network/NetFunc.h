/*
 * =====================================================================================
 *
 *       Filename:  NetFunc.h
 *
 *    Description:  common net function
 *
 *        Version:  1.0
 *        Created:  2014年11月12日 15时28分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_NETWORK_NET_FUNCTION_H_
#define _MYLIB_NETWORK_NET_FUNCTION_H_

#include <vector>
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include "BaseCode.h"

uint32_t net2local(uint32_t val);
uint16_t net2local(uint16_t val);
uint32_t local2net(uint32_t val);
uint16_t local2net(uint16_t val);

#define NETORDER(b)				local2net((b))
#define HOSTORDER(b)			net2local((b))
#define HAS_PROP(var, attr)		( ((var)&(attr)) != 0)

// get host ip, maybe multi, the ip is network order
// return -1 if fail, or zero succeed
int32_t host_to_ip(const char *pszHost, std::vector<uint32_t> &arIp);

// get the hostname of ip_str, if multi result, return the first
// return NULL if fail
const char* ip_to_host(const char *ip_str);

// convert (network order) ip to str (like 192.168.0.1)
// return NULL if szText is to overflow
const char* ip2str(uint32_t addr, char *szText, size_t cbText);

// convert ip_str to ip (network order)
// return INADDR_NONE if the ip_str is invalid
uint32_t str2ip(const char *ip_str);

// get the last error of sockfd
// return -1 if fail, and the errno valid
// 		  >0 is the error number, you can call strerror() to gain detail message.
// 		  =0 is no error
int32_t WSAGetLastError(int32_t sockfd);

uint32_t GetLocalIp(void);
int32_t GetLocalIp(std::vector<uint32_t> &arIp);

// 简化socket的创建及打开 
// open_tcp_socket 调用 create_tcp_socket + open_tcp_port
int32_t open_tcp_socket(uint16_t port, bool tcp=true, 
		bool blocked=false, bool linger=false, 
		int32_t rcvbuf=-1, int32_t sndbuf=-1, 
		const char* bindip=NULL);

// 关闭tcp/udp socket
// lingler [true ==>close immediately, false ==>with system pleasure]
void close_tcp_socket(int32_t &sockfd, bool linger=false);

int32_t set_sck_nonblock(int32_t sockfd);

// 创建tcp/udp socket
// tcp     [true ==>TCP,                false ==>UDP]
// blocket [true ==>blocked socket,     false ==>non-blocked socket]
// linger  [true ==>set socket linger,  false ==>not set linger]
// rcvbuf  [>0   ==>set receive buffer, <=0   ==>not set]
// sndbuf  [>0   ==>set send buffer,    <=0   ==>not set]
// return -1 indicate error
//        >0 socket handler
int32_t create_tcp_socket(bool tcp=true, bool blocked=false, bool linger=false, int32_t rcvbuf=-1, int32_t sndbuf=-1);

// 打开tcp或udp服务
// listen_port [true ==>侦听端口,udp也可以侦听; false ==>不侦听，udp缺省是不侦听端口]
// bindip NULL或空则绑定全部IP，指定IP则绑定单个IP
int32_t open_tcp_port(int32_t fd, uint16_t port, bool listen_port=true, const char *bindip= NULL);

// connect to remote host
// return: 0 connect ok
// 		  -1 set no-block failure
// 		  -2 connect timeout
// 		  -3 connect fault
//int32_t open_tcp_connect(int32_t fd, const char *pszHost, uint16_t port, uint32_t timeout);

bool open_tcp_connect(int32_t fd, const char *pszHost, uint16_t port);
int32_t open_tcp_connect(int32_t fd, const char *pszHost, uint16_t port, uint32_t timeout);

// 打印socket错误信息
// result:	0  not error
// 		   -1  system error
// 		   >0  errno
int32_t print_socket_error(int32_t sockfd, const char *ext_msg = NULL);


// send data through sockfd
// maxloop_ 	 0   not any retry
// 			   	>0   max re-send count
// sendbuf	   	-1   not use sendbuf size
// 				 0   error, return 0
//			   	>0   max size each send
// return: 		>0   send data size
// 				 0 	 not any data send
// 		   		-1	 socket fail, or not socket
// 		   		-2	 connect reset by peer (对端主动关闭)
// 		   		-3   close by send side    (本端主动关闭) 
// 		   		-4	 other send error
// 		   		-5   send buffer overflow
// 		   		-6   send retry time oversize
// 		   		-9   send try count>maxloop_
//
// NOTE: 发送成功后，返回值有可能小于cbLen，这种情况有可能是发送缓冲区满了
// 		   
enum SendError
{
	errSendRetryCount 	= -9,		// 重试次数超限
	errSendRetryTimeout = -6,		// 重试超时
	errSendRetry		= -5,		// 发送缓存为0，请重试
	errSendError		= -4,		// 其他发送错误
	errCloseScocket		= -3,		// 本端主动关闭
	errClosePassive		= -2,		// 对端关闭
	errSocketError		= -1,		// SOCKET错误
	errSendNoData		= 0,		// 未发送数据
};
int32_t send_data(int32_t sockfd, void *pBuf, size_t cbLen, 
		uint32_t maxloop_=0, uint32_t sendbuf_=(uint32_t)-1);
/* 
 * return: = 0 表示处理成功，具体发送了多少字节，以totalSend为准
 * 			-1 无效的sockfd
 *			-2 客户端关闭
 *			-3 服务端关闭
 *			-4 未知错误
 *			-7 发送超时
 * */
int32_t send_data_ex(int32_t sockfd, void *pBuf, size_t cbLen, uint32_t &totalSend, uint32_t timeout=(uint32_t)-1);

// check the socket can read or write
// dwTimeout  in millisecond (1/1000 s)
// return 	[1 ==>can read, 2 ==>can write, 3 ==>can read & write]
// 			0	timeout
// 			-1	invalid socket
// 			-2	select failure
//			-3  socket catch exception
int32_t check_socket_status(int32_t sockfd, uint32_t dwTimeout);

/* 设置socket的接收缓存
 * param: rcvbuf >0 设置接收缓存;  <=0 获得原先的接收缓存
 * return 当前socket的接收缓存大小
 */
int32_t set_sock_rcvbuf(int32_t sockfd, int32_t rcvbuf);

/* 设置socket的发送缓存
 * param: sndbuf >0 设置发送缓存;  <=0 获得原先的发送缓存
 * return 当前socket的发送缓存大小
 */
int32_t set_sock_sndbuf(int32_t sockfd, int32_t sndbuf);
/* 设置socket的linger 
 * linger = true, 则为系统缺省处理 (会将全部数据发送再关闭)
 * linger = false, linger_timeout=0,  socket立即关闭，未发送数据丢失
 * 				   linger_timeout>0,  socket最多等待的秒数
 * */
void set_sock_linger(int32_t sockfd, bool linger=false, int32_t linger_timeout=0);

// Disable Nagle's algorithm
// https://blog.csdn.net/lclwjl/article/details/80154565
void set_sock_nagle(int32_t sockfd, bool bDisable);

bool decode_url(const CMyString &url, CMyString &strHost, int32_t &port, CMyString &strCgi, CMyString &strParam, bool &bSSL);
bool decode_url_ex(const CMyString &strUrl, CMyString &strHost, int32_t &port, CMyString &strCgi, CMyString &strParam, bool &bSSL);

#endif	// _MYLIB_NETWORK_NET_FUNCTION_H_


