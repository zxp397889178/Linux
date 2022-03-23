#include "MyFieldSet.h"
#include "MyField.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//MYPOOL_EX_IMPLEMENTATION(CMyFieldSet, s_objHeap)

//////////////////////////////////////////////////////////////////////////
//CMyFieldSet
CMyFieldSet::CMyFieldSet()
    : m_arFields()
    , m_mapFieldByName()
{
}

CMyFieldSet::~CMyFieldSet(void)
{
    this->Reset();
}

void CMyFieldSet::Init(MYSQL_FIELD *pFields, uint8_t nFieldCount)
{
    //构建CMyField数据
    for(uint8_t nIndex = 0; nIndex < nFieldCount; ++nIndex)
    {
        CMyField *pFieldObj = new CMyField(&pFields[nIndex]);
        //CMyField *pFieldObj = new CMyField(&pFields[nIndex], nIndex);
        DB_ASSERT(pFieldObj != NULL);
        if (NULL == pFieldObj)
            continue;
        
        m_arFields.push_back(pFieldObj);
        m_mapFieldByName[pFieldObj->GetName()] = pFieldObj;
    }
}

void CMyFieldSet::Reset()
{
    m_mapFieldByName.clear();

    for(FIELDS::iterator it(m_arFields.begin()); it != m_arFields.end(); ++it)
    {
        IMyField *pFieldObj = *it;
        SAFE_RELEASE(pFieldObj);
    }
    m_arFields.clear();
}


IMyField* CMyFieldSet::Find(uint8_t nIndex)
{
    return this->operator[](nIndex);
}

IMyField* CMyFieldSet::operator[](int32_t nIndex)
{
    if (nIndex<0 || nIndex>=(int)this->FieldCount())
    {
        return NULL;
    }
    return m_arFields[nIndex];
}

IMyField* CMyFieldSet::FindByName(const char *pszName)
{
    FIELD_MAP_BY_NAME::iterator it(m_mapFieldByName.find(pszName));
    if (it != m_mapFieldByName.end())
    {
        return it->second;
    }
    return NULL;
}

int32_t CMyFieldSet::FieldIndex(const char *pszName)
{
    if (EMPTY_STR(pszName))
    {
        return -1;
    }

    FIELDS::iterator it(m_arFields.begin());
    for(int32_t nIndex=0; it != m_arFields.end(); ++it, ++nIndex)
    {
        IMyField *pField = *it;
        if (!strcmp(pField->GetName(), pszName))
        {
            return nIndex;
        }
    }
    return -1;
}

size_t CMyFieldSet::FieldCount(void) const
{
    return m_arFields.size();
}

bool CMyFieldSet::HasPrimaryField(void)
{
    bool bFind(false);
    for(FIELDS::iterator it(m_arFields.begin()); it != m_arFields.end(); ++it)
    {
        IMyField *pFieldObj = *it;
        if (NULL != pFieldObj)
        {
            if (pFieldObj->IsPrimaryKeyField())
            {
                bFind = true;
                break;
            }
        }
    }
    return bFind;
}

size_t CMyFieldSet::GetPrimaryField(std::vector<IMyField*> &arKeyField)
{
    for(FIELDS::iterator it(m_arFields.begin()); it != m_arFields.end(); ++it)
    {
        IMyField *pFieldObj = *it;
        if (NULL != pFieldObj)
        {
            if (pFieldObj->IsPrimaryKeyField())
            {
                arKeyField.push_back(pFieldObj);
            }
        }
    }
    return arKeyField.size();
}

size_t CMyFieldSet::GetPrimaryFieldIndex(std::vector<int> &arKeyField)
{
    int32_t nIndex(0);
    for(FIELDS::iterator it(m_arFields.begin()); it != m_arFields.end();
        ++it, ++nIndex)
    {
        IMyField *pFieldObj = *it;
        if (NULL != pFieldObj)
        {
            if (pFieldObj->IsPrimaryKeyField())
            {
                arKeyField.push_back(nIndex);
            }
        }
    }
    return arKeyField.size();
}
