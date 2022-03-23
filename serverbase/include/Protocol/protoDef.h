/*
 * =====================================================================================
 *
 *       Filename:  protoDef.h
 *
 *    Description:  protocol define
 *
 *        Version:  1.0
 *        Created:  2015年09月07日 19时45分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_PROTOCOL_DEFINE_H_
#define _QW_CRAZYFISH_PROTOCOL_DEFINE_H_


/*
 *====================================================================================
 * 系统内部通讯消息
 *====================================================================================
 */
#define  ECHO                               1       //test echo
#define  PROTO_SYS_TESTSPEED                2       //测速协议
#define  PROTO_SYS_HEARTBEAT                3       //心跳协议
#define  PROTO_SYS_HEARTBEAT_EX             5       //心跳协议
#define  PROTO_SYS_ENCRYPTCODE              9       //更换加密密钥

#define  CLIENTCLOSEREQUEST                 1002    //客户端与proxy断开后，proxy向gamesvr通知client断开
#define  CLIENT_CLOSE_RESPONSE				1003	// bgsvr收到protoClientShutdown后应答给gamesvr

#define  PROXYNOTIFYHEARTREQUEST            1010    //proxynotify与proxy心跳请求
#define  GAMESVRSTOPREQUEST                 1011    //游戏服务器停止请求
#define  CONNECT_REGISTER_REQUEST           1012    //连接服务器注册请求
#define  USER_ENTER_HALL_REQUEST            1013    //proxy收到gamesvr认证确认后，通知gamesvr说客户端进入大厅（统计用）

#define  RECHARGE_NOTIFY_REQUEST            1014    //充值通知
#define  RECHARGE_NOTIFYEX_REQUEST			1015	//充值通知(RMB直接兑换钻石)
#define  RECHARGE_MALL_NOTIFY_REQ			1016	//充值通知 mallsvr通知
#define  RECHARGE_MALL_NOTIFY_RESP			1017	//充值通知响应 
#define  PROTO_SVR_ITEMCHARGE_REQ			1018	//商城购买道具扣款请求
#define  PROTO_SVR_ITEMCHARGE_RESP			1019	//商城购买道具扣款应答
#define  PROTO_SVR_RECHARGE_NUM_NOTIFY		1020	//玩家充值金额通知商城 gamesvr->mallsvr
#define  RECHARGE_NOTIFY								 1021    //充值通知(rechargesvr->hallsvr)
#define  EXCHANGE_RECHARGE_NOTIFY           1022    //兑换订单充值通知(hallsvr->gamesvr)
#define  RECHARGE_SYNC			            1023    //订单充值通知(hallsvr->centersvr)

#define  RECHARGE_NOTIFY_REQ				1024	//订单充值通知 	(hallsvr->gamesvr)
#define  RECHARGE_NOTIFY_RESP				1025	//订单充值通知结果应答 (gamesvr->hallsvr)

#define  PROTO_SYS_AUTHORIZE				5001	//内部服务器身份验证
#define  PROTO_SYS_AUTHORIZE_RESP			5002	//内部服务器身份证验确认
#define  PROTO_SYS_USERDATA_SEND			5003	// authsvr -> gamesvr 发送用户数据
#define  PROTO_SYS_USERDATA_WRITE			5004	//用户数据回写

#define  PROTO_SVR_DBLOGGER					5006	// 记录数据库日志流水
#define  PROTO_SVR_DAILYRESET_REQ			5007	// 请求游戏服务器做每日重置
#define  PROTO_SVR_DAILYRESET_RESP			5008	// GameSvr重置完毕后的应答

#define  PROTO_SVR_HITTICKET_REQ			5009	// 请求判断是否命中奖券鱼
#define  PROTO_SVR_HITTICKET_RESP			5010	// 奖券鱼命中结果应答

#define  PROTO_SVR_ENTERCOMPET_REG 			5011	// 用户的进入比赛房登记
#define  PROTO_SVR_COMPETDATA_REG			5012	// gamesvr => bgsvr 用户竞技赛结果登记
#define  PROTO_SVR_COMPETDATA_RESP			5029	// bgsvr => gamesvr 返回用户比赛的分数，以及排名数据
#define  PROTO_SVR_QUERY_COMPETDATA_REQ		5013	// 请求获取用户比赛数据
#define  PROTO_SVR_QUERY_COMPETDATA_RESP	5015	// 用户比赛数据结果返回

#define  PROTO_SVR_RANKRENEW_NOTIFY			5016	// bgsvr => gamesvr	 金币、奖券更新通知
#define	 PROTO_SVR_COMPETRENEW_NOTIFY 		5017	// bgsvr => gamesvr	 房间排行更新通知
#define  PROTO_SVR_RANKDATA_REQUEST			5018	// gamesvr => bgsvr	 请求获得金币、奖券排行
#define  PROTO_SVR_RANKDATA_RESPONE			5019	// bgsvf => gamesvr	 金币、奖券排行数据
#define  PROTO_SVR_COMPETRANK_REQUEST		5020	// gamesvr => bgsvr  请求获得房间排行
#define  PROTO_SVR_COMPETRANK_RESPONE		5021	// bgsvr => gamesvr  房间排行数据

#define	 PROTO_SVR_COMPETENTER_RESET		5022	// gamesvr => authsvr 清除指定房的进入次数

//#define  PROTO_SVR_FLUSH_ALLUSER_REQ		5023	// bgsvr => gamesvr	  请求游戏服务器回写所有用户数据
//#define  PROTO_SVR_FLUSH_ALLUSER_RESP		5025	// gamesvr => bgsvr   用户数据回写处理完毕
//#define  PROTO_SVR_COMPET_RANK_NOTIFY		5026	// bgsvr => gamesvr	  通知游戏服务器去更新奖励数据(只处理在线的用户)
//#define  PROTO_SVR_COMPET_SYNC_REQ			5027	// gamesvr => authsvr 请求从数据库同步指定比赛房的数据
#define	 PROTO_SVR_COMPET_REWARD_NOTIFY		5028	// bgsvr => gamesvr	  用户奖励数据广播，游服只处理自己的在线用户(数据需要压缩)

#define	 PROTO_SVR_SEND_SMS_REQUEST			5029	// gamesvr => msgsvr  发送手机短信验证码请求
#define	 PROTO_SVR_SEND_EMAIL_REQUEST		5030	// gamesvr => msgsvr  发送邮件验证码请求
//5029
#define  PROTO_BG_USERDATA_REQUEST			5031	// 请求BG写用户数据
#define  PROTO_BG_USERDATA_RESPONE			5032	// BG写用户数据应答

#define  PROTO_SVR_SEND_SMS_RESPONE         5033	// 发送手机短信验证码响应
#define  PROTO_SVR_SEND_EMAIL_RESPONE		5034	// 发送邮件验证码响应

#define  PROTO_PHONE_NUMBER_VERIFY_REQUEST	5035	// 校验手机号码请求
#define  PROTO_PHONE_NUMBER_VERIFY_RESPONE	5036	// 校验手机号码响应

#define PROTO_SEND_ALARM_SMS_REQUEST		5037	// 发送告警短信请求
#define PROTO_SEND_ALARM_SMS_RESPONE		5038	// 发送告警短信响应

#define PROTO_SEND_VOICE_REQUEST				5039	//发送电话语音


#define  PROTO_BG_COMPET_SCHEDULE 			5040	// 竞技场开始或结束（广播）
#define  PROTO_SVR_COMPET_GAIN				5041 	// 竞技场用户得分上传
#define  PROTO_BG_COMPET_NOTIFY				5042	// 竞技场通知（广播）
#define  PROTO_SVR_COMPETRANK_REQ			5043	// 竞技场排行数据请求
#define  PROTO_BG_COMPETRANK_RESP			5045	// 竞技场排行数据应答
#define  PROTO_SVR_COMPETREWARD_REQ			5046	// 竞技场奖励请求
#define  PROTO_BG_COMPETREWARD_RESP 		5047	// 兑技场奖励处理应答（广播）
#define  PROTO_SVR_COMPETUSERSCORE_REQ      5048	// 竞技场用户得分请求
#define  PROTO_BG_COMPETUSERSCORE_RESP      5049	// 竞技场用户得分应答
#define  PROTO_BG_USER_NEWMAIL_NOTIFY		5050	// 用户新邮件通知 bgsvr->gamesvr(broadcast)
#define  PROTO_DAILY_RESET_NOTIFY			5051	// 日常重置完成通知 bgsvr->gamesvr(broadcast)
#define  PROTO_SVR_SYNCCOMPETUSERNICK		5052	// gamesvr -> bgsvr 修改比赛排名昵称
#define	 PROTO_SVR_COMPETROOMINFO_NOTIFY	5053	// competsvr -> gamesvr 竞技赛房间信息通知
#define	 PROTO_QUERY_USERARENAINFO_REQ		5054	// gamesvr -> competsvr 查询用户竞技赛信息请求
#define	 PROTO_QUERY_USERARENAINFO_RESP		5055	// competsvr -> gamesvr 查询用户竞技赛信息应答
#define  PROTO_SVR_ACCEPTSHARE_REQ			5056	// gamesvr -> competsvr 用户接受分享请求
#define  PROTO_SVR_SHAREINFODAILY_REQ		5057	// gamesvr -> competsvr 用户今日分享信息请求
#define  PROTO_SVR_SHAREINFODAILY_RESP		5058	// competsvr -> gamesvr 用户今日分享信息应答
#define	 PROTO_SVR_USER_GROUP_REQ			5059	// gamesvr -> competsvr 用户分组请求
#define	 PROTO_SVR_USER_GROUP_RESP			5060	// competsvr -> gamesvr 用户分组应答

#define  PROTO_SVR_TALENT_LOOK_REQ			5070	//达人挑战查看请求
#define  PROTO_BG_TALENT_LOOK_RESP			5071	//达人挑战查看应答
#define  PROTO_SVR_TALENT_SIGNUP_REQ		5072	//达人挑战报名请求
#define  PROTO_BG_TALENT_SIGNUP_RESP		5073	//达人挑战报名应答
#define  PROTO_SVR_TALENT_RANK_REQ			5074	//达人挑战排行请求
#define  PROTO_BG_TALENT_RANK_RESP			5075	//达人挑战排行应答
#define  PROTO_SVR_TALENT_GAIN				5076	//达人挑战得分上传
#define  PROTO_BG_TALENT_STATUS				5077	//达人挑战状态应答

#define  PROTO_MACHINE_ALARM_REQUEST			5080	//监控信息请求
#define  PROTO_MACHINE_ALARM_RESPONE			5081	//监控信息响应
#define  PROTO_THREAD_ALARM_REQUEST			5082	//线程监控请求
#define	 PROTO_THREAD_ALARM_RESPONE			5083	//线程监控响应
#define  PROTO_CONNECT_ALARM_REQUEST		5084	//连接监控请求
#define	 PROTO_CONNECT_ALARM_RESPONE		5085	//连接监控响应
#define  PROTO_DB_ALARM_REQUEST				5086	//数据库监控请求
#define	 PROTO_DB_ALARM_RESPONE				5087	//数据库监控响应
#define  PROTO_NOTIFY_ALARM_REQUEST			5088	//监控通知请求
#define  PROTO_NOTIFY_ALARM_RESPONE			5089	//监控通知响应
#define  PROTO_NODE_ALARM_REQUEST			5090       //node监控请求

#define PROTO_SEND_VOICE_RESPONE				5091	//发送电话语音响应
#define PROTO_SEND_WEB_MSG_REQUEST				5092	//发送消息请求
#define PROTO_SEND_3RD_MSG_NOTIFY			5093	//发送第三方消息请求


#define  PROTO_CLIREQ_ENTERGAME			    5100	// 请求进入游戏
#define  PROTO_AUTH_UINAUTHOK				5103	// authsvr -> gamesvr 用户认证通过,请求装载数据
#define  PROTO_GAME_UINACCEPT				5105	// gamesvr -> authsvr 确认接收到用户数据装载请求
#define  PROTO_GAME_UINTICK					5107	// gamesvr -> authsvr 游服中玩家的心跳
#define  PROTO_GAME_SYNCUSERREQ				5109	// gamesvr -> authsvr 批量同步用户请求
#define  PROTO_GAME_CARRY_WEALTH			5111	// gamesvr -> authsvr 将用户的金币,奖券带出游戏
#define  PROTO_SVR_UIN_LOGOUT				5112	// authsvr -> gamesvr 请求将指定的用户下线
#define  PROTO_GAME_SHUTDOWN				5113	// authsvr 内部协议,游服已断开
#define  PROTO_GAME_SYNCNICK				5114	// gamesvr -> authsvr,hallsvr 修改昵称同步
#define  PROTO_GAME_QUERYWEALTH				5115	// gamesvr -> authsvr 查询丰游货币信息请求
#define  PROTO_AUTH_QUERYWEALTH				5116	// authsvr -> gamesvr 查询丰游货币信息应答
#define  PROTO_GAME_DRAWWEALTH				5117	// gamesvr -> authsvr 代出丰游金币、奖券请求
#define  PROTO_AUTH_DRAWWEALTH				5118	// authsvr -> gamesvr 代出丰游金币、奖券应答
#define  PROTO_GAME_DEPOSITWEALTH			5119	// gamesvr -> authsvr 代入丰游金币、奖券请求
#define  PROTO_AUTH_DEPOSITWEALTH			5120	// authsvr -> gamesvr 代入丰游金币、奖券应答

