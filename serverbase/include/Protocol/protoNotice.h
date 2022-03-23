/*
* =====================================================================================
*
*       Filename:  protoNotice.h
*
*    Description:  公告协议
*
*        Version:  1.0
*        Created:  2015年12月5日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTO_NOTICE_H_
#define _QW_PROTO_NOTICE_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct NoticeInfo
{
	uint32_t level;
	char title[1024];
	char note[2048];
	char time[20];
	
	NoticeInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};


struct protoNoticeRespone : MSG_HEAD
{
	enum { IDD = NOTICERESPONE, };
	std::vector<NoticeInfo> noticeList;

	protoNoticeRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//更新服务器公告
struct protoUpdateSvrNoticeRequest : MSG_HEAD
{
	enum { IDD = UPD_NOTICE_REQUEST, };
	uint32_t version;
	char channel[64];

	protoUpdateSvrNoticeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoUpdateSvrNoticeRespone : MSG_HEAD
{
	enum { IDD = UPD_NOTICE_RESPONSE, };
	std::vector<NoticeInfo> noticeList;

	protoUpdateSvrNoticeRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//  大厅公告通知（凤凰城）
struct protoHallNoticeNotify : MSG_HEAD
{
	enum { IDD = PROTO_HALL_NOTICE_NOTIFY, };
	char text[4096];

	protoHallNoticeNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

#endif	//_QW_PROTO_NOTICE_H_
