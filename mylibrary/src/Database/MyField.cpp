#include "MyField.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////////
//CMyField
CMyField::CMyField(MYSQL_FIELD *pField)
    : m_objField()
{
    assert(pField);

    memcpy(&m_objField, (const char*)pField, sizeof(m_objField));

    int32_t n = strlen(pField->name);
    m_objField.name = new char[n + 1];
    memset(m_objField.name, 0, n + 1);
    memcpy(m_objField.name, pField->name, n);

    n = strlen(pField->table);
    m_objField.table = new char[n + 1];
    memset(m_objField.table, 0, n + 1);
    memcpy(m_objField.table, pField->table, n);
}

CMyField::CMyField()
    : m_objField()
{
    DB_ASSERT(0);
}

CMyField::~CMyField()
{
     SAFE_DELETE_ARRAY(m_objField.name);
     SAFE_DELETE_ARRAY(m_objField.table);
}

void CMyField::release(void)
{
	delete this;
}

const char* CMyField::GetName(void) const
{
    return (const char*)m_objField.name;
}

const char* CMyField::GetTable(void) const
{
    //if (!EMPTY_STR(m_objField.org_table))
    //    return (const char*)m_objField.org_table;
    return (const char*)m_objField.table;
}


FIELD_TYPE CMyField::GetType(void) const
{
    return m_objField.type;
}

uint32_t CMyField::GetLength(void) const
{
    return m_objField.length;
}

uint8_t CMyField::GetPrecision(void) const
{
    return (uint8_t)m_objField.decimals;
}

bool CMyField::IsNumberField() const
{
    switch(m_objField.type)
    {
    case MYSQL_TYPE_TINY:
    case MYSQL_TYPE_SHORT:
    case MYSQL_TYPE_LONG:
    case MYSQL_TYPE_LONGLONG:
    case MYSQL_TYPE_INT24:
    case MYSQL_TYPE_YEAR:
        return true;
    default:
        break;
    }
    return false;
}

bool CMyField::IsNumericField() const
{
    return (MYSQL_TYPE_DECIMAL == m_objField.type);
}

bool CMyField::IsBigintField() const
{
	return (MYSQL_TYPE_LONGLONG == m_objField.type);
}

bool CMyField::IsStringField() const
{
    switch(m_objField.type)
    {
    case MYSQL_TYPE_STRING:
    case MYSQL_TYPE_VAR_STRING:
        return true;
    default:
        break;
    }
    return false;
}

bool CMyField::IsDoubleField() const
{
    switch(m_objField.type)
    {
    case MYSQL_TYPE_FLOAT:
    case MYSQL_TYPE_DOUBLE:
        return true;
    default:
        break;
    }
    return false;
}

bool CMyField::IsDateField() const
{
    return (MYSQL_TYPE_DATE == m_objField.type);
}

bool CMyField::IsTimeField() const
{
    return (MYSQL_TYPE_TIME == m_objField.type);
}

bool CMyField::IsDateTimeField() const
{
    return (MYSQL_TYPE_DATETIME == m_objField.type);
}

bool CMyField::IsTimestampField() const
{
    return (MYSQL_TYPE_TIMESTAMP == m_objField.type 
			|| HAS_PROPERTY(TIMESTAMP_FLAG, m_objField.flags) );
}

bool CMyField::IsBlobField() const
{
    switch(m_objField.type)
    {
    case MYSQL_TYPE_BLOB:
    case MYSQL_TYPE_TINY_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:
        return true;
    default:
        break;
    }
    return (HAS_PROPERTY(BLOB_FLAG, m_objField.flags));
}

bool CMyField::IsBinaryField() const
{
    return (HAS_PROPERTY(BINARY_FLAG, m_objField.flags));
}

bool CMyField::IsPrimaryKeyField() const
{
    return (HAS_PROPERTY(PRI_KEY_FLAG, m_objField.flags));
}

bool CMyField::IsNotNullField() const
{
    return (HAS_PROPERTY(NOT_NULL_FLAG, m_objField.flags));
}

bool CMyField::IsUnsignedField() const
{
    return (HAS_PROPERTY(UNSIGNED_FLAG, m_objField.flags));
}

