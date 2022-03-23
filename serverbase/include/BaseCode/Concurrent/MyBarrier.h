/*
 * =====================================================================================
 *
 *       Filename:  MyBarrier.h
 *
 *    Description:  barrier object
 *
 *        Version:  1.0
 *        Created:  2014/11/17 23时47分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MY_BARRIER_H_
#define _MYLIB_BASECODE_MY_BARRIER_H_


// http://blog.csdn.net/keyboardota/article/details/6867346
/* µÈ´ý¶à¸öÏß³ÌÍ¬Ê±Íê³É
    CMyBarrier barrier;
    barrier.Init(2);

    >>>Ïß³Ì1
    ...
    barrier.Wait();

    >>>Ïß³Ì2
    ...
    barrier.Wait();

*/

#include "../mydef.h"
#include "../ILock.h"
#include <pthread.h>

#ifndef OS_MAC
class CMyBarrier
{
public:
    CMyBarrier(void);
    ~CMyBarrier(void);

    bool Init(unsigned long nCount = 0);
    bool Wait();

private:
    pthread_barrier_t   m_LockObj;
};

#endif  //OS_MAC


#endif  //_MYLIB_BASECODE_MY_BARRIER_H_


