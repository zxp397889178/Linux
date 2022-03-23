#include "DictDbData.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "MyDB.h"
#include "DaField.hpp"
#include "DaFieldSet.h"
#include "DaFieldTable.h"
#include "DaData.h"
#include "DaRecord.h"
#include "DaRecordSet.h"
#include "Bucket.h"
#include "UserDbData.h"
#include "UserData.h"
#include "IBucketEvent.h"
#include <assert.h>


using namespace my_data;
//将数据库中当前记录打包到pData中，包的实际长度为cbData
// bPackData 指示是否打包数据，缺省为 false
extern void PackRow(ISimDB *pDB, CFieldSet *pFieldset, 
    byte_t *&pData, size_t &cbData, bool bPackData=false);

//////////////////////////////////////////////////////////////////////////
CDBDictData::CDBDictData()
	: IDataOperator()
    , m_pFieldsetVec(new CFieldSetVector())
    , m_bNewResult(false)
    , m_nResultIndex(-1)
{
    DB_ASSERT(m_pFieldsetVec);
}

CDBDictData::~CDBDictData()
{
    SAFE_DELETE(m_pFieldsetVec);
}

bool CDBDictData::Init(const char* pszTable, ISimDB* pDB)
{
    m_pDB = pDB;

    // 获取绝对路径
    char szFile[256] = "";
	GetFullPath(szFile, sizeof(szFile), pszTable);
	char szPath[256] = "";
	GetFullPathName(szPath, sizeof(szPath), szFile);

    std::ifstream ifile;
    ifile.open(szFile);
    if (!ifile.is_open())
        return false;

    while(!ifile.eof())
    {
        char buf[1024] = "";
        ifile.getline(buf, sizeof(buf));
        char *pc = strchr(buf, '#');
        if (NULL != pc)
        {
            *pc = '\0';
        }
        trim(buf);
        //内容太短，认为是非法
        if (strlen(buf)<10)
        {
            continue;
        }
        //加载具体的文件
        if (!strnicmp(buf, "[RECORD]", 8))
        {
            CFieldSet *pFieldset = FIELD_TABLE.LoadTable(szPath, &buf[8]);
            DB_ASSERT(pFieldset);
            if (pFieldset)
            {
                m_pFieldsetVec->push_back(pFieldset);
            }
        }
        else if (!strnicmp(buf, "[RCDSET]", 8))
        {
            CFieldSet *pFieldset = FIELD_TABLE.LoadTable(szPath, &buf[8]);
            DB_ASSERT(pFieldset);
            if (pFieldset)
            {
                m_pFieldsetVec->push_back(pFieldset);
            }
        }
    }

    if (m_pFieldsetVec->empty())
    {
        //没有装载入任何数据
        return false;
    }

    return true;
}

bool CDBDictData::LoadDict(CDictData &objDict)
{
    CFieldSetVector::iterator it(m_pFieldsetVec->begin());
	CMyString strSQL;
    for (; it != m_pFieldsetVec->end(); ++it)
    {
        CFieldSet *pFieldset = *it;
        DB_ASSERT(pFieldset);
        strSQL = m_pDB->Cmd(pFieldset->GetSelectSql());

		CRecordSet *pRecordSet = objDict.QueryRecordSet(pFieldset->GetUniqueName());
		if (pRecordSet == NULL)
		{
			LogFatal2("DICT", 0, "load dict get unique name [%s] failure", pFieldset->GetUniqueName());
			return false;
		}

		pRecordSet->SetSelectSQL(pFieldset->GetSelectSql());
		pRecordSet->BindDataOperator(this);
    }

	std::vector<CMyString> arSQL;
	std::string::size_type pos(0);
	while(true)
	{
		std::string::size_type pos1 = strSQL.find_first_of(";", pos);
		if (pos1 == std::string::npos)
		{
			break;
		}
		CMyString sql = strSQL.substr(pos, pos1-pos);
		pos = pos1 + 1;
		//printf("sql [%s]\n", sql.c_str());
		arSQL.push_back(sql);
	}

    //设置句柄，用于获取数据
    m_bNewResult = false;
    size_t cbLength(0);
    CFieldSet *pFieldset(NULL);
    CRecordSet *pRecordSet(NULL);
    m_pDB->SetHandler(this);

    while(m_pDB->More())
    {
        if (m_bNewResult)
        {
            m_bNewResult = false;
            pFieldset = m_pFieldsetVec->at(m_nResultIndex);
            DB_ASSERT(pFieldset);
            pRecordSet = objDict.QueryRecordSet(pFieldset->GetUniqueName());
            DB_ASSERT(pRecordSet);
            cbLength = pFieldset->GetLength();
			CMyString strSQL = arSQL.at(m_nResultIndex);
			//printf("[DICT]load dict exec sql: [%s] success\n", strSQL.c_str());
			//pRecordSet->SetSelectSQL(strSQL.c_str());
			//pRecordSet->BindDataOperator(this);
        }

        byte_t* pData(NULL);
        PackRow(m_pDB, pFieldset, pData, cbLength, false);
        pRecordSet->AddRecord(pData, cbLength, false);
        free(pData);
    }

	// m_nResultIndex -1 开始，出错了是不会++的， 所以要加2
	if (m_nResultIndex + 2 <= arSQL.size())
	{
		CMyString strSQL = arSQL.at(m_nResultIndex + 1);
		LogFatal2("DICT", 0, "ERR:load dict exec sql [%s] error.", strSQL.c_str());
		return false;
	}

    m_pDB->SetHandler(NULL);
	printf("[DICT]load all dict success\n");
    return true;
}

