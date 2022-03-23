/*
 * =====================================================================================
 *
 *       Filename:  Weather.cpp
 *
 *    Description:  天气API,仅用于好玩 (https://www.tianqiapi.com)
 *
 *        Version:  1.0
 *        Created:  2020年08月15日 17时11分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Weather.h"
//#include "http/HttpClient.h"
#include "MyNet.h"

CWeather::CWeather()
{
    m_appId = "82213898";
    m_appKey = "9C1C1LUZ";
}

CWeather::~CWeather()
{

}

int32_t CWeather::getById(uint32_t cityId, Json::Value &resp)
{
    static const CMyString s_url = "https://yiketianqi.com/api";

    http::CHttpClient net;
    CMyString strUrl("%s?version=v61&appid=%s&appsecret=%s&cityid=%u",
            s_url.c_str(), m_appId.c_str(), m_appKey.c_str(), cityId);
    std::string strResp;
    std::string strHeader;
    int32_t ret = net.HttpGet(strUrl.c_str(), strResp, strHeader);
    LogDebug("[weath header]%s", strHeader.c_str());
    LogDebug("[weath resp  ]%s", strResp.c_str());
    if (ret != 0)
    {
        return -1;
    }

    CJsonWrapper jr;
    if (!jr.Load(strResp))
    {
        return -2;
    }
    resp = jr.getJson();

    return 0;
}

int32_t CWeather::getByIp(const CMyString &strIp, Json::Value &resp)
{
    static const CMyString s_url = "https://yiketianqi.com/api";

    http::CHttpClient net;
    CMyString strUrl("%s?version=v61&appid=%s&appsecret=%s&ip=%s",
            s_url.c_str(), m_appId.c_str(), m_appKey.c_str(), strIp.c_str());
    std::string strResp;
    std::string strHeader;
    int32_t ret = net.HttpGet(strUrl.c_str(), strResp, strHeader);
    LogDebug("[weath header]%s", strHeader.c_str());
    LogDebug("[weath resp  ]%s", strResp.c_str());
    if (ret != 0)
    {
        return -1;
    }
    
    CJsonWrapper jr;
    if (!jr.Load(strResp))
    {
        return -2;
    }
    resp = jr.getJson();

    return 0;
}


