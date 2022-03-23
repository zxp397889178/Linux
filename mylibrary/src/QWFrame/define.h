/*
 * =====================================================================================
 *
 *       Filename:  define.h
 *
 *    Description:  QWFrame 增加的定义
 *
 *        Version:  1.0
 *        Created:  06/16/2020 10:42:32 PM
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

/*
 *====================================================================================
 * server 角色预定义
 *====================================================================================
 */
enum Actor
{
    actorDummy = 0,

    actorUpdsvr = 1,        // 自动更新服务器
    actorWebproxy = 2,      // webproxy, 所有WEB请求
    actorProxy = 3,         // proxy, 所有TCP请求
    actorGameSvr = 4,       // gamesvr, 处理游戏逻辑
    actorAuthSvr = 5,       // authsvr, 认证及用户数据服务器
    actorRechargesvr = 6,   // recharge,充值服务器
    actorBgsvr = 7,         // bgsvr, 后台服务、数据流水服务、全局服务器
    actorStatServer = 8,    // statsvr, 预计与bgsvr合并
    actorRobotSvr = 9,      // robotSvr 机器人服务器
    actorBehaveSvr = 10,        // behavesvr 客户端的行为服务器
    actorPartner = 11,      // 合作方认证服务器
    actorTableSvr = 12,     // 桌子服务器
    actorCompetSvr = 13,        // 比赛服务器
    actorMsgSvr = 14,       // 消息服务器
    actorAlarmsvr = 15,     // 监控报警服务器
    actorWebStore = 16,     // Web商城服务器
    actorAccountCenter = 17,    // 账号中心
    actorMallSvr = 18,      // 商城
    actorFishSvr = 19,      // 打鱼服务器
    actorHallSvr = 20,      // 大厅
    actorCenterSvr = 21,        // 中心节点
    actorActivitysvr = 22,  //activitysvr
    actorBusinesssvr = 23,      // 业务服务器
    actorLogsvr = 24,           // 日志服务器
    actorHallSvrSync = 25,      // hallsvr 同步访问
    actorGameClient = 100,
};

/*
 *====================================================================================
 * server 的clientId 预定义
 *====================================================================================
 */
#define CLIENTID_RECHARGESVR        0xFFFE0001
#define CLIENTID_GAMESVR            0xFFFE0002
#define CLIENTID_PROXYSVR           0xFFFE0003
#define CLIENTID_WEBPROXY           0xFFFE0005
#define CLIENTID_WEBSTAT            0xFFFE0006
#define CLIENTID_STATSVR            0xFFFE0007
#define CLIENTID_GLOBALSVR          0xFFFE0008
#define CLIENTID_AUTHSVR            0xFFFE0009
#define CLIENTID_ACCOUNTCENTER      0xFFFE0009

// 消息同步头
const uint8_t g_cSyncCode[] = {219, 109, 'S', 'y', 'n', 'C', 85, 170};

/*
*====================================================================================
* protobuf的协议头
*====================================================================================
*/
struct BASE_MSG_INFO
{
    unsigned short  usLen;          // 消息长度
    unsigned short  usMsgId;        // 消息ID

    void Cast(void)
    {
        usLen = CByteOrder::Cast(usLen);
        usMsgId = CByteOrder::Cast(usMsgId);
    }
};

#define MSG_HEAD_LEN sizeof(BASE_MSG_INFO)


