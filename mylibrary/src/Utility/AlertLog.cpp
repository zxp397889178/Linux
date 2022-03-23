/*
 * =====================================================================================
 *
 *       Filename:  AlertLog.cpp
 *
 *    Description:  含警告信息的记录日志
 *
 *        Version:  1.0
 *        Created:  2021年08月30日 13时56分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "AlertLog.h"
#include "MyNet.h"
#include "Security.h"
#include "Process.h"

using namespace Comm;
static __thread char staAlertLogBuff[MAX_RICHLOG_LINE + 8] = {0};

CAlertLog& CAlertLog::Instance()
{
    static CAlertLog s_obj;
    return s_obj;
}

CAlertLog::CAlertLog()
    : m_bActive(false)
{

}

CAlertLog::~CAlertLog()
{

}

bool CAlertLog::Init(CXmlFile &xml)
{
    static const char *sect_ = "main/service/alert";

    int32_t ret = xml.getString(sect_, "appName", m_appName);
    if (ret != 0)
    {
        return false;
    }
    ret = xml.getString(sect_, "appKey", m_appKey);
    if (ret != 0)
    {
        return false;
    }
    ret = xml.getString(sect_, "url", m_alertUrl);
    if (ret != 0)
    {
        return false;
    }
    ret = xml.getString(sect_, "ids", m_alertIds);
    if (ret != 0)
    {
        return false;
    }

    if (m_appName.empty() || m_appKey.empty() 
            || m_alertUrl.empty() || m_alertIds.empty())
    {
        return false;
    }

    this->InitProcessInfo();

    m_bActive = true;
    return true;
}

bool CAlertLog::Init(const CMyString &appName, const CMyString &appKey, const CMyString &alertUrl, const CMyString &alertIds)
{
    m_appName = appName;
    m_appKey = appKey;
    m_alertUrl = alertUrl;
    m_alertIds = alertIds;
    if (m_appName.empty() || m_appKey.empty() 
            || m_alertUrl.empty() || m_alertIds.empty())
    {
        return false;
    }

    this->InitProcessInfo();
    m_bActive = true;
    return true;
}

bool CAlertLog::SendMsg(const CMyString &strMsg)
{
    if (!m_bActive)
    {
        LogFatal("AlertLog not active.");
        return false;
    }

    char szTime[128] = "";
    TimeToStr(time(NULL), szTime, sizeof(szTime));

    Json::Value req;
    req["appName"] = m_appName;
    req["chatIds"] = m_alertIds;
    CMyString strText("Time:%s, Ip:%s, Process:%s, Path:%s\r\n%s", 
            szTime, m_Ip.c_str(), m_Name.c_str(), m_Path.c_str(), strMsg.c_str());
    req["message"] = strText;
    LogDebug("ip:%s, msg:%s", m_Ip.c_str(), strText.c_str());

    // 加签名
    http::CHttpRespMaker req_maker;
    req_maker.AddJson(req);
    CMyString str1 = req_maker.http_str_manual('&', '=', false, true, false);
    str1.Append("&mch_key=%s", m_appKey.c_str());
    CMyString strSign = CMD5Wrapper::encode(str1.c_str());
    strSign.MakeLower();
    req["sign"] = strSign;

    CJsonWrapper jw(req);
    CMyString strData = jw.toString();

    CMyString strResp;
    CMyString strRespHeader;
    http::VEC_STRING headers;
    headers.push_back("Content-Type: application/json; charset=utf-8");
    http::CHttpClient net;
    int32_t ret = net.HttpPostWithHeaders(m_alertUrl.c_str(), strResp, strRespHeader, headers, strData.c_str());
    if (ret>0)
    {
        CMyString strErr;
        if (net.getError(ret, strErr))
        {
            LogErr("alert message [%s] failure, error:%s", strMsg.c_str(), strErr.c_str());
        }
        else
        {
            LogErr("alert message [%s] failure, errno:%d", strMsg.c_str(), ret);
        }
        return false;
    }
    else if (ret<0)
    {
        LogErr("alert message [%s] failure, errno:%d", strMsg.c_str(), ret);
        return false;
    }
    LogDebug("Alert message reply:[Data]%s\n[Header]%s", strResp.c_str(), strRespHeader.c_str());
    return true;
}


bool CAlertLog::AlertFatal(const char *sFormat, ...)
{
	va_list args;
	va_start(args, sFormat);
	vsnprintf(staAlertLogBuff, MAX_RICHLOG_LINE, sFormat, args);
	va_end(args);

	LogErr("[ASSERT]\033[41;37m%s\033[0m", staAlertLogBuff);
    printf("[FATAL]\033[41;37m%s\033[0m\n", staAlertLogBuff);

    CMyString strNote = "[FATAL]" + CMyString(staAlertLogBuff);
    return this->SendMsg(strNote);
}

bool CAlertLog::AlertErr(const char *sFormat, ...)
{
	va_list args;
	va_start(args, sFormat);
	vsnprintf(staAlertLogBuff, MAX_RICHLOG_LINE, sFormat, args);
	va_end(args);

    // 黑底亮红字
	LogErr("[ERR]\033[40;31;1m%s\033[0m", staAlertLogBuff);
    printf("[ERROR]\033[40;31;1m%s\033[0m\n", staAlertLogBuff);

    CMyString strNote = "[ERROR]" + CMyString(staAlertLogBuff);
    return this->SendMsg(strNote);
}

bool CAlertLog::AlertWarn(const char *sFormat, ...)
{
	va_list args;
	va_start(args, sFormat);
	vsnprintf(staAlertLogBuff, MAX_RICHLOG_LINE, sFormat, args);
	va_end(args);

    // 黑底亮黄字
	LogErr("[WRN]\033[40;33;1m%s\033[0m", staAlertLogBuff);
    printf("[WARN]\033[40;33;1m%s\033[0m\n", staAlertLogBuff);

    CMyString strNote = "[WARN]" + CMyString(staAlertLogBuff);
    return this->SendMsg(strNote);
}

void CAlertLog::InitProcessInfo(void)
{
    static const char *s_url = "http://api.ipify.org";

    getProcessInfoByPid(getpid(), m_Path, m_Name);

    http::CHttpClient net;
    CMyString strHeader;
    std::string strIp;
    if (0==net.HttpGet(s_url, strIp, strHeader))
    {
        m_Ip = strIp;
    }
}


