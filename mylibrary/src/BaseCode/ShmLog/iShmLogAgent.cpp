
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
#include "iLogShmErr.h"

#include "iShmLogAgent.h"


using namespace Comm;

typedef struct _CShmLogAgentImpl_t {
	char sPath[ 512 ];
	unsigned long long iMaxSize; // in bytes 
	int iInterval;

	char sLogName[ 256 ];
	int iLogFD;
	int iSizeCheckTime;
	unsigned long long iUsedSize;
	char *pcLogBuffer;

    LogShmQueue tLogShmQueue;

    int iMaxLogNotReadyCount;
    int iSleepWhenEmpyt;
    int iSleepWhenNotReady;
} CShmLogAgentImpl_t;


static int g_iShmLogAgentStop = 0;

void StopShmLogAgent() {
    g_iShmLogAgentStop = 1;
}


CShmLogAgent::CShmLogAgent() {
    m_ptImpl = NULL;
}

CShmLogAgent::~CShmLogAgent() {

    if(NULL != m_ptImpl) {
        if(NULL != m_ptImpl->pcLogBuffer) {
            delete m_ptImpl->pcLogBuffer;
            m_ptImpl->pcLogBuffer = NULL;
        }

        if(-1 != m_ptImpl->iLogFD) {
            close(m_ptImpl->iLogFD);
        }

        delete m_ptImpl;
        m_ptImpl = NULL;
    }
}

int CShmLogAgent::Init(const char *sPath, const int iMaxSize, const int iMaxLogNotReadyCount,
        const int iSleepWhenEmpyt, const int iSleepWhenNotReady) {

    if(NULL != m_ptImpl) {
        return 0;
    }

    m_ptImpl = new CShmLogAgentImpl_t;
    if(NULL == m_ptImpl) {
        return -1;
    }

    memset(m_ptImpl, 0x0, sizeof(CShmLogAgentImpl_t));

    snprintf ( m_ptImpl->sPath, sizeof(m_ptImpl->sPath), "%s/error", sPath );
	m_ptImpl->iMaxSize = ((unsigned long long)iMaxSize) * 1024 * 1024;
	m_ptImpl->iInterval = 3600;


    m_ptImpl->pcLogBuffer = new char[CLog::eMAX_LINE_SIZE + 64];

    if(NULL == m_ptImpl->pcLogBuffer) {
        delete m_ptImpl;
        m_ptImpl = NULL;
        return -1;
    }
    m_ptImpl->pcLogBuffer[0] = '\0';

    int iRet = m_ptImpl->tLogShmQueue.create();
    if(0 != iRet) {
        delete m_ptImpl->pcLogBuffer;
        m_ptImpl->pcLogBuffer = NULL;
        delete m_ptImpl;
        m_ptImpl = NULL;
        return -1;
    }

    m_ptImpl->iMaxLogNotReadyCount = iMaxLogNotReadyCount;
    m_ptImpl->iSleepWhenEmpyt = iSleepWhenEmpyt;
    m_ptImpl->iSleepWhenNotReady = iSleepWhenNotReady;


    //m_iStop = 0;

    g_iShmLogAgentStop = 0;


    return 0;
}

int CShmLogAgent :: CheckLogSize()
{
	int ret=0;

    if( (m_ptImpl->iLogFD >= 0) )
    {

        char * pcLogBuffer = m_ptImpl->pcLogBuffer;

		if ( time(NULL) > m_ptImpl->iSizeCheckTime + 10 )
		{
			struct stat filestat;
			ret = fstat(m_ptImpl->iLogFD, &filestat);
			if ( 0 == ret )
			{
				m_ptImpl->iUsedSize = (unsigned long long)(filestat.st_size);
				m_ptImpl->iSizeCheckTime = time(NULL);
			} else {
				printf("fstat failed ret %d errno %d\n", ret, errno);
			}
		}

		if( m_ptImpl->iMaxSize <= m_ptImpl->iUsedSize )
		{
			if ( m_ptImpl->iMaxSize + 1024*1024 >= m_ptImpl->iUsedSize )
			{
				/* log warning msg at [iMaxSize, iMaxSize+10MB] */
				int iBodyLen = 0;

				iBodyLen = snprintf ( pcLogBuffer, (CLog::eMAX_LINE_SIZE ),
					"LOG_FILE_IS_FULL, limit %llu, current size %llu", m_ptImpl->iMaxSize, m_ptImpl->iUsedSize );

				pcLogBuffer[iBodyLen] = '\n';
                ret = 0;
            } else {
                ret = -1;
            }
		}
	}

	return ret;
}

