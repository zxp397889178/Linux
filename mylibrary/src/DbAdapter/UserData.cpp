#include "UserData.h"
#include "DaFieldTable.h"

using namespace my_data;
//////////////////////////////////////////////////////////////////////////
CUserData::CUserData()
    : m_pBucket(new CBucket() )
    , m_pBase(NULL)
    //, m_pBuildingSet(NULL)
{
    ASSERT(m_pBucket);
}

CUserData::~CUserData()
{
    SAFE_DELETE(m_pBucket);
}

bool CUserData::Init(IBucketEvent *pEvent, const char *pszConfig)
{
    if (!m_pBucket->Init(pEvent, pszConfig))
    {
        return false;
    }

    m_pBase = m_pBucket->QueryRecord("USER_BASE");
    //m_pBuildingSet = m_pBucket->QueryRecordSet("USER_BUILDING");
    ASSERT(m_pBase);
    //ASSERT(m_pBuildingSet);

    return true;
}

CRecord* CUserData::QueryRecord(const char *pszUniqueName) const
{
    return m_pBucket->QueryRecord(pszUniqueName);
}

CRecordSet* CUserData::QueryRecordSet(const char *pszUniqueName) const
{
    return m_pBucket->QueryRecordSet(pszUniqueName);
}

uint32_t CUserData::GetUserID(void)
{
    return this->Base()->Field(0)->AsUint32();
}

uint32_t CUserData::GetID(void)
{
    return this->Base()->Field(1)->AsUint32();
}

int CUserData::PackData(byte_t *pData, size_t cbData, bool bPacked /* = false */, bool bChangedOnly /* = false */ )
{
    return m_pBucket->PackData(pData, cbData, bPacked, bChangedOnly);
}

int CUserData::PackDataEx(byte_t *&pData, size_t &cbData, bool &bCompressed, bool bChangedOnly /* = false */)
{
    return m_pBucket->PackDataEx(pData, cbData, bCompressed, bChangedOnly);
}

int CUserData::UnpackData(byte_t *pData, size_t cbData, bool bPacked /* = false */)
{
    return m_pBucket->UnpackData(pData, cbData, bPacked);
}

int CUserData::UnpackDataEx(byte_t *pData, size_t cbData)
{
    return m_pBucket->UnpackDataEx(pData, cbData);
}

void CUserData::SetBucketID(uint32_t _ID)
{
    m_pBucket->SetID(_ID);
}

void CUserData::SetKeepRecordFlag(bool val)
{
    m_pBucket->SetKeepRecordFlag(val);
}

void CUserData::BindDataSource(IDataOperator *pDbSource, uint32_t userId)
{
	m_pBucket->BindDataSource(pDbSource, userId);
}




