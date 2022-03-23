#ifndef _FishStructs_H_
#define _FishStructs_H_
#include <stdint.h>
#include <string.h>
#include <vector>

using namespace std;
#include "protoDef.h"
#include "protoBase.h"
//#include "protoAutorize.h"

/* 
const uint32_t ECHO = 1;	//test echo
const uint32_t CLIENTCLOSEREQUEST = 1002;	//ClientCloseRequest
const uint32_t PROXYNOTIFYHEARTREQUEST = 1010;	//proxynotify与proxy心跳请求
const uint32_t GAMESVRSTOPREQUEST = 1011;	//游戏服务器停止请求
const uint32_t CONNECT_REGISTER_REQUEST = 1012;	//连接服务器注册请求
const uint32_t USER_ENTER_HALL_REQUEST = 1013;	//玩家进入大厅，统计用
const uint32_t RECHARGE_NOTIFY_REQUEST = 1014;	//充值通知
const uint32_t CLOSE_CLIENT_ERROR_RESPONE = 9999;	//关闭客户端错误应答
const uint32_t LOGIN_REQUEST = 10000;	//登录请求
const uint32_t ROOMLISTREQUEST = 10001;	//RoomListRequest
const uint32_t INTOGAMEREQUEST = 10002;	//IntoGameRequest
const uint32_t OUTGAMEREQUEST = 10003;	//OutGameRequest
const uint32_t HITFISHREQUEST = 10005;	//HitFishRequest
const uint32_t CHANGEBULLETREQUEST = 10007;	//ChangeBulletRequest
const uint32_t CHANGEGUNREQUEST = 10008;	//ChangeGunRequest
const uint32_t SHOOTREQUEST = 10009;	//ShootRequest
const uint32_t SEVENGIFTREQUEST = 10012;	//SevenGiftRequest
const uint32_t GETSEVENGIFTRESPONE = 10013;	//GetSevenGiftRespone
const uint32_t ONLINETIMEREQUEST = 10014;	//OnLineTimeRequest
const uint32_t GETONLINEGIFTREQUEST = 10015;	//GetOnLineGiftRequest
const uint32_t SENDMSGREQUEST = 10017;	//SendMsgRequest
const uint32_t UPDATEACCTINFOREQUEST = 10020;	//修改账号信息请求
const uint32_t GETPLAYERPICREQUEST = 10021;	//获取头像名称请求
const uint32_t SENDPUBLICMSGREQUEST = 10023;	//公聊发送消息请求
const uint32_t FASTINTOROOMREQUEST = 10024;	//快速进入游戏请求
const uint32_t GETFREECHATCOUNTREQUEST = 10025;	//获取免费聊天次数请求
const uint32_t GETNICKIFUPDATEREQUEST = 10027;	//获取昵称是否修改请求
const uint32_t DATARELOADREQUEST = 10028;	//数据重新装载请求
const uint32_t DESIGNROOMINFOREQUEST = 10033;	//指定房间信息请求
const uint32_t BILLCOMPOUNDREQUEST = 10034;	//合成话费卡请求
const uint32_t BILLPIECECOUNTREQUEST = 10035;	//获得话费卡碎片请求
const uint32_t PVPROOMSTATUSREQUEST = 10036;	//获取玩家闯关状态请求
const uint32_t PVPROOMGETREWARDREQUEST = 10037;	//玩家获取奖励请求
const uint32_t PVPROOMOUTRESPONE = 10038;	//PVP房玩家退出应答
const uint32_t PVPROOMWAITSTOPREQUEST = 10040;	//PVP房玩家等待时终止请求
const uint32_t BUY_ITEM_REQUEST = 10050;	//购买道具请求
const uint32_t SHOP_LIST_REQUEST = 10051;	//获取商城列表请求
const uint32_t ADD_FRIEND_REQUEST = 10052;	//增加好友请求
const uint32_t DELETE_FRIEND_REQUEST = 10053;	//删除好友请求
const uint32_t GIVE_COIN_REQUEST = 10054;	//好友送金请求
const uint32_t CREATE_ROBOT_REQUEST = 10055;	//创建机器人请求
const uint32_t DELETE_ROBOT_REQUEST = 10056;	//删除机器人请求
const uint32_t QUERY_TASK_INFO_REQUEST = 10057;	//任务信息查询请求
const uint32_t ACCEPT_TASK_REQUEST = 10058;	//接受任务请求
const uint32_t COMPLETE_TASK_REQUEST = 10059;	//完成任务请求
const uint32_t ACCESS_BANK_COIN_REQUEST = 10061;	//存取银行金币请求
const uint32_t GIVE_BANK_COIN_REQUEST = 10062;	//赠送银行金币请求
const uint32_t MODIFY_BANK_PASSWD_REQUEST = 10063;	//修改银行密码请求
const uint32_t RECHARGE_REQUEST = 10064;	//充值请求
const uint32_t RECEIVE_VIP_REWARD_REQUEST = 10065;	//接收vip礼包请求
const uint32_t QUERY_VIP_REWARD_REQUEST = 10066;	//查询vip礼包请求
const uint32_t NEWBIE_COMPLETE_REQUEST = 10068;	//新手引导完成请求
const uint32_t RECHARGE_CHECK_REQUEST = 10069;	//充值校验请求
const uint32_t BUY_OBJECT_ITEM_REQUEST = 10070;	//购买实物道具请求
const uint32_t VERIFY_CODE_REQUEST = 10071;	//获取验证码请求
const uint32_t ALL_RANK_REQUEST = 10072;	//所有玩家排名请求
const uint32_t USER_RANK_REQUEST = 10073;	//玩家排名请求
const uint32_t READ_MAIL_REQUEST = 10075;	//读取邮件请求
const uint32_t MAIL_LIST_REQUEST = 10076;	//邮件列表请求
const uint32_t USE_TEL_FARE_ITEM_REQUEST = 10077;	//使用话费道具请求
const uint32_t USE_OBJECT_ITEM_REQUEST = 10078;	//使用实物道具请求
const uint32_t ORDER_INFO_REQUEST = 10079;	//订单信息请求
const uint32_t OBJECT_SHOP_LIST_REQUEST = 10080;	//获取实物商城列表请求
const uint32_t COIN_RANK_REQUEST = 10081;	//金币排名请求
const uint32_t TICKET_RANK_REQUEST = 10082;	//奖券排名请求
const uint32_t QUERY_EVENT_TASK_INFO_REQUEST = 10085;	//查询比赛活动任务信息请求
const uint32_t COMPLETE_EVENT_TASK_REQUEST = 10086;	//完成比赛活动任务请求
const uint32_t UPD_UPLOAD_REQUEST = 10100;	//上传更新包请求
const uint32_t UPD_UPDATELIST_REQUEST = 10101;	//请求更新列表
const uint32_t CLIENTRECHARGEREQUEST = 10200;	//客户端充值请求
const uint32_t RECHGEIDFYCODEREQUEST = 10201;	//RechgeIdfyCodeRequest
const uint32_t RECHGEMESSAGEREQUEST = 10202;	//RechgeMessageRequest
const uint32_t PAYRESULTREQUEST = 10203;	//PayResultRequest
const uint32_t PINGCLIENTRECHARGEREQUEST = 11000;	//Ping客户端充值请求
const uint32_t PINGPAYRESULTREQUEST = 11001;	//Ping支付结果请求
const uint32_t BAIDUCLIENTLOGININREQUEST = 11010;	//百度客户端登入成功请求
const uint32_t BAIDUPAYRESULTREQUEST = 11011;	//百度支付结果请求
const uint32_t BAIDUMOBGAMEPAYRESULTREQUEST = 11013;	//百度移动游戏支付结果请求
const uint32_t APPLECLIENTRECHARGEREQUEST = 11020;	//Apple充值请求
const uint32_t APPLEPAYRESULTREQUEST = 11021;	//Apple充值应答校验
const uint32_t LOGIN_RESPONE = 110000;	//登录应答
const uint32_t ROOMLISTRESPONE = 110001;	//RoomListRespone
const uint32_t INTOGAMERESPONE = 110002;	//IntoGameRespone
const uint32_t OUTGAMERESPONE = 110003;	//OutGameRespone
const uint32_t SCENERESPONE = 110004;	//SceneRespone
const uint32_t HITFISHRESPONE = 110005;	//HitFishRespone
const uint32_t PLAYERJOINRESPONE = 110006;	//PlayerJoinRespone
const uint32_t PLAYERINFOCHANGERESPONE = 110007;	//PlayerInfoChangeRespone
const uint32_t SHOOTRESPONE = 110009;	//ShootRespone
const uint32_t SCENESTATERESPONE = 110010;	//SceneStateRespone
const uint32_t ACCOUNTRESPONE = 110011;	//AccountRespone
const uint32_t SEVENGIFTRESPONE = 110012;	//SevenGiftRespone
const uint32_t ONLINETIMERESPONE = 110014;	//OnLineTimeRespone
const uint32_t SENDMSGRESPONE = 110017;	//SendMsgRespone
const uint32_t RECVMSGRESPONE = 110018;	//RecvMsgRespone
const uint32_t GIVEONLINEGIFTRESPONE = 110019;	//GiveOnLineGiftRespone
const uint32_t UPDATEACCTINFORESPONE = 110020;	//修改账号信息应答
const uint32_t GETPLAYERPICRESPONE = 110021;	//获取头像名称应答
const uint32_t SENDMSGTOCLIENTRESPONE = 110022;	//世界聊天应答
const uint32_t SENDPUBLICMSGRESPONE = 110023;	//公聊发送消息应答
const uint32_t GETFREECHATCOUNTRESPONE = 110025;	//获取免费聊天次数应答
const uint32_t OUTCOMPETRESPONE = 110026;	//比赛房退出应答
const uint32_t GETNICKIFUPDATERESPONE = 110027;	//获取昵称是否修改应答
const uint32_t SYSTEMBROADCASTRESPONE = 110029;	//发送系统广播应答
const uint32_t HITFISHMISSRESPONE = 110030;	//捕鱼未命中应答
const uint32_t SHOOTSELFRESPONE = 110031;	//发射子弹应答给玩家本身
const uint32_t COMPETBROADCASTRESPONE = 110032;	//比赛广播应答
const uint32_t DESIGNROOMINFORESPONE = 110033;	//指定房间信息应答
const uint32_t BILLCOMPOUNDRESPONE = 110034;	//合成话费卡应答
const uint32_t BILLPIECECOUNTRESPONE = 110035;	//获取话费卡碎片应答
const uint32_t PVPROOMSTATUSRESPONE = 110036;	//获取玩家闯关状态应答
const uint32_t PVPROOMGETREWARDRESPONE = 110037;	//玩家获取奖励应答
const uint32_t NOTICERESPONE = 110039;	//公告应答
const uint32_t PVPROOMWAITSTOPRESPONE = 110040;	//PVP房玩家等待时终止应答
const uint32_t BUY_ITEM_RESPONE = 110050;	//购买道具应答
const uint32_t SHOP_LIST_RESPONE = 110051;	//获取商城列表应答
const uint32_t ADD_FRIEND_RESPONE = 110052;	//增加好友应答
const uint32_t DELETE_FRIEND_RESPONE = 110053;	//删除好友应答
const uint32_t GIVE_COIN_RESPONE = 110054;	//好友送金应答
const uint32_t CREATE_ROBOT_RESPONE = 110055;	//创建机器人应答
const uint32_t DELETE_ROBOT_RESPONE = 110056;	//删除机器人应答
const uint32_t TASK_INFO_RESPONE = 110060;	//任务信息应答
const uint32_t ACCESS_BANK_COIN_RESPONE = 110061;	//存取银行金币应答
const uint32_t MODIFY_BANK_PASSWD_RESPONE = 110063;	//修改银行密码应答
const uint32_t RECHARGE_RESPONE = 110064;	//充值应答
const uint32_t VIP_REWARD_RESPONE = 110067;	//vip礼包应答
const uint32_t NEWBIE_COMPLETE_RESPONE = 110068;	//新手引导完成应答
const uint32_t RECHARGE_CHECK_RESPONE = 110069;	//充值校验应答
const uint32_t BUY_OBJECT_ITEM_RESPONE = 110070;	//购买实物道具应答
const uint32_t VERIFY_CODE_RESPONE = 110071;	//获取验证码应答
const uint32_t ALL_RANK_RESPONE = 110072;	//所有玩家排名应答
const uint32_t USER_RANK_RESPONE = 110073;	//玩家排名应答
const uint32_t SEND_MAIL_RESPONE = 110074;	//发送邮件应答
const uint32_t READ_MAIL_RESPONE = 110075;	//读取邮件应答
const uint32_t MAIL_LIST_RESPONE = 110076;	//邮件列表应答
const uint32_t USE_TEL_FARE_ITEM_RESPONE = 110077;	//使用话费道具应答
const uint32_t USE_OBJECT_ITEM_RESPONE = 110078;	//使用实物道具应答
const uint32_t ORDER_INFO_RESPONE = 110079;	//订单信息应答
const uint32_t OBJECT_SHOP_LIST_RESPONE = 110080;	//获取实物商城列表应答
const uint32_t COIN_RANK_RESPONE = 110081;	//金币排名应答
const uint32_t TICKET_RANK_RESPONE = 110082;	//奖券排名应答
const uint32_t USER_ITEM_RESPONE = 110083;	//用户道具应答
const uint32_t USER_CURRENCY_RESPONE = 110084;	//用户货币信息应答
const uint32_t QUERY_EVENT_TASK_INFO_RESPONE = 110085;	//查询比赛活动任务信息应答
const uint32_t COMPLETE_EVENT_TASK_RESPONE = 110086;	//完成比赛活动任务应答
const uint32_t EVENT_TASK_INFO_RESPONE = 110087;	//比赛活动任务信息应答
const uint32_t UPD_UPLOAD_RESPONSE = 110100;	//上传更新包应答
const uint32_t UPD_UPDATELIST_RESPONSE = 110101;	//更新列表应答
const uint32_t CLIENTRECHARGERESPONE = 110200;	//客户端充值应答
const uint32_t RECHGEIDFYCODERESPONE = 110201;	//RechgeIdfyCodeRespone
const uint32_t RECHGEMESSAGERESPONE = 110202;	//RechgeMessageRespone
const uint32_t PAYRESULTRESPONE = 110203;	//PayResultRespone
const uint32_t PINGCLIENTRECHARGERESPONE = 111000;	//Ping客户端充值应答
const uint32_t PINGPAYRESULTRESPONE = 111001;	//Ping支付结果应答
const uint32_t BAIDUCLIENTLOGININRESPONE = 111010;	//百度客户端登入成功应答
const uint32_t BAIDUPAYRESULTRESPONE = 111011;	//百度支付结果应答
const uint32_t BAIDUMOBGAMEPAYRESULTRESPONE = 111013;	//百度移动游戏支付结果应答
const uint32_t APPLECLIENTRECHARGERESPONSE = 111020;	//Apple充值请求应答
const uint32_t APPLEPAYRESULTRESPONSE = 111021;	//Apple充值应答校验回复
const uint32_t ERROR_RESPONE = 190000;	//错误应答
*/

