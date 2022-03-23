#ifndef _GFX_MYDB_SIMDB_INTERFACE_
#define _GFX_MYDB_SIMDB_INTERFACE_

#include "BaseCode.h"

#define  VER_MYSQL3         3
#define  VER_MYSQL4         4
#define  VER_MYSQL5         5
#define  RESULT_STORED      0      //一次性取出并存储在本地
#define  RESULT_QUERY       1      //需要时才去取出

#define MYSQL_RESULT    RESULT_STORED

class ISimDBEvent
{
public:
    enum DB_EVENT
    {
        eventDummy = 0,
        eventUpdate,        //UPDATE 语句
        eventInsert,        //INSERT 语句
        eventDelete,        //DELETE 语句
        eventProcedure,     //CALL 存储过程
        eventSelect,        //SELECT 语句
        eventFetchResult,   //取结果集,此事件仅用于出错时提醒
        eventOther          //其他语句
    };

    //事件响应，其中timeCost指本事件耗费的毫秒数
    virtual void OnEvent(const DB_EVENT dbEvent, double timeCost) = 0;

    //出错提醒，指示在执行某事件时出错，
    //需要调用ISimDB的GetLastError()来获取更详细的错误信息
    virtual void OnError(const DB_EVENT dbEvent, const char *pszSql) = 0;

    //有新结果集时，提醒事件管理重新构建字段信息
    virtual void OnNewResult(void* results, void* fields, uint32_t field_num) = 0;
    //virtual void OnNewResult(MYSQL_RES *results, MYSQL_FIELD *fields, uint32_t field_num) = 0;

    //有新的行数据时
    virtual void OnNewRow(char **row, uint32_t *lengths) = 0;
    //virtual void OnNewRow(MYSQL_ROW row, uint32_t *lengths) = 0;

    //设置连接的别名,用于指示之用
    virtual void SetAppName(const char *pszAppName) = 0;
	

	virtual ~ISimDBEvent() {}
};

class ISimDB
{
public:
    enum { MAX_SQL_BUFFER = 40960 };
    enum RESULT_STATUS
    {
        statusDummy,            //初始状态
        statusHasSql,           //有SQL命令
        statusQuery,            //已经执行成功
        statusHasResult,        //已有结果集
        statusHasError          //出错了
    };

    virtual void release(void) = 0;

    /* 打开数据库连接,返回值：
       0    连接成功
       -1   参数不合法
       -2   mysql初始化失败
       -3   mysql连接失败，估计是参数出错
      注意: appname暂时无用处
            pEvent为NULL时,不响应事件消息
    */
    virtual int32_t Open(const char *user_name, const char *passwd, 
        const char *host, const char *db=NULL, int32_t port=0, 
        ISimDBEvent *pEvent = NULL, const char *appname=NULL,
        const char *charset="utf8") = 0;

    virtual void BindEvent(ISimDBEvent *pEvent = NULL) = 0;

    //关闭SQL连接
    virtual void Close(void) = 0;

    //完整复制一份MySql连接
    virtual ISimDB* Duplicate(ISimDBEvent *pEvent = 0) = 0;

    //传入SQL命令
    virtual const char* Cmd(const char *fmt, ...) = 0;

	// escape mysql string, return is src.length()
	virtual const char* SafeString(std::string &src) { return src.c_str(); }
    /* 执行SQL命令，返回值
       0    对于SELECT或存储过程，表示执行成功，SELECT成功之后必须调用MORE取结果集；对于INSERT/UPDATE/DELETE 表示无符合条件的行
       >0   对于INSERT/UPDATE/DELETE 表示处理的行数
       -1   无SQL语句
       -2   SQL语句执行出错，可以用GetLastError()来获得错误信息
    */
    virtual int32_t Exec(void) = 0;
    //执行多条Exec语句
    virtual int32_t MoreExec(void) = 0;

    /* 获取结果集，仅对于SELECT或有返回值的存储过程有效
       返回 true 表示还有结果行未处理
       返回 false 时需要与GetLastError()结合判断，若GetLastError()返回
            0   表示所有结果行都处理完了
            -1  数据库连接未初始化
            >0  表示出错了，具体内容在GetLastError()函数中的文本信息中提示
    */
    virtual bool More(void) = 0;

    /* 取消所有结果集，仅在More()还有数据时调用有效，
       对于INSERT/UPDATE/DELETE 及存储过程调用时无效
       在出错时，系统内部会自动调用Cancel()来取消未处理的结果集
    */
    virtual void Cancel(void) = 0;

    /* 获取当前行指定列的结果信息
        buffer      [OUT] 传出的缓冲区指针
        cbBuffer    [OUT] 缓冲区大小
       返回值：
        -1 指定的列不存在
        0  处理成功
    */
    virtual int32_t GetData(const char* field_name, byte_t *&buffer, size_t &cbBuffer) = 0;
    virtual int32_t GetData(int32_t field_index, byte_t *&buffer, size_t &cbBuffer) = 0;

    /* 获取MySQL最近的错误信息
       返回值： 
        -1  数据库连接未初始化
        >0  表示出错了，具体内容在GetLastError()函数中的文本信息中提示
      注意：在调用 Open(),Close(),Duplicate(),Cancel() 函数时，GetLastError()返回的信息无效
    */
    virtual int32_t GetLastError(char *buffer=NULL, size_t cbBuffer=0) = 0;

    virtual void SetHandler(ISimDBEvent *pEvent) = 0;

	// 在执行完More()之后，可以获得列数量
	virtual uint32_t GetFieldCount(void) { return 0; }

    // ping, 
    // timeout 表示与最近一次执行的时间的间隔，以秒数计
    // 返回值：1 表示调用了一次ping, 0 表示无需调用ping -1 表示出错
    virtual int32_t Ping(uint32_t timeout) = 0;

    // 对多结果集，获得当前结果集的序号
    // 返回值：-1 表示结果集已轮询完毕，或无结果集
    //         >0 第 n+1 个结果集
    virtual int getResultIndex(void)  { return -1; }
public:
    //获取数据结果
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

//字符集处理函数
public:
    //设置外部使用的字符集编码
    virtual void SetOutCharset(const char *pszCharset) = 0;
    //设置系统内部使用的字符集编码，一般在Open是已指定
    virtual bool SetCharset(const char *pszCharset) = 0;

protected:
    virtual ~ISimDB() {}

};
//typedef TObjectPtrGuard<ISimDB, policy_ptr_release<ISimDB> >                CAutoDB;
typedef TObjectPtrGuard<ISimDB, policy_ptr_release<ISimDB> >	CAutoDB; 

#endif  //_GFX_MYDB_SIMDB_INTERFACE_


