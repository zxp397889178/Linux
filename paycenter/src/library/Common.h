/*
 * =====================================================================================
 *
 *       Filename:  Common.h
 *
 *    Description:  公用函数
 *
 *        Version:  1.0
 *        Created:  03/04/2020 06:49:29 PM
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
#include "MyDB.h"
#include "MyNet.h"
#include "Config.h"

class IClient;

int32_t open_mysql(const CMyString &strCfgName, ISimDB *&pDB);
int32_t open_mysql(const CMyString &strSect, const CMyString &strKey, ISimDB *&pDB);

int32_t open_redis(const CMyString &strCfgName, CMyRedis &redis);
int32_t open_redis(const CMyString &strSect, const CMyString &strKey, CMyRedis &redis);

bool getConfigInt(const CMyString sect_, const CMyString key_, int32_t &val);
bool getConfigStr(const CMyString sect_, const CMyString key_, CMyString &val);
bool getConfigBool(const CMyString sect_, const CMyString key_, bool &val);
bool getConfigDouble(const CMyString sect_, const CMyString key_, double &val);

/*
<main>
    <clientIp source="header" 
        source = [net|header|param|cookie]
            net 是指从IClient* 获取， 如 pClient->getIpStr()，忽略 prop
            header 是指从 CHttpRequest 的 header中获取, prop是指名称
            param  是指从 CHttpRequest 的 param 中获取，prop是指名称
        prop="X-Real-IP" />
</main>
*/
CMyString getClientIp(IClient *pClient, http::CHttpRequest &req);

/* type : 获取IP的方式  
 *      0-从pClient中获得IP; 
 *      1-从req的header中获得IP， 
 *      2-从req的param中获得IP,
 *      3-从cookie中获得IP
 */
enum getIpStyle
{
    getIpFromConnect = 0,
    getIpFromHeader,
    getIpFromParam,
    getIpFromCookie
};
CMyString getClientIpEx(int32_t type, IClient *pClient, http::CHttpRequest &req, const CMyString &ipField);

// 判断Content-Type 是否为文本类型
bool IsTextType(const CMyString &strContType);

/* 以chunked 模式发送http的返回信息
 * 参数： pClient       客户端对象指针
 *        pData         数据缓存指针
 *        cbData        数据块大小，超过 HTTPRESP_MIN_BUFFER 后自动启用压缩
 *        ver           HTTP版本号，一般填 "HTTP/1.1"
 *        code          HTTP返回码，成功填200
 *        status        对code的简要描述
 *        strContType   返回内容的类型
 *        strSvrInfo    服务器信息
 * 返回值：
 *        < 0   错误，具本可参见错误描述
 *        = 0   发送失败
 *        > 0   发送的总字节数 
 */
int32_t send_chunked_data(IClient *pClient, byte_t *pData, size_t cbData, 
        const CMyString &ver, int32_t code, const CMyString &status, 
        const CMyString &strContType, const CMyString &strSvrInfo);

/* 以chunked 模式发送文件
 * 参数： pClient       客户端对象指针
 *        strFile       需要传送的文件名
 *        bTrailer      是否在尾部带上MD5信息
 *        bDownload     是否下载文件
 *        strSvrInfo    服务器信息
 * 返回值：
 *        < 0   错误，具本可参见错误描述
 *        = 0   发送失败
 *        > 0   发送的总字节数 
 */
int32_t send_chunked_file(IClient *pClient,
        const CMyString &strFile, bool bTrailer, bool bDownload, 
        const CMyString &strSvrInfo);



enum SiblingCommand
{
    sc_reloadAll        = 0,    // 全部重载
    sc_reloadConfig,            // config.xml 重载
    sc_increaseBillpoint,       // 修改计费点使用
    sc_decreaseBillpoint,
};
int32_t Broadcast2Sibling(int32_t cmd, Json::Value &data);
