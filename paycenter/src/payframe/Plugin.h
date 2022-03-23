/*
 * =====================================================================================
 *
 *       Filename:  Plugin.h
 *
 *    Description:  plugin agent
 *
 *        Version:  1.0
 *        Created:  2019年09月11日 18时21分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __WEBFRAME_PLUGIN_H__
#define __WEBFRAME_PLUGIN_H__

#include "BaseCode.h"
#include "IWebPlugin.h"
#include "Config.h"

class CPlugin
{
    struct Slot
    {
        void*           pHandler;
        CMyString       dll;
        CGIFUNC_MAP     mapCgiFunc;
        MSGFUNC_MAP     mapMsgFunc;
        PLUGIN_RUN      pMaintance;

        Slot() : pHandler(NULL), pMaintance(NULL) {}
    };
public:
    CPlugin();
    ~CPlugin();

    bool Init(struct PluginCfg &cfg);
    // 检查config.xml中的dll 是否有变更
    int32_t Update();

    void Maintance(void);

    PCGIFUNC getCgiFunc(const CMyString &cgi);
    PMSGFUNC getMsgFunc(uint32_t msgId);

    const char* getName(void) { return m_pluginName.c_str(); }
private:
    int32_t plugin_load_dll(struct PluginCfg &cfg, int32_t index, struct Slot &slot);
    int32_t plugin_init_dll(const CMyString &strInit, struct Slot &slot);
    int32_t plugin_run_dll(const CMyString &strRun, struct Slot &slot);
    int32_t plugin_load_entry(const CMyString &strEntry, VEC_ALIASE &vecAliase, struct Slot &slot);
    int32_t plugin_load_cgifunc(PluginEntryData &item, VEC_ALIASE &vecAliase, struct Slot &slot);
    int32_t plugin_load_msgfunc(PluginEntryData &item, struct Slot &slot);

    struct Slot& getNewerSlot();

private:
    CMyString       m_pluginName;
    Slot            m_slot[2];

    VEC_ALIASE      m_vecAliase;

    PLUGIN_INIT     m_pfnInit;
};

#endif //__WEBFRAME_PLUGIN_H__ 


