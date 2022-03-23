#ifndef _GFX_MYDB_RECORD_H_
#define _GFX_MYDB_RECORD_H_

#include <mysql.h>
#include "IMyRecord.h"
#include "MyFieldSet.h"
#include "BaseCode.h"


enum REC_STATUS
{
    REC_STATUS_ORIGIN = 0,
    REC_STATUS_UPDATE = 1,
    REC_STATUS_DELETE = 1 << 1,
    REC_STATUS_INSERT = 1 << 2,
};

enum MyRecordOpFlag
{
    recordOpDummy = 0,
    recordOpSelect,         //有数据集
    recordOpInsert,
    recordOpUpdate,
    recordOpDelete,
};

class CMyRecord : public IMyRecord
{
    typedef std::vector<IMyData*>   MY_DATA;
	NON_COPYABLE(CMyRecord)
    //MYPOOL_EX_DECLARATION(CMyRecord, s_objHeap)
public:
    CMyRecord();
    explicit CMyRecord(CMyFieldSet* pFieldSet);
    CMyRecord(CMyFieldSet* pFieldSet, MYSQL_ROW row, uint32_t *lengths);
    virtual ~CMyRecord();

    void SetReadOnly(bool val);

    virtual IMyData* operator[](int32_t nIndex);
    virtual IMyData* Field(int32_t nIndex);
    virtual IMyData* Field(const char *pszName);
    virtual size_t FieldCount(void) const;
    virtual void release(void);

public:
    virtual bool IsValid(void);
    virtual bool IsChanged(void);
    virtual int32_t Delete(void);
    virtual bool Flush(ISimDB *pConn);
    virtual bool GetKey(char *szKey, size_t cbKey);

protected:
    bool PreProcessRecord(void);

private:
    bool PreProcessInsert(void);
    bool PreProcessSelect(void);
    bool PreProcessDelete(void);
    bool PreProcessUpdate(void);

    const char* GetTableName(void);
    bool FormatDeleteSQL(CMyString &strSql);
    bool FormatInsertSQL(CMyString &strSql);
    bool FormatUpdateSQL(CMyString &strSql);

protected:
    MY_DATA                 m_data;
    enum MyRecordOpFlag     m_opFlag;
    CMyFieldSet*            m_pFieldSet;
};


#endif  //_GFX_MYDB_RECORD_H_

