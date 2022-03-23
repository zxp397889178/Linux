#ifndef _GFX_MYDB_SIMDB_INTERFACE_
#define _GFX_MYDB_SIMDB_INTERFACE_

#include "BaseCode.h"

#define  VER_MYSQL3         3
#define  VER_MYSQL4         4
#define  VER_MYSQL5         5
#define  RESULT_STORED      0      //һ����ȡ�����洢�ڱ���
#define  RESULT_QUERY       1      //��Ҫʱ��ȥȡ��

#define MYSQL_RESULT    RESULT_STORED

class ISimDBEvent
{
public:
    enum DB_EVENT
    {
        eventDummy = 0,
        eventUpdate,        //UPDATE ���
        eventInsert,        //INSERT ���
        eventDelete,        //DELETE ���
        eventProcedure,     //CALL �洢����
        eventSelect,        //SELECT ���
        eventFetchResult,   //ȡ�����,���¼������ڳ���ʱ����
        eventOther          //�������
    };

    //�¼���Ӧ������timeCostָ���¼��ķѵĺ�����
    virtual void OnEvent(const DB_EVENT dbEvent, double timeCost) = 0;

    //�������ѣ�ָʾ��ִ��ĳ�¼�ʱ����
    //��Ҫ����ISimDB��GetLastError()����ȡ����ϸ�Ĵ�����Ϣ
    virtual void OnError(const DB_EVENT dbEvent, const char *pszSql) = 0;

    //���½����ʱ�������¼��������¹����ֶ���Ϣ
    virtual void OnNewResult(void* results, void* fields, uint32_t field_num) = 0;
    //virtual void OnNewResult(MYSQL_RES *results, MYSQL_FIELD *fields, uint32_t field_num) = 0;

    //���µ�������ʱ
    virtual void OnNewRow(char **row, uint32_t *lengths) = 0;
    //virtual void OnNewRow(MYSQL_ROW row, uint32_t *lengths) = 0;

    //�������ӵı���,����ָʾ֮��
    virtual void SetAppName(const char *pszAppName) = 0;
	

	virtual ~ISimDBEvent() {}
};

class ISimDB
{
public:
    enum { MAX_SQL_BUFFER = 40960 };
    enum RESULT_STATUS
    {
        statusDummy,            //��ʼ״̬
        statusHasSql,           //��SQL����
        statusQuery,            //�Ѿ�ִ�гɹ�
        statusHasResult,        //���н����
        statusHasError          //������
    };

    virtual void release(void) = 0;

    /* �����ݿ�����,����ֵ��
       0    ���ӳɹ�
       -1   �������Ϸ�
       -2   mysql��ʼ��ʧ��
       -3   mysql����ʧ�ܣ������ǲ�������
      ע��: appname��ʱ���ô�
            pEventΪNULLʱ,����Ӧ�¼���Ϣ
    */
    virtual int32_t Open(const char *user_name, const char *passwd, 
        const char *host, const char *db=NULL, int32_t port=0, 
        ISimDBEvent *pEvent = NULL, const char *appname=NULL,
        const char *charset="utf8") = 0;

    virtual void BindEvent(ISimDBEvent *pEvent = NULL) = 0;

    //�ر�SQL����
    virtual void Close(void) = 0;

    //��������һ��MySql����
    virtual ISimDB* Duplicate(ISimDBEvent *pEvent = 0) = 0;

    //����SQL����
    virtual const char* Cmd(const char *fmt, ...) = 0;

	// escape mysql string, return is src.length()
	virtual const char* SafeString(std::string &src) { return src.c_str(); }
    /* ִ��SQL�������ֵ
       0    ����SELECT��洢���̣���ʾִ�гɹ���SELECT�ɹ�֮��������MOREȡ�����������INSERT/UPDATE/DELETE ��ʾ�޷�����������
       >0   ����INSERT/UPDATE/DELETE ��ʾ���������
       -1   ��SQL���
       -2   SQL���ִ�г���������GetLastError()����ô�����Ϣ
    */
    virtual int32_t Exec(void) = 0;
    //ִ�ж���Exec���
    virtual int32_t MoreExec(void) = 0;

