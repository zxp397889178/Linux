/**
@file
@brief 日志类函数
@note Editor: tab=4
@note Platform: Linux
*/

#ifndef _QS_BASELIB_SHMLOG_REAL_LOG_H_
#define  _QS_BASELIB_SHMLOG_REAL_LOG_H_

#include <stdarg.h>

#define LOGRESULT_SUCCESS       1       //Process Success
#define LOGRESULT_FAILURE       2       //Process Failure

#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>

namespace Comm
{

/**
	@brief enum类型，日志级别定义
*/
enum {
	COMM_LOG_DISABLE=0,  /**< 禁止日志输出  */
	COMM_LOG_INFO=1,     /**< info日志级别，日志会输出在info目录下  */
	COMM_LOG_IMPT=1,     /**< important日志级别，用来记录重要操作日志 */
	COMM_LOG_INIT=2,     /**< init日志级别，一般用于模块初始化时的日志输出  */
	COMM_LOG_ERR=2,      /**< error日志级别，运行出现错误时输出，包括系统错误和逻辑错误 */  
	COMM_LOG_MSG=2,      /**< msg日志级别，用来记录非错误输出，但需要记录运行状态的操作 */
	COMM_LOG_MONITOR=2,  /**< monitor日志级别 */
	COMM_LOG_DEBUG=3,    /**< debug日志级别 */
	COMM_LOG_TRACE=4,	 /**< verbose日志级别 */
	COMM_LOG_VERBOSE=4,	 /**< verbose日志级别 */
	COMM_LOG_IN=16,      /**< 基础库输出日志级别 */
	COMM_LOG_ALL=32      /**< 输出所有日志 */
};

/**
	@brief enum类型，线程安全模式设置
*/
enum
{
	COMM_LOG_TM_DEFAULT=0, /**< 默认线程安全模式：关闭 */
	COMM_LOG_TM_DISABLE=0, /**< 关闭线程安全模式 */
	COMM_LOG_TM_ENABLE=1   /**< 打开线程安全模式 */
};

/**
	@brief enum类型，线程共享内存模式设置
*/
enum
{
	COMM_LOG_SHM_DEFAULT=0, 
	COMM_LOG_SHM_DISABLE=0, 
	COMM_LOG_SHM_ENABLE=1   
};

enum
{
	COMM_LOG_CHECKLOGSIZE_ENABLE=0,
	COMM_LOG_CHECKLOGSIZE_DISABLE=1
};


/**
	@brief enum类型，指明每行日志的头输出格式
	@note 当使用线程安全模式时，日志头中的进程号格式为：进程ID,线程ID
*/
enum
{
	COMM_LOG_HEAD_INFO=1,   /**< info日志头格式： <级别> <模块名(进程号)> 4位年-2位月-2位天 24小时:2位分钟:2位秒数 */
	COMM_LOG_HEAD_COMM=2,   /**< 普通日志头格式： <级别> <模块名(进程号)> 4位年-2位月-2位天 24小时:2位分钟:2位秒数 */
	COMM_LOG_HEAD_MONITOR=8, /**< monitor日志头格式：4位年-2位月-2位天 24小时:2位分钟:2位秒数,模块名,主机名称 */
	COMM_LOG_HEAD_NONE=16 /**< 不输出日志头，日志内容完全由调用者控制 */
};


/**
	@brief 初始化日志操作
	@param sModuleName 模块名称，非空，可以调用SetLogModuleName进行更改
	@param iLogLevel 日志级别，可以调用SetLogLevel()进行更改
	@param sPath 日志文件根路径，函数会在该路径下自动创建error和info子目录
	@retval 0 成功
	@retval 非0 失败
	@note 当初始化成功，在完成了所有的日志操作后，必须调用CloseLog()来释放资源
*/
int OpenLog( const char *sModuleName, const int iLogLevel, const char *sPath );

/**
	@brief 初始化日志操作
	@param sModuleName 模块名称，非空，可以调用SetLogModuleName进行更改
	@param iLogLevel 日志级别，可以调用SetLogLevel()进行更改
	@param sPath 日志文件根路径，函数会在该路径下自动创建error和info子目录
	@param iMaxSize 日志文件最大大小，单位为MB
	@retval 0 成功
	@retval 非0 失败
	@note 当初始化成功，在完成了所有的日志操作后，必须调用CloseLog()来释放资源
*/
int OpenLog( const char *sModuleName, const int iLogLevel, const char *sPath, const int iMaxSize );

/**
	@brief 关闭日志操作
	@retval 0 成功
	@retval 非0 失败
	@note
*/
int CloseLog();

/**
	@brief 设置日志中的模块名称
	@param sModuleName 模块名称，非空
	@note 打开TraceLog
*/
int OpenTraceLog( const char *sModuleName, const char *sPath );

int OpenTraceLog( const char *sModuleName, const char *sPath, const int iMaxSize );

/**
	@brief 设置日志中的模块名称
	@param sModuleName 模块名称，非空
	@note
*/
void SetLogModuleName( const char *sModuleName );

/**
	@brief 获取日志中的模块名称
	@retval 模块名称
	@note
*/
const char * GetLogModuleName();

/**
	@brief 设置日志级别
	@param iLogLevel 日志级别
	@note
*/
void SetLogLevel ( const int iLogLevel );

/**
	@brief 获取日志级别
	@retval 日志级别
	@note
*/
const int GetLogLevel();

/**
	@brief 设置日志目录的根路径
	@param sLogPath 模块名称，非空
	@retval 模块名称
	@note
*/
void SetLogPath ( const char *sLogPath );

/**
	@brief 设置日志目录的根路径
	@param sLogPath 模块名称，非空
	@param iMaxSize 日志文件最大大小，单位为MB
	@retval 模块名称
	@note
*/
void SetLogPath ( const char *sLogPath, const int iMaxSize );

/**
	@brief 获取日志目录的根路径
	@retval 目录根路径
	@note
*/
const char * GetLogPath();

/**
	@brief 设置线程安全模式
	@param iThreadMode 线程模式
	@note
*/
void SetLogThreadMode(const int iThreadMode);

/**
	@brief 设置线程共享内存模式 
	@param iShmMode 共享内存模式
	@note
*/
int SetLogShmMode(const int iShmMode);

/**
	@brief 设置是否检查日志大小，必须与共享内存模式一起使用
	@param iCheckLogSize 日志大小检查模式
	@note
*/
void SetLogCheckSizeMode(const int iCheckLogSizeMode);

/**
	@brief 获取线程安全模式设置
	@retval 线程安全模式设置值
	@note
*/
const int GetLogThreadMode();

/**
	@brief 获取线程共享内存模式设置
	@retval 线程共享内存模式设置值
	@note
*/
const int GetLogShmMode();

/**
	@brief 获取日志检查模式
	@retval 日志检查模式
	@note
*/
const int GetLogCheckSizeMode();


/**
	@brief 设置日志session id
	@param iSID 日志session id
	@note 如果设置该值，则每条日志都会打印该id，设置 0 表示取消session id
*/
void SetLogSessionID( const unsigned int iSID );

/**
	@brief 获取日志session id
	@retval 日志session id
	@note
*/
const unsigned int GetLogSessionID();

/**
	@brief 通用日志函数
	@param iLevel 日志级别
	@param sFormat 变长列表
	@note
*/
void Log ( const int iLevel, const char *sFormat, ... )
	__attribute__((format(printf, 2, 3)));

void LogCheckName();

/**
	@brief Info日志函数，日志会记录在info目录下
	@note
*/
void LogInfo ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/**
	@brief Monitor日志函数，日志会记录在monitor目录下
	@note
*/
void LogMonitor ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/**
	@brief Init日志函数，一般用于模块初始化时的日志输出，日志会记录在error目录下
	@note
*/
void LogInit ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/**
	@brief important日志级别，用来记录重要操作日志
*/
void LogImpt ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/**
	@brief Error日志函数，运行出现错误时输出，包括系统错误和逻辑错误，日志会记录在error目录下
	@note
*/
void LogErr ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/**
	@brief Msg日志函数，日志会记录在error目录下
	@note
*/
void LogMsg ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/**
	@brief Debug日志函数，日志会记录在error目录下
	@note
*/
void LogDebug ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/** 不重要的日志，等级4
 * */	
void LogTrace( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));
void LogVerbose( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

void LogInternal( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));


