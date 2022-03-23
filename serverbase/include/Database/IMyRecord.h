
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
    //�жϼ�¼�Ƿ���Ч
    virtual bool IsValid(void) = 0;

    //�жϼ�¼�е������Ƿ��б��ı��
    virtual bool IsChanged(void) = 0;

    //ɾ��������¼
    //��������¼�������ģ����������ݿ����Ѵ��ڣ���ֱ���ͷ��ڴ棬���� 0
    //���򷵻� 1
    virtual int32_t Delete(void) = 0;

    //ˢд�����ݿ�
    virtual bool Flush(ISimDB *pConn) = 0;

    //��ȡ������¼��Keyֵ,����Key�򷵻�false
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

    /* ��ֵ����֧�ֶ���ֶ����PrimaryKey�Ĵ���
       ��PrimaryKey�н���һ���ֶΣ���pszKeyΪ��ֵ���ݣ����ַ����ֶ�ֱ����\"������
            �磺 key=1234,�� pszKey="1234"
       ��PrimaryKey��������ֶΣ��򰴼���˳����':'�ָ���
            �磺 key1='abcd',key2=123����pszKey="abcd:123"
    */
    virtual IMyRecord* Find(const char *pszKey) = 0;
    virtual bool Add(const char *pszKey, IMyRecord *pRecord) = 0;
    virtual bool Remove(const char *pszKey) = 0;
    //ˢд�����ݿ��У�����ֵ��ʾ��Ӧ������
    virtual int32_t Flush() = 0;
    //����һ���µļ�¼��
    virtual IMyRecord* NewRecord(void) = 0;

public:
    virtual int32_t SaveToFile(const char *pszFile, enum ExportFileType eType) = 0;

protected:
    virtual ~ISimRecordSet() {}
};

//typedef TObjectPtrGuard<ISimRecordSet, policy_ptr_release<ISimRecordSet> >  CAutoRecordSet;


#endif  //_GFX_MYDB_RECORD_DB_INTERFACE_

