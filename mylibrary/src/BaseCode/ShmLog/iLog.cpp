
#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif 

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>
#include <stdarg.h>

#include "iLog.h"

#include "iLogShm.h"

//#include "ossattrapi/iOssAttr4Svr.h"

using namespace std;

namespace Comm
{


typedef struct tagCLogImpl {
	char sModuleName[ 64 ];
	int iLevel;
	char sPath[ 256 ];
	unsigned long long iMaxSize; // in bytes 
	int iThreadMode;
	int iInterval;
	char sPrefix[ 64 ];

	char sLogName[ 256 ];
	int iLogFD;
	int iSizeCheckTime;
	unsigned long long iUsedSize;
	char *pcLogBuffer;

	char sSID[ 32 ];

    int iShmMode;

    LogShmQueue * ptLogShmQueue;

    int iCheckLogSizeMode;
} CLogImpl_t;

#define LOG_WITH_VALIST(handler, level, format, headtype)  \
{                                                          \
	va_list args;                                          \
	va_start ( args, format );                             \
	(handler)->Log ( level, format, args, headtype );      \
	va_end ( args );                                       \
}

//================================================
static CLog g_oBase;
static CLog g_oDebug;
static CLog g_oInfo;
static CLog g_oMonitor;
static CLog g_oTrace;

//default close the TraceLog
static bool g_isOpenTraceLog = false;

static pthread_key_t g_logkey;
static pthread_once_t g_logkey_once = PTHREAD_ONCE_INIT;
static void LogThreadModeMakeKey()
{
    (void) pthread_key_create(&g_logkey, NULL);
}

int OpenLog( const char *sModuleName, const int iLogLevel, const char *sPath )
{
	return OpenLog(sModuleName,iLogLevel,sPath,CLog::eMAX_LOG_SIZE);
}

int OpenLog( const char *sModuleName, const int iLogLevel, const char *sPath, const int iMaxSize )
{
	if ( (NULL == sModuleName) || (NULL == sPath) )
	{
		return -2;
	}

	int iRet = 0;

	{
		iRet = g_oBase.Init ( sModuleName, iLogLevel, sPath, iMaxSize );
		if ( 0 != iRet )
		{
			return iRet;
		}
	}

	char sSubPath[512] = { 0 };
	{
		snprintf ( sSubPath, sizeof(sSubPath), "%s/error", sPath );
		iRet = g_oDebug.Init ( sModuleName, iLogLevel, sSubPath, iMaxSize );
		if ( 0 != iRet )
		{
			return iRet;
		}
	}

	{
		snprintf ( sSubPath, sizeof(sSubPath), "%s/info", sPath );
		iRet = g_oInfo.Init ( sModuleName, COMM_LOG_ALL, sSubPath, iMaxSize );
		if ( 0 != iRet )
		{
			return iRet;
		}
	}

	{
		snprintf ( sSubPath, sizeof(sSubPath), "%s/monitor", sPath );
		iRet = g_oMonitor.Init ( sModuleName, COMM_LOG_ALL, sSubPath,iMaxSize );
		if ( 0 != iRet )
		{
			return iRet;
		}
	}

	return iRet;
}

int CloseLog()
{
	g_oBase.Destroy();
	g_oDebug.Destroy();
	g_oInfo.Destroy();
	g_oMonitor.Destroy();

	if ( g_isOpenTraceLog )
		g_oTrace.Destroy();
	
	return 0;
}

int OpenTraceLog( const char *sModuleName, const char *sPath)
{

	return OpenTraceLog(sModuleName, sPath, CLog::eMAX_LOG_SIZE);
}



int OpenTraceLog( const char *sModuleName, const char *sPath, const int iMaxSize )
{
	if ( NULL == sModuleName || NULL == sPath )
	{
		return -2;
	}

	char sSubPath[512] = { 0 };
	snprintf ( sSubPath, sizeof(sSubPath), "%s/trace", sPath );
	int iRet = g_oTrace.Init ( sModuleName, COMM_LOG_ALL, sSubPath, iMaxSize );

	if ( 0 == iRet )
		g_isOpenTraceLog = true;

	return iRet;
}


void SetLogModuleName( const char *sModuleName )
{
	g_oBase.SetLogModuleName(sModuleName);
	g_oDebug.SetLogModuleName(sModuleName);
	g_oInfo.SetLogModuleName(sModuleName);
	g_oMonitor.SetLogModuleName(sModuleName);
}

const char * GetLogModuleName()
{
	return g_oBase.GetLogModuleName();
}

void SetLogLevel ( const int iLogLevel )
{
	g_oBase.SetLogLevel(iLogLevel);
	g_oDebug.SetLogLevel(iLogLevel);
}

const int GetLogLevel()
{
	return g_oBase.GetLogLevel();
}

void SetLogPath ( const char *sLogPath)
{
	return SetLogPath(sLogPath, CLog::eMAX_LOG_SIZE);
}

void SetLogPath ( const char *sLogPath, const int iMaxSize )
{
	if ( NULL == sLogPath )
	{
		return;
	}

	if ( 0 == strcmp(sLogPath, g_oBase.GetLogPath()) )
	{
		return;
	}

	/* reinit */
	OpenLog ( g_oBase.GetLogModuleName(), g_oBase.GetLogLevel(), sLogPath, iMaxSize );
}

const char * GetLogPath()
{
	return g_oBase.GetLogPath();
}

void SetLogThreadMode(const int iThreadMode)
{
	g_oBase.SetLogThreadMode(iThreadMode);
	g_oDebug.SetLogThreadMode(iThreadMode);
	g_oInfo.SetLogThreadMode(iThreadMode);
	g_oMonitor.SetLogThreadMode(iThreadMode);
	g_oTrace.SetLogThreadMode(iThreadMode);
}

int SetLogShmMode(const int iShmMode)
{
    if( 0 != g_oDebug.SetLogShmMode(iShmMode) ) {
        g_oDebug.DisableShmMode();
        return -1;
    }
    return 0;
}

const int GetLogThreadMode()
{
	return g_oBase.GetLogThreadMode();
}

const int GetLogShmMode()
{
	return g_oDebug.GetLogShmMode();
}


void SetLogCheckSizeMode(const int iCheckLogSizeMode)
{
    g_oDebug.SetLogCheckSizeMode(iCheckLogSizeMode);
}

const int GetLogCheckSizeMode() 
{
	return g_oDebug.GetLogCheckSizeMode();
}

void SetLogSessionID( const unsigned int iSID )
{
	char buffer[32] = { 0 };
	if ( 0 != iSID )
	{
		snprintf ( buffer, sizeof(buffer), "%u", iSID );
	}

	g_oBase.SetLogSessionID(buffer);
	g_oDebug.SetLogSessionID(buffer);
	g_oInfo.SetLogSessionID(buffer);
	g_oMonitor.SetLogSessionID(buffer);
}

const unsigned int GetLogSessionID()
{
	return strtol(g_oBase.GetLogSessionID(), NULL, 10);
}

void Log ( const int iLevel, const char *sFormat, ... )
{
	if ( COMM_LOG_INFO == iLevel )
	{
		LOG_WITH_VALIST(&g_oInfo, COMM_LOG_INFO, sFormat, COMM_LOG_HEAD_INFO);
	}
	else if ( COMM_LOG_MONITOR == iLevel )
	{
		LOG_WITH_VALIST(&g_oMonitor, COMM_LOG_MONITOR, sFormat, COMM_LOG_HEAD_MONITOR);
	}
	else
	{
		LOG_WITH_VALIST(&g_oDebug, iLevel, sFormat, COMM_LOG_HEAD_COMM);
	}
}

void LogCheckName()
{
	CLog::CheckLogName( g_oDebug.m_ptLogImpl );
}

void LogInfo ( const char *sFormat, ... )
{
	LOG_WITH_VALIST(&g_oInfo, COMM_LOG_INFO, sFormat, COMM_LOG_HEAD_INFO);
}

void LogMonitor ( const char *sFormat, ... )
{
	LOG_WITH_VALIST(&g_oMonitor, COMM_LOG_MONITOR, sFormat, COMM_LOG_HEAD_MONITOR);
}

void LogInit ( const char *sFormat, ... )
{
	LOG_WITH_VALIST(&g_oDebug, COMM_LOG_INIT, sFormat, COMM_LOG_HEAD_COMM);
}

void LogImpt ( const char *sFormat, ... )
{
	LOG_WITH_VALIST(&g_oDebug, COMM_LOG_IMPT, sFormat, COMM_LOG_HEAD_COMM);
}

void LogErr ( const char *sFormat, ... )
{
	LOG_WITH_VALIST(&g_oDebug, COMM_LOG_ERR, sFormat, COMM_LOG_HEAD_COMM);
}

void LogDebug ( const char *sFormat, ... )
{
	LOG_WITH_VALIST(&g_oDebug, COMM_LOG_DEBUG, sFormat, COMM_LOG_HEAD_COMM);
}

void LogTrace( const char *sFormat, ... )
{
	LOG_WITH_VALIST(&g_oDebug, COMM_LOG_TRACE, sFormat, COMM_LOG_HEAD_COMM);
}

void LogVerbose( const char *sFormat, ... )
{
	LOG_WITH_VALIST(&g_oDebug, COMM_LOG_VERBOSE, sFormat, COMM_LOG_HEAD_COMM);
}

void LogInternal ( const char *sFormat, ... )
{
	LOG_WITH_VALIST(&g_oDebug, COMM_LOG_IN, sFormat, COMM_LOG_HEAD_COMM);

}

void LogMsg ( const char *sFormat, ... )
{
	LOG_WITH_VALIST(&g_oDebug, COMM_LOG_MSG, sFormat, COMM_LOG_HEAD_COMM);
}

void LogTrace( const char * moduel, const char * function, unsigned int uin, const char * step, int ret )
{
	LogTrace( moduel, function, uin, step, ret, NULL);
}

void LogTrace( const char * moduel, const char * function, unsigned int uin, const char * step, int ret, const char *sFormat, ... )
{

	if ( g_isOpenTraceLog )
	{
		char format[1024] = {0};
		if ( NULL != sFormat )
			snprintf( format, sizeof(format), "%s %s %u %s %d %s", moduel, function, uin, step, ret, sFormat );
		else{
			snprintf( format, sizeof(format), "%s %s %u %s %d", moduel, function, uin, step, ret );
		}

	//	LOG_WITH_VALIST(&g_oTrace, COMM_LOG_ALL, format, COMM_LOG_HEAD_COMM);
		
		va_list args;                                          
		va_start ( args, sFormat );                             
		(&g_oTrace)->Log ( 0 , format , args, 0 );      
		va_end ( args );            
	}
}

void LogTrace( const char * function, unsigned int uin, const char *sFormat, ... )
{
	if ( g_isOpenTraceLog )
	{
		char format[1024] = {0};
		snprintf( format, sizeof(format), "%s %u %s", function, uin, sFormat );
		//LOG_WITH_VALIST(&g_oTrace, COMM_LOG_ALL, format, COMM_LOG_HEAD_COMM);

		va_list args;                                          
		va_start ( args, sFormat );                             
		(&g_oTrace)->Log ( 0 , format , args, 0 );      
		va_end ( args );            

	}
}


//========================================================================================================


CLog :: CLog()
{
	m_ptLogImpl = NULL;
	// init default log env
	Init ( "OTHER", COMM_LOG_DEBUG, "/home/qspace/log/error" );
}

CLog :: CLog(const int iMaxSize)
{
	m_ptLogImpl = NULL;
	// init default log env
	Init ( "OTHER", COMM_LOG_DEBUG, "/home/qspace/log/error", iMaxSize );
}



CLog :: ~CLog()
{
	Destroy();
}

CLog * CLog :: GetDefault()
{
	static CLog log;
	return &log;
}

int CLog :: Init( const char *sModuleName, const int iLogLevel, const char *sPath, const int iMaxSize )
{
	if ( (NULL == sModuleName) || (NULL == sPath) )
	{
		return -2;
	}

	int iRet = 0;
	if ( 0 > access(sPath, F_OK) )
	{
		iRet = mkdir( sPath, S_IRWXU | S_IRWXG | S_IRWXO );

		if( 0 != iRet )
		{
			return -1;
		}

		chmod ( sPath, S_IRWXU | S_IRWXG | S_IRWXO );
	}

	if( NULL != m_ptLogImpl )
	{
		CleanImpl( m_ptLogImpl );
	} else {
		m_ptLogImpl = new CLogImpl_t;
	}

	iRet = InitImpl( m_ptLogImpl, sModuleName, iLogLevel, sPath, iMaxSize );
	if( 0 != iRet )
	{
		delete m_ptLogImpl, m_ptLogImpl = NULL;
	}

	return iRet;
}

int CLog :: InitImpl( CLogImpl_t * ptImpl, const char *sModuleName,
			const int iLogLevel, const char *sPath, const int iMaxSize )
{
	memset( ptImpl, 0, sizeof( CLogImpl_t ) );

	strncpy( ptImpl->sModuleName, sModuleName, sizeof( ptImpl->sModuleName ) - 1 );
	ptImpl->iLevel = iLogLevel;
	strncpy( ptImpl->sPath, sPath, sizeof( ptImpl->sPath ) - 1 );
	ptImpl->iMaxSize = ((unsigned long long)iMaxSize) * 1024 * 1024;

	ptImpl->iThreadMode = COMM_LOG_TM_DEFAULT;
	ptImpl->iInterval = 3600;
	ptImpl->sPrefix[0] = '\0';

	ptImpl->pcLogBuffer = new char[ eMAX_LINE_SIZE + 64 ];

	if( NULL == ptImpl->pcLogBuffer )
	{
		return -1;
	}

	memset(ptImpl->pcLogBuffer, 0, eMAX_LINE_SIZE + 64);

	ptImpl->pcLogBuffer[0] = '\0';

	ptImpl->iLogFD = -1;
	ptImpl->iShmMode = COMM_LOG_SHM_DEFAULT;
    ptImpl->ptLogShmQueue = NULL;

    ptImpl->iCheckLogSizeMode = COMM_LOG_CHECKLOGSIZE_ENABLE;

	return 0;
}

int CLog :: CleanImpl( CLogImpl_t * ptImpl )
{
	int fd = ptImpl->iLogFD;
	ptImpl->iLogFD = -1;
	if( fd >= 0 ) close( fd );

	if( NULL != ptImpl->pcLogBuffer ) delete [] ptImpl->pcLogBuffer;
	ptImpl->pcLogBuffer = NULL;


    if(COMM_LOG_SHM_ENABLE == ptImpl->iShmMode) {
        if(NULL != ptImpl->ptLogShmQueue) {
            delete ptImpl->ptLogShmQueue;
            ptImpl->ptLogShmQueue = NULL;
        }
        ptImpl->iShmMode = COMM_LOG_SHM_DISABLE;
        ptImpl->iCheckLogSizeMode = COMM_LOG_CHECKLOGSIZE_ENABLE;
    }

    if(ptImpl->iThreadMode == COMM_LOG_TM_ENABLE) {
        char * pcLogBuffer = NULL;
        if ((pcLogBuffer = (char*)pthread_getspecific(g_logkey)) != NULL) {
            delete [] pcLogBuffer;
            pcLogBuffer = NULL;
            pthread_setspecific(g_logkey, NULL);
        }
        ptImpl->iThreadMode = COMM_LOG_TM_DEFAULT;
    }

    return 0;
}

void CLog :: Destroy()
{
	if ( NULL != m_ptLogImpl )
	{
		CleanImpl( m_ptLogImpl );
		delete m_ptLogImpl, m_ptLogImpl = NULL;
	}

	Init ( "OTHER", COMM_LOG_DEBUG, "/home/qspace/log/error" );
}

void CLog :: SetLogModuleName( const char *sModuleName )
{
	if ( NULL != sModuleName )
	{
		strncpy ( m_ptLogImpl->sModuleName, sModuleName, sizeof(m_ptLogImpl->sModuleName)-1 );
	}
}

const char * CLog :: GetLogModuleName()
{
	return m_ptLogImpl->sModuleName;
}

void CLog :: SetPrefix( const char *sPrefix )
{
	if ( NULL != sPrefix )
	{
		strncpy ( m_ptLogImpl->sPrefix, sPrefix, sizeof(m_ptLogImpl->sPrefix)-1 );
	}
}

const char * CLog :: GetPrefix()
{
	return m_ptLogImpl->sPrefix;
}

void CLog :: SetLogLevel ( const int iLogLevel )
{
	m_ptLogImpl->iLevel = iLogLevel;
}

const int CLog :: GetLogLevel()
{
	return m_ptLogImpl->iLevel;
}

void CLog :: SetLogPath ( const char *sLogPath )
{
	if ( (NULL != sLogPath) && (0 != strcmp(sLogPath, m_ptLogImpl->sPath)) )
	{
		strncpy ( m_ptLogImpl->sPath, sLogPath, sizeof(m_ptLogImpl->sPath)-1 );
	}
}

const char * CLog :: GetLogPath()
{
	return m_ptLogImpl->sPath;
}

void CLog :: SetLogInterval( int iInterval )
{
	if( iInterval >= 5 )
	{
		m_ptLogImpl->iInterval = ( iInterval / 5 ) * 5;
	}
}

int CLog :: GetLogInterval()
{
	return m_ptLogImpl->iInterval;
}

void CLog :: SetLogThreadMode(const int iThreadMode)
{

    if(COMM_LOG_TM_ENABLE == iThreadMode) {

        m_ptLogImpl->iThreadMode = iThreadMode;

        (void) pthread_once(&g_logkey_once, LogThreadModeMakeKey);

        char * pcLogBuffer = NULL;
        if ((pcLogBuffer = (char*)pthread_getspecific(g_logkey)) == NULL) {

            pcLogBuffer = new char[ eMAX_LINE_SIZE + 64 ];

            if(NULL == pcLogBuffer) {
                //m_ptLogImpl->iThreadMode = COMM_LOG_TM_DISABLE;
                return;
            }

            memset(pcLogBuffer, 0x0, eMAX_LINE_SIZE + 64);

            int iRet =  pthread_setspecific(g_logkey, (void*)pcLogBuffer);
            if(0 != iRet) {
                //m_ptLogImpl->iThreadMode = COMM_LOG_TM_DISABLE;
                delete [] pcLogBuffer;
                pcLogBuffer = NULL;
            }
        }
    } else {

        if(m_ptLogImpl->iThreadMode == COMM_LOG_TM_ENABLE) {

            char * pcLogBuffer = NULL;
            if ((pcLogBuffer = (char*)pthread_getspecific(g_logkey)) != NULL) {
                delete [] pcLogBuffer;
                pcLogBuffer = NULL;
                pthread_setspecific(g_logkey, NULL);
            }

        }
        m_ptLogImpl->iThreadMode = iThreadMode;
    }

}

const int CLog :: GetLogThreadMode()
{
	return m_ptLogImpl->iThreadMode;
}

void CLog :: DisableShmMode() 
{
    if(NULL != m_ptLogImpl->ptLogShmQueue) {
        delete m_ptLogImpl->ptLogShmQueue;
        m_ptLogImpl->ptLogShmQueue = NULL;
        m_ptLogImpl->iShmMode = COMM_LOG_SHM_DISABLE;
    }
}

void CLog :: SetLogCheckSizeMode(const int iCheckLogSizeMode)
{
    if(COMM_LOG_CHECKLOGSIZE_DISABLE == iCheckLogSizeMode &&
            COMM_LOG_SHM_DISABLE == m_ptLogImpl->iShmMode) {
        return;
    }
    m_ptLogImpl->iCheckLogSizeMode = iCheckLogSizeMode;
}

const int CLog :: GetLogCheckSizeMode()
{
	return m_ptLogImpl->iCheckLogSizeMode;
}

int CLog :: SetLogShmMode(const int iShmMode)
{
    if(iShmMode == m_ptLogImpl->iShmMode) {
        return 0;
    }

	m_ptLogImpl->iShmMode = iShmMode;

    if(COMM_LOG_SHM_ENABLE == iShmMode) {

        if(NULL != m_ptLogImpl->ptLogShmQueue) {
            DisableShmMode();
            //should not happen, just return failure
            return -1;
        }

        m_ptLogImpl->ptLogShmQueue = new Comm::LogShmQueue();
        if(NULL == m_ptLogImpl->ptLogShmQueue) {
            m_ptLogImpl->iShmMode = COMM_LOG_SHM_DISABLE;
            return -1;
        }

        int iRet = m_ptLogImpl->ptLogShmQueue->init();
        if(0 != iRet) {
            DisableShmMode();
            return -1;
        }
    } else {
        DisableShmMode();
    }
    return 0;
}

const int CLog :: GetLogShmMode()
{
	return m_ptLogImpl->iShmMode;
}

void CLog :: SetLogSessionID( const char *sSID )
{
	if ( '\0' != sSID[0] )
	{
		strncpy ( m_ptLogImpl->sSID, sSID, sizeof(m_ptLogImpl->sSID)-1 );
	}
	else
	{
		m_ptLogImpl->sSID[0] = '\0';
	}
}

const char * CLog :: GetLogSessionID()
{
	return m_ptLogImpl->sSID;
}

void CLog :: MakeLog(char * pcLogBuffer, const int iLevel, const char *sFormat, va_list args, const int iHeadType, int &iLogLen ) {

    int iHeadLen = 0;
    int iSIDLen = 0;
    int iPrefixLen = 0;
    int iBodyLen = 0;
    char sSID[32] = { 0 };
    char sBuffer[512] = { 0 };
    //int iend = 0;

    GetLogHead( iLevel, iHeadType, sBuffer, sizeof(sBuffer), &iHeadLen );
    GetLogSID ( sSID, sizeof(sSID), &iSIDLen );

    iPrefixLen = iHeadLen + iSIDLen ;

    if(COMM_LOG_SHM_ENABLE == m_ptLogImpl->iShmMode) {
        iBodyLen = vsnprintf ( (pcLogBuffer + iPrefixLen + 2), (eMAX_LINE_SIZE - iPrefixLen),
                sFormat, args );

        if( eMAX_LINE_SIZE - iPrefixLen <= iBodyLen) {
            iBodyLen = eMAX_LINE_SIZE - iPrefixLen - 1;
        }

        //iend=iBodyLen + iPrefixLen + 2;
    } else {
        iBodyLen = vsnprintf ( (pcLogBuffer + iPrefixLen), (eMAX_LINE_SIZE - iPrefixLen),
                sFormat, args );

        if( eMAX_LINE_SIZE - iPrefixLen <= iBodyLen) {
            iBodyLen = eMAX_LINE_SIZE - iPrefixLen - 1;
        }

        //iend=iBodyLen + iPrefixLen;
    }

    //replace \r\n  by space
    //iBodyLen = strlen(m_ptLogImpl->pcLogBuffer) - iPrefixLen;
#if 0
    for ( int i=0; i<iend; ++i )
    {
        if ( pcLogBuffer[i] == '\r' || pcLogBuffer[i] == '\n')
        {
            pcLogBuffer[i] = ' ';
        }
    }
#endif

    if ( 0 < iBodyLen )
    {

        if(COMM_LOG_SHM_ENABLE == m_ptLogImpl->iShmMode) {
            memcpy ( pcLogBuffer + 2, sBuffer, iHeadLen );
            memcpy ( (pcLogBuffer + iHeadLen + 2), sSID, iSIDLen );
            pcLogBuffer[iPrefixLen + iBodyLen + 2] = '\n';

            *((unsigned short*)(pcLogBuffer)) = iPrefixLen + iBodyLen + 1;
            *((char*)(pcLogBuffer + iPrefixLen + iBodyLen + 3)) = LogShmQueue::COMM_LOG_ITEM_MAGICNUM_2; 

            iLogLen = iPrefixLen + iBodyLen + 4;

        } else {
            memcpy ( pcLogBuffer, sBuffer, iHeadLen );
            memcpy ( (pcLogBuffer + iHeadLen), sSID, iSIDLen );
            pcLogBuffer[iPrefixLen + iBodyLen] = '\n';
            iLogLen = iPrefixLen+iBodyLen+1;
        }
    }

}



void CLog :: Log ( const int iLevel, const char *sFormat, ... )
{
	LOG_WITH_VALIST(this, iLevel, sFormat, COMM_LOG_HEAD_COMM);
}

void CLog :: Log ( const int iLevel, const int iHeadType, const char *sFormat, ... )
{
	LOG_WITH_VALIST(this, iLevel, sFormat, iHeadType);
}

void CLog :: Log ( const char *sFormat, ... )
{
	LOG_WITH_VALIST(this, COMM_LOG_ALL, sFormat, COMM_LOG_HEAD_COMM);
}

void CLog :: Log ( const int iLevel, const char *sFormat, va_list args, const int iHeadType )
{

	if( NULL == m_ptLogImpl ) return;

	if ( (COMM_LOG_ALL != iLevel) && (COMM_LOG_ALL != m_ptLogImpl->iLevel)
		&& (COMM_LOG_INFO != iLevel) && (COMM_LOG_MONITOR != iLevel) )
	{
		if ( (iLevel > m_ptLogImpl->iLevel) || (COMM_LOG_DISABLE == m_ptLogImpl->iLevel) )
		{
			return ;
		}
	}

	//OssAttr4CGILog( iLevel, 1 );

    char * pcLogBuffer = NULL;
    if(COMM_LOG_TM_ENABLE == m_ptLogImpl->iThreadMode) {

        if ((pcLogBuffer = (char*)pthread_getspecific(g_logkey)) == NULL) {

            pcLogBuffer = new char[ eMAX_LINE_SIZE + 64 ];

            if(NULL == pcLogBuffer) {
                return;
            }

            memset(pcLogBuffer, 0x0, eMAX_LINE_SIZE + 64);

            int iRet =  pthread_setspecific(g_logkey, (void*)pcLogBuffer);
            if(0 != iRet) {
                delete [] pcLogBuffer;
                pcLogBuffer = NULL;
                return;
            }
        }

    } else {
        pcLogBuffer = m_ptLogImpl->pcLogBuffer;
    }

    {
        if(COMM_LOG_CHECKLOGSIZE_ENABLE == m_ptLogImpl->iCheckLogSizeMode)
        {
            Lock();
            CheckLogName( m_ptLogImpl );

            /* BUGFIXED: check file size every 10 seconds to avoid signal 25 */
            if ( 0 != CheckLogSize() )
            {
                Unlock();
                return;
            }
            Unlock();
        }
    }

    {
        if(COMM_LOG_SHM_ENABLE == m_ptLogImpl->iShmMode) {
            int iLogLen = 0;
            MakeLog(pcLogBuffer, iLevel, sFormat, args, iHeadType, iLogLen );
            if ( 0 < iLogLen )
            {
                m_ptLogImpl->ptLogShmQueue->push(iLogLen, pcLogBuffer);
                //int iPushRet = m_ptLogImpl->ptLogShmQueue->push(iLogLen, pcLogBuffer);
                //printf("%d\n",iPushRet);
            }

            return;
        } else {
            if( m_ptLogImpl->iLogFD >= 0 )
            {

                int iLogLen = 0;
                MakeLog(pcLogBuffer, iLevel, sFormat, args, iHeadType, iLogLen );
                if ( 0 < iLogLen )
                {
                    write( m_ptLogImpl->iLogFD, pcLogBuffer, iLogLen );
                }
            }
        }
    }
	return; 
}

int CLog :: CheckLogSize()
{
	int ret=0;

    if( (m_ptLogImpl->iLogFD >= 0) )
    {

        char * pcLogBuffer = NULL;
        if(COMM_LOG_TM_ENABLE == m_ptLogImpl->iThreadMode) {

            if ((pcLogBuffer = (char*)pthread_getspecific(g_logkey)) == NULL) {

                pcLogBuffer = new char[ eMAX_LINE_SIZE + 64 ];

                if(NULL == pcLogBuffer) {
                    return -1;
                }

                memset(pcLogBuffer, 0x0, eMAX_LINE_SIZE + 64);

                int iRet =  pthread_setspecific(g_logkey, (void*)pcLogBuffer);
                if(0 != iRet) {
                    delete [] pcLogBuffer;
                    pcLogBuffer = NULL;
                    return -1;
                }
            }
        } else {
            pcLogBuffer = m_ptLogImpl->pcLogBuffer;
        }

		if ( time(NULL) > m_ptLogImpl->iSizeCheckTime + 10 )
		{
			struct stat filestat;
			ret = fstat(m_ptLogImpl->iLogFD, &filestat);
			if ( 0 == ret )
			{
				m_ptLogImpl->iUsedSize = (unsigned long long)(filestat.st_size);
				m_ptLogImpl->iSizeCheckTime = time(NULL);
			} else {
				printf("fstat failed ret %d errno %d\n", ret, errno);
			}
		}

		if( m_ptLogImpl->iMaxSize <= m_ptLogImpl->iUsedSize )
		{
			if ( m_ptLogImpl->iMaxSize + 1024*1024 >= m_ptLogImpl->iUsedSize )
			{
				/* log warning msg at [iMaxSize, iMaxSize+10MB] */
				int iHeadLen = 0;
				int iBodyLen = 0;
				char sBuffer[512] = { 0 };
				GetLogHead( COMM_LOG_IMPT, COMM_LOG_HEAD_COMM, sBuffer, sizeof(sBuffer), &iHeadLen );

				iBodyLen = snprintf ( (pcLogBuffer + iHeadLen), (eMAX_LINE_SIZE - iHeadLen),
					"LOG_FILE_IS_FULL, limit %llu, current size %llu", m_ptLogImpl->iMaxSize, m_ptLogImpl->iUsedSize );

				memcpy ( pcLogBuffer, sBuffer, iHeadLen );
				pcLogBuffer[iHeadLen + iBodyLen] = '\n';

				/* NOTE: need external lock */
				write( m_ptLogImpl->iLogFD, pcLogBuffer, (iHeadLen+iBodyLen+1) );
			}

			ret = -1;
		}
	}

	return ret;
}

int CLog :: CheckLogName( CLogImpl_t * ptImpl )
{
	char name[ 64 ] = { 0 };

	GetLogName( ptImpl->iInterval, name, sizeof( name ) );

	if ( 0 > ptImpl->iLogFD )
	{
		ptImpl->sLogName[0] = '\0';
	}

	if( 0 != strcmp( name, ptImpl->sLogName ) )
	{
		// change log file
		int tmpFD = ptImpl->iLogFD;
		ptImpl->iLogFD = -1;
		if( tmpFD >= 0 ) close( tmpFD );

		ptImpl->iUsedSize = 0;

		strncpy( ptImpl->sLogName, name, sizeof( ptImpl->sLogName ) );

		char path[ 512 ] = { 0 };

		if(COMM_LOG_SHM_ENABLE == ptImpl->iShmMode) {
			//XXX hard code here
			snprintf( path, sizeof( path ), "/home/qspace/log/error/%s.log",
			    ptImpl->sLogName );
		} else {
			snprintf( path, sizeof( path ), "%s/%s%s.log", ptImpl->sPath,
			    ptImpl->sPrefix, ptImpl->sLogName );
		}

		int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH ;

		//modify by bingchen, close fd. 2014-10-13
		if ( ptImpl->iLogFD > 0 )
		{
			close(ptImpl->iLogFD);
			ptImpl->iLogFD = -1;
		}
		
		ptImpl->iLogFD = open( path, O_RDWR | O_NONBLOCK | O_APPEND | O_CREAT | O_LARGEFILE, mode );

		// skip fd 0, amended at 2010/05/04, leak one fd at most and maybe one log missed
		if ( 0 == ptImpl->iLogFD )
		{
			for ( int i=0; i<3; ++i )
			{
				ptImpl->iLogFD = -1;
				ptImpl->iLogFD = open( path, O_RDWR | O_NONBLOCK | O_APPEND | O_CREAT | O_LARGEFILE, mode );
				if ( ptImpl->iLogFD > 0 )
				{
					break;
				}
			}

			if ( 0 == ptImpl->iLogFD ) ptImpl->iLogFD = -1;
		}

		if( ptImpl->iLogFD >= 0 )
		{
			/* change all rw mode */
			fchmod ( ptImpl->iLogFD, mode );
 
			struct stat filestat;
			if ( 0 == fstat(ptImpl->iLogFD, &filestat) )
			{
				ptImpl->iUsedSize = filestat.st_size;
				ptImpl->iSizeCheckTime = time(NULL);
			}
		}
	}

	return 0;
}

void CLog :: GetLogName( int iInterval, char * sName, int size )
{
	time_t tt = time( NULL );  
	struct tm temp;

	tt = ( tt / iInterval ) * iInterval;
	struct tm *curtime = localtime_r( &tt, &temp );
	int year = 1900 + curtime->tm_year;
	int month = curtime->tm_mon + 1;
	int day = curtime->tm_mday;
	int hour = curtime->tm_hour;
	int minute = curtime->tm_min;
	int second = curtime->tm_sec;

	if( iInterval >= 3600 )
	{
		snprintf( sName, size, "%04d%02d%02d%02d", year, month, day, hour );
	} else {
		snprintf( sName, size, "%04d%02d%02d%02d%02d%02d", year, month, day, hour, minute, second );
	}
}

void CLog :: GetLogHead( const int iLevel, const int iHeadType, char *sLogHead, const int iSize, int *piHeadLen )
{
	int headlen = 0;
	char timenow[128] = { 0 };
	char procpid[64] = { 0 };
	// time_t tt = time( NULL );  
	struct tm temp;

    // 此处可决定是否记录毫秒值, 2021-10-31
    //
    
    struct timeval tv = {0};
    gettimeofday(&tv, NULL);
    time_t tt = tv.tv_sec;

	struct tm *curtime = localtime_r( &tt, &temp );
	int year = 1900 + curtime->tm_year;
	int month = curtime->tm_mon + 1;
	int day = curtime->tm_mday;
	int hour = curtime->tm_hour;
	int minute = curtime->tm_min;
	int second = curtime->tm_sec;
    int microsec = tv.tv_usec/1000;

    snprintf( timenow, sizeof(timenow), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
		year, month, day, hour, minute, second, microsec );

	if ( COMM_LOG_TM_ENABLE == m_ptLogImpl->iThreadMode )
	{
		snprintf ( procpid, sizeof(procpid), "%d,%lu", getpid(), pthread_self() );
	}
	else
	{
		snprintf ( procpid, sizeof(procpid), "%d", getpid() );
	}

	if ( (COMM_LOG_HEAD_COMM == iHeadType) || (COMM_LOG_HEAD_INFO == iHeadType) )
	{
		headlen = snprintf ( sLogHead, iSize, "<%d> <%s(%s)> %s ",
			iLevel, m_ptLogImpl->sModuleName, procpid, timenow );
	}
	else if ( COMM_LOG_HEAD_MONITOR == iHeadType )
	{
		char hostname[64] = { 0 };
		gethostname ( hostname, sizeof(hostname)-1 );
		headlen = snprintf ( sLogHead, iSize, "%s,%s,%s,",
			timenow, m_ptLogImpl->sModuleName, hostname );
	}
	else if ( COMM_LOG_HEAD_NONE == iHeadType )
	{
		sLogHead[0] = '\0';
		headlen = 0;
	}
	else
	{
		headlen = snprintf ( sLogHead, iSize, "<%d> <%s(%s)> %s ",
			iLevel, m_ptLogImpl->sModuleName, procpid, timenow );
	}

	*piHeadLen = headlen;
	return;
}

void CLog :: GetLogSID( char *sLogSID, const int iSize, int *piSIDLen )
{
	sLogSID[0] = '\0';
	*piSIDLen = 0;

	if ( (NULL != m_ptLogImpl) && ('\0' != *m_ptLogImpl->sSID) )
	{
		*piSIDLen = snprintf ( sLogSID, iSize, "%s ", m_ptLogImpl->sSID );
	}

	return;
}

static pthread_mutex_t g_tMutex = PTHREAD_MUTEX_INITIALIZER;
void CLog::Lock()
{
	if ( COMM_LOG_TM_ENABLE == m_ptLogImpl->iThreadMode ) pthread_mutex_lock(&g_tMutex);
}

void CLog::Unlock()
{
	if ( COMM_LOG_TM_ENABLE == m_ptLogImpl->iThreadMode ) pthread_mutex_unlock(&g_tMutex);
}

} // end namespace Comm