#define  PROTO_GAME_QUERYWEALTH_3RD			5130	// gamesvr -> authsvr 查询第三方货币信息请求
#define  PROTO_AUTH_QUERYWEALTH_3RD			5131	// authsvr -> gamesvr 查询第三方货币信息应答
#define  PROTO_GAME_DRAWWEALTH_3RD			5132	// gamesvr -> authsvr 请求代出第三方账户的财富
#define  PROTO_AUTH_DRAWWEALTH_3RD			5133	// authsvr -> gamesvr 第三方账户财富的代出确认
#define  PROTO_GAME_DEPOSITWEALTH_3RD		5134	// gamesvr -> authsvr 请求代入第三方账户的账富
#define  PROTO_AUTH_DEPOSITWEALTH_3RD		5135	// authsvr -> gamesvr 第三方账户财富代入确认
#define  PROTO_GAME_QUERYORDER_3RD			5136	// gamesvr -> authsvr 查询订单完成情况
#define  PROTO_AUTH_QUERYORDER_3RD			5137	// authsvr -> gamesvr 订单完成情况查询

#define  PROTO_GAME_WRITE_MAIL				5140	// gamesvr -> authsvr 请求写邮件


#define  PROTO_TABLEOPEN_REQUEST			5200	// tablesvr -> gamesvr 桌子开启请求
#define  PROTO_TABLEOPEN_RESPONE			5201	// gamesvr -> tablesvr 桌子应答
#define  PROTO_TABLELIST_REQUEST			5202	// gamesvr -> tablesvr 桌子列表请求
#define  PROTO_TABLELIST_RESPONE			5203	// tablesvr -> gamesvr 桌子列表应答
#define  PROTO_SEATDOWNRANDOM_REQUEST		5204	// gamesvr -> tablesvr 随机座位坐下请求
#define  PROTO_SEATDOWNSPECIFY_REQUEST		5205	// gamesvr -> tablesvr 指定座位坐下请求
#define  PROTO_SEATDOWN_RESPONE				5206	// tablesvr -> gamesvr 坐下应答
#define  PROTO_LEAVETABLE_NOTIFY			5207	// gamesvr -> tablesvr 离开桌子通知
#define  PROTO_SYNCSEATINFO_REQUEST			5209	// tablesvr -> gamesvr 同步座位信息请求
#define  PROTO_SYNCSEATINFO_RESPONE			5210	// gamesvr -> tablesvr 同步座位信息应答
#define  PROTO_SVR_CHANGESEAT_REQUEST		5211	// gamesvr -> tablesvr 改变座位请求
#define  PROTO_SVR_CHANGESEAT_RESPONE		5212	// tablesvr -> gamesvr 改变座位应答

#define  PROTO_MIGRATE_DATA_REQUEST			5220	// gamesvr1 -> proxy -> gamesvr2  数据迁移请求
#define  PROTO_MIGRATE_DATA_RESPONE			5221    // gamesvr2 -> proxy -> gamesvr1  数据迁移应答
#define  PROTO_AUTH_CHANGE_GAMESVR			5222	// gamesvr -> authsvr  通知authsvr更改游服

#define  PROTO_SVR_CHALLENGE_INFO_SYNC		5230	// competsvr -> gamesvr 新竞技赛场次信息同步
#define  PROTO_SVR_CHALLENGE_SIGNUP_REQ		5231	// gamesvr -> competsvr 新竞技赛报名请求
#define  PROTO_SVR_CHALLENGE_SIGNUP_RESP	5232	// competsvr -> gamesvr 新竞技赛报名应答
#define  PROTO_SVR_CHALLENGE_RANK_REQ		5233	// gamesvr -> competsvr 新竞技赛排行榜请求
#define  PROTO_SVR_CHALLENGE_RANK_RESP		5234	// competsvr -> gamesvr 新竞技赛排行榜应答
#define  PROTO_SVR_CHALLENGE_SCORE_REG		5235	// gamesvr -> competsvr 新竞技赛得分登记
#define  PROTO_SVR_CHALLENGE_DYNAMIC_RANK_REQ 5236	// gamesvr -> competsvr 新竞技赛排行榜请求
#define  PROTO_SVR_CHALLENGE_DYNAMIC_RANK_RESP 5237	// competsvr -> gamesvr 新竞技赛排行榜应答
#define  PROTO_SVR_CHALLENGE_MYSELF_REQ		5238	// gamesvr -> competsvr 新竞技赛我的比赛请求
#define  PROTO_SVR_CHALLENGE_MYSELF_RESP	5239	// competsvr -> gamesvr 新竞技赛我的比赛应答
#define  PROTO_SVR_CHALLENGE_RESET_NOTIFY	5240	// competsvr -> gamesvr 新竞技赛重置场次信息通知

#define  PROTO_SINA_RED_PACKET_NOTIFY			5241	// gamesvr -> authsvr 新浪红包通知
#define	 PROTO_SVR_GAIN_NOTIFY				5242	// gamesvr -> centersvr 获得和消耗通知
#define	 PROTO_SVR_ACTIVITY_DATA_NOTIFY		5243	// gamesvr -> centersvr 活动数据通知
#define	 PROTO_SVR_UPDATE_PICTURE_NOTIFY 5244 //customer -> hallsvr 通知用户更新头像

#define	 PROTO_SVR_RED_POINT_NOTIFY			5245 //  -> centersvr 通知红点

#define  PROTO_SVR_BRUSH_NUMBER_FLAG_NOTIFY 5246	// 刷号标识通知



#define  PROTO_SVR_DEDUCT_WEALTH_REQUEST	5249	// 扣财富请求（金币，奖券，钻石，喇叭，道具）
#define  PROTO_SVR_DEDUCT_WEALTH_RESPONSE	5250	// 扣财富应答（金币，奖券，钻石，喇叭，道具）

#define	 PROTO_SVR_SUBSCRIBE_REQUEST		5251	// 消息订阅请求
#define	 PROTO_SVR_SUBSCRIBE_RESPONSE		5252	// 消息订阅应答

#define	 PROTO_SVR_SYNC_VIP					5253	// 同步VIP

#define	 PROTO_SVR_RECHARGE_INFO_NOTIFY		5254	// 充值信息通知 hallsvr->gamesvr（记录日志）

#define	 PROTO_SVR_SYNCUSER_REQ				5255	// 同步用户请求

#define	 PROTO_SVR_USERLEAVEROOM_NOTIFY		5256	// 用户离开房间通知 gamesvr->hallsvr

#define  PROTO_SVR_DBLOGGER_EX				5257	// 传SQL语句写日志

#define  PROTO_SVR_QUERY_WEALTH_REQUEST		5258	// 查询财富请求（金币，奖券，钻石，喇叭，道具）
#define  PROTO_SVR_QUERY_WEALTH_RESPONSE	5259	// 查询财富应答（金币，奖券，钻石，喇叭，道具）

#define  PROTO_SVR_POINTKILL_NOTIFY			5260	// 点杀通知 authsvr -> gamesvr
#define  PROTO_SVR_POINTKILL_VALUE_NOTIFY	5261	// 点杀数据通知 gamesvr -> authsvr
#define  PROTO_SVR_FISH_BROADCAST_NOTIFY	5262	// 捕鱼广播通知 gamesvr -> authsvr
#define  PROTO_SVR_SYNC_COMPETINFO_REQ		5263	// 同步比赛信息请求 gamesvr->competsvr
#define  PROTO_SVR_DRAW_WEALTH_REQUEST		5264	// 通知游服开始兑换金币

#define  PROTO_SVR_ADD_WEALTH_REQUEST		5265	// 通知游服增加财富
#define  PROTO_SVR_ADD_WEALTH_RESPONSE		5266	// 游服增加财富响应

#define  PROTO_EXTERNAL_ADDCURSCORE			5267	// 外部添加下分请求

#define  CLOSE_CLIENT_ERROR_RESPONE         9999    //gamesvr通知proxy断开客户端连接

/*
 *====================================================================================
 * 客户端上传请求
 *====================================================================================
 */

#define  PROTO_3RD_BEGIN					9000
// 第三方认证协议
#define	 PROTO_3RD_MAHJONG_AUTHORIZE		9001	// 丰游麻将登陆认证协议
#define  PROTO_3RD_MEMEYULE_AUTHORIZE		9003	// 么么直接登陆认证协议
#define  PROTO_3RD_WECHAT_AUTHORIZE			9005	// 微信账号登陆认证协议

#define  PROTO_3RD_END						9100

#define  PROTO_CLIREQ_AUTHORIZE             10000   //登录请求
#define  ROOMLISTREQUEST                    10001   //RoomListRequest
#define  INTOGAMEREQUEST                    10002   //IntoGameRequest
#define  OUTGAMEREQUEST                     10003   //OutGameRequest
#define  HITFISHREQUEST                     10005   //HitFishRequest
#define  CHANGEBULLETREQUEST                10007   //ChangeBulletRequest
#define  CHANGEGUNREQUEST                   10008   //ChangeGunRequest
#define  SHOOTREQUEST                       10009   //ShootRequest
#define	 SHOOTSKILLREQUEST					10010	//发射激光炮/黑洞请求
#define	 DRAGSKILLREQUEST					10016	//激光炮/黑洞拖动请求	
#define  SEVENGIFTREQUEST                   10012   //SevenGiftRequest
#define  GETSEVENGIFTREQUEST				10013   //GetSevenGiftRequest
#define  ONLINETIMEREQUEST                  10014   //OnLineTimeRequest
#define  GETONLINEGIFTREQUEST               10015   //GetOnLineGiftRequest
#define  SENDMSGREQUEST                     10017   //SendMsgRequest
#define  UPDATEACCTINFOREQUEST              10020   //修改账号信息请求
#define  GETPLAYERPICREQUEST                10021   //获取头像名称请求
#define  SENDPUBLICMSGREQUEST               10023   //公聊发送消息请求
#define  FASTINTOROOMREQUEST                10024   //快速进入游戏请求
#define  GETFREECHATCOUNTREQUEST            10025   //获取免费聊天次数请求
#define  GETNICKIFUPDATEREQUEST             10027   //获取昵称是否修改请求
#define  DATARELOADREQUEST                  10028   //数据重新装载请求
#define  DESIGNROOMINFOREQUEST              10033   //指定房间信息请求
#define  BILLCOMPOUNDREQUEST                10034   //合成话费卡请求
#define  BILLPIECECOUNTREQUEST              10035   //获得话费卡碎片请求
#define  PVPROOMSTATUSREQUEST               10036   //获取玩家闯关状态请求
#define  PVPROOMGETREWARDREQUEST            10037   //玩家获取奖励请求
#define  PVPROOMOUTRESPONE                  10038   //PVP房玩家退出应答
#define  PVPROOMWAITSTOPREQUEST             10040   //PVP房玩家等待时终止请求
#define	 ACCOUNTALMSINFOREQUEST				10041	//玩家可领救济金信息请求
#define	 ACCOUNTALMSREQUEST					10042	//玩家领取救济金请求
#define  USER_MANUAL_FINDPASSWD				10043	// 用户找回密码
#define  USER_MANUAL_RESETPASSWD			10044	// 用户手工重置密码
#define  QUERY_SHARPTIME_GIFT_REQUEST		10046	// 查询整点礼包请求
#define  RECEIVE_SHARPTIME_GIFT_REQUEST		10047	// 领取整点礼包请求
#define  BUY_ITEM_REQUEST                   10050   //购买道具请求
#define  SHOP_LIST_REQUEST                  10051   //获取商城列表请求
#define  ADD_FRIEND_REQUEST                 10052   //增加好友请求
#define  DELETE_FRIEND_REQUEST              10053   //删除好友请求
#define  GIVE_COIN_REQUEST                  10054   //赠送金币请求
#define  CREATE_ROBOT_REQUEST               10055   //创建机器人请求
#define  DELETE_ROBOT_REQUEST               10056   //删除机器人请求
#define  QUERY_TASK_INFO_REQUEST            10057   //任务信息查询请求
#define  ACCEPT_TASK_REQUEST                10058   //接受任务请求
#define  COMPLETE_TASK_REQUEST              10059   //完成任务请求
#define  ACCESS_BANK_COIN_REQUEST           10061   //存取银行金币请求
#define  GIVE_BANK_COIN_REQUEST             10062   //赠送银行金币请求
#define  MODIFY_BANK_PASSWD_REQUEST         10063   //修改银行密码请求
#define  RECHARGE_REQUEST                   10064   //充值请求
#define  RECEIVE_VIP_REWARD_REQUEST         10065   //接收vip礼包请求
#define  QUERY_VIP_REWARD_REQUEST           10066   //查询vip礼包请求
#define  NEWBIE_COMPLETE_REQUEST            10068   //新手引导完成请求
#define  RECHARGE_CHECK_REQUEST             10069   //充值校验请求
#define  BUY_OBJECT_ITEM_REQUEST            10070   //购买实物道具请求
#define  VERIFY_CODE_REQUEST                10071   //获取验证码请求
#define  ALL_RANK_REQUEST                   10072   //所有玩家排名请求
#define  USER_RANK_REQUEST                  10073   //玩家排名请求
#define  USE_TEL_FARE_ITEM_REQUEST          10077   //使用话费道具请求
#define  USE_OBJECT_ITEM_REQUEST            10078   //使用实物道具请求
#define  ORDER_INFO_REQUEST                 10079   //订单信息请求
#define  OBJECT_SHOP_LIST_REQUEST           10080   //获取实物商城列表请求
#define  COIN_RANK_REQUEST                  10081   //金币排名请求
#define  TICKET_RANK_REQUEST                10082   //奖券排名请求
#define  USER_CURRENCY_REQUEST              10084   //用户货币信息请求
#define  QUERY_EVENT_TASK_INFO_REQUEST      10085   //查询比赛活动任务信息请求
#define  COMPLETE_EVENT_TASK_REQUEST        10086   //完成比赛活动任务请求
#define  OBJ_ORDER_BROADCAST_REQUEST		10088	//实物订单广播请求
#define  USER_SHARE_REQUEST 			 	10090	//用户分享请求
#define  USER_ACCEPT_SHARE_REQUEST  		10091	//用户接受分享请求
#define  COMPET_RANK_REQUEST				10092	//捕鱼达人排名请求
#define  COMPET_REWARD_REQUEST				10093	//捕鱼达人领取奖励请求
#define  READ_MAIL_REQUEST                  10095   //读取邮件请求
#define  GET_MAIL_AWARD_REQUEST				10096	//获取邮件奖励请求
#define  MAIL_LIST_REQUEST                  10097   //邮件列表请求
#define	 CLIENT_HEARTBEAT_REQUEST			10098	//客户端心跳请求
#define  COMPET_RANK_NEW_REQUEST			10099	//捕鱼达人排名请求（新）
#define  UPD_UPLOAD_REQUEST                 10100   //上传更新包请求
#define  UPD_UPDATELIST_REQUEST             10101   //请求更新列表
#define  GET_MAIL_AWARD_NOTIFY				10103	//获取奖励数据通知
#define  UPD_NOTICE_REQUEST					10105	//公告列表请求（更新服务器）
#define  UPD_SWITCH_REQUEST					10106	//开关列表请求（更新服务器）
#define  GAME_SWITCH_REQUEST				10107	//开关列表请求（游戏服务器）
#define  QUERY_DICT_UI_REQUEST				10108	//查询UI请求
#define  QUERY_DICT_RULE_REQUEST			10109	//查询规则请求
#define  MAIL_LIST_REQUEST_EX               10110   //邮件列表请求(支持翻页)
#define  SKILLHITFISHREQUEST                10111   //激光炮/黑洞命中鱼请求
#define	 PROTO_BERSERK_SKILL_REQ			10112   //使用狂暴技能请求
#define  PROTO_BERSERK_TRACK_NOTIFY		    10113	//狂暴技能锁定鱼通知
#define  PROTO_BERSERK_PAUSE_NOTIFY			10114   //狂暴技能暂停通知
#define  PROTO_BERSERK_CONTINUE_REQ			10115   //狂暴技能继续通知
#define  PROTO_SKILL_STAGE_CHANGE_REQ		10119	//技能阶段变化请求

