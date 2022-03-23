/**
@file
@brief ��־�ຯ��
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
	@brief enum���ͣ���־������
*/
enum {
	COMM_LOG_DISABLE=0,  /**< ��ֹ��־���  */
	COMM_LOG_INFO=1,     /**< info��־������־�������infoĿ¼��  */
	COMM_LOG_IMPT=1,     /**< important��־����������¼��Ҫ������־ */
	COMM_LOG_INIT=2,     /**< init��־����һ������ģ���ʼ��ʱ����־���  */
	COMM_LOG_ERR=2,      /**< error��־�������г��ִ���ʱ���������ϵͳ������߼����� */  
	COMM_LOG_MSG=2,      /**< msg��־����������¼�Ǵ������������Ҫ��¼����״̬�Ĳ��� */
	COMM_LOG_MONITOR=2,  /**< monitor��־���� */
	COMM_LOG_DEBUG=3,    /**< debug��־���� */
	COMM_LOG_TRACE=4,	 /**< verbose��־���� */
	COMM_LOG_VERBOSE=4,	 /**< verbose��־���� */
	COMM_LOG_IN=16,      /**< �����������־���� */
	COMM_LOG_ALL=32      /**< ���������־ */
};

/**
	@brief enum���ͣ��̰߳�ȫģʽ����
*/
enum
{
	COMM_LOG_TM_DEFAULT=0, /**< Ĭ���̰߳�ȫģʽ���ر� */
	COMM_LOG_TM_DISABLE=0, /**< �ر��̰߳�ȫģʽ */
	COMM_LOG_TM_ENABLE=1   /**< ���̰߳�ȫģʽ */
};

/**
	@brief enum���ͣ��̹߳����ڴ�ģʽ����
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
	@brief enum���ͣ�ָ��ÿ����־��ͷ�����ʽ
	@note ��ʹ���̰߳�ȫģʽʱ����־ͷ�еĽ��̺Ÿ�ʽΪ������ID,�߳�ID
*/
enum
{
	COMM_LOG_HEAD_INFO=1,   /**< info��־ͷ��ʽ�� <����> <ģ����(���̺�)> 4λ��-2λ��-2λ�� 24Сʱ:2λ����:2λ���� */
	COMM_LOG_HEAD_COMM=2,   /**< ��ͨ��־ͷ��ʽ�� <����> <ģ����(���̺�)> 4λ��-2λ��-2λ�� 24Сʱ:2λ����:2λ���� */
	COMM_LOG_HEAD_MONITOR=8, /**< monitor��־ͷ��ʽ��4λ��-2λ��-2λ�� 24Сʱ:2λ����:2λ����,ģ����,�������� */
	COMM_LOG_HEAD_NONE=16 /**< �������־ͷ����־������ȫ�ɵ����߿��� */
};


/**
	@brief ��ʼ����־����
	@param sModuleName ģ�����ƣ��ǿգ����Ե���SetLogModuleName���и���
	@param iLogLevel ��־���𣬿��Ե���SetLogLevel()���и���
	@param sPath ��־�ļ���·�����������ڸ�·�����Զ�����error��info��Ŀ¼
	@retval 0 �ɹ�
	@retval ��0 ʧ��
	@note ����ʼ���ɹ�������������е���־�����󣬱������CloseLog()���ͷ���Դ
*/
int OpenLog( const char *sModuleName, const int iLogLevel, const char *sPath );

/**
	@brief ��ʼ����־����
	@param sModuleName ģ�����ƣ��ǿգ����Ե���SetLogModuleName���и���
	@param iLogLevel ��־���𣬿��Ե���SetLogLevel()���и���
	@param sPath ��־�ļ���·�����������ڸ�·�����Զ�����error��info��Ŀ¼
	@param iMaxSize ��־�ļ�����С����λΪMB
	@retval 0 �ɹ�
	@retval ��0 ʧ��
	@note ����ʼ���ɹ�������������е���־�����󣬱������CloseLog()���ͷ���Դ
*/
int OpenLog( const char *sModuleName, const int iLogLevel, const char *sPath, const int iMaxSize );

/**
	@brief �ر���־����
	@retval 0 �ɹ�
	@retval ��0 ʧ��
	@note
*/
int CloseLog();

/**
	@brief ������־�е�ģ������
	@param sModuleName ģ�����ƣ��ǿ�
	@note ��TraceLog
*/
int OpenTraceLog( const char *sModuleName, const char *sPath );

int OpenTraceLog( const char *sModuleName, const char *sPath, const int iMaxSize );

/**
	@brief ������־�е�ģ������
	@param sModuleName ģ�����ƣ��ǿ�
	@note
*/
void SetLogModuleName( const char *sModuleName );

/**
	@brief ��ȡ��־�е�ģ������
	@retval ģ������
	@note
*/
const char * GetLogModuleName();

/**
	@brief ������־����
	@param iLogLevel ��־����
	@note
*/
void SetLogLevel ( const int iLogLevel );

/**
	@brief ��ȡ��־����
	@retval ��־����
	@note
*/
const int GetLogLevel();

