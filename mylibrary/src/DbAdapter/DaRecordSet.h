/*
 * =====================================================================================
 *
 *       Filename:  DaRecordSet.h
 *
 *    Description:  记录集合
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

/* 对 RecordSet的操作规定
   1. 从数据库中装载记录集
      CRecordSet recordSet;
      ... ( 从数据库中获得数据内容)
      //逐行添加到记录集中
      for(...)
      {
         recordSet.AddRecord(pData, cbLen, false);
      }

   2. 增加一条空白的记录
      CRecordSet recordSet;
      CRecord *pRecord = recordSet.NewRecord();
      //或者直接传入数据
      // CRecord* pRecord = recordSet.NewRecord(pData, cbLen, false);

   3. 删除一条记录
      CRecordSet recordSet;
      CRecord *pRecord = recordSet.Find(nKey);
      pRecord->Delete();
      // 不支持直接从RecordSet中删除某条记录

   4. 有变更的数据回写到数据库
      CRecordSet recordSet;
      ...
      recordSet.Flush();
      // 或者可以编写刷新策略，自动处理回写
*/


#ifndef _GFX_SERVER_DA_RECORD_SET_H_
#define _GFX_SERVER_DA_RECORD_SET_H_

#include "DaRecord.h"
namespace my_data{

class CRecordVector;
class CRecordIndex;
class CFieldSet;
class IDataOperator;
class CRecordSet
{
    friend class CRecord;
    friend class CBucket;
    typedef std::vector<CRecord*>     RECORD_VECTOR;
public:
    CRecordSet(const char *pszPath, const char *pszTable);
    ~CRecordSet();

    //查找指定的记录集
    CRecord* operator[] (int nIndex);
    CRecord* at(int nIndex);
    CRecord* Find(uint32_t nKey);
    size_t Count();

    //创建一行新的记录(为数据库中不存在，对应 INSERT 语句)
    CRecord* NewRecord(uint32_t nKey);
    CRecord* NewRecord(byte_t* pData, size_t cbData, bool bPackData = false);
    
    //从外部导入数据进Record
    CRecord* AddRecord(byte_t* pData, size_t cbData, bool bPackData = false);
    CRecord* AddRecord(CRecord *pRecord);

    //更新Record数据
    CRecord* UpdateRecord(CRecord *pRecord, bool bKeepFlag = false);

    //删除Record数据
    void DeleteRecord(uint32_t nKey);

	void ClearAll(void);

    //将数据打包到pData中
    //
    //打包数据格式为 : [STATUS(1byte)][UNIQUE_NAME(16byte)][RECORD_COUNT(2byte)][DATA]
    //  其中 STATUS只能为 'S'，表示一个集合
    //
    // 参数: pData          数据缓冲区
    //       cbData         缓冲区大小,必须保证有足够的大小
    //       bPacked        是否打包数据，仅对String 和 Binary 数据有效
    //       bChangedOnly   是否仅打包有变更的数据
    // 返回值: -1  缓冲区太小
    //          0  无变更数据
    //         >0  已填充的数据大小
    int PackData(byte_t *pData, size_t cbData, bool bPacked = false, bool bChangedOnly = false);

    //回写有变更的记录
    int Flush(byte_t *pData, size_t cbData);
    void Flush(CMyString &strSql);

    const char* GetUniqueName() const;
    CFieldSet* GetFieldSet(void) const { return m_pFieldSet; }

	void SetSelectSQL(const char *pszSql)  { m_strSelectSQL = pszSql; }
	CMyString& GetSelectSql(void) { return m_strSelectSQL; }

	void BindDataOperator(IDataOperator *op) { m_pDataOperator = op; }
	bool Reload(bool ignoreChange);

private:
    //删除记录
    void Delete(CRecord *pRecord);

    void ClearFlag(void);

private:
    char                        m_szTable[128];     //表定义文件
    CRecordVector*              m_pRecordVec;       //记录集合
    CRecordIndex*               m_pIndex;           //索引
    CFieldSet*                  m_pFieldSet;        //字段映射表
	IDataOperator*				m_pDataOperator;	// 数据重载指针，用于重新装载数据之用，不可释放

	CMyString					m_strPath;
	CMyString					m_strSelectSQL;		// 用于重新装载的SQL语句
};

}   //namespace my_data


#endif  //_GFX_SERVER_DA_RECORD_SET_H_