#define  PROTO_QUICK_CHAT_REQ				10120	//快捷聊天请求
#define  PROTO_GUN_PRO_LIST_REQ				10121	//炮座附加属性列表请求
#define  PROTO_RAND_GUN_PRO_REQ				10122	//随机炮座附加属性请求
#define  PROTO_GET_GUN_PRO_REQ				10124	//获取当前炮座附加属性请求
#define  PROTO_SKILL_BULLET_POS_REQ			10125	//技能子弹位置请求
#define  PROTO_EXTEND_EVENT_DO				10131	//鱼场额外事件请求(金蛋鱼 翻牌鱼等)

#define  CLIENTRECHARGEREQUEST              10200   //客户端充值请求
#define  RECHGEIDFYCODEREQUEST              10201   //RechgeIdfyCodeRequest
#define  RECHGEMESSAGEREQUEST               10202   //RechgeMessageRequest
#define  PAYRESULTREQUEST                   10203   //PayResultRequest
#define	 ORDER_BROADCAST_REQUEST			10301	//发送订单广播请求
#define  LANGUAGE_VERSION_REQUEST			10302	//语言版本请求
#define  TICKET_EXCHANGE_REQUEST			10303	//奖券兑换请求
#define  LOGOUT_REQUEST						10304	//登出请求
#define	 ROOM_TABLE_REQUEST					10305	//房间桌子列表请求
#define	 ENTER_SPECIFY_SEAT_REQUEST			10306	//进入指定座位请求
#define  FINDPASSWD_VERIFYCODE_NEW_REQUEST	10307	//找回密码验证码请求（新）
#define  RESET_PASSWD_NEW_REQUEST			10308	//重置密码请求（新）
#define  QUERY_PHONEU_WEALTH_REQUEST		10309	//丰游金币奖券查询请求
#define  DRAW_PHONEU_WEALTH_REQUEST			10310	//丰游金币奖券代出请求
#define  DEPOSIT_PHONEU_WEALTH_REQUEST		10311	//丰游金币奖券代入请求
#define  PAUSE_HEARTBEAT_REQUEST			10312	//暂停心跳请求
#define  RESUME_HEARTBEAT_REQUEST			10313	//恢复心跳请求
#define  QUERY_VIPFISHREWARD_REQUEST  		10314	//查询vip捕鱼鱼分奖励请求
#define  USER_ARENAINTO_REQUEST 		 	10316	//用户竞技赛信息请求
#define  ARENA_RANK_REQUEST  				10317	//竞技赛排名请求
#define  ARENA_TOPTHREE_REQUEST  			10318	//竞技赛前3名请求
#define  IPLIST_REQUEST						10319	//IP列表请求
#define  TALENT_LOOK_REQ					10320	//达人挑战查看请求
#define  TALENT_SIGNUP_REQ					10321	//达人挑战报名请求
#define  TALENT_RANK_REQ					10322	//达人挑战排行请求
#define  QUERY_3RD_WEALTH_REQUEST			10330	//第三方财富查询请求
#define  DRAW_3RD_WEALTH_REQUEST			10331	//第三方财富代出请求
#define  DEPOSIT_3RD_WEALTH_REQUEST			10332	//第三方财富代入出请求
#define  CHANGE_SEAT_REQUEST				10333	//改变座位请求
#define  SYNC_SCENE_REQUEST					10334	//同步场景请求
#define  QUERY_3RD_ITEM_REQUEST			    10335	//第三方道具查询请求
#define  EXCHANGE_3RD_ITEM_REQUEST          10336	//第三方道具兑换请求
#define  COIN_EXCHANGE_RECORD_REQUEST       10337   //第三方金币兑换记录请求
#define  BETRESULT_3RD_NOTIFY				10338   //比赛结果通知
#define  QUERY_SIGNIN_REQUEST  				10340	//签到查询请求
#define  SIGNIN_REQUEST  					10341	//签到请求
#define  QUERY_LIVENESS_TASK_INFO_REQUEST	10345	//查询活跃度任务信息请求
#define  GET_LIVENESS_GIFT_REQUEST			10346	//获取活跃度礼包请求
#define  ROOM_STATUS_REQUEST				10347	//房间状态请求（是否开启）
#define  FUNCTION_LIMIT_REQUEST  			10348	//功能限制请求
#define  QUERY_IDINFO_REQUEST  				10350	//查询实名认证信息请求
#define  SUBMIT_IDINFO_REQUEST  			10351	//提交实名认证信息请求
#define  LIMIT_IDINFO_REQUEST  				10352	//实名认证信息限制请求
#define  QUERY_GIFT_REQUEST  				10355	//查询礼包兑换码请求
#define  USE_GIFT_REQUEST  					10356	//使用礼包兑换码请求
#define  QUERY_OPERATE_ACTIVITY_REQUEST  	10360	//查询运营活动请求
#define  GET_OPERATE_ACTIVITY_REQUEST  		10361	//获取运营活动奖励请求
#define  RULE_OPERATE_ACTIVITY_REQUEST  	10362	//获取运营活动规则请求
#define  ACTIVITY_LIST_REQUEST  			10365	//获取活动列表请求
#define  QUERY_DISCOUNTGIFT_REQUEST  		10367	//查询特惠礼包请求
#define  BUYRET_DISCOUNTGIFT_REQUEST  		10368	//特惠礼包购买结果请求
#define  RECHARGE_CHECKEX_REQUEST  			10369	//充值校验请求（新）
#define  GIFTSHOP_LIST_REQUEST				10370	//兑奖商城列表请求
#define  QUERY_ITEM_DESC_REQUEST			10371	//查询商品描述请求
#define  QUERY_ITEM_INFO_REQUEST			10372	//查询商品信息请求
#define  ROOM_STATUS_NEW_REQUEST			10373	//房间状态请求(新)
#define	 SHOPLIST_NEW_REQUEST				10375	//商城列表请求（新）
#define	 GIFTSHOP_LOGIN_REQUEST				10376	//礼品商城兑换请求
#define	 QUERY_DISCOUNTGIFTEX_REQUEST  		10377	//查询特惠礼包请求（包含下线特惠礼包）
#define	 QUERY_RECHARGE_ITEM_REQUEST  		10378	//查询充值商品请求
#define	 QUERY_NEWUSER_GIFT_REQUEST  		10379	//查询新手充值礼包请求
#define  QUERY_SHAREINFO_REQUEST			10380	//查询分享信息请求
#define  QUERY_REWARDINFO_REQUEST			10381	//查询奖励信息请求
#define  USER_GROUP_REQUEST					10382	//用户分组请求
#define  QUERY_CORNUCOPIA_REQUEST			10385	//查询聚宝盆请求
#define  RECEIVE_CORNUCOPIA_REQUEST			10386	//领取聚宝盆请求
#define  CHALLENGE_SIGNUP_REQUEST			10390	//新竞技赛报名请求
#define  CHALLENGE_LIST_REQUEST				10392	//新竞技赛比赛列表请求
#define  CHALLENGE_RANK_REQUEST				10393	//新竞技赛排行榜请求
#define  QUERY_AD_REQUEST					10394	//新竞技赛排行榜请求
#define  CHALLENGE_DYNAMIC_RANK_REQUEST		10395	//新竞技赛动态排行请求
#define  PROTO_REQ_QUERY_DICT_UPDATE_COMMON 10396	//获取UI,规则,弹窗配置请求
#define  PROTO_REQ_QUERY_DICT_UPDATE_UI	    10397	//获取UI,规则,弹窗配置请求 版本2
#define  ACCOUTN_REGISTER_REQUEST			10400	//注册账号请求
#define  ACCOUNT_REGISTER_VERIFY_REQUEST	10401	//注册校验请求
#define  OFFERAREWARD_RANK_REQUEST			10402	//悬赏排行版
#define  OFFERAREWARD_TASK_INFO_REQUEST		10403	//悬常任务请求
#define  LANGUAGE_REQUEST					10405	//语言内容请求
#define  GET_TASK_REWARD_REQUEST 			10406	//获取任务奖励请求
#define  USER_LEVEL_REQUEST 				10407	//玩家等级请求
#define  USER_NICK_REQUEST 					10409	//玩家昵称请求
#define  QUERY_DICT_GIVECOIN_REQUEST 		10411	//赠送金币配置请求
#define  FUNCTION_REQUEST 					10412	//功能请求
#define  VIP_ROOMLIST_REQUEST				10416	//VIP房间列表请求
#define  VIPROOM_SIGNUP_STATUS_REQUEST		10417	//VIP房间报名状态请求
#define  USER_TRACK_FISH_NOTIFY				10418	//用户锁定鱼通知
#define  ITEM_COMPOUND_REQUEST_V1			10420	//道具合成请求V1
#define  USER_WEARED_ITEMS_REQUEST			10422	//玩家已装备道具请求
#define  ITEM_WEAR_REQUEST_V1				10424	//道具装备请求V1
#define  USE_TORPEDO_ITEM_REQUEST			10426	//使用鱼雷请求
#define  CONTI_TASK_REQUEST					10427	//连续任务请求
#define  RECV_CONTI_TASK_REWARD_REQUEST		10428	//领取连续任务奖励请求
#define  RESET_FIRE_TIME_NOTIFY				10429	//重置发炮时间通知

#define  PROTO_SYSTEM_UNIFY_BROADCAST		10430	// 系统统一广播
#define  SELL_ITEM_REQUEST					10431	// 出售道具请求
#define	 PACKAGE_LIST_REQUEST				10432	// 背包物品列表请求
#define	 PACKAGE_ITEM_INFO_REQUEST			10433	// 背包物品详情请求
#define	 USER_SKILL_CONTINUE_REQ			10435	// 用户技能暂停后继续请求

#define  PROTO_CLIREQ_AUTHORIZE_NEW			10500   //登录请求(新)
#define  PROTO_CLIREQ_AUTHORIZE_V2			10501   //登录请求(加密)
#define  PROTO_CLI_ALIAUTH_INFO				10502   //客户端阿里认证信息上传
#define	 PROTO_CLIREQ_AUTHORIZE_V3			10503   //登录请求V3(加密)
#define	 PROTO_CLIREQ_AUTHORIZE_V4			10504   //登录请求V4(加密)

#define PROTO_FISHING_ROOMLIST_REQUEST           10601       // 钓鱼房间列表请求
#define PROTO_FISHING_ENTER_ROOM_REQUEST          10602       // 进入钓鱼房间请求
#define PROTO_FISHING_ANGLE_REQUEST              10604       // 甩杆请求
#define PROTO_FISHING_ROD_LIST_REQUEST            10606       // 鱼竿列表请求
#define PROTO_FISHING_CHANGE_ROD_REQUEST          10607       // 更换鱼竿请求
#define PROTO_FISHING_BUY_ROD_REQUEST             10609       // 购买鱼竿请求
#define PROTO_FISHING_LEAVE_ROOM_REQUEST          10610       // 离开钓鱼房间请求
#define PROTO_FISHING_PLAYER_COUNT_NOTIFY        10600       // 钓鱼人数同步
#define PROTO_FISHING_ROOMLIST_RESPONSE           210601      // 钓鱼房间列表请求响应
#define PROTO_FISHING_ENTER_ROOM_RESPONSE          210602      // 进入钓鱼房间请求响应
#define PROTO_FISHING_ENTER_ROOM_NOTIFY          210603      // 进入钓鱼房间消息同步
#define PROTO_FISHING_ANGLE_RESPONSE              210604      // 甩杆请求响应
#define PROTO_FISHING_ANGLE_NOTIFY              10605       // 甩杆消息同步
#define PROTO_FISHING_ROD_LIST_RESPONSE            210606      // 鱼竿列表请求响应
#define PROTO_FISHING_CHANGE_ROD_RESPONSE          210607      // 更换鱼竿请求响应
#define PROTO_FISHING_CHANGE_ROD_NOTIFY          210608      // 更换鱼竿消息同步
#define PROTO_FISHING_BUY_ROD_RESPONSE             210609      // 购买鱼竿请求请求响应
#define PROTO_FISHING_LEAVE_ROOM_RESPONSE          210610      // 离开钓鱼房间请求响应
#define PROTO_FISHING_LEAVE_ROOM_NOTIFY          210611      // 离开钓鱼房间消息同步

