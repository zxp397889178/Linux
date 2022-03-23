/*
 * =====================================================================================
 *
 *       Filename:  Weather.h
 *
 *    Description:  天气API，仅用于好玩
 *
 *        Version:  1.0
 *        Created:  2020年08月15日 17时10分20秒
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
class CWeather
{
public:
    CWeather();
    ~CWeather();

    // https://www.tianqiapi.com/index/doc
    int32_t getById(uint32_t cityId, Json::Value &resp);
    int32_t getByIp(const CMyString &strIp, Json::Value &resp);

private:
    CMyString   m_appId;
    CMyString   m_appKey;
};

