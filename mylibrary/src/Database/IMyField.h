
#ifndef _GFX_MYDB_FIELD_INTERFACE_
#define _GFX_MYDB_FIELD_INTERFACE_

#include "BaseCode.h"
#include "mysql.h"

/*


enum FIELD_TYPE
{
    TYPE_DECIMAL,           //decimal
    TYPE_TINY,              //char
    TYPE_SHORT,             //short
    TYPE_LONG,              //int
    TYPE_FLOAT,             //float
    TYPE_DOUBLE,            //double
    TYPE_NULL,              //
    TYPE_TIMESTAMP,         //timestamp
    TYPE_LONGLONG,          //__int64
    TYPE_INT24,             //long
    TYPE_DATE,              //date
    TYPE_TIME,              //time
    TYPE_DATETIME,          //datetime
    TYPE_YEAR,              //
    TYPE_NEWDATE,           //
    TYPE_ENUM       =247,
    TYPE_SET        =248,
    TYPE_TINY_BLOB  =249,   //blob
    TYPE_MEDIUM_BLOB=250,   //blob
    TYPE_LONG_BLOB  =251,   //blob
    TYPE_BLOB       =252,   //blob
    TYPE_VAR_STRING =253,   //string or binary
    TYPE_STRING     =254,   //string or binary
    TYPE_GEOMETRY   =255
};
*/

//以下定义字段的标识
//#pragma message("若有包含mysql.h，请在此头文件前包含")
#ifndef _mysql_com_h
enum FIELD_FLAG_DEFINE
{
    NOT_NULL_FLAG       = 1,
    PRI_KEY_FLAG        = 2,
    UNIQUE_KEY_FLAG     = 4,
    MULTIPLE_KEY_FLAG   = 8,
    BLOB_FLAG           = 16,
    UNSIGNED_FLAG       = 32,
    ZEROFILL_FLAG       = 64,
    BINARY_FLAG         = 128,
    ENUM_FLAG           = 256,
    AUTO_INCREMENT_FLAG = 512,
    TIMESTAMP_FLAG      = 1024,
    SET_FLAG            = 2048
};

enum enum_field_types { MYSQL_TYPE_DECIMAL, MYSQL_TYPE_TINY,
			MYSQL_TYPE_SHORT,  MYSQL_TYPE_LONG,
			MYSQL_TYPE_FLOAT,  MYSQL_TYPE_DOUBLE,
			MYSQL_TYPE_NULL,   MYSQL_TYPE_TIMESTAMP,
			MYSQL_TYPE_LONGLONG,MYSQL_TYPE_INT24,
			MYSQL_TYPE_DATE,   MYSQL_TYPE_TIME,
			MYSQL_TYPE_DATETIME, MYSQL_TYPE_YEAR,
			MYSQL_TYPE_NEWDATE, MYSQL_TYPE_VARCHAR,
			MYSQL_TYPE_BIT,
            MYSQL_TYPE_NEWDECIMAL=246,
			MYSQL_TYPE_ENUM=247,
			MYSQL_TYPE_SET=248,
			MYSQL_TYPE_TINY_BLOB=249,
			MYSQL_TYPE_MEDIUM_BLOB=250,
			MYSQL_TYPE_LONG_BLOB=251,
			MYSQL_TYPE_BLOB=252,
			MYSQL_TYPE_VAR_STRING=253,
			MYSQL_TYPE_STRING=254,
			MYSQL_TYPE_GEOMETRY=255

};
#endif  //_mysql_com_h

typedef enum_field_types	FIELD_TYPE;

struct FieldInfo
{
    uint8_t             	index;          //字段索引
    char                	name[65];       //字段名: mysql 字段名长度<=64
    uint32_t            	length;         //长度
    uint8_t             	precision;      //numeric类型的小数位
    FIELD_TYPE				eType;          //字段类型, 参见 FIELD_TYPE
    uint32_t            	flag;           //参见 FIELD_FLAG_DEFINE
};

class IMyField
{
public:
    virtual void release(void) = 0;

    virtual const char*     GetName(void) const = 0;
    virtual const char*     GetTable(void) const = 0;
    virtual FIELD_TYPE		GetType(void) const = 0;
    virtual uint32_t        GetLength(void) const = 0;
    virtual uint8_t         GetPrecision(void) const = 0;

    //以下为判断字段类型
    //---------------------------------------------------------------------
    virtual bool IsNumberField() const = 0;
    virtual bool IsNumericField() const = 0;
	virtual bool IsBigintField() const = 0;
    virtual bool IsStringField() const = 0;
    virtual bool IsDoubleField() const = 0;
    virtual bool IsDateField() const = 0;       //日期格式 yyyy-mm-dd
    virtual bool IsTimeField() const = 0;       //时间格式 HH:MM:SS
    virtual bool IsDateTimeField() const = 0;   //日期时间格式 yyyy-mm-dd HH:MM:SS
    virtual bool IsTimestampField() const = 0;  //时间戳格式 yyyymmddHHMMSS
    virtual bool IsBlobField() const = 0;

    //binary字段内置的类型也是string，
    //若存在binary与string同时判断时,必须先判断是否binary字段
    virtual bool IsBinaryField() const = 0;

    virtual bool IsPrimaryKeyField() const = 0;
    virtual bool IsNotNullField() const = 0;
    virtual bool IsUnsignedField() const = 0;

protected:
    virtual ~IMyField() {}
};


#endif  //_GFX_MYDB_FIELD_INTERFACE_

