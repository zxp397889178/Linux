/*
 * =====================================================================================
 *
 *       Filename:  HttpRespMaker.h
 *
 *    Description:  make respone data
 *
 *        Version:  1.0
 *        Created:  11/04/2019 06:47:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __QW_NETWORK_LIB_HTTP_RESPONEDATA_MAKER_H__
#define __QW_NETWORK_LIB_HTTP_RESPONEDATA_MAKER_H__

#pragma once

#include "BaseCode.h"
#include <map>
#include <vector>

namespace http {

class CHttpRespMaker
{
    struct KvItem
    {
        CMyString key;
        CMyString val;
        char      cType;    // 'C' - 字符型   'I'-有符号整型, 'N'-无符号整型  'F' - 浮点/双精度型, 'B' - boolean 型
        KvItem(const CMyString &_key, const CMyString &_val, char _type)
            : key(_key), val(_val), cType(_type)
        {  }
        KvItem(const CMyString &_key, const CMyString &_val)
            : key(_key), val(_val), cType('C')
        { }
        KvItem(const CMyString &_key, int64_t _val)
            : key(_key), cType('I')
        {
            val.Format("%ld", _val);
        }
        KvItem(const CMyString &_key, uint64_t _val)
            : key(_key), cType('N')
        {
            val.Format("%lu", _val);
        }
        KvItem(const CMyString &_key, int32_t _val)
            : key(_key), cType('I')
        {
            val.Format("%d", _val);
        }
        KvItem(const CMyString &_key, uint32_t _val)
            : key(_key), cType('N')
        {
            val.Format("%u", _val);
        }
        KvItem(const CMyString &_key, double _val)
            : key(_key), cType('F')
        {
            val.Format("%lf", _val);
        }
        KvItem(const CMyString &_key, bool _val)
            : key(_key), cType('B')
        {
            val.Format("%s", _val ? "true" : "false");
        }
    };
    typedef std::vector<KvItem*>         KV_VECTOR;
    typedef std::map<CMyString, KvItem*> KV_MAP;
public:
    CHttpRespMaker();
    ~CHttpRespMaker();

public:
    bool AddJsonStr(const CMyString &strJson);
    void AddJson(Json::Value &root);
    void AddUrlParam(const CMyString &strParam);

    void add(const CMyString &key, const CMyString &val);
    void add(const CMyString &key, const char* val);
    void add(const CMyString &key, int32_t val);
    void add(const CMyString &key, uint32_t val);
    void add(const CMyString &key, int64_t val);
    void add(const CMyString &key, uint64_t val);
    void add(const CMyString &key, double val);
    void add(const CMyString &key, bool val);

    // 修改字段的类型,仅用于需要返回json格式的环境
    //  'C' - 字符型   'I'-有符号整型, 'N'-无符号整型  'F' - 浮点/双精度型, 'B' - boolean 型
    bool setFieldType(const CMyString &key, char cType);
     
    // 删除某个key
    void erase(const CMyString &key);

    // 返回类似 key1=val1&key2=val2&key3=val3...
    CMyString http_str(bool bSort = true);
    /* cGroupSpliter 分组的分隔符，一般为&, \0表示无组分隔符
     * cKvSplit 同一组的KV的分隔符，一般为= , \0表示无分隔符
     * ignoreEmpty 值为空时，不取出来
     * bSort 是否排序方式获取
     */
    CMyString http_str_manual(char cGroupSpliter, char cKvSpliter, bool ignoreEmpty, bool bSort, bool bUrlEncode=false);
    // 仅对值进行排序，忽略KEY
    CMyString http_str_values(char cGroupSpliter, bool bSort, bool bUrlEncode=false);

    // 返回类似 {"key1":"val1", "key2":"val2", "key3":"val3",...} 
    CMyString json_str_styled(bool bStyled = true);
    // 返回顺序清加的json_string
    CMyString json_str(bool bSort = true);

    /* 对 http_str 进行加密， extraData用于附加在 http_str() 之后进行加密的
     * 加密方法为： md5( http_str(bSort) + extraData )
     * 返回加密后的结果
     */
    CMyString sign_by_md5(const CMyString &signName, const CMyString &extraData, bool bSort, bool bLowercase);

    // 对 http_str() 进行MD5签名
    CMyString signWithMd5(const CMyString &signName, bool bLowerCase);

    // 按照加入的先后顺序进行 MD5签名
    CMyString signWithMd5NoSort(const CMyString &signName, bool bLowerCase, const char *pszExtraKey=NULL);

private:
    KvItem* find(const CMyString &key);
    void clear(void);
    void add_ex(const CMyString &key, const CMyString &val, char cType, bool bFormatted = true);

private:
    KV_VECTOR   m_vecKV;
    KV_MAP      m_mapKV;
};

}

#endif // __QW_NETWORK_LIB_HTTP_RESPONEDATA_MAKER_H__
