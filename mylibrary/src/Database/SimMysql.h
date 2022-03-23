#ifndef _GFX_MYDB_SIMPLE_MYSQL_H_
#define _GFX_MYDB_SIMPLE_MYSQL_H_

#include <mysql.h>
#include "ISimDB.h"
#include <assert.h>

#include "BaseCode.h"
#include "MyFieldSet.h"

class CSimMySql : public ISimDB
{
public:
    CSimMySql();
    virtual ~CSimMySql();

public:
    virtual void release(void);

    virtual int32_t Open(const char *user_name, const char *passwd, 
        const char *host, const char *db=NULL, int32_t port=3306, 
        ISimDBEvent *pEvent = NULL, const char *appname=NULL,
        const char *charset="utf8");
    virtual void BindEvent(ISimDBEvent *pEvent = NULL);

    virtual void Close(void);
    virtual ISimDB* Duplicate(ISimDBEvent *pEvent = 0);
    virtual const char* Cmd(const char *fmt, ...);
    virtual int32_t Exec(void);
    virtual int32_t MoreExec(void);
    virtual bool More(void);
    virtual void Cancel(void);

	virtual const char* SafeString(std::string &src);
    virtual int32_t GetLastError(char *buffer=NULL, size_t cbBuffer=0);
    virtual void SetHandler(ISimDBEvent *pEvent);

    virtual int32_t GetData(const char* field_name, byte_t *&buffer, size_t &cbBuffer);
    virtual int32_t GetData(int32_t field_index, byte_t *&buffer, size_t &cbBuffer);
	virtual CMyString GetString(const char *field_name);
	virtual CMyString GetString(int32_t field_index);
    virtual const char* GetString(const char *field_name, char *szText, size_t cbText);
    virtual const char* GetString(int32_t field_index, char *szText, size_t cbText);
    virtual time_t GetTime(const char *field_name);
    virtual time_t GetTime(int32_t field_index);
    virtual int32_t GetInt(const char *field_name);
    virtual int32_t GetInt(int32_t field_index);
    virtual uint32_t GetUInt(const char *field_name);
    virtual uint32_t GetUInt(int32_t field_index);
    virtual int64_t GetInt64(const char* field_name);
    virtual int64_t GetInt64(int32_t field_index);
    virtual uint64_t GetUInt64(const char* field_name);
    virtual uint64_t GetUInt64(int32_t field_index);
    virtual float GetFloat(const char *field_name);
    virtual float GetFloat(int32_t field_index);
    virtual double GetDouble(const char *field_name);
    virtual double GetDouble(int32_t field_index);

    virtual bool SetCharset(const char *pszCharset);
    virtual void SetOutCharset(const char *pszCharset);

	virtual uint32_t GetFieldCount(void) { return m_field_num; }
    // timeout Ϊ��, 0 ��ʾ����ping
    // ����ֵ��true ��ʾ������һ��ping, false ��ʾδ����ping
    virtual int32_t Ping(uint32_t timeout);

    ISimDBEvent* GetHandler(void) { return m_pEvent; }

    bool GetFields(CMyFieldSet &fields);

    virtual int getResultIndex(void) { return m_nResultIndex; }

protected:
    int32_t GetFieldIndex(const char *field_name);

    template<typename T>
    T GetValue(int32_t field_index, T (*func_ptr)(const char*))
    {
        if (m_row == NULL)
            return (T)0;

        byte_t *buffer(NULL);
        size_t cbBuffer(0);
        int32_t ret = this->GetData(field_index, buffer, cbBuffer);
        DB_ASSERT(ret == 0);

        if ((ret != 0 ) || (NULL==buffer) )
        {
            return (T)0;
        }

        return func_ptr((const char*)buffer);
    }

    template<typename T>
    T GetValue(const char *field_name, T (*func_ptr)(const char *))
    {
        int32_t field_index = GetFieldIndex(field_name);
        DB_ASSERT(field_index >= 0);
        
        if (field_index < 0)
        {
            return (T)0;
        }
        return this->GetValue(field_index, func_ptr);
    }

private:
    bool GetResult(void);
    bool IsInsertSql(CMyString &strSql);
    bool IsUpdateSql(CMyString &strSql);
    bool IsDeleteSql(CMyString &strSql);

protected:
    MYSQL*          m_conn;
    MYSQL_RES*      m_results;          //�����ָ��
    MYSQL_ROW       m_row;              //��ָ��
    unsigned long* 	m_lengths;          //�ֶγ���
    uint32_t   		m_field_num;        //�ֶ�����
    MYSQL_FIELD*    m_fields;           //�ֶ���Ϣ

    ISimDBEvent*    m_pEvent;           //���ݿ��¼�����
    RESULT_STATUS   m_eResultStatus;    //�����״̬
    CMyString       m_strSQL;           //SQL

private:
    CMyString       m_strCurCharset;    //ϵͳ�ڲ�ʹ�õ��ַ���
    CMyString       m_strOutCharset;    //�ⲿʹ�õ��ַ���

private:
    uint64_t        m_lastHeartbeat;    // �������ʱ��

    int32_t         m_nResultIndex;     // �������ţ��Զ�����ʱ��, -1��ʾ�޽����
};


#endif  //_GFX_MYDB_SIMPLE_MYSQL_H_

