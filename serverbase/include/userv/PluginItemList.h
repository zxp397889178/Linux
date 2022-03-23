/*
 * =====================================================================================
 *
 *       Filename:  PluginItemList.h
 *
 *    Description:  插件的配置信息
 *
 *        Version:  1.0
 *        Created:  2021年08月09日 23时59分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "BaseCode.h"
#include <map>
#include <vector>

struct PluginItem
{
    CMyString           name;           // 插件别名
    CMyString           soFile[2];      // 插件的文件名
    CMyString           initFunc;       // 插件的初始化程序
    CMyString           configXml;      // 插件所使用xml文件，空则用系统config.xml
    CMyString           section;        // xml对应的section

    PluginItem() 
        : name(), initFunc(), configXml(), section()
    {
    }
    bool Equal(struct PluginItem &rhs)
    {
        return (name==rhs.name) && 
            (soFile[0]==rhs.soFile[0]) && (soFile[1]==rhs.soFile[1]) && 
            (configXml==rhs.configXml) && (section==rhs.section);
    }
    void toJson(Json::Value &plugin_arr);
};
typedef std::vector<PluginItem>     VEC_PLUGIN;

class CPluginItemList
{
public:
    CPluginItemList()  {  }
    ~CPluginItemList() {  }

    int32_t Init(CXmlFile &xml);
    PluginItem* FindByName(const CMyString &name);
    int32_t GetPluginList(VEC_PLUGIN &vecPlugin);
    void toJson(Json::Value &plugin_arr);

    int32_t Update(const CMyString &soName, int32_t slotId, const CMyString &soFile);

private:
    int32_t ParsePlugin(CXmlFile &xml, void *pItem);

private:
    typedef std::map<CMyString, PluginItem>     MAP_PLUGIN;
    MAP_PLUGIN      m_mapPlugin;
};


