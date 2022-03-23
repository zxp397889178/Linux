/*
 * =====================================================================================
 *
 *       Filename:  IHttpResult.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月20日 18时31分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_HTTP_RESULT_CALLBACK_H_
#define _QW_CRAZYFISH_HTTP_RESULT_CALLBACK_H_

#include "BaseCode.h"

class IHttpResult
{
public:
	virtual ~IHttpResult() { }

	// 与远端主机建立连接成功
	virtual void OnEstablished(int32_t nExecId, int32_t cost) { }

	// SSL连接成功
	virtual void OnSSLConnect(int32_t nExecId, int32_t cost) { }

	/* HTTP执行结果返回
	 * code > 0		http 返回值
	 * 		 -901	http 解析出错
	 *		 -902	返回的结果超长（1M)
	 *		 -909	其他错误
	 *		 -990	连接超时
	 *		 -999	执行超时(发送、接收数据)
	 */
	virtual void OnHttpResult(int32_t nExecId, int32_t code, CByteStream *pOutBuf, int32_t cost) = 0;
};


#endif // _QW_CRAZYFISH_HTTP_RESULT_CALLBACK_H_

