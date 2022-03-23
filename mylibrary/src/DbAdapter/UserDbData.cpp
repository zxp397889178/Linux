#include <vector>
#include <fstream>
#include <iostream>
#include "UserDbData.h"
#include "DaFieldTable.h"
#include "DaFieldSet.h"

using namespace my_data;

//将数据库中当前记录打包到pData中，包的实际长度为cbData
// bPackData 指示是否打包数据，缺省为 false
void PackRow(ISimDB *pDB, CFieldSet *pFieldset, 
    byte_t *&pData, size_t &cbData, bool bPackData=false);
extern void ReplaceAll(CMyString &strSource, CMyString &strSub, CMyString &strVal);

//////////////////////////////////////////////////////////////////////////
CDBUserData::CDBUserData()
	: IDataOperator()
    //, m_pFieldsetVec(new CFieldSetVector())
    , m_bNewResult(false)
    , m_nResultIndex(-1)
{
    //ASSERT(m_pFieldsetVec);
	m_vecFieldSet.reserve(20);
}

CDBUserData::~CDBUserData()
{
    //SAFE_DELETE(m_pFieldsetVec);
	m_vecFieldSet.clear();
}

bool CDBUserData::Init(const char *pszTable, ISimDB *pDB)
{
    m_pDB = pDB;

    // 获取绝对路径
    char szPath[256] = "";
	char szFile[256] = "";
	GetFullPath(szFile, sizeof(szFile), pszTable);
	GetFullPathName(szPath, sizeof(szPath), szFile);

    std::ifstream ifile;
    ifile.open(szFile);
    if (!ifile.is_open())
    {
        return false;
    }

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
            ASSERT(pFieldset);
            if (pFieldset)
            {
				FieldSetInfo field(pFieldset, false);
				m_vecFieldSet.push_back(field);
                //m_pFieldsetVec->push_back(pFieldset);
            }
        }
        else if (!strnicmp(buf, "[RCDSET]", 8))
        {
            CFieldSet *pFieldset = FIELD_TABLE.LoadTable(szPath, &buf[8]);
            ASSERT(pFieldset);
            if (pFieldset)
            {
				FieldSetInfo field(pFieldset, true);
				m_vecFieldSet.push_back(field);
                //m_pFieldsetVec->push_back(pFieldset);
            }
        }
    }
	if (m_vecFieldSet.empty())
    //if (m_pFieldsetVec->empty())
    {
        //没有装载入任何数据
        return false;
    }
    return true;
}

bool CDBUserData::LoadUserBaseData(uint32_t userId, CUserData &objUser)
{
    //CFieldSetVector::iterator it(m_pFieldsetVec->begin());
	FieldSetVector::iterator it(m_vecFieldSet.begin());
	FieldSetInfo &baseFieldInfo = *it;

    //判断数据库中是否存在用户
    bool bFindUser(false);
    CFieldSet *pBaseFieldset = baseFieldInfo.pFieldSet;
    ASSERT(pBaseFieldset);
    size_t cbLength = pBaseFieldset->GetLength();
    CMyString strSQL = m_pDB->Cmd(pBaseFieldset->GetSelectSql(), userId);
	//printf("exec sql [%s]\n", strSQL.c_str());
    while(m_pDB->More())
    {
        byte_t *pData = NULL;
        PackRow(m_pDB, pBaseFieldset, pData, cbLength, false);
		my_data::CRecord *pRecord = objUser.QueryRecord("USER_BASE");
		if (NULL == pRecord)
		{
			return false;
		}
        int32_t ret = pRecord->Init(pData, cbLength);
        ASSERT(ret>0);
		//pRecord->SetSelectSQL(strSQL.c_str());
		//pRecord->BindDataOperator(this);
        free(pData);
        bFindUser = true;
    }
    char szError[256] = "";
    int nError = m_pDB->GetLastError(szError, sizeof(szError));
    if (0 == nError)
    {
        return bFindUser;
    }
    //出错
    return false;
}

bool CDBUserData::AddUserData(uint32_t userId)
{
    //执行SQL语句
    long nResult(0);
    long nError(-1);
    char szNote[256] = "";
    m_pDB->Cmd("CALL new_user(%lu)", userId);
    while(m_pDB->More())
    {
        nResult = m_pDB->GetInt(0);
        nError = m_pDB->GetInt(2);
        m_pDB->GetString(3, szNote, sizeof(szNote));
    }
    size_t cbNote = strlen(szNote);
    char szNote2[256] = "";
    size_t cbNote2 = sizeof(szNote2);
    //Convert("UTF8", "GBK", szNote2, cbNote2, szNote, cbNote*sizeof(wchar_t));
    utf8_to_gbk(szNote2, cbNote2, szNote, cbNote);

    if (nResult <= 0)
    {
        std::cout << "创建新用户失败，[" << nError << "]" << szNote2 << std::endl;
        return false;
    }
    else
    {
        std::cout << szNote2 << std::endl;
    }

    return true;
}

