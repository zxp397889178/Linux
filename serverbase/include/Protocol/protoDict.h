/*
* =====================================================================================
*
*       Filename:  protoDict.h
*
*    Description:  dict protocol
*
*        Version:  1.0
*        Created:  2016年8月24日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_DICT_H_
#define _PROTO_DICT_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct DictReward
{
	uint32_t reward_id;
	uint32_t room_id;
	char rankings[128];
	char item_sets[256];
	uint32_t ticket_num;
	uint32_t coin_num;
	uint32_t diamond_num;
	uint32_t billpiece;

	DictReward();
	int32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//查询奖励信息请求
struct protoQueryDictRewardRequest : MSG_HEAD
{
	enum { IDD = QUERY_REWARDINFO_REQUEST };

	uint32_t uin;
	uint32_t queryType;
	char queryIdList[128];
	uint32_t reqId;

	protoQueryDictRewardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询奖励信息响应
struct protoQueryDictRewardResponse : MSG_HEAD
{
	enum { IDD = QUERY_REWARDINFO_RESPONE };

	uint32_t reqId;
	vector<DictReward> vecDictReward;
	
	protoQueryDictRewardResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//查询节日UI请求
struct protoQueryDictUiRequest : MSG_HEAD
{
	enum { IDD = QUERY_DICT_UI_REQUEST };

	char deviceId[64];
	uint32_t source;
	char channel[64];
	uint32_t version;

	protoQueryDictUiRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询节日UI响应
struct protoQueryDictUiRespone : MSG_HEAD
{
	enum { IDD = QUERY_DICT_UI_RESPONSE };

	uint32_t id;
	char name[64];
	char url[128];
	uint32_t reserver1;
	char reserver2[64];
	
	protoQueryDictUiRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询规则请求
struct protoQueryDictRuleRequest : MSG_HEAD
{
	enum { IDD = QUERY_DICT_RULE_REQUEST };

	uint32_t type;
	uint32_t language;
	char deviceId[64];
	uint32_t source;
	char channel[64];
	uint32_t version;

	protoQueryDictRuleRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct DictRuleInfo
{
	uint32_t id;
	uint32_t type;
	uint32_t language;	
	char name[64];
	char url[128];
	uint32_t reserver1;
	char reserver2[64];

	DictRuleInfo();
	int32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//查询规则响应
struct protoQueryDictRuleRespone : MSG_HEAD
{
	enum { IDD = QUERY_DICT_RULE_RESPONSE };

	vector<DictRuleInfo> vecDictRuleInfo;
		
	protoQueryDictRuleRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 语言配置请求
struct protoLanguageRequest : MSG_HEAD
{
	enum { IDD = LANGUAGE_REQUEST, };

	uint32_t uin;
	uint32_t languageId;

	protoLanguageRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 语言配置响应
struct protoLanguageResponse : MSG_HEAD
{
	enum { IDD = LANGUAGE_RESPONSE, };

	uint32_t languageId;
	char value[8196];

	protoLanguageResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct GiveCoinInfo
{
	uint32_t id;
	uint32_t coin;
	uint32_t tax;

	GiveCoinInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 赠送金币配置请求
struct protoQueryDictGiveCoinRequest : MSG_HEAD
{
	enum { IDD = QUERY_DICT_GIVECOIN_REQUEST, };

	uint32_t uin;

	protoQueryDictGiveCoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 赠送金币配置响应
struct protoQueryDictGiveCoinResponse : MSG_HEAD
{
	enum { IDD = QUERY_DICT_GIVECOIN_RESPONSE, };

	vector<GiveCoinInfo> vecDictGiveCoin;

	protoQueryDictGiveCoinResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct VIP_INFO
{
	uint32_t id;
	uint32_t level;
	uint32_t exp;
	uint32_t exp_interval;
	char reward[64];
	uint32_t fish_conversion;
	uint32_t conversion_lottery;
	uint32_t seven_reward;
	uint32_t match_plus;
	uint32_t privileges;
	uint32_t permutation;
	uint32_t seven_describe;
	uint32_t signin_plus;
	uint32_t oar;
	uint32_t oar_times;
	uint32_t remittance_times;
	uint32_t present_coin_tax;

	VIP_INFO();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct VIP_INFO_EX
{
	uint32_t id;
	uint32_t level;
	uint32_t exp;
	uint32_t exp_interval;
	char reward[64];
	uint32_t fish_conversion;
	uint32_t conversion_lottery;
	uint32_t seven_reward;
	uint32_t match_plus;
	uint32_t privileges;
	uint32_t permutation;
	uint32_t seven_describe;
	uint32_t signin_plus;
	uint32_t oar;
	uint32_t oar_times;
	uint32_t remittance_times;
	uint32_t present_coin_tax;
	uint32_t present_limit_daily;
	uint32_t present_limit_everytime;

	VIP_INFO_EX();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// VIP配置请求
struct protoVipDictRequest : MSG_HEAD
{
	enum { IDD = HALL_VIP_DICT_REQUEST, };
	uint32_t uin;
	char sign[64];

	protoVipDictRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// VIP配置响应
struct protoVipDictResponse : MSG_HEAD
{
	enum { IDD = HALL_VIP_DICT_RESPONSE, };
	int32_t result;					// 0:数据已经最新; 1:数据有更新; 小于0失败
	char sign[64];

	vector<VIP_INFO> vecDictVip;

	protoVipDictResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// VIP配置请求
struct protoVipDictExRequest : MSG_HEAD
{
	enum { IDD = HALL_VIP_DICT_EX_REQUEST, };
	uint32_t uin;
	char sign[64];

	protoVipDictExRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// VIP配置响应
struct protoVipDictExResponse : MSG_HEAD
{
	enum { IDD = HALL_VIP_DICT_EX_RESPONSE, };
	int32_t result;					// 0:数据已经最新; 1:数据有更新; 小于0失败
	char sign[64];

	vector<VIP_INFO_EX> vecDictVip;

	protoVipDictExResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct COMMON_INFO
{
	uint32_t id;
	char param1[128];
	char param2[128];
	char param3[128];
	char param4[128];
	char param5[128]; 
	char param6[128];
	char param7[128];
	char param8[128];
	COMMON_INFO();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// DICTCOMMON 配置请求
struct protoCommonDictRequest : MSG_HEAD
{
	enum { IDD = HALL_COMMON_DICT_REQUEST, };
	uint32_t uin;
	uint32_t id;
	char sign[64];

	protoCommonDictRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// DICTCOMMON 配置响应
struct protoCommonDictResponse : MSG_HEAD
{
	enum { IDD = HALL_COMMON_DICT_RESPONSE, };
	int32_t result;					// 0:数据已经最新; 1:数据有更新; 小于0失败
	char sign[64];

	vector<COMMON_INFO> vecDictCommon;

	protoCommonDictResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// DICTCustomer 配置请求
struct protoCustomerRequest : MSG_HEAD
{
	enum { IDD = GAME_CUSTMOER_REQUEST, };
	uint32_t uin;					// 用户ID
	char channel[64];			//渠道

	protoCustomerRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// DICTCOMMON 配置响应
struct protoCustomerResponse : MSG_HEAD
{
	enum { IDD = GAME_CUSTMOER_RESPONSE, };
	int32_t result;					// 0：成功
	char customer[512];

	protoCustomerResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询充值商品
struct DICT_RECHARGE_ITEM
{
	uint32_t recharge_id;
	char name[64];
	int32_t name_key;
	int32_t pos;
	char icon[64];
	int32_t type;
	int32_t label;
	int32_t rmb;
	int32_t money;
	char gift_daily[64];
	char gift_extra[64];
	int32_t vip;
	int32_t discount;
	int32_t num;

	char Field1[64];
	char Field2[64];
	char Field3[64];
	int32_t Field2_Key;
	uint32_t item_typeid;


	DICT_RECHARGE_ITEM();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// VIP RECHARGE 请求
struct protoRechargeItemRequest : MSG_HEAD
{
	enum { IDD = HALL_RECHARGE_ITEM_REQUEST, };
	uint32_t uin; //商品类型
	uint32_t item_type; //商品类型
	char szMd5Key[32]; //商口KEY

	protoRechargeItemRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoRechargeItemResponse : MSG_HEAD
{
	enum { IDD = HALL_RECHARGE_ITEM_RESPONSE, };
	uint32_t item_type; //商品类型
	char szMd5Key[32]; //商口KEY
	vector<DICT_RECHARGE_ITEM> recharge_item;

	protoRechargeItemResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


// 活动杂项配置表（积分）
struct DictQuotaInfo
{
	uint32_t type;
	uint32_t value;
	uint32_t point;

	DictQuotaInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoDictQuotaInfoRequest : MSG_HEAD
{
	enum { IDD = DICT_QUOTA_INFO_REQUEST, };
	uint32_t uin;
	uint32_t type;

	protoDictQuotaInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoDictQuotaInfoResponse : MSG_HEAD
{
	enum { IDD = DICT_QUOTA_INFO_RESPONSE, };
	vector<DictQuotaInfo> infoList;

	protoDictQuotaInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//积分字典查询
struct protoDictPointRequest : MSG_HEAD
{
	enum { IDD = HALL_POINT_DICT_REQUEST, };
	uint32_t uin;

	protoDictPointRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//积分字典查询应答
struct protoDictPointResponse : MSG_HEAD
{
	enum { IDD = HALL_POINT_DICT_RESPONSE, };
	int32_t result;
	uint32_t curLevelPoint;
	uint32_t nextLevelPoint;

	uint32_t curLevelTimes;
	uint32_t nextLevelTimes;

	uint32_t curLevelTotalTimes;
	uint32_t nextLevelTotalTimes;

	uint32_t totalPresentedTimes;

	protoDictPointResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// DICTCustomer 配置请求
struct protoNewCustomerRequest : MSG_HEAD
{
    enum { IDD = GAME_NEW_CUSTMOER_REQUEST, };
    uint32_t uin;					// 用户ID
    char channel[64];			//渠道

    protoNewCustomerRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
};

// DICTCOMMON 配置响应
struct protoNewCustomerResponse : MSG_HEAD
{
    enum { IDD = GAME_NEW_CUSTMOER_RESPONSE, };
    int32_t result;					// 0：成功
    char customer[512];
    char serviceTime[512];

    protoNewCustomerResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
};

struct DictUiInfo
{
    uint32_t id;
    char name[64];
    char url[128];
    uint32_t reserve1;
    char reserve2[64];

    DictUiInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct DictADInfo
{
    char icon[128];
    uint32_t action;
    char content[512];

    DictADInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 获取UI,规则,弹窗配置请求
struct protoQueryDictUpdateCommonRequest : MSG_HEAD
{
    enum { IDD = PROTO_REQ_QUERY_DICT_UPDATE_COMMON, };

    uint32_t type;
    uint32_t language;
    uint32_t source;
    char channel[64];
    uint32_t version;

    protoQueryDictUpdateCommonRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取UI,规则,弹窗配置请求应答
struct protoQueryDictUpdateCommonResponse : MSG_HEAD
{
    enum { IDD = PROTO_RSP_QUERY_DICT_UPDATE_COMMON, };

    std::vector<DictRuleInfo> vecDictRuleInfo;
    std::vector<DictUiInfo> vecDictUiInfo;
    std::vector<DictADInfo> vecDictADInfo;

    protoQueryDictUpdateCommonResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};





struct protoQueryDictUpdateUiRequest : MSG_HEAD
{
    enum { IDD = PROTO_REQ_QUERY_DICT_UPDATE_UI, };

    uint32_t ui_type;
    uint32_t language;
    uint32_t source;
    char channel[64];
    uint32_t version;

    protoQueryDictUpdateUiRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};




struct UpdateUiInfo
{
	uint32_t type;
	uint32_t language;	
	char name[64];
	char url[128];
    uint32_t action;
    char content[128];
	uint32_t reserver1;
	char reserver2[64];

	UpdateUiInfo();
	int32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};


struct protoQueryDictUpdateUiResponse : MSG_HEAD
{
    enum { IDD = PROTO_RSP_QUERY_DICT_UPDATE_UI, };

    std::vector<UpdateUiInfo> vecUiInfo;

    protoQueryDictUpdateUiResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};




#endif //_PROTO_DICT_H_