struct AreaInfo
{
	uint32_t game_id;
	char game_name[64];
	uint32_t area_id;
	char area_name[64];
	uint32_t area_flags;
	char area_note[256];
	size_t length()
	{
		return sizeof(uint32_t)+sizeof(uint16_t)+strlen(game_name)+sizeof(uint32_t)+sizeof(uint16_t)+strlen(area_name)+sizeof(uint32_t)+sizeof(uint16_t)+strlen(area_note);
	}
};

struct FriendInfo
{
	int32_t f_type;
	uint32_t f_uin;
	char f_name[256];
	int32_t today_gift;
	char f_icon[128];
	size_t length()
	{
		return sizeof(int32_t)+sizeof(uint32_t)+sizeof(uint16_t)+strlen(f_name)+sizeof(int32_t)+sizeof(uint16_t)+strlen(f_icon);
	}
};

struct PkgInfo
{
	uint32_t item_id;
	uint32_t item_typeid;
	uint32_t num;
	size_t length()
	{
		return sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t);
	}
};

struct RechargeInfo
{
	uint32_t recharge_id;
	uint32_t buy_num;
	uint32_t limit_num;
	uint32_t buy_flag;
	size_t length()
	{
		return sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t);
	}
};

struct FishData
{
	uint32_t fishid;
	size_t length()
	{
		return sizeof(uint32_t);
	}
};

