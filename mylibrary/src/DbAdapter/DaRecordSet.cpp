#include "DaRecordSet.h"
#include "DaRecordIndex.hpp"
#include "DaFieldTable.h"
#include "IDataOperator.h"

namespace my_data{
class CRecordVector : public std::vector< CRecord* >
{
};
class CRecordIndex : public TRecordIndex
{
};

} //namespace my_data

using namespace my_data;
//////////////////////////////////////////////////////////////////////////
CRecordSet::CRecordSet(const char *pszPath, const char *pszTable)
    : m_pRecordVec(new CRecordVector())
    , m_pIndex(new CRecordIndex())
    , m_pFieldSet()
	, m_pDataOperator(NULL)	 
	, m_strPath(pszPath)
	, m_strSelectSQL()	 
{
    DB_ASSERT(m_pRecordVec);
    DB_ASSERT(m_pIndex);
    safe_strcpy(m_szTable, sizeof(m_szTable), pszTable);
    m_pFieldSet = FIELD_TABLE.LoadTable(m_strPath.c_str(), m_szTable);
    DB_ASSERT(m_pFieldSet);
}

CRecordSet::~CRecordSet()
{
    //数据回写
// #pragma todo("析构前是否需要数据回写？")
    //this->Flush();

	this->ClearAll();

    SAFE_DELETE(m_pRecordVec);
    SAFE_DELETE(m_pIndex);
}

void CRecordSet::Delete(CRecord *pRecord)
{
    // 删除索引
    CData* pData = pRecord->GetKey();
    if (NULL != pData)
    {
        uint32_t nKey = pData->AsUint32();
        m_pIndex->Erase(nKey);
    }

    // 删除记录集
    m_pRecordVec->erase(
        std::remove(m_pRecordVec->begin(), m_pRecordVec->end(), pRecord), 
        m_pRecordVec->end());

    // 删除内存
    SAFE_RELEASE(pRecord);
}

CRecord* CRecordSet::at(int nIndex)
{
    if (nIndex<0 || nIndex >= (int)m_pRecordVec->size())
    {
        return NULL;
    }
    return m_pRecordVec->at(nIndex);
}

CRecord* CRecordSet::operator[](int nIndex)
{
    return this->at(nIndex);
}

CRecord* CRecordSet::Find(uint32_t nKey)
{
    return m_pIndex->Find(nKey);
}

size_t CRecordSet::Count()
{
    return m_pRecordVec->size();
}

CRecord* CRecordSet::NewRecord(uint32_t nKey)
{
    DB_ASSERT(nKey >0);
    CRecord *pRecord = new CRecord(m_strPath.c_str(), m_szTable, this);
    if (NULL == pRecord)
    {
        return NULL;
    }
    CData *pKey = pRecord->GetKey();
    DB_ASSERT(pKey!=NULL);
    pRecord->SetInsertFlag();   //设置新增状态
    pKey->Set(nKey);
    return this->AddRecord(pRecord);
}

CRecord* CRecordSet::NewRecord(byte_t* pData, size_t cbData, bool bPackData/* = false*/)
{
    CRecord *pRecord = this->AddRecord(pData, cbData, bPackData);
    if (NULL == pRecord)
    {
        return NULL;
    }
    pRecord->SetInsertFlag();
    return this->AddRecord(pRecord);
}

CRecord* CRecordSet::AddRecord(byte_t* pData, size_t cbData, bool bPackData /* = false */)
{
    CRecord *pRecord = new CRecord(m_strPath.c_str(), m_szTable, this);
    if (NULL == pRecord)
    {
        return NULL;
    }

    if (!pRecord->Init(pData, cbData, bPackData))
    {
        SAFE_RELEASE(pRecord);
    }

    return this->AddRecord(pRecord);
}

CRecord* CRecordSet::AddRecord(CRecord *pRecord)
{
    m_pRecordVec->push_back(pRecord);
    m_pIndex->Build(pRecord);
    return pRecord;
}

CRecord* CRecordSet::UpdateRecord(CRecord *pRecord, bool bKeepFlag /* = false*/)
{
    CData* pKey = pRecord->GetKey();
    if (!pKey)
        return NULL;

    uint32_t nKey = pKey->AsUint32();
    CRecord* pOldRecord = this->Find(nKey);
    if (NULL == pOldRecord)
    {
        return this->AddRecord(pRecord);
    }
    //更新记录数据
    pOldRecord->UpdateRecord(pRecord);
    //保持更新的Flag
    if (bKeepFlag)
    {
        pOldRecord->Update();
    }
    SAFE_RELEASE(pRecord);
    return pOldRecord;
}

void CRecordSet::DeleteRecord(uint32_t nKey)
{
    CRecord* pRecord = this->Find(nKey);
    if (NULL == pRecord)
    {
        return;
    }
    pRecord->Delete();
}

