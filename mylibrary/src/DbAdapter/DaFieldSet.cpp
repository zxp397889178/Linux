#include "DaFieldSet.h"
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <assert.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
namespace my_data{

class CFieldVector : public std::vector<CField*>
{
};

class CFieldMap : public std::map<std::string, CField*>
{
};

CField* NewField(FieldInfo &fieldInfo)
{
    CField *pField = new CField(fieldInfo);
    DB_ASSERT(pField);
    return pField;
}

}   // namespace my_data

using namespace my_data;
//////////////////////////////////////////////////////////////////////////
static bool Table2Fields(const char *pszTable, 
    CFieldVector &fieldSet,
    CFieldMap &fieldMap,
    char *pszUniqueName, size_t cbName, //全局唯一名称，用于索引指示之用
    char *&pszSelect, char *&pszInsert, char *&pszUpdate, char *&pszDelete);
    static void DestroyFieldSet(CFieldVector &fieldSet,
    CFieldMap &fieldMap);

//////////////////////////////////////////////////////////////////////////
CFieldSet::CFieldSet(const char* pszTable)
    : m_pFieldVec(new CFieldVector() )
    , m_pFieldMap(new CFieldMap() )
    , m_pszSelect(NULL)
    , m_pszInsert(NULL)
    , m_pszUpdate(NULL)
    , m_pszDelete(NULL)
{
    DB_ASSERT(m_pFieldVec);
    DB_ASSERT(m_pFieldMap);
    memset(m_szUniqueName, 0, sizeof(m_szUniqueName));
    bool ret = Table2Fields(pszTable, *m_pFieldVec, *m_pFieldMap, 
        m_szUniqueName, sizeof(m_szUniqueName),
        m_pszSelect, m_pszInsert, m_pszUpdate, m_pszDelete);
    DB_ASSERT(ret);
    if (!ret)
    {
        DestroyFieldSet(*m_pFieldVec, *m_pFieldMap);
    }

	//printf("name [%s], sql [%s]\n", m_szUniqueName, m_pszSelect);
}

CFieldSet::~CFieldSet()
{
    DestroyFieldSet(*m_pFieldVec, *m_pFieldMap);
    SAFE_DELETE(m_pFieldVec);
    SAFE_DELETE(m_pFieldMap);
    free(m_pszSelect);
    free(m_pszInsert);
    free(m_pszUpdate);
    free(m_pszDelete);
}

CField* CFieldSet::Field(int nIndex)
{
    if (nIndex<0 || nIndex>=(int)m_pFieldVec->size())
    {
        return NULL;
    }
    return m_pFieldVec->at(nIndex);
}

CField* CFieldSet::Field(const char* pszName)
{
    CFieldMap::iterator it(m_pFieldMap->find(pszName));
    if (it != m_pFieldMap->end())
    {
        return it->second;
    }
    return NULL;
}

CField* CFieldSet::operator[](int index)
{
    return this->Field(index);
}

bool CFieldSet::IsValid()
{
    return !m_pFieldVec->empty();
}

size_t CFieldSet::GetLength()
{
    size_t cbLength(0);
    CFieldVector::iterator it(m_pFieldVec->begin());
    for(; it != m_pFieldVec->end(); ++it)
    {
        CField *pField = *it;
        cbLength += pField->GetLength();
    }
    return cbLength;
}

size_t CFieldSet::GetCount()
{
    return m_pFieldVec->size();
}

