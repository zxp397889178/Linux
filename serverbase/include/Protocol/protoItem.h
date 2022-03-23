/*
* =====================================================================================
*
*       Filename:  protoItem.h
*
*    Description:  Item protocol
*
*        Version:  1.0
*        Created:  2015年10月09日 17时00分32秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  liuwx
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _GFX_PROXY_ITEMPROTOCOL_H_
#define _GFX_PROXY_ITEMPROTOCOL_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"
#include "protoRoom.h"


//购买道具请求
struct protoBuyItemRequest : MSG_HEAD
{
	enum { IDD = BUY_ITEM_REQUEST, };
	uint32_t		uin;
	uint32_t		item_typeid;

	protoBuyItemRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//购买道具应答
struct protoBuyItemRespone : MSG_HEAD
{
	enum { IDD = BUY_ITEM_RESPONE, };
	uint32_t		uin;
	uint32_t		item_typeid;

	protoBuyItemRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//购买实物道具请求
struct protoBuyObjectItemRequest : MSG_HEAD
{
	enum { IDD = BUY_OBJECT_ITEM_REQUEST, };
	uint32_t		uin;
	uint32_t		item_typeid;

	protoBuyObjectItemRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//购买实物道具应答
struct protoBuyObjectItemRespone : MSG_HEAD
{
	enum { IDD = BUY_OBJECT_ITEM_RESPONE, };
	uint32_t		uin;
	uint32_t		item_typeid;

	protoBuyObjectItemRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//使用话费道具请求
struct protoUseTelFareItemRequest : MSG_HEAD
{
	enum { IDD = USE_TEL_FARE_ITEM_REQUEST, };
	uint32_t		uin;
	uint32_t		item_id;
	uint32_t		item_typeid;
	char			msisdn[16];
	uint32_t		verify_code;

	protoUseTelFareItemRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//使用话费道具应答
struct protoUseTelFareItemRespone : MSG_HEAD
{
	enum { IDD = USE_TEL_FARE_ITEM_RESPONE, };
	uint32_t		uin;
	uint32_t		item_typeid;

	protoUseTelFareItemRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//使用实物道具请求
struct protoUseObjectItemRequest : MSG_HEAD
{
	enum { IDD = USE_OBJECT_ITEM_REQUEST, };
	uint32_t		uin;
	uint32_t		item_id;
	uint32_t		item_typeid;
	char			address[256];
	char			consignee[256];
	char			phone[32];

	protoUseObjectItemRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//使用实物道具应答
struct protoUseObjectItemRespone : MSG_HEAD
{
	enum { IDD = USE_OBJECT_ITEM_RESPONE, };
	uint32_t		uin;
	uint32_t		item_typeid;

	protoUseObjectItemRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//用户道具应答
struct protoUserItemRespone : MSG_HEAD
{
	enum { IDD = USER_ITEM_RESPONE, };
	struct PkgInfo
	{
		uint32_t	item_id;
		uint32_t	item_typeid;
		uint32_t	num;

		PkgInfo();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};
	std::vector<PkgInfo>	pkg_list;

	protoUserItemRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//使用话费道具验证码请求
struct protoVerifyCodeRequest : MSG_HEAD
{
	enum { IDD = VERIFY_CODE_REQUEST, };
	uint32_t		uin;
	char			msisdn[16];

	protoVerifyCodeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//使用话费道具验证码应答
struct protoVerifyCodeRespone : MSG_HEAD
{
	enum { IDD = VERIFY_CODE_RESPONE, };
	uint32_t		uin;

	protoVerifyCodeRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//奖券兑换请求
struct protoTicketExchangeRequest : MSG_HEAD
{
	enum { IDD = TICKET_EXCHANGE_REQUEST, };
	uint32_t uin;
	uint32_t itemTypeId;
	int32_t num;

	protoTicketExchangeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//奖券兑换应答
struct protoTicketExchangeRespone : MSG_HEAD
{
	enum { IDD = TICKET_EXCHANGE_RESPONE, };
	uint32_t itemTypeId;

	protoTicketExchangeRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*查询物品描述*/
struct QueryItemDescRequest : MSG_HEAD
{
	enum { IDD = QUERY_ITEM_DESC_REQUEST, };
	uint32_t		uin;
	uint32_t		itemTypeId;

	QueryItemDescRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct QueryItemDescRespone : MSG_HEAD
{
	enum { IDD = QUERY_ITEM_DESC_RESPONE, };
	uint32_t		itemTypeId;
	char			desciption[1024];
	uint32_t		reverse1;
	char			reverse2[256];

	QueryItemDescRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*查询物品信息*/
struct QueryItemInfoRequest : MSG_HEAD
{
	enum { IDD = QUERY_ITEM_INFO_REQUEST, };
	uint32_t		uin;
	uint32_t		itemTypeId;

	QueryItemInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct QueryItemInfoRespone : MSG_HEAD
{
	enum { IDD = QUERY_ITEM_INFO_RESPONE, };
	uint32_t		itemTypeId;
	char			itemName[128];
	uint32_t		itemClass;
	char			pngUrl[128];
	uint32_t		reverse1;
	char			reverse2[256];

	QueryItemInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*使用物品*/
struct protoUseItemRequest : MSG_HEAD
{
	enum { IDD = USE_ITEM_REQUEST, };
	uint32_t		uin;
	uint32_t		itemTypeId;
	uint32_t		num;
	uint32_t		itemId;

	protoUseItemRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoUseItemResponse : MSG_HEAD
{
	enum { IDD = USE_ITEM_RESPONSE, };

	enum eResult
	{
		Success = 0,
		ItemNotFound,
		ItemNumError,
	};

	uint32_t		result;
	uint32_t		itemTypeId;
	uint32_t		num;

	protoUseItemResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*用户背包*/
struct PackageInfo
{
	uint32_t	itemId;
	uint32_t	itemTypeId;
	uint32_t	num;
	char		url[256];
	uint32_t	itemClass;			
	uint32_t	reverse2;
	char		reverse3[256];
	char		reverse4[256];

	PackageInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoPackageInfoRequest : MSG_HEAD
{
	enum { IDD = PACKAGE_INFO_REQUEST, };
	uint32_t		uin;

	protoPackageInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoPackageInfoResponse : MSG_HEAD
{
	enum { IDD = PACKAGE_INFO_RESPONSE, };

	std::vector<PackageInfo>	packageList;

	protoPackageInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoPackageInfoChangeNotify : MSG_HEAD
{
	enum { IDD = PACKAGE_INFO_CHANGE_NOTIFY, };

	std::vector<PackageInfo>	packageList;

	protoPackageInfoChangeNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//购买道具请求(hallsvr)
struct protoHallBuyItemRequest : MSG_HEAD
{
	enum { IDD = HALL_BUY_ITEM_REQUEST, };
	uint32_t		uin;
	uint32_t		itemTypeId;
	uint32_t		num;

	protoHallBuyItemRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//购买道具应答(hallsvr)
struct protoHallBuyItemResponse : MSG_HEAD
{
	enum { IDD = HALL_BUY_ITEM_RESPONSE, };
	int32_t			result;
	uint32_t		itemTypeId;
	uint32_t		num;

	protoHallBuyItemResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*查询物品信息（hallsvr）*/
struct protoQueryItemInfoRequest : MSG_HEAD
{
	enum { IDD = HALL_QUERY_ITEM_INFO_REQUEST, };
	uint32_t		uin;
	uint32_t		itemTypeId;

	protoQueryItemInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoQueryItemInfoResponse : MSG_HEAD
{
	enum { IDD = HALL_QUERY_ITEM_INFO_RESPONSE, };
	uint32_t		itemTypeId;
	char			itemName[128];
	uint32_t		itemClass;
	char			pngUrl[128];
	char			itemDesc[256];
	uint32_t		buyPrice;
	uint32_t		buyCurrencyType;
	uint32_t		sellPrice;
	uint32_t		sellCurrencyType;
	uint32_t		reverse1;			// 最低赠送个数，0为不可赠送
	uint32_t		reverse2;
	char			reverse3[256];
	char			reverse4[256];

	protoQueryItemInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*查询道具数量*/
struct protoQueryPackageItemNumRequest : MSG_HEAD
{
	enum { IDD = HALL_QUERY_ITEM_NUM_REQUEST, };
	uint32_t		uin;
	uint32_t		itemTypeId;

	protoQueryPackageItemNumRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoQueryPackageItemNumResponse : MSG_HEAD
{
	enum { IDD = HALL_QUERY_ITEM_NUM_RESPONSE, };
	uint32_t		itemTypeId;
	uint32_t		num;

	protoQueryPackageItemNumResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//购买道具扣款请求(hallsvr->gamesvr)
struct protoSvrBuyItemCostRequest : MSG_HEAD
{
	enum { IDD = PROTO_HALL_BUY_ITEM_COST_REQ, };
	uint32_t		uin;
	uint32_t		itemTypeId;
	uint32_t		coin;
	uint32_t		ticket;
	uint32_t		money;
	uint32_t		num;

	protoSvrBuyItemCostRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//购买道具扣款应答(gamesvr->hallsvr)
struct protoSvrBuyItemCostResponse : MSG_HEAD
{
	enum { IDD = PROTO_HALL_BUY_ITEM_COST_RESP, };
	uint32_t		uin;
	uint32_t		itemTypeId;
	uint32_t		coin;
	uint32_t		ticket;
	uint32_t		money;
	int32_t			result;
	uint32_t		num;
	char			channel[64];

	protoSvrBuyItemCostResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//增加道具通知(gamesvr->hallsvr)
struct protoSvrAddItemNotify : MSG_HEAD
{
	enum { IDD = PROTO_HALL_ADD_ITEM_NOTIFY, };
	uint32_t		uin;
	uint32_t		itemTypeId;
	uint32_t		num;
	uint32_t		coin;
	uint32_t		ticket;
	uint32_t		money;
	char			channel[64];
	uint32_t		type;

	enum eType
	{
		BuyItem = 1,			// 商城购买
		LevelGift = 3,			// 等级礼包
		Recharge = 4,			// 充值购买
		MailItem = 5,			// 邮件发放
		VipRoomRefund = 6,		// VIP房间报名费退款
	};

	protoSvrAddItemNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


/*道具合成*/
struct protoItemCompoundRequest : MSG_HEAD
{
	enum { IDD = ITEM_COMPOUND_REQUEST, };
	uint32_t		uin;
	uint32_t		itemTypeId;
	uint32_t		targetItemTypeId;

	protoItemCompoundRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoItemCompoundRequestV1 : MSG_HEAD
{
	enum { IDD = ITEM_COMPOUND_REQUEST_V1, };
	uint32_t		uin;
	uint32_t		itemTypeId;
	uint32_t		targetItemTypeId;
	uint32_t		num;

	protoItemCompoundRequestV1();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoItemCompoundResponse : MSG_HEAD
{
	enum { IDD = ITEM_COMPOUND_RESPONSE, };
	int32_t		result;
	uint32_t	targetItemTypeId;

	protoItemCompoundResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


/*道具装备*/
struct protoItemWearRequest : MSG_HEAD
{
	enum { IDD = ITEM_WEAR_REQUEST, };
	uint32_t		uin;
	uint32_t		itemTypeId;
	uint32_t		type;

	enum eType
	{
		TakeOff = 0,		// 卸下
		Wear = 1,			// 装备
	};

	protoItemWearRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoItemWearResponse : MSG_HEAD
{
	enum { IDD = ITEM_WEAR_RESPONSE, };
	int32_t		result;
	uint32_t	itemTypeId;
	uint32_t	type;

	protoItemWearResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*已装备道具查询*/
struct protoWearedItemsRequest : MSG_HEAD
{
	enum { IDD = WEARED_ITEMS_REQUEST, };
	uint32_t		uin;

	protoWearedItemsRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoWearedItemsResponse : MSG_HEAD
{
	enum { IDD = WEARED_ITEMS_RESPONSE, };
	std::vector<uint32_t> itemList;

	protoWearedItemsResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*玩家已装备物品同步*/
struct UserWearedItems
{
	uint32_t uin;
	std::vector<uint32_t> itemList;

	UserWearedItems();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoWearedItemsSync : MSG_HEAD
{
	enum { IDD = WEARED_ITEMS_SYNC, };
	std::vector<UserWearedItems> syncList;

	protoWearedItemsSync();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoSvrWearedItemsNotify : MSG_HEAD
{
	enum { IDD = PROTO_HALL_WEARED_ITEMS_NOTIFY, };
	uint32_t uin;
	std::vector<uint32_t> itemList;

	protoSvrWearedItemsNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*道具合成信息*/
struct CompoundInfo
{
	uint32_t	itemTypeId;
	uint32_t	Material1;
	uint32_t	MtrNum1;
	uint32_t	Material2;
	uint32_t	MtrNum2;
	uint32_t	Material3;
	uint32_t	MtrNum3;

	CompoundInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoItemCompoundInfoRequest : MSG_HEAD
{
	enum { IDD = ITEM_COMPOUND_INFO_REQUEST, };
	uint32_t uin;
	std::vector<uint32_t> itemList;

	protoItemCompoundInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct protoItemCompoundInfoResponse : MSG_HEAD
{
	enum { IDD = ITEM_COMPOUND_INFO_RESPONSE, };
	std::vector<CompoundInfo> infoList;

	protoItemCompoundInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//增加道具通知(gamesvr->hallsvr)
struct protoSvrHitItemNotify : MSG_HEAD
{
	enum { IDD = PROTO_HALL_HIT_ITEM_NOTIFY, };
	uint32_t		uin;
	uint32_t		itemTypeId;
	uint32_t		num;
	uint32_t		roomId;
	uint32_t		bulletLevel;
	char			channel[64];

	protoSvrHitItemNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//扣道具请求(gamesvr->hallsvr)
struct protoSvrCostItemRequest : MSG_HEAD
{
	enum { IDD = PROTO_HALL_COST_ITEM_REQUEST, };
	uint32_t		uin;
	uint32_t		roomId;
	uint32_t		type;
	char			items[128];

	protoSvrCostItemRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//扣道具应答(hallsvr->gamesvr)
struct protoSvrCostItemResponse : MSG_HEAD
{
	enum { IDD = PROTO_HALL_COST_ITEM_RESPONSE, };
	int32_t			result;
	uint32_t		uin;
	uint32_t		roomId;
	uint32_t		type;
	char			items[128];

	protoSvrCostItemResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


/*查询物品信息（hallsvr）*/
struct protoQueryItemInfoBatchRequest : MSG_HEAD
{
	enum { IDD = QUERY_ITEM_INFO_BATCH_REQUEST, };
	uint32_t		uin;
	std::vector<uint32_t> vecItemTypeId;

	protoQueryItemInfoBatchRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

/*道具字典信息*/
struct DictItemInfo
{
	uint32_t		itemTypeId;
	char			itemName[128];
	uint32_t		itemClass;
	char			pngUrl[128];
	char			itemDesc[256];
	uint32_t		buyPrice;
	uint32_t		buyCurrencyType;
	uint32_t		sellPrice;
	uint32_t		sellCurrencyType;
	uint32_t		minPresentNum;			// 最低赠送个数，0为不可赠送
	uint32_t		reverse1;
	uint32_t		reverse2;
	uint32_t		reverse3;
	char			reverse4[256];
	char			reverse5[256];
	char			reverse6[256];

	DictItemInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoQueryItemInfoBatchResponse : MSG_HEAD
{
	enum { IDD = QUERY_ITEM_INFO_BATCH_RESPONSE, };
	
	std::vector<DictItemInfo> infoList;

	protoQueryItemInfoBatchResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


/*玩家已装备道具查询*/
struct protoUserWearedItemsRequest : MSG_HEAD
{
	enum { IDD = USER_WEARED_ITEMS_REQUEST, };
	uint32_t uin;

	protoUserWearedItemsRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct WearedItemInfo
{
	uint32_t		itemTypeId;
	uint32_t		leftTime;
	uint32_t		status;
	uint32_t		reverse1;
	char			reverse2[256];

	WearedItemInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoUserWearedItemsResponse : MSG_HEAD
{
	enum { IDD = USER_WEARED_ITEMS_RESPONSE, };

	std::vector<WearedItemInfo> infoList;

	protoUserWearedItemsResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*道具装备*/
struct protoItemWearRequestV1 : MSG_HEAD
{
	enum { IDD = ITEM_WEAR_REQUEST_V1, };
	uint32_t		uin;
	uint32_t		itemTypeId;
	uint32_t		type;

	enum eType
	{
		TakeOff = 0,		// 卸下
		Wear = 1,			// 装备
	};

	protoItemWearRequestV1();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoItemWearResponseV1 : MSG_HEAD
{
	enum { IDD = ITEM_WEAR_RESPONSE_V1, };
	int32_t		result;
	uint32_t	itemTypeId;
	uint32_t	type;

	protoItemWearResponseV1();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoUseTorpedoItemSync : MSG_HEAD
{
	enum { IDD = USE_TORPEDO_TIEM_SYNC, };
	uint32_t	uin;
	uint32_t	itemTypeId;
	uint32_t	gunId;

	protoHitFishNotify::FISHDATA_VEC vecDeathFish;

	protoUseTorpedoItemSync();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoUseTorpedoItemRequest : MSG_HEAD
{
	enum { IDD = USE_TORPEDO_ITEM_REQUEST, };
	uint32_t	uin;
	uint32_t	itemTypeId;
	uint32_t	num;
	std::vector<uint32_t> vecFishInsId;

	protoUseTorpedoItemRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoUseTorpedoItemResponse : MSG_HEAD
{
	enum { IDD = USE_TORPEDO_ITEM_RESPONSE, };
	int32_t	result;
	uint32_t	itemTypeId;
	uint32_t	num;

	protoUseTorpedoItemResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoSellItemRequest : MSG_HEAD
{
	enum { IDD = SELL_ITEM_REQUEST, };
	uint32_t	uin;
	uint32_t	itemTypeId;
	uint32_t	num;

    protoSellItemRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoSellItemResponse : MSG_HEAD
{
	enum { IDD = SELL_ITEM_RESPONSE, };
	int32_t	result;
	uint32_t	currencyType;
	uint32_t	currencyValue;

    protoSellItemResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct PackageListInfo
{
	uint32_t	itemId;
	uint32_t	itemClass;	
	char		itemName[128];
	uint32_t	num;
	uint32_t	label;			
	char		url[256];
	uint32_t    dispaly;
	
	PackageListInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoPackageListRequest : MSG_HEAD
{
	enum { IDD = PACKAGE_LIST_REQUEST, };
	uint32_t		uin;
	uint32_t		label;
	uint32_t		page;
	uint32_t		getAll;
	protoPackageListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoPackageListResponse : MSG_HEAD
{
	enum { IDD = PACKAGE_LIST_RESPONSE, };

	std::vector<PackageListInfo>	packageList;

	protoPackageListResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoPackageItemInfoRequest : MSG_HEAD
{
	enum { IDD = PACKAGE_ITEM_INFO_REQUEST, };
	uint32_t		uin;
	uint32_t		itemId;
	
	protoPackageItemInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoPackageItemInfoResponse : MSG_HEAD
{
	enum { IDD = PACKAGE_ITEM_INFO_RESPONSE, };
	uint32_t		itemId;
	char			desc[256];
	uint32_t		sellCurrencyType;
	uint32_t		sellPrice;
	uint32_t		buyCurrencyType;
	uint32_t		buyPrice;
	uint32_t		compositCount;
	uint32_t		maxUseCount;
	uint32_t		cooldown;
	uint32_t		bagSell;	
	uint32_t		bagUse;
	uint32_t		lifeCycle;
	uint32_t		disappear;
	char			funcPtr[128];
	uint32_t		param1;
	
	protoPackageItemInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct GunMountProperty
{
	uint32_t	id;
	uint32_t	desc;	
	uint32_t	value;
	uint32_t	percent;			
	uint32_t    probability;
	
	GunMountProperty();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoGunMountPropertyListReq : MSG_HEAD
{
	enum { IDD = PROTO_GUN_PRO_LIST_REQ, };
	uint32_t		uin;
	
	protoGunMountPropertyListReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoGunMountPropertyListResp : MSG_HEAD
{
	enum { IDD = PROTO_GUN_PRO_LIST_RESP, };
	std::vector<GunMountProperty>	list;
	
	protoGunMountPropertyListResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoRandGunMountPropertyReq : MSG_HEAD
{
	enum { IDD = PROTO_RAND_GUN_PRO_REQ, };
	uint32_t		gunMount;
	std::vector<uint32_t>	propertyLock;
	
	protoRandGunMountPropertyReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoRandGunMountPropertyResp : MSG_HEAD
{
	enum { IDD = PROTO_RAND_GUN_PRO_RESP, };
	int32_t		result;
	std::vector<uint32_t>	property;
	
	protoRandGunMountPropertyResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct OtherGunMountProperty
{
	uint32_t	uin;
	std::vector<uint32_t>	property;
	
	OtherGunMountProperty();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoGunMountPropertyNotify : MSG_HEAD
{
	enum { IDD = PROTO_GUN_PRO_NOTIFY, };
	std::vector<OtherGunMountProperty>	list;
	
	protoGunMountPropertyNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoGetCurGunMountPropertyReq : MSG_HEAD
{
	enum { IDD = PROTO_GET_GUN_PRO_REQ, };
	uint32_t		gunMount;
	
	protoGetCurGunMountPropertyReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoGetCurGunMountPropertyResp : MSG_HEAD
{
	enum { IDD = PROTO_GET_GUN_PRO_RESP, };
	std::vector<uint32_t>	property;
	
	protoGetCurGunMountPropertyResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};
#endif //  _GFX_PROXY_ITEMPROTOCOL_H_
