#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
//#include "iFileFunc.h"

#include "iLogShmErr.h"
#include "iLogShm.h"
//#include "../trafficmgr/iBigLock.h"


namespace Comm {
    char LogShmQueue::COMM_LOG_ITEM_MAGICNUM_1 = 0x01;
    char LogShmQueue::COMM_LOG_ITEM_MAGICNUM_2 = 0x03;
    int  LogShmQueue::COMM_LOG_ITEM_SECT       = 1048576; //1MB

    LogShmQueue::LogShmQueue(int iPushRetryCount) {
        m_iShmSize = 0;
        m_ptHeader = NULL;
        m_iPushRetryCount = iPushRetryCount;
        m_iShmKey = 0;
        memset(m_sLockPath, 0x0, 512);
        m_iQueueSize = 0;
    }

    LogShmQueue::~LogShmQueue() {

    }

    int LogShmQueue::init( const int shmkey, const int shmsize )
    {
        size_t size = shmsize * 1024 * 1024;

        int shmid = 0;
        {
            shmid = shmget ( shmkey, size, 0666 );

            if ( shmid<0 )
            {
                writeownlog ( "shmque: init shmet failed key %i size %i failed %i errno %i '%s'\n",
                        shmkey, size, shmid, errno, strerror(errno) );
                return -1;
            }
        }

        {
            m_ptHeader = (LogShmQueueHead_t *) shmat ( shmid, NULL, 0 ) ;
            if ( -1 == (long)m_ptHeader )
            {
                writeownlog ( "shmque: init shmat failed key %i size %i failed id %i errno %i '%s'\n",
                        shmkey, size, shmid, errno, strerror(errno) );
                m_ptHeader = NULL;
                return -1;
            }


            if ( (COMM_LOG_SHM_MAINVER == m_ptHeader->hMainVer) && 
                    (COMM_LOG_SHM_SUBVER == m_ptHeader->hSubVer) &&
                    (COMM_LOG_SHM_MAGIC == m_ptHeader->iMagicNum) &&
                    ((unsigned int)(m_ptHeader->iQueueSize) == (size - sizeof(LogShmQueueHead_t)))
               )
            {
                writeownlog ( "log shmque: init verify ok key %i size %i id %i\n", shmkey, size, shmid );
            }
            else
            {
                writeownlog ( "log shmque: init verify failed key %i size %i id %i, begin to reset queue\n", shmkey, size, shmid );
                m_ptHeader = NULL;
                return -1;
            }
        }

        m_iShmSize = size;
        m_iShmKey = shmkey;

        m_iQueueSize = (size - sizeof(LogShmQueueHead_t));

        return 0;
    }

