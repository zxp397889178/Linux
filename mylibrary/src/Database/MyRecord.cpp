#include "MyRecord.h"
#include "MyData.h"
#include <assert.h>

//MYPOOL_EX_IMPLEMENTATION(CMyRecord, s_objHeap)
//////////////////////////////////////////////////////////////////////////
//
CMyRecord::CMyRecord(CMyFieldSet* pFieldSet, MYSQL_ROW row, uint32_t *lengths)
    : m_data()
    , m_opFlag(recordOpSelect)
    , m_pFieldSet(pFieldSet)
{
    if (!pFieldSet || NULL==lengths)
        return;

    int32_t nFieldCnt = pFieldSet->FieldCount();
    for (uint8_t nIndex = 0; nIndex < nFieldCnt; ++nIndex)
    {
        IMyField* pField = pFieldSet->Find(nIndex);
        DB_ASSERT(pField);
        if (NULL == pField)
            continue;

        CMyData *pData = new CMyData((byte_t*)row[nIndex], lengths[nIndex], pField);
        if (NULL == pData)
            continue;

        m_data.push_back(pData);
    }
}

CMyRecord::CMyRecord(CMyFieldSet* pFieldSet)
    : m_data()
    , m_opFlag(recordOpInsert)
    , m_pFieldSet(pFieldSet)
{
    if (!pFieldSet)
    {
        return;
    }

    int32_t nFieldCnt = pFieldSet->FieldCount();
    for (uint8_t nIndex = 0; nIndex < nFieldCnt; ++nIndex)
    {
        IMyField* pField = pFieldSet->Find(nIndex);
        DB_ASSERT(pField);
        if (NULL == pField)
            continue;

        CMyData *pData = new CMyData(pField);
        if (NULL == pData)
            continue;

        m_data.push_back(pData);
    }
}

CMyRecord::CMyRecord()
    : m_data()
    , m_opFlag(recordOpDummy)
    , m_pFieldSet(NULL)
{
     DB_ASSERT(0);
}

CMyRecord::~CMyRecord()
{
    MY_DATA::iterator it(m_data.begin());
    for(; it != m_data.end(); ++it)
    {
        IMyData *pData = *it;
        SAFE_RELEASE(pData);
    }
    m_data.clear();
}

IMyData* CMyRecord::Field(int32_t nIndex)
{
    if (nIndex>=(int32_t)m_data.size() || nIndex<0)
    {
        return NULL;
    }
    return m_data.at(nIndex);
}

IMyData* CMyRecord::Field(const char *pszName)
{
    int32_t nIndex = m_pFieldSet->FieldIndex(pszName);
    if (-1 == nIndex)
    {
        return NULL;
    }
    return this->Field(nIndex);
}


IMyData* CMyRecord::operator[](int32_t nIndex)
{
    return this->Field(nIndex);
}

size_t CMyRecord::FieldCount(void) const
{
    return m_data.size();
}

void CMyRecord::release(void)
{
    delete this;
}

bool CMyRecord::IsChanged(void)
{
    bool bChanged(false);
    MY_DATA::iterator it(m_data.begin());
    for(; it != m_data.end(); ++it)
    {
        IMyData *pData = *it;
        DB_ASSERT(pData);
        if (!pData) continue;
        switch(pData->getFlag())
        {
        case dataOpDummy:
        case dataOpSelect:
            break;
        default:
            bChanged = true;
            break;
        }

        if (bChanged)
        {
            break;
        }
    }
    return bChanged;
}

bool CMyRecord::IsValid(void)
{
    return (m_opFlag != recordOpDelete);
}


int32_t CMyRecord::Delete(void)
{
    if (m_opFlag != recordOpInsert)
    {
        m_opFlag = recordOpDelete;
        return 1;
    }
    delete this;
    return 0;
}

bool CMyRecord::PreProcessInsert()
{
    if (m_opFlag != recordOpInsert)
    {
        return false;
    }
    //检查主键是否有赋值
    int32_t nKeys(0);
    std::vector<int> arField;
    m_pFieldSet->GetPrimaryFieldIndex(arField);
    for(std::vector<int>::iterator iter(arField.begin()); 
        iter != arField.end(); ++iter)
    {
        int32_t nIndex = *iter;
        IMyData *pData = m_data.at(nIndex);
        if (NULL == pData)
            continue;

        if (pData->getFlag() != dataOpInsert)
        {
            break; //todo: ghfghfghf 如果所有的主键没都赋初始值执行时会出错
        }
        ++nKeys;
    }
    if (nKeys <= 0)
    {
        return false;
    }

    //判断是否需要增加进数据库
    int32_t nInsert(0);
    for(MY_DATA::iterator it(m_data.begin()); it != m_data.end(); ++it)
    {
        IMyData *pData = *it;
        if (!pData) continue;
        if (pData->getFlag()==dataOpInsert)
        {
            ++nInsert;
        }
    }
    return (nInsert>0);
}

