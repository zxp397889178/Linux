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

#include "BaseCode.h"
#include "MyDB.h"
#include "MyNet.h"
#include "pay_error.h"

class IClient;

namespace payframe {

    /*
enum payfram_error
{
    err_OK = 0,

    err_payframe_max = -1300,
    err_SystemFault,                    // 系统错误
    err_MissingParameters,              // 缺少必要的参数
    err_NotValidGame,                   // 游戏代码不合法
    err_SignMismatch,                   // 签名出错
    err_NotAvaiablePayChannel,          // 没有可用的支付渠道
    err_ConfigMissingParam,             // config.xml中有缺失参数
    err_MysqlOpenFailure,               // mysql 打开失败
    err_MysqlQueryFault,                // mysql 查询语句错误
};
*/

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


// 判断Content-Type 是否为文本类型
bool IsTextType(const CMyString &strContType);

/* 以chunked 模式发送http的返回信息
 * 参数： pClient       客户端对象指针
 *        pData         数据缓存指针
 *        cbData        数据块大小，超过 HTTPRESP_MIN_BUFFER 后自动启用压缩
 *        ver           HTTP版本号，一般填 1.1
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

}