    int LogShmQueue::create( const char *lockpath, const int shmkey, const int shmsize)
    {
        size_t size = shmsize * 1024 * 1024;

        {
            if ( (NULL == lockpath) || ('\0' == *lockpath) || 
                    (0 > shmkey) || (sizeof(LogShmQueueHead_t) >= size) )
            {
                writeownlog ( "log shm queue: arg init failed lockpath %s key %i size %i \n", lockpath, shmkey, size );
                return -1;
            }

            int fd = open(lockpath,O_CREAT|O_RDWR,0666); 
            if ( -1 == fd )
            {
                writeownlog ( "log shme queue: init lockpath failed %s '%s'\n", lockpath, strerror(errno) );
                return -1;
            }
            close(fd);
        }

        //CBigLock bigLock(lockpath);

        /* get shm queue */
        int shmid = 0;
        {
            shmid = shmget ( shmkey, size, 0666 );
            if( shmid<0 )
            {
                shmid = shmget ( shmkey, size, 0666|IPC_CREAT ) ;
            }

            if ( shmid<0 )
            {
                writeownlog ( "shmque: init shmet failed key %i size %i failed %i errno %i '%s'\n",
                        shmkey, size, shmid, errno, strerror(errno) );
                return -1;
            }
        }

        {
            m_ptHeader = (LogShmQueueHead_t *) shmat ( shmid, NULL, 0 ) ;
            if ( -1 == (long)m_ptHeader )
            {
                writeownlog ( "shmque: init shmat failed key %i size %i failed id %i errno %i '%s'\n",
                        shmkey, size, shmid, errno, strerror(errno) );
                m_ptHeader = NULL;
                return -1;
            }


            if ( (COMM_LOG_SHM_MAINVER == m_ptHeader->hMainVer) && 
                    (COMM_LOG_SHM_SUBVER == m_ptHeader->hSubVer) &&
                    (COMM_LOG_SHM_MAGIC == m_ptHeader->iMagicNum) &&
                    ((unsigned int)(m_ptHeader->iQueueSize) == (size - sizeof(LogShmQueueHead_t)))
               )
            {
                writeownlog ( "shmque: init verify ok key %i size %i id %i\n", shmkey, size, shmid );
            }
            else
            {
                writeownlog ( "shmque: init verify failed key %i size %i id %i, begin to reset queue\n", shmkey, size, shmid );

                memset ( m_ptHeader, 0x0, sizeof(LogShmQueueHead_t) );

                sleep(10);

                m_ptHeader->hMainVer = COMM_LOG_SHM_MAINVER;
                m_ptHeader->hSubVer = COMM_LOG_SHM_SUBVER;
                m_ptHeader->iMagicNum = COMM_LOG_SHM_MAGIC;
                m_ptHeader->iQueueSize = size - sizeof(LogShmQueueHead_t) ;
                m_ptHeader->iHeadPos = m_ptHeader->iTailPos = 0;
                memset((char*)m_ptHeader+sizeof(LogShmQueueHead_t), 0x0, m_ptHeader->iQueueSize);
                m_ptHeader->iResetFlag = COMM_LOG_SHM_RESET_FLAG;

                writeownlog ( "shmque: init ok key %i size %i id %i\n", shmkey, size, shmid );
            }
        }

        m_iShmSize = size;
        m_iShmKey = shmkey;

        snprintf(m_sLockPath, sizeof(m_sLockPath), "%s", lockpath);

        m_iQueueSize = (size - sizeof(LogShmQueueHead_t));

        return 0;
    }

    // buffer format len(unsigned short) + log + magic number(char)
    int LogShmQueue::push( const int iBufLen, const char * pBuffer) {

        if(!m_ptHeader) {
            return COMM_LOG_SHM_ERROR_ARG;
        }

        if(COMM_LOG_SHM_RESET_FLAG != m_ptHeader->iResetFlag) {
            return COMM_LOG_SHM_ERROR_RESET;
        }

        int iTailPos = 0;
		int iRoom = 0;
        int i = 0;
        int iWriteLen = COMM_LOG_ITEM_MAGICNUM_SIZE + iBufLen ;
        int iRet = 0;

        for (i = 0; i < m_iPushRetryCount; i++) {
            iTailPos = m_ptHeader->iTailPos;
            iRoom = m_ptHeader->iHeadPos - iTailPos - COMM_LOG_ITEM_SECT;
            if (iRoom < 0)
            {
                iRoom += m_iQueueSize;
            }

            if( iRoom < iWriteLen ) {
                iRet = COMM_LOG_SHM_ERROR_NO_ROOM;
                continue;
            }

            if( __sync_bool_compare_and_swap( &(m_ptHeader->iTailPos), iTailPos,
                        (iTailPos + iWriteLen)%m_iQueueSize ) ) {
                break;
            } 
            //else { 
                //iRet =  COMM_LOG_SHM_ERROR_CAS_FAILED;
                //printf("-----------------cas failed -------------------\n");
            //}
            iRet =  COMM_LOG_SHM_ERROR_CAS_FAILED;
        }

        if(i < m_iPushRetryCount) {

            char *dest = NULL;
            int iTailRoom = 0;

            int iTmpTailPos = (iTailPos + COMM_LOG_ITEM_MAGICNUM_SIZE)%m_iQueueSize;
            dest = m_ptHeader->sData + iTmpTailPos;

            if ( m_iQueueSize < iTmpTailPos + iBufLen )
            {
                iTailRoom = m_iQueueSize - iTmpTailPos;

                memcpy(dest, pBuffer, iTailRoom);
                dest = m_ptHeader->sData;
                memcpy(dest, pBuffer+iTailRoom, iBufLen-iTailRoom);
            } else {
                memcpy(dest, pBuffer, iBufLen);
            }

            dest = m_ptHeader->sData + iTailPos;
            *dest = COMM_LOG_ITEM_MAGICNUM_1;

        } else {
            return iRet;
            //return COMM_LOG_SHM_ERROR_PUSH_RETRY_FAILED;
        }

        return 0;
    }