bool CMyRecord::PreProcessSelect(void)
{
    if (m_opFlag != recordOpSelect)
    {
        return false;
    }
    //判断是否有修改
    int32_t nUpdate(0);
    for(MY_DATA::iterator it(m_data.begin()); it != m_data.end(); ++it)
    {
        IMyData *pData = *it;
        if (!pData) continue;
        switch(pData->getFlag())
        {
        case dataOpUpdate:
        //case dataOpDelete:
        case dataOpInsert:
            ++nUpdate;
            break;
        default:
            break;
        }
        if (nUpdate > 0)
        {
            m_opFlag = recordOpUpdate;
            break;
        }
    }
    return (nUpdate>0);
}

bool CMyRecord::PreProcessDelete(void)
{
    return true;
}

bool CMyRecord::PreProcessUpdate(void)
{
    return true;
}

bool CMyRecord::PreProcessRecord(void)
{
    bool ret(false);
    switch(m_opFlag)
    {
    case recordOpSelect:
        ret = this->PreProcessSelect();
        break;
    case recordOpInsert:
        ret = this->PreProcessInsert();
        break;
    case recordOpDelete:
        ret = this->PreProcessDelete();
        break;
    case recordOpUpdate:
        ret = this->PreProcessUpdate();
        break;
    default:
        break;
    }
    return ret;
}

const char* CMyRecord::GetTableName(void)
{
    if (NULL == m_pFieldSet)
        return "";

    IMyField *pField = m_pFieldSet->Find(0);
    DB_ASSERT(pField);
    if (NULL == pField)
        return "";

    return pField->GetTable();
}

bool CMyRecord::FormatDeleteSQL(CMyString &strSql)
{
    if (NULL == m_pFieldSet)
        return false;

    std::vector<int> arField;
    size_t cnt = m_pFieldSet->GetPrimaryFieldIndex(arField);
    if (cnt==0)
    {
        return false;
    }

    strSql.Format("DELETE FROM %s WHERE ", this->GetTableName());
    int32_t nKeyField(0);
    for(std::vector<int>::iterator it(arField.begin()); 
        it != arField.end(); ++it, ++nKeyField)
    {
        uint8_t nIndex = (uint8_t)(*it);
        IMyField *pField = m_pFieldSet->Find(nIndex);
        IMyData *pData = m_data.at(nIndex);
        if (nKeyField > 0)
        {
            strSql.Append(" AND ");
        }
        if (pField->IsNumberField())
            strSql.Append("%s=%u", pField->GetName(), pData->toUInt());
        else if (pField->IsNumericField() || pField->IsDoubleField())
            strSql.Append("%s=%lf", pField->GetName(), pData->toDouble());
        else
            strSql.Append("%s='%s'", pField->GetName(), pData->toString());
    }

    return true;
}

bool CMyRecord::FormatInsertSQL(CMyString &strSql)
{
    if (NULL == m_pFieldSet)
        return false;

    strSql.Format("INSERT INTO %s (", this->GetTableName());

    uint8_t nIndex(0);
    for(; nIndex<m_pFieldSet->FieldCount(); ++nIndex)
    {
        IMyField *pField = m_pFieldSet->Find(nIndex);
        DB_ASSERT(pField);
        if (nIndex>0)
        {
            strSql.Append(",");
        }
        strSql.Append("%s", pField->GetName());
    }
    strSql.Append(") VALUES(");

    nIndex = 0;
    for(MY_DATA::iterator it(m_data.begin()); it != m_data.end(); 
        ++it, ++nIndex)
    {
        IMyData *pData = *it;
        IMyField *pField = m_pFieldSet->Find(nIndex);
        if (nIndex>0)
        {
            strSql.Append(",");
        }
        if (pField->IsNumberField())
            strSql.Append("%u", pData->toUInt());
        else if (pField->IsNumericField() || pField->IsDoubleField())
            strSql.Append("%lf", pData->toDouble());
        else
            strSql.Append("'%s'", pData->toString());
    }
    strSql.Append(")");

    return true;
}

