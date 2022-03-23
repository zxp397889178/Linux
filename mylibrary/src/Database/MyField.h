#ifndef _GFX_MYDB_FIELD_H_
#define _GFX_MYDB_FIELD_H_

#include "IMyField.h"
#include "mysql.h"


class CMyField : public IMyField
{
    //MYPOOL_EX_DECLARATION(CMyField, s_objHeap)
	NON_COPYABLE(CMyField)
public:
    CMyField(MYSQL_FIELD *pField);
    CMyField();
    virtual ~CMyField();

    virtual void release(void);

    virtual const char*     GetName(void) const;
    virtual const char*     GetTable(void) const;
    virtual FIELD_TYPE 		GetType(void) const;
    virtual uint32_t        GetLength(void) const;
    virtual uint8_t         GetPrecision(void) const;

    virtual bool IsNumberField() const;
    virtual bool IsNumericField() const;
	virtual bool IsBigintField() const;
    virtual bool IsStringField() const;
    virtual bool IsDoubleField() const;
    virtual bool IsDateField() const;       //日期格式 yyyy-mm-dd
    virtual bool IsTimeField() const;       //时间格式 HH:MM:SS
    virtual bool IsDateTimeField() const;   //日期时间格式 yyyy-mm-dd HH:MM:SS
    virtual bool IsTimestampField() const;  //时间戳格式 yyyymmddHHMMSS
    virtual bool IsBlobField() const;
    virtual bool IsBinaryField() const;
    virtual bool IsPrimaryKeyField() const;
    virtual bool IsNotNullField() const;
    virtual bool IsUnsignedField() const;

protected:
    MYSQL_FIELD     m_objField;
};


#endif  //_GFX_MYDB_FIELD_H_

