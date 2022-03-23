/*
 * =====================================================================================
 *
 *       Filename:  GeoIPWrapper.h
 *
 *    Description:  对GeoIP数据库的封装处理
 *
 *        Version:  1.0
 *        Created:  2021年07月17日 22时56分47秒
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

struct mmdb_t;

class CGeoIPWrapper
{
public:
    CGeoIPWrapper();
    ~CGeoIPWrapper();

    bool Open(const CMyString &mmdb_file);

    bool Lookup(const CMyString &ip, Json::Value &resp, const CMyString &region="CN");

private:
    mmdb_t*         m_pHandler;
};

