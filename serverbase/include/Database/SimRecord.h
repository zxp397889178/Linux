#ifndef _GFX_MYDB_SIMPLE_RECORD_H_
#define _GFX_MYDB_SIMPLE_RECORD_H_

#include "ISimDB.h"
#include "IMyRecord.h"
#include "MyFieldSet.h"
#include <vector>
#include <map>

class CSimRecord : public ISimDBEvent, public ISimRecordSet
{
    typedef std::vector<IMyRecord*>        CONTAINER;
    typedef std::map<int64_t, IMyRecord*>  MAP_RECORDS; // <keyValue, record> 
    //MYPOOL_EX_DECLARATION(CSimRecord, s_objHeap)
public:
    CSimRecord();
    virtual ~CSimRecord();

    //装载SQL语句到内存，允许做任何改动，但不回写到数据库
    bool Open(ISimDB *pDB, const char *fmt, ...);

    //装载数据库表到内存，允许对非主键做修改，可回写到数据库
    bool LoadTable(ISimDB *pDB, const char *pszTable, const char *fmtCond, ...);

    //bool Open(const char *user_name, const char *passwd, 
    //    const char *host, const char *db=NULL, int32_t port=0, 
    //    const char *appname=NULL);

public:
    virtual void release(void);
    //virtual bool Execute(const char *fmt, ...);
    virtual size_t Count(void) const;
    virtual IMyRecord* at(int32_t index);
    virtual const IMyRecord* at(int32_t index) const;
    virtual IMyRecord* operator[](int32_t index);
    virtual const IMyRecord* operator[] (int32_t index) const;

    virtual int32_t GetFieldCount() const;
    virtual const char* GetFieldName(int32_t nIndex);

public:
    virtual IMyRecord* Find(const char *pszKey);
    virtual bool Add(const char *pszKey, IMyRecord *pRecord);
    virtual bool Remove(const char *pszKey);
    virtual int32_t Flush(void);
    virtual IMyRecord* NewRecord(void);

public:
    virtual void OnEvent(const DB_EVENT dbEvent, double timeCost);
    virtual void OnError(const DB_EVENT dbEvent, const char *pszSql);
    virtual void OnNewResult(void* results, void* fields, uint32_t field_num);
    virtual void OnNewRow(char **row, uint32_t *lengths);
    virtual void SetAppName(const char *pszAppName);

public:
    virtual int32_t SaveToFile(const char *pszFile, enum ExportFileType eType);

protected:
    const char* GetFieldNameByIndex(int32_t nIndex);
    const char* GetTableName();

private:
    ISimDB*         m_pDB;
    CMyFieldSet     m_fields;

    CONTAINER       m_records;      // 按顺序记录record
    MAP_RECORDS     m_mapRecords;   // 按主键记录record
    CONTAINER       m_vecRemoved;   // 需要从数据库中删除的记录

    bool            m_bCanFlush;
};
	


#endif  //_GFX_MYDB_SIMPLE_RECORD_H_

