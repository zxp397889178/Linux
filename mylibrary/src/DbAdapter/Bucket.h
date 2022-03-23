/*
 * =====================================================================================
 *
 *       Filename:  Bucket.h
 *
 *    Description:  多个记录、记录集的整合
 *
 *        Version:  1.0
 *        Created:  2014-4-1
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DA_BUCKET_H_
#define _GFX_SERVER_DA_BUCKET_H_

#include "DaRecord.h"
#include "DaRecordSet.h"
#include "IBucketEvent.h"

namespace my_data{

class CRecordsetMap;
class CRecordMap;
class IDataOperator;
class CBucket
{
    NON_COPYABLE(CBucket)
public:
    CBucket();
    ~CBucket();

    //初始化bucket文件
    bool Init(IBucketEvent *pEvent, const char *pszConfig);

    CRecord* QueryRecord(const char *pszName);
    CRecordSet* QueryRecordSet(const char *pszName);

    //对所有记录、记录集的数据 打包进pData
    //返回 -1 表示缓存空间不足
    int PackData(byte_t *pData, size_t cbData, bool bPacked = false, bool bChangedOnly = false );
    //打包并压缩数据,小于1000字节的包文不启用压缩，由 bCompressed 指示是否已压缩
    int PackDataEx(byte_t *&pData, size_t &cbData, bool &bCompressed, bool bChangedOnly = false);

    //将数据反向导入记录集中,此处bPacked 是指对包文内部的字符串或二进制是否有采用压缩模式，非指对数据包采用Compress
    int UnpackData(byte_t *pData, size_t cbData, bool bPacked = false);
    //解压并导入数据
    int UnpackDataEx(byte_t *pData, size_t cbData);

    //刷写到服务端,返回值－1表示失败
    int Flush(void);

    //刷写到服务端
    int Flush2DB(void);

    inline void SetID(uint32_t _ID)         { m_ID = _ID; }
    inline void SetKeepRecordFlag(bool val) { m_bKeepRecordFlag = val; }

	void BindDataSource(IDataOperator *pDbSource, uint32_t userId);
private:
    int UnpackRecordSet(byte_t *pData, size_t cbData, bool bPacked = false);
    int UnpackRecord(char cType, byte_t *pData, size_t cbData, bool bPacked = false);
    int UnpackRecord(byte_t *pData, size_t cbData, CRecordSet *pRecordSet, bool bPacked = false);

    void OnFlushOK(void);

private:
    CRecordMap*         m_pRecordMap;       //记录名称 <-> 记录 对照表
    CRecordsetMap*      m_pRecordSetMap;    //记录集名称 <-> 记录集对照表
    IBucketEvent*       m_pEvent;           //回调事件

    uint32_t            m_ID;               //唯一标识CBucket的ID
    bool                m_bKeepRecordFlag;  //是否保留Unpack后记录的标识，对于Client端为false,对于Server端为true
};

} //namespace my_data

#endif  //_GFX_SERVER_DA_BUCKET_H_
