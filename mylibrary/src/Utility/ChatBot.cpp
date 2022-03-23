/*
 * =====================================================================================
 *
 *       Filename:  ChatBot.cpp
 *
 *    Description:  聊天机器人实现
 *
 *        Version:  1.0
 *        Created:  2020年07月21日 14时28分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "ChatBot.h"
#include "MyNet.h"
//#include "http/HttpClient.h"
//#include "http/HttpRespone.h"
#include "Security.h"
#include <curl/curl.h>
#include <math.h>
#include "BaseCode/JsonReader.h"

CChatBot::CChatBot()
{
    http::CHttpClient::GlobalInit();
}

CChatBot::~CChatBot()
{
    http::CHttpClient::GlobalFini();
}

void CChatBot::setDingtalkToken(const CMyString &strToken, const CMyString &strKey)
{
    m_dingtalk_token = strToken;
    m_dingtalk_key = strKey;
}

int32_t CChatBot::dingtalk(const CMyString &content, const CMyString &atMobiles)
{
    // refer to: http://www.mamicode.com/info-detail-2742576.html
    // https://ding-doc.dingtalk.com/doc#/serverapi2/qf2nxq/XAzBI
    static CMyString s_dingtalk_url("https://oapi.dingtalk.com/robot/send");

    if (m_dingtalk_token.empty())
    {
        LogErr("dingtalk access token is empty.");
        return -1001;
    }

    // 构建url (有可能要加密)
    http::CHttpClient net;
    CMyString strUrl;
    if (m_dingtalk_key.empty())
    {
        strUrl.Format("%s?access_token=%s", s_dingtalk_url.c_str(), m_dingtalk_token.c_str());
    }
    else
    {
        uint8_t cipher[256] = {0,};
        size_t cbCipher(256);
        uint64_t timestamp = Now(false);
        CMyString strOrigin("%llu\n%s", timestamp, m_dingtalk_key.c_str());
        int32_t ret = ShaWrapper::Hmac("sha256", m_dingtalk_key.c_str(), strOrigin.c_str(), cipher, cbCipher);
        if (ret != 0)
        {
            return -1003; // 加密KEY出错
        }
        CMyString strHamcSha256Base64 = CBase64Wrapper::Encode((const char*)cipher, cbCipher, false);
        CMyString strCipher = CHttpWrapper::encodeURIComponentEx(strHamcSha256Base64, false);
        strUrl.Format("%s?access_token=%s&timestamp=%llu&sign=%s",
                s_dingtalk_url.c_str(), m_dingtalk_token.c_str(), 
                timestamp, strCipher.c_str());
        //LogDebug("origin:             [%s]", strOrigin.c_str());
        //LogDebug("Base64(HamcSha256): [%s]", strHamcSha256Base64.c_str());
        //LogDebug("sign:               [%s]", strCipher.c_str());
    }
    //LogDebug("url: %s", strUrl.c_str());

    // 构建dingtalk发送消息列表
    CStrSpliter css;
    css.Split(atMobiles, ',');
    Json::Value dingtalk_data;
    dingtalk_data["msgtype"] = "text";
    Json::Value dingtalk_text;
    if (css.size()==0)
    {
        dingtalk_text["content"] = content;
    }
    else
    {
        Json::Value dingtalk_atarr;
        CMyString strTemp = content;
        for(int32_t i(0); i<css.size(); ++i)
        {
            CMyString mob = css[i];
            strTemp.Append("@%s", mob.c_str());
            dingtalk_atarr.append(mob);
        }
        dingtalk_text["content"] = strTemp;
        Json::Value dingtalk_at;
        dingtalk_at["atMobiles"] = dingtalk_atarr;
        dingtalk_at["isAtAll"] = "false";
        dingtalk_data["at"] = dingtalk_at;
    }
    dingtalk_data["text"] = dingtalk_text;
    CJsonWrapper writer;
    writer.Load(dingtalk_data);
    CMyString strData = writer.toString();
    //printf("%s\n", strData.c_str());

    // 发送HTTP POST 请求
    CMyString strHeader("Content-Type: application/json");
    http::VEC_STRING header_list;
    header_list.push_back(strHeader);
    CMyString strResp;
    CMyString strRespHeader;
    int32_t ret = net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list, strData.c_str());
    if (ret != 0)
    {
        // 解析curl错误
        LogDebug("dingtalk-header:%s", strRespHeader.c_str());
        LogDebug("dingtalk-data  :%s", strResp.c_str());
        LogErr("curl error when send telegram msg, %s", curl_easy_strerror((CURLcode)ret));
        return -ret;
    }

    // 解析返回内容
    LogDebug("dingtalk-header:%s", strRespHeader.c_str());
    LogDebug("dingtalk-data  :%s", strResp.c_str());

    http::CHttpRespone resp;
    resp.read((void*)strRespHeader.c_str(), strRespHeader.length());
    CMyString strContType = resp.getHeader("Content-Type");
    if (strContType == "application/json")
    {
        CJsonWrapper jr;
        if (!jr.Load(strResp))
        {
            LogErr("dingtalk return data [%s] not valid json string.", strResp.c_str());
            return -1005;  // 返回数据不正确
        }
        int32_t code(0);
        jr.Read("errcode", code);
        if (code != 0)
        {
            CMyString errmsg;
            jr.Read("errmsg", errmsg);
            LogErr("send dingtalk msg failure, error:%d, msg:%s", 
                    code, errmsg.c_str());
            return -abs(code);
        }
    }

    return 0;
}

int32_t CChatBot::dingtalk_link(const CMyString &title, const CMyString &content, const CMyString &picUrl, const CMyString &contentUrl)
{
    // refer to: http://www.mamicode.com/info-detail-2742576.html
    // https://ding-doc.dingtalk.com/doc#/serverapi2/qf2nxq/XAzBI
    static CMyString s_dingtalk_url("https://oapi.dingtalk.com/robot/send");

    if (m_dingtalk_token.empty())
    {
        LogErr("dingtalk access token is empty.");
        return -1001;
    }

    // 构建url (有可能要加密)
    http::CHttpClient net;
    CMyString strUrl;
    if (m_dingtalk_key.empty())
    {
        strUrl.Format("%s?access_token=%s", s_dingtalk_url.c_str(), m_dingtalk_token.c_str());
    }
    else
    {
        uint8_t cipher[256] = {0,};
        size_t cbCipher(256);
        uint64_t timestamp = Now(false);
        CMyString strOrigin("%llu\n%s", timestamp, m_dingtalk_key.c_str());
        int32_t ret = ShaWrapper::Hmac("sha256", m_dingtalk_key.c_str(), strOrigin.c_str(), cipher, cbCipher);
        if (ret != 0)
        {
            return -1003; // 加密KEY出错
        }
        CMyString strHamcSha256Base64 = CBase64Wrapper::Encode((const char*)cipher, cbCipher, false);
        CMyString strCipher = CHttpWrapper::encodeURIComponentEx(strHamcSha256Base64, false);
        strUrl.Format("%s?access_token=%s&timestamp=%llu&sign=%s",
                s_dingtalk_url.c_str(), m_dingtalk_token.c_str(), 
                timestamp, strCipher.c_str());
        //LogDebug("origin:             [%s]", strOrigin.c_str());
        //LogDebug("Base64(HamcSha256): [%s]", strHamcSha256Base64.c_str());
        //LogDebug("sign:               [%s]", strCipher.c_str());
    }
    //LogDebug("url: %s", strUrl.c_str());

    // 构建dingtalk发送消息列表
    Json::Value dingtalk_data;
    dingtalk_data["msgtype"] = "link";
    Json::Value dingtalk_link;
    dingtalk_link["text"] = content;
    dingtalk_link["title"] = title;
    dingtalk_link["picUrl"] = picUrl;
    dingtalk_link["messageUrl"] = contentUrl;
    dingtalk_data["link"] = dingtalk_link;
    CJsonWrapper writer;
    writer.Load(dingtalk_data);
    CMyString strData = writer.toString();
    //printf("%s\n", strData.c_str());

    // 发送HTTP POST 请求
    CMyString strHeader("Content-Type: application/json");
    http::VEC_STRING header_list;
    header_list.push_back(strHeader);
    CMyString strResp;
    CMyString strRespHeader;
    int32_t ret = net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list, strData.c_str());
    if (ret != 0)
    {
        // 解析curl错误
        LogDebug("dingtalk-header:%s", strRespHeader.c_str());
        LogDebug("dingtalk-data  :%s", strResp.c_str());
        LogErr("curl error when send telegram msg, %s", curl_easy_strerror((CURLcode)ret));
        return -ret;
    }

    // 解析返回内容
    LogDebug("dingtalk-header:%s", strRespHeader.c_str());
    LogDebug("dingtalk-data  :%s", strResp.c_str());

    http::CHttpRespone resp;
    resp.read((void*)strRespHeader.c_str(), strRespHeader.length());
    CMyString strContType = resp.getHeader("Content-Type");
    if (strContType == "application/json")
    {
        CJsonWrapper reader;
        if (!reader.Load(strResp))
        {
            LogErr("dingtalk return data [%s] not valid json string.", strResp.c_str());
            return -1005;  // 返回数据不正确
        }
        int32_t code = reader.getInt("errcode");
        if (code != 0)
        {
            LogErr("send dingtalk msg failure, error:%d, msg:%s", 
                    code, reader.getString("errmsg").c_str());
            return -abs(code);
        }
    }
    return 0;
}

int32_t CChatBot::dingtalk_markdown(const CMyString &title, const CMyString &content, const CMyString &atMobiles)
{
    // refer to: http://www.mamicode.com/info-detail-2742576.html
    // https://ding-doc.dingtalk.com/doc#/serverapi2/qf2nxq/XAzBI
    static CMyString s_dingtalk_url("https://oapi.dingtalk.com/robot/send");

    if (m_dingtalk_token.empty())
    {
        LogErr("dingtalk access token is empty.");
        return -1001;
    }

    // 构建url (有可能要加密)
    http::CHttpClient net;
    CMyString strUrl;
    if (m_dingtalk_key.empty())
    {
        strUrl.Format("%s?access_token=%s", s_dingtalk_url.c_str(), m_dingtalk_token.c_str());
    }
    else
    {
        uint8_t cipher[256] = {0,};
        size_t cbCipher(256);
        uint64_t timestamp = Now(false);
        CMyString strOrigin("%llu\n%s", timestamp, m_dingtalk_key.c_str());
        int32_t ret = ShaWrapper::Hmac("sha256", m_dingtalk_key.c_str(), strOrigin.c_str(), cipher, cbCipher);
        if (ret != 0)
        {
            return -1003; // 加密KEY出错
        }
        CMyString strHamcSha256Base64 = CBase64Wrapper::Encode((const char*)cipher, cbCipher, false);
        CMyString strCipher = CHttpWrapper::encodeURIComponentEx(strHamcSha256Base64, false);
        strUrl.Format("%s?access_token=%s&timestamp=%llu&sign=%s",
                s_dingtalk_url.c_str(), m_dingtalk_token.c_str(), 
                timestamp, strCipher.c_str());
        //LogDebug("origin:             [%s]", strOrigin.c_str());
        //LogDebug("Base64(HamcSha256): [%s]", strHamcSha256Base64.c_str());
        //LogDebug("sign:               [%s]", strCipher.c_str());
    }
    //LogDebug("url: %s", strUrl.c_str());

    // 构建dingtalk发送消息列表
    CStrSpliter css;
    css.Split(atMobiles, ',');
    Json::Value dingtalk_data;
    dingtalk_data["msgtype"] = "markdown";
    Json::Value dingtalk_markdown;
    dingtalk_markdown["title"] = title;
    if (css.size()==0)
    {
        dingtalk_markdown["text"] = content;
    }
    else
    {
        Json::Value dingtalk_atarr;
        CMyString strTemp = content;
        for(int32_t i(0); i<css.size(); ++i)
        {
            CMyString mob = css[i];
            strTemp.Append("@%s", mob.c_str());
            dingtalk_atarr.append(mob);
        }
        dingtalk_markdown["text"] = strTemp;

        Json::Value dingtalk_at;
        dingtalk_at["atMobiles"] = dingtalk_atarr;
        dingtalk_at["isAtAll"] = "false";
        dingtalk_data["at"] = dingtalk_at;
    }
    dingtalk_data["markdown"] = dingtalk_markdown;
    CJsonWrapper writer;
    writer.Load(dingtalk_data);
    CMyString strData = writer.toString();
    //printf("%s\n", strData.c_str());

    // 发送HTTP POST 请求
    CMyString strHeader("Content-Type: application/json");
    http::VEC_STRING header_list;
    header_list.push_back(strHeader);
    CMyString strResp;
    CMyString strRespHeader;
    int32_t ret = net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list, strData.c_str());
    if (ret != 0)
    {
        // 解析curl错误
        LogDebug("dingtalk-header:%s", strRespHeader.c_str());
        LogDebug("dingtalk-data  :%s", strResp.c_str());
        LogErr("curl error when send telegram msg, %s", curl_easy_strerror((CURLcode)ret));
        return -ret;
    }

    // 解析返回内容
    LogDebug("dingtalk-header:%s", strRespHeader.c_str());
    LogDebug("dingtalk-data  :%s", strResp.c_str());

    http::CHttpRespone resp;
    resp.read((void*)strRespHeader.c_str(), strRespHeader.length());
    CMyString strContType = resp.getHeader("Content-Type");
    if (strContType == "application/json")
    {
        CJsonWrapper reader;
        if (!reader.Load(strResp))
        {
            LogErr("dingtalk return data [%s] not valid json string.", strResp.c_str());
            return -1005;  // 返回数据不正确
        }
        int32_t code = reader.getInt("errcode");
        if (code != 0)
        {
            LogErr("send dingtalk msg failure, error:%d, msg:%s", 
                    code, reader.getString("errmsg").c_str());
            return -abs(code);
        }
    }

    return 0;

}

void CChatBot::setTelegramToken(const CMyString &strToken)
{
    m_telegram_token = strToken;
}

int32_t CChatBot::telegram(const CMyString &chatId, const CMyString &strText, const CMyString &atUsers)
{
    // refer to:  https://core.telegram.org/bots/api#sendmessage
    if (m_telegram_token.empty())
    {
        LogErr("telegram access token is empty.");
        return -1001;
    }
    http::CHttpClient net;
    
    CMyString strUrl("https://api.telegram.org/bot%s/sendMessage", m_telegram_token.c_str());
    //CMyString strHeader("Content-Type: application/x-www-form-urlencode");
    http::VEC_STRING header_list;
    //header_list.push_back(strHeader);

    CMyString strData("chat_id=%s&text=%s", chatId.c_str(), strText.c_str());
    CStrSpliter css;
    css.Split(atUsers, ',');
    for(int i(0); i<css.size(); ++i)
    {
        strData.Append("@%s", css[i].c_str());
    }

    CMyString strResp;
    CMyString strRespHeader;
    int32_t ret = net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list, strData.c_str());
    if (ret != 0)
    {
        // 解析curl错误
        LogErr("curl error when send telegram msg, %s, url:%s", 
                curl_easy_strerror((CURLcode)ret), strUrl.c_str());
        return -ret;
    }
    LogDebug("telegram resp header: %s", strRespHeader.c_str());
    LogDebug("telegram resp data  : %s", strResp.c_str());

    http::CHttpRespone resp;
    resp.read((void*)strRespHeader.c_str(), strRespHeader.length());
    CMyString strContType = resp.getHeader("Content-Type");
    if (strContType == "application/json")
    {
        CJsonWrapper reader;
        if (!reader.Load(strResp))
        {
            LogErr("telegram return data [%s] not valid json string.", strResp.c_str());
            return -1005;  // 返回数据不正确
        }
        bool bOK = reader.getBool("ok");
        if (!bOK)
        {
            int32_t code = reader.getInt("error_code");
            CMyString errmsg = reader.getString("description");
            LogErr("send telegram msg failure, error:%d, msg:%s", 
                    code, errmsg.c_str());
            return -300000-abs(code);
        }
    }

    return 0;
}

int32_t CChatBot::telegram_ex(const CMyString &chatId, const CMyString &strText, const CMyString &atUsers, const CMyString &parseMode)
{
    // refer to:  https://core.telegram.org/bots/api#sendmessage
    if (m_telegram_token.empty())
    {
        LogErr("telegram access token is empty.");
        return -1001;
    }
    CMyString mode = parseMode;
    mode.trim();
    if (mode != "MarkdownV2" && mode != "HTML" && mode !="Markdown")
    {
        LogErr("unsupport parse_mode [%s]", parseMode.c_str());
        return -1003;
    }
    http::CHttpClient net;
    
    CMyString strUrl("https://api.telegram.org/bot%s/sendMessage", m_telegram_token.c_str());
    //CMyString strHeader("Content-Type: application/x-www-form-urlencode");
    http::VEC_STRING header_list;
    //header_list.push_back(strHeader);

    CMyString strData("chat_id=%s&text=%s", chatId.c_str(), strText.c_str());
    CStrSpliter css;
    css.Split(atUsers, ',');
    for(int i(0); i<css.size(); ++i)
    {
        strData.Append("@%s", css[i].c_str());
    }
    strData.Append("&parse_mode=%s", mode.c_str());

    CMyString strResp;
    CMyString strRespHeader;
    int32_t ret = net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list, strData.c_str());
    if (ret != 0)
    {
        // 解析curl错误
        LogErr("curl error when send telegram msg, %s, url:%s", 
                curl_easy_strerror((CURLcode)ret), strUrl.c_str());
        return -ret;
    }
    LogDebug("telegram resp header: %s", strRespHeader.c_str());
    LogDebug("telegram resp data  : %s", strResp.c_str());

    http::CHttpRespone resp;
    resp.read((void*)strRespHeader.c_str(), strRespHeader.length());
    CMyString strContType = resp.getHeader("Content-Type");
    if (strContType == "application/json")
    {
        CJsonWrapper reader;
        if (!reader.Load(strResp))
        {
            LogErr("telegram return data [%s] not valid json string.", strResp.c_str());
            return -1005;  // 返回数据不正确
        }
        bool bOK = reader.getBool("ok");
        if (!bOK)
        {
            int32_t code = reader.getInt("error_code");
            CMyString errmsg = reader.getString("description");
            LogErr("send telegram msg failure, error:%d, msg:%s", 
                    code, errmsg.c_str());
            return -300000-abs(code);
        }
    }

    return 0;
}
int32_t dingtalk_sendmsg(const CMyString &strText, const CMyString &strToken, 
        const CMyString &strKey, const CMyString &strAtMobiles)
{
    CChatBot bot;
    bot.setDingtalkToken(strToken, strKey);
    return bot.dingtalk(strText, strAtMobiles);
}

int32_t telegram_sendmsg(const CMyString &chatId, const CMyString &strText, const CMyString &strToken, const CMyString &atUsers)
{
    CChatBot bot;
    bot.setTelegramToken(strToken);
    return bot.telegram(chatId, strText, atUsers);
}

