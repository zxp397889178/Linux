#include "DaData.h"
#include "DaRecord.h"
#include "DaField.hpp"
#include "MyDB.h"

namespace my_data
{
	
template<typename T>
T Cast(byte_t* const pData)
{
	return *(T*)pData;
}
template<>
const char* Cast<const char*>(byte_t* const pData)
{
	return (const char*)pData;
}
template<>
char* Cast<char*>(byte_t* const pData)
{
	return const_cast<char*>((const char*)pData);
}
template<>
const byte_t* Cast(byte_t* const pData)
{
	return (const byte_t*)pData;
}
template<>
byte_t* Cast(byte_t* const pData)
{
	return const_cast<byte_t*>(pData);
}

template<typename T> 
bool Set(CField* const pField, CData *pObj, T val)
{
    switch (pField->GetType())
    {
	case my_data::TYPE_DATETIME:
	case my_data::TYPE_DATE:
	case my_data::TYPE_TIME:
        {
            time_t tv = static_cast<time_t>(val);
            return pObj->Set((byte_t*)&tv, sizeof(time_t));
        }
        break;
	case my_data::TYPE_DECIMAL:
        {
            double d = static_cast<double>(val);
            return pObj->Set((byte_t*)&d, sizeof(double));
        }
        break;
	case my_data::TYPE_UINT:
        {
            switch(pField->GetLength())
            {
            case 1:
                {
                    uint8_t u = static_cast<uint8_t>(val);
                    return pObj->Set((byte_t*)&u, sizeof(uint8_t));
                }
                break;
            case 2:
                {
                    uint16_t u = static_cast<uint16_t>(val);
                    return pObj->Set((byte_t*)&u, sizeof(uint16_t));
                }
                break;
            case 4:
                {
                    uint32_t u = static_cast<uint32_t>(val);
                    return pObj->Set((byte_t*)&u, sizeof(uint32_t));
                }
                break;
			case 8:
				{
                    uint64_t u = static_cast<uint64_t>(val);
                    return pObj->Set((byte_t*)&u, sizeof(uint64_t));
				}
				break;
            default:
                DB_ASSERT(0);
            }
        }
        break;
	default:
		break;
    }

    return pObj->Set((byte_t*)&val, sizeof(T));
}

}
using namespace my_data;
//////////////////////////////////////////////////////////////////////////
//
CData::CData(CField* pField, byte_t* pData, size_t cbData, CRecord *pOwner)
    : m_pField(pField)
    , m_pData(pData)
    , m_cbData(cbData)
    , m_pRecord(pOwner)
{
    DB_ASSERT(m_pField);
    DB_ASSERT(m_pData);
    DB_ASSERT(m_cbData<=m_pField->GetLength());
}

CData::~CData()
{
}

bool CData::CanWrite(void)
{
    if (NULL == m_pRecord || NULL == m_pField)
    {
        return false;
    }

    if (m_pRecord->IsNew())
    {
        return true;
    }

    if (!m_pField->CanWrite())
    {
		//LogDebug("=================this field is readonly.");
        return false;
    }

    return true;
}

bool CData::Set(byte_t* pData, size_t cbData)
{
    //只读，或者空间大小不足，返回失败
    size_t cbMaxLen = this->GetMaxLength();
    if (!this->CanWrite() || cbData>cbMaxLen)
    {
        DB_ASSERT(0);
        return false;
    }

    m_cbData = cbData;
    //memset(m_pData, 0, cbMaxLen);
    memcpy(m_pData, pData, __min(cbData, cbMaxLen));
    m_pRecord->Update();
    return true;
}

const char* CData::AsString(void) const
{
	return (const char*)m_pData;
    //return (const char*)this->Get<const char*>();
}

int CData::AsInt32(void) const
{
    if (!m_pField->IsNumberField())
    {
        return -1;
    }
    uint32_t cbLen = m_pField->GetLength();
    switch(cbLen)
    {
    case 1:
        return my_data::Cast<char>(m_pData);
        break;
    case 2:
        return my_data::Cast<int16_t>(m_pData);
        break;
    case 4:
        return my_data::Cast<int32_t>(m_pData);
        break;
    case 8:
        return (int)my_data::Cast<int64_t>(m_pData);
        break;
    default:
        break;
    }
    return -1;
}

