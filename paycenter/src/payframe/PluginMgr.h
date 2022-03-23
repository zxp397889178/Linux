/*
 * =====================================================================================
 *
 *       Filename:  PluginMgr.h
 *
 *    Description:  plugin manager
 *
 *        Version:  1.0
 *        Created:  2019年09月11日 21时25分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __WEBFRAME_PLUGINMGR_H__
#define __WEBFRAME_PLUGINMGR_H__

#include "BaseCode.h"
#include "Plugin.h"
#include <map>

typedef std::map<CMyString, CPlugin*>   PLUGIN_MAP;
class CPluginMgr
{
public:
    CPluginMgr();
    ~CPluginMgr();

    bool Init(void);

    int32_t Update(void);

    bool Maintance(void);

    PCGIFUNC getCgiFunc(const CMyString &cgi);
    PMSGFUNC getMsgFunc(uint32_t msgId);

private:
    PLUGIN_MAP      m_mapPlugin;
};

#endif // __WEBFRAME_PLUGINMGR_H__

