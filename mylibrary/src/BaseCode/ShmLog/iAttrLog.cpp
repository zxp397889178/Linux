#include "iLog.h"

#include "iAttrLog.h"
#include "iLogShmErr.h"
#include <cstring>

using namespace Comm;

static CAttrLog iLog;


CAttrLog :: CAttrLog()
{
	m_ptImpl = NULL;
	int ret = NewImpl();
	if ( ret != 0 )
	{
		Comm::LogErr("ERR: %s(mario) newimpl %d\n", __func__, ret );
	}
}

CAttrLog :: ~CAttrLog()
{
	ClearImpl();
}

int CAttrLog :: push( int len, char * pcBuf )
{
	if ( m_ptImpl == NULL || m_ptImpl->queue == NULL )
	{
		int ret = NewImpl();
		if ( ret != 0 )
		{
			return ret;
		}
	}
	int ret(0);
	for( int i = 0; i < 5; ++i )
	{
		ret = m_ptImpl->queue->push(len, pcBuf);
		if ( ret != COMM_LOG_SHM_ERROR_PUSH_RETRY_FAILED )
		{
			break;
		}
	}
	return ret;
}

int CAttrLog :: NewImpl()
{
	if ( NULL != m_ptImpl )
	{
		ClearImpl();
	}
	
	m_ptImpl = new CAttrLogImpl_t;

	if ( NULL == m_ptImpl )
	{
		return -1;
	}

	m_ptImpl->queue = new LogShmQueue();

	if ( NULL == m_ptImpl->queue )
	{
		ClearImpl();
		return -1;
	}

	int ret = m_ptImpl->queue->init( COMM_LOG_ATTR_SHM_KEY, COMM_LOG_SHM_SIZE ) ;
	if ( ret != 0 )
	{
		Comm::LogErr("ERR: %s(mario) init queue failed ret %d", __func__, ret );
		ClearImpl();
		return ret;
	}
	return ret;
}

int CAttrLog :: ClearImpl()
{
	if ( NULL == m_ptImpl )
	{
		return 0;
	}

	if ( m_ptImpl->queue != NULL )
	{
		delete m_ptImpl->queue, m_ptImpl->queue = NULL;
	}
	delete m_ptImpl, m_ptImpl = NULL;
	return 0;
}
	

int ReportAttrLog( int len, char * pcBuf )
{
	if ( len == 0 || pcBuf == NULL )
	{
		Comm::LogErr("ERR: %s(mario) report len is 0 or pcBuf is NULL", __func__ );
		return 0;
	}
	int ret = -1;
	if ( iLog.m_ptImpl == NULL )
	{
		ret = iLog.NewImpl();
		if ( ret != 0 )
		{
			iLog.ClearImpl();
			Comm::LogErr("ERR: %s(mario) newimpl %d\n", __func__, ret );
			return ret;
		}
	}
	char * pcLogBuf = iLog.m_ptImpl->pcLogBuf; 

	unsigned short tmpLen = (unsigned short)len;
	memcpy( pcLogBuf, &tmpLen, sizeof(tmpLen));
	memcpy( pcLogBuf + 2, pcBuf, len );
	(* (pcLogBuf + len + 2) ) = LogShmQueue::COMM_LOG_ITEM_MAGICNUM_2;
	len += 3;

	ret = iLog.push( len, pcLogBuf );
	if ( ret != 0 )
	{
		Comm::LogErr("ERR: %s(mario) push ret %d", __func__, ret );
		//TODO : ERROR deal
	}
	return ret;
}


//