    /* ��ȡ�������������SELECT���з���ֵ�Ĵ洢������Ч
       ���� true ��ʾ���н����δ����
       ���� false ʱ��Ҫ��GetLastError()����жϣ���GetLastError()����
            0   ��ʾ���н���ж���������
            -1  ���ݿ�����δ��ʼ��
            >0  ��ʾ�����ˣ�����������GetLastError()�����е��ı���Ϣ����ʾ
    */
    virtual bool More(void) = 0;

    /* ȡ�����н����������More()��������ʱ������Ч��
       ����INSERT/UPDATE/DELETE ���洢���̵���ʱ��Ч
       �ڳ���ʱ��ϵͳ�ڲ����Զ�����Cancel()��ȡ��δ����Ľ����
    */
    virtual void Cancel(void) = 0;

    /* ��ȡ��ǰ��ָ���еĽ����Ϣ
        buffer      [OUT] �����Ļ�����ָ��
        cbBuffer    [OUT] ��������С
       ����ֵ��
        -1 ָ�����в�����
        0  ����ɹ�
    */
    virtual int32_t GetData(const char* field_name, byte_t *&buffer, size_t &cbBuffer) = 0;
    virtual int32_t GetData(int32_t field_index, byte_t *&buffer, size_t &cbBuffer) = 0;

    /* ��ȡMySQL����Ĵ�����Ϣ
       ����ֵ�� 
        -1  ���ݿ�����δ��ʼ��
        >0  ��ʾ�����ˣ�����������GetLastError()�����е��ı���Ϣ����ʾ
      ע�⣺�ڵ��� Open(),Close(),Duplicate(),Cancel() ����ʱ��GetLastError()���ص���Ϣ��Ч
    */
    virtual int32_t GetLastError(char *buffer=NULL, size_t cbBuffer=0) = 0;

    virtual void SetHandler(ISimDBEvent *pEvent) = 0;

	// ��ִ����More()֮�󣬿��Ի��������
	virtual uint32_t GetFieldCount(void) { return 0; }

    // ping, 
    // timeout ��ʾ�����һ��ִ�е�ʱ��ļ������������
    // ����ֵ��1 ��ʾ������һ��ping, 0 ��ʾ�������ping -1 ��ʾ����
    virtual int32_t Ping(uint32_t timeout) = 0;

    // �Զ���������õ�ǰ����������
    // ����ֵ��-1 ��ʾ���������ѯ��ϣ����޽����
    //         >0 �� n+1 �������
    virtual int getResultIndex(void)  { return -1; }
public:
    //��ȡ���ݽ��
	virtual CMyString GetString(const char *field_name) = 0;
	virtual CMyString GetString(int32_t field_index) = 0;
    virtual const char* GetString(const char *field_name, char *szText, size_t cbText) = 0;
    virtual const char* GetString(int32_t field_index, char *szText, size_t cbText) = 0;
    virtual time_t GetTime(const char *field_name) = 0;
    virtual time_t GetTime(int32_t field_index) = 0;
    virtual int32_t GetInt(const char *field_name) = 0;
    virtual int32_t GetInt(int32_t field_index) = 0;
    virtual uint32_t GetUInt(const char *field_name) = 0;
    virtual uint32_t GetUInt(int32_t field_index) = 0;
    virtual int64_t GetInt64(const char *field_name) = 0;
    virtual int64_t GetInt64(int32_t field_index) = 0;
    virtual uint64_t GetUInt64(const char *field_name) = 0;
    virtual uint64_t GetUInt64(int32_t field_index) = 0;
    virtual float GetFloat(const char *field_name) = 0;
    virtual float GetFloat(int32_t field_index) = 0;
    virtual double GetDouble(const char *field_name) = 0;
    virtual double GetDouble(int32_t field_index) = 0;

//�ַ���������
public:
    //�����ⲿʹ�õ��ַ�������
    virtual void SetOutCharset(const char *pszCharset) = 0;
    //����ϵͳ�ڲ�ʹ�õ��ַ������룬һ����Open����ָ��
    virtual bool SetCharset(const char *pszCharset) = 0;

protected:
    virtual ~ISimDB() {}

};
//typedef TObjectPtrGuard<ISimDB, policy_ptr_release<ISimDB> >                CAutoDB;
typedef TObjectPtrGuard<ISimDB, policy_ptr_release<ISimDB> >	CAutoDB; 

#endif  //_GFX_MYDB_SIMDB_INTERFACE_