/**
	@brief Trace日志函数，运行输出由于跟踪定位的信息，日志会记录在trace目录下 
	@note
*/
void LogTrace( const char * function, unsigned int uin, const char * sFormat, ... )
	__attribute__((format(printf, 3, 4)));
void LogTrace( const char * moduel, const char * function, unsigned int uin, const char * step, int ret );
void LogTrace( const char * moduel, const char * function, unsigned int uin, const char * step, int ret, const char *sFormat, ... )
	__attribute__((format(printf, 6, 7)));

typedef struct tagCLogImpl CLogImpl_t;

/**
	@brief 基础日志类
*/
class CLog
{
public:

	/**
		@brief 构造函数
		@note
	*/
	CLog();

	/**
		@brief 构造函数
		@param iMaxSize 日志文件最大大小，单位为MB
		@note
	*/
	CLog(const int iMaxSize);

	/**
		@brief 释构函数
		@note
	*/
	~CLog();

	/**
		@brief 获取单一日志类句柄
		@note
	*/
	static CLog * GetDefault();

	/**
		@brief enum类型，指明日志单文件最大及行最大的默认值
	*/
	enum
	{
		eMAX_LOG_SIZE=1600,  /**< 日志单文件最大值，单位MB */
		eMAX_LINE_SIZE=65536 /**< 行日志最大值，单位bytes */
	};

