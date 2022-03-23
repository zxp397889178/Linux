#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <algorithm>
#include "SimRecord.h"
#include "MyRecord.h"
#include "SimMysql.h"



//extern ISimDB* CreateMysqlConn(const char *user_name, const char *passwd, 
//    const char *host, const char *db, int32_t port, ISimDBEvent *pEvent,
//    const char *appname);

//MYPOOL_EX_IMPLEMENTATION(CSimRecord, s_objHeap)
//////////////////////////////////////////////////////////////////////////
//CSimRecord
CSimRecord::CSimRecord()
    : m_pDB(NULL)
	, m_fields()  
    , m_records()
    , m_mapRecords()
    , m_vecRemoved()
    , m_bCanFlush(false)
{
}

CSimRecord::~CSimRecord()
{
    //SAFE_RELEASE(m_pDB);
}

void CSimRecord::release(void)
{
    delete this;
}

class RestoreDbHandler
{
public:
    RestoreDbHandler(ISimDB* pDB, ISimDBEvent* pHandler)
        : m_pDB(pDB)
        , m_pOldHandler(NULL)
    {
        CSimMySql *pMySql = dynamic_cast<CSimMySql*>(m_pDB);
        DB_ASSERT(pMySql);
        m_pOldHandler = pMySql->GetHandler();
        pMySql->SetHandler(pHandler);
    }

    ~RestoreDbHandler()
    {
        if (m_pDB)
            m_pDB->SetHandler(m_pOldHandler);
    }

private:
    ISimDB*             m_pDB;
    ISimDBEvent*        m_pOldHandler;
};

bool CSimRecord::Open(ISimDB *pDB, const char *fmt, ...)
{
    m_pDB = pDB;
    if (NULL == m_pDB)
    {
        return false;
    }

	RestoreDbHandler restoreHandler(m_pDB, this);
    //构建SQL命令
    char buffer[ISimDB::MAX_SQL_BUFFER] = "";
    va_list args;
    va_start(args, fmt);
    int32_t cnt = vsnprintf(buffer, sizeof(buffer)-1, fmt, args);
    va_end(args);
    if (cnt<0)
    {
        return false;
    }

    //执行SQL语句
    m_pDB->Cmd(buffer);
    int32_t ret = m_pDB->Exec();
    if (ret < 0)
    {
        return false;
    }
    while(m_pDB->More());
    int32_t err = m_pDB->GetLastError();
    return (err==0);
}

bool CSimRecord::LoadTable(ISimDB *pDB, const char *pszTable, const char *fmtCond, ...)
{
    char buffer[ISimDB::MAX_SQL_BUFFER] = "";
    va_list args;
    va_start(args, fmtCond);
    int32_t cnt = vsnprintf(buffer, sizeof(buffer)-1, fmtCond, args);
    va_end(args);
    if (cnt<0)
    {
        return false;
    }

    //可回写状态
    m_bCanFlush = true;
    CMyString strSql("SELECT * FROM %s WHERE %s", pszTable, buffer);
    bool ret = this->Open(pDB, strSql.c_str());
    return ret;
}

//bool CSimRecord::Open(const char *user_name, const char *passwd,
//    const char *host, const char *db/* =NULL */, int32_t port/* =0 */,
//    const char *appname/* =NULL */)
//{
//    m_pDB = CreateMysqlConn(user_name, passwd, host, db, port, this, appname);
//    DB_ASSERT(m_pDB!=NULL);
//    return (m_pDB != NULL);
//}
//
//bool CSimRecord::Execute(const char *fmt, ...)
//{
//    DB_ASSERT(m_pDB != NULL);
//
//    //构建SQL命令
//    char buffer[ISimDB::MAX_SQL_BUFFER] = "";
//    va_list args;
//    va_start(args, fmt);
//#if (defined WIN32 && _MSC_VER>=1400)
//    int32_t cnt = _vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, fmt, args);
//#else
//    int32_t cnt = _vsnprintf(buffer, sizeof(buffer)-1, fmt, args);
//#endif
//    va_end(args);
//    if (cnt<0)
//    {
//        return false;
//    }
//    
//    //执行SQL语句
//    m_pDB->Cmd(buffer);
//    int32_t ret = m_pDB->Exec();
//    if (ret < 0)
//    {
//        return false;
//    }
//    while(m_pDB->More());
//    int32_t err = m_pDB->GetLastError();
//    return (err==0);
//}