    int LogShmQueue::pop(int &iLogLen, char* pBuffer) {

        if(NULL == m_ptHeader || NULL == pBuffer) {
            return COMM_LOG_SHM_ERROR_ARG;
        }

        if(COMM_LOG_SHM_RESET_FLAG != m_ptHeader->iResetFlag) {
            return COMM_LOG_SHM_ERROR_RESET;
        }

        int iHeadPos = m_ptHeader->iHeadPos;
        int iTailRoom = m_iQueueSize - iHeadPos;

        char * src = m_ptHeader->sData + iHeadPos;

        char iMagicNum1 = *src;

        if( 0 == iMagicNum1 ) {
            if(iHeadPos == m_ptHeader->iTailPos) {
                return COMM_LOG_SHM_QUEUE_EMPTY;
            } else {
                return COMM_LOG_SHM_LOG_NOT_READY;
            }
        }

        if( COMM_LOG_ITEM_MAGICNUM_1 != iMagicNum1 ) {
            return COMM_LOG_SHM_ERROR_MAGICNUM1_CORRUPT;
        }

        src = m_ptHeader->sData + (iHeadPos + COMM_LOG_ITEM_MAGICNUM_SIZE) % m_iQueueSize;

        unsigned short iLen = 0; 

        if( 2 == iTailRoom ) {
            memcpy(((char*)&iLen), src, 1);
            memcpy(((char*)&iLen + 1), m_ptHeader->sData, 1);
        } else {
            iLen = *((unsigned short *)src);
        }

        char iMagicNum2 = 0;
        int iLen2 = COMM_LOG_ITEM_MAGICNUM_SIZE + 2 + iLen;

        if( iTailRoom <= iLen2) {
            // in case magic number is right, but len is wrong and too big
            if(iLen2 - iTailRoom > m_iQueueSize) {
                return COMM_LOG_SHM_ERROR_LEN_CORRUPT;
            }

            src = m_ptHeader->sData + (iHeadPos + iLen2) % m_iQueueSize;
        } else if (iTailRoom > iLen2)  {
            src = m_ptHeader->sData + iHeadPos + iLen2;
        } 

        iMagicNum2 = *((char*)src);

        if(COMM_LOG_ITEM_MAGICNUM_2 != iMagicNum2) {
            return COMM_LOG_SHM_ERROR_MAGICNUM2_CORRUPT;
        }

        int iTmpPos = (iHeadPos + 3) % m_iQueueSize;

        iTailRoom = m_iQueueSize - iTmpPos;
        src = m_ptHeader->sData + iTmpPos;

        if(iTailRoom < iLen) {
            memcpy(pBuffer, src, iTailRoom);
            src = m_ptHeader->sData;
            memcpy(pBuffer + iTailRoom, src, iLen - iTailRoom);
        } else {
            memcpy(pBuffer, src, iLen);
        }

        iLogLen = iLen;

        //reset 
        int iTotalLen = 2*COMM_LOG_ITEM_MAGICNUM_SIZE + 2 + iLen;
        iTailRoom = m_iQueueSize - iHeadPos;

        if(iTailRoom < iTotalLen) {
            src = m_ptHeader->sData + iHeadPos;
            memset(src, 0x0, iTailRoom);

            src = m_ptHeader->sData;
            memset(src, 0x0, iTotalLen - iTailRoom);
        } else {
            memset(m_ptHeader->sData + iHeadPos, 0x0, iTotalLen);
        }

        m_ptHeader->iHeadPos = (iHeadPos + iTotalLen) % m_iQueueSize;

        return 0;
    } 

