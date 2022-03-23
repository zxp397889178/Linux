#ifndef __IATTRLOG_H__
#define __IATTRLOG_H__

#include "iLogShm.h"
#include <cstring>


using Comm::LogShmQueue;

typedef struct tagCAttrLog
{
	LogShmQueue * queue;

	char pcLogBuf[2048];
}CAttrLogImpl_t;

class CAttrLog
{
	public:
		CAttrLog();

		~CAttrLog();

		int push( int len, char *pcBuf );

		int NewImpl();

		int ClearImpl();
	public:
		CAttrLogImpl_t * m_ptImpl;
};

int ReportAttrLog( int len, char * pcBuf );


#endif 

