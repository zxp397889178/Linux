/*
 * =====================================================================================
 *
 *       Filename:  PayManager.h
 *
 *    Description:  支付渠道选管理
 *
 *        Version:  1.0
 *        Created:  01/01/2020 01:09:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "MyNet.h"
#include "MyDB.h"
#include <vector>

#include "GameList.h"
#include "SdkList.h"
#include "SdkChannelList.h"
#include "ChannelList.h"
#include "BillPointList.h"
#include "BillpointDataList.h"
#include "VipList.h"
#include "Dict_ClientType.h"
#include "Dict_PayStyle.h"
/* config 中需要以下配置
<main>
    <paycenter>
        <cgi_pay cgi="pay"/>
        <cgi_reload cgi="api/reload" validIp="127.0.0.1" />
        <db main="paycenter" userinfo="userinfo" redis="redis" />
    </paycenter>
</main>
 */

class IClient;
class CPayManager
{
public:
    CPayManager();
    ~CPayManager();

    int32_t Init(int32_t workerId);

    int32_t GetChannel(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, IClient *pClient);

    int32_t GetWealthStat(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, IClient *pClient);

    int32_t QueryCgi(http::CHttpRequest &req, 
        char *szResp, uint32_t &cbResp, CMyString &strCgiNew);

    int32_t QuerySignCgi(http::CHttpRequest &req, 
        char *szResp, uint32_t &cbResp, CMyString &strCgiNew);
    
    int32_t QueryVerifyCgi(http::CHttpRequest &req, 
        char *szResp, uint32_t &cbResp, CMyString &strCgiNew);

private:
    CSdkChannel* selectChannel(std::vector<CSdkChannel*> &vecChannel, int32_t totalWeight);

    void OnPayFailure(int32_t code, const char *msg, char *szResp, uint32_t &cbResp);
    
    int32_t VerifySign(http::CHttpRequest &req, char *szResp, uint32_t &cbResp);

    // 返回值: 0  表示无计费点配置
    //         >0 指定bpId
    int32_t selectBillpoint(int32_t configId, int64_t money, const CMyString &uin);

private:
    bool                m_bNeedSign;

    ISimDB*             m_pDB;
    int32_t             m_workerId;
};


