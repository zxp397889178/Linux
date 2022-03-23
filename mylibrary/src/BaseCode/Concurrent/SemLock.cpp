/*
 * =====================================================================================
 *
 *       Filename:  SemLock.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年11月20日 20时37分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "SemLock.h"
#include <errno.h>
#include <string.h>
#include <string>



CSemLock::CSemLock()
{

}

CSemLock::CSemLock(key_t iKey)
{

}

CSemLock::~CSemLock()
{

}

void CSemLock::init(key_t iKey)
{
    #if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)  
    /* union semun is defined by including <sys/sem.h> */  
    #else  
    /* according to X/OPEN we have to define it ourselves */  
    union semun  
    {  
         int val;                  /* value for SETVAL */  
         struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */  
         unsigned short *array;    /* array for GETALL, SETALL */  
                                   /* Linux specific part: */  
         struct seminfo *__buf;    /* buffer for IPC_INFO */  
    };  
    #endif  
  
    int  iSemID;  
    union semun arg;  
    u_short array[2] = { 0, 0 };  
  
    // 生成信号量集, 包含两个信号量  
    if ( (iSemID = semget( iKey, 2, IPC_CREAT | IPC_EXCL | 0666)) != -1 )  
    {  
        arg.array = &array[0];  
  
        //将所有信号量的值设置为0  
        if ( semctl( iSemID, 0, SETALL, arg ) == -1 )  
        {  
            // throw TC_SemMutex_Exception("[TC_SemMutex::init] semctl error:" + std::string(strerror(errno)));  
			// TODO
			return;
        }  
    }  
    else  
    {  
        //信号量已经存在  
        if ( errno != EEXIST )  
        {  
            // throw TC_SemMutex_Exception("[TC_SemMutex::init] sem has exist error:" + std::string(strerror(errno)));  
			// TODO
			return;
        }  
  
        //连接信号量  
        if ( (iSemID = semget( iKey, 2, 0666 )) == -1 )  
        {  
            // throw TC_SemMutex_Exception("[TC_SemMutex::init] connect sem error:" + std::string(strerror(errno)));  
			// TODO
			return;
        }  
    }  
  
    m_semKey = iKey;  
    m_semID  = iSemID;  	
}

int CSemLock::rlock() const  
{  
    //进入共享锁, 第二个信号量的值表示当前使用信号量的进程个数  
    //等待第一个信号量变为0(排他锁没有使用)  
    //占用第二个信号量(第二个信号量值+1, 表示被共享锁使用)  
    struct sembuf sops[2] = { {0, 0, SEM_UNDO}, {1, 1, SEM_UNDO} };  
    size_t nsops = 2;  
  
    return semop( m_semID, &sops[0], nsops);  
}  
  
int CSemLock::unrlock( ) const  
{  
    //解除共享锁, 有进程使用过第二个信号量  
    //等到第二个信号量可以使用(第二个信号量的值>=1)  
    struct sembuf sops[1] = { {1, -1, SEM_UNDO} };  
    size_t nsops = 1;  
  
    return semop( m_semID, &sops[0], nsops);  
}  
  
bool CSemLock::tryrlock() const  
{  
    struct sembuf sops[2] = { {0, 0, SEM_UNDO|IPC_NOWAIT}, {1, 1, SEM_UNDO|IPC_NOWAIT}};  
    size_t nsops = 2;  
  
    int iRet = semop( m_semID, &sops[0], nsops );  
    if(iRet == -1)  
    {  
        if(errno == EAGAIN)  
        {  
            //无法获得锁  
            return false;  
        }  
        else  
        {  
            // throw TC_SemMutex_Exception("[TC_SemMutex::tryrlock] semop error : " + string(strerror(errno)));  
			// TODO
			return false;
        }  
    }  
    return true;  
}  
  
int CSemLock::wlock() const  
{  
    //进入排他锁, 第一个信号量和第二个信号都没有被使用过(即, 两个锁都没有被使用)  
    //等待第一个信号量变为0  
    //等待第二个信号量变为0  
    //释放第一个信号量(第一个信号量+1, 表示有一个进程使用第一个信号量)  
    struct sembuf sops[3] = { {0, 0, SEM_UNDO}, {1, 0, SEM_UNDO}, {0, 1, SEM_UNDO} };  
    size_t nsops = 3;  
  
    return semop( m_semID, &sops[0], nsops);  
}  
  
int CSemLock::unwlock() const  
{  
    //解除排他锁, 有进程使用过第一个信号量  
    //等待第一个信号量(信号量值>=1)  
    struct sembuf sops[1] = { {0, -1, SEM_UNDO} };  
    size_t nsops = 1;  
  
    return semop( m_semID, &sops[0], nsops);  
  
}  
  
bool CSemLock::trywlock() const  
{  
    struct sembuf sops[3] = { {0, 0, SEM_UNDO|IPC_NOWAIT}, {1, 0, SEM_UNDO|IPC_NOWAIT}, {0, 1, SEM_UNDO|IPC_NOWAIT} };  
    size_t nsops = 3;  
  
    int iRet = semop( m_semID, &sops[0], nsops );  
    if(iRet == -1)  
    {  
        if(errno == EAGAIN)  
        {  
            //无法获得锁  
            return false;  
        }  
        else  
        {  
            // throw TC_SemMutex_Exception("[TC_SemMutex::trywlock] semop error : " + string(strerror(errno)));  
			// TODO
			return false;
        }  
    }  
  
    return true;  
}  

bool CSemLock::Lock(bool bReadLock)
{
	int ret(0);
	if (bReadLock)
	{
		ret = this->rlock();
	}
	else
	{
		ret = this->wlock();
	}
	m_bReadLock = bReadLock;
	return (ret==0);
}

void CSemLock::Unlock()
{
	int ret(0);
	if (m_bReadLock)
	{
		ret = this->unrlock();
	}
	else
	{
		ret = this->unwlock();
	}
}

bool CSemLock::TryLock(unsigned long nMilliSeconds, bool bReadLock)
{
	int ret(0);
	m_bReadLock = bReadLock;
	if (m_bReadLock)
	{
		ret = this->tryrlock();
	}
	else
	{
		ret = this->trywlock();
	}
	return (ret==0);
}