bool CDBUserData::LoadUser(uint32_t userId, CUserData &objUser, bool bAutoNewUser)
{
    bool ret(false);
    int times(0);

	objUser.BindDataSource(this, userId);

    do 
    {
		//printf("try load user base data.\n");
        ret = this->LoadUserBaseData(userId, objUser);
        if (!ret)
        {
			if (bAutoNewUser)
			{
            	this->AddUserData(userId);
			}
			else
			{
				return false;
			}
        }
        if (++times > 2)
        {
            return false;
        }
    } while (!ret);

    //读取用户的其他数据
    //uint32_t userId = objUser.GetUserID();
	CMyString strSQL;
    //CFieldSetVector::iterator it(m_pFieldsetVec->begin());
    //for(++it; it != m_pFieldsetVec->end(); ++it)
	FieldSetVector::iterator it(m_vecFieldSet.begin());
	for(++it; it != m_vecFieldSet.end(); ++it)
    {
        //CFieldSet *pFieldset = *it;
		FieldSetInfo &tmpField = *it;
		CFieldSet *pFieldset = tmpField.pFieldSet;
        ASSERT(pFieldset);
        strSQL = m_pDB->Cmd(pFieldset->GetSelectSql(), userId);
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
    m_nResultIndex = 0; //首个记录得跳过
    //it = m_pFieldsetVec->begin();
	it = m_vecFieldSet.begin();
    CFieldSet *pFieldset(NULL);
    size_t cbLength(0);
	bool bIsRecordset(true);
    CRecordSet *pRecordSet(NULL);
	CRecord *pNewRecord(NULL);
    m_pDB->SetHandler(this);
    while(m_pDB->More())
    {
        if (m_bNewResult)
        {
		
            m_bNewResult = false;

            //pFieldset = m_pFieldsetVec->at(m_nResultIndex);
			FieldSetInfo &fieldInfo = m_vecFieldSet.at(m_nResultIndex);
			pFieldset 		= fieldInfo.pFieldSet;
			bIsRecordset 	= fieldInfo.isRecordset;
            ASSERT(pFieldset);
            cbLength 		= pFieldset->GetLength();
			//CMyString sql 	= arSQL.at(m_nResultIndex-1);
			CMyString strUniqueName = pFieldset->GetUniqueName();
			if (bIsRecordset)
			{
				LogDebug("=== load %s, is RECORDSET!!!===", strUniqueName.c_str());
				pRecordSet = objUser.QueryRecordSet(strUniqueName.c_str());
				//pRecordSet->SetSelectSQL(sql.c_str());
				//pRecordSet->BindDataOperator(this);
			}
			else
			{
				LogDebug("=== load %s, is RECORD  ===", strUniqueName.c_str());
				pNewRecord = objUser.QueryRecord(strUniqueName.c_str());
				//pNewRecord->SetSelectSQL(sql.c_str());
				//pNewRecord->BindDataOperator(this);
			}
            //pRecordSet = objUser.QueryRecordSet(pFieldset->GetUniqueName());
            //ASSERT(pRecordSet);
        }
        byte_t* pData(NULL);
        PackRow(m_pDB, pFieldset, pData, cbLength, false);
		if (bIsRecordset)
		{
        	pRecordSet->AddRecord(pData, cbLength, false);
		}
		else
		{
			pNewRecord->Init(pData, cbLength, false);
		}
        free(pData);
    }
    m_pDB->SetHandler(NULL);

    return true;
}

void CDBUserData::OnEvent(const DB_EVENT dbEvent, double timeCost)
{
    UNUSED(dbEvent);
    UNUSED(timeCost);
}

void CDBUserData::OnError(const DB_EVENT dbEvent, const char *pszSql)
{
    UNUSED(dbEvent);
    UNUSED(pszSql);
}

void CDBUserData::OnNewResult(void* results, void* fields, uint32_t field_num)
{
    UNUSED(results);
    UNUSED(fields);
    UNUSED(field_num);
    m_bNewResult = true;
    ++m_nResultIndex;
}

void CDBUserData::OnNewRow(char **row, uint32_t *lengths)
{
    UNUSED(row);
    UNUSED(lengths);
}

void CDBUserData::SetAppName(const char *pszAppName)
{
    UNUSED(pszAppName);
}


bool CDBUserData::Reload(CRecord *pRecord, bool bIgnoreChange)
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
        ASSERT(ret>0);
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

bool CDBUserData::Reload(CRecordSet *pRecordSet, bool bIgnoreChange)
{
	if (NULL == pRecordSet)
	{
		//printf("==== record set is null.\n");
		return false;
	}

	CMyString strSQL = pRecordSet->GetSelectSql();
	if (strSQL.empty())
	{
		//printf("==== sql empty.\n");
		return false;
	}

	if (!bIgnoreChange)
	{
		for(size_t idx(0); idx<pRecordSet->Count(); ++idx)
		{
			CRecord *pRecord = pRecordSet->at(idx);
			if (NULL != pRecord && pRecord->IsChanged())
			{
				//printf("==== record set is changed.\n");
				return false;
			}
		}
	}

	CFieldSet *pFieldSet = pRecordSet->GetFieldSet();
	size_t cbLength = pFieldSet->GetLength();
	pRecordSet->ClearAll();
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
	LogMsg2("USERDBDATA:RELOAD", 0, "reload [%s] success. count=[%ld]", pFieldSet->GetUniqueName(), pRecordSet->Count());
	return true;
}

//////////////////////////////////////////////////////////////////////////
void PackRow(ISimDB *pDB, CFieldSet *pFieldset,
    byte_t *&pData, size_t &cbData, bool bPackData/* = false*/)
{
    cbData = pFieldset->GetLength();
    pData = (byte_t*)calloc(sizeof(byte_t), cbData+1);
    memset(pData, 0, cbData+1);
    byte_t *ptr = pData;
    size_t cbReal = 0;

    for(size_t index=0; index<pFieldset->GetCount(); ++index)
    {
        CField *pField = pFieldset->Field(index);
		my_data::FIELD_TYPE eType = pField->GetType();
        uint32_t cbFieldLen = pField->GetLength();
		//LogDebug("Field [%s], len [%u], type[%u]",
		//		pField->GetName(), cbFieldLen, eType);
        size_t cbRealFieldLen(cbFieldLen);
        switch(eType)
        {
		case my_data::TYPE_INT:
            {
                long val = pDB->GetInt(index);
                switch(cbFieldLen)
                {
                case 1:
                    ptr[0] = (char)val;
                    break;
                case 2:
                    *(short*)ptr = (short)val;
                    break;
                case 4:
                    *(int*)ptr = (int)val;
                    break;
                case 8:
                    *(int64_t*)ptr = pDB->GetInt64(index);
                    break;
                default:
                    break;
                }
            }
            break;
		case my_data::TYPE_UINT:
            {
                uint32_t val = pDB->GetUInt(index);
                switch(cbFieldLen)
                {
                case 1:
                    ptr[0] = (uint8_t)val;
                    break;
                case 2:
                    *(uint16_t*)ptr = (uint16_t)val;
                    break;
                case 4:
                    *(uint32_t*)ptr = val;
                    break;
                case 8:
                    *(uint64_t*)ptr = pDB->GetUInt64(index);
                    break;
                default:
                    break;
                }
            }
            break;
		case my_data::TYPE_DECIMAL:
            {
                double val = pDB->GetDouble(index);
                *(double*)ptr = val;
            }
            break;
		case my_data::TYPE_STRING:
            if (bPackData)
            {
                char *buff = (char*)&ptr[2];
                const char *pTemp = pDB->GetString(index, buff, cbFieldLen-2);
                uint16_t cbTemp2 = (uint16_t)strlen(pTemp); 
                *(uint16_t*)ptr = cbTemp2;
                cbRealFieldLen = cbTemp2+2;
            }
            else
            {
                pDB->GetString(index, (char*)ptr, cbFieldLen);
            }
            break;
		case my_data::TYPE_BINARY:
            {
                byte_t *pc(NULL);
                size_t cbTemp(0);
                pDB->GetData(index, pc, cbTemp);
                if (bPackData)
                {
                    uint16_t cbTemp2 = (uint16_t)__min(cbTemp, cbFieldLen-2);
                    *(uint16_t*)ptr = cbTemp2;
                    memcpy(&ptr[2], pc, cbTemp2);
                    cbRealFieldLen = cbTemp2+2;
                }
                else
                {
                    memcpy(ptr, pc, __min(cbTemp, cbFieldLen));
                }
            }
            break;
		case my_data::TYPE_DATE:
		case my_data::TYPE_TIME:
		case my_data::TYPE_DATETIME:
            {
                time_t val = pDB->GetTime(index);
                *(time_t*)ptr = val;
            }
            break;
        }

        ptr += cbRealFieldLen;
        cbReal += cbRealFieldLen;
    }

    if (bPackData)
    {
        cbData = cbReal;
    }
}





