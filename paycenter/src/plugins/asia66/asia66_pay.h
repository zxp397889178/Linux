/*
 * =====================================================================================
 *
 *       Filename:  asia66_pay.h
 *
 *    Description:  亚洲娱乐支付
 *
 *        Version:  1.0
 *        Created:  2020年05月16日 20时37分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "../PayWrapper.h"
#include "IWebPlugin.h"

class CPluginAsia66Pay : public CPayWrapper
{
public:
    CPluginAsia66Pay();
    virtual ~CPluginAsia66Pay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo);
};


