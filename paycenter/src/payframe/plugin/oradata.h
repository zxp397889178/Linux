/*
 * =====================================================================================
 *
 *       Filename:  oradata.h
 *
 *    Description:  game api write to oracle
 *
 *        Version:  1.0
 *        Created:  2019年09月12日 18时19分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __WEBFRAME_PLUGIN_ORADATA_H__
#define __WEBFRAME_PLUGIN_ORADATA_H__

#include "BaseCode.h"
#include "MyDB.h"
#include "../IWebPlugin.h"
#include <typeinfo>

class COraData
{
public:
    COraData();
    ~COraData();

    bool Init(void);

    ENTRY_FUNC_DEFINE(doUserLogin)

private:
    ISimDB*     m_pDB;
};

#endif // __WEBFRAME_PLUGIN_ORADATA_H__

