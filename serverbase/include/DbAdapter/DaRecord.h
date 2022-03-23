/*
 * =====================================================================================
 *
 *       Filename:  CRecord.h
 *
 *    Description:  数据缓存的记录
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

/* 对 Record的操作规定
   1. 创建单条记录
      CRecord *pRecord = new CRecord("abc.tbl");
      //对于记录集中的空白记录，请使用RecordSet中的NewRecord方法

   2. 更新记录中的数据
      //整行数据更新，一般用于初始化过程
      pRecord->Init(pData, cbLen, false);
      //记录中某个字段更新
      pRecord->Field[1]->Set("hello,world!");

   3. 记录删除
      pRecord->Delete();
      //不充许在RecordSet中调用Delete方染
   
*/

#ifndef _GFX_SERVER_DARECORD_H_
#define _GFX_SERVER_DARECORD_H_

#include <vector>
#include "DaFieldSet.h"
#include "DaData.h"

class ISimDB;
namespace my_data{

enum RECORD_FLAG
{
    OP_DUMMY,       // 空闲状态
    OP_INSERT,      // 新增状态
    OP_UPDATE,      // 更新状态
    OP_DELETE,      // 删除状态
};

class CDataVector;
class CRecordSet;
class CBucket;
class IDataOperator;
class CRecord
{
    friend class CData;
    friend class CBitArray;
    friend class CRecordSet;
    friend class CBucket;
    NON_COPYABLE(CRecord)
public:
    //创建一个空的CRecord
    CRecord(const char *pszPath, const char *pszTable);
    explicit CRecord(CFieldSet *pFieldSet);
    void release();

    //初始化数据,返回-1 表示错误（一般是空间不足），>=0 表示读取了pData的长度
    int Init(byte_t *pData, size_t cbData, bool bPackData = false);

    //打包数据到缓冲区
    //
    //打包的数据格式: [STATUS(1byte)][UNIQUE_NAME(16byte)][DATA]
    //  其中是否有UNIQUE_NAME,依赖于 bTitle参数
    //  STATUS： 'N' 无变化,'I' 新增数据, 'U' 更新数据, 'D' 删除数据
    //
    // 参数       pData   -- 缓冲区
    //            cbData  -- 缓冲区大小
    //            bTitle  -- 是否在首部加上UniqueName, 主要是用于区分记录的类型
    //            bPacked -- 是否压缩，仅对 String 及 Binary 类型的字段有效
    // 返回值: -1 缓冲区大小不足
    //        >=0 打包的数据大小
    int PackData(byte_t *pData, size_t cbData, bool bTitle = false, bool bPacked = false);

    //获得对应字段的数据
    CData* operator[](int nIndex);
    CData* Field(int nIndex);
    CData* Field(const char *pszName);
	size_t FieldCount(void);

    //索引值
    CData* GetKey();
    bool HasKey(void) { return this->GetKey() != NULL; }
    bool IsChanged(void) const { return m_flag != my_data::OP_DUMMY; }

    //设置记录为删除状态
    void Delete();

    //刷写数据库
    int Flush(byte_t *pData, size_t cbData);

    //获得SQL语句
    void Flush(CMyString &strSql);

    //获取全局名称
    const char* GetUniqueName(void) const;
    //获取记录的长度
    size_t GetLength(void) const;
    
    inline CFieldSet* GetFieldSet(void) const { return m_pFieldset; }
    inline bool IsNew(void) const { return m_flag == OP_INSERT; }
    inline bool IsDelete(void) const { return m_flag == OP_DELETE; }

public:
	void BindDataOperator(IDataOperator *op) { m_pDataOperator = op; }
	bool Reload(bool ignoreChange);

	void SetSelectSQL(const char *pszSql) { m_strSelectSQL = pszSql; }
	CMyString& GetSelectSql(void) { return m_strSelectSQL; }

	ISimDB* getDBConn(void);

	// 打印记录集中的数据，用于调用
	void Dump(void);

private:
    //创建一个记录集中的空白记录
    // pOwner 用于指示父集合，NULL表示独立的Record,非Null表示为RecordSet中的子集
    CRecord(const char *pszPath, const char *pszTable, CRecordSet *pOwner);
    CRecord(CFieldSet *pFieldSet, CRecordSet *pOwner);

    ~CRecord();

    int UpdateRecord(CRecord *pRecord);

private:
    //设置记录为变更状态
    void Update();
    //设置记录为新增状态
    void SetInsertFlag();
    //设置为初始状态
    void ClearFlag();

    //查找字段名对应的索引号
    int FieldIndex(const char *pszName);

    //初始化压缩的数据
    int InitPackData(byte_t *pData, size_t cbData);

private:
    byte_t*                 m_pData;        //数据缓存，真实的数据空间，需要释放
	std::vector<CData*>		m_vecData;
    //CDataVector*            m_pDataVec;     //数据集合
    CFieldSet*              m_pFieldset;    //全局管理，不释放
	IDataOperator*			m_pDataOperator;//数据操作类，用于重新装载数据之用，不释放

    enum RECORD_FLAG        m_flag;
    CRecordSet* const       m_pRecordSet;   //仅用于删除,只充许在构造函数中赋值

	CMyString 				m_strPath;

	CMyString				m_strSelectSQL;	// 用于重新装载的SQL语句,若是RecordSet中的子集，则为空
};

} // namespace my_data

#endif  //_GFX_SERVER_DARECORD_H_

