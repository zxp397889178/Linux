/*
 * =====================================================================================
 *
 *       Filename:  HttpData.h
 *
 *    Description:  http data
 *
 *        Version:  1.0
 *        Created:  05/21/2015 10:38:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_NETWORK_HTTP_DATA_H_
#define _QW_CRAZYFISH_NETWORK_HTTP_DATA_H_

#include <string>
#include <vector>
#include "BaseCode.h"

namespace http{

class CHttpData
{
public:
	enum State
	{
		stateNone = 0,
		stateOK,
		statePending,
		stateError,
		stateTooLong, // >10MB
        
        stateChunkedEnd, // chunked 读取完毕
	};

	struct Node
	{
		CMyString  name;
		CMyString  value;
		Node() : name(), value()  {}
		Node(const CMyString &key, const CMyString &val)
			: name(key)
			, value(val)
		{
		}
	};
	typedef std::vector<Node>	NodeVector;

	CHttpData();
	virtual ~CHttpData();
	CHttpData &operator =(CHttpData &obj);

	int32_t read(void *pBuff, size_t cbBuff);
	virtual void clear(void);
	virtual void pack(CByteStream &data, bool bSetContentLength);

public:
	enum State getState(void) const { return m_eState; }
	bool IsFinished(void);
    bool IsStateValid(void);

	const CMyString& getVersion(void) const	{ return m_version; }
	void setVersion(const CMyString &ver) { m_version = ver; }

	void setHeader(const CMyString &strKey, const CMyString &strVal);
	void removeHeader(const CMyString &strKey);
	CMyString getHeader(const CMyString &strKey);
    CMyString& getFullHeader(void) { return m_strOriginHeader; }
	NodeVector &GetHeads() { return m_headers ; }
	bool IsHeaderEnd(void) const;
    // 判断是否存在 header
    bool IsExistHeader(const CMyString &strKey);
    // 获得头的数据，没有数据返回false
    bool getHeader(const CMyString &strKey, CMyString &strVal);

	CMyString getCookie(const CMyString &strCookie);
	void setCookie(const CMyString &strCookie, const CMyString &strVal);
	void removeCookie(const CMyString &strCookie);
    CMyString& getFullCookie(void) { return m_strOriginCookie; }
	NodeVector &GetCookies() { return m_cookies; }

	uint32_t getContentLength(void) const { return m_contentLength; }
	uint32_t getUnreadLength(void) const;

	CByteStream& getBuff(void) { return m_buffer; }
	CByteStream& GetPost() { return m_post; }

protected:
	int32_t GetHeaderIndex() const { return m_headerIndex; }
	uint32_t GetHeaderLength() const { return m_headerLength; }
	uint32_t GetTotalLength() const { return m_totalLength; }
	bool    IsContentLengthValid(void);

protected:
	int32_t parse(void);

	virtual int32_t onHeaderBegin(const CMyString &strLine) = 0;
	virtual void    onHeaderEnd(void);
	bool    parseHeader(void);
	int32_t parseHeaderLine(const CMyString &strLine);
	int32_t onHeader(const CMyString &strKey, const CMyString &strVal);

	void    onCookie(const CMyString &strVal);
	void    addOneCookie(const CMyString &strCookie);

	bool IsChunkedData(void) { return m_bIsChunked; }
	virtual void parseChunkedData(void);
    virtual void parsePostData(void);
	bool readChunkedLine(char *pc, int32_t bufLen, int32_t &pos, int32_t &chunkSize);

protected:
	CMyString node_get(NodeVector &nodes, const CMyString &strKey);
	void node_set(NodeVector &nodes, const CMyString& strKey, const CMyString& strVal);
	void node_remove(NodeVector &nodes, const CMyString &strKey);
	CMyString node_encode(NodeVector &nodes, 
			const CMyString &nodeDelimiter, const CMyString &rowDelimiter, size_t maxLength);
	CMyString node_get_all(NodeVector &nodes, std::vector<CMyString> &vecExcept);
    bool node_find(NodeVector &nodes, const CMyString &strKey);

    bool node_get(NodeVector &nodes, const CMyString &strKey, CMyString &strVal);

private:
	int32_t parseChunkSize(char *pc, int32_t bufLen);
    
    // void parseChunkExtData(void);
    // void parseChunkedExtLine(char *pc);

protected:
	CByteStream			m_buffer;
	NodeVector			m_headers;
	NodeVector			m_cookies;

	uint32_t			m_contentLength;
	CMyString			m_version;

	enum State			m_eState;
	int32_t				m_headerIndex;

protected:
	uint32_t			m_headerLength;
	uint32_t			m_totalLength;

	CByteStream			m_post;

private:
    CMyString           m_strOriginCookie;
    bool                m_bIsChunked;
    int32_t             m_chunkIndex;
    CMyString           m_strOriginHeader;     // 原始的头

};

} 

#endif 


