/*
 * =====================================================================================
 *
 *       Filename:  protoUpdate.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/12/2015 01:57:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _GFX_UPDATESVR_PROTOCOL_DEFINE_H_
#define _GFX_UPDATESVR_PROTOCOL_DEFINE_H_

#include <vector>
#include "protoDef.h"
#include "protoBase.h"

// 内存辅助类
class BufferGuard
{
public:
	BufferGuard(void *ptr);
	~BufferGuard();
	void Keep(bool val = true);
private:
	void*	m_ptr;
	bool  	m_autoRelease;
};

enum PackageSource 
{ 
	sourcePC = 0,
	sourceAndroid,
	sourceIOS,
};

class protoUploadRequest
{
public:
	enum {IDD = UPD_UPLOAD_REQUEST, };
public:
	protoUploadRequest(uint32_t ver, uint32_t verType, const char *pszFile, uint32_t source);
	protoUploadRequest(const char *pszPath);
	~protoUploadRequest();

	/* 序列化数据，并将数据导出到pData
	 * return: -1 文件名出错
	 * 		   -2 文件出错
	 * 		   -3 序列化出错
	 * 		   0  成功
	 * */
	int32_t Serialize(void *&pData, size_t &cbData);
	/* 反序列化数据
	 * return: -1 数据出错,或数据包不完整
	 * 		   -2 文件出错
	 * 		   -3 反序列化出错
	 * 		   -4 MD5不匹配
	 * 		   -5 校验码不正确，怀疑数据被篡改
	 * 		   0  成功
	 * */
	int32_t Deserialize(void *pData, size_t cbData);
	uint32_t getProtoLen(uint32_t fileSize = 0);

public:
	uint32_t 		m_version;		// 版本号 
	uint32_t		m_verType;		// 版本类型: 1增量包;999完全包
	std::string		m_fileName;		// 文件名
	std::string		m_fileMD5;		// MD5
	uint32_t		m_fileSize;		// 文件大小
	MSG_HEAD		m_head;
	uint32_t		m_withFileData;	// 是否上传文件数据，0-不上传,1上传 
	uint32_t		m_source;		// 版本来源: 1-PC; 2-Android; 3-iOS

private:
	std::string		m_filePath;		// 文件路径
	std::string		m_strFile;		// 临时存储的文件
};

class protoUploadResponse
{
public:
	enum { IDD = UPD_UPLOAD_RESPONSE, };

public:
	protoUploadResponse(uint32_t ver, uint32_t ret, const char *pszFile);
	protoUploadResponse();
	~protoUploadResponse();

	int32_t Serialize(void *&pData, size_t &cbData);
	int32_t Deserialize(void *pData, size_t cbData);

	uint32_t getProtoLen(void);

public:
	uint32_t		m_version;
	std::string		m_fileName;
	uint32_t		m_result;	// 1 成功, 2失败
	MSG_HEAD		m_head;
};

class protoUpdateListRequest
{
public:
	enum { IDD = UPD_UPDATELIST_REQUEST, };

	protoUpdateListRequest(uint32_t ver, uint32_t source);
	protoUpdateListRequest();
	~protoUpdateListRequest();

	int32_t Serialize(void *&pData, size_t &cbData);
	int32_t Deserialize(void *pData, size_t cbData);
	uint32_t getProtoLen(void);

public:
	uint32_t		m_version;
	uint32_t		m_source;		// 客户端来源: 1-PC; 2-Android; 3-iOS
	char			m_channel[32];
	MSG_HEAD		m_head;
};

class protoUpdateListResponse
{
public:
	enum { IDD = UPD_UPDATELIST_RESPONSE, };
	struct VerItem
	{
		uint32_t		version;
		uint32_t		vertype;
		uint32_t		fsize;
		std::string		fname;
		std::string		fmd5;
		std::string		url;
	};
	typedef std::vector<VerItem>	VERITEM_VEC;

	protoUpdateListResponse();
	~protoUpdateListResponse();

	int32_t Serialize(void *&pData, size_t &cbData);
	int32_t Deserialize(void *pData, size_t cbData);
	uint32_t getProtoLen(void);

public:
	uint32_t		m_count;
	VERITEM_VEC		m_vecVer;
	MSG_HEAD		m_head;
};


#endif // _GFX_UPDATESVR_PROTOCOL_DEFINE_H_