//充值协议
#define  PINGCLIENTRECHARGEREQUEST          11000   //Ping客户端充值请求
#define  PINGPAYRESULTREQUEST               11001   //Ping支付结果请求
#define  BAIDUCLIENTLOGININREQUEST          11010   //百度客户端登入成功请求
#define  BAIDUPAYRESULTREQUEST              11011   //百度支付结果请求
#define  BAIDUMOBGAMEPAYRESULTREQUEST       11013   //百度移动游戏支付结果请求
#define  APPLECLIENTRECHARGEREQUEST         11020   //Apple充值请求
#define  APPLEPAYRESULTREQUEST              11021   //Apple充值应答校验
#define  QIHOOUSERINFOREQUEST				11022	//360获取用户信息请求
#define  QIHOOCLIENTPAYREQUEST				11023	//360用户充值请求
#define  QIHOOPAYRESULTREQUEST				11024	//360用户充值结果校验
#define  XIAOMICLIENTLOGININREQUEST			11025	//小米用户登录成功请求
#define  XIAOMICLIENTPAYREQUEST				11026	//小米用户充值请求
#define  XIAOMIPAYRESULTREQUEST				11027	//小米用户充值结果校验
#define  MIGUCLIENTPAYREQUEST				11028	//咪咕客户端充值请求
#define  MIGUPAYRESULTREQUEST				11029	//咪咕充值应答校验
#define  WAN360CLIENTLOGINREQUEST			11031   //360开放平台客户端登入成功请求
#define  WAN360PAYRESULTREQUEST             11032   //360开放平台支付结果请求
#define  WAN360QUERYREQUEST					11033	//360开放平台用户查询请求

#define  GIFTSHOP_QUERY_TICKET_REQUEST		11034	//礼品商场查询用户TICKET
#define  GIFTSHOP_COST_TICKET_REQUEST		11035	//礼品商场扣除用户TICKET
#define  GIFTSHOP_BRAODCAST_REQUEST			11036	//礼品商场兑换广播

#define  WEBPROXY_REQUEST					11037   //webproxy发送的代理数据
#define  USER_FUNCTION_LIMIT_REQUEST		11038   //用户功能限制
#define  PROTO_TREASURE_REWARD_REQUEST		11039   //用户领取宝箱



//注意----------------------------游服新的协议从11240开始----------------------------------
#define  GAME_CUSTMOER_REQUEST					   11240   //查询客服


// 家族协议
#define CLAN_CREATE_REQUEST					30000
#define CLAN_CREATE_CONDITION_REQ			30001
#define CLAN_SEARCH_DATA_REQUEST			30002
#define CLAN_SEARCH_PALYER_DATA_REQUEST		30003
#define CLAN_JOIN_REQUEST					30004
#define CLAN_ASK_JOINLIST_REQUEST			30005
#define CLAN_ALLOW_JOIN_REQUEST				30006
#define CLAN_INVITE_JOIN_REQUEST			30007
#define CLAN_INVITE_JOINLIST_REQUEST		30008
#define CLAN_INVITE_JOINOPER_REQUEST		30009
#define CLAN_GETOUT_PERFUND_REQUEST			30010
#define CLAN_GETOUT_CLANFUND_REQUEST		30011
#define CLAN_MODIFY_FUNDRATE_REQUEST		30012
#define CLAN_MODIFY_FISHPRICE_REQUEST		30013
#define CLAN_EXCHANGE_FISHCONIN_REQUEST		30014
#define CLAN_DEPOSIT_COIN_REQUEST			30015
#define CLAN_QUIT_REQUEST					30016
#define CLAN_GET_INFO_REQUEST				30017
#define CLAN_MODIFY_INFO_REQUEST			30018
#define CLAN_MAKEOVER_REQUEST				30019
#define CLAN_DISBAND_REQUEST				30020
#define CLAN_GUARD_INFO_REQUEST				30021
#define CLAN_MEMBER_LIST_REQUEST			30022
#define CLAN_EXPEL_MEMBER_REQUEST			30023
#define CLAN_CHANGE_PASSWD_REQUEST			30024
#define CLAN_USER_ROLE_REQUEST				30025

#define CLAN_SEND_MESSAGE_REQUEST			30027
#define CLAN_SEND_MEMBERMSG_REQUEST			30028
#define CLAN_SEARCH_MESSAGE_REQUEST			30029
#define CLAN_CANCEL_JOINCLAN_REQUEST		30032

#define WEALTH_RANK_REQUEST					30050			// 财富排行榜请求
#define SEND_BROADCAST_REQUEST				30051			// 发送飞鸽广播请求
#define PRESENT_COIN_REQUEST				30052			// 赠送金币请求
#define PRESENT_COIN_COND_REQUEST			30053			// 赠送金币条件请求
#define PRESENT_COIN_RECORD_REQUEST			30054			// 赠送金币记录请求
#define HALL_UPDATE_ACCT_INFO_REQUEST		30055			// 修改账号信息请求
#define QUERY_ACCT_INFO_REQUEST				30056			// 查询账号信息请求
#define PRESENT_REQUEST						30057			// 赠送请求
#define PACKAGE_INFO_REQUEST 				30058			// 查询背包信息请求
#define USE_ITEM_REQUEST 					30059			// 使用道具请求
#define HALL_BUY_ITEM_REQUEST				30061			// 购买道具请求
#define CLAN_CRYSTAL_RANK_REQUEST 			30062			// 家族水晶排行请求
#define HALL_QUERY_ITEM_INFO_REQUEST		30063			// 查询道具信息请求
#define HALL_QUERY_ITEM_NUM_REQUEST 		30064			// 查询背包道具数量请求
#define HALL_VIP_DICT_REQUEST				30065			// vip 字典查询
#define HALL_COMMON_DICT_REQUEST			30066			// COMMON 字典查询
#define HALL_VIP_DICT_EX_REQUEST			30067			// vip 字典查询
#define HALL_QUERY_H5_SHARE_REQUEST 		30068			// 查询H5分享应答
#define HALL_H5_SHARE_REQUEST				30069			// H5分享应答
#define HALL_QUERY_H5_ADDDESKTOP_REQUEST	30070			// 查询H5添加桌面应答
#define HALL_H5_ADDDESKTOP_REQUEST			30071			// H5添加桌面应答
#define HALL_CLIENT_CHECK_FAILURE_NOTIFY	30072			// 客户端校验用户失败通知

#define CLAN_ORDER_MEMBER_RESQUEST			30073			//查询具有排序的成员列表
#define HALL_QUERY_IDINFO_REQUEST  			30074			//查询实名认证信息请求-hallsvr
#define HALL_SUBMIT_IDINFO_REQUEST  		30075			//提交实名认证信息请求-hallsvr
#define HALL_LIMIT_IDINFO_REQUEST			30076			//实名认证信息限制请求-hallsvr
#define USER_TOTAL_ONLINE_REQUEST			30077			//玩家累计在线时间请求
#define PRESENT_RECORD_REQUEST_V1			30078			// 赠送金币记录请求v1
#define HALL_RECHARGE_ITEM_REQUEST			30079			// dictviprecharge 查询
#define ITEM_COMPOUND_REQUEST				30080			// 道具合成请求
#define ITEM_WEAR_REQUEST					30081			// 装备道具请求
#define WEARED_ITEMS_REQUEST				30082			// 已装备道具请求
#define ITEM_COMPOUND_INFO_REQUEST			30083			// 道具合成信息请求
#define  QUERY_DISCOUNTGIFT_REQUEST_V2  	30084			//查询特惠礼包请求(第二版，支付特惠活动)
#define PRESENT_GUN_POINT_REQUEST			30085			// 赠送炮座积分条件请求
#define DICT_QUOTA_INFO_REQUEST				30086			// 活动杂项配置表信息请求（积分）


#define  TREASURE_DETAIL_REQUEST			30087			//寻宝详情查询
#define  TREASURE_NOTICE_REQUEST			30088			//寻宝公告查询
#define  TREASURE_OPEN_REQUEST				30089			//寻宝开宝箱查询
#define  TREASURE_RESET_REQUEST				30090			//寻宝重置查询

#define  HALL_COMPETE_INFO_REQUEST			30091			//比赛活动信息请求
#define  HALL_COMPETE_RANK_REQUEST			30092			//比赛活动排行榜请求


#define  HALL_POINT_DICT_REQUEST			30093			//比赛活动排行榜请求
#define  HALL_ACCOUNT_POINT_REQUEST			30094			//比赛活动排行榜请求
#define  USER_MONTH_CARD_REQUEST			30095			//用户月卡信息查询请求
#define  RECEIVE_MONTH_CARD_REWARD_REQUEST	30096			//领取月卡奖励请求
#define  PRESENT_CHECK_REQUEST				30098			//赠送校验请求
#define  USER_PRESENT_INFO_REQUEST			30099			//用户赠送信息请求

#define HALL_H5_LOGINGIFT_REQUEST			30100			// h5登录礼包请求
#define  PROTO_REQ_DIAMONDEXCOIN			30101       	// 钻石兑换金币请求
#define  PROTO_REQ_GETUPLOADTOKEN			30102       	// 获取上传自定义头像token请求
#define  QUERY_USER_INFO_REQUEST			30103      		// 查询玩家信息请求
#define  QUERY_ITEM_INFO_BATCH_REQUEST		30104      		// 批量查询道具信息请求
#define  HALL_GET_PLAYER_PIC_REQUEST        30105           // 查询玩家头像
#define  QUERY_USER_ECWS_ADDRESS_REQUEST    30106           // 查询以太坊用户地址请求
#define  ECWS_WITHDRAW_REQUEST				30107           // 以太坊提现请求
#define  DICT_EOS_REQUEST					30108           // 以太坊兑换配置请求
#define  QUERY_ECWS_RECORD_REQUEST			30109           // 以太坊交易记录请求
#define  ECWS_LIMIT_DAILY_REQUEST			30110           // 以太坊每日限额请求
#define  VERIFY_CODE_REQUEST_V2				30111           // 验证码请求V2
#define  WEALTH_RANK_REQUEST_V2				30112			// 排行榜请求V2
#define	 HALL_SUBMIT_CUSTOM_PIC_REQUEST	    30113	        // 提交自定义头像请求

#define  IAPPPAY_LIMIT_REQUEST              30416	        //爱贝支付限制条件请求
#define  GAME_NEW_CUSTMOER_REQUEST          31240           // 客服信息查询请求(新)
#define  NEWUSER_GIFT_STATUS_REQUEST        31241           // 新手礼包状态

#define  PROTO_DRIFTBOTTLE_CONDITION_REQUEST 30120          //漂流瓶限制条件请求
#define  PROTO_DRIFTBOTTLE_MAKE_REQUEST      30121          //制作漂流瓶请求
#define  PROTO_DRIFTBOTTLE_OPEN_REQUEST      30122          //打开漂流瓶请求
#define  PROTO_DRIFTBOTTLE_TAKEBACK_REQUEST  30123          //收回漂流瓶请求
#define  PROTO_DRIFTBOTTLE_RECORD_REQUEST    30124          //漂流瓶记录请求
#define  PROTO_DRIFTBOTTLE_CHANGEPWD_REQUEST 30125          //修改漂流瓶密码请求
#define  PROTO_DRIFTBOTTLE_QUERY_REQUEST     30126          //漂流瓶查询请求

//保险箱
#define  PROTO_ACC_BANK_COIN_REQ_V2         30130           //存取银行金币请求V2
#define  PROTO_CHK_BANK_PASSWD_REQ          30131           //检查银行密码请求
#define  PROTO_GET_USER_BANK_INFO_REQ       30132           //获取玩家银行信息

/* centersvr协议*/
#define  SEVENDAYS_RECHARGE_INFO_REQUEST	35001			// 七日充值活动信息请求
#define  LOTTERY_DRAW_REQUEST				35002			// 抽奖请求
#define  NEWBEE_LOGIN_INFO_REQUEST			35003			// 新用户登录活动信息请求
#define  RECEIVE_NEWBEE_REWARD_REQUEST		35004			// 领取新手登录活动奖励请求
#define  FORTUNECAT_INFO_REQUEST			35005			// 招财猫信息请求
#define  SIGNIN_INFO_REQUEST				35006			// 签到信息请求
#define  WINLOTTERY_INFO_REQUEST			35007			// 中奖信息请求
//add by meilinfeng
#define  QUERY_TREASURE_CHEST_TITLE_REQUEST 35008			// 宝箱抽奖标题请求
#define  QUERY_TREASURE_CHEST_INFO_REQUEST  35009			// 宝箱抽奖信息请求
#define  TREASURE_CHEST_LOTTERY_REQUEST		35010			// 宝箱抽奖请求

// 摇钱树
#define  PROTO_MONEYTREE_INFO_REQUEST		35011			// 摇钱树信息请求
#define  PROTO_SHAKE_MONEYTREE_REQUEST		35012			// 摇树请求
#define  PROTO_MONEYTREE_RULE_REQUEST		35013			// 摇钱树规则请求

