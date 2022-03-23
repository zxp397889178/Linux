/*
 * =====================================================================================
 *
 *       Filename:  IBucketEvent.h
 *
 *    Description:  对CBucket刷写事件的处理
 *
 *        Version:  1.0
 *        Created:  2014-4-2
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DA_BUCKET_EVENT_INTERFACE_
#define _GFX_SERVER_DA_BUCKET_EVENT_INTERFACE_

namespace my_data{

class  IBucketEvent
{
public:
    virtual ~IBucketEvent() {}

	// 调用IBucketEvent的初始化
	virtual bool Init(void) = 0;

    //将数据刷写到数据缓存服务器
    //涉及到网络发送
    virtual int OnFlush2Serve(uint32_t userId, byte_t *pData, size_t cbData, bool bCompressed) = 0;

    //将数据刷写到数据库中
    //涉及到数据库操作
    virtual int OnFlush2DB(uint32_t userId, const char *pszSql, size_t cbSql) = 0;
};

}   //namespace my_data

#endif  //_GFX_SERVER_DA_BUCKET_EVENT_INTERFACE_