int CShmLogAgent :: CheckLogName()
{
	char name[ 64 ] = { 0 };

	GetLogName( m_ptImpl->iInterval, name, sizeof( name ) );

	if ( 0 > m_ptImpl->iLogFD )
	{
		m_ptImpl->sLogName[0] = '\0';
	}

	if( 0 != strcmp( name, m_ptImpl->sLogName ) )
	{
		// change log file
		int tmpFD = m_ptImpl->iLogFD;
		m_ptImpl->iLogFD = -1;
		if( tmpFD >= 0 ) close( tmpFD );

		m_ptImpl->iUsedSize = 0;

		strncpy( m_ptImpl->sLogName, name, sizeof( m_ptImpl->sLogName ) );

		char path[ 512 ] = { 0 };
		snprintf( path, sizeof( path ), "%s/%s.log", m_ptImpl->sPath,
                m_ptImpl->sLogName );

		int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH ;
		m_ptImpl->iLogFD = open( path, O_RDWR | O_NONBLOCK | O_APPEND | O_CREAT | O_LARGEFILE, mode );

		// skip fd 0, leak one fd at most and maybe one log missed
		if ( 0 == m_ptImpl->iLogFD )
		{
			for ( int i=0; i<3; ++i )
			{
				m_ptImpl->iLogFD = -1;
				m_ptImpl->iLogFD = open( path, O_RDWR | O_NONBLOCK | O_APPEND | O_CREAT | O_LARGEFILE, mode );
				if ( m_ptImpl->iLogFD > 0 )
				{
					break;
				}
			}

			if ( 0 == m_ptImpl->iLogFD ) m_ptImpl->iLogFD = -1;
		}

		if( m_ptImpl->iLogFD >= 0 )
		{
			/* change all rw mode */
			fchmod ( m_ptImpl->iLogFD, mode );
 
			struct stat filestat;
			if ( 0 == fstat(m_ptImpl->iLogFD, &filestat) )
			{
				m_ptImpl->iUsedSize = filestat.st_size;
				m_ptImpl->iSizeCheckTime = time(NULL);
			}
		}
	}
	return 0;
}

void CShmLogAgent :: GetLogName( int iInterval, char * sName, int size )
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

int CShmLogAgent::Stop() {
    //m_iStop = 1;
    g_iShmLogAgentStop = 1;
    return 0;
}

int CShmLogAgent::Run() {

    int iRet = 0;
    int iLogLen = 0;

    int iLogNotReadyCount = 0;


    int iMaxLogNotReadyCount = m_ptImpl->iMaxLogNotReadyCount;
    int iSleepWhenEmpyt = m_ptImpl->iSleepWhenEmpyt;
    int iSleepWhenNotReady = m_ptImpl->iSleepWhenNotReady;

    while(!g_iShmLogAgentStop) {

        iRet = m_ptImpl->tLogShmQueue.pop(iLogLen, m_ptImpl->pcLogBuffer);

        if(COMM_LOG_SHM_QUEUE_EMPTY == iRet) {
            usleep(iSleepWhenEmpyt); 
            continue;
        }

        if(COMM_LOG_SHM_LOG_NOT_READY == iRet) {

            iLogNotReadyCount++;

            if(iMaxLogNotReadyCount <= iLogNotReadyCount) {
                m_ptImpl->tLogShmQueue.reset();
                iLogNotReadyCount = 0;
            }

            usleep(iSleepWhenNotReady);  
            continue;
        }

        if(COMM_LOG_SHM_ERROR_RESET == iRet ||
                COMM_LOG_SHM_ERROR_ARG == iRet ||
                COMM_LOG_SHM_ERROR_MAGICNUM1_CORRUPT == iRet || 
                COMM_LOG_SHM_ERROR_MAGICNUM2_CORRUPT == iRet || 
                COMM_LOG_SHM_ERROR_LEN_CORRUPT == iRet) {

            m_ptImpl->tLogShmQueue.reset();
            continue;
        } 

        if(0 == iRet) {

            iLogNotReadyCount = 0;

            CheckLogName();

            if ( 0 != CheckLogSize() )
            {
                continue;
            }

            if( m_ptImpl->iLogFD >= 0 )
            {

                write( m_ptImpl->iLogFD, m_ptImpl->pcLogBuffer, iLogLen );
            }

        }
    }

    return 0;
}




