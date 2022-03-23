#include "MyData.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////
//CMyData
CMyData::CMyData()
    : m_pData(NULL)
    , m_nDataLen(0)
    , m_bAllocBuff(0)
    , m_opFlag(dataOpDummy)
    , m_bReadOnly(false)
    , m_pField(NULL)
{
    DB_ASSERT(0);
}

CMyData::CMyData(IMyField* pField)
    : m_pData(NULL)
    , m_nDataLen(0)
    , m_bAllocBuff(0)
    , m_opFlag(dataOpDummy)
    , m_bReadOnly(false)
    , m_pField(pField)
{
    DB_ASSERT(m_pField);
    if (!m_pField)
        return;

    size_t field_len = m_pField->GetLength();
    if (field_len >= sizeof(m_buff))
    {
        m_bAllocBuff = 1;
        m_pData = new byte_t[field_len + 1];
        memset(m_pData, 0, field_len + 1);
    }
    else
    {
        m_pData = &m_buff[0];
        memset(m_pData, 0, sizeof(m_buff));
    }
}


CMyData::CMyData(byte_t* data_ptr, size_t data_len, IMyField *pField)
    : m_pData(NULL)
    , m_nDataLen((uint16_t)data_len)
    , m_bAllocBuff(0)
    , m_opFlag(dataOpSelect)
    , m_bReadOnly(false)
    , m_pField(pField)
{
    DB_ASSERT(m_pField);
    if (!m_pField)
        return;

    size_t field_len = m_pField->GetLength();
    if (field_len >= sizeof(m_buff))
    {
        m_bAllocBuff = 1;
        m_pData = new byte_t[field_len + 1];
        memset(m_pData, 0, field_len + 1);
    }
    else
    {
        m_pData = &m_buff[0];
        memset(m_pData, 0, sizeof(m_buff));
    }

    if (data_len > 0)
    {
        memcpy(m_pData, data_ptr, std::min(field_len, data_len) );
    }

    if (pField->IsPrimaryKeyField())
    {
        this->SetReadOnly(true);
    }
}

CMyData::~CMyData()
{
    if (m_bAllocBuff != 0)
    {
        SAFE_DELETE_ARRAY(m_pData);
    }
}

void CMyData::release(void)
{
	delete this;
}

bool CMyData::setString(const char *val)
{ 
    if (EMPTY_STR(val))
        return this->setData<const char*>(NULL, 0);
    return this->setData<const char*>(val, strlen(val));
}

bool CMyData::setShort(int16_t val) 
{ 
	return this->setInt64(val);
}

bool CMyData::setInt(int32_t val)
{ 
	return this->setInt64(val);
}

bool CMyData::setInt64(int64_t val)
{
	char szText[64] = "";
	_i64toa(val, szText, 10);
	return this->setData(szText, strlen(szText));
}

bool CMyData::setUInt(uint32_t val)
{ 
	return this->setUInt64(val);
}

bool CMyData::setUInt64(uint64_t val)
{ 
    char szText[64] = "";
    _ui64toa(val, szText, 10);
    return this->setData(szText, strlen(szText));
}

bool CMyData::setFloat(float val)
{ 
	return this->setDouble(val);
}

bool CMyData::setDouble(double val)
{ 
    char fmt[16] = "";
    char szText[64] = "";
    safe_sprintf(fmt, sizeof(fmt), "%%.%dlf", m_pField->GetPrecision());
    safe_sprintf(szText, sizeof(szText), fmt, val);
    return this->setData(szText, strlen(szText));
}

bool CMyData::setTime(time_t val)
{
    char szText[64] = "";
    TimeToStr(val, szText, sizeof(szText), "yyyy-mm-dd HH:MM:SS");
    return this->setData(szText, strlen(szText));
}

const char* CMyData::toString(void) const
{
	return (const char*)m_pData;
}

int32_t	CMyData::toInt(void) const
{
	return StrToLong((const char*)m_pData);
}

uint32_t CMyData::toUInt(void) const
{
	return StrToUint32((const char*)m_pData);
}

uint64_t CMyData::toUInt64(void) const
{ 
	return StrToUint64((const char*)m_pData);
}

int64_t	CMyData::toInt64(void) const
{ 
	return StrToInt64((const char*)m_pData);
}

float CMyData::toFloat(void) const
{
	return (float)StrToDouble((const char*)m_pData);
}

double CMyData::toDouble(void) const
{ 
	return StrToDouble((const char*)m_pData);
}

time_t CMyData::toTime() const
{ 
	return StrToTime((const char*)m_pData); 
}

