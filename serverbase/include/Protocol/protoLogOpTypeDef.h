/*
* =====================================================================================
*
*       Filename:  protoLogOpTypeDef.h
*
*    Description:  日志操作业务类型定义，以LOGTYPE_开头
*
*        Version:  1.0
*        Created:  2018年03月12日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  meilinfeng
*   Organization:  quwan
*
* =====================================================================================
*/


#ifndef _QW_PROTOCOL_LOG_OPTYPE_DEF_H_
#define _QW_PROTOCOL_LOG_OPTYPE_DEF_H_

/*logcurrency op_type定义*/
#define LOGTYPE_TICKETEXCHANGECOIN		1				// 奖券兑换金币
#define LOGTYPE_TICKETEXCHANGEITEM		2				// 奖券兑换道具
#define LOGTYPE_RECHARGE				3				// 充值
#define LOGTYPE_SEVENGIFT				4				// 七日礼包
#define LOGTYPE_ONLINEGIFT				5				// 在线礼包
#define LOGTYPE_MAIL					6				// 邮件
#define LOGTYPE_MONEYEXCHANGECOIN		7				// 钻石换金币
#define LOGTYPE_MOIDFYNICK				8				// 修改昵称
#define LOGTYPE_SHARPTIMEGIFT			9				// 整点礼包
#define LOGTYPE_TALENT					10				// 达人挑战
#define LOGTYPE_COMPET					11				// 竞技赛
#define LOFTYPE_VIPGIFT					12				// VIP礼包
#define LOFTYPE_SIGNIN					13				// 每日签到
#define LOFTYPE_GIFTKEY					14				// 礼包兑换码
#define LOFTYPE_OPERATEACTIVITY			15				// 运营活动
#define LOFTYPE_ALMS					16				// 救济金
#define LOFTYPE_BINDMSISDNREWARD		17				// 绑定手机
#define LOFTYPE_CORNUCOPIA				18				// 聚宝盆
#define LOFTYPE_OFFERAREWARD			19				// 悬赏任务
#define LOFTYPE_LEVELUP					20				// 升级
#define LOFTYPE_CREATECLAN				21				// 创建家族
#define LOFTYPE_EXCHANGEFISHCOIN		22				// 兑换鱼宝
#define LOFTYPE_GETOUTPERFUND			23				// 提取个人基金
#define LOFTYPE_GETOUTCLANFUND			24				// 提取家族基金
#define LOFTYPE_DEPOSITCLANCOIN			25				// 存入家族金币
#define LOFTYPE_PRESENTCOINCOST			26				// 赠送金币消耗
#define LOFTYPE_PRESENTCOINGAIN			27				// 赠送金币获得
#define LOFTYPE_BROADCAST				28				// 飞鸽广播
#define LOFTYPE_BUYITEM					29				// 购买道具
#define LOFTYPE_USEITEM					30				// 使用道具
#define LOFTYPE_PRESENTCRYSTALCOST		31				// 赠送水晶消耗
#define LOFTYPE_TREASURE				32				// 宝箱
#define LOFTYPE_MONTHCARD				50				// 月卡
#define LOFTYPE_HAPPYTREASURE			60				// 欢乐夺宝金币
#define LOFTYPE_HAPPYTREASUREITEM		61				// 欢乐夺宝道具
#define LOFTYPE_LIVENESSREWARD			62              // 活跃度任务
#define LOFTYPE_H5LOGINGIFT				63				// H5登录礼包
#define LOFTYPE_EXCHANGEORDERREFUND		64				// 取消兑换订单后返还
#define LOFTYPE_ACTIVITYSIGNIN			66				// 签到活动
#define LOFTYPE_TREASURECHESTREWARD		70				// 宝箱抽奖中奖奖励
#define LOGTYPE_THIRDEXCHANGECOIN		71				// 第三方兑入金币
#define LOGTYPE_THIRDDEPOSITREFUND		72				// 第三方兑出金币返还
#define LOGTYPE_GETGUNMOUNTPROPERTY		73				// 随机炮座附加属性
#define LOGTYPE_USER_SAFEBOX_DRAW       78              // 保险箱提取
#define LOGTYPE_USER_SAFEBOX_SAVE       79              // 保险箱存入
#define LOGTYPE_TORPEDO_PLATFORM        80              // 平台赠送鱼雷
#define LOGTYPE_RUNNINGFISH3D_EXCHANGE  81              // 3d赛鱼兑换
#define LOGTYPE_RUNNINGFISH3D_BET       82              // 3d赛鱼赌注
#define LOGTYPE_RUNNINGFISH3D_WIN       83              // 3d赛鱼胜利
#define LOGTYPE_RUNNINGFISH3D_DEPOSIT   84              // 3d赛鱼兑换消耗

