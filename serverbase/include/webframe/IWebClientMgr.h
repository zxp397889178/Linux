/*
 * =====================================================================================
 *
 *       Filename:  IWebClientMgr.h
 *
 *    Description:  client manager
 *
 *        Version:  1.0
 *        Created:  2019年09月11日 22时37分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __WEBFRAME_ICLIENTMGR_H__
#define __WEBFRAME_ICLIENTMGR_H__

#include "BaseCode.h"
#include "MyNet.h"

class IClient;
typedef std::map< CMyString, CMyString >    STRING_MAP;
class IWebClientMgr
{
public:
    virtual ~IWebClientMgr() {  }

    virtual int32_t ExecuteCGI(const CMyString& strCgi, 
            http::CHttpRequest& req, char *szResp, uint32_t &cbResp,
            STRING_MAP& ext_header, 
            IClient* pClient) 
    {
        return -1;
    }
};

#endif // __WEBFRAME_ICLIENTMGR_H__

