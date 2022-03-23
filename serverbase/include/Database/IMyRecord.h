
#ifndef _GFX_MYDB_RECORD_DB_INTERFACE_
#define _GFX_MYDB_RECORD_DB_INTERFACE_

#include "ISimDB.h"
#include "IMyData.h"
#include <vector>

class IMyRecord
{
public:
    virtual IMyData* operator[](int32_t nIndex) = 0;
    virtual IMyData* Field(int32_t nIndex) = 0;
    virtual IMyData* Field(const char *pszName) = 0;
    virtual size_t FieldCount(void) const  = 0;
    virtual void release(void) = 0;

public:
    //判断记录是否有效
    virtual bool IsValid(void) = 0;

    //判断记录中的数据是否有被改变过
    virtual bool IsChanged(void) = 0;

    //删除本条记录
    //若本条记录是新增的，而不是数据库中已存在，则直接释放内存，返回 0
    //否则返回 1
    virtual int32_t Delete(void) = 0;

    //刷写进数据库
    virtual bool Flush(ISimDB *pConn) = 0;

    //获取本条记录的Key值,若无Key则返回false
    virtual bool GetKey(char *szKey, size_t cbKey) = 0;

protected:
    IMyRecord() {}
    virtual ~IMyRecord() {}
};

enum ExportFileType
{
    ftCsv,
    ftJson,
    ftXml
};

class ISimRecordSet
{
public:
    virtual void release(void) = 0;

    //virtual bool Execute(const char *fmt, ...) = 0;

    virtual size_t Count(void) const = 0;
    virtual IMyRecord* at(int32_t index) = 0;
    virtual const IMyRecord* at(int32_t index) const = 0;
    virtual IMyRecord* operator[](int32_t index) = 0;
    virtual const IMyRecord* operator[] (int32_t index) const = 0;

    virtual int32_t GetFieldCount() const = 0;
    virtual const char* GetFieldName(int32_t nIndex) = 0;

    /* 键值处理，支持多个字段组成PrimaryKey的处理
       若PrimaryKey中仅有一个字段，则pszKey为键值内容，非字符的字段直接用\"括起来
            如： key=1234,则 pszKey="1234"
       若PrimaryKey包含多个字段，则按键的顺序以':'分隔开
            如： key1='abcd',key2=123，则pszKey="abcd:123"
    */
    virtual IMyRecord* Find(const char *pszKey) = 0;
    virtual bool Add(const char *pszKey, IMyRecord *pRecord) = 0;
    virtual bool Remove(const char *pszKey) = 0;
    //刷写到数据库中，返回值表示响应的行数
    virtual int32_t Flush() = 0;
    //分配一个新的记录集
    virtual IMyRecord* NewRecord(void) = 0;

public:
    virtual int32_t SaveToFile(const char *pszFile, enum ExportFileType eType) = 0;

protected:
    virtual ~ISimRecordSet() {}
};

//typedef TObjectPtrGuard<ISimRecordSet, policy_ptr_release<ISimRecordSet> >  CAutoRecordSet;


#endif  //_GFX_MYDB_RECORD_DB_INTERFACE_

