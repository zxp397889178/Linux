#include "DaFieldTable.h"
#include <string>
#include <map>
#include <assert.h>

namespace my_data{

class CFieldSetMap : public std::map<std::string, CFieldSet* >
{
};

}   //namespace my_data

using namespace my_data;

CFieldTable::CFieldTable()
    : m_pFieldsetMap(new CFieldSetMap())
    , m_pUniqueNameMap(new CFieldSetMap() )
{
    DB_ASSERT(m_pFieldsetMap);
    DB_ASSERT(m_pUniqueNameMap);
}

CFieldTable::~CFieldTable()
{
    this->Destroy();
    SAFE_DELETE(m_pFieldsetMap);
    SAFE_DELETE(m_pUniqueNameMap);
}

CFieldSet* CFieldTable::LoadTable(const char *pszPath, const char *pszTable)
{
    //判断是否已经装载成功
    CFieldSet *pFieldset = this->Find(pszTable);
    if (pFieldset != NULL)
    {
        return pFieldset;
    }

    //从pszTable 定义中装载表数据
	char szFile[256] = "";
	safe_sprintf(szFile, sizeof(szFile), "%s/%s", pszPath, pszTable);
    pFieldset = new CFieldSet(szFile);
    if (NULL == pFieldset || !pFieldset->IsValid())
    {
        delete pFieldset;
        return NULL;
    }
    
    //置入映射表
    m_pFieldsetMap->insert(std::make_pair(pszTable, pFieldset));
    m_pUniqueNameMap->insert(std::make_pair(pFieldset->GetUniqueName(), pFieldset));
    return pFieldset;
}

CFieldSet* CFieldTable::Find(const char *pszTable)
{
    CFieldSetMap::iterator it(m_pFieldsetMap->find(pszTable));
    if (it == m_pFieldsetMap->end())
    {
        return NULL;
    }
    return it->second;
}

void CFieldTable::Destroy()
{
    m_pFieldsetMap->clear();

    CFieldSetMap::iterator it(m_pFieldsetMap->begin());
    for(; it != m_pFieldsetMap->end(); ++it)
    {
        CFieldSet *pFieldset = it->second;
        SAFE_RELEASE(pFieldset);
    }
    m_pFieldsetMap->clear();
}

CFieldTable& CFieldTable::Instance()
{
    static CFieldTable s_table;
    return s_table;
}

const char* CFieldTable::GetUniqueName(const char *pszPath, const char *pszTable)
{
    CFieldSet* pFieldSet = this->LoadTable(pszPath, pszTable);
    if (NULL == pFieldSet)
    {
        return NULL;
    }
    return pFieldSet->GetUniqueName();
}

CFieldSet* CFieldTable::FindByUniqueName(const char *pszUniqueName)
{
    CFieldSetMap::iterator it(m_pUniqueNameMap->find(pszUniqueName));
    if (it == m_pUniqueNameMap->end())
    {
        return NULL;
    }
    CFieldSet *pFieldSet = it->second;
    return pFieldSet;
}
