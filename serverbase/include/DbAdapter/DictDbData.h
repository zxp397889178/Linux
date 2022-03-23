/*
 * =====================================================================================
 *
 *       Filename:  UserDbData.h
 *
 *    Description:  字典表在数据库中的数据管理
 *
 *        Version:  1.0
 *        Created:  2014-4-20
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  wsk
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DA_DICT_DBDATA_H_
#define _GFX_SERVER_DA_DICT_DBDATA_H_

#include "DictData.h"
#include "BaseCode.h"
#include "MyDB.h"
#include "IDataOperator.h"
#include "DaFieldSet.h"

namespace my_data{

//class CFieldSetVector;
class CFieldSetVector : public std::vector< my_data::CFieldSet* >
{
};
//typedef std::vector<my_data::CFieldSet* > CFieldSetVector;
class CDBDictData : public ISimDBEvent, public IDataOperator
{
public: 
    CDBDictData();
    virtual ~CDBDictData();

    bool Init(const char* pszTable, ISimDB* pDB);
    bool LoadDict(CDictData& objDict);

public:
	virtual bool Reload(CRecord *pRecord, bool bIgnoreChange);
	virtual bool Reload(CRecordSet *pRecordSet, bool bIgnoreChange);

public:
    virtual void OnEvent(const DB_EVENT dbEvent, double timeCost);
    virtual void OnError(const DB_EVENT dbEvent, const char *pszSql);
    virtual void OnNewResult(void* results, void* fields, uint32_t field_num);
    virtual void OnNewRow(char **row, uint32_t *lengths);
    virtual void SetAppName(const char *pszAppName);

private:
    //ISimDB*             m_pDB;
    CFieldSetVector*    m_pFieldsetVec;

    bool                m_bNewResult;
    int                 m_nResultIndex;     //结果集索引顺序，有可能是空结果集，跳过
};
}

#endif  //_GFX_SERVER_DA_DICT_DBDATA_H_