#if 0
void dump(void *pc, uint32_t cbLen)
{
	uint8_t c(0);
	char * pbuf = (char*)pc;
	for(uint32_t i(0); i<cbLen; ++i)
	{
		c = (uint8_t)pbuf[i];
		printf("%02x", c);
	}
}
#endif

int64_t CData::AsInt64(void) const
{
    if (!m_pField->IsNumberField())
    {
        return -1;
    }
    uint32_t cbLen = m_pField->GetLength();
	//printf("PLAYMON======:field length %d\n", cbLen);
	//dump(m_pData, cbLen);
	//printf("\n\n");
    switch(cbLen)
    {
    case 1:
        return my_data::Cast<int8_t>(m_pData);
        break;
    case 2:
        return my_data::Cast<int16_t>(m_pData);
        break;
    case 4:
        return my_data::Cast<int32_t>(m_pData);
        break;
    case 8:
        return my_data::Cast<int64_t>(m_pData);
        break;
    default:
        break;
    }
    return -1;
}

uint32_t CData::AsUint32(void) const
{
    if (!m_pField->IsUnsignedField())
    {
        return (uint32_t)-1;
    }
    uint32_t cbLen = m_pField->GetLength();
    switch(cbLen)
    {
    case 1:
        return my_data::Cast<uint8_t>(m_pData);
        break;
    case 2:
        return my_data::Cast<uint16_t>(m_pData);
        break;
    case 4:
        return my_data::Cast<uint32_t>(m_pData);
        break;
    case 8:
        return (uint32_t)my_data::Cast<uint64_t>(m_pData);
        break;
    default:
        break;
    }
    return (uint32_t)-1;
}

uint64_t CData::AsUint64(void) const
{
    if (!m_pField->IsUnsignedField())
    {
        return (uint64_t)-1;
    }
    uint32_t cbLen = m_pField->GetLength();
    switch(cbLen)
    {
    case 1:
        return my_data::Cast<uint8_t>(m_pData);
        break;
    case 2:
        return my_data::Cast<uint16_t>(m_pData);
        break;
    case 4:
        return my_data::Cast<uint32_t>(m_pData);
        break;
    case 8:
        return my_data::Cast<uint64_t>(m_pData);
        break;
    default:
        break;
    }
    return (uint64_t)-1;
}

byte_t* CData::AsBinary(void) const
{
    return m_pData;
}

CBitArray CData::AsBitArray(void)
{
    CBitArray bit_array(m_pData, m_cbData, this);
    return bit_array;
}

double  CData::AsDouble(void)  const
{
    return my_data::Cast<double>(m_pData);
}

time_t  CData::AsTime(void)  const
{
    return my_data::Cast<time_t>(m_pData);
}

void CData::GetBinaryStr(CMyString &str)
{
    CBitArray bit_array(m_pData, m_cbData);
    bit_array.GetBinaryStr(str);
}

bool CData::Set(char val)
{
	return my_data::Set<char>(m_pField, this, val);
}

bool CData::Set(uint8_t val)
{
	return my_data::Set<uint8_t>(m_pField, this, val);
}

bool CData::Set(uint16_t val)
{
	return my_data::Set<uint16_t>(m_pField, this, val);
}

bool CData::Set(uint32_t val)
{
	return my_data::Set<uint32_t>(m_pField, this, val);
}

bool CData::Set(uint64_t val)
{
	return my_data::Set<uint64_t>(m_pField, this, val);
}

bool CData::Set(int8_t val)
{
	return my_data::Set<int8_t>(m_pField, this, val);
}

bool CData::Set(int16_t val)
{
	return my_data::Set<int16_t>(m_pField, this, val);
}

bool CData::Set(int32_t val)
{
	return my_data::Set<int32_t>(m_pField, this, val);
}

