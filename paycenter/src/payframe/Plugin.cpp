/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  plugin wrapper
 *
 *        Version:  1.0
 *        Created:  2019年09月11日 18时32分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Plugin.h"
#include <dlfcn.h>
#include "pcrecpp.h"

CPlugin::CPlugin()
    : m_pfnInit(NULL)
{
}

CPlugin::~CPlugin()
{
    if (m_slot[0].pHandler)
    {
        dlclose(m_slot[0].pHandler);
        m_slot[0].pHandler = NULL;
    }
    if (m_slot[1].pHandler)
    {
        dlclose(m_slot[1].pHandler);
        m_slot[1].pHandler = NULL;
    }
}

int32_t CPlugin::plugin_load_dll(struct PluginCfg &cfg, int32_t index, struct Slot &slot)
{
    slot.dll = cfg.dll[index];
    slot.pHandler = dlopen(slot.dll.c_str(), RTLD_NOW|RTLD_GLOBAL);
    if (NULL == slot.pHandler)
    {
        const char *err = dlerror();
        if (err)
        {
            LogFatal("load plugin %s faulure, error %s", slot.dll.c_str(), err);
        }
        else
        {
            LogFatal("load plugin %s failure.", slot.dll.c_str());
        }
        return -1001;
    }
    LogDebug("dll %s, addr=%p", slot.dll.c_str(), slot.pHandler);
    return 0;
}

int32_t CPlugin::plugin_init_dll(const CMyString &strInit, struct Slot &slot)
{
    if (strInit.empty())
    {
        return 0;
    }

    PLUGIN_INIT pfnCgiInit = (PLUGIN_INIT)dlsym(slot.pHandler, strInit.c_str());
    if (NULL == pfnCgiInit)
    {
        const char *err = dlerror();
        if (err)
        {
            LogFatal("load function [%s::%s] failure, erorr %s",
                    slot.dll.c_str(), strInit.c_str(), err);
        }
        else
        {
            LogFatal("load function [%s::%s] failure",
                    slot.dll.c_str(), strInit.c_str());
        }
        return -2001;
    }
    int32_t ret = pfnCgiInit(init_pluginInit);
    if (ret < 0)
    {
        LogFatal("call function [%s::%s] failure, ret:%d", 
                slot.dll.c_str(), strInit.c_str(), ret);
        return -2003;
    }

    m_pfnInit = pfnCgiInit;
    return 1;
}

int32_t CPlugin::plugin_run_dll(const CMyString &strRun, struct Slot &slot)
{
    slot.pMaintance = NULL;
    if (strRun.empty())
    {
        return 0;
    }

    PLUGIN_RUN pMaintance = (PLUGIN_RUN)dlsym(slot.pHandler, strRun.c_str());
    if (NULL == pMaintance)
    {
        const char *err = dlerror();
        if (err)
        {
            LogFatal("load function [%s::%s] failure, erorr %s",
                    slot.dll.c_str(), strRun.c_str(), err);
        }
        else
        {
            LogFatal("load function [%s::%s] failure",
                    slot.dll.c_str(), strRun.c_str());
        }
        return -2007;
    }
    slot.pMaintance = pMaintance;
    return 1;
}

int32_t CPlugin::plugin_load_cgifunc(PluginEntryData &item, VEC_ALIASE &vecAliase, struct Slot &slot)
{
    PCGIFUNC pFn = (PCGIFUNC)dlsym(slot.pHandler, item.funcName.c_str());
    if (NULL == pFn)
    {
        const char *err = dlerror();
        if (err)
        {
            LogFatal("load function [%s::%s] failure, error %s",
                    slot.dll.c_str(), item.funcName.c_str(), err);
        }
        else
        {
            LogFatal("load function [%s::%s] failure.",
                    slot.dll.c_str(), item.funcName.c_str());
        }
        return -3101;
    }

    LogDebug("load cgi [%s, addr=%p] from dll %s", item.cgiName.c_str(), pFn, slot.dll.c_str());
    std::pair< CGIFUNC_MAP::iterator, bool> result =
        slot.mapCgiFunc.insert(CGIFUNC_MAP::value_type(item.cgiName, pFn));
    if (!result.second)
    {
        LogErr("duplicate cgi entry %s in plugin %s.", 
                item.cgiName.c_str(), slot.dll.c_str());
        return 0;
    }

    // add aliase
    for(VEC_ALIASE::iterator it(vecAliase.begin()); it != vecAliase.end(); ++it)
    {
        struct AliaseCfg &aliase = *it;
        if (aliase.cgi == item.cgiName)
        {
            std::pair< CGIFUNC_MAP::iterator, bool> result2 =
                slot.mapCgiFunc.insert(CGIFUNC_MAP::value_type(aliase.cgi_aliase, pFn));
            if (!result2.second)
            {
                LogWarn("duplicate cgi [%s] aliase [%s], IGNORE", 
                        item.cgiName.c_str(), aliase.cgi_aliase.c_str());
            }
            else
            {
                LogDebug("cgi [%s] aliase [%s], func %p", 
                        item.cgiName.c_str(), aliase.cgi_aliase.c_str(), pFn);
            }
        }
    }
    return 1;
}

