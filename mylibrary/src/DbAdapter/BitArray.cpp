#include "BitArray.h"
#include "DaData.h"
#include "DaRecord.h"

using namespace my_data;
//////////////////////////////////////////////////////////////////////////
//CBitArray

CBitArray::CBitArray(byte_t* pData, size_t cbData, CData *pOwner/* = NULL*/)
    : m_pData(pData)
    , m_cbData(cbData)
	, m_pRecord(NULL)  
    , m_pOwner(pOwner)
{
    DB_ASSERT(m_cbData>0);
    if (m_pOwner != NULL)
    {
        m_pRecord = m_pOwner->m_pRecord;
    }
}

CBitArray::~CBitArray()
{
}

CBitArray::CBitArray(const CBitArray &rhs)
    : m_pData(rhs.m_pData)
    , m_cbData(rhs.m_cbData)
    , m_pRecord(rhs.m_pRecord)
    , m_pOwner(rhs.m_pOwner)
{
}

CBitArray& CBitArray::operator=(const CBitArray& rhs)
{
    if (&rhs != this)
    {
        this->m_pData = rhs.m_pData;
        this->m_cbData = rhs.m_cbData;
        this->m_pOwner = rhs.m_pOwner;
        this->m_pRecord = rhs.m_pRecord;
    }
    return *this;
}

bool CBitArray::Set(size_t index)
{
    if (index >= m_cbData*8)
    {
        DB_ASSERT(0);
        return false;
    }

    size_t left = index%8;
    size_t pos = index/8;
    byte_t &c = m_pData[pos];
    if (! ISBITSET(c, left) )
    {
        SETBIT(c, left);
        if (NULL != m_pOwner && NULL != m_pRecord)
        {
            m_pRecord->Update();
        }
    }

    return true;
}

bool CBitArray::Reset(size_t index)
{
    if (index >= m_cbData*8)
    {
        DB_ASSERT(0);
        return false;
    }

    size_t left = index%8;
    size_t pos = index/8;
    byte_t &c = m_pData[pos];
    if ( ISBITSET(c, left) )
    {
        CLEARBIT(c, left);
        if (NULL != m_pOwner && NULL != m_pRecord)
        {
            m_pRecord->Update();
        }
    }

    return true;
}

bool CBitArray::IsSet(size_t index)
{
    if (index >= m_cbData*8)
    {
        DB_ASSERT(0);
        return false;
    }

    size_t left = index%8;
    size_t pos = index/8;
    byte_t &c = m_pData[pos]; 
    return ISBITSET(c, left);
}

bool CBitArray::IsAnySet(void)
{
    for(size_t i=0; i<m_cbData; ++i)
    {
        byte_t c = m_pData[i];
        if (c!=0)
        {
            return true;
        }
    }
    return false;
}

void CBitArray::Output()
{
    char szText[1024] = "";
    safe_strcpy(szText, sizeof(szText), "0x");
    for(size_t i=0; i<m_cbData; ++i)
    {
        byte_t c = m_pData[i];
        char temp[3] = "";
        safe_sprintf(temp, sizeof(temp), "%02X", c);
        safe_strcat(szText, sizeof(szText), temp);
    }
    std::cout << "[" << szText << "]" << std::endl;
}

const char* CBitArray::GetBinaryStr(char *szText, size_t cbText)
{
    safe_strcpy(szText, cbText, "0x");
    for(size_t i=0; i<m_cbData; ++i)
    {
        byte_t c = m_pData[i];
        char temp[3] = "";
        safe_sprintf(temp, sizeof(temp), "%02X", c);
        safe_strcat(szText, cbText, temp);
    }
    return szText;
}

const char* CBitArray::GetBinaryStr(CMyString &str)
{
    str = "0x";
    for(size_t i=0; i<m_cbData; ++i)
    {
        byte_t c = m_pData[i];
        char temp[3] = "";
        safe_sprintf(temp, sizeof(temp), "%02X", c);
        str += temp;
    }
    return str.c_str();
}

void CBitArray::ResetAll(void)
{
    memset(m_pData, 0, m_cbData);
    if (NULL != m_pOwner && NULL != m_pRecord)
    {
        m_pRecord->Update();
    }
}

