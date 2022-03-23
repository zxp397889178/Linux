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
#include "IWebPlugin.h"
#include "IClient.h"
#include "IWebClientMgr.h"
#include "Config.h"
#include "pay_error.h"

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