void CSimRecord::OnEvent(const DB_EVENT dbEvent, double timeCost)
{
    UNUSED(dbEvent);
    UNUSED(timeCost);
}

void CSimRecord::OnError(const DB_EVENT dbEvent, const char *pszSql)
{
    UNUSED(dbEvent);
    char szError[512] = "";
    int32_t err = 0;
    if (m_pDB)
        err = m_pDB->GetLastError(szError, sizeof(szError));
	LogErr("<err> id %d, msg:%s, sql:%s",  err, szError, SAFE_STR(pszSql));
}

void CSimRecord::OnNewResult(void* results, void* fields, uint32_t field_num)
{
    UNUSED(results);
    m_fields.Reset();
    m_records.clear();
    m_mapRecords.clear();

    //构建字段
    m_fields.Init((MYSQL_FIELD*)fields, (uint8_t)field_num);

    //判断是否存在主键
    if (m_bCanFlush)
    {
        std::vector<IMyField*> arKeyField;
        size_t nKeyCount = m_fields.GetPrimaryField(arKeyField);
        if (nKeyCount==0)
        {
            DB_ASSERT(0);
        }
    }
}

void CSimRecord::OnNewRow(char **row, uint32_t *lengths)
{
    CMyRecord *pRecord = new CMyRecord(&m_fields, (MYSQL_ROW)row, lengths);
    if (!pRecord)
    {
        return;
    }

    //获得主键
    char szKey[256] = "";
    bool ret = pRecord->GetKey(szKey, sizeof(szKey));
    if (ret)
    {
        uint32_t nKey = OpenSSL_Hash(szKey);
        std::pair<MAP_RECORDS::iterator, bool> result = 
            m_mapRecords.insert(MAP_RECORDS::value_type(nKey, pRecord));
        if (!result.second)
        {
            delete pRecord;
            return; //直接返回吧
        }
    }

    //写入记录集
    m_records.push_back(pRecord);
}

void CSimRecord::SetAppName(const char *pszAppName)
{
    UNUSED(pszAppName);
}

size_t CSimRecord::Count(void) const
{
    return m_records.size();
}

IMyRecord* CSimRecord::at(int32_t index)
{
    if (index<0 || index>=(int)this->Count())
    {
        return NULL;
    }
    return m_records.at(index);
}

const IMyRecord* CSimRecord::at(int32_t index) const
{
    if (index<0 || index>=(int)this->Count())
    {
        return NULL;
    }
    return m_records.at(index);
}

IMyRecord* CSimRecord::operator[](int32_t index)
{
    if (index<0 || index>=(int)this->Count())
    {
        return NULL;
    }
    return m_records[index];
}

const IMyRecord* CSimRecord::operator[](int32_t index) const
{
    if (index<0 || index>=(int)this->Count())
    {
        return NULL;
    }
    return m_records[index];
}

int32_t CSimRecord::GetFieldCount() const
{
    return m_fields.FieldCount();
}

const char* CSimRecord::GetFieldName(int32_t nIndex)
{
    IMyField* pField = m_fields.Find((uint8_t)nIndex);
    if (!pField)
        return "";

    return pField->GetName();
}

IMyRecord* CSimRecord::Find(const char *pszKey)
{
    uint32_t nKey = OpenSSL_Hash(pszKey);
    if (0 == nKey)
    {
        return NULL;
    }
    MAP_RECORDS::iterator it(m_mapRecords.find(nKey));
    if (it == m_mapRecords.end())
    {
        return NULL;
    }
    IMyRecord *pRecord = it->second;
    if (NULL == pRecord)
    {
        return NULL;
    }
    if (!pRecord->IsValid())
    {
        return NULL;
    }
    return pRecord;
}

bool CSimRecord::Add(const char *pszKey, IMyRecord *pRecord)
{
    uint32_t nKey = OpenSSL_Hash(pszKey);
    if (0 == nKey)
    {
        return false;
    }
    std::pair<MAP_RECORDS::iterator, bool> result = 
        m_mapRecords.insert(MAP_RECORDS::value_type(nKey, pRecord));
    if (!result.second)
    {
        return false;
    }
    m_records.push_back(pRecord);
    return true;
}

