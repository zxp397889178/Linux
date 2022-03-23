/*
 * =====================================================================================
 *
 *       Filename:  HttpRespone.h
 *
 *    Description:  response HTTP
 *
 *        Version:  1.0
 *        Created:  05/22/2015 09:34:27 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_NETWORK_HTTP_RESPONE_H_
#define _QW_CRAZYFISH_NETWORK_HTTP_RESPONE_H_

#include "HttpData.h"

namespace http { 

class CHttpRespone : public CHttpData
{
public:
	CHttpRespone();
	~CHttpRespone();

	virtual void clear(void);
	virtual void pack(CByteStream &data, bool bSetContentLength);

	int32_t getStatusCode(void) const { return m_nStatus; }
	void setStatusCode(int32_t val)   { m_nStatus = val; };
	const char* getStatus(void) const { return m_strStatus.c_str(); }
	void setStatus(const char* val)   { m_strStatus = val; }

	void setContentLength(uint32_t val) { m_contentLength = val; }

public:
	void getPost(CByteStream &buff);

protected:
	virtual int32_t onHeaderBegin(const CMyString &strLine);
	virtual void    onHeaderEnd(void);

private:
	int32_t		m_nStatus;
	CMyString	m_strStatus;
};

} // nameapce http

#endif // _QW_CRAZYFISH_NETWORK_HTTP_RESPONE_H_

