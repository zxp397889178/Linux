#include "DictData.h"
#include "Bucket.h"
#include "MyDB.h"

using namespace my_data;
//////////////////////////////////////////////////////////////////////////
CDictData::CDictData()
    : m_pBucket(new CBucket() )
{
    DB_ASSERT(m_pBucket);
}

CDictData::~CDictData()
{
    SAFE_DELETE(m_pBucket);
}

bool CDictData::Init(IBucketEvent *pEvent, const char *pszConfig)
{
    if (!m_pBucket->Init(pEvent, pszConfig))
        return false;

    return true;
}

CRecordSet* CDictData::QueryRecordSet(const char *pszUniqueName) const
{
    return m_pBucket->QueryRecordSet(pszUniqueName);
}

int CDictData::PackDataEx(byte_t *&pData, size_t &cbData, bool &bCompressed)
{
    return m_pBucket->PackDataEx(pData, cbData, bCompressed);
}

int CDictData::UnpackData(byte_t *pData, size_t cbData, bool bPacked /* = false */)
{
    return m_pBucket->UnpackData(pData, cbData, bPacked);
}

int CDictData::UnpackDataEx(byte_t *pData, size_t cbData)
{
    return m_pBucket->UnpackDataEx(pData, cbData);
}

int CDictData::Flush(void)
{  
	return m_pBucket->Flush(); 
}

int CDictData::Flush2DB(void)
{ 
	return m_pBucket->Flush2DB(); 
}