bool CSimRecord::Remove(const char *pszKey)
{
    uint32_t nKey = OpenSSL_Hash(pszKey);
    if (0 == nKey)
    {
        return false;
    }
    MAP_RECORDS::iterator it(m_mapRecords.find(nKey));
    if (it != m_mapRecords.end())
    {
        IMyRecord *pRecord = it->second;
        if (NULL != pRecord)
        {
            m_records.erase(std::remove(m_records.begin(), m_records.end(), pRecord),
                m_records.end());
        }
        int32_t val = pRecord->Delete();
        if (val != 0)
        {
            //压入队列后续删除
            m_vecRemoved.push_back(pRecord);
        }
        m_mapRecords.erase(it);
        return true;
    }
    return false;
}

const char* CSimRecord::GetFieldNameByIndex(int32_t nIndex)
{
    IMyField* pField = m_fields.Find((uint8_t)nIndex);
    if (!pField)
        return "";

    return pField->GetName();
}

const char* CSimRecord::GetTableName()
{
    IMyField* pField = m_fields.Find(0);
    if (!pField)
        return "";

    return pField->GetTable();
}

int32_t CSimRecord::Flush()
{
    if (!m_bCanFlush)
    {
        return 0;
    }

    int32_t nAffectRow(0);
    //删除优先
    for(CONTAINER::iterator it(m_vecRemoved.begin()); it != m_vecRemoved.end(); ++it)
    {
        IMyRecord *pRecord = *it;
        if (NULL == pRecord)
            continue;

        if (pRecord->Flush(m_pDB))
        {
            ++nAffectRow;
            pRecord = NULL;
            //DELETE_RECORD(pRecord); //Flush里面已经释放了，此处不再重复释放
        }
    }
    m_vecRemoved.clear();

    //新增加或更新
    for(CONTAINER::iterator iter(m_records.begin()); iter != m_records.end(); ++iter)
    {
        IMyRecord *pRecord = *iter;
        if (NULL == pRecord)
            continue;

        if (pRecord->Flush(m_pDB))
        {
            ++nAffectRow;
        }
    }

    return nAffectRow;
}

IMyRecord* CSimRecord::NewRecord(void)
{
    CMyRecord *pRecord = new CMyRecord(&m_fields);
    DB_ASSERT(pRecord!=NULL);
    return pRecord;
}

int32_t CSimRecord::SaveToFile(const char *pszFile, enum ExportFileType eType)
{
    if (eType != ftCsv)
    {
        //其他文件格式暂不支持
        return -1;
    }

    std::ofstream ofile;
    ofile.open(pszFile, std::ios_base::out|std::ios_base::trunc);
    if (!ofile.is_open())
    {
        //无法创建文件
        return -1;
    }

    int32_t nFieldCount = m_fields.FieldCount();
    //输出表头
    for(int32_t i=0; i<nFieldCount; ++i)
    {
        ofile << m_fields[i]->GetName();
        if (i < nFieldCount-1)
        {
            ofile << ",";
        }
    }
    ofile << std::endl;

    int32_t nRow(0);
    //逐行输出表内容
    for(size_t row=0; row<m_records.size(); ++row)
    {
        IMyRecord *pRecord = m_records.at(row);
        DB_ASSERT(pRecord);
        for(int32_t col=0; col<nFieldCount; ++col)
        {
            IMyData *pData = pRecord->Field(col);
            DB_ASSERT(pData);
#if 0
            IMyField *pField = m_fields[col];
            switch(pField->GetType())
            {
            case TYPE_DECIMAL:
                ofile << pData->toString();
                break;
            case TYPE_TINY:
            case TYPE_SHORT:
            case TYPE_LONG:
                ofile << pData->toString();
                break;
            case TYPE_FLOAT:
            case TYPE_DOUBLE:
                ofile << pData->toString();
                break;
            case TYPE_NULL:
                ofile << '';
                break;
            case TYPE_LONGLONG:
            case TYPE_INT24:
                ofile << pData->toString();
                break;
            case TYPE_VAR_STRING:
            case TYPE_STRING:
                ofile << pData->toString();
                break;
            default:
                DB_ASSERT(0);
                break;
            }
#else
            ofile << pData->toString();
#endif
            if (col < nFieldCount-1)
                ofile << ",";
        }
        ofile << std::endl;
        ++nRow;
    }
    return nRow;
};