int32_t CPlugin::plugin_load_msgfunc(PluginEntryData &item, struct Slot &slot)
{
    PMSGFUNC pFn = (PMSGFUNC)dlsym(slot.pHandler, item.funcName.c_str());
    if (NULL == pFn)
    {
        const char *err = dlerror();
        if (err)
        {
            LogFatal("load function [%s::%s] failure, error %s",
                    slot.dll.c_str(), item.funcName.c_str(), err);
        }
        else
        {
            LogFatal("load function [%s::%s] failure.",
                    slot.dll.c_str(), item.funcName.c_str());
        }
        return -3201;
    }

    LogDebug("load msg [%u] from dll %s", item.msgId, slot.dll.c_str());
    std::pair< MSGFUNC_MAP::iterator, bool> result =
        slot.mapMsgFunc.insert(MSGFUNC_MAP::value_type(item.msgId, pFn));
    if (!result.second)
    {
        LogErr("duplicate msg entry %u in plugin %s.", 
                item.msgId, slot.dll.c_str());
        return 0;
    }
    return 1;
}

int32_t CPlugin::plugin_load_entry(const CMyString &strEntry, VEC_ALIASE &vecAliase, struct Slot &slot)
{
    if (strEntry.empty())
    {
        return 0;
    }

    // 调和函数入口表
    PLUGIN_REG pfnReg = (PLUGIN_REG)dlsym(slot.pHandler, strEntry.c_str());
    if (NULL == pfnReg)
    {
        const char *err = dlerror();
        if (err)
        {
            LogFatal("load function [%s::%s] failure, erorr %s",
                    slot.dll.c_str(), strEntry.c_str(), err);
        }
        else
        {
            LogFatal("load function [%s::%s] failure",
                    slot.dll.c_str(), strEntry.c_str());
        }
        return -3001;
    }

    // 获得插件类的导出函数指针
    ENTRY_VECTOR vecEntry;
    pfnReg(vecEntry);
    
    ENTRY_VECTOR::iterator it(vecEntry.begin());
    for(; it != vecEntry.end(); ++it)
    {
        PluginEntryData &item = *it;
        if (0==item.msgId)
        {
            plugin_load_cgifunc(item, vecAliase, slot);
        }
        else
        {
            plugin_load_msgfunc(item, slot);
        } // if (0==item.msgId)
    } // for(ENTRY_VECTOR::...
    return 0;
}

bool CPlugin::Init(struct PluginCfg &cfg)
{
    m_pluginName = cfg.name;

    struct Slot &slot = m_slot[0];
    int32_t ret(0);
    try
    {
        ret = plugin_load_dll(cfg, 0, slot);
        if (ret < 0)
        {
            return false;
        }

        // 调用init函数
        ret = plugin_init_dll(cfg.init_func, slot);
        if (ret < 0)
        {
            return false;
        }

        // 调用所有函数入口
        ret = plugin_load_entry(cfg.entry_func, cfg.vecAliase, slot);
        if (ret < 0)
        {
            return false;
        }

        // 调用定时处理函数
        ret = plugin_run_dll(cfg.run_func, slot);
        if (ret < 0)
        {
            return false;
        }
    }
    catch(CException &e)
    {
        LogFatal("[%s:%s:%d]init plugin %s failure, %s",
                __FILE__, __func__, __LINE__, m_pluginName.c_str(), e.what());
        return false;
    }
    catch(...)
    {
        LogFatal("[%s:%s:%d]init plugin %s failures",
                __FILE__, __func__, __LINE__, m_pluginName.c_str());
        return false;
    }
    LogImpt("init plugin %s succeed.", m_pluginName.c_str());

    return true;
}


int32_t CPlugin::Update(void)
{
    if (NULL != m_pfnInit)
    {
        int32_t ret = m_pfnInit(init_pluginReloadCfg);
        if (ret < 0)
        {
            LogErr("[%s:%s:%d]reinit plugin [%s] FAILURE.", 
                    __FILE__, __func__, __LINE__, m_pluginName.c_str());
            return -1;
        }
        LogImpt("reinit plugin [%s] SUCCEED.", m_pluginName.c_str());
        return 1;
    }
    return 0;
}

CPlugin::Slot& CPlugin::getNewerSlot()
{
    if (m_slot[1].pHandler)
    {
        return m_slot[1];
    }
    return m_slot[0];
}

PCGIFUNC CPlugin::getCgiFunc(const CMyString &cgi)
{
    PCGIFUNC pfnCgi(NULL);

    struct Slot &slot = getNewerSlot();
    CGIFUNC_MAP::iterator it(slot.mapCgiFunc.find(cgi));
    if (it == slot.mapCgiFunc.end())
    {
        // 利用正则表达式来查找是否有匹配的
        it = slot.mapCgiFunc.begin();
        for(; it != slot.mapCgiFunc.end(); ++it)
        {
            CMyString strCgiTemp = it->first;
            pcrecpp::RE oPattern(strCgiTemp);
            if (oPattern.FullMatch(cgi))
            {
                pfnCgi = it->second;
                break;
            }
        }
    }
    else
    {
        pfnCgi = it->second;
    }
    return pfnCgi;
}

PMSGFUNC CPlugin::getMsgFunc(uint32_t msgId)
{
    struct Slot &slot = getNewerSlot();
    MSGFUNC_MAP::iterator it(slot.mapMsgFunc.find(msgId));
    if (it == slot.mapMsgFunc.end())
    {
        return NULL;
    }
    return it->second;
}

void CPlugin::Maintance(void)
{
    struct Slot &slot = this->getNewerSlot();
    if (slot.pMaintance)
    {
        slot.pMaintance();
    }
}

