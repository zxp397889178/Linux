/*
 * =====================================================================================
 *
 *       Filename:  TwilioWrapper.h
 *
 *    Description:  针对Twilio.com中的部份API的封装
 *
 *        Version:  1.0
 *        Created:  2020年08月29日 18时32分42秒
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

class CTwilioWrapper
{
public:
    CTwilioWrapper();
    ~CTwilioWrapper();

public:
    void setToken(const CMyString &sid, const CMyString &token);

    int32_t sendMsg(const CMyString &from, const CMyString &to, const CMyString &note, const CMyString &refer);

private:
    CMyString       m_twilioUrl;
    CMyString       m_accountSid;
    CMyString       m_strAuthHeader;
};