void CRecordSet::ClearAll(void)
{
	CRecordVector vecRecord(*m_pRecordVec);
	CRecordVector::iterator it(vecRecord.begin()); 	
	for(; it != vecRecord.end(); ++it)
	{
		CRecord *pRecord = *it;
		SAFE_RELEASE(pRecord);
	}
	vecRecord.clear();
	m_pRecordVec->clear();
	// 清除索引
	m_pIndex->Clear();
}

void CRecordSet::Flush(CMyString &strSql)
{
    RECORD_VECTOR::iterator it(m_pRecordVec->begin());
    for(; it != m_pRecordVec->end(); ++it)
    {
        CRecord* pRecord = *it;
        if (!pRecord->IsChanged())
            continue;

        pRecord->Flush(strSql);
    }
}

int CRecordSet::Flush(byte_t *pData, size_t cbData)
{
    byte_t *pc = pData;
    int nTotalLen(0);
    int nByteLeft(cbData);
    int nRowLen = (int)m_pFieldSet->GetLength();
    RECORD_VECTOR::iterator it(m_pRecordVec->begin());
    for(; it != m_pRecordVec->end(); ++it)
    {
        CRecord* pRecord = *it;
        if (nByteLeft < nRowLen)
        {
            DB_ASSERT(0);
            break;
        }

        int ret = pRecord->Flush(pc, nByteLeft);
        if (ret>0)
        {
            pc += ret;
            nByteLeft -= ret;
            nTotalLen += ret;
        }
        else if (ret<0)
        {
            DB_ASSERT(0);
        }
    }

    return nTotalLen;
}

const char* CRecordSet::GetUniqueName() const
{
    return m_pFieldSet->GetUniqueName();
}

int CRecordSet::PackData(byte_t *pData, size_t cbData, 
    bool bPacked /* = false */, bool bChangedOnly /* = false*/)
{
    //检查cbData是否满足最小空间的需求
    if (cbData < UNIQUE_NAME_MAXLEN+sizeof(uint16_t)+sizeof(char))
    {
        return -1;
    }

    byte_t *pc = pData;
    size_t nTotalSize(0);
    int nLeft = (int)cbData;

    //标记为记录集
    *pc = 'S';
    pc += sizeof(char);
    nTotalSize += sizeof(char);
    nLeft -= sizeof(char);

    //打包TITLE
    safe_strcpy((char*)pc, UNIQUE_NAME_MAXLEN, this->GetUniqueName());
    pc += UNIQUE_NAME_MAXLEN;
    nTotalSize += UNIQUE_NAME_MAXLEN;
    nLeft -= UNIQUE_NAME_MAXLEN;

    //预留出总记录的数据
    uint16_t* pCount = (uint16_t*)pc;
    pc += sizeof(uint16_t);
    nTotalSize += sizeof(uint16_t);
    nLeft -= sizeof(uint16_t);

    uint16_t nCount(0);
    bool bOverSize(false);
    //逐个打包数据
    for(CRecordVector::iterator it(m_pRecordVec->begin()); it != m_pRecordVec->end(); ++it )
    {
        CRecord *pRecord = *it;
        DB_ASSERT(pRecord);
        if (nLeft <= 0)
        {
            bOverSize = true;
            break;
        }
        if (bChangedOnly && !pRecord->IsChanged())
        {
            continue;
        }
        //打包数据记录
        int ret = pRecord->PackData(pc, (size_t)nLeft, false, bPacked);
        if (ret < 0)
        {
            bOverSize = true;
            break;
        }
        pc += ret;
        nTotalSize += (size_t)ret;
        nLeft -= ret;
        ++nCount;
    }

    if (bOverSize)
    {
        return -1;
    }
    if (0 == nCount)
    {
        return 0;
    }
    *pCount = nCount;

    return (int)nTotalSize;
}

void CRecordSet::ClearFlag()
{
    //首先删除已标识成 DELETE 状态的内存记录
    for(CRecordVector::iterator it(m_pRecordVec->begin()); it != m_pRecordVec->end(); )
    {
        CRecord *pRecord = *it;
        DB_ASSERT(pRecord);
        if (pRecord->IsDelete())
        {
            // 删除索引
            CData* pData = pRecord->GetKey();
            if (NULL != pData)
            {
                uint32_t nKey = pData->AsUint32();
                m_pIndex->Erase(nKey);
            }

            //从记录集中删除
            it = m_pRecordVec->erase(it);
            
            //删除内存
            SAFE_DELETE(pRecord);
        }
        else
        {
            pRecord->ClearFlag();
            ++it;
        }
    }

    //for(CRecordVector::iterator it(m_pRecordVec->begin()); it != m_pRecordVec->end(); ++it)
    //{
    //    CRecord *pRecord = *it;
    //    DB_ASSERT(pRecord);
    //    pRecord->ClearFlag();
    //}
}

bool CRecordSet::Reload(bool ignoreChange)
{
	if (NULL == m_pDataOperator)
	{
		// printf("ERR:not set data operator!\n");
		return false;
	}
	return m_pDataOperator->Reload(this, ignoreChange);
}



