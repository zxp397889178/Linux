/*
 * =====================================================================================
 *
 *       Filename:  http_post.cpp
 *
 *    Description:  test for ssl post
 *
 *        Version:  1.0
 *        Created:  2020年02月24日 15时51分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "MyNet.h"
#include "Security.h"

int main(int argc, char **argv)
{
    CMyString strUrl = "https://api.adminserv88.com/oauth/token";
    CMyString strHeader= "Authorization:Basic QVZJQUNOWV9hdXRoOnhhaXB4U29NJFF0Y1htM1Jua2Z5JGNjZw==;X-DAS-TX-ID:1582194219326;X-DAS-TZ:UTC+8;X-DAS-CURRENCY:CNY;X-DAS-LANG:zh-CN;Content-Type:application/x-www-form-urlencoded";
    CMyString strData = "grant_type=password&username=HXQPCNYA_api&password=AAbb1122++";
    
    //CMyString strDataEx = CHttpWrapper::urlEncode(strData);
    //CMyString strDataEx = CHttpWrapper::UrlEncode(strData);
    CMyString strDataEx = "grant_type=password&username=HXQPCNYA_api&password=AAbb1122%2B%2B";
    //CMyString strDataEx = strData;
    printf("data [%s]\n", strData.c_str());
    printf("dataex [%s]\n", strDataEx.c_str());

    http::CHttpRespone resp;
    httpex::CHttpClient net;
    //int32_t ret = net.Post(strUrl, strHeader, (byte_t*)strDataEx.c_str(), strDataEx.size(), resp);
    int32_t ret = net.Post(strUrl, strHeader, (byte_t*)strData.c_str(), strData.size(), resp);
    printf("result:%s%d%s\n", _color_red, ret, _color_end);
    printf("headers:\n%s%s%s\n", _color_blue, resp.getFullHeader().c_str(), _color_end);
    
    CMyString strResp;
    CByteStream &buff = resp.GetPost();
    buff.Read(strResp);
    printf("respone:\n%s%s%s\n", _color_green, strResp.c_str(), _color_end);

    printf("\ntest finished.\n");
    return 0;
}