struct GameUserData
{
	uint32_t uin;
	char name[64];
	uint32_t seatid;
	uint32_t bullet;
	uint8_t gunid;
	uint32_t score;
	size_t length()
	{
		return sizeof(uint32_t)+sizeof(uint16_t)+strlen(name)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint8_t)+sizeof(uint32_t);
	}
};

struct MailInfotemp
{
	uint32_t mail_id;
	uint32_t sender_id;
	uint32_t recv_id;
	char sender_name[64];
	char title[64];
	char content[256];
	int32_t mail_type;
	uint32_t is_read;
	char send_time[16];
	size_t length()
	{
		return sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint16_t)+strlen(sender_name)+sizeof(uint16_t)+strlen(title)+sizeof(uint16_t)+strlen(content)+sizeof(int32_t)+sizeof(uint32_t)+sizeof(uint16_t)+strlen(send_time);
	}
};

struct NoticeData
{
	uint32_t level;
	char title[1024];
	char note[2048];
	char time[20];
	size_t length()
	{
		return sizeof(uint32_t)+sizeof(uint16_t)+strlen(title)+sizeof(uint16_t)+strlen(note)+sizeof(uint16_t)+strlen(time);
	}
};

struct PkgVersionList
{
	uint32_t version;
	uint32_t vertype;
	uint32_t fsize;
	char fname[128];
	char fmd5[32];
	char furl[256];
	size_t length()
	{
		return sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint16_t)+strlen(fname)+sizeof(uint16_t)+strlen(fmd5)+sizeof(uint16_t)+strlen(furl);
	}
};

struct RankInfo
{
	uint32_t uin;
	uint32_t rank;
	char nick[128];
	uint32_t score;
	char begin_time[16];
	size_t length()
	{
		return sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint16_t)+strlen(nick)+sizeof(uint32_t)+sizeof(uint16_t)+strlen(begin_time);
	}
};

struct RoomListData
{
	uint32_t room_id;
	char room_name[64];
	uint8_t room_flag;
	uint32_t online_numbers;
	uint32_t mincoin;
	char gunlist[64];
	uint32_t chage_count;
	uint32_t maxchage_count;
	uint32_t openflag;
	uint32_t lefttime;
	char endtime[20];
	char nexttime[20];
	uint32_t staytime;
	uint32_t cur_cost;
	uint32_t cur_score;
	uint32_t user_score;
	uint32_t user_ranking;
	size_t length()
	{
		return sizeof(uint32_t)+sizeof(uint16_t)+strlen(room_name)+sizeof(uint8_t)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint16_t)+strlen(gunlist)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint16_t)+strlen(endtime)+sizeof(uint16_t)+strlen(nexttime)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint32_t);
	}
};

struct broadcastInfo
{
	uint32_t type;
	char data[256];
	size_t length()
	{
		return sizeof(uint32_t)+sizeof(uint16_t)+strlen(data);
	}
};

class MsgHead
{
public:
	uint32_t data_len;
	uint32_t protocol_id;
	uint64_t check_data;

public:
	MsgHead();

	virtual ~MsgHead();

	MsgHead(const MsgHead &obj);

	MsgHead &operator =(const MsgHead &obj);

	const char *DataToBytes();

	void BytesToData(char *bytes, int left_len);

	size_t DataLength();

protected:
	virtual size_t HeadLength();

	virtual void HeadToBytes();

	virtual void BytesToHead();

	virtual size_t BodyLength()
	{
		return 0;
	}
	virtual void BodyToBytes()
	{
	}
	virtual void BytesToBody()
	{
	}

	void ToBytes(void *src,size_t count);

	void ToBytes(char *src);

	void ToData(void *dest,size_t count);

	void ToData(char *dest,uint16_t max_len);

private:
	char *m_buf;
	int32_t m_left_len;
};

class RecordInfo : public MsgHead
{
public:
	uint64_t	uin;			//?ͻ???ΨһID
	uint32_t	clientId;		//?ͻ????׽???ID,??proxy??
	uint32_t	cmdId;		//ͨѶЭ??
	uint32_t	flag;		//??¼??ʶ
	uint32_t	mask;		//Ԥ??
	uint64_t	clientIp;		//?ͻ???IP

public:
	RecordInfo();

	~RecordInfo();

	RecordInfo(const RecordInfo &obj);

	RecordInfo &operator =(const RecordInfo &obj);

protected:
	size_t HeadLength();

	void HeadToBytes();

	void BytesToHead();

	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class AccessBankCoinRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t coin_num;
	uint32_t flag;
	char passwd[128];

public:
	AccessBankCoinRequest();

	~AccessBankCoinRequest();

	AccessBankCoinRequest(const AccessBankCoinRequest &obj); 

