/*
 * =====================================================================================
 *
 *       Filename:  UserData.h
 *
 *    Description:  字典集合
 *
 *        Version:  1.0
 *        Created:  2014－4－21
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  wsk
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DA_DICTDATA_H_
#define _GFX_SERVER_DA_DICTDATA_H_

#include "BaseCode.h"

namespace my_data{

class IBucketEvent;
class CRecordSet;
class CBucket;
class CDictData
{
    friend class CDictMap;
    NON_COPYABLE(CDictData);
public:
    CDictData();
    ~CDictData();

    //config => DictData.tbl
    bool Init(IBucketEvent *pEvent, const char *pszConfig);

    //查询记录集合名称
    CRecordSet* QueryRecordSet(const char *pszUniqueName) const;

    int PackDataEx(byte_t *&pData, size_t &cbData, bool &bCompressed);

    int UnpackData(byte_t *pData, size_t cbData, bool bPacked = false);
    int UnpackDataEx(byte_t *pData, size_t cbData);

    int Flush(void);
    int Flush2DB(void);

private:
    CBucket*            m_pBucket;     //数据集合
};

}   //namespace my_data

#endif // _GFX_SERVER_DA_DICTDATA_H_
