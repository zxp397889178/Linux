/*
 * =====================================================================================
 *
 *       Filename:  IClientMgr.h
 *
 *    Description:  client manager
 *
 *        Version:  1.0
 *        Created:  2019年09月11日 22时37分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "BaseCode.h"
#include "MyNet.h"
#include "IPlugin.h"

class CClient;

typedef void (*PFN_CLOSE_CB)(CClient*);
class IClientMgr
{
public:
    virtual ~IClientMgr() {  }

    /* msg 消息调用
     * 参数：[IN]   pClient     客户端指针
     *       [IN]   msgId       调用的消息ID
     *       [IN]   pData       消息数据内容
     *       [IN]   pkgLen      pData的长度
     * 返回值：0   操作成功
     *         <0  处理失败，断开与pClient的连接
     */
    virtual int32_t ExecuteMSG(CClient *pClient, uint32_t msgId, 
            byte_t *pData, uint16_t pkgLen)
    {
        return -1;
    }

    /* http 消息调用
     * 参数：[IN]     pClient       客户端指针
     *       [IN]     req           请求内容
     *       [IN]     strCgi        请求的CGI
     *       [OUT]    szResp        HTTP返回结果缓冲区,大小由cbResp指定
     *       [IN/OUT] cbResp        HTTP缓冲区的大小，在config.xml的 /main/service/param/http/resp_size 指定
     *                              返回输出缓冲区的大小，0表示不需要返回结果
     *       [OUT]    ext_headers   HTTP返回结果的头定义，如指定json格式等等
     * 返回值：0    不需要对szResp进行返回处理
     *         >0   HTTP返回的状态码，成功返回200, 前端错误返回400等，服务端错误返回500等
     *         <0   出错，直接断开HTTP连接
     */
    virtual int32_t ExecuteCGI(CClient *pClient, http::CHttpRequest &req, 
            const CMyString &strCgi, char *szResp, uint32_t &cbResp, 
            STRING_MAP &ext_headers)
    {
        return -1;
    }

    virtual int32_t Multicast(std::vector<int64_t> &vecClient, 
            void *pData, size_t cbData)  
    { 
        return 0; 
    }

    virtual int32_t Broadcast(void *pData, size_t cbData) 
    { 
        return 0; 
    }

    // 释放client
    virtual void closeClient(CClient *pClient) = 0;

    // 获得客户端的CClient指针
    virtual CClient* Find(int64_t clientId) = 0;

    // 注册CClient断开的事件
    virtual void registerCloseEvent(PFN_CLOSE_CB pFn) {  }
};

static IClientMgr* s_mgrClient;