	AccessBankCoinRequest &operator =(const AccessBankCoinRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class AccessBankCoinRespone : public MsgHead
{
public:
	uint32_t uin;
	uint32_t coin;
	uint32_t bank_coin;

public:
	AccessBankCoinRespone();

	~AccessBankCoinRespone();

	AccessBankCoinRespone(const AccessBankCoinRespone &obj); 

	AccessBankCoinRespone &operator =(const AccessBankCoinRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class AccountALMSInfoRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	AccountALMSInfoRequest();

	~AccountALMSInfoRequest();

	AccountALMSInfoRequest(const AccountALMSInfoRequest &obj); 

	AccountALMSInfoRequest &operator =(const AccountALMSInfoRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class AccountALMSInfoRespone : public MsgHead
{
public:
	uint32_t alms;
	uint32_t money;

public:
	AccountALMSInfoRespone();

	~AccountALMSInfoRespone();

	AccountALMSInfoRespone(const AccountALMSInfoRespone &obj); 

	AccountALMSInfoRespone &operator =(const AccountALMSInfoRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class AccountALMSRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	AccountALMSRequest();

	~AccountALMSRequest();

	AccountALMSRequest(const AccountALMSRequest &obj); 

	AccountALMSRequest &operator =(const AccountALMSRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class AccountALMSRespone : public MsgHead
{
public:
	uint32_t alms;
	uint32_t money;

public:
	AccountALMSRespone();

	~AccountALMSRespone();

	AccountALMSRespone(const AccountALMSRespone &obj); 

	AccountALMSRespone &operator =(const AccountALMSRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class AccountRespone : public MsgHead
{
public:
	uint32_t uin;
	uint32_t type;
	uint32_t number;

public:
	AccountRespone();

	~AccountRespone();

	AccountRespone(const AccountRespone &obj); 

	AccountRespone &operator =(const AccountRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class AddFriendRequest : public MsgHead
{
public:
	uint32_t uin;
	int32_t f_type;
	uint32_t f_uin;
	char f_name[256];

public:
	AddFriendRequest();

	~AddFriendRequest();

	AddFriendRequest(const AddFriendRequest &obj); 

	AddFriendRequest &operator =(const AddFriendRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class AddFriendRespone : public MsgHead
{
public:
	vector<FriendInfo> friend_list;

public:
	AddFriendRespone();

	~AddFriendRespone();

	AddFriendRespone(const AddFriendRespone &obj); 

	AddFriendRespone &operator =(const AddFriendRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class AllRankRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t room_id;

public:
	AllRankRequest();

	~AllRankRequest();

	AllRankRequest(const AllRankRequest &obj); 

	AllRankRequest &operator =(const AllRankRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class AllRankRespone : public MsgHead
{
public:
	vector<RankInfo> rank_list;

public:
	AllRankRespone();

	~AllRankRespone();

	AllRankRespone(const AllRankRespone &obj); 

	AllRankRespone &operator =(const AllRankRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class BillCompoundRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t itemid;

public:
	BillCompoundRequest();

	~BillCompoundRequest();

	BillCompoundRequest(const BillCompoundRequest &obj); 

	BillCompoundRequest &operator =(const BillCompoundRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class BillCompoundRespone : public MsgHead
{
public:
	uint32_t flag;
	uint32_t number;

public:
	BillCompoundRespone();

	~BillCompoundRespone();

	BillCompoundRespone(const BillCompoundRespone &obj); 

	BillCompoundRespone &operator =(const BillCompoundRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class BillPieceCountRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	BillPieceCountRequest();

	~BillPieceCountRequest();

	BillPieceCountRequest(const BillPieceCountRequest &obj); 

	BillPieceCountRequest &operator =(const BillPieceCountRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class BillPieceCountRespone : public MsgHead
{
public:
	uint32_t number;

public:
	BillPieceCountRespone();

	~BillPieceCountRespone();

	BillPieceCountRespone(const BillPieceCountRespone &obj); 

	BillPieceCountRespone &operator =(const BillPieceCountRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class BuyItemRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t item_typeid;

public:
	BuyItemRequest();

	~BuyItemRequest();

	BuyItemRequest(const BuyItemRequest &obj); 

	BuyItemRequest &operator =(const BuyItemRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class BuyItemRespone : public MsgHead
{
public:
	uint32_t uin;
	uint32_t item_typeid;

public:
	BuyItemRespone();

	~BuyItemRespone();

	BuyItemRespone(const BuyItemRespone &obj); 

	BuyItemRespone &operator =(const BuyItemRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class BuyObjectItemRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t item_typeid;

public:
	BuyObjectItemRequest();

	~BuyObjectItemRequest();

	BuyObjectItemRequest(const BuyObjectItemRequest &obj); 

	BuyObjectItemRequest &operator =(const BuyObjectItemRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class BuyObjectItemRespone : public MsgHead
{
public:
	uint32_t uin;
	uint32_t item_typeid;

public:
	BuyObjectItemRespone();

	~BuyObjectItemRespone();

	BuyObjectItemRespone(const BuyObjectItemRespone &obj); 

	BuyObjectItemRespone &operator =(const BuyObjectItemRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ChangeBulletRequest : public MsgHead
{
public:
	uint32_t uin;
	int16_t action;
	uint32_t robotflag;

public:
	ChangeBulletRequest();

	~ChangeBulletRequest();

	ChangeBulletRequest(const ChangeBulletRequest &obj); 

	ChangeBulletRequest &operator =(const ChangeBulletRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ChangeGunRequest : public MsgHead
{
public:
	uint32_t uin;
	int16_t id;

public:
	ChangeGunRequest();

	~ChangeGunRequest();

	ChangeGunRequest(const ChangeGunRequest &obj); 

	ChangeGunRequest &operator =(const ChangeGunRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ClientCloseRequest : public MsgHead
{
public:
	uint32_t reason;

public:
	ClientCloseRequest();

	~ClientCloseRequest();

	ClientCloseRequest(const ClientCloseRequest &obj); 

	ClientCloseRequest &operator =(const ClientCloseRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ClientRechargeRequest : public MsgHead
{
public:
	uint32_t mz;
	char mob[20];
	char uid[20];
	char imsi[20];
	char imei[20];
	char ip[20];

public:
	ClientRechargeRequest();

	~ClientRechargeRequest();

	ClientRechargeRequest(const ClientRechargeRequest &obj); 

	ClientRechargeRequest &operator =(const ClientRechargeRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ClientRechargeRespone : public MsgHead
{
public:
	uint32_t result;
	char data[128];
	uint32_t type;
	char editdata[128];
	char sernumber[20];
	char idfyflag[20];
	uint32_t opcode;

public:
	ClientRechargeRespone();

	~ClientRechargeRespone();

	ClientRechargeRespone(const ClientRechargeRespone &obj); 

	ClientRechargeRespone &operator =(const ClientRechargeRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class CloseClientErrorRespone : public MsgHead
{
public:
	int32_t error_code;

public:
	CloseClientErrorRespone();

	~CloseClientErrorRespone();

	CloseClientErrorRespone(const CloseClientErrorRespone &obj); 

	CloseClientErrorRespone &operator =(const CloseClientErrorRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class CoinRankRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	CoinRankRequest();

	~CoinRankRequest();

	CoinRankRequest(const CoinRankRequest &obj); 

	CoinRankRequest &operator =(const CoinRankRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class CoinRankRespone : public MsgHead
{
public:
	vector<RankInfo> rank_list;
	uint32_t rank;

public:
	CoinRankRespone();

	~CoinRankRespone();

	CoinRankRespone(const CoinRankRespone &obj); 

	CoinRankRespone &operator =(const CoinRankRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class CompetBroadcastRespone : public MsgHead
{
public:
	char data[1024];
	uint32_t roomid;

public:
	CompetBroadcastRespone();

	~CompetBroadcastRespone();

	CompetBroadcastRespone(const CompetBroadcastRespone &obj); 

	CompetBroadcastRespone &operator =(const CompetBroadcastRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ConnectRegisterRequest : public MsgHead
{
public:
	char program_name[64];
	uint32_t server_id;

public:
	ConnectRegisterRequest();

	~ConnectRegisterRequest();

	ConnectRegisterRequest(const ConnectRegisterRequest &obj); 

	ConnectRegisterRequest &operator =(const ConnectRegisterRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class CreateRobotRequest : public MsgHead
{
public:
	uint32_t room_id;

public:
	CreateRobotRequest();

	~CreateRobotRequest();

	CreateRobotRequest(const CreateRobotRequest &obj); 

	CreateRobotRequest &operator =(const CreateRobotRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class CreateRobotRespone : public MsgHead
{
public:
	uint32_t uin;

public:
	CreateRobotRespone();

	~CreateRobotRespone();

	CreateRobotRespone(const CreateRobotRespone &obj); 

	CreateRobotRespone &operator =(const CreateRobotRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class DataReloadRequest : public MsgHead
{
public:
	char dictname[1024];

public:
	DataReloadRequest();

	~DataReloadRequest();

	DataReloadRequest(const DataReloadRequest &obj); 

	DataReloadRequest &operator =(const DataReloadRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class DeleteFriendRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t f_uin;

public:
	DeleteFriendRequest();

	~DeleteFriendRequest();

	DeleteFriendRequest(const DeleteFriendRequest &obj); 

	DeleteFriendRequest &operator =(const DeleteFriendRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class DeleteFriendRespone : public MsgHead
{
public:
	vector<FriendInfo> friend_list;

public:
	DeleteFriendRespone();

	~DeleteFriendRespone();

	DeleteFriendRespone(const DeleteFriendRespone &obj); 

	DeleteFriendRespone &operator =(const DeleteFriendRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class DeleteRobotRequest : public MsgHead
{
public:
	uint32_t room_id;

public:
	DeleteRobotRequest();

	~DeleteRobotRequest();

	DeleteRobotRequest(const DeleteRobotRequest &obj); 

	DeleteRobotRequest &operator =(const DeleteRobotRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class DeleteRobotRespone : public MsgHead
{
public:
	uint32_t uin;

public:
	DeleteRobotRespone();

	~DeleteRobotRespone();

	DeleteRobotRespone(const DeleteRobotRespone &obj); 

	DeleteRobotRespone &operator =(const DeleteRobotRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class DesignRoomInfoRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t room_id;

public:
	DesignRoomInfoRequest();

	~DesignRoomInfoRequest();

	DesignRoomInfoRequest(const DesignRoomInfoRequest &obj); 

	DesignRoomInfoRequest &operator =(const DesignRoomInfoRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class DesignRoomInfoRespone : public MsgHead
{
public:
	uint32_t room_id;
	char room_name[64];
	uint8_t room_flag;
	uint32_t online_numbers;
	uint32_t mincoin;
	char gunlist[64];
	uint32_t chage_count;
	uint32_t maxchage_count;
	uint32_t openflag;
	uint32_t lefttime;
	char endtime[20];
	char nexttime[20];
	uint32_t staytime;
	uint32_t cur_cost;
	uint32_t cur_score;
	uint32_t user_score;
	uint32_t user_ranking;

public:
	DesignRoomInfoRespone();

	~DesignRoomInfoRespone();

	DesignRoomInfoRespone(const DesignRoomInfoRespone &obj); 

	DesignRoomInfoRespone &operator =(const DesignRoomInfoRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class Echo : public MsgHead
{
public:
	char test[64];

public:
	Echo();

	~Echo();

	Echo(const Echo &obj); 

	Echo &operator =(const Echo &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};


class EncryptCodeRespone : public MsgHead
{
public:
	uint32_t code;
	uint32_t reverse1;
	uint32_t reverse2;
	uint32_t reverse3;
	uint32_t reverse4;
	uint32_t reverse5;
	uint32_t reverse6;
	uint32_t reverse7;
	uint32_t reverse8;
	uint32_t reverse9;

public:
	EncryptCodeRespone();

	~EncryptCodeRespone();

	EncryptCodeRespone(const EncryptCodeRespone &obj); 

	EncryptCodeRespone &operator =(const EncryptCodeRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};


class ErrorRespone : public MsgHead
{
public:
	int32_t error_code;

public:
	ErrorRespone();

	~ErrorRespone();

	ErrorRespone(const ErrorRespone &obj); 

	ErrorRespone &operator =(const ErrorRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class FastIntoRoomRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t game_typeid;

public:
	FastIntoRoomRequest();

	~FastIntoRoomRequest();

	FastIntoRoomRequest(const FastIntoRoomRequest &obj); 

	FastIntoRoomRequest &operator =(const FastIntoRoomRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GameSvrStopRequest : public MsgHead
{
public:
	uint32_t error_code;

public:
	GameSvrStopRequest();

	~GameSvrStopRequest();

	GameSvrStopRequest(const GameSvrStopRequest &obj); 

	GameSvrStopRequest &operator =(const GameSvrStopRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GetFreeChatCountRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	GetFreeChatCountRequest();

	~GetFreeChatCountRequest();

	GetFreeChatCountRequest(const GetFreeChatCountRequest &obj); 

	GetFreeChatCountRequest &operator =(const GetFreeChatCountRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GetFreeChatCountRespone : public MsgHead
{
public:
	uint32_t number;

public:
	GetFreeChatCountRespone();

	~GetFreeChatCountRespone();

	GetFreeChatCountRespone(const GetFreeChatCountRespone &obj); 

	GetFreeChatCountRespone &operator =(const GetFreeChatCountRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GetNickIfUpdateRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	GetNickIfUpdateRequest();

	~GetNickIfUpdateRequest();

	GetNickIfUpdateRequest(const GetNickIfUpdateRequest &obj); 

	GetNickIfUpdateRequest &operator =(const GetNickIfUpdateRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GetNickIfUpdateRespone : public MsgHead
{
public:
	uint32_t number;

public:
	GetNickIfUpdateRespone();

	~GetNickIfUpdateRespone();

	GetNickIfUpdateRespone(const GetNickIfUpdateRespone &obj); 

	GetNickIfUpdateRespone &operator =(const GetNickIfUpdateRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GetOnLineGiftRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t id;

public:
	GetOnLineGiftRequest();

	~GetOnLineGiftRequest();

	GetOnLineGiftRequest(const GetOnLineGiftRequest &obj); 

	GetOnLineGiftRequest &operator =(const GetOnLineGiftRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GetPlayerPicRequest : public MsgHead
{
public:
	uint32_t type;
	char data[128];

public:
	GetPlayerPicRequest();

	~GetPlayerPicRequest();

	GetPlayerPicRequest(const GetPlayerPicRequest &obj); 

	GetPlayerPicRequest &operator =(const GetPlayerPicRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GetPlayerPicRespone : public MsgHead
{
public:
	uint32_t flag;
	char data[128];

public:
	GetPlayerPicRespone();

	~GetPlayerPicRespone();

	GetPlayerPicRespone(const GetPlayerPicRespone &obj); 

	GetPlayerPicRespone &operator =(const GetPlayerPicRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GetSevenGiftRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	GetSevenGiftRequest();

	~GetSevenGiftRequest();

	GetSevenGiftRequest(const GetSevenGiftRequest &obj); 

	GetSevenGiftRequest &operator =(const GetSevenGiftRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GiveBankCoinRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t coin_num;
	char target_name[128];
	char passwd[128];

public:
	GiveBankCoinRequest();

	~GiveBankCoinRequest();

	GiveBankCoinRequest(const GiveBankCoinRequest &obj); 

	GiveBankCoinRequest &operator =(const GiveBankCoinRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GiveCoinRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t f_uin;

public:
	GiveCoinRequest();

	~GiveCoinRequest();

	GiveCoinRequest(const GiveCoinRequest &obj); 

	GiveCoinRequest &operator =(const GiveCoinRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GiveCoinRespone : public MsgHead
{
public:
	uint32_t coin;

public:
	GiveCoinRespone();

	~GiveCoinRespone();

	GiveCoinRespone(const GiveCoinRespone &obj); 

	GiveCoinRespone &operator =(const GiveCoinRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class GiveOnLineGiftRespone : public MsgHead
{
public:
	uint32_t id;

public:
	GiveOnLineGiftRespone();

	~GiveOnLineGiftRespone();

	GiveOnLineGiftRespone(const GiveOnLineGiftRespone &obj); 

	GiveOnLineGiftRespone &operator =(const GiveOnLineGiftRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class HitFishMissRespone : public MsgHead
{
public:
	uint32_t flag;

public:
	HitFishMissRespone();

	~HitFishMissRespone();

	HitFishMissRespone(const HitFishMissRespone &obj); 

	HitFishMissRespone &operator =(const HitFishMissRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class HitFishRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t order;
	vector<FishData> fishdata;
	uint32_t fishtypedata;
	vector<FishData> fishtypeid;
	uint32_t fishscreendata;
	uint32_t fishbilldata;
	int32_t is_robot;

public:
	HitFishRequest();

	~HitFishRequest();

	HitFishRequest(const HitFishRequest &obj); 

	HitFishRequest &operator =(const HitFishRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class HitFishRespone : public MsgHead
{
public:
	uint32_t uin;
	uint32_t score;
	uint32_t gunid;
	vector<FishData> fishdata;

public:
	HitFishRespone();

	~HitFishRespone();

	HitFishRespone(const HitFishRespone &obj); 

	HitFishRespone &operator =(const HitFishRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class IntoGameRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t room_id;
	int32_t is_robot;

public:
	IntoGameRequest();

	~IntoGameRequest();

	IntoGameRequest(const IntoGameRequest &obj); 

	IntoGameRequest &operator =(const IntoGameRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class IntoGameRespone : public MsgHead
{
public:
	int16_t intoroom_flag;
	uint32_t room_id;
	vector<GameUserData> userdata;
	uint32_t maxscore;
	uint32_t lefttime;

public:
	IntoGameRespone();

	~IntoGameRespone();

	IntoGameRespone(const IntoGameRespone &obj); 

	IntoGameRespone &operator =(const IntoGameRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

#if 0
class LoginRequest : public MsgHead
{
public:
	char device_id[64];
	char account[256];
	char passwd[128];
	uint32_t new_flag;
	uint32_t source;
	uint32_t chg_account_flag;
	uint32_t version;
	char channel[32];

public:
	LoginRequest();

	~LoginRequest();

	LoginRequest(const LoginRequest &obj); 

	LoginRequest &operator =(const LoginRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class LoginRespone : public MsgHead
{
public:
	char device_id[64];
	char account[256];
	uint32_t uin;
	uint32_t source;
	char nick[128];
	uint32_t vip;
	char mail[64];
	char mobile[64];
	uint32_t money;
	uint32_t coin;
	uint32_t ticket;
	uint32_t pkgsize;
	uint32_t signin;
	uint32_t bank_coin;
	uint32_t total_recharge;
	uint32_t newbie_flag;
	uint32_t trumpet;
	vector<AreaInfo> area_list;
	vector<FriendInfo> friend_list;
	vector<PkgInfo> pkg_list;
	vector<RechargeInfo> recharge_list;

public:
	LoginRespone();

	~LoginRespone();

	LoginRespone(const LoginRespone &obj); 

	LoginRespone &operator =(const LoginRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};
#endif

class MailListRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	MailListRequest();

	~MailListRequest();

	MailListRequest(const MailListRequest &obj); 

	MailListRequest &operator =(const MailListRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class MailListRespone : public MsgHead
{
public:
	vector<MailInfotemp> mail_list;

public:
	MailListRespone();

	~MailListRespone();

	MailListRespone(const MailListRespone &obj); 

	MailListRespone &operator =(const MailListRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ModifyBankPassWdRequest : public MsgHead
{
public:
	uint32_t uin;
	char old_passwd[128];
	char new_passwd[128];

public:
	ModifyBankPassWdRequest();

	~ModifyBankPassWdRequest();

	ModifyBankPassWdRequest(const ModifyBankPassWdRequest &obj); 

	ModifyBankPassWdRequest &operator =(const ModifyBankPassWdRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ModifyBankPassWdRespone : public MsgHead
{
public:
	uint32_t uin;

public:
	ModifyBankPassWdRespone();

	~ModifyBankPassWdRespone();

	ModifyBankPassWdRespone(const ModifyBankPassWdRespone &obj); 

	ModifyBankPassWdRespone &operator =(const ModifyBankPassWdRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class NewbieCompleteRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	NewbieCompleteRequest();

	~NewbieCompleteRequest();

	NewbieCompleteRequest(const NewbieCompleteRequest &obj); 

	NewbieCompleteRequest &operator =(const NewbieCompleteRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class NewbieCompleteRespone : public MsgHead
{
public:
	uint32_t uin;

public:
	NewbieCompleteRespone();

	~NewbieCompleteRespone();

	NewbieCompleteRespone(const NewbieCompleteRespone &obj); 

	NewbieCompleteRespone &operator =(const NewbieCompleteRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class NoticeRespone : public MsgHead
{
public:
	vector<NoticeData> data;

public:
	NoticeRespone();

	~NoticeRespone();

	NoticeRespone(const NoticeRespone &obj); 

	NoticeRespone &operator =(const NoticeRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ObjOrderBroadcastRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	ObjOrderBroadcastRequest();

	~ObjOrderBroadcastRequest();

	ObjOrderBroadcastRequest(const ObjOrderBroadcastRequest &obj); 

	ObjOrderBroadcastRequest &operator =(const ObjOrderBroadcastRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ObjOrderBroadcastRespone : public MsgHead
{
public:
	vector<broadcastInfo> broadcastData;

public:
	ObjOrderBroadcastRespone();

	~ObjOrderBroadcastRespone();

	ObjOrderBroadcastRespone(const ObjOrderBroadcastRespone &obj); 

	ObjOrderBroadcastRespone &operator =(const ObjOrderBroadcastRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class OnLineTimeRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	OnLineTimeRequest();

	~OnLineTimeRequest();

	OnLineTimeRequest(const OnLineTimeRequest &obj); 

	OnLineTimeRequest &operator =(const OnLineTimeRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class OnLineTimeRespone : public MsgHead
{
public:
	uint32_t id;
	uint32_t online_time;

public:
	OnLineTimeRespone();

	~OnLineTimeRespone();

	OnLineTimeRespone(const OnLineTimeRespone &obj); 

	OnLineTimeRespone &operator =(const OnLineTimeRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class OutCompetRespone : public MsgHead
{
public:
	uint32_t score;
	uint32_t ranking;
	uint32_t lefttime;
	uint32_t cost;

public:
	OutCompetRespone();

	~OutCompetRespone();

	OutCompetRespone(const OutCompetRespone &obj); 

	OutCompetRespone &operator =(const OutCompetRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class OutGameRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	OutGameRequest();

	~OutGameRequest();

	OutGameRequest(const OutGameRequest &obj); 

	OutGameRequest &operator =(const OutGameRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class OutGameRespone : public MsgHead
{
public:
	uint32_t uin;

public:
	OutGameRespone();

	~OutGameRespone();

	OutGameRespone(const OutGameRespone &obj); 

	OutGameRespone &operator =(const OutGameRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class PlayerInfoChangeRespone : public MsgHead
{
public:
	uint32_t uin;
	int16_t flag;
	uint32_t score;
	uint32_t bullet;
	uint32_t gunid;

public:
	PlayerInfoChangeRespone();

	~PlayerInfoChangeRespone();

	PlayerInfoChangeRespone(const PlayerInfoChangeRespone &obj); 

	PlayerInfoChangeRespone &operator =(const PlayerInfoChangeRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class PlayerJoinRespone : public MsgHead
{
public:
	uint32_t uin;
	char name[64];
	uint32_t seatid;
	uint32_t bullet;
	int16_t gunid;
	uint32_t score;

public:
	PlayerJoinRespone();

	~PlayerJoinRespone();

	PlayerJoinRespone(const PlayerJoinRespone &obj); 

	PlayerJoinRespone &operator =(const PlayerJoinRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ProxyNotifyHeartRequest : public MsgHead
{
public:
	uint32_t error_code;

public:
	ProxyNotifyHeartRequest();

	~ProxyNotifyHeartRequest();

	ProxyNotifyHeartRequest(const ProxyNotifyHeartRequest &obj); 

	ProxyNotifyHeartRequest &operator =(const ProxyNotifyHeartRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class PvpRoomGetRewardRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	PvpRoomGetRewardRequest();

	~PvpRoomGetRewardRequest();

	PvpRoomGetRewardRequest(const PvpRoomGetRewardRequest &obj); 

	PvpRoomGetRewardRequest &operator =(const PvpRoomGetRewardRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class PvpRoomGetRewardRespone : public MsgHead
{
public:
	uint32_t flag;

public:
	PvpRoomGetRewardRespone();

	~PvpRoomGetRewardRespone();

	PvpRoomGetRewardRespone(const PvpRoomGetRewardRespone &obj); 

	PvpRoomGetRewardRespone &operator =(const PvpRoomGetRewardRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class PvpRoomOutRespone : public MsgHead
{
public:
	uint32_t flag;
	uint32_t self_score;
	uint32_t other_score;
	char other_name[64];

public:
	PvpRoomOutRespone();

	~PvpRoomOutRespone();

	PvpRoomOutRespone(const PvpRoomOutRespone &obj); 

	PvpRoomOutRespone &operator =(const PvpRoomOutRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class PvpRoomStatusRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	PvpRoomStatusRequest();

	~PvpRoomStatusRequest();

	PvpRoomStatusRequest(const PvpRoomStatusRequest &obj); 

	PvpRoomStatusRequest &operator =(const PvpRoomStatusRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class PvpRoomStatusRespone : public MsgHead
{
public:
	uint32_t status;

public:
	PvpRoomStatusRespone();

	~PvpRoomStatusRespone();

	PvpRoomStatusRespone(const PvpRoomStatusRespone &obj); 

	PvpRoomStatusRespone &operator =(const PvpRoomStatusRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class PvpRoomWaitStopRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	PvpRoomWaitStopRequest();

	~PvpRoomWaitStopRequest();

	PvpRoomWaitStopRequest(const PvpRoomWaitStopRequest &obj); 

	PvpRoomWaitStopRequest &operator =(const PvpRoomWaitStopRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class PvpRoomWaitStopRespone : public MsgHead
{
public:
	uint32_t flag;

public:
	PvpRoomWaitStopRespone();

	~PvpRoomWaitStopRespone();

	PvpRoomWaitStopRespone(const PvpRoomWaitStopRespone &obj); 

	PvpRoomWaitStopRespone &operator =(const PvpRoomWaitStopRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ReadMailRequest : public MsgHead
{
public:
	uint32_t mail_id;
	uint32_t uin;

public:
	ReadMailRequest();

	~ReadMailRequest();

	ReadMailRequest(const ReadMailRequest &obj); 

	ReadMailRequest &operator =(const ReadMailRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ReadMailRespone : public MsgHead
{
public:
	uint32_t mail_id;

public:
	ReadMailRespone();

	~ReadMailRespone();

	ReadMailRespone(const ReadMailRespone &obj); 

	ReadMailRespone &operator =(const ReadMailRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class RechargeCheckRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t money;

public:
	RechargeCheckRequest();

	~RechargeCheckRequest();

	RechargeCheckRequest(const RechargeCheckRequest &obj); 

	RechargeCheckRequest &operator =(const RechargeCheckRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class RechargeCheckRespone : public MsgHead
{
public:
	uint32_t uin;
	uint32_t money;

public:
	RechargeCheckRespone();

	~RechargeCheckRespone();

	RechargeCheckRespone(const RechargeCheckRespone &obj); 

	RechargeCheckRespone &operator =(const RechargeCheckRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class RechargeNotifyRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	RechargeNotifyRequest();

	~RechargeNotifyRequest();

	RechargeNotifyRequest(const RechargeNotifyRequest &obj); 

	RechargeNotifyRequest &operator =(const RechargeNotifyRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class RechargeRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t recharge_id;

public:
	RechargeRequest();

	~RechargeRequest();

	RechargeRequest(const RechargeRequest &obj); 

	RechargeRequest &operator =(const RechargeRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class RechargeRespone : public MsgHead
{
public:
	uint32_t uin;
	uint32_t total_recharge;
	uint32_t vip;
	uint32_t money;
	uint32_t coin;
	uint32_t ticket;
	uint32_t recharge_money;
	uint32_t recharge_rmb;

public:
	RechargeRespone();

	~RechargeRespone();

	RechargeRespone(const RechargeRespone &obj); 

	RechargeRespone &operator =(const RechargeRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class RechgeIdfyCodeRequest : public MsgHead
{
public:
	char orderid[20];
	char code[20];

public:
	RechgeIdfyCodeRequest();

	~RechgeIdfyCodeRequest();

	RechgeIdfyCodeRequest(const RechgeIdfyCodeRequest &obj); 

	RechgeIdfyCodeRequest &operator =(const RechgeIdfyCodeRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class RechgeIdfyCodeRespone : public MsgHead
{
public:
	uint32_t result;
	char data[128];

public:
	RechgeIdfyCodeRespone();

	~RechgeIdfyCodeRespone();

	RechgeIdfyCodeRespone(const RechgeIdfyCodeRespone &obj); 

	RechgeIdfyCodeRespone &operator =(const RechgeIdfyCodeRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class RechgeMessageRequest : public MsgHead
{
public:
	char orderid[20];

public:
	RechgeMessageRequest();

	~RechgeMessageRequest();

	RechgeMessageRequest(const RechgeMessageRequest &obj); 

	RechgeMessageRequest &operator =(const RechgeMessageRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class RechgeMessageRespone : public MsgHead
{
public:
	uint32_t result;
	char data[128];

public:
	RechgeMessageRespone();

	~RechgeMessageRespone();

	RechgeMessageRespone(const RechgeMessageRespone &obj); 

	RechgeMessageRespone &operator =(const RechgeMessageRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class RecvMsgRespone : public MsgHead
{
public:
	char name[128];
	char data[128];

public:
	RecvMsgRespone();

	~RecvMsgRespone();

	RecvMsgRespone(const RecvMsgRespone &obj); 

	RecvMsgRespone &operator =(const RecvMsgRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class RoomListRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t game_typeid;
	uint32_t area_typeid;

public:
	RoomListRequest();

	~RoomListRequest();

	RoomListRequest(const RoomListRequest &obj); 

	RoomListRequest &operator =(const RoomListRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class RoomListRespone : public MsgHead
{
public:
	uint32_t area_typeid;
	vector<RoomListData> room_data;

public:
	RoomListRespone();

	~RoomListRespone();

	RoomListRespone(const RoomListRespone &obj); 

	RoomListRespone &operator =(const RoomListRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class SceneRespone : public MsgHead
{
public:
	uint32_t sceneid;
	uint32_t usetime;
	vector<FishData> fishdata;

public:
	SceneRespone();

	~SceneRespone();

	SceneRespone(const SceneRespone &obj); 

	SceneRespone &operator =(const SceneRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class SceneStateRespone : public MsgHead
{
public:
	uint32_t sceneid;
	uint32_t usetime;
	uint32_t state;

public:
	SceneStateRespone();

	~SceneStateRespone();

	SceneStateRespone(const SceneStateRespone &obj); 

	SceneStateRespone &operator =(const SceneStateRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class SendMailRespone : public MsgHead
{
public:
	vector<MailInfotemp> mail_list;

public:
	SendMailRespone();

	~SendMailRespone();

	SendMailRespone(const SendMailRespone &obj); 

	SendMailRespone &operator =(const SendMailRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class SendMsgRequest : public MsgHead
{
public:
	char send_name[128];
	char recv_name[128];
	char data[128];

public:
	SendMsgRequest();

	~SendMsgRequest();

	SendMsgRequest(const SendMsgRequest &obj); 

	SendMsgRequest &operator =(const SendMsgRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class SendMsgRespone : public MsgHead
{
public:
	uint32_t flag;

public:
	SendMsgRespone();

	~SendMsgRespone();

	SendMsgRespone(const SendMsgRespone &obj); 

	SendMsgRespone &operator =(const SendMsgRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class SendMsgToClientRespone : public MsgHead
{
public:
	char name[128];
	char data[128];

public:
	SendMsgToClientRespone();

	~SendMsgToClientRespone();

	SendMsgToClientRespone(const SendMsgToClientRespone &obj); 

	SendMsgToClientRespone &operator =(const SendMsgToClientRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class SendPublicMsgRequest : public MsgHead
{
public:
	uint32_t uin;
	char data[128];

public:
	SendPublicMsgRequest();

	~SendPublicMsgRequest();

	SendPublicMsgRequest(const SendPublicMsgRequest &obj); 

	SendPublicMsgRequest &operator =(const SendPublicMsgRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class SendPublicMsgRespone : public MsgHead
{
public:
	uint32_t flag;

public:
	SendPublicMsgRespone();

	~SendPublicMsgRespone();

	SendPublicMsgRespone(const SendPublicMsgRespone &obj); 

	SendPublicMsgRespone &operator =(const SendPublicMsgRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class SevenGiftRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	SevenGiftRequest();

	~SevenGiftRequest();

	SevenGiftRequest(const SevenGiftRequest &obj); 

	SevenGiftRequest &operator =(const SevenGiftRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class SevenGiftRespone : public MsgHead
{
public:
	uint32_t day;
	uint32_t flag;

public:
	SevenGiftRespone();

	~SevenGiftRespone();

	SevenGiftRespone(const SevenGiftRespone &obj); 

	SevenGiftRespone &operator =(const SevenGiftRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ShootRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t order;
	int32_t x;
	int32_t y;
	int32_t is_robot;

public:
	ShootRequest();

	~ShootRequest();

	ShootRequest(const ShootRequest &obj); 

	ShootRequest &operator =(const ShootRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ShootRespone : public MsgHead
{
public:
	uint32_t uin;
	uint32_t order;
	int32_t x;
	int32_t y;
	int32_t is_compute;

public:
	ShootRespone();

	~ShootRespone();

	ShootRespone(const ShootRespone &obj); 

	ShootRespone &operator =(const ShootRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class ShootSelfRespone : public MsgHead
{
public:
	uint32_t flag;

public:
	ShootSelfRespone();

	~ShootSelfRespone();

	ShootSelfRespone(const ShootSelfRespone &obj); 

	ShootSelfRespone &operator =(const ShootSelfRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class SystemBroadcastRespone : public MsgHead
{
public:
	uint32_t type;
	char data[1024];

public:
	SystemBroadcastRespone();

	~SystemBroadcastRespone();

	SystemBroadcastRespone(const SystemBroadcastRespone &obj); 

	SystemBroadcastRespone &operator =(const SystemBroadcastRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class TicketRankRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	TicketRankRequest();

	~TicketRankRequest();

	TicketRankRequest(const TicketRankRequest &obj); 

	TicketRankRequest &operator =(const TicketRankRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class TicketRankRespone : public MsgHead
{
public:
	vector<RankInfo> rank_list;
	uint32_t rank;

public:
	TicketRankRespone();

	~TicketRankRespone();

	TicketRankRespone(const TicketRankRespone &obj); 

	TicketRankRespone &operator =(const TicketRankRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UnificationRespone : public MsgHead
{
public:
	uint32_t flag;

public:
	UnificationRespone();

	~UnificationRespone();

	UnificationRespone(const UnificationRespone &obj); 

	UnificationRespone &operator =(const UnificationRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UpdateAcctInfoRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t type;
	char data[128];
	char value[128];

public:
	UpdateAcctInfoRequest();

	~UpdateAcctInfoRequest();

	UpdateAcctInfoRequest(const UpdateAcctInfoRequest &obj); 

	UpdateAcctInfoRequest &operator =(const UpdateAcctInfoRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UpdateAcctInfoRespone : public MsgHead
{
public:
	uint32_t flag;
	uint32_t type;

public:
	UpdateAcctInfoRespone();

	~UpdateAcctInfoRespone();

	UpdateAcctInfoRespone(const UpdateAcctInfoRespone &obj); 

	UpdateAcctInfoRespone &operator =(const UpdateAcctInfoRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UpdateListRequest : public MsgHead
{
public:
	uint32_t version;
	uint32_t source;

public:
	UpdateListRequest();

	~UpdateListRequest();

	UpdateListRequest(const UpdateListRequest &obj); 

	UpdateListRequest &operator =(const UpdateListRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UpdateListResponse : public MsgHead
{
public:
	vector<PkgVersionList> pkg_list;

public:
	UpdateListResponse();

	~UpdateListResponse();

	UpdateListResponse(const UpdateListResponse &obj); 

	UpdateListResponse &operator =(const UpdateListResponse &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UploadRequest : public MsgHead
{
public:
	uint32_t version;
	uint32_t vertype;
	char fname[128];
	char fmd5[32];
	uint32_t is_need_data;
	vector<uint8_t> fbuf;
	uint32_t source;

public:
	UploadRequest();

	~UploadRequest();

	UploadRequest(const UploadRequest &obj); 

	UploadRequest &operator =(const UploadRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UploadResponse : public MsgHead
{
public:
	uint32_t version;
	char fname[128];
	int32_t result;

public:
	UploadResponse();

	~UploadResponse();

	UploadResponse(const UploadResponse &obj); 

	UploadResponse &operator =(const UploadResponse &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UseObjectItemRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t item_id;
	uint32_t item_typeid;
	char address[256];
	char consignee[256];
	char phone[32];

public:
	UseObjectItemRequest();

	~UseObjectItemRequest();

	UseObjectItemRequest(const UseObjectItemRequest &obj); 

	UseObjectItemRequest &operator =(const UseObjectItemRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UseObjectItemRespone : public MsgHead
{
public:
	uint32_t uin;
	uint32_t item_typeid;

public:
	UseObjectItemRespone();

	~UseObjectItemRespone();

	UseObjectItemRespone(const UseObjectItemRespone &obj); 

	UseObjectItemRespone &operator =(const UseObjectItemRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UseTelFareItemRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t item_id;
	uint32_t item_typeid;
	char msisdn[16];
	uint32_t verify_code;

public:
	UseTelFareItemRequest();

	~UseTelFareItemRequest();

	UseTelFareItemRequest(const UseTelFareItemRequest &obj); 

	UseTelFareItemRequest &operator =(const UseTelFareItemRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UseTelFareItemRespone : public MsgHead
{
public:
	uint32_t uin;
	uint32_t item_typeid;

public:
	UseTelFareItemRespone();

	~UseTelFareItemRespone();

	UseTelFareItemRespone(const UseTelFareItemRespone &obj); 

	UseTelFareItemRespone &operator =(const UseTelFareItemRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UserEnterHallRequest : public MsgHead
{
public:
	uint32_t uin;

public:
	UserEnterHallRequest();

	~UserEnterHallRequest();

	UserEnterHallRequest(const UserEnterHallRequest &obj); 

	UserEnterHallRequest &operator =(const UserEnterHallRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UserItemRespone : public MsgHead
{
public:
	vector<PkgInfo> pkg_list;

public:
	UserItemRespone();

	~UserItemRespone();

	UserItemRespone(const UserItemRespone &obj); 

	UserItemRespone &operator =(const UserItemRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UserRankRequest : public MsgHead
{
public:
	uint32_t uin;
	uint32_t room_id;

public:
	UserRankRequest();

	~UserRankRequest();

	UserRankRequest(const UserRankRequest &obj); 

	UserRankRequest &operator =(const UserRankRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class UserRankRespone : public MsgHead
{
public:
	uint32_t rank;
	uint32_t score;

public:
	UserRankRespone();

	~UserRankRespone();

	UserRankRespone(const UserRankRespone &obj); 

	UserRankRespone &operator =(const UserRankRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class VerifyCodeRequest : public MsgHead
{
public:
	uint32_t uin;
	char msisdn[16];

public:
	VerifyCodeRequest();

	~VerifyCodeRequest();

	VerifyCodeRequest(const VerifyCodeRequest &obj); 

	VerifyCodeRequest &operator =(const VerifyCodeRequest &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};

class VerifyCodeRespone : public MsgHead
{
public:
	uint32_t uin;

public:
	VerifyCodeRespone();

	~VerifyCodeRespone();

	VerifyCodeRespone(const VerifyCodeRespone &obj); 

	VerifyCodeRespone &operator =(const VerifyCodeRespone &obj); 

protected:
	size_t BodyLength();

	void BodyToBytes();

	void BytesToBody();

};


#endif
