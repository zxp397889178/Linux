/*
 * =====================================================================================
 *
 *       Filename:  DictMap
 *
 *    Description:  字典对照表
 *
 *        Version:  1.0
 *        Created:  2014-4-20
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  wsk
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DA_DICTMAP_H_
#define _GFX_SERVER_DA_DICTMAP_H_

#include "DictDbData.h"
#include "DictData.h"

namespace my_data{

class CDictMap
{
public:
    static CDictMap& Instance();
    void release() { delete this; }   

    //Server
    bool Init(IBucketEvent *pEvent, const char *pszConfig, ISimDB *pDB);
    CDictData* Load();

    //Client
    bool Init(IBucketEvent *pEvent, const char *pszConfig);
    CDictData* Load(byte_t *pData, size_t cbData, bool bCompressed);

private:
    CDictMap();
    ~CDictMap();

private:
    CDBDictData     m_dbDict;
    char            m_szConfig[128];
    IBucketEvent*       m_pEvent;
};

}   // namespace my_data

#define DA_DICTMAP        my_data::CDictMap::Instance()
#define FREE_DA_DICTMAP   { my_data::CDictMap::Instance().release(); }

#endif  //_GFX_SERVER_DA_DICTMAP_H_