bool CMyRecord::FormatUpdateSQL(CMyString &strSql)
{
    if (NULL == m_pFieldSet)
        return false;

    std::vector<int> arField;
    size_t cnt = m_pFieldSet->GetPrimaryFieldIndex(arField);
    if (cnt==0)
    {
        return false;
    }

    strSql.Format("UPDATE %s SET ", this->GetTableName());
    uint8_t nIndex(0);
    uint8_t nChangedIndex(0);
    for(MY_DATA::iterator it(m_data.begin()); it != m_data.end(); 
        ++it, ++nIndex)
    {
        IMyData *pData = *it;
        IMyField *pField = m_pFieldSet->Find(nIndex);
        enum MyDataOpFlag opFlag = pData->getFlag();
        switch(opFlag)
        {
        case dataOpInsert:
        case dataOpUpdate:
        //case dataOpDelete:
            if (nChangedIndex++>0)
            {
                strSql.Append(",");
            }
            if (pField->IsNumberField())
                strSql.Append("%s=%u", pField->GetName(), pData->toUInt());
            else if (pField->IsNumericField() || pField->IsDoubleField())
                strSql.Append("%s=%lf", pField->GetName(), pData->toDouble());
            else
                strSql.Append("%s='%s'", pField->GetName(), pData->toString());
            break;
        default:
            break;
        }
    }

    strSql.Append(" WHERE ");
    int32_t nKeyField(0);
    for(std::vector<int>::iterator it(arField.begin()); 
        it != arField.end(); ++it, ++nKeyField)
    {
        nIndex = (uint8_t)(*it);
        IMyField *pField = m_pFieldSet->Find(nIndex);
        IMyData *pData = m_data.at(nIndex);
        if (nKeyField > 0)
        {
            strSql.Append(" AND ");
        }
        if (pField->IsNumberField())
            strSql.Append("%s=%u", pField->GetName(), pData->toUInt());
        else if (pField->IsNumericField() || pField->IsDoubleField())
            strSql.Append("%s=%lf", pField->GetName(), pData->toDouble());
        else
            strSql.Append("%s='%s'", pField->GetName(), pData->toString());
    }

    return true;
}


bool CMyRecord::Flush(ISimDB *pConn)
{
    if (NULL == pConn)
    {
        return false;
    }

    //检查记录状态
    if (!this->PreProcessRecord())
    {
        return false;
    }

    //构建SQL语句
    bool bSqlValid(false);
    CMyString strSql;
    switch(m_opFlag)
    {
    case recordOpDelete:
        bSqlValid = this->FormatDeleteSQL(strSql);
        break;
    case recordOpInsert:
        bSqlValid = this->FormatInsertSQL(strSql);
        break;
    case recordOpUpdate:
        bSqlValid = this->FormatUpdateSQL(strSql);
        break;
    default:
        break;
    }
    if (!bSqlValid)
    {
        return false;
    }

    //写入数据库
    pConn->Cmd(strSql.c_str());
    int32_t ret = pConn->Exec();
    if (ret<0)
    {
        return false;
    }

    //删除记录
    if (m_opFlag == recordOpDelete)
    {
        delete this;
        return true;
    }

    //都设置成查询状态
    m_opFlag = recordOpSelect;

    //更新本记录中数据的状态
    for(MY_DATA::iterator it(m_data.begin()); it != m_data.end(); ++it)
    {
        IMyData *pData = *it;
        if (NULL == pData) continue;
        pData->setFlag(dataOpSelect);
    }
    return true;
}

bool CMyRecord::GetKey(char *szKey, size_t cbKey)
{
    if (NULL == m_pFieldSet)
        return false;

    std::vector<int> arField;
    size_t cnt = m_pFieldSet->GetPrimaryFieldIndex(arField);
    if (cnt==0)
    {
        return false;
    }
    CMyString strKey;
    int32_t nKeyField(0);
    for(std::vector<int>::iterator it(arField.begin()); 
        it != arField.end(); ++it, ++nKeyField)
    {
        uint8_t nIndex = (uint8_t)(*it);
        IMyField *pField = m_pFieldSet->Find(nIndex);
        if (!pField->IsPrimaryKeyField())
        {
            continue;
        }
        IMyData *pData = m_data.at(nIndex);
        if (nKeyField > 0)
        {
            strKey.Append(":");
        }
        if (pField->IsNumberField())
            strKey.Append("%u", pData->toUInt());
        else if (pField->IsNumericField() || pField->IsDoubleField())
            strKey.Append("%lf", pData->toDouble());
        else
            strKey.Append("%s", pData->toString());
    }
    safe_strcpy(szKey, cbKey, strKey.c_str());
    return true;
}

void CMyRecord::SetReadOnly(bool val)
{
    for(MY_DATA::iterator it(m_data.begin()); it != m_data.end(); ++it)
    {
        IMyData *pData = *it;
        if (NULL == pData)
            continue;

        ((CMyData*)pData)->SetReadOnly(val);
    }
}