// 新手签到
#define  PROTO_NEWBIE_SIGNIN_INFO_REQUEST	35015			// 新手签到信息请求
#define  PROTO_NEWBIE_SIGNIN_REQUEST		35016			// 新手签到信息请求

// 捕鱼奖池活动
#define  PROTO_JACKPOT_INFO_REQUEST			35017			// 捕鱼奖池信息请求
#define  PROTO_RECEIVE_JACKPOT_REQUEST		35018			// 领取奖池奖励请求
#define  PROTO_TOTAL_JACKPOT_REQUEST		35019			// 总奖池查询请求

// 新活动协议簇
#define  PROTO_OP_ACTI_LIST_REQUEST			35020			// 请求活动列表
#define  PROTO_OP_ACTI_DETIAL_REQUEST		35021			// 请求活动详情
#define  PROTO_OP_ACTI_DO_REQUEST			35022			// 参与活动
#define  PROTO_OP_ACTI_NOTICE_REQUEST		35023			// 请求活动公告
#define  PROTO_OP_ACTI_DO_HISTORY_REQUEST	35024			// 参与活动查询
#define  PROTO_OP_ACTI_REWARD_REQUEST		35025			// 领取活动奖励

#define  PROTO_MINIGAME_LIST_REQ			35030			// 小游戏列表请求
#define  PROTO_ENTER_MINIGAME_REQ			35031			// 进入小游戏请求
#define  PROTO_TROPEDO_CMP_ATTACK_REQ		35033			// 鱼雷赛攻击箱子请求
#define  PROTO_TROPEDO_CMP_WIN_INFO_REQ		35034			// 鱼雷赛获奖信息请求
#define  PROTO_TROPEDO_CMP_REC_INFO_REQ		35035			// 鱼雷赛记录信息请求
#define  PROTO_TROPEDO_CMP_RANK_INFO_REQ	35036			// 鱼雷赛排行榜信息请求

#define  PROTO_REALNAME_SUBMIT_REQ			35040			// 实名认证信息提交请求

#define  PROTO_REQ_AUCTIONITEMINFO          36000           // 获取竞购商品信息请求
#define  PROTO_REQ_AUCTIONSHARE             36001           // 竞购商品请求
#define  PROTO_REQ_AUCTIONONGOING           36003           // 获取竞购商品记录（进行中）请求
#define  PROTO_REQ_AUCTIONPUBLISH           36004           // 获取竞购商品记录（已开奖,中奖记录）请求
#define  PROTO_REQ_AUCTIONLASTPUBLISH       36005           // 获取最新揭晓请求
#define  PROTO_REQ_AUCTIONHISTORY           36006           // 获取往期揭晓请求
#define  PROTO_REQ_SETDELIVERYADDRESS       36007           // 设置收货地址请求
#define  PROTO_SYN_USERQUITAUCTIONNOTIFY    36008      	    // 用户退出一元购通知
#define  PROTO_REQ_GETDELIVERYADDRESS		36010       	// 获取收货地址请求
#define  PROTO_REQ_GETAUCTIONRULE			36011       	// 获取一元购规则请求
#define  PROTO_REQ_MSG_BOARD_INFO			36020			// 获取留言板信息请求
#define  PROTO_REQ_PUBLISH_MSG				36021			// 发布信息请求
#define  PROTO_SYN_USER_QUIT_MSG_BOARD_NOTIFY	36023			// 获取留言板信息请求

#define  FORESTPARTY_LIMIT_REQUEST 		36101 			// 获取森林舞会金币限制请求
#define  FORESTPARTY_JOIN_REQUEST		36102			// 进入舞会房间请求
#define  FORESTPARTY_BETTING_REQUEST	36103			// 投注请求
#define  FORESTPARTY_HISTORY_REQUEST 	36106			// 往期结果请求
#define  FORESTPARTY_USER_QUIT			36107			// 用户退出森林舞会通知
#define  FORESTPARTY_CLEAR_BETTING_REQUEST 36108        // 清除投注信息请求
#define  FORESTPARTY_PARTY_STATUS_REQUEST 36109        // 房间状态信息请求

#define  BETFISH_SYN_START_NOTIFY 		36201 			// 同步生成竞猜通知
#define  BETFISH_SYN_ACTIVE		36202			// 同步竞猜激活通知
#define  BETFISH_BET_REQUEST	36203			// 发起竞猜消息请求
#define  BETFISH_SYN_GIVEUP 	36204			// 同步放弃竞猜通知

#define  PIRATESHIP_CARDFISH_SYNC 	    36206			// 海盗船打爆卡片鱼桌面同步
#define  PIRATESHIP_TREASUREFISH_SYNC 	36207			// 海盗船宝藏鱼领奖桌面同步


#define  REDPACKAGEFISH_CLICK_REQUEST 36302     // 点击红包请求

#define  PROTO_NEWUSER_GIFT_STATUS_REQUEST 36310  //新手礼包状态请求
#define PROTO_GET_NEWUSER_GIFT_REQUEST 36311      //领取新手礼包请求

#define PROTO_WEEKLY_COMPET_RANK_REQUEST 36320  //周赛排名请求

//add by meilinfeng
#define GET_RED_POINT_STATUS_REQUEST		35100			// 获取红点状态
#define SET_RED_POINT_STATUS_REQUEST		35101			// 设置红点状态

//pvp Battle
#define PVP_GET_INFO_REQUEST				37000			// 请求对战的基本信息
#define PVP_GET_ROOM_LIST_REQUEST			37001			// 请求房间列表
#define PVP_CREATE_ROOM_REQUEST				37002			// 请求创建房间
#define PVP_EVENT_REQUEST					37003			// 准备,开始,踢出,退出的事件请求
#define PVP_JOIN_ROOM_REQUEST				37005			// 加入房间

#define PVP_MORAW_WHAT_REQUEST				37006			// 出拳

#define PROTO_FISH_MATCH_RESULT_REQUEST		37009			// 查询比赛结果
#define PROTO_FISH_MATCH_INFO_REQUEST		37010			// 查询比赛信息
#define PROTO_FISH_MATCH_RESULT_REQUESTV2	37014			// 查询比赛结果V2

//trumpet 喇叭
#define PROTO_BROADCAST_CONFIG_REQUEST		37011		// 请求喇叭的配置信息
#define PROTO_BROADCAST_MSG_REQUEST			37012		// 请求发送广播信息


/*
 *====================================================================================
 * 服务器端下发的消息
 *====================================================================================
 */
#define  PROTO_GAMERESP_USERDATA            110000  //登录应答
#define  PROTO_AUTHRESP_AUTHRESULT			109001	//认证结果确认
#define  PROTO_AUTHRESP_GAMEREADY			109002	//游戏已准备好
#define  PROTO_WXAUTHRESP_AUTHRESULT		109003	//微信绑定账号认证结果确认
#define  ROOMLISTRESPONE                    110001  //RoomListRespone
#define  INTOGAMERESPONE                    110002  //IntoGameRespone
#define  OUTGAMERESPONE                     110003  //OutGameRespone
#define  SCENERESPONE                       110004  //SceneRespone
#define  HITFISHRESPONE                     110005  //HitFishRespone
#define  PLAYERJOINRESPONE                  110006  //PlayerJoinRespone
#define  PLAYERINFOCHANGERESPONE            110007  //PlayerInfoChangeRespone
#define  SHOOTRESPONE                       110009  //ShootRespone
#define  SCENESTATERESPONE                  110010  //SceneStateRespone
#define  ACCOUNTRESPONE                     110011  //AccountRespone
#define  SEVENGIFTRESPONE                   110012  //SevenGiftRespone
#define  ONLINETIMERESPONE                  110014  //OnLineTimeRespone
#define  SENDMSGRESPONE                     110017  //SendMsgRespone
#define  RECVMSGRESPONE                     110018  //RecvMsgRespone
#define  GIVEONLINEGIFTRESPONE              110019  //GiveOnLineGiftRespone
#define  UPDATEACCTINFORESPONE              110020  //修改账号信息应答
#define  GETPLAYERPICRESPONE                110021  //获取头像名称应答
#define  SENDMSGTOCLIENTRESPONE             110022  //世界聊天应答
#define  SENDPUBLICMSGRESPONE               110023  //公聊发送消息应答
#define  GETFREECHATCOUNTRESPONE            110025  //获取免费聊天次数应答
#define  OUTCOMPETRESPONE                   110026  //比赛房退出应答
#define  GETNICKIFUPDATERESPONE             110027  //获取昵称是否修改应答
#define  SYSTEMBROADCASTRESPONE             110029  //发送系统广播应答
#define  HITFISHMISSRESPONE                 110030  //捕鱼未命中应答
#define  SHOOTSELFRESPONE                   110031  //发射子弹应答给玩家本身
#define  COMPETBROADCASTRESPONE             110032  //比赛广播应答
#define  DESIGNROOMINFORESPONE              110033  //指定房间信息应答
#define  BILLCOMPOUNDRESPONE                110034  //合成话费卡应答
#define  BILLPIECECOUNTRESPONE              110035  //获取话费卡碎片应答
#define  PVPROOMSTATUSRESPONE               110036  //获取玩家闯关状态应答
#define  PVPROOMGETREWARDRESPONE            110037  //玩家获取奖励应答
#define  NOTICERESPONE                      110039  //公告应答
#define  PVPROOMWAITSTOPRESPONE             110040  //PVP房玩家等待时终止应答
#define  ACCOUNTALMSINFORESPONE				110041	//玩家可领救济金信息应答
#define  ACCOUNTALMSRESPONE					110042  //玩家领取救济金应答
#define  STOP_SERVER_NOTICE  				110045	//停服通知
#define  QUERY_SHARPTIME_GIFT_RESPONE  		110046	//查询整点礼包应答
#define  RECEIVE_SHARPTIME_GIFT_RESPONE  	110047	//领取整点礼包应答
#define  BUY_ITEM_RESPONE                   110050  //购买道具应答
#define  SHOP_LIST_RESPONE                  110051  //获取商城列表应答
#define  ADD_FRIEND_RESPONE                 110052  //增加好友应答
#define  DELETE_FRIEND_RESPONE              110053  //删除好友应答
#define  GIVE_COIN_RESPONSE                 110054  //赠送金币应答
#define  CREATE_ROBOT_RESPONE               110055  //创建机器人应答
#define  DELETE_ROBOT_RESPONE               110056  //删除机器人应答
#define  TASK_INFO_RESPONE                  110060  //任务信息应答
#define  TASK_INFO_NEW_RESPONE              110060  //任务信息应答（新）
#define  ACCESS_BANK_COIN_RESPONE           110061  //存取银行金币应答
#define  MODIFY_BANK_PASSWD_RESPONE         110063  //修改银行密码应答
#define  RECHARGE_RESPONE                   110064  //充值应答
#define  RECEIVE_VIP_REWARD_RESPONE  		110065	//接收vip礼包应答
#define  VIP_REWARD_RESPONE                 110067  //vip礼包应答
#define  NEWBIE_COMPLETE_RESPONE            110068  //新手引导完成应答
#define  RECHARGE_CHECK_RESPONE             110069  //充值校验应答
#define  BUY_OBJECT_ITEM_RESPONE            110070  //购买实物道具应答
#define  VERIFY_CODE_RESPONE                110071  //获取验证码应答
#define  ALL_RANK_RESPONE                   110072  //所有玩家排名应答
#define  USER_RANK_RESPONE                  110073  //玩家排名应答
#define  SEND_MAIL_RESPONE                  110074  //发送邮件应答
#define  USE_TEL_FARE_ITEM_RESPONE          110077  //使用话费道具应答
#define  USE_OBJECT_ITEM_RESPONE            110078  //使用实物道具应答
#define  ORDER_INFO_RESPONE                 110079  //订单信息应答
#define  OBJECT_SHOP_LIST_RESPONE           110080  //获取实物商城列表应答
#define  COIN_RANK_RESPONE                  110081  //金币排名应答
#define  TICKET_RANK_RESPONE                110082  //奖券排名应答
#define  USER_ITEM_RESPONE                  110083  //用户道具应答
#define  USER_CURRENCY_RESPONE              110084  //用户货币信息应答
#define  QUERY_EVENT_TASK_INFO_RESPONE      110085  //查询比赛活动任务信息应答
#define  COMPLETE_EVENT_TASK_RESPONE        110086  //完成比赛活动任务应答
#define  EVENT_TASK_INFO_RESPONE            110087  //比赛活动任务信息应答
#define  OBJ_ORDER_BROADCAST_RESPONE		110088	//实物订单广播应答
#define  CLIENT_CONTROL_RESPONE				110089	//客户端控制应答
#define  USER_SHARE_RESPONE  				110090	//用户分享应答
#define  USER_ACCEPT_SHARE_RESPONE  		110091	//用户接受分享应答
#define  COMPET_RANK_RESPONE				110092	//捕鱼达人排名应答
#define  COMPET_REWARD_RESPONE				110093	//捕鱼达人领取奖励应答
#define  NEW_MAIL_NOTIFY					110094	//用户新邮件通知
#define  READ_MAIL_RESPONE                  110095  //读取邮件应答
#define	 GET_MAIL_AWARD_RESPONE  			110096	//领取邮件应答
#define  MAIL_LIST_RESPONE                  110097  //邮件列表应答
#define	 CLIENT_HEARTBEAT_RESPONE			110098	//客户端心跳应答
#define  COMPET_RANK_NEW_RESPONE			110099	//捕鱼达人排名应答（新）
#define  UPD_UPLOAD_RESPONSE                110100  //上传更新包应答
#define  UPD_UPDATELIST_RESPONSE            110101  //更新列表应答
#define  UPD_NOTICE_RESPONSE				110105	//公告列表应答（更新服务器）
#define  UPD_SWITCH_RESPONSE				110106  //开关列表应答（更新服务器）
#define  GAME_SWITCH_RESPONSE				110107  //开关列表应答（游戏服务器）
#define  QUERY_DICT_UI_RESPONSE			    110108	//查询UI应答
#define  QUERY_DICT_RULE_RESPONSE			110109	//查询规则应答
#define  MAIL_LIST_RESPONE_EX               110110  //邮件列表应答(支持翻页)
#define  SKILLHITFISHRESPONE                110111  //激光炮/黑洞命中鱼应答(整张桌子广播）
#define  PROTO_BERSERK_SKILL_RESP			110112  //使用狂暴技能请求回复
#define  PROTO_BERSERK_SCORE_NOTIFY         110113  //狂暴积分同步
#define  PROTO_BERSERK_SKILL_NOTIFY			110114  //获得狂暴技能通知
#define  PROTO_BERSERK_DURATION_NOTIFY		110115  //狂暴技能持续/冷却时间同步
#define  PROTO_BERSERK_TRACK_SYNC			110116  //狂暴技能锁定鱼同步
#define  PROTO_BERSERK_END_NOTIFY			110117  //狂暴技能结束同步
#define  PROTO_BERSERK_CONTINUE_NOTIFY		110118  //狂暴技能继续同步
#define  PROTO_SKILL_STAGE_CHANGE_NOTIFY	110119	//技能阶段变化同步

