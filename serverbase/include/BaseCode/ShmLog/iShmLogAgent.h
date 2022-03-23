
#pragma once

#include <stdarg.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>

#include "iLog.h"

typedef struct _CShmLogAgentImpl_t CShmLogAgentImpl_t;

void StopShmLogAgent();

class CShmLogAgent {
    public:

        enum {
            eMax_LOG_SLEEP_WHEN_NOT_READY=100,
            eMax_LOG_SLEEP_WHEN_EMPTY=500,
            eMax_LOG_NOT_READY_COUNT=1000
        };

        CShmLogAgent ();
        virtual ~CShmLogAgent ();

        int Init(const char *sPath=COMM_LOG_SHM_PATH, const int iMaxSize=Comm::CLog::eMAX_LOG_SIZE,
                const int iMaxLogNotReadyCount=eMax_LOG_NOT_READY_COUNT, const int iSleepWhenEmpyt=eMax_LOG_SLEEP_WHEN_EMPTY,
                const int iSleepWhenNotReady=eMax_LOG_SLEEP_WHEN_NOT_READY );

        int Run();
        int Stop();

    private:

        void GetLogName( int iInterval, char * sName, int size );
        int CheckLogName();
        int CheckLogSize();

        CShmLogAgentImpl_t * m_ptImpl; 

        //int m_iStop;

};