void CDBDictData::OnEvent(const DB_EVENT dbEvent, double timeCost)
{
    UNUSED(dbEvent);
    UNUSED(timeCost);
}

void CDBDictData::OnError(const DB_EVENT dbEvent, const char *pszSql)
{
    //printf("CDBDictData::OnError m_nResultIndex: %d\n", m_nResultIndex);
    DB_ASSERT(0);

    UNUSED(dbEvent);
    UNUSED(pszSql);
}

void CDBDictData::OnNewResult(void* results, void* fields, uint32_t field_num)
{
    UNUSED(results);
    UNUSED(fields);
    UNUSED(field_num);
    m_bNewResult = true;
    ++m_nResultIndex;

	CFieldSet *pFieldset = m_pFieldsetVec->at(m_nResultIndex);
	if (pFieldset != NULL)
	{
		printf("[DICT]load dict exec sql: [%s] success\n", pFieldset->GetSelectSql());
	}
	
}

void CDBDictData::OnNewRow(char **row, uint32_t *lengths)
{
    UNUSED(row);
    UNUSED(lengths);
}

void CDBDictData::SetAppName(const char *pszAppName)
{
    UNUSED(pszAppName);
}

bool CDBDictData::Reload(CRecord *pRecord, bool bIgnoreChange)
{
	if (NULL == pRecord)
	{
		return false;
	}

	CMyString strSQL = pRecord->GetSelectSql();
	if (strSQL.empty())
	{
		return false;
	}

	if (pRecord->IsChanged() && !bIgnoreChange)
	{
		return false;
	}

	CFieldSet *pFieldSet = pRecord->GetFieldSet();
	size_t cbLength = pFieldSet->GetLength();
	// 重新装载数据
	m_pDB->Cmd(strSQL.c_str());
	while(m_pDB->More())
	{
        byte_t *pData = NULL;
        PackRow(m_pDB, pFieldSet, pData, cbLength, false);
        int ret = pRecord->Init(pData, cbLength);
        DB_ASSERT(ret>0);
        free(pData);
	}
    char szError[256] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogFatal("error:%d, msg:%s, sql:%s", err, szError, strSQL.c_str());
        return false;
    }
	return true;
}

bool CDBDictData::Reload(CRecordSet *pRecordSet, bool bIgnoreChange)
{
	if (NULL == pRecordSet)
	{
		//printf("ERR:record set is null\n");
		return false;
	}

	CMyString strSQL = pRecordSet->GetSelectSql();
	if (strSQL.empty())
	{
		//printf("ERR:select sql is empty.\n");
		return false;
	}

	if (!bIgnoreChange)
	{
		for(size_t idx(0); idx<pRecordSet->Count(); ++idx)
		{
			CRecord *pRecord = pRecordSet->at(idx);
			if (NULL != pRecord && pRecord->IsChanged())
			{
				//printf("ERR: record set has modified.\n");
				return false;
			}
		}
	}

	CFieldSet *pFieldSet = pRecordSet->GetFieldSet();
	size_t cbLength = pFieldSet->GetLength();
    pRecordSet->ClearAll();
	//printf("exec sql: [%s]\n", strSQL.c_str());
	m_pDB->Cmd(strSQL.c_str());
    while(m_pDB->More())
    {
        byte_t* pData(NULL);
        PackRow(m_pDB, pFieldSet, pData, cbLength, false);
        pRecordSet->AddRecord(pData, cbLength, false);
        free(pData);
    }
    char szError[256] = "";
    int32_t err = m_pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogFatal("error:%d, msg:%s, sql:%s", err, szError, strSQL.c_str());
        return false;
    }
	LogMsg2("DICT:RELOAD", 0, "reload [%s] success. count=[%ld]", pFieldSet->GetUniqueName(), pRecordSet->Count());
	return true;
}