#define	 PROTO_QUICK_CHAT_SYNC				110120	//快捷聊天同步
#define	 PROTO_GUN_PRO_LIST_RESP			110121	//炮座附加属性列表请求回复
#define	 PROTO_RAND_GUN_PRO_RESP			110122	//随机炮座附加属性请求回复
#define	 PROTO_GUN_PRO_NOTIFY				110123	//炮座附加属性同步
#define  PROTO_GET_GUN_PRO_RESP				110124	//获取当前炮座附加属性请求回复
#define  PROTO_SKILL_BULLET_POS_NOTIFY		110125	//技能子弹位置同步
#define	 PROTO_EXTEND_EVENT_NOTIFY			110130	//鱼场额外事件触发通知(金蛋鱼 翻牌鱼等)
#define	 PROTO_EXTEND_EVENT_RESULT			110131	//鱼场额外事件结果(金蛋鱼 翻牌鱼等)
#define  CLIENTRECHARGERESPONE              110200  //客户端充值应答
#define  RECHGEIDFYCODERESPONE              110201  //RechgeIdfyCodeRespone
#define  RECHGEMESSAGERESPONE               110202  //RechgeMessageRespone
#define  PAYRESULTRESPONE                   110203  //PayResultRespone
#define  FISH_BROADCAST_RESPONE				110300	//发送捕鱼广播
#define  ORDER_BROADCAST_RESPONE			110301	//发送订单广播应答
#define  ITEM_BROADCAST_RESPONE			    110302	//发送道具兑换广播
#define	 TICKET_EXCHANGE_RESPONE			110303	//奖券兑换应答
#define  ROOM_TABLE_RESPONE					110305	//房间桌子列表应答
#define  ENTER_SPECIFY_SEAT_RESPONE			110306	//进入指定座位应答
#define  FINDPASSWD_VERIFYCODE_NEW_RESPONE	110307	//找回密码验证码应答（新）
#define  RESET_PASSWD_NEW_RESPONE			110308	//重置密码应答（新）
#define  QUERY_PHONEU_WEALTH_RESPONSE		110309	//丰游金币奖券查询应答
#define  DRAW_PHONEU_WEALTH_RESPONSE		110310	//丰游金币奖券代出应答
#define  DEPOSIT_PHONEU_WEALTH_RESPONSE		110311	//丰游金币奖券代入应答
#define  QUERY_VIPFISHREWARD_RESPONE  		110314	//查询vip捕鱼鱼分奖励应答
#define  CLIENT_IP_PUSH						110315  //客户端IP推送
#define  USER_ARENAINTO_RESPONE  			110316	//用户竞技赛信息应答
#define  ARENA_RANK_RESPONE  				110317	//竞技赛排名应答
#define  ARENA_TOPTHREE_RESPONE  			110318	//竞技赛前3名应答
#define  IPLIST_RESPONE						110319  //IP列表应答
#define  TALENT_LOOK_RESP					110320	//达人挑战查看应答
#define  TALENT_SIGNUP_RESP					110321	//达人挑战报名应答
#define  TALENT_RANK_RESP					110322	//达人挑战排行应答
#define  TALENT_STATUS						110323	//达人挑战状态应答
#define  QUERY_3RD_WEALTH_RESPONSE			110330	//第三方财富查询应答
#define  DRAW_3RD_WEALTH_RESPONSE			110331	//第三方财富代出应答
#define  DEPOSIT_3RD_WEALTH_RESPONSE		110332	//第三方财富代入出应答
#define  CHANGE_SEAT_RESPONE				110333	//改变座位应答
#define  SYNC_SCENE_RESPONE					110334	//同步场景应答
#define  QUERY_3RD_ITEM_RESPONSE			110335	//第三方道具查询应答
#define  EXCHANGE_3RD_ITEM_RESPONSE			110336	//第三方道具兑换应答
#define  COIN_EXCHANGE_RECORD_RESPONSE      110337  //第三方金币兑换记录应答
#define  QUERY_SIGNIN_RESPONE  				110340	//签到查询应答
#define  SIGNIN_RESPONE  					110341	//签到应答
#define  QUERY_LIVENESS_TASK_INFO_RESPONE	110345	//查询活跃度任务信息应答
#define  GET_LIVENESS_GIFT_RESPONE			110346	//获取活跃度礼包应答
#define  ROOM_STATUS_RESPONE				110347	//房间状态应答（是否开启）
#define  FUNCTION_LIMIT_RESPONE  			110348	//功能限制应答
#define  QUERY_IDINFO_RESPONE  				110350	//查询实名认证信息响应
#define  SUBMIT_IDINFO_RESPONE  			110351	//提交实名认证信息响应
#define  LIMIT_IDINFO_RESPONE  				110352	//实名认证权限响应
#define  QUERY_GIFT_RESPONE  				110355	//查询礼包兑换码响应
#define  USE_GIFT_RESPONE  					110356	//使用礼包兑换码响应
#define  QUERY_OPERATE_ACTIVITY_RESPONE  	110360	//查询运营活动响应
#define  GET_OPERATE_ACTIVITY_RESPONE  		110361	//获取运营活动奖励响应
#define  RULE_OPERATE_ACTIVITY_RESPONE  	110362	//获取运营活动规则响应
#define  DATA_OPERATE_ACTIVITY_RESPONE  	110363	//获取运营活动数据响应
#define  ACTIVITY_LIST_RESPONE  			110365	//获取活动列表响应
#define  RECHARGE_RESPONE_EX				110366	//充值应答（新）
#define  QUERY_DISCOUNTGIFT_RESPONE  		110367	//查询特惠礼包响应
#define  BUYRET_DISCOUNTGIFT_RESPONE  		110368	//特惠礼包购买结果响应
#define  RECHARGE_CHECKEX_RESPONE  			110369	//充值校验应答（新）
#define  GIFTSHOP_LIST_RESPONE				110370	//兑奖商城列表应答
#define  QUERY_ITEM_DESC_RESPONE			110371	//查询商品描述应答
#define  QUERY_ITEM_INFO_RESPONE			110372	//查询商品信息应答
#define  ROOM_STATUS_NEW_RESPONE			110373	//房间状态请求(新)
#define  LEAVE_COMPET_RESPONE				110374	//离开竞技赛应答

#define	 GIFTSHOP_LOGIN_RESPONSE			110376	//礼品商城兑换响应
#define  QUERY_DISCOUNTGIFTEX_RESPONE  		110377	//查询特惠礼包响应(包含当前用户的下一级礼包)
#define	 QUERY_RECHARGE_ITEM_RESPONSE 		110378	//查询充值商品请求响应
#define	 QUERY_NEWUSER_GIFT_RESPONSE  		110379	//查询新手充值礼包响应
#define  QUERY_SHAREINFO_RESPONE			110380	//查询分享信息响应
#define  QUERY_REWARDINFO_RESPONE			110381	//查询奖励信息响应
#define  USER_GROUP_RESPONSE				110382	//用户分组应答
#define  QUERY_CORNUCOPIA_RESPONSE			110385	//查询聚宝盆响应
#define  RECEIVE_CORNUCOPIA_RESPONSE		110386	//领取聚宝盆响应
#define  CHALLENGE_SIGNUP_RESPONSE			110390	//新竞技赛报名响应
#define  CHALLENGE_STATUS_NOTIFY			110391	//新竞技赛状态通知
#define  CHALLENGE_LIST_RESPONSE			110392	//新竞技赛比赛列表响应
#define  CHALLENGE_RANK_RESPONSE			110393	//新竞技赛排行榜响应
#define  QUERY_AD_RESPONSE					110394	//新竞技赛排行榜请求
#define  CHALLENGE_DYNAMIC_RANK_RESPONSE	110395	//新竞技赛动态排行应答
#define	 PROTO_RSP_QUERY_DICT_UPDATE_COMMON 110396	//获取UI,规则,弹窗配置请求应答
#define	 PROTO_RSP_QUERY_DICT_UPDATE_UI		110397	//获取UI,规则,弹窗配置请求应答V2

#define  ACCOUTN_REGISTER_RESPONSE			110400	//注册应答
#define  ACCOUNT_REGISTER_VERIFY_RESPONE	110401	//注册校验应答
#define  OFFERAREWARD_RANK_RESPONSE			110402	//悬赏排行版应答
#define  OFFERAREWARD_TASK_INFO_RESPONSE	110403	//悬常任务应答
#define  LANGUAGE_RESPONSE					110405	//语言内容应答
#define  GET_TASK_REWARD_RESPONSE 			110406	//获取任务奖励应答
#define  USER_LEVEL_RESPONSE 				110407	//玩家等级应答
#define  USER_LEVEL_UP_NOTIFY 				110408	//玩家升级通知
#define  USER_NICK_RESPONSE 				110409	//玩家昵称应答
#define  USER_LEVEL_SYNC					110410	//玩家等级同步
#define  QUERY_DICT_GIVECOIN_RESPONSE 		110411	//赠送金币配置应答
#define  FUNCTION_RESPONSE 					110412	//功能应答
#define  FUNCTION_OPEN_NOTIFY 				110413	//功能开启通知
#define  WEARED_ITEMS_SYNC 					110414	//玩家已装备物品通知
#define  FISH_GAIN_ITEMS_NOTIFY 			110415	//捕鱼获得道具通知
#define  VIP_ROOMLIST_RESPONSE				110416	//VIP房间列表应答
#define  VIPROOM_SIGNUP_STATUS_RESPONSE		110417	//VIP房间报名状态应答
#define  USER_TRACK_FISH_NOTIFY_SYNC		110418	//用户锁定鱼通知同步
#define  USER_TRACKSKILL_END_NOTIFY			110419	//用户锁定技能结束通知
#define  USER_TRACKSKILL_PAUSE_NOTIFY		110420	//用户锁定技能暂停通知
#define  USER_SKILL_DURATION_NOTIFY			110421	//用户技能持续时间通知
#define  USER_WEARED_ITEMS_RESPONSE			110422	//玩家已装备道具应答
#define  USER_GAIN_MONEY_NOTIFY				110423	//用户获得钻石通知
#define  ITEM_WEAR_RESPONSE_V1				110424	//道具装备应答V1
#define  USE_TORPEDO_TIEM_SYNC				110425	//使用鱼雷同步
#define  USE_TORPEDO_ITEM_RESPONSE			110426	//使用鱼雷应答
#define  CONTI_TASK_RESPONSE				110427	//连续任务应答
#define  RECV_CONTI_TASK_REWARD_RESPONSE	110428	//领取连续任务奖励应答
#define  USER_SKILL_CONTINUE_NOTIFY			110429  //用户技能暂停后继续通知
#define  PROTO_HALL_NOTICE_NOTIFY			110430	//大厅公告通知
#define  SELL_ITEM_RESPONSE					110431	//出售道具应答
#define	 PACKAGE_LIST_RESPONSE				110432	//背包物品列表应答
#define  PACKAGE_ITEM_INFO_RESPONSE			110433	//背包物品详情应答

#define  SHOOTSKILLNOTIFY					110435  //发射激光炮/黑洞通知(给同桌的基他玩家)
#define  DRAGSKILLNOTIFY					110436	//激光炮/黑洞拖动同步(给同桌的基他玩家)
#define  PROTO_EXTEND_SCENE_NOTIFY			110450	//额外场景信息同步
#define  PROTO_SCENE_WARN_NOTIFY			110451	//鱼阵预警同步

#define  PROTO_CLI_ALIAUTH_INFO_RESP	110502	//阿里认证结果
#define  PROTO_ENCRYPT_KEY_NOTIFY		110504	//加密key通知(DES加密)