/**
	@brief ������־Ŀ¼�ĸ�·��
	@param sLogPath ģ�����ƣ��ǿ�
	@retval ģ������
	@note
*/
void SetLogPath ( const char *sLogPath );

/**
	@brief ������־Ŀ¼�ĸ�·��
	@param sLogPath ģ�����ƣ��ǿ�
	@param iMaxSize ��־�ļ�����С����λΪMB
	@retval ģ������
	@note
*/
void SetLogPath ( const char *sLogPath, const int iMaxSize );

/**
	@brief ��ȡ��־Ŀ¼�ĸ�·��
	@retval Ŀ¼��·��
	@note
*/
const char * GetLogPath();

/**
	@brief �����̰߳�ȫģʽ
	@param iThreadMode �߳�ģʽ
	@note
*/
void SetLogThreadMode(const int iThreadMode);

/**
	@brief �����̹߳����ڴ�ģʽ 
	@param iShmMode �����ڴ�ģʽ
	@note
*/
int SetLogShmMode(const int iShmMode);

/**
	@brief �����Ƿ�����־��С�������빲���ڴ�ģʽһ��ʹ��
	@param iCheckLogSize ��־��С���ģʽ
	@note
*/
void SetLogCheckSizeMode(const int iCheckLogSizeMode);

/**
	@brief ��ȡ�̰߳�ȫģʽ����
	@retval �̰߳�ȫģʽ����ֵ
	@note
*/
const int GetLogThreadMode();

/**
	@brief ��ȡ�̹߳����ڴ�ģʽ����
	@retval �̹߳����ڴ�ģʽ����ֵ
	@note
*/
const int GetLogShmMode();

/**
	@brief ��ȡ��־���ģʽ
	@retval ��־���ģʽ
	@note
*/
const int GetLogCheckSizeMode();


/**
	@brief ������־session id
	@param iSID ��־session id
	@note ������ø�ֵ����ÿ����־�����ӡ��id������ 0 ��ʾȡ��session id
*/
void SetLogSessionID( const unsigned int iSID );

/**
	@brief ��ȡ��־session id
	@retval ��־session id
	@note
*/
const unsigned int GetLogSessionID();

/**
	@brief ͨ����־����
	@param iLevel ��־����
	@param sFormat �䳤�б�
	@note
*/
void Log ( const int iLevel, const char *sFormat, ... )
	__attribute__((format(printf, 2, 3)));

void LogCheckName();

/**
	@brief Info��־��������־���¼��infoĿ¼��
	@note
*/
void LogInfo ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/**
	@brief Monitor��־��������־���¼��monitorĿ¼��
	@note
*/
void LogMonitor ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/**
	@brief Init��־������һ������ģ���ʼ��ʱ����־�������־���¼��errorĿ¼��
	@note
*/
void LogInit ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/**
	@brief important��־����������¼��Ҫ������־
*/
void LogImpt ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/**
	@brief Error��־���������г��ִ���ʱ���������ϵͳ������߼�������־���¼��errorĿ¼��
	@note
*/
void LogErr ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/**
	@brief Msg��־��������־���¼��errorĿ¼��
	@note
*/
void LogMsg ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/**
	@brief Debug��־��������־���¼��errorĿ¼��
	@note
*/
void LogDebug ( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

/** ����Ҫ����־���ȼ�4
 * */	
void LogTrace( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));
void LogVerbose( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));

void LogInternal( const char *sFormat, ... )
	__attribute__((format(printf, 1, 2)));


/**
	@brief Trace��־����������������ڸ��ٶ�λ����Ϣ����־���¼��traceĿ¼�� 
	@note
*/
void LogTrace( const char * function, unsigned int uin, const char * sFormat, ... )
	__attribute__((format(printf, 3, 4)));
void LogTrace( const char * moduel, const char * function, unsigned int uin, const char * step, int ret );
void LogTrace( const char * moduel, const char * function, unsigned int uin, const char * step, int ret, const char *sFormat, ... )
	__attribute__((format(printf, 6, 7)));

typedef struct tagCLogImpl CLogImpl_t;

/**
	@brief ������־��
*/
class CLog
{
public:

	/**
		@brief ���캯��
		@note
	*/
	CLog();

	/**
		@brief ���캯��
		@param iMaxSize ��־�ļ�����С����λΪMB
		@note
	*/
	CLog(const int iMaxSize);

	/**
		@brief �͹�����
		@note
	*/
	~CLog();

	/**
		@brief ��ȡ��һ��־����
		@note
	*/
	static CLog * GetDefault();

	/**
		@brief enum���ͣ�ָ����־���ļ����������Ĭ��ֵ
	*/
	enum
	{
		eMAX_LOG_SIZE=1600,  /**< ��־���ļ����ֵ����λMB */
		eMAX_LINE_SIZE=65536 /**< ����־���ֵ����λbytes */
	};

