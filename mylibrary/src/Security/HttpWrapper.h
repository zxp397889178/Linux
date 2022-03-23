/*
 * =====================================================================================
 *
 *       Filename:  HttpWrapper.h
 *
 *    Description:  encode & decode for http use
 *
 *        Version:  1.0
 *        Created:  2019年09月03日 11时50分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __SECURITY_LIBRARY_HTTPWRAPPER_H__
#define __SECURITY_LIBRARY_HTTPWRAPPER_H__

#include "ISecurity.h"

class CHttpWrapper
{
// public:
//    // 以下编解码对中文有误
//    static CMyString UrlEncode(CMyString& input);
//    static CMyString UrlDecode(CMyString& cipher);
//
public:
    // 支持中文的编解码
    // https://blog.csdn.net/hankai1024/article/details/8233366
    static CMyString urlEncode(CMyString &input);
    static bool urlDecode(const CMyString &cipher, CMyString &result);

    // 以下为 javaScript 等同的编码方式
    // https://www.cnblogs.com/liuhongfeng/p/5006341.html
    
    // 安全字符： */@+-._0-9a-zA-Z 
    static CMyString escape(const CMyString &input);
    // 安全字符： !#$&'()*+,/:;=?@-._~0-9a-zA-Z
    static CMyString encodeURI(const CMyString &input);
    // 安全字符：!'()*-._~0-9a-zA-Z
    static CMyString encodeURIComponent(const CMyString &input);

    static CMyString encodeURIComponentEx(const CMyString &input, bool bUpperCase);

public:
    // 扩展的URL编解码，处理中文问题
    // https://blog.csdn.net/hankai1024/article/details/8233824

    // utf8 => unicode
    static void UTF_8ToUnicode(wchar_t* pOut,char *pText);

    // unicode => utf8
    static void UnicodeToUTF_8(char* pOut,wchar_t* pText);

    // unicode => gb2312
    // gb2312 => unicode
    // gb2312 => utf8
    // utf8 => gb2312


};

#endif // __SECURITY_LIBRARY_HTTPWRAPPER_H__