//充值协议
#define  PINGCLIENTRECHARGERESPONE          111000  //Ping客户端充值应答
#define  PINGPAYRESULTRESPONE               111001  //Ping支付结果应答
#define  BAIDUCLIENTLOGININRESPONE          111010  //百度客户端登入成功应答
#define  BAIDUPAYRESULTRESPONE              111011  //百度支付结果应答
#define  BAIDUMOBGAMEPAYRESULTRESPONE       111013  //百度移动游戏支付结果应答
#define  APPLECLIENTRECHARGERESPONSE        111020  //Apple充值请求应答
#define  APPLEPAYRESULTRESPONSE             111021  //Apple充值应答校验回复
#define	 QIHOOUSERINFORESPONSE				111022	//360获取用户信息应答
#define	 QIHOOCLIENTPAYRESPONSE				111023	//360用户充值请求应答
#define  QIHOOPAYRESULTRESPONSE				111024	//360用户充值结果校验应答
#define  XIAOMICLIENTLOGININRESPONSE		111025	//小米用户登录成功请求应答
#define	 XIAOMICLIENTPAYRESPONSE			111026  //小米用户充值请求应答
#define  XIAOMIPAYRESULTRESPONSE			111027	//小米用户充值结果校验应答
#define  MIGUCLIENTPAYRESPONSE				111028	//咪咕客户端充值请求应答
#define  MIGUPAYRESULTRESPONSE				111029	//咪咕充值应答校验回复
#define  WAN360CLIENTLOGINRESPONE			111031  //360开放平台客户端登入成功应答
#define  WAN360PAYRESULTRESPONE             111032  //360开放平台支付结果应答
#define  WAN360QUERYRESPONE					111033	//360开放平台用户查询应答


#define  GIFTSHOP_QUERY_TICKET_RESPONSE		111034	//礼品商场查询用户TICKET应答
#define  GIFTSHOP_COST_TICKET_RESPONSE		111035	//礼品商场扣除用户TICKET应答
#define  GIFTSHOP_BRAODCAST_RESPONSE		111036	//礼品商场兑换广播应答
#define  WEBPROXY_RESPONSE					111037  //发送给webproxy的响应数据
#define  USER_FUNCTION_LIMIT_RESPONSE		111038   //用户功能限制响应

#define  PROTO_TREASURE_REWARD_RESPONSE  111039   //用户领取宝箱

//-------------------------------------------------------------------------------------------------------------
#define  GAME_CUSTMOER_RESPONSE  111240  //客服信息响应


//---------------------------------------------------------------------------------------


#define  PROTO_DOUBLE_REWARD				120001	// 双倍奖券通知
#define	 PROTO_DOUBLE_REWARD_READY			120002	// 双倍奖励准备（能源炮）
#define  PROTO_AD_NOTIFY					120003  // 程序运行时，弹窗广告
#define	 PROTO_SKILL_READY					120005  // 激光炮/黑洞准备
#define	 PROTO_SKILL_PREPARED				120006  // 激光炮/黑洞已经准备好
#define  ERROR_RESPONE                      190000  //错误应答

#define PROTO_TREASURE_REWARD_NOTIFY			120004    //宝箱掉落通知



/*
*====================================================================================
* 家族应答协议  230000开始
*====================================================================================
*/
#define CLAN_CREATE_RESPONSE				230000
#define CLAN_CREATE_CONDITION_RESP			230001
#define CLAN_SEARCH_DATA_RESPONSE			230002
#define CLAN_SEARCH_PALYER_DATA_RESPONSE	230003
#define CLAN_JOIN_RESPONSE					230004
#define CLAN_ASK_JOINLIST_RESPONSE			230005
#define CLAN_ALLOW_JOIN_RESPONSE			230006
#define CLAN_INVITE_JOIN_RESPONSE			230007
#define CLAN_INVITE_JOINLIST_RESPONSE		230008
#define CLAN_INVITE_JOINOPER_RESPONSE		230009
#define CLAN_GETOUT_PERFUND_RESPONSE		230010
#define CLAN_GETOUT_CLANFUND_RESPONSE		230011
#define CLAN_MODIFY_FUNDRATE_RESPONSE		230012
#define CLAN_MODIFY_FISHPRICE_RESPONSE		230013
#define CLAN_EXCHANGE_FISHCONIN_RESPONSE	230014
#define CLAN_DEPOSIT_COIN_RESPONSE			230015
#define CLAN_QUIT_RESPONSE					230016
#define CLAN_GET_INFO_RESPONSE				230017
#define CLAN_MODIFY_INFO_RESPONSE			230018
#define CLAN_MAKEOVER_RESPONSE				230019
#define CLAN_DISBAND_RESPONSE				230020
#define CLAN_GUARD_INFO_RESPONSE			230021
#define CLAN_MEMBER_LIST_RESPONSE			230022
#define CLAN_EXPEL_MEMBER_RESPONSE			230023
#define CLAN_CHANGE_PASSWD_RESPONSE			230024
#define CLAN_USER_ROLE_RESPONSE				230025
#define CLAN_USER_CLANINFO_NOTIFY			230026

#define CLAN_SEND_MESSAGE_RESPONSE			230027
#define CLAN_SEND_MEMBERMSG_RESPONSE		230028
#define CLAN_SEARCH_MESSAGE_RESPONSE		230029
#define CLAN_USER_CLANMESSAGE_NOTIFY		230030

#define CLAN_USER_QUIT_NOTIFY				230031		// 玩家退出家族通知
#define CLAN_CANCEL_JOINCLAN_RESPONSE		230032	


#define WEALTH_RANK_RESPONSE				230050			// 财富排行榜应答
#define SEND_BROADCAST_RESPONSE				230051			// 发送飞鸽广播应答
#define PRESENT_COIN_RESPONSE				230052			// 赠送金币应答
#define PRESENT_COIN_COND_RESPONSE			230053			// 赠送金币条件应答
#define PRESENT_COIN_RECORD_RESPONSE		230054			// 赠送金币记录应答
#define HALL_UPDATE_ACCT_INFO_RESPONSE		230055			// 修改账号信息应答
#define QUERY_ACCT_INFO_RESPONSE			230056			// 查询账号信息应答
#define PRESENT_RESPONSE					230057			// 赠送应答
#define PACKAGE_INFO_RESPONSE 				230058			// 查询背包信息应答
#define USE_ITEM_RESPONSE 					230059			// 使用道具应答
#define PACKAGE_INFO_CHANGE_NOTIFY			230060			// 玩家背包改变通知
#define HALL_BUY_ITEM_RESPONSE				230061			// 购买道具应答
#define CLAN_CRYSTAL_RANK_RESPONSE 			230062			// 家族水晶排行应答

#define HALL_QUERY_ITEM_INFO_RESPONSE		230063			// 查询道具信息应答
#define HALL_QUERY_ITEM_NUM_RESPONSE 		230064			// 查询背包道具数量应答
#define HALL_VIP_DICT_RESPONSE				230065			// vip 字典查询应答
#define HALL_COMMON_DICT_RESPONSE			230066			// COMMON 字典查询应答
#define HALL_VIP_DICT_EX_RESPONSE			230067			// vip 字典查询

#define HALL_QUERY_H5_SHARE_RESPONSE 		230068			// 查询H5分享应答
#define HALL_H5_SHARE_RESPONSE				230069			// H5分享应答
#define HALL_QUERY_H5_ADDDESKTOP_RESPONSE	230070			// 查询H5添加桌面应答
#define HALL_H5_ADDDESKTOP_RESPONSE			230071			// H5添加桌面应答

#define CLAN_ORDER_MEMBER_RESPONSE			230073			//获取具有排序的家族成功
#define HALL_QUERY_IDINFO_RESPONSE  		230074			//查询实名认证信息响应-hallsvr
#define HALL_SUBMIT_IDINFO_RESPONSE  		230075			//提交实名认证信息响应-hallsvr
#define HALL_LIMIT_IDINFO_RESPONSE			230076			//实名认证信息限制响应-hallsvr
#define USER_TOTAL_ONLINE_NOTIFY			230077			//玩家累计在线时间通知
#define PRESENT_RECORD_RESPONSE_V1			230078			// 赠送金币记录应答v1
#define HALL_RECHARGE_ITEM_RESPONSE			230079			// dictviprecharge 响应
#define ITEM_COMPOUND_RESPONSE				230080			// 道具合成应答
#define ITEM_WEAR_RESPONSE					230081			// 装备道具应答
#define WEARED_ITEMS_RESPONSE				230082			// 已装备道具应答
#define ITEM_COMPOUND_INFO_RESPONSE			230083			// 道具合成信息应答
#define  QUERY_DISCOUNTGIFT_RESPONSE_V2  	230084			//查询特惠礼包响应(第二版)
#define PRESENT_GUN_POINT_RESPONSE			230085			// 赠送炮座积分条件应答
#define DICT_QUOTA_INFO_RESPONSE			230086			// 活动杂项配置表信息应答（积分）

#define  TREASURE_DETAIL_RESPONSE			230087			//寻宝详情查询
#define  TREASURE_NOTICE_RESPONSE			230088			//寻宝公告查询
#define  TREASURE_OPEN_RESPONSE				230089			//寻宝开宝箱查询
#define  TREASURE_RESET_RESPONSE			230090			//寻宝重置查询

#define  HALL_COMPETE_INFO_RESPONSE			230091			//比赛活动信息应答
#define  HALL_COMPETE_RANK_RESPONSE			230092			//比赛活动排行榜应答


#define  HALL_POINT_DICT_RESPONSE			230093			//比赛活动排行榜请求
#define  HALL_ACCOUNT_POINT_RESPONSE		230094			//比赛活动排行榜请求
#define  USER_MONTH_CARD_RESPONSE			230095			//用户月卡信息查询应答
#define  RECEIVE_MONTH_CARD_REWARD_RESPONSE	230096			//领取月卡奖励应答

#define  POINT_COMPETE_STATUS_NOTIFY		230097			//积分赛状态通知
#define  PRESENT_CHECK_RESPONSE				230098			//赠送校验应答
#define  USER_PRESENT_INFO_RESPONSE			230099			//用户赠送信息应答
#define	 HALL_H5_LOGINGIFT_RESPONSE			230100			// h5登录礼包应答
#define  PROTO_RSP_DIAMONDEXCOIN			230101      	// 钻石兑换金币请求应答
#define  PROTO_RSP_GETUPLOADTOKEN			230102      	// 获取上传自定义头像token请求应答
#define  QUERY_USER_INFO_RESPONSE			230103      	// 查询玩家信息应答
#define  QUERY_ITEM_INFO_BATCH_RESPONSE		230104      	// 批量查询道具信息应答
#define  HALL_GET_PLAYER_PIC_RESPONSE       230105          // 查询玩家头像应答
#define  QUERY_USER_ECWS_ADDRESS_RESPONSE   230106           // 查询以太坊用户地址应答
#define  ECWS_WITHDRAW_RESPONSE				230107           // 以太坊提现应答
#define  DICT_EOS_RESPONSE					230108           // 以太坊兑换配置应答
#define  QUERY_ECWS_RECORD_RESPONSE			230109           // 以太坊交易记录应答
#define  ECWS_LIMIT_DAILY_RESPONSE			230110           // 以太坊每日限额应答
#define  VERIFY_CODE_RESPONSE_V2			230111           // 验证码应答V2
#define  WEALTH_RANK_RESPONSE_V2			230112			 // 排行榜应答V2
#define	 HALL_SUBMIT_CUSTOM_PIC_RESPONE	    230113	        // 提交自定义头像应答
#define	 HALL_AUDITING_CUSTOM_PIC_RESPONE	230114	        // 自定义头像审核结果通知

#define  PROTO_ACC_BANK_COIN_RESP_V2        230130  //存取银行金币应答V2
#define  PROTO_CHK_BANK_PASSWD_RESP         230131  //检查银行密码应答
#define  PROTO_GET_USER_BANK_INFO_RESP      230132  //查询银行信息应答


#define  IAPPPAY_LIMIT_RESPONSE             230416	        //爱贝支付限制条件请求应答
#define  GAME_NEW_CUSTMOER_RESPONSE        231240          // 客服信息请求应答(新)
#define  NEWUSER_GIFT_STATUS_RESPONSE      231241          // 新手礼包状态应答

#define  PROTO_DRIFTBOTTLE_CONDITION_RESPONSE 230120          //漂流瓶限制条件响应
#define  PROTO_DRIFTBOTTLE_MAKE_RESPONSE      230121          //制作漂流瓶响应
#define  PROTO_DRIFTBOTTLE_OPEN_RESPONSE      230122          //打开漂流瓶响应
#define  PROTO_DRIFTBOTTLE_TAKEBACK_RESPONSE  230123          //收回漂流瓶响应
#define  PROTO_DRIFTBOTTLE_RECORD_RESPONSE    230124          //漂流瓶记录响应
#define  PROTO_DRIFTBOTTLE_CHANGEPWD_RESPONSE    230125       //修改漂流瓶密码响应
#define  PROTO_DRIFTBOTTLE_QUERY_RESPONSE     230126          //漂流瓶查询响应

/*centersvr应答协议*/
#define	 SEVENDAYS_RECHARGE_INFO_RESPONSE	235001			// 七日充值活动信息应答
#define  LOTTERY_DRAW_RESPONSE				235002			// 抽奖应答
#define  NEWBEE_LOGIN_INFO_RESPONSE			235003			// 新用户登录活动信息应答
#define  RECEIVE_NEWBEE_REWARD_RESPONSE		235004			// 领取新手登录活动奖励应答
#define  FORTUNECAT_INFO_RESPONSE			235005			// 招财猫信息应答
#define  SIGNIN_INFO_RESPONSE				235006			// 签到信息应答
#define  WINLOTTERY_INFO_RESPONSE			235007			// 中奖信息应答

//add by meilinfeng
#define  QUERY_TREASURE_CHEST_TITLE_RESPONE 235008			// 宝箱抽奖标题应答
#define  QUERY_TREASURE_CHEST_INFO_RESPONSE 235009			// 宝箱抽奖信息应答
#define  TREASURE_CHEST_LOTTERY_RESPONSE	235010			// 宝箱抽奖中奖应答

// 摇钱树
#define  PROTO_MONEYTREE_INFO_RESPONSE		235011			// 摇钱树信息应答
#define  PROTO_SHAKE_MONEYTREE_RESPONSE		235012			// 摇树应答
#define  PROTO_MONEYTREE_RULE_RESPONSE		235013			// 摇钱树规则应答