bool CData::Set(int64_t val)
{
	return my_data::Set<int64_t>(m_pField, this, val);
}

bool CData::Set(float val)
{
	return my_data::Set<float>(m_pField, this, val);
}

bool CData::Set(double val)
{
	return my_data::Set<double>(m_pField, this, val);
}

bool CData::Set(const char *val)
{
	//额外的+1是指 '\0'
	std::string strVal(val);
	ISimDB *pDB = m_pRecord->getDBConn();
	if (NULL != pDB)
	{
		pDB->SafeString(strVal);
	}
	return this->Set((byte_t*)strVal.c_str(), strVal.length()+1);
}

bool CData::Inc(int32_t val)
{
	// 仅对整型做增减操作
    switch (m_pField->GetType())
    {
	case my_data::TYPE_DATETIME:
	case my_data::TYPE_DATE:
	case my_data::TYPE_TIME:
		return false;
        break;
	case my_data::TYPE_DECIMAL:
        {
            double d = this->AsDouble() + val;
            return this->Set((byte_t*)&d, sizeof(double));
        }
        break;
	case my_data::TYPE_UINT:
        {
            switch(m_pField->GetLength())
            {
            case 1:
                {
					uint8_t u1 = (uint8_t)this->AsUint32();
					uint8_t u(0);
					if (val >= 0)
					{
						if ((u1 + val) > (uint8_t)-1)
							u = (uint8_t)-1;
						else
							u = u1 + val;
					}
					else
					{
						if (u1 < -val)
							u = 0;
						else
							u = u1 + val;
					}
                    return this->Set((byte_t*)&u, sizeof(uint8_t));
                }
                break;
            case 2:
                {
					uint16_t u1 = (uint16_t)this->AsUint32();
					uint16_t u(0);
					if (val >= 0)
					{
						if ((u1+val) > (uint16_t)-1)
							u = (uint16_t)-1;
						else
							u = u1 + val;
					}
					else
					{
						if (u1 < -val)
							u = 0;
						else
							u = u1 + val;
					}
                    return this->Set((byte_t*)&u, sizeof(uint16_t));
                }
                break;
            case 4:
                {
					uint32_t u1 = this->AsUint32();
					uint32_t u(0);
					if (val >= 0)
					{
						if ((u1+val) > (uint32_t)-1)
							u = (uint32_t)-1;
						else
							u = u1 + val;
					}
					else
					{
						if (u1 < (uint32_t)-val)
							u = 0;
						else
							u = u1 + val;
					}
                    return this->Set((byte_t*)&u, sizeof(uint32_t));
                }
                break;
			case 8:
				{
					uint64_t u1 = this->AsUint64();
					uint64_t u(0);
					if (val >= 0)
					{
						if ((u1+val) > (uint64_t)-1)
							u = (uint64_t)-1;
						else
							u = u1 + val;
					}
					else
					{
						if (u1 < (uint64_t)-val)
							u = 0;
						else
							u = u1 + val;
					}
					return this->Set((byte_t*)&u, sizeof(uint64_t));
				}
				break;
            default:
                DB_ASSERT(0);
            }
        }
        break;
	case my_data::TYPE_INT:
        {
            switch(m_pField->GetLength())
            {
            case 1:
                {
                    char u = (char)(this->AsInt32() + val);
                    return this->Set((byte_t*)&u, sizeof(char));
                }
                break;
            case 2:
                {
                    int16_t u = (int16_t)(this->AsInt32() + val);
                    return this->Set((byte_t*)&u, sizeof(int16_t));
                }
                break;
            case 4:
                {
                    int32_t u = (int32_t)(this->AsInt32() + val);
                    return this->Set((byte_t*)&u, sizeof(int32_t));
                }
                break;
			case 8:
				{
                    int64_t u = (int64_t)(this->AsInt64() + val);
                    return this->Set((byte_t*)&u, sizeof(int64_t));
				}
            default:
                DB_ASSERT(0);
            }
        }
        break;
	default:
		break;
    }

	return false;
}

bool CData::Dec(int32_t val)
{
	return this->Inc(-val);
}

