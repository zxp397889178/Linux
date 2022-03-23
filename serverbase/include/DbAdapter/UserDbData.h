/*
 * =====================================================================================
 *
 *       Filename:  UserDbData.h
 *
 *    Description:  用户在数据库中的数据管理
 *
 *        Version:  1.0
 *        Created:  2014-3-29
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DA_USER_DBDATA_H_
#define _GFX_SERVER_DA_USER_DBDATA_H_

#include "UserData.h"
#include "BaseCode.h"
#include "MyDB.h"
#include "IDataOperator.h"
//#include <map>

namespace my_data{

struct FieldSetInfo
{
	my_data::CFieldSet*		pFieldSet;
	bool					isRecordset;

	FieldSetInfo(my_data::CFieldSet* _fieldset, bool isset = true)
		: pFieldSet(_fieldset)
		, isRecordset(isset)  
	{
	}
};
typedef std::vector<FieldSetInfo> 	FieldSetVector;
//class CFieldSetVector : public std::vector< my_data::CFieldSet* >
//{
//};

class CDBUserData : public ISimDBEvent, public IDataOperator
{
public:
    CDBUserData();
    virtual ~CDBUserData();

    bool Init(const char *pszTable, ISimDB *pDB);

	/* 从数据库中装载用户数据
	 * param:	userId			需要装载的user id
	 * 			objUser			装载的用户数据容器
	 * 			bAutoNewUser	true-数据库不存在userId时自动创建新用户; false-不自动创建用户
	 * */
    bool LoadUser(uint32_t userId, CUserData &objUser, bool bAutoNewUser = false);

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
    bool LoadUserBaseData(uint32_t userId, CUserData &objUser);
    bool AddUserData(uint32_t userId);

private:
    //ISimDB*             m_pDB;
    //CFieldSetVector*    m_pFieldsetVec;
	FieldSetVector		m_vecFieldSet;

    bool                m_bNewResult;
    int                 m_nResultIndex;     //结果集索引顺序，有可能是空结果集，跳过
};

}   //namespace my_data

#endif  //_GFX_SERVER_DA_USER_DBDATA_H_

