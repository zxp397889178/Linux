/*
 * =====================================================================================
 *
 *       Filename:  AlertLog.h
 *
 *    Description:  含告警信息的记录日志
 *
 *        Version:  1.0
 *        Created:  2021年08月30日 13时54分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"

class CAlertLog
{
public:
    ~CAlertLog();
    static CAlertLog& Instance();

    bool Init(CXmlFile &xml);
    bool Init(const CMyString &appName, const CMyString &appKey, const CMyString &alertUrl, const CMyString &alertIds);

    bool AlertFatal(const char *sFormat, ...);
    bool AlertErr(const char *sFormat, ...);
    bool AlertWarn(const char *sFormat, ...);

private:
    CAlertLog();
    void InitProcessInfo(void);
    bool SendMsg(const CMyString &strMsg);

private:
    CMyString       m_appName;
    CMyString       m_appKey; 
    CMyString       m_alertUrl;
    CMyString       m_alertIds;

    bool            m_bActive;
private:
    CMyString       m_Name;
    CMyString       m_Path;
    CMyString       m_Ip;
};

#define ALERT_LOG       CAlertLog::Instance()

#define LogWarnAlt      CAlertLog::Instance().AlertWarn
#define LogErrAlt       CAlertLog::Instance().AlertErr
#define LogFatalAlt     CAlertLog::Instance().AlertFatal