    void LogShmQueue::writeownlog(const char *sFormat, ...) {

        va_list args;                                          
        va_start ( args, sFormat );                            

        int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH ;

        int fd = open("/home/qspace/log/error/logshmqueue",O_RDWR |O_APPEND | O_CREAT , mode ); 

        if(-1 != fd) {

            struct stat filestat;
            int ret = fstat(fd, &filestat);
            unsigned long long iFileSize = 0;
            if ( 0 == ret )
            {
                iFileSize = (unsigned long long)(filestat.st_size);
                if(iFileSize >= 1600 * 1024 * 1024) {
                    ftruncate(fd, 0);
                }
            }

            char buffer[2048] = {0};

            char head[512] = { 0 };
            int headlen = 0;
            char timenow[128] = { 0 };
            char procpid[64] = { 0 };
            time_t tt = time( NULL );  
            struct tm temp;

            struct tm *curtime = localtime_r( &tt, &temp );
            int year = 1900 + curtime->tm_year;
            int month = curtime->tm_mon + 1;
            int day = curtime->tm_mday;
            int hour = curtime->tm_hour;
            int minute = curtime->tm_min;
            int second = curtime->tm_sec;

            snprintf( timenow, sizeof(timenow), "%04d-%02d-%02d %02d:%02d:%02d",
                    year, month, day, hour, minute, second );

            snprintf ( procpid, sizeof(procpid), "%d,%ld", getpid(), pthread_self() );

            headlen = snprintf ( head, 512, "<%s(%s)> ",
                    procpid, timenow);

            vsnprintf ( (buffer + headlen), (2048 - headlen),
                    sFormat, args );

            memcpy(buffer, head, headlen);

            write(fd, buffer, strlen(buffer));
            close(fd);

        }

        va_end ( args );                                       
    }

    int LogShmQueue::reset()
    {

        writeownlog("----- caution call LogShmQueue reset here !!!!!-----------\n");

        if(NULL != m_ptHeader) {
            m_ptHeader->iResetFlag = 0;
        }


        //sleep 10 seconds, in order to make sure that all other clients can read the reset flag
        //so we will lost logs for 10 seconds
        sleep(10);

        int shmid = 0;
        {
            for(int i = 0; i < 10; i++) {
                shmid = shmget ( m_iShmKey, m_iShmSize, 0666 );
                if( shmid<0 )
                {
                    shmid = shmget ( m_iShmKey, m_iShmSize, 0666|IPC_CREAT ) ;
                }
                if( shmid < 0) {
                    continue;
                } else {
                    break;
                }
            }

            if ( shmid<0 )
            {
                return -1;
            }
        }

        {

            LogShmQueueHead_t * ptHeader = NULL;

            for(int i = 0; i < 10; i++) {
                ptHeader = (LogShmQueueHead_t *) shmat ( shmid, NULL, 0 ) ;
                if ( -1 == (long)ptHeader )
                {
                    continue;
                } else {
                    break;
                }
            }

            if ( -1 == (long)ptHeader )
            {
                ptHeader = NULL;
                return -1;
            }

            m_ptHeader = ptHeader;

            memset ( m_ptHeader, 0x0, sizeof(LogShmQueueHead_t) );
            m_ptHeader->hMainVer = COMM_LOG_SHM_MAINVER;
            m_ptHeader->hSubVer = COMM_LOG_SHM_SUBVER;
            m_ptHeader->iMagicNum = COMM_LOG_SHM_MAGIC;
            m_ptHeader->iQueueSize = m_iShmSize - sizeof(LogShmQueueHead_t) ;
            m_ptHeader->iHeadPos = m_ptHeader->iTailPos = 0;
            memset((char*)m_ptHeader+sizeof(LogShmQueueHead_t), 0x0, m_ptHeader->iQueueSize);
        }
        m_ptHeader->iResetFlag = COMM_LOG_SHM_RESET_FLAG;
        return 0;
    }

	int LogShmQueue :: pop_with_size( int iMaxSize, int * tLen, char ** ptBuf, int & retCount )
	{
		int iRet = 0;

		retCount = 0;
		while( iMaxSize-- )
		{

			iRet = pop( tLen[retCount], ptBuf[retCount] );

			if (  0 == iRet )
			{
				retCount++;
				continue;
			}

			if(COMM_LOG_SHM_QUEUE_EMPTY == iRet) { 
				break;
			}

			if(COMM_LOG_SHM_LOG_NOT_READY == iRet) { 
				break;
			}

			if(COMM_LOG_SHM_ERROR_RESET == iRet || 
					COMM_LOG_SHM_ERROR_ARG == iRet || 
					COMM_LOG_SHM_ERROR_MAGICNUM1_CORRUPT == iRet || 
					COMM_LOG_SHM_ERROR_MAGICNUM2_CORRUPT == iRet || 
					COMM_LOG_SHM_ERROR_LEN_CORRUPT == iRet) 
			{ 

				break;
			}

		}
		if ( retCount != 0 && iRet == COMM_LOG_SHM_QUEUE_EMPTY )
		{
			iRet = 0;
		}
		return iRet;
	}
}

