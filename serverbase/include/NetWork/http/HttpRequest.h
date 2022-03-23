/*
 * =====================================================================================
 *
 *       Filename:  HttpRequest.h
 *
 *    Description:  http request
 *
 *        Version:  1.0
 *        Created:  05/21/2015 03:17:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_NETWORK_HTTP_REQUEST_H_
#define _QW_CRAZYFISH_NETWORK_HTTP_REQUEST_H_

#pragma once 

#include <vector>
#include "BaseCode.h"
#include "HttpData.h"
#include "MultiPartData.h"
#include "HttpRespMaker.h"

namespace http {

class CHttpRequest : public CHttpData
{
    friend class CMultiPartData;
public:
	CHttpRequest();
	~CHttpRequest();
	CHttpRequest &operator =(CHttpRequest &obj);

	virtual void clear(void);
	virtual void pack(CByteStream &data, bool bSetContentLength);

	const CMyString getMethod(void);
	const CMyString getUrl(void);
	CMyString getUrl(std::vector<CMyString> &except_key);
	
	const CMyString getFullUrl(void);
	
    bool IsParamExist(const CMyString &strParam);
	CMyString getParam(const CMyString &strParam);
    bool getParam(const CMyString &strParam, CMyString &val);
    bool getParam(const CMyString &strParam, int32_t &val);
    bool getParam(const CMyString &strParam, int64_t &val);
    bool getParam(const CMyString &strParam, uint32_t &val);
    bool getParam(const CMyString &strParam, uint64_t &val);
    bool getParamAsTime(const CMyString &strParam, time_t &val);
    bool getParamAsDate(const CMyString &strParam, time_t &val);
    bool getParam(const CMyString &strParam, double &val);
    bool getParam(const CMyString &strParam, bool &val);

    const char* getParamStr(const CMyString &param);
    int32_t getParamInt(const CMyString &param);
    int64_t getParamInt64(const CMyString &param);
    void getFullParamStr(CMyString &strParamStr); 

	NodeVector &getParams() { return m_params; }

	void setParam(const CMyString &strParam, const CMyString &strVal);
	void setParam(const CMyString &strParam, int32_t val);
	void setParam(const CMyString &strParam, uint32_t val);
	void setParam(const CMyString &strParam, int64_t val);
	void setParam(const CMyString &strParam, uint64_t val);


	void getPost(CByteStream &buff);

	void setIp(const CMyString &strIp);
	const char* getIp(void) { return m_strIp.c_str(); }
	uint64_t getBeginTime(void) { return m_begin_time; }

	void getMapNode(map<string, string> &mapNode);

	void UrlEncode();
	void UrlDecode();

public:
	void setMethod(const CMyString& method);
	void setUrl(const CMyString& url);
	void setPost(void *pBuff, size_t cbBuff);

	void getPostFromMultipart(map<string, string>& mapPara);

public:
    // params 以','分隔开的多个参数
    bool ValidParam(const CMyString &params, CMyString &strResult);
    // 校验vecParam中是否有值
    // 返回：true 表示结果校验成功，false表示缺少参数
    bool ValidParam(std::vector<CMyString> &vecParam, CMyString &strResult);

    // 从http request 的param中构那家CHttpRespMaker,主要是用于验签之用
    // 返回值为参数个数
    int32_t BuildRespMaker(CHttpRespMaker &respMaker);
    // 从http request 的param中构那家CHttpRespMaker,主要是用于验签之用
    // 仅需要vecParam中的参数
    int32_t BuildRespMaker(std::vector<CMyString> &vecParam, CHttpRespMaker &respMaker);

    // 从http request 的param中构那家CHttpRespMaker,主要是用于验签之用
    // 排除vecParam中的参数
    int32_t BuildRespMaker_Except(std::vector<CMyString> &vecParam, CHttpRespMaker &respMaker);

protected:
	virtual int32_t onHeaderBegin(const CMyString &strLine);
	virtual void    onHeaderEnd(void);
    
    virtual void parsePostData(void);

protected:
	void parseParam(const CMyString &strLine);
	int32_t addOneParam(const CMyString &strParam);

    void parseParamByJson(const CMyString &strJson);
    void parseParamByXml(const CMyString &strXml);

    int32_t parseMultiPart(CByteStream &buff, const CMyString &strBoundary);

private:
	CMyString	m_method;
	CMyString	m_url;
	CMyString	m_strIp;	

	NodeVector	m_params;

	uint64_t	m_begin_time;	

//private:
//    CMultiPartData  m_mpd;
};

} 

#endif 

