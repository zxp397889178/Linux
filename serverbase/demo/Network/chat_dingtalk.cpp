/*
 * =====================================================================================
 *
 *       Filename:  chat_dingtalk.cpp
 *
 *    Description:  钉钉聊天机器人测试
 *
 *        Version:  1.0
 *        Created:  2020年08月15日 16时38分42秒
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

int32_t main(int32_t argc, char **argv)
{
    CChatBot chat;
    chat.setDingtalkToken("676a31f14661850798fac47ec65acec5f4c967a12c9ccd5488ec644a3f47d677", "SEC2406a27826f51cbc83fba82d7cc6b28d520ca206bb0ca5f5d8b5ec3cccbe75e6");
    int32_t ret(0);
    ret = chat.dingtalk("大家好，我是测试机器人,我现在可以@人了:D", "18350008739,+86-18606068611");
    printf("test dingtalk text result:%d\n", ret);

    ret = chat.dingtalk_link("我是云机器人", "每个机器人每分钟最多发送20条。消息发送太频繁会严重影响群成员的使用体验，大量发消息的场景 (譬如系统监控报警) 可以将这些信息进行整合，通过markdown消息以摘要的形式发送到群里。", "https://ss0.bdstatic.com/70cFuHSh_Q1YnxGkpoWK1HF6hhy/it/u=85932202,1947553998&fm=26&gp=0.jpg", "https://ding-doc.dingtalk.com/doc#/serverapi2/qf2nxq/e9d991e2");
    printf("test dingtalk link result:%d\n", ret);

    CWeather sky;
    Json::Value resp;
    ret = sky.getByIp("220.160.62.131", resp);
    if (ret == 0)
    {
        CMyString strTitle("%s (id=%s)天气", resp["city"].asString().c_str(), resp["cityid"].asString().c_str());
        CMyString strWeather("天气:%s, 温度:%s℃ (最高%s℃, 最低%s℃ ), 湿度:%s, %s %s,%s, 空气质量:%s(%s), PM2.5:%s>%s",
                resp["wea"].asString().c_str(), resp["tem"].asString().c_str(),
                    resp["tem1"].asString().c_str(), resp["tem2"].asString().c_str(),
                    resp["humidity"].asString().c_str(),
                resp["win"].asString().c_str(), resp["win_speed"].asString().c_str(),resp["win_meter"].asString().c_str(),
                resp["air"].asString().c_str(), resp["air_level"].asString().c_str(), resp["air_pm25"].asString().c_str(),
                resp["air_tips"].asString().c_str());
        // printf("%s\n%s\n", strTitle.c_str(), strWeather.c_str());

        CMyString strScreen = "http://pic1.win4000.com/wallpaper/2018-08-02/5b626b38ba57b.jpg";

        //CMyString strScreen = "https://img.alicdn.com/tfs/TB1NwmBEL9TBuNjy1zbXXXpepXa-2400-1218.png";
        CMyString strUrl="http://www.quwangame.com";
        CMyString strNote("#### %s天气 \n> %s\n> ![screenshot](%s)\n> #### %s %s发布[天气](%s)\n", 
                resp["city"].asString().c_str(),
                strWeather.c_str(), 
                strScreen.c_str(),
                resp["date"].asString().c_str(), resp["update_time"].asString().c_str(),
                strUrl.c_str());
        ret = chat.dingtalk_markdown(strTitle, strNote, "13950482595");
        printf("test dingtalk markdown result:%d\n", ret);
    }


    return 0;
}