	/**
		@brief ��ʼ����־����
		@param sModuleName ģ�����ƣ��ǿգ����Ե���SetLogModuleName���и���
		@param iLogLevel ��־���𣬿��Ե���SetLogLevel()���и���
		@param sPath ��־�ļ���·��
		@param iMaxSize ����־�ļ����ֵ
		@retval 0 �ɹ�
		@retval ��0 ʧ��
		@note �����ʼ���ɹ�������������е���־�����󣬱������Destroy()���ͷ���Դ
	*/
	int Init( const char *sModuleName, const int iLogLevel, const char *sPath, const int iMaxSize=eMAX_LOG_SIZE );

	/**
		@brief �ر���־����
		@note �����ʼ���ɹ�������������е���־�����󣬱������Destroy()���ͷ���Դ
	*/
	void Destroy();

	/**
		@brief ������־�е�ģ������
		@param sModuleName ģ�����ƣ��ǿ�
		@note
	*/
	void SetLogModuleName( const char *sModuleName );

	/**
		@brief ��ȡ��־�е�ģ������
		@retval ģ������
		@note
	*/
	const char * GetLogModuleName();

	/**
		@brief ������־�ļ�����ǰ׺
		@param sPrefix ǰ׺���ƣ��ǿ�
		@note
	*/
	void SetPrefix( const char *sPrefix );

	/**
		@brief ��ȡ��־�ļ�����ǰ׺
		@retval �ļ���ǰ׺
		@note
	*/
	const char * GetPrefix();

	/**
		@brief ������־����
		@param iLogLevel ��־����
		@note
	*/
	void SetLogLevel ( const int iLogLevel );

	/**
		@brief ��ȡ��־����
		@retval ��־����
		@note
	*/
	const int GetLogLevel();

	/**
		@brief ������־Ŀ¼�ĸ�·��
		@param sLogPath ģ�����ƣ��ǿ�
		@retval ģ������
		@note
	*/
	void SetLogPath ( const char *sLogPath );

	/**
		@brief ��ȡ��־Ŀ¼�ĸ�·��
		@retval Ŀ¼��·��
		@note
	*/
	const char * GetLogPath();

	/**
		@brief ������־�ֻ���ʱ����
		@param iInterval �ֻ����
	*/
	void SetLogInterval( int iInterval );

	/**
		@brief ��ȡ��־�ֻ���ʱ����
		@retval �ֻ����
	*/
	int GetLogInterval();

	/**
		@brief �����̰߳�ȫģʽ
		@param iThreadMode �߳�ģʽ
		@note
	*/
	void SetLogThreadMode(const int iThreadMode);

	/**
		@brief ��ȡ�̰߳�ȫģʽ����
		@retval �̰߳�ȫģʽ����ֵ
		@note
	*/
	const int GetLogThreadMode();

    /**
      @brief �����̹߳����ڴ�ģʽ 
      @param iShmMode �����ڴ�ģʽ
      @note
      */
    int SetLogShmMode(const int iShmMode);

    /**
      @brief ��ȡ�̹߳����ڴ�ģʽ����
      @retval �̹߳����ڴ�ģʽ����ֵ
      @note
      */
    const int GetLogShmMode();

    /**
      @brief �����Ƿ�����־��С�������빲���ڴ�ģʽһ��ʹ��
      @param iCheckLogSize ��־��С���ģʽ
      @note
      */
    void SetLogCheckSizeMode(const int iCheckLogSizeMode);

    /**
      @brief ��ȡ��־���ģʽ
      @retval ��־���ģʽ
      @note
      */
    const int GetLogCheckSizeMode();

    

	/**
		@brief ������־session id
		@param sSID ��־session id��� 32byte
		@note ������ø�ֵ����ÿ����־�����ӡ��id������ "" ��ʾȡ��session id
	 */
	void SetLogSessionID( const char *sSID );

	/**
		@brief ��ȡ��־session id
		@retval ��־session id����NULL
		@note
	*/
	const char * GetLogSessionID();

	/**
		@brief ͨ����־����
		@param iLevel ��־����
		@param sFormat �䳤�б�
		@note �ú�������־ͷ��ʽΪ COMMLOG_HEAD_COMM ���ͣ����Ҫ�Զ�����־ͷ��ʽ����Ҫʹ�ô�����־ͷ��ʽ��Log����
	*/
	void Log ( const int iLevel, const char *sFormat, ... );

	/**
		@brief ͨ����־����
		@param iLevel ��־����
		@param iHeadType ��־ͷ��ʽ
		@param sFormat �䳤�б�
		@note
	*/
	void Log ( const int iLevel, const int iHeadType, const char *sFormat, ... );

	/**
		@brief ͨ����־������û����־��������
		@note �ú�������־ͷ��ʽΪ COMMLOG_HEAD_COMM ���ͣ����Ҫ�Զ�����־ͷ��ʽ����Ҫʹ�ô�����־ͷ��ʽ��Log����
	*/
	void Log ( const char *sFormat, ... );

	/**
		@brief ͨ����־����
		@param iLevel ��־����
		@param sFormat �䳤�б�
		@param args ͨ��va_start()/va_end()�������ɵĲ����б�
		@param iHeadType ��־ͷ��ʽ
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

