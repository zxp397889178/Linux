/*
 * =====================================================================================
 *
 *       Filename:  DaRecordIndex.hpp
 *
 *    Description:  DaRecord的索引表
 *
 *        Version:  1.0
 *        Created:  2014-3-27
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DA_RECORD_INDEX_HPP_
#define _GFX_SERVER_DA_RECORD_INDEX_HPP_

#include "DaRecord.h"
#include <map>
#include <iterator>
#include <algorithm>

namespace my_data{


//template<typename T>
class TRecordIndex
{
    typedef std::map<uint32_t, CRecord*>     INDEX_MAP;
public:
    TRecordIndex() : m_mapRecord()
    {
    }

    ~TRecordIndex()
    {
    }

    bool Build(CRecord* pVal)
    {
        CData *pData = pVal->GetKey();
        if (NULL == pData)
        {
            return false;
        }

        uint32_t key = pData->AsUint32();
        if (this->Find(key))
        {
            return false;
        }

        m_mapRecord.insert(std::make_pair(key, pVal));
        return true;
    }

    CRecord* Find(uint32_t key)
    {
		//typename INDEX_MAP::iterator it(m_mapRecord.find(key));
		INDEX_MAP::iterator it(m_mapRecord.find(key));
        if (it == m_mapRecord.end())
        {
            return NULL;
        }
        return it->second;
    }

    void Erase(uint32_t key)
    {
        m_mapRecord.erase(key);
    }

    void Clear()
    {
        m_mapRecord.clear();
    }
private:
    INDEX_MAP       m_mapRecord;
};


}


#endif  //_GFX_SERVER_DA_RECORD_INDEX_HPP_