	/**
		@brief 初始化日志操作
		@param sModuleName 模块名称，非空，可以调用SetLogModuleName进行更改
		@param iLogLevel 日志级别，可以调用SetLogLevel()进行更改
		@param sPath 日志文件根路径
		@param iMaxSize 单日志文件最大值
		@retval 0 成功
		@retval 非0 失败
		@note 如果初始化成功，在完成了所有的日志操作后，必须调用Destroy()来释放资源
	*/
	int Init( const char *sModuleName, const int iLogLevel, const char *sPath, const int iMaxSize=eMAX_LOG_SIZE );

	/**
		@brief 关闭日志操作
		@note 如果初始化成功，在完成了所有的日志操作后，必须调用Destroy()来释放资源
	*/
	void Destroy();

	/**
		@brief 设置日志中的模块名称
		@param sModuleName 模块名称，非空
		@note
	*/
	void SetLogModuleName( const char *sModuleName );

	/**
		@brief 获取日志中的模块名称
		@retval 模块名称
		@note
	*/
	const char * GetLogModuleName();

	/**
		@brief 设置日志文件名的前缀
		@param sPrefix 前缀名称，非空
		@note
	*/
	void SetPrefix( const char *sPrefix );

	/**
		@brief 获取日志文件名的前缀
		@retval 文件名前缀
		@note
	*/
	const char * GetPrefix();

	/**
		@brief 设置日志级别
		@param iLogLevel 日志级别
		@note
	*/
	void SetLogLevel ( const int iLogLevel );

	/**
		@brief 获取日志级别
		@retval 日志级别
		@note
	*/
	const int GetLogLevel();

	/**
		@brief 设置日志目录的根路径
		@param sLogPath 模块名称，非空
		@retval 模块名称
		@note
	*/
	void SetLogPath ( const char *sLogPath );

	/**
		@brief 获取日志目录的根路径
		@retval 目录根路径
		@note
	*/
	const char * GetLogPath();

