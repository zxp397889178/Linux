/*
 * =====================================================================================
 *
 *       Filename:  UserData.h
 *
 *    Description:  角色数据集合
 *
 *        Version:  1.0
 *        Created:  2014－3－28
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DA_USERDATA_H_
#define _GFX_SERVER_DA_USERDATA_H_

#include "DaRecord.h"
#include "DaRecordSet.h"
#include "Bucket.h"
#include "IBucketEvent.h"

namespace my_data{

class IDataOperator;
class CUserData
{
    friend class CUserMap;
    NON_COPYABLE(CUserData)
public:
    CUserData();
    virtual ~CUserData();

    //config => UserData.tbl
    bool Init(IBucketEvent *pEvent, const char *pszConfig);
    //查询记录集合名称
    CRecord* QueryRecord(const char *pszUniqueName) const;
    CRecordSet* QueryRecordSet(const char *pszUniqueName) const;

    //查询用户的ID
    uint32_t GetUserID(void);
    virtual uint32_t GetID(void);

    //返回 -1 表示缓存空间不足
    int PackData(byte_t *pData, size_t cbData, bool bPacked = false, bool bChangedOnly = false );
    int PackDataEx(byte_t *&pData, size_t &cbData, bool &bCompressed, bool bChangedOnly = false);

    //将数据反向导入记录集中
    int UnpackData(byte_t *pData, size_t cbData, bool bPacked = false);
    int UnpackDataEx(byte_t *pData, size_t cbData);
    
    inline int Flush(void)     {  return m_pBucket->Flush(); }
    inline int Flush2DB(void)  { return m_pBucket->Flush2DB(); }
    void SetBucketID(uint32_t _ID);
    void SetKeepRecordFlag(bool val);

	void BindDataSource(IDataOperator *pDbSource, uint32_t userId);
public:
    inline CRecord* Base(void) const        { return m_pBase; }
    //inline CRecordSet* Building(void) const { return m_pBuildingSet; }

private:
    CBucket*            m_pBucket;     //数据集合

    CRecord*            m_pBase;            //用户基础数据记录
    //CRecordSet*         m_pBuildingSet;     //建筑的记录集
};


}   //namespace my_data

#endif  //_GFX_SERVER_DA_USERDATA_H_