// 新手签到
#define  PROTO_NEWBIE_SIGNIN_INFO_RESPONSE	235015			// 新手签到信息应答
#define  PROTO_NEWBIE_SIGNIN_RESPONSE		235016			// 新手签到信息应答

// 捕鱼奖池活动
#define  PROTO_JACKPOT_INFO_RESPONSE		235017			// 捕鱼奖池信息应答
#define  PROTO_RECEIVE_JACKPOT_RESPONSE		235018			// 领取奖池奖励应答
#define  PROTO_TOTAL_JACKPOT_RESPONSE		235019			// 总奖池应答

// 新活动协议簇
#define  PROTO_OP_ACTI_LIST_RESPONSE		235020			// 请求活动列表应答
#define  PROTO_OP_ACTI_DETIAL_RESPONSE		235021			// 请求活动详情应答
#define  PROTO_OP_ACTI_DO_RESPONSE			235022			// 参与活动应答
#define  PROTO_OP_ACTI_NOTICE_RESPONSE		235023			// 请求活动公告应答
#define  PROTO_OP_ACTI_DO_HISTORY_RESPONSE	235024			// 参与活动查询应答
#define  PROTO_OP_ACTI_REWARD_RESPONSE		235025			// 领取活动奖励应答

#define  PROTO_MINIGAME_LIST_RESP			235030			// 小游戏列表请求回复
#define  PROTO_ENTER_MINIGAME_RESP			235031			// 进入小游戏请求回复
#define  PROTO_TROPEDO_CMP_ROOMINFO_NOTIFY	235032			// 鱼雷赛房间信息同步
#define  PROTO_TROPEDO_CMP_ATTACK_RESP		235033			// 鱼雷赛攻击箱子请求回复
#define  PROTO_TROPEDO_CMP_WIN_INFO_RESP	235034			// 鱼雷赛获奖信息请求回复
#define  PROTO_TROPEDO_CMP_REC_INFO_RESP	235035			// 鱼雷赛记录信息请求回复
#define  PROTO_TROPEDO_CMP_RANK_INFO_RESP	235036			// 鱼雷赛排行榜信息请求回复

#define  PROTO_REALNAME_SUBMIT_RESP 		235040			// 实名认证信息提交请求回复
#define  PROTO_REALNAME_LIMIT_NOTIFY 		235041			// 实名认证限制信息同步

#define  PROTO_RSP_AUCTIONITEMINFO          236000          // 获取竞购商品信息请求应答
#define  PROTO_RSP_AUCTIONSHARE             236001          // 竞购商品请求应答
#define  PROTO_SYN_AUCTIONSHAREINFONOTIFY   236002          // 竞购商品份额信息通知
#define  PROTO_RSP_AUCTIONONGOING           236003          // 获取竞购商品记录（进行中）请求应答
#define  PROTO_RSP_AUCTIONPUBLISH           236004          // 获取竞购商品记录（已开奖,中奖记录）请求应答
#define  PROTO_RSP_AUCTIONLASTPUBLISH       236005          // 获取最新揭晓请求应答
#define  PROTO_RSP_AUCTIONHISTORY           236006          // 获取往期揭晓请求应答
#define  PROTO_RSP_SETDELIVERYADDRESS       236007          // 设置收货地址请求应答
#define  PROTO_SYN_AUCTIONPUBLISHINFONOTIFY 236009      	// 开奖信息通知
#define PROTO_RSP_GETDELIVERYADDRESS        236010   	    // 获取收货地址请求应答
#define  PROTO_RSP_GETAUCTIONRULE			236011          // 获取一元购规则请求应答
#define  PROTO_RSP_MSG_BOARD_INFO			236020          // 获取留言板信息请求应答
#define  PROTO_RSP_PUBLISH_MSG				236021          // 发布信息请求应答
#define  PROTO_SYN_NEW_MSG_NOTIFY			236022          // 留言板新信息通知

#define  FORESTPARTY_LIMIT_RESPONSE		236101			// 获取森林舞会金币限制请求应答 
#define  FORESTPARTY_JOIN_RESPONSE 		236102			// 进入舞会房间请求应答
#define  FORESTPARTY_BETTING_RESPONSE	236103			// 投注请求应答
#define  FORESTPARTY_SYN_STATUS			236104			// 同步房间状态
#define  FORESTPARTY_SYN_RESULT 		236105			// 同步抽奖结果
#define  FORESTPARTY_HISTORY_RESPONSE 	236106			// 往期结果请求应答
#define  FORESTPARTY_CLEAR_BETTING_RESPONSE 236108      // 清除投注信息请求应答
#define  FORESTPARTY_PARTY_STATUS_RESPONSE 236109      // 房间状态信息请求应答

#define  BETFISH_BET_RESPONSE 236203      // 发起竞猜消息请求应答
#define  BETFISH_SYN_QUIT_NOTIFY 236205      // 退出竞猜通知

#define  REDPACKAGEFISH_SYN_START_NOTIFY 236301      // 同步生成红包通知
#define  REDPACKAGEFISH_CLICK_RESPONSE 236302      // 点击红包应答
#define  REDPACKAGEFISH_SYN_CLICKED_NOTIFY 236303      // 同步红包领取通知

#define PROTO_NEWUSER_GIFT_STATUS_RESPONSE 236310  //新手礼包状态应答
#define PROTO_GET_NEWUSER_GIFT_RESPONSE 236311     //领取新手礼包应答

#define PROTO_WEEKLY_COMPET_RANK_RESPONE 236320  //周赛排名应答

//add by meilinfeng
#define GET_RED_POINT_STATUS_RESPONSE		235100			// 获取红点状态应答
#define SET_RED_POINT_STATUS_RESPONSE		235101			// 设置红点应答
//PVP
#define PVP_GET_INFO_RESPONSE				237000			// 请求对战的基本信息应答
#define PVP_GET_ROOM_LIST_RESPONSE			237001			// 请求房间列表应答
#define PVP_CREATE_ROOM_RESPONSE			237002			// 请求创建房间应答
#define PVP_EVENT_RESPONSE					237003			// 准备,开始,踢出,退出的事件请求应答

#define PVP_EVENT_NOTIFY					237004			// 通知客户端事件
#define PVP_JOIN_ROOM_RESPONSE				237005			// 加入房间应答
#define PVP_MORAW_WHAT_RESPONSE				237006			// 出拳应答
#define PVP_RESULT_NOTIFY					237007			// 通知比赛结果
#define PVP_INVITE_NOTIFY					237008			// 通知客户端邀请事件

#define PROTO_FISH_MATCH_RESULT_RESPONSE	 237009			// 查询比赛结果
#define PROTO_FISH_MATCH_INFO_RESPONSE	 237010			    // 查询比赛具体结果
#define PROTO_FISH_MATCH_RESULT_RESPONSEV2	 237014			// 查询比赛结果v2

//trumpet 喇叭
#define PROTO_BROADCAST_CONFIG_RESPONSE		237011		// 响应喇叭的配置信息
#define PROTO_BROADCAST_MSG_RESPONSE		237012		// 响应发送广播信息
#define	PROTO_BROADCAST_MSG_NOTIFY			237013		// 广播消息


/*
*====================================================================================
* 账号中心协议  6000开始
*====================================================================================
*/
#define PROTO_ACCOUNT_AUTH_REQ					6000	// 账号认证请求
#define PROTO_ACCOUNT_AUTH_RESP					6001	// 账号认证应答
#define PROTO_ACCOUNT_REGISTER_REQ				6002	// 账号注册请求
#define PROTO_ACCOUNT_REGISTER_RESP				6003	// 账号注册应答
#define PROTO_ACCOUNT_BINDMOBILE_REQ			6004	// 账号绑定手机请求
#define PROTO_ACCOUNT_BINDMOBILE_RESP			6005	// 账号绑定手机应答
#define PROTO_ACCOUNT_MODIFYPWD_REQ				6006	// 账号修改密码请求
#define PROTO_ACCOUNT_MODIFYPWD_RESP			6007	// 账号修改密码应答
#define PROTO_ACCOUNT_MODIFYNICK_REQ			6008	// 账号修改昵称请求
#define PROTO_ACCOUNT_MODIFYNICK_RESP			6009	// 账号修改昵称应答
#define PROTO_ACCOUNT_MODIFYPIC_REQ				6010	// 账号修改头像请求
#define PROTO_ACCOUNT_MODIFYPIC_RESP			6011	// 账号修改头像应答
#define PROTO_ACCOUNT_RESETPWD_REQ				6012	// 账号重置密码请求
#define PROTO_ACCOUNT_RESETPWD_RESP				6013	// 账号重置密码应答
#define PROTO_ACCOUNT_VERIFYCODE_REQ			6014	// 账号中心验证码请求
#define PROTO_ACCOUNT_VERIFYCODE_RESP			6015	// 账号中心验证码应答

/*
*====================================================================================
* 大厅服务器协议  6200开始
*====================================================================================
*/
#define PROTO_HALL_CLAN_CREATE_COST_REQ			6200	// 家族创建扣款请求
#define PROTO_HALL_CLAN_CREATE_COST_RESP		6201	// 家族创建扣款请求
#define PROTO_HALL_CLAN_ADDCOIN_NOTIFY			6202	// 通知游服增加金币
#define PROTO_HALL_CLAN_DEPOSIT_COIN_REQ		6203	// 家族长存金币扣款请求
#define PROTO_HALL_CLAN_DEPOSIT_COIN_RESP		6204	// 家族长存金币扣款应答
#define PROTO_HALL_PIC_SYNC						6205	// gamesvr->hallsvr 头像同步
#define PROTO_HALL_SEND_BROADCAST_COST_REQ		6206	// 发送飞鸽广播扣款请求
#define PROTO_HALL_SEND_BROADCAST_COST_RESP		6207	// 发送飞鸽广播扣款应答
#define PROTO_HALL_PRESENT_COIN_REQ				6208	// 赠送金币请求 hallsvr->gamesvr
#define PROTO_HALL_PRESENT_COIN_RESP			6209	// 赠送金币应答 gamesvr->hallsvr
#define PROTO_HALL_BUY_ITEM_COST_REQ			6210	// 购买道具扣款请求
#define PROTO_HALL_BUY_ITEM_COST_RESP			6211	// 购买道具扣款应答
#define PROTO_HALL_PRESENT_REQ					6212	// 赠送请求 hallsvr->gamesvr
#define PROTO_HALL_PRESENT_RESP					6213	// 赠送应答 gamesvr->hallsvr
#define PROTO_HALL_ADD_ITEM_NOTIFY				6214	// 增加道具通知 gamesvr->hallsvr
#define PROTO_HALL_GAIN_HALVE_NOTIFY			6215	// 收益减半通知 hallsvr->gamesvr
#define PROTO_HALL_WEARED_ITEMS_NOTIFY 			6216	// 玩家已装备物品通知
#define PROTO_HALL_HIT_ITEM_NOTIFY				6217	// 捕鱼命中道具通知 gamesvr->hallsvr
#define PROTO_HALL_SIGNIN_BONUS_NOTIFY			6218	// 签到倍数通知 hallsvr->gamesvr
#define PROTO_HALL_CLAN_ADDITEMEX_NOTIFY		6219	// 通知游服增加道具
#define	PROTO_HALL_DEDUCT_COIN_REQUEST			6220	// hallsvr -> gamesvr 扣款请求
#define PROTO_HALL_DEDUCT_COIN_RESPONSE			6221	// gamesvr -> hallsvr 扣款应答
#define PROTO_HALL_COST_ITEM_REQUEST			6222	// gamesvr -> hallsvr 扣道具请求
#define PROTO_HALL_COST_ITEM_RESPONSE			6223	// hallsvr -> gamesvr 扣道具应答
#define PROTO_HALL_ADD_ITEM_PRESENT_NOTIFY		6224	// 赠送增加道具通知 hallsvr->gamesvr
#define PROTO_HALL_SUBMIT_IDINFO_NOTIFY			6225	// 提交实名认证通知

#define PROTO_HALL_DRIFTBOTTLE_REQ				6226	// 制作漂流瓶请求 hallsvr->gamesvr
#define PROTO_HALL_DRIFTBOTTLE_RESP				6227	// 制作漂流瓶应答 gamesvr->hallsvr
#define PROTO_HALL_ADD_BOTTLEITEM_NOTIFY		6228	// 赠送漂流瓶道具通知 hallsvr->gamesvr
/*
*====================================================================================
* 机器人服务协议  6500开始
*====================================================================================
*/
#define PROTO_ROBOT_CREATE_REQ					6500	// 机器人创建请求
#define PROTO_ROBOT_CREATE_RESP					6501	// 机器人创建应答
#define PROTO_ROBOT_DROP_REQ					6502	// 机器人删除请求
#define PROTO_ROBOT_DROP_RESP					6503	// 机器人删除应答


//打鱼服务器 
#define USER_INFO_NOTIFY 7000 //用户数据通知
#define HIT_FISH_NOTIFY	 7001 //打鱼命中通知

/*
 *====================================================================================
 * 系统服务器ID预定义
 *====================================================================================
 */
#define PROXY_BASE					1000
#define GAMESVR_BASE				2000
#define WEBPROXY_BASE				3000
#define UPDSVR_BASE					4000

#define AUTHSVR_ID					5001
#define BGSVR_ID					5011
#define RECHARGESVR_ID				5021

#define SESSION_ID_BASE				10000

#define SESSION_ID_AUTHSVR_BASE		18000

#define USER_EXTEND_INFO "user_extend#"
#define USER_ACCOUNT_TYPE "account_type#%u"
#define USER_AGE "crazyfish#age#%u"  
#endif // _QW_CRAZYFISH_PROTOCOL_DEFINE_H_


