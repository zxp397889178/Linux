/*
* =====================================================================================
*
*       Filename:  Signal.hpp
*
*    Description:  信号
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

#ifndef _QW_FRAME_SIGNAL_H_
#define _QW_FRAME_SIGNAL_H_

#include "Slot.hpp"
#include <map>
#include <string>

using std::string;
using std::map;

template<typename T1, typename T2>
class CSignal
{
	typedef std::map<T1, CSlot<T2>* > SLOT_MAP;
public:
	CSignal()
	{
	}
	~CSignal()
	{
		typename SLOT_MAP::iterator iter = m_mapSlot.begin();
		for (; iter != m_mapSlot.end(); ++iter)
		{
			if (iter->second != NULL)
			{
				delete iter->second;
			}
		}
	}
	template<typename T>
	void Bind(T1 messageId, T* pObj, int32_t(T::*pMemberFunc)(T2&))
	{
		CSlot<T2> *pSlot = new CSlot<T2>(pObj, pMemberFunc);
		if (pSlot != NULL)
		{
			m_mapSlot.insert(make_pair(messageId, pSlot));
			//LogDebug("CSignal::Bind message [%s]", messageId.c_str());
		}
	}

	int32_t Handle(T1& messageId, T2& para)
	{
		//LogDebug("CSignal::Handle message [%s], message size:[%ld]", messageId.c_str(), m_mapSlot.size());
		typename SLOT_MAP::iterator iter = m_mapSlot.find(messageId);
		if (iter == m_mapSlot.end())
		{
			return -999;
		}
		return iter->second->Execute(para);
	}

	bool Find(T1& messageId)
	{
		typename SLOT_MAP::iterator iter = m_mapSlot.find(messageId);
		if (iter == m_mapSlot.end())
		{
			return false;
		}

		return true;
	}

	int32_t Size() 
	{
		return m_mapSlot.size();
	}

private:
	SLOT_MAP m_mapSlot;
};


#endif	// _QW_FRAME_SIGNAL_H_