	/**
		@brief 设置日志轮换的时间间隔
		@param iInterval 轮换间隔
	*/
	void SetLogInterval( int iInterval );

	/**
		@brief 获取日志轮换的时间间隔
		@retval 轮换间隔
	*/
	int GetLogInterval();

	/**
		@brief 设置线程安全模式
		@param iThreadMode 线程模式
		@note
	*/
	void SetLogThreadMode(const int iThreadMode);

	/**
		@brief 获取线程安全模式设置
		@retval 线程安全模式设置值
		@note
	*/
	const int GetLogThreadMode();

    /**
      @brief 设置线程共享内存模式 
      @param iShmMode 共享内存模式
      @note
      */
    int SetLogShmMode(const int iShmMode);

    /**
      @brief 获取线程共享内存模式设置
      @retval 线程共享内存模式设置值
      @note
      */
    const int GetLogShmMode();

    /**
      @brief 设置是否检查日志大小，必须与共享内存模式一起使用
      @param iCheckLogSize 日志大小检查模式
      @note
      */
    void SetLogCheckSizeMode(const int iCheckLogSizeMode);

    /**
      @brief 获取日志检查模式
      @retval 日志检查模式
      @note
      */
    const int GetLogCheckSizeMode();

    

	/**
		@brief 设置日志session id
		@param sSID 日志session id，最长 32byte
		@note 如果设置该值，则每条日志都会打印该id，设置 "" 表示取消session id
	 */
	void SetLogSessionID( const char *sSID );

	/**
		@brief 获取日志session id
		@retval 日志session id，非NULL
		@note
	*/
	const char * GetLogSessionID();

	/**
		@brief 通用日志函数
		@param iLevel 日志级别
		@param sFormat 变长列表
		@note 该函数的日志头格式为 COMMLOG_HEAD_COMM 类型，如果要自定义日志头格式，需要使用传入日志头格式的Log函数
	*/
	void Log ( const int iLevel, const char *sFormat, ... );

	/**
		@brief 通用日志函数
		@param iLevel 日志级别
		@param iHeadType 日志头格式
		@param sFormat 变长列表
		@note
	*/
	void Log ( const int iLevel, const int iHeadType, const char *sFormat, ... );

	/**
		@brief 通用日志函数，没有日志级别限制
		@note 该函数的日志头格式为 COMMLOG_HEAD_COMM 类型，如果要自定义日志头格式，需要使用传入日志头格式的Log函数
	*/
	void Log ( const char *sFormat, ... );

	/**
		@brief 通用日志函数
		@param iLevel 日志级别
		@param sFormat 变长列表
		@param args 通过va_start()/va_end()函数生成的参数列表
		@param iHeadType 日志头格式
		@note
	*/
	void Log ( const int iLevel, const char *sFormat, va_list args, const int iHeadType = COMM_LOG_HEAD_COMM );

    void DisableShmMode(); 
private:
	static int InitImpl( CLogImpl_t * ptImpl, const char *sModuleName,
			const int iLogLevel, const char *sPath, const int iMaxSize );
	static int CleanImpl( CLogImpl_t * ptImpl );

	static int CheckLogName( CLogImpl_t * ptImpl );
	static void GetLogName( int iInterval, char * sName, int size );

	int CheckLogSize();
	void GetLogHead( const int iLevel, const int iHeadType, char *sLogHead, const int iSize, int *piHeadLen );
	void GetLogSID( char *sLogSID, const int iSize, int *piSIDLen );

	void Lock();
	void Unlock();

    void MakeLog(char * pcLogBuffer, const int iLevel, const char *sFormat, va_list args, const int iHeadType, int &iLogLen );

private:
	CLogImpl_t * m_ptLogImpl;

	friend void LogCheckName();
};

} // end namespace Comm

#endif //   _QS_BASELIB_SHMLOG_REAL_LOG_H_

