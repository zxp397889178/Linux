#include "DictMap.h"
#include <assert.h>

using namespace my_data;
CDictMap& CDictMap::Instance()
{
    static CDictMap* s_pObject = new CDictMap();

    return *s_pObject;
}

CDictMap::CDictMap()
    : m_dbDict()
    , m_pEvent(NULL)
{
    memset(m_szConfig, 0, sizeof(m_szConfig));
}

CDictMap::~CDictMap()
{

}

bool CDictMap::Init(IBucketEvent *pEvent, const char *pszConfig, ISimDB *pDB)
{
    m_pEvent = pEvent;
    safe_strcpy(m_szConfig, sizeof(m_szConfig), pszConfig);

    // 初始化数据库结构
    bool ret = m_dbDict.Init(m_szConfig, pDB);
    DB_ASSERT(ret);
    if (!ret)
        return false;

    return true;
}

bool CDictMap::Init(IBucketEvent *pEvent, const char *pszConfig)
{
    m_pEvent = pEvent;
    safe_strcpy(m_szConfig, sizeof(m_szConfig), pszConfig);

    return true;
}


CDictData* CDictMap::Load()
{
    CDictData* pDict = new CDictData();
    if (!pDict)
        return NULL;

    bool ret = pDict->Init(m_pEvent, m_szConfig);
    if (!ret)
    {
        SAFE_DELETE(pDict);
        return NULL;
    }

    ret = m_dbDict.LoadDict(*pDict);
    if (!ret)
    {
        SAFE_DELETE(pDict);
        return NULL;
    }

    return pDict;
}

// client use
CDictData* CDictMap::Load(byte_t *pData, size_t cbData, bool bCompressed)
{
    CDictData* pDict = new CDictData();
    if (!pDict->Init(m_pEvent, m_szConfig))
    {
        DB_ASSERT(0);
        SAFE_DELETE(pDict);
        return NULL;
    }

    int ret(0);
    if (bCompressed)
    {
        ret = pDict->UnpackDataEx(pData, cbData);
    }
    else
    {
        ret = pDict->UnpackData(pData, cbData);
    }

    if (ret < 0)
    {
        DB_ASSERT(0);
        SAFE_DELETE(pDict);
        return NULL;
    }

    return pDict;
}