CField* CFieldSet::GetKeyField(void)
{
    CFieldVector::iterator it(m_pFieldVec->begin());
    for(; it != m_pFieldVec->end(); ++it)
    {
        CField *pField = *it;
        if (pField->IsKeyField())
        {
            return pField;
        }
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////
static bool IsValidLine(const char *buf)
{
    static const int MIN_CHAR_SIZE = 10;

    //长度太短，认为不是合法的行
    size_t cbLen = strlen(buf);
    if (cbLen < (size_t)MIN_CHAR_SIZE) return false;

    return true;
}

static bool GetFieldType(const char *buf, FieldInfo &fieldInfo)
{
    bool ret(false);
    if (!strnicmp(buf, "INT", 3))
    {
        fieldInfo.eType = TYPE_INT;
        fieldInfo.length = atoi(&buf[3])/8;
        ret = true;
    }
    else if (!strnicmp(buf, "UINT", 4))
    {
        fieldInfo.eType = TYPE_UINT;
        fieldInfo.length = atoi(&buf[4])/8;
        ret = true;
    }
    else if (!strnicmp(buf, "NUMERIC", 7))
    {
        fieldInfo.eType = TYPE_DECIMAL;
        fieldInfo.length = sizeof(double);   //默认为float
        const char *pc = strchr(buf, ',');
        if (NULL != pc)
        {
            pc++;
            fieldInfo.precision = (uint8_t)atoi(pc);
        }
        ret = true;
    }
    else if (!strnicmp(buf, "TIME", 4))
    {
        fieldInfo.eType = TYPE_TIME;
        fieldInfo.length = sizeof(time_t);
        ret = true;
    }
    else if (!strnicmp(buf, "DATETIME", 8))
    {
        fieldInfo.eType = TYPE_DATETIME;
        fieldInfo.length = sizeof(time_t);
        ret = true;
    }
    else if (!strnicmp(buf, "DATE", 4))
    {
        fieldInfo.eType = TYPE_DATE;
        fieldInfo.length = sizeof(time_t);
        ret = true;
    }
    else if (!strnicmp(buf, "CHAR", 4))
    {
        //包含 '\0'
        fieldInfo.eType = TYPE_STRING;
        const char *pc = strchr(buf, '(');
        if (NULL != pc)
        {
            char *pc2 = (char*)pc;
            ++pc2;
            fieldInfo.length = atoi(pc2)+1;
            ret = true;
        }
    }
    else if (!strnicmp(buf, "BINARY", 6))
    {
        fieldInfo.eType = TYPE_BINARY;
        const char *pc = strchr(buf, '(');
        if (NULL != pc)
        {
            char *pc2 = (char*)pc;
            ++pc2;
            fieldInfo.length = atoi(pc2);
            ret = true;
        }
    }

    return ret;
}

static bool GetFieldFlag(const char *buf, FieldInfo &fieldInfo)
{
    char seps[] = ",";
    char *token = NULL;
    char *next_token = NULL;
    char text[128] = "";
    safe_strcpy(text, sizeof(text), buf);
    token = strtok_r(text, seps, &next_token);
    bool bValid(true);
    while(token != NULL)
    {
        if (!stricmp(token, "KEY"))  fieldInfo.flag |= FLAG_KEY;
        else if (!stricmp(token, "R")) fieldInfo.flag |= FLAG_READ;
        else if (!stricmp(token, "RW")) fieldInfo.flag |= (FLAG_WRITE | FLAG_READ);
        else
        {
            bValid = false;
            break;
        }
        token = strtok_r(NULL, seps, &next_token);
    }
    
    if (!bValid ) return false;
    return true;
}

static bool DecodeField(char *buf, FieldInfo &fieldInfo)
{
    char *token = NULL;
    char seps[] = " \t\n\r";
    char *next_token = NULL;
    token = strtok_r(buf, seps, &next_token);
    int nIndex(0);

    memset(&fieldInfo, 0, sizeof(fieldInfo));
    while(token != NULL)
    {
        bool bValid(false);
        switch(nIndex)
        {
        case 0:
            if (safe_strcpy(fieldInfo.name, sizeof(fieldInfo.name), token) >0)
            {
                bValid = true;
            }
            break;
        case 1:
            bValid = GetFieldType(token, fieldInfo);
            break;
        case 2:
            bValid = GetFieldFlag(token, fieldInfo);
            break;
        default:
            break;
        }
        if (!bValid) break;
        ++nIndex;
        token = strtok_r(NULL, seps, &next_token);
    }
    if (nIndex<3) return false;

    return true;
}

static bool Table2Fields(const char *pszTable, 
    CFieldVector &fieldSet,
    CFieldMap &fieldMap, 
    char *pszUniqueName, size_t cbName, //全局唯一名称，用于索引指示之用
    char *&pszSelect, char *&pszInsert, char *&pszUpdate, char *&pszDelete)
{
    static const int MIN_SQLSTATEMENT_SIZE = 10;

    std::ifstream ifile;
    // 获取绝对路径
    char szPath[256] = "";
	GetFullPath(szPath, sizeof(szPath), pszTable);
    

    ifile.open(szPath, std::ios_base::in);
    if (!ifile.is_open())
    {
        return false;
    }

    //逐行处理
    bool bValid(true);
    uint8_t nFieldIndex(0);
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

        //去除无效行
        if (!IsValidLine(buf))
        {
            continue;
        }

        //判断是否为SQL语句
#define ALLOC_COPY_SQL(sql_type, source, target) (!strnicmp(source, sql_type, strlen(sql_type))){ \
                                                    size_t nOffset = strlen(sql_type); \
                                                    size_t nSqlLen = strlen(&source[nOffset]);\
                                                    target = NULL;\
                                                    if (nSqlLen > (size_t)MIN_SQLSTATEMENT_SIZE) {\
                                                        target = (char*)calloc(sizeof(char), nSqlLen+10); \
                                                        safe_strcpy(target, nSqlLen+10, &source[nOffset]); }\
                                                    continue; }
        if (!strnicmp(buf, "[UNIQUE]", 8)){
            safe_strcpy(pszUniqueName, cbName, (const char*)&buf[8]);
            continue;
        }
        else if ALLOC_COPY_SQL("[SQL:SELECT]", buf, pszSelect)
        else if ALLOC_COPY_SQL("[SQL:INSERT]", buf, pszInsert)
        else if ALLOC_COPY_SQL("[SQL:UPDATE]", buf, pszUpdate)
        else if ALLOC_COPY_SQL("[SQL:DELETE]", buf, pszDelete)

        //解析字段内容
        FieldInfo fieldInfo = {0};
        //解析命令行
        if (DecodeField(buf, fieldInfo))
        {
            fieldInfo.index = nFieldIndex++;
            CField *pField = NewField(fieldInfo);
            fieldSet.push_back(pField);
            fieldMap.insert(std::make_pair(pField->GetName(), pField));
        }
        else
        {
            bValid = false;
            break;
        }
    }

    ifile.close();
    if (!bValid || 0 == fieldSet.size()) return false;
    return true;
}

static void DestroyFieldSet(CFieldVector &fieldSet,
    CFieldMap &fieldMap)
{
    fieldMap.clear();
    CFieldVector::iterator it(fieldSet.begin());
    for(; it != fieldSet.end(); ++it)
    {
        CField *pField = *it;
        SAFE_RELEASE(pField);
    }
    fieldSet.clear();
}