/*logcurrency2 op_type定义*/

#define LOGTYPE_TC_LOTTERY				100							// 宝箱抽奖日志
#define LOGTYPE_PVP_BATTLE				101							// pvp对战
#define LOGTYPE_MSGBOARD				102							// 留言板
#define LOGTYPE_BETFISH					103							// 竞猜鱼
#define LOGTYPE_REDPACKAGEFISH			104							// 红包鱼
#define LOGTYPE_GRANDPRIX				105							// 大奖赛
#define LOGTYPE_CONTI_TASK				106							// 连续任务
#define LOGTYPE_FORESTPARTY				107							// 森林舞会
#define LOGTYPE_WECHATSHARE				108							// 微信分享
#define LOGTYPE_EXCHANGE_ITEM			109							// 商城兑换道具
#define QUWAN_EXCHANGE					110							// 趣玩游戏大厅兑换
#define LOGTYPE_EXCHANGE_COIN			111							// 商城兑换金币 
#define LOGTYPE_3RD_COIN				112							// 第三方兑换金币
#define LOGTYPE_UPDAET_REDPOCKET		113							// 升级领红包
#define LOGTYPE_EXCHANGE_TICKET			114							// 钻石商城兑换(奖券或鱼雷)
#define LOGTYPE_3RD_EXCHANGE_ITEM		115							// 第三方兑换道具
#define LOGTYPE_TRUMPET_ITEM			116							// 喇叭功能
#define LOGTYPE_3RD_MAIL_COMPENSATION	117							// 第三方兑换丢失金币后邮件补偿
#define LOGTYPE_PLAY					118							// 房间打鱼
#define LOGTYPE_MONEYTREE				119							// 摇钱树
#define LOGTYPE_RECHARGE_FUND			120							// 充值基金
#define LOGTYPE_NEWBIE_SIGNIN			121							// 新手签到
#define LOGTYPE_JACKPOT					122							// 捕鱼奖池
#define LOGTYPE_3RD_EXCHANGEOUT			124							// 第三方兑出金币， 其他游戏金币->捕鱼平台

#define LOGTYPE_GOLDENEGG_FISH			125							// 金蛋鱼
#define LOGTYPE_CARD_FISH				126							// 卡牌鱼
#define LOGTYPE_GIFT_FISH				127							// 礼物鱼
#define LOGTYPE_SLOTMACHINE_FISH		128							// 老虎机鱼
/*op_type定义活动日志类型*/

#define LOGTYPE_ACTIVITY_NEWBEELOGIN        1000			        // 活动_新手登录任务
#define LOGTYPE_ACTIVITY_SEVENDAYSRECHARGE  1001				    // 活动_7日充值
#define LOGTYPE_ACTIVITY_SINGLERECHARGE     1002				    // 活动_单笔充值活动
#define LOGTYPE_ACTIVITY_MOONCAKE	        1003			        // 活动_中秋博饼
#define LOGTYPE_ACTIVITY_TURNPLATE	        1004			        // 活动_转盘活动
#define LOGTYPE_ACTIVITY_CHECKINGAME	    1005			        // 活动_宝藏抢夺赛
#define LOGTYPE_ACTIVITY_JACKPOTWAR	        1006			        // 活动_奖池战争活动
#define LOGTYPE_ACTIVITY_SWEEPINGMINES	    1007			        // 活动_红包扫雷活动
#define LOGTYPE_ACTIVITY_POINTSDRAW	        1008			        // 活动_奖池积分抽奖活动
#define LOGTYPE_ACTIVITY_DAILYTASK2	        1009			        // 活动_每日任务2.0
#define LOGTYPE_ACTIVITY_ANNIVERSARY	    1010			        // 活动_周年活动
#define LOGTYPE_ACTIVITY_PIRATESHIP	        1011			        // 活动_海盗船
#define LOGTYPE_ACTIVITY_HOLIDAYBONUS       1012			        // 活动_节日红包
#define LOGTYPE_ACTIVITY_DAILYRECHARGE		1013			        // 活动_每日充值
#define LOGTYPE_ACTIVITY_AUCTION			1014			        // 活动_一元夺宝
#define LOGTYPE_ACTIVITY_TREASUREPOT		1015			        // 活动_聚宝盆
#endif

