/*
 * =====================================================================================
 *
 *       Filename:  HttpData.cpp
 *
 *    Description:  implement of HttpData.h
 *
 *        Version:  1.0
 *        Created:  05/21/2015 10:49:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#include "HttpData.h"
#include <algorithm>
#include "Security.h"
#include "ChunkBlock.h"

namespace http {
#define LAST_HEADER_ROW		99999

CHttpData::CHttpData()
	: m_buffer()
	, m_headers()
	, m_cookies()
	, m_contentLength(0)
	, m_version()
	, m_eState(stateNone)
	, m_headerIndex(0)  
	, m_headerLength(0)
	, m_totalLength(0)
{
    m_bIsChunked = false;
    m_chunkIndex = 0;
}

CHttpData::~CHttpData()
{

}

CHttpData &CHttpData::operator =(CHttpData &obj)
{
	if (this == &obj)
	{
		return *this;
	}
	
	//m_buffer = obj.getBuff();
    m_buffer = obj.m_buffer;
	m_contentLength = obj.getContentLength();
	m_version = obj.getVersion();
	m_eState = obj.getState();
	m_headerIndex = obj.GetHeaderIndex();
	m_headerLength = obj.GetHeaderLength();
	m_totalLength = obj.GetTotalLength();
	m_headers = obj.GetHeads();
	m_cookies = obj.GetCookies();
	m_post = obj.GetPost();

    m_bIsChunked = obj.m_bIsChunked;
    m_chunkIndex = obj.m_chunkIndex;
    m_strOriginHeader = obj.m_strOriginHeader;

	return *this;
}


int32_t CHttpData::read(void *pBuff, size_t cbBuff)
{
    // LogWarn("DEBUG>[%s]", (char*)pBuff);
	m_buffer.Write(pBuff, cbBuff);
	m_totalLength += cbBuff;

	return this->parse();
}

void CHttpData::clear(void)
{
	m_buffer.clear();
	m_headers.clear();
	m_cookies.clear();
	m_contentLength = 0;
	m_version.clear();
	m_eState = stateNone;
	m_headerIndex = 0;

	m_headerLength = 0;
	m_totalLength = 0;
    m_post.clear();

    m_strOriginCookie.clear();
    m_bIsChunked = false;
    m_chunkIndex = 0;
    m_strOriginHeader.clear();

}

int32_t CHttpData::parse(void)
{
	if (m_buffer.empty())
	{
		return m_eState;
	}

	if (this->parseHeader())
	{
		if (this->getUnreadLength() > 0)
		{
			m_eState = statePending;
		}
		else
		{
            //LogDebug("HTTP>read all data.");
			m_eState = stateOK;
		}
	}
	
    if (this->IsHeaderEnd())
    {
        // LogDebug("HTTP>header end, begin to parse data, buffer size:%lu", m_buffer.size());
        if (m_bIsChunked)
        {
            this->parseChunkedData();
        }
        else if(m_eState == stateOK)
        {
            // 非chunked 状态下，在全部读完后再进行解析
            // LogDebug("HTTP>begin to parse post data.");
            this->parsePostData();
        }
    }

	return m_eState;
}

bool CHttpData::parseHeader(void)
{
	if (m_buffer.empty() || this->IsHeaderEnd())
	{
		return this->IsHeaderEnd();
	}

	int32_t stepSize(0);
	do {
		char buffer[1024 * 64] = "";
		size_t bufLen(sizeof(buffer)-1);
		bufLen = std::min(bufLen, m_buffer.size());
		m_buffer.SetPeekMode(true);
		m_buffer.Read((void*)&buffer[0], bufLen);
		m_buffer.SetPeekMode(false);

        //LogDebug("DBG>full line=%s", buffer);
		CMyString strLine;
        int32_t nLine(0);
		for(size_t idx(0); idx<bufLen; )
		{
			uint8_t c = (uint8_t)buffer[idx];
			if (c != 0x0D && c != 0x0A )
			{
				strLine += (char)c;
				++idx;
				continue;
			}

			
			int32_t tailCount(0);
			while(idx<bufLen)
			{
				uint8_t c1 = (uint8_t)buffer[idx];
				if (c1 == 0x0D || c1 == 0x0A)
				{
					++idx;
					++stepSize;
					++tailCount;
					continue;
				}
				break;
			}

            m_strOriginHeader += strLine;
            m_strOriginHeader += "\r\n";
            ++nLine;
            LogTrace("HTTP-DBG>ln#%d [%s]", nLine, strLine.c_str() );
			int32_t val = this->parseHeaderLine(strLine);
			if ( val != 0)
			{
                LogErr("HTTP-ERR>[%s]", strLine.c_str());
				if (m_eState != stateTooLong)
				{
					m_eState = stateError;
				}
				return false;
			}
			stepSize += strLine.length();
			strLine.clear();

			
			if (tailCount>=4)
			{
				this->onHeaderEnd();
				m_headerIndex = LAST_HEADER_ROW;
				break;
			}
		}
		
		if (stepSize > 0)
		{
			m_buffer.skip(stepSize);
			m_headerLength += stepSize;
		}

		if (this->IsHeaderEnd()) break;
	}while(!m_buffer.empty() && stepSize>0 );

	return this->IsHeaderEnd();
}

int32_t CHttpData::parseHeaderLine(const CMyString &strLine)
{
	++m_headerIndex;

	if (1 == m_headerIndex)
	{
		return this->onHeaderBegin(strLine);
	}

	CMyString::size_type pos = strLine.find_first_of(":");
	if (pos == CMyString::npos)
	{
		return 0;
	}

	CMyString strKey = strLine.substr(0, pos);
	CMyString strVal = strLine.substr(pos +1);
	strKey.trim();
	strVal.trim();
	return this->onHeader(strKey, strVal);
}

bool CHttpData::IsContentLengthValid(void)
{
	static uint32_t s_maxHeaderLength = 10*1024*1024; // 10MB

	return (m_contentLength <= s_maxHeaderLength);
}

void CHttpData::onHeaderEnd(void)
{
#if 1
	NodeVector::iterator it(m_headers.begin());
	for(; it != m_headers.end(); ++it)
	{
		Node &node = *it;
		LogDebug("Header> [%s] = [%s]", node.name.c_str(), node.value.c_str());
	}

	NodeVector::iterator iter(m_cookies.begin());
	for(; iter != m_cookies.end(); ++iter)
	{
		Node &node = *iter;
		LogDebug("Cookie> [%s] = [%s]", node.name.c_str(), node.value.c_str());
	}
#endif
}

void CHttpData::onCookie(const CMyString &strVal)
{
    m_strOriginCookie = strVal;
    CMyString strTemp;
    CHttpWrapper::urlDecode(strVal, strTemp);

    CStrSpliter css;
    if (!css.Split(strTemp, ';'))
    {
        return;
    }
    for(int32_t i=0; i<css.size(); ++i)
    {
        this->addOneCookie(css[i]);
    }
}

void CHttpData::addOneCookie(const CMyString &strCookie)
{
	Node node;
	CMyString::size_type pos = strCookie.find("=");
	if (pos == CMyString::npos)
	{
		node.value = strCookie;
	}
	else
	{
		CMyString strKey = strCookie.substr(0, pos);
		CMyString strVal = strCookie.substr(pos+1);
		strKey.trim();
		strVal.trim();
		node.name = strKey.c_str();
		node.value = strVal.c_str();
	}
	m_cookies.push_back(node);
}

int32_t CHttpData::onHeader(const CMyString &strKey, const CMyString &strVal)
{
    //if (strKey.empty() || strVal.empty())
    if (strKey.empty() ) 
    {
        return -1;
    }
    
    if (strcasecmp(strKey.c_str(), "Content-Length")==0)
    {
        // m_contentLength = strVal.asUint();
        m_contentLength = StrToUint32(strVal.c_str());
		if (!this->IsContentLengthValid())
		{
			m_eState = stateTooLong;
			return -1;
		}
    }
    if (strcasecmp(strKey.c_str(), "Cookie")==0)
    {
        this->onCookie(strVal);
        return 0;
    }
    if (strcasecmp(strKey.c_str(), "Transfer-Encoding")==0)
    {
        if (strcasecmp(strVal.c_str(), "chunked")==0)
        {
            m_bIsChunked = true;
            LogDebug("post data is chunked.");
        }
    }
    m_headers.push_back(Node(strKey, strVal));

	return 0;
}

uint32_t CHttpData::getUnreadLength(void) const
{
    // 对于 chunk 传输的方式不适用
    if (m_bIsChunked && (m_eState != stateChunkedEnd) )
    {
        return 1;
    }

    // 其他模式的判断方式未变
	if (0 == m_contentLength)
	{
		return 0;
	}
	if (m_contentLength + m_headerLength > m_totalLength)
		return m_contentLength + m_headerLength - m_totalLength;
	return 0;
}

bool CHttpData::IsHeaderEnd(void) const
{
	return m_headerIndex == LAST_HEADER_ROW;
}

CMyString CHttpData::node_get(NodeVector &nodes, const CMyString &strKey)
{
	NodeVector::iterator it(nodes.begin());
	for(; it != nodes.end(); ++it)
	{
		Node &node = *it;
		if (strcasecmp(node.name.c_str(), strKey.c_str()) == 0)
		{
			return node.value;
		}
	}
	return "";
}

bool CHttpData::node_get(NodeVector &nodes, const CMyString &strKey, CMyString &strVal)
{
    bool bFind(false);
	NodeVector::iterator it(nodes.begin());
	for(; it != nodes.end(); ++it)
	{
		Node &node = *it;
		if (strcasecmp(node.name.c_str(), strKey.c_str()) == 0)
		{
			strVal = node.value;
            bFind = true;
		}
	}
	return bFind;
}

bool CHttpData::node_find(NodeVector &nodes, const CMyString &strKey)
{
	NodeVector::iterator it(nodes.begin());
	for(; it != nodes.end(); ++it)
	{
		Node &node = *it;
		if (strcasecmp(node.name.c_str(), strKey.c_str()) == 0)
		{
			return true;
		}
	}
	return false;
}

CMyString CHttpData::node_get_all(NodeVector &nodes, std::vector<CMyString> &vecExcept)
{
	CMyString strUrl;
	NodeVector::iterator it(nodes.begin());
	for(; it != nodes.end(); ++it)
	{
		Node &node = *it;
		CMyString strKey = node.name;
		if (!vecExcept.empty())
		{
			bool bExcept(false);
			for(std::vector<CMyString>::iterator iter(vecExcept.begin());
					iter != vecExcept.end(); ++iter)
			{
				CMyString strExcept = *iter;
				if (strcasecmp(strKey.c_str(), strExcept.c_str())==0)
				{
					bExcept = true;
					break;
				}
			}
			if (bExcept)
			{
				continue;
			}
		}
		if (!strUrl.empty())
			strUrl += "&&";
		strUrl.Append("%s=%s",strKey.c_str(), node.value.c_str());
	}

	return strUrl.c_str();
}

void CHttpData::node_set(NodeVector &nodes, const CMyString &strKey, const CMyString &strVal)
{
	NodeVector::iterator it(nodes.begin());
	for(; it != nodes.end(); ++it)
	{
		Node &node = *it;
		if (strcasecmp(node.name.c_str(), strKey.c_str()) == 0)
		{
			node.value = strVal;
			return;
		}
	}

	Node node(strKey, strVal);
	nodes.push_back(node);
}

void CHttpData::node_remove(NodeVector &nodes, const CMyString &strKey)
{
	NodeVector::iterator it(nodes.begin());
	for(; it != nodes.end(); )
	{
		Node &node = *it;
		if (strcasecmp(node.name.c_str(), strKey.c_str()) == 0)
		{
			nodes.erase(it++);
			continue;
		}
		++it;
	}
}

CMyString CHttpData::node_encode(NodeVector &nodes, const CMyString &nodeDelimiter, 
		const CMyString &rowDelimiter, size_t maxLength)
{
	CMyString str;
	NodeVector::iterator it(nodes.begin());
	for(; it != nodes.end(); ++it)
	{
		Node &node = *it;
		if (!node.name.empty() && maxLength!=0 &&
			((node.name.length()+node.value.length())>maxLength) )
		{
			continue;
		}
		str.append(node.name).append(nodeDelimiter);
		str.append(node.value).append(rowDelimiter);
	}

	return str;
}

bool CHttpData::IsExistHeader(const CMyString &strKey)
{
    return this->node_find(m_headers, strKey);
}

bool CHttpData::getHeader(const CMyString &strKey, CMyString &strVal, bool urlDecode)
{
    CMyString strTemp;
    if (!this->node_get(m_headers, strKey, strTemp))
    {
        return false;
    }
    if (urlDecode)
    {
        CHttpWrapper::urlDecode(strTemp, strVal);
    }
    else
    {
        strVal = strTemp;
    }
    return true;
}

CMyString CHttpData::getHeader(const CMyString &strKey, bool urlDecode)
{
    CMyString strTemp;
    this->getHeader(strKey, strTemp, urlDecode);
    return strTemp;
}

void CHttpData::setHeader(const CMyString &strKey, const CMyString &strVal)
{
	this->node_set(m_headers, strKey, strVal);
}

void CHttpData::removeHeader(const CMyString &strKey)
{
	this->node_remove(m_headers, strKey);
}

CMyString CHttpData::getCookie(const CMyString &strCookie)
{
	return this->node_get(m_cookies, strCookie);
}

void CHttpData::setCookie(const CMyString &strCookie, const CMyString &strVal)
{
	this->node_set(m_cookies, strCookie, strVal);
}

void CHttpData::removeCookie(const CMyString &strCookie)
{
	this->node_remove(m_cookies, strCookie);
}

void CHttpData::pack(CByteStream &data, bool bSetContentLength)
{
	static size_t s_maxLineLength = 1024;
	
	CMyString headers = this->node_encode(m_headers, ": ", "\r\n", s_maxLineLength);
	data.Write((void*)headers.c_str(), headers.size());
	
	if (!m_cookies.empty())
	{
		CMyString cookies_str = this->node_encode(m_cookies, "=", ";", s_maxLineLength);
        CMyString cookies_result;
        CHttpWrapper::urlDecode(cookies_str, cookies_result);
		CMyString cookie("Cookie: %s\r\n", cookies_result.c_str());
		data.Write((void*)cookie.c_str(), cookie.size());
	}
	
    if (bSetContentLength)
    {
	    uint32_t contLen = std::min(m_contentLength, (uint32_t)m_buffer.size());
    	CMyString contentLength("Content-Length: %u\r\n", contLen);
	    data.Write((void*)contentLength.c_str(), contentLength.size());
    	LogTrace("content-length %u", contLen);
    }

	CMyString strEndl("\r\n");
	data.Write((void*)strEndl.c_str(), strEndl.size());
	
	data.Write(m_buffer);
}

void CHttpData::parsePostData(void)
{
    if (m_buffer.empty())
    {
        LogWarn("buffer is empty.");
        return;
    }

    // 2020-02-20 处理完毕后，把 m_buffer 清空，留下 m_post
    uint32_t bufLen = m_buffer.size();
    uint32_t readSize = std::min(bufLen, m_contentLength);
    if (readSize >0)
    {
        m_buffer.Read(m_post, readSize);
    }
}

void CHttpData::parseChunkedData(void)
{
	if (m_buffer.empty())
	{
		return;
	}
    LogDebug("this is chunk data. buffer size:%lu\n", m_buffer.size());

	m_contentLength = 0;
	uint32_t bufLen = (int32_t)m_buffer.size();
	CTempBuffer tmpBuf(bufLen);
	m_buffer.SetPeekMode(true);
	m_buffer.Read((void*)tmpBuf.data(), bufLen);
	m_buffer.SetPeekMode(false);

    char *pc = tmpBuf.data();
    int32_t bufLeft = (int32_t)bufLen;
    CChunkBlock chunk;

    do {
        if (!chunk.Decode(pc, bufLeft))
        {
            chunk.dump();
            break;
        }
        
        if (chunk.IsFinished())
        {
            m_eState = stateChunkedEnd;
            break;
        }

        m_post.Write(chunk.getDataBlock(), chunk.getDataSize());
        chunk.dumpIndicator();
        chunk.dumpHeader();

        pc += chunk.getSize();
        bufLeft -= chunk.getSize();
        m_contentLength += chunk.getDataSize();
        LogDebug("buffer left:%d, contentLength:%d", bufLeft, m_contentLength);

        m_buffer.skip(chunk.getSize());
    }while(bufLeft>0);

    LogTrace("CHK>post buffer size:%lu, buffer left size:%lu", m_post.size(), m_buffer.size());
}

bool CHttpData::IsFinished(void)
{
    if (m_bIsChunked)
    {
        return (m_eState == stateChunkedEnd);
    }

    return (m_eState == stateOK);
}

/* 获取一行的结束的pos
 * 返回值:  -1 未找到
 *           0 首部就是
 *          >0 \r\n 的位置
 */
