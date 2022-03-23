/*
 * =====================================================================================
 *
 *       Filename:  PluginMgr.cpp
 *
 *    Description:  plugin manager
 *
 *        Version:  1.0
 *        Created:  2019年09月11日 21时29分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "PluginMgr.h"
#include "Config.h"

CPluginMgr::CPluginMgr()
{

}

CPluginMgr::~CPluginMgr()
{
    PLUGIN_MAP::iterator it(m_mapPlugin.begin());
    for(; it != m_mapPlugin.end(); ++it)
    {
        CPlugin *pPlugin = it->second;
        if (NULL != pPlugin)
        {
            SAFE_DELETE(pPlugin);
        }
    }
    m_mapPlugin.clear();
}

bool CPluginMgr::Init()
{
    int32_t cnt = CConfig::Instance().getPluginCount();
    LogDebug("load plugin, total count %d", cnt);
    for(int32_t i=0; i<cnt; ++i)
    {
        PluginCfg pcfg;
        int32_t ret = CConfig::Instance().getPlugin(i, pcfg);
        if (ret <0)
        {
            continue;
        }

        CPlugin *plugin = new CPlugin();
        if (NULL == plugin)
        {
            continue;
        }

        try
        {
            if (!plugin->Init(pcfg))
            {
                return false;
            }
        }
        catch(CException &e)
        {
            LogFatal("[%s:%s:%d]init plugin %s failure, %s",
                    __FILE__, __func__, __LINE__, pcfg.name.c_str(), e.what()); 
            return false;
        }
        catch(...)
        {
            LogFatal("[%s:%s:%d]init plugin %s failure",
                    __FILE__, __func__, __LINE__, pcfg.name.c_str()); 
            return false;
        }
        m_mapPlugin.insert(std::make_pair(pcfg.name, plugin));
        LogDebug("add plugin (%s, %p)", pcfg.name.c_str(), plugin);
    }

    return true;
}

int32_t CPluginMgr::Update(void)
{
    // 对所有pluign 进行重载
    int32_t cnt(0);
    PLUGIN_MAP::iterator it(m_mapPlugin.begin());
    for(; it != m_mapPlugin.end(); ++it)
    {
        CPlugin *plugin = it->second; 
        if (NULL == plugin)
        {
            continue;
        }
        if (plugin->Update()>=0)
        {
            ++cnt;
        }
    }
    return cnt;
}

PCGIFUNC CPluginMgr::getCgiFunc(const CMyString &cgi)
{
    PLUGIN_MAP::iterator it(m_mapPlugin.begin());
    for(; it != m_mapPlugin.end(); ++it)
    {
        CPlugin *plugin = it->second;
        if (!plugin)
        {
            continue;
        }
        PCGIFUNC pFn = plugin->getCgiFunc(cgi);
        if (NULL==pFn)
        {
            continue;
        }
        return pFn;
    }
    return NULL;
}

PMSGFUNC CPluginMgr::getMsgFunc(uint32_t msgId)
{
    PLUGIN_MAP::iterator it(m_mapPlugin.begin());
    for(; it != m_mapPlugin.end(); ++it)
    {
        CPlugin *plugin = it->second;
        if (!plugin)
        {
            continue;
        }
        PMSGFUNC pFn = plugin->getMsgFunc(msgId);
        if (NULL == pFn)
        {
            continue;
        }
        return pFn;
    }
    return NULL;
}

bool CPluginMgr::Maintance(void)
{
    bool result(true);
    PLUGIN_MAP::iterator it(m_mapPlugin.begin());
    for(; it != m_mapPlugin.end(); ++it)
    {
        CPlugin *plugin = it->second;
        if (!plugin)
        {
            continue;
        }
        //LogDebug("DBG>step 1: begin to enter maintance()");
        try
        {
            plugin->Maintance();
        }
        catch(CException &e)
        {
            LogFatal("[%s:%s:%d]call plugin %s maintance failure, %s",
                    __FILE__, __func__, __LINE__, plugin->getName(), e.what());
            result = false;
            break;
        }
        catch(...)
        {
            LogFatal("[%s:%s:%d]call plugin %s maintance failure",
                    __FILE__, __func__, __LINE__, plugin->getName());
            result = false;
            break;
        }
    }
    return result;
}


