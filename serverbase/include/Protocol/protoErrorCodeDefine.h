/*
* =====================================================================================
*
*       Filename:  protoErrorCodeDefine.h
*
*    Description:  错误码定义
*
*        Version:  1.0
*        Created:  2017年03月17日 19时45分00秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/


#ifndef _QW_PROTOCOL_ERRORCODE_DEFINE_H_
#define _QW_PROTOCOL_ERRORCODE_DEFINE_H_


#define		SUCCEED						0			// 成功
#define		PLAYER_IN_GAMESVR			1			// 玩家在游服
/*
*====================================================================================
* 兑换商城错误码 998900~999000 对应tDictLanguage表 id
*====================================================================================
*/

#define		SHOP_DATA_NOTFOUND			998900		// 商城数据未找到
#define		ITEM_NOTFOUND				998901		// 道具未找到
#define		ITEM_EXPIRE					998902		// 道具已过期
#define		TICKET_NOT_ENOUGH			998903		// 奖券不足
#define		PLAYER_NOT_ONLINE			998904		// 玩家不在线
#define		BUY_ITEM_NUM_ERROR			998905		// 购买道具数量错误
#define		BUY_ITEM_LIMIT				998906		// 道具已被限制购买
#define		VERIFYCODE_ERROR			998907		// 验证码错误
#define		SHOP_COIN_NOT_ENOUGH		998908		// 金币不足
#define		SHOP_MONEY_NOT_ENOUGH		998909		// 钻石不足
#define		ORDER_NOT_FOUND				998910		// 订单没找到
#define		STOCK_NOT_ENOUGH			998911		// 库存不足
#define		QUOTA_NOT_ENOUGH			998912		// 额度不足
#define		PHONE_NUM_LIMIT 			998913		// 无法同时充值同一个手机
#define		ORDER_STATUS_ERROR			180124001	// 订单状态错误
#define		SHOP_VERSION_ERROR			180124002	// 版本过低
#define     RECHARGE_COUNT_LIMIT        180917001   // 充值额度限制
#define     EXCHANGE_REDEMPTION_CODE    181015004   // 即将兑换折扣券
#define     EXCHANGE_REDEMPTION_CODE_OK 181015005   // 兑换折扣券成功
/*
*====================================================================================
* hallsvr错误码 对应tDictLanguage表 id
*====================================================================================
*/
#define		TRUMPET_NOT_ENOUTH			126000		// 发送飞鸽广播喇叭不足	
#define		BROADCAST_IN_CD				126001		// 发送飞鸽广播冷却中
#define		PRESENT_TIMES_LIMIT			126002		// 赠送次数不足不能赠送
#define		PRESENT_DAILY_LIMIT			126003		// 每日赠送金额不足
#define		PRESENT_EVERYTIME_LIMIT		126004		// 每次赠送金额不足
#define		PRESENT_USER_COIN_NOT_ENOUGH 126005		// 用户金币不足（赠送金币）
#define		PRESENT_PASSWD_ERROR		126006		// 赠送金币密码错误
#define		USER_COIN_NOT_ENOUGH		126007		// 用户金币不足
#define		USER_MONEY_NOT_ENOUGH		126008		// 用户钻石不足
#define		USER_TICKET_NOT_ENOUGH		126009		// 用户奖券不足
#define		USER_ITEM_NOT_ENOUGH		126010		// 用户道具不足
#define		USER_ITEM_NOT_FOUND			126011		// 道具未找到
#define		USE_ITEM_NUM_ERROR			126012		// 使用道具数量错误
#define		PRESENT_LOWER_LIMIT			126013		// 每次赠送未超过下限
#define		USER_BANKCOIN_NOT_ENOUGH	126014		// 用户银行金币不足
#define		MIN_PRESENT_NUM_LIMIT		126015		// 赠送最低数量限制
#define		TARGET_USER_NOT_ONLINE		126016		// 目标用户不在线

#define		USER_POINT_NOT_ENOUGH					10000045	// 剩余可用积分不足
#define		PRESENT_DAILY_TIMES_NOT_ENOUTH			10000030	// 每日赠送次数用尽
#define		PRESENT_TOTAL_TIMES_NOT_ENOUTH			10000031	// 总赠送次数用尽
#define		PRESENT_POINT_SINGLE_LIMIT				10000042	// 单次赠送使用积分不可超过{0}
#define		EXCHANGE_DAILY_TIMES_NOT_ENOUTH			10000039	// 每日兑换次数用尽
#define		EXCHANGE_TOTAL_TIMES_NOT_ENOUTH			10000040	// 总兑换次数用尽
#define		EXCHANGE_POINT_SINGLE_LIMIT				10000046	// 单次兑换使用积分不可超过{0}
#define		PRESENT_PRICE_LIMIT_DAILY				180321002	// 当日可赠送价值不足

#define		PRESENT_OPERATION_FAILED			130329		// 操作失败，请重试

#define		PRESENT_TARGET_VERSION_TOO_LOW			180316004	// 对方版本过低（赠送炮座）


#define		LOTTERY_DRAW_TIMES_NOT_ENOUTH			10000100			// 抽奖次数用尽
#define		TODAY_BOUGHT_FOR_DAILY_RECHARGE			10000101			// 今日已购买（七日充值）
#define		TODAY_HAS_BEEN_SIGNIN					10000102			// 今日已签到
#define		ACTIVITY_END							10000103			// 活动已结束
#define		COST_FAILD  							180312005			// 扣款失败
#define		REWARD_HAD_GOT							2018030109			// 奖励已经领取
#define     ACTI_NOT_RUNNING                        190822005           // 活动未开始
#define     ACTI_FINISHED                           190822006           // 活动未开始

#define		ACCOUNT_LOCKED				29999		// 账号被封（禁止进入房间）


/*
*====================================================================================
* centersvr错误码 对应tDictLanguage表 id
*====================================================================================
*/
#define     ERROR_TC_RECHARGE               180308001       //累积充值不足
#define     ERROR_TC_TIMES_NOT_ENOUGH       180308002       //抽奖次数不足
#define     ERROR_TC_VIP_LEVEL              171127003       //VIP等级不足
#define     ERROR_TC_EXP_LEVEL              171127002       //经验等级不足

#define     ERROR_TC_COIN_NOT_ENOUGH        150114          //金币不足
#define		ERROR_TC_MONEY_NOT_ENOUGH		171127011       //用户钻石不足
#define		ERROR_TC_TICKET_NOT_ENOUGH		171127014		//用户奖券不足
#define		ERROR_TC_ITEM_NOT_ENOUGH		171127013       //用户道具不足

#define     ERROR_TC_LOTTERY_FAILED         110             //抽奖失败

#define 	ERROR_SYS_ERROR					171130002		//系统错误

#endif	// _QW_PROTOCOL_ERRORCODE_DEFINE_H_