int32_t getEndlPos(char *pc, int32_t len, int32_t offset)
{
    uint8_t c1(0);
    uint8_t c2(0);
    int32_t pos(-1);
    for(int i=offset; i<len-1; ++i)
    {
        c1 = pc[i];
        if (c1==0x0D)
        {
            c2 = pc[i+1];
            if (c2==0x0A)
            {
                pos = i;
                break;
            }
        }
    }
    return pos;
}
bool CHttpData::readChunkedLine(char *pc, int32_t bufLen, int32_t &pos, int32_t &chunkSize)
{
    // 去掉首部的 \r\n
    char *pbuf = pc;
    int32_t offset(0);
	for(int32_t i(0); i<bufLen; ++i)
	{
		uint8_t c1 = pbuf[i];
		if (c1 == 0x0D || c1 == 0x0A)
		{
			continue;
		}
		offset = i;
		break;
	}
    pbuf += offset;
    LogTrace("CHK>offset=%d, pc=%p, pbuf=%p", offset, pc, pbuf);

    // 读取chunk 的长度行
    int32_t pos1 = getEndlPos(pc, bufLen-offset, offset);
    LogTrace("CHK>pos1:%d, offset:%d", pos1, offset);
    if (pos1 <= 0)
    {
        return false;
    }

    // 获得chunk的大小
    char szChunkHeader[64] = "";
    strncpy(szChunkHeader, pbuf, (pos1-offset));
    LogTrace("CHK>header:[%s]", szChunkHeader);
    if (strlen(szChunkHeader)==0)
    {
        return false;
    }
	chunkSize = this->parseChunkSize( szChunkHeader, strlen(szChunkHeader) );
	if (0 == chunkSize)
	{
	    LogImpt("CHK>chunk finished.");
		return true;
	}
	
    // 判断是否完整的line
    pos = pos1 + 2;
    LogTrace("CHK>bufLen %d, pos:%d, chunkSize:%d", bufLen, pos, chunkSize);
	if (bufLen >= pos + chunkSize)
	{
		return true;
	}
	return false;
}

int32_t CHttpData::parseChunkSize(char *pc, int32_t bufLen)
{
	//char buffer[1024] = "";
	//strncpy(buffer, pc, std::min((int32_t)(sizeof(buffer)-1), bufLen) );

	char szText[64] = "";
	strcpy(szText, "0x");

	char *pc1 = strchr(pc, ';');
	if (NULL == pc1)
	{
		strncpy(&szText[2], pc, bufLen);
	}
	else
	{
		strncpy(&szText[2], pc, (pc1 - pc));
	}
    LogTrace("CHK>chunk block size [%s]", szText);

	char *endstring;
	return strtol(szText, &endstring, 16);
}

bool CHttpData::IsStateValid(void) 
{ 
    return m_eState==stateOK || m_eState==statePending || m_eState==stateChunkedEnd; 
}

} 






