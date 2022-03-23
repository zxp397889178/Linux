/*
* =====================================================================================
*
*       Filename:  Slot.hpp
*
*    Description:  槽
*
*        Version:  1.0
*        Created:  2017年06月29日 09时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  www.quwangame.com
*
* =====================================================================================
*/

#ifndef _QW_FRAME_SLOT_H_
#define _QW_FRAME_SLOT_H_

#include <stdint.h>

template<typename T1>
class CSlotBase
{
public:
	virtual int32_t Execute(T1& para) = 0;
};


template<typename T, typename T1>
class CSlotImpl : public CSlotBase<T1>
{
public:
	CSlotImpl(T* pObj, int32_t (T::*pMemberFunc)(T1&))
	{
		m_pObj = pObj;
		m_pMemberFunc = pMemberFunc;
	}
	virtual int32_t Execute(T1& para)
	{
		return (m_pObj->*m_pMemberFunc)(para);
	}
private:
	T* m_pObj;
	int32_t (T::*m_pMemberFunc)(T1&);
};

template<typename T1>
class CSlot
{
public:
	template<typename T>
	CSlot(T* pObj, int32_t(T::*pMemberFunc)(T1&))
	{
		m_pSlotBase = new CSlotImpl<T, T1>(pObj, pMemberFunc);
	}
	~CSlot()
	{
		if (m_pSlotBase != NULL)
		{
			delete m_pSlotBase;
		}
	}

	int32_t Execute(T1& para)
	{
		return m_pSlotBase->Execute(para);
	}
private:
	CSlotBase<T1>* m_pSlotBase;
};

#endif	// _QW_FRAME_SLOT_H_

