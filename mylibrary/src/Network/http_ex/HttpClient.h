/*
 * =====================================================================================
 *
 *       Filename:  HttpClient.h
 *
 *    Description:  HTTP 客户端，基于 TcpClient
 *
 *        Version:  1.0
 *        Created:  2020年02月21日 19时00分05秒
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
#include <vector>
#include "../http/HttpData.h"
#include "../http/HttpRespone.h"

namespace httpex {

enum error_code
{
    errOK = 0,
    errUrlInvalid = 100,    // URL 地址不合法
    errUnsupportMethod,     // 方法不支持
    errHostUnreach,         // 服务器不可到达
    errSendHeaderFailure,   // 网络错误: 发送header 失败
    errSendDataFailure,     // 网络错误: 发送data 失败
    errRecvDataFailure,     // 网络错误: 接收resp 失败
};

class CHttpClient
{
    struct HeaderItem
    {
        CMyString       key;
        CMyString       value;
    };
    typedef std::vector<HeaderItem>     VEC_HEADER;
    typedef HeaderItem  ParamItem;
    typedef std::vector<ParamItem>      VEC_PARAM;

    enum Method
    {
        GET = 0,
        POST,
        OPTIONS,
        HEAD,
        PUT,
        DELETE,
        TRACE,
    };
public:
    CHttpClient();
    ~CHttpClient();

    // 执行 GET 方法
    // header: key1:value1;key2:value2;...
    int32_t Get(const CMyString &strHost, const CMyString &strHeader, http::CHttpRespone &resp);

    // 执行 POST 方法
    // header: key1:value1;key2:value2;...
    int32_t Post(const CMyString &strHost, const CMyString &strHeader, byte_t* pData, size_t cbData, http::CHttpRespone &resp);

    // 执行Http请求
    // 返回：<0  执行异常
    //        0  执行成功
    //      resp HTTP返回结果
    int32_t Exec(http::CHttpRespone &resp);

public:
    // 设计主机地址
    bool setHost(const CMyString &host);

    // 设计访问方法： GET POST
    bool setMethod(const CMyString &method);

    // 是否保持连接，目前暂未使用，一次执行完毕后就断开
    void setKeepAlive(bool keep_alive);

    // 设置多个header,字符串以;分隔
    // headers => key1:value1;key2:value2;...
    int32_t addHeader(const CMyString &headers, bool bOverride=true);
    // 设置单个header k : value
    // 返回值： 0 已存在key, 不作替换
    //          1 设置成功，新增或替换
    int32_t addHeader(const CMyString &key, const CMyString &value, bool bOverride=true);

    // 增加多个parameter, 字符串以&分隔
    // params => key1=value1&key2=value2&...
    int32_t addParam(const CMyString &params, bool bOverride=true);
    // 增加单个param k: v
    // 返回值： 0 已存在key, 不作替换
    //          1 设置成功，新增或替换
    int32_t addParam(const CMyString &key, const CMyString &value, bool bOverride=true);

    int32_t addCookie(const CMyString &cookies, bool bOverride = true);
    int32_t addCookie(const CMyString &key, const CMyString &value, bool bOverride=true);

    // 增加数据
    /* 增加post数据
     * 参数: pData          => 需要加入到post的数据
     *       cbData         => pData 的长度
     *       strExtraInfo   => 扩展信息，仅对 multipart/form-data 格式生效,类似于下面的参数
     *                         Content-Disposition: form-data;name="desc";filename="abc.txt"\r\n
     *                         Content-Type: text/plain; charset=UTF-8\r\n
     *                         Content-Transfer-Encoding: 8bit\r\n
     *       bLast          => 是否最后一个post, 多用于 multipart/form-data 传输
     *  !!!注意!!! addPost 必须在addHeader 之后
     */
    void addPost(byte_t *pData, size_t cbData, const CMyString &strExtraInfo, bool bLast=true);

    // 获得错误号
    enum error_code getLastError(void) { return m_errno; }
    // 获得错误内容描述
    const char* getLastErrorNote(void) { return m_errNote.c_str(); }

private:
    int32_t build_header(CMyString &strHeader);

    void build_param(CMyString &strParam);

    /* 解析 key-value 字符串
     * key-value 之间以 delimiter为分隔符
     * 返回值： 0  解析成功
     *          <0 失败， key-value 字符串不合法
     */
    int32_t decode_kvstring(const CMyString &kvstring, char delimiter, CMyString &key, CMyString &value);


    /* 增加 多个key-value 到 容器 cont 中
     * key-value 之间以 delimiter1 分隔
     * key-value 内部以 delimiter2 分隔
     * bOverride 指示，若key相同时，是否替换旧值, true 表示替换
     * 返回值：写入到cont中key-value数，含替换的值
     */
    int32_t addKvString(VEC_HEADER &cont, const CMyString &kv_strings, char delimiter1, char delimiter2, bool bOverride);
    /* 增加一个 key-value 到容器 cont 中
     * bOverride 指示，若key相同时，是否替换旧值, true 表示替换
     * 返回值：
     *      1 写入到cont中key-value数，含替换的值
     *      0 已存在旧值，不作替换
     */
    int32_t addKvItem(VEC_HEADER &cont, const CMyString &key, const CMyString &value, bool bOverride);

private:
    bool getHeader(const CMyString &key, CMyString &value);
    void removeHeader(const CMyString &key);
    void buildContentLength(void);

    void parseUrlEncodeParam(byte_t *pData, size_t cbData, CMyString &strOut);

    bool IsChunkedMode(void);
    bool IsMultipartMode(CMyString &strBoundary);
    bool IsUrlEncodeParam(void);
    void BuildBoundary(CMyString &strBoundary);

private:
    CMyString           m_strHost;      // http://ip:port/api/upload?filename=...
    CMyString           m_strMethod;
    enum Method         m_method;
    bool                m_bKeepAlive;
    VEC_HEADER          m_vecHeader;    // header
    VEC_PARAM           m_vecParam;     // param
    VEC_PARAM           m_vecCookie;    // cookie
    CByteStream         m_data;         // data (for post)

    enum error_code     m_errno;
    CMyString           m_errNote;
};


} // namespace httpex


