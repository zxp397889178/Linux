/*
 * =====================================================================================
 *
 *       Filename:  TwilioWrapper.cpp
 *
 *    Description:  针对Twilio.com的部份API进行封装
 *
 *        Version:  1.0
 *        Created:  2020年08月29日 18时33分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "TwilioWrapper.h"
#include "MyNet.h"
#include "Security.h"

CTwilioWrapper::CTwilioWrapper()
{
    m_twilioUrl = "https://api.twilio.com/2010-04-01/Accounts";
}

CTwilioWrapper::~CTwilioWrapper()
{

}

void CTwilioWrapper::setToken(const CMyString &sid, const CMyString &token)
{
    // 对sid 与 token 进行加密
    // https://blog.csdn.net/zxw75192/article/details/91664061
    CMyString strTemp("%s:%s", sid.c_str(), token.c_str());
    CMyString strB64 = CBase64Wrapper::Encode(strTemp.c_str(), strTemp.length(), false);
    m_strAuthHeader.Format("Basic %s", strB64.c_str());
    m_accountSid = sid;
}

int32_t CTwilioWrapper::sendMsg(const CMyString &from, const CMyString &to, const CMyString &note, const CMyString &refer)
{
    CMyString strUrl("%s/%s/Message.json", m_twilioUrl.c_str(), m_accountSid.c_str());
    http::VEC_STRING headers;
    CMyString auth_str("Authorization: %s", m_strAuthHeader.c_str());
    headers.push_back(auth_str);

    CMyString strFrom("From=whatsapp:%s", from.c_str());
    CMyString strTo("To=whatsapp:%s", to.c_str());
    CMyString strNote("Body=%s", note.c_str());

    strNote = CHttpWrapper::encodeURIComponentEx(strNote, true);
    strFrom = CHttpWrapper::encodeURIComponentEx(strFrom, true);
    strTo = CHttpWrapper::encodeURIComponentEx(strTo, true);
    CMyString strData("%s&%s&%s", strFrom.c_str(), strTo.c_str(), strNote.c_str());

    http::CHttpClient net;
    CMyString strHeader;
    CMyString strResp;
    int32_t ret = net.HttpPostWithHeaders(strUrl.c_str(), strResp, strHeader, headers, strData.c_str());
    if (ret != 0)
    {
        //resp => code , message
        LogErr("twilio header: %s", strHeader.c_str());
        LogErr("twilio resp  : %s", strData.c_str());
        return -1;
    }
    return 0;
}


