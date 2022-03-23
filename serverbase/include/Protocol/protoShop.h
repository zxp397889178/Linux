/*
* =====================================================================================
*
*       Filename:  protoShop.h
*
*    Description:  商城协议（虚拟商城，实物商城）
*
*        Version:  1.0
*        Created:  2015年10月9日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _GFX_PROXY_SHOPPROTOCOL_H_
#define _GFX_PROXY_SHOPPROTOCOL_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct ShopInfo
{
	uint32_t item_typeid;
	uint32_t type;
	char pos[64];
	uint32_t item_label;
	uint32_t currency;
	uint32_t price;
	uint32_t discount;
	uint32_t reward_coin;
	char term[128];
	char describe[128];
	char percentage[128];

	ShopInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct GiftShopInfo
{
	uint32_t itemTypeid;
	char itemName[128];
	uint32_t itemClass;
	uint32_t type;
	char pos[64];
	uint32_t itemLabel;
	uint32_t currency;
	uint32_t price;
	uint32_t discount;
	uint32_t rewardCoin;
	char term[128];
	char pngUrl[128];
	uint32_t reverse1;
	char reverse2[128];

	GiftShopInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

/*虚拟道具商城*/
struct protoShopListRequest : MSG_HEAD
{
	enum { IDD = SHOP_LIST_REQUEST, };
	uint32_t		uin;

	protoShopListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

/*虚拟道具商城*/
struct protoShopListNewRequest : MSG_HEAD
{
	enum { IDD = SHOPLIST_NEW_REQUEST, };
	uint32_t		uin;

	protoShopListNewRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoShopListRespone : MSG_HEAD
{
	enum { IDD = SHOP_LIST_RESPONE, };
	std::vector<ShopInfo> shop_list;

	protoShopListRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*实物道具商城*/
struct protoObjectShopListRequest : MSG_HEAD
{
	enum { IDD = OBJECT_SHOP_LIST_REQUEST, };
	uint32_t		uin;

	protoObjectShopListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoObjectShopListRespone : MSG_HEAD
{
	enum { IDD = OBJECT_SHOP_LIST_RESPONE, };
	std::vector<ShopInfo> shop_list;

	protoObjectShopListRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*兑奖商城*/
struct protoGiftShopListRequest : MSG_HEAD
{
	enum { IDD = GIFTSHOP_LIST_REQUEST, };
	uint32_t		uin;

	protoGiftShopListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoGiftShopListRespone : MSG_HEAD
{
	enum { IDD = GIFTSHOP_LIST_RESPONE, };
	std::vector<GiftShopInfo> shopList;

	protoGiftShopListRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

#endif
