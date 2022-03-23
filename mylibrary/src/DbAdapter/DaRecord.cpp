#include "DaRecord.h"
#include "DaFieldTable.h"
#include "DaRecordSet.h"
#include "IDataOperator.h"

namespace my_data {
//class CDataVector : public  std::vector< CData* >
//{
//};

//逐行数据添加
#define DISTRIBUTE_RECORD   { byte_t* ptr = m_pData; \
                              for(size_t index=0; index<m_pFieldset->GetCount(); ++index){ \
                                  CField *pField = m_pFieldset->Field(index);\
                                  CData *pData = new CData(pField, ptr, pField->GetLength(), this );\
                                  m_vecData.push_back(pData);\
                                  ptr += pField->GetLength();} \
                            }

//清除数据记录
#define DESTROY_RECORD_DATA { for(std::vector<CData*>::iterator it(m_vecData.begin()); it != m_vecData.end(); ++it) { \
                                  CData *pData = *it;\
                                  SAFE_DELETE(pData); } \
                              m_vecData.clear();\
                            }
}   //namespace my_data

using namespace my_data;
static int FormatSQL(CFieldSet *pFieldset, RECORD_FLAG eFlag, 
    CRecord *pRecord, CMyString &strSQL);


//////////////////////////////////////////////////////////////////////////
CRecord::CRecord(const char *pszPath, const char *pszTable)
    : m_pData(NULL)
	, m_vecData()  
    //, m_pDataVec(new CDataVector() )
    , m_pFieldset(NULL)
	, m_pDataOperator(NULL)	 
    , m_flag(OP_DUMMY)  //设为空白状态，若是新增记录，由SetInsertFlag来改变
    , m_pRecordSet(NULL)
	, m_strPath(pszPath)
	, m_strSelectSQL()	 
{
    //DB_ASSERT(m_pDataVec);
    m_pFieldset = FIELD_TABLE.LoadTable(pszPath, pszTable);
    DB_ASSERT(m_pFieldset);
    size_t cbLength = m_pFieldset->GetLength();
    m_pData = (byte_t*)calloc(sizeof(byte_t), cbLength+1);
    memset(m_pData, 0, cbLength+1);

    DISTRIBUTE_RECORD
}

CRecord::CRecord(CFieldSet *pFieldSet)
    : m_pData(NULL)
	, m_vecData()  
    //, m_pDataVec(new CDataVector() )
    , m_pFieldset(pFieldSet)
	, m_pDataOperator(NULL)	 
    , m_flag(OP_DUMMY)  //设为空白状态，若是新增记录，由SetInsertFlag来改变
    , m_pRecordSet(NULL)
	, m_strPath()
	, m_strSelectSQL()	 
{
    //DB_ASSERT(m_pDataVec);
    DB_ASSERT(m_pFieldset);
    size_t cbLength = m_pFieldset->GetLength();
    m_pData = (byte_t*)calloc(sizeof(byte_t), cbLength+1);
    memset(m_pData, 0, cbLength+1);

    DISTRIBUTE_RECORD
}

CRecord::CRecord(CFieldSet *pFieldSet, CRecordSet *pOnwer)
    : m_pData(NULL)
	, m_vecData()  
    //, m_pDataVec(new CDataVector() )
    , m_pFieldset(pFieldSet)
	, m_pDataOperator(NULL)	 
    , m_flag(OP_DUMMY)  //设为空白状态，若是新增记录，由SetInsertFlag来改变
    , m_pRecordSet(pOnwer)
	, m_strPath()
	, m_strSelectSQL()	 
{
    //DB_ASSERT(m_pDataVec);
    DB_ASSERT(m_pFieldset);
    size_t cbLength = m_pFieldset->GetLength();
    m_pData = (byte_t*)calloc(sizeof(byte_t), cbLength+1);
    memset(m_pData, 0, cbLength+1);

    DISTRIBUTE_RECORD
}

CRecord::CRecord(const char *pszPath, const char *pszTable, CRecordSet *pOnwer)
    : m_pData(NULL)
	, m_vecData()  
    //, m_pDataVec(new CDataVector() )
    , m_pFieldset(NULL)
	, m_pDataOperator(NULL)	 
    , m_flag(OP_DUMMY)  //设为空白状态，若是新增记录，由SetInsertFlag来改变
    , m_pRecordSet(pOnwer)
	, m_strPath(pszPath)  
	, m_strSelectSQL()	 
{
    //DB_ASSERT(m_pDataVec);
    m_pFieldset = FIELD_TABLE.LoadTable(pszPath, pszTable);
    DB_ASSERT(m_pFieldset);
    size_t cbLength = m_pFieldset->GetLength();
    m_pData = (byte_t*)calloc(sizeof(byte_t), cbLength+1);
    memset(m_pData, 0, cbLength+1);

    DISTRIBUTE_RECORD
}

CRecord::~CRecord()
{
    //std::vector<CData*>::iterator it(m_pDataVec->begin());
    //for(; it != m_pDataVec->end(); ++it)
    //{
    //    CData *pData = *it;
    //    SAFE_DELETE(pData);
    //}
    //m_pDataVec->clear();
	
    free(m_pData);
    DESTROY_RECORD_DATA
    //SAFE_DELETE(m_pDataVec);
}

void CRecord::release()
{
    delete this;
}

int CRecord::Init(byte_t *pData, size_t cbData, bool bPackData/* = false*/)
{
    if (bPackData)
    {
        int nStepLen = this->InitPackData(pData, cbData);
		if (nStepLen != -1)
		{
			m_flag = my_data::OP_DUMMY;
		}
        return nStepLen;
    }

    //长度不匹配，认为是非法
    size_t cbLength = m_pFieldset->GetLength();
    if (cbData < cbLength)
    {
        return -1;
    }
    memcpy(m_pData, pData, cbLength);

    //不需要再对CData中的数据进行处理

	m_flag = my_data::OP_DUMMY;
    return (int)cbLength;
}

int CRecord::InitPackData(byte_t *pData, size_t cbData)
{
    UNUSED(cbData);
    size_t cbLength = m_pFieldset->GetLength();
    memset(m_pData, 0, cbLength+1);

    byte_t *ptr = pData;
    byte_t *ptr2 = m_pData;
    int nStepLen(0);    //使用了pData的长度
    for(size_t index=0; index<m_pFieldset->GetCount(); ++index)
    {
        CField *pField = m_pFieldset->Field(index);
        //仅对 STRING 以及 BINARY 方式有压缩，压缩的格式为 nnxxxx... 
        // 其中 nn  为压缩的大小，二字节
        //      xxx 为数据
        //CData *pData = m_vecData.at(index);
        //DB_ASSERT(pData);
        if (pField->IsBinaryField() || pField->IsStringField())
        {
            uint16_t nLen(0);
            nLen = *(uint16_t*)ptr;     //获取长度
            ptr += sizeof(uint16_t);
            memcpy(ptr2, ptr, nLen);            //获取数据内容
            ptr += nLen;
            nStepLen += (nLen+sizeof(uint16_t));
        }
        else
        {
            size_t nLength = pField->GetLength();
            memcpy(ptr2, ptr, nLength);
            ptr += nLength;
            nStepLen += nLength;
        }
        ptr2 += pField->GetLength();    //数据指针移动
        if (nStepLen > (int)cbData)
        {
            return -1;
        }
    }

    return nStepLen;
}

int CRecord::FieldIndex(const char *pszName)
{
    CField *pField = m_pFieldset->Field(pszName);
    if (NULL == pField)
    {
        return -1;
    }
    return pField->GetIndex();
}

CData* CRecord::operator[](int nIndex)
{
    return this->Field(nIndex);
}

CData* CRecord::Field(int nIndex)
{
    if (nIndex<0 || nIndex>=(int)m_vecData.size())
    {
        return NULL;
    }
    return m_vecData.at(nIndex);
}

CData* CRecord::Field(const char *pszName)
{
    int nIndex = this->FieldIndex(pszName);
    return this->Field(nIndex);
}

size_t CRecord::FieldCount(void)
{
	return m_pFieldset->GetCount();
}

void CRecord::Dump(void)
{
	for(size_t idx(0); idx<this->FieldCount(); ++idx)
	{
		CData *pData = this->Field(idx);
		CField *pField = m_pFieldset->Field(idx);
		switch(pField->GetType())
		{
		case TYPE_INT:
			printf("%d", pData->AsInt32());
			break;		
		case TYPE_UINT:
			printf("%u", pData->AsUint32());
			break;
		case TYPE_DECIMAL:
			printf("%.2lf", pData->AsDouble());
			break;
		case TYPE_DATE:
		case TYPE_TIME:
		case TYPE_DATETIME:
			printf("%s", getGMTDate(pData->AsTime()).c_str() );
			break;
		case TYPE_BINARY:
		case TYPE_STRING:
		default:
			printf("%s", pData->AsString());
			break;
		}
		if (idx != this->FieldCount())
			printf("|");
	}
	printf("\n");
}


void CRecord::Update()
{
    switch(m_flag)
    {
    case OP_DELETE:
        //flag 不变化
        break;
    case OP_INSERT:
        //flag 不变化
        break;
    case OP_UPDATE:
        //flag 不变化
        break;
    case OP_DUMMY:
		//LogWarn("=================set update status.");
        m_flag = OP_UPDATE;
        break;
    }
}

void CRecord::Delete()
{
    if (NULL == m_pRecordSet)
    {
        //单条记录存在，不允许要Record中调用删除指令
        DB_ASSERT(0);
        return;
    }

    if (m_flag == OP_INSERT)
    {
        //新增记录，可以直接删除
        m_pRecordSet->Delete(this);
    }
    else
    {
        //增加删除标识
        m_flag = OP_DELETE;
    }
}

void CRecord::SetInsertFlag()
{
    if (m_flag == OP_DUMMY)
    {
        m_flag = OP_INSERT;
    }
    else
    {
        DB_ASSERT(0);
    }
}

void CRecord::ClearFlag(void)
{
    m_flag = OP_DUMMY;
}


CData* CRecord::GetKey()
{
    CField *pField = m_pFieldset->GetKeyField();
    if (NULL == pField)
    {
        return NULL;
    }
    int nIndex = pField->GetIndex();
    return this->Field(nIndex);
}

const char* CRecord::GetUniqueName() const
{
    return m_pFieldset->GetUniqueName();
}

int CRecord::PackData(byte_t *pData, size_t cbData, bool bTitle /* = false */, bool bPacked /* = false */)
{
    byte_t *pc = pData;
    size_t nTotalSize(0);
    int nLeft = (int)cbData;

    //状态
    switch(m_flag)
    {
    case OP_INSERT:
        pc[0] = 'I';
        break;
    case OP_UPDATE:
        pc[0] = 'U';
        break;
    case OP_DELETE:
        pc[0] = 'D';
        break;
    default:
        pc[0] = 'N';
        break;
    }
    pc += sizeof(char);
    nTotalSize += sizeof(char);
    nLeft -= sizeof(char);

    //是否需要加标题
    if (bTitle)
    {
        //判断是否符合最小空间要求
        if (cbData < UNIQUE_NAME_MAXLEN + sizeof(char))
        {
            DB_ASSERT(0);
            return -1;
        }

        safe_strcpy((char*)pc, UNIQUE_NAME_MAXLEN, this->GetUniqueName());
        pc += UNIQUE_NAME_MAXLEN;
        nTotalSize += UNIQUE_NAME_MAXLEN;
        nLeft -= UNIQUE_NAME_MAXLEN;
    }

    //未压缩状态，直接填充数据
    if (!bPacked)
    {
        size_t nLength = m_pFieldset->GetLength();
        if (nLeft < (int)nLength)
        {
            return -1;
        }
        memcpy(pc, m_pData, nLength);
        nTotalSize += nLength;
        return nTotalSize;
    }

    bool bOverSize(false);
    //按压缩包方式进行打包
    for(size_t index=0; index<m_pFieldset->GetCount(); ++index)
    {
        CField *pField = m_pFieldset->Field(index);
        DB_ASSERT(pField);
        CData *pData = m_vecData.at(index);
        DB_ASSERT(pData);
        switch(pField->GetType())
        {
        case TYPE_BINARY:
        case TYPE_STRING:
            {
                size_t nLen = __min(pData->GetLength(), pData->GetMaxLength()-sizeof(uint16_t));
                if (nLeft < (int)(nLen + sizeof(uint16_t)))
                {
                    bOverSize = true;
                    break;
                }
                *(uint16_t*)pc = (uint16_t)pData->GetLength();
                pc += sizeof(uint16_t);
                memcpy(pc, pData->AsBinary(), nLen);
                pc += nLen;
                nTotalSize += (nLen + sizeof(uint16_t));
                nLeft -= (nLen + sizeof(uint16_t));
            }
            break;
        default:
            {
                size_t nLen = pData->GetMaxLength();
                if (nLeft < (int)nLen)
                {
                    bOverSize = true;
                    break;
                }
                memcpy(pc, pData->AsBinary(), nLen);
                pc += nLen;
                nTotalSize += nLen;
                nLeft -= nLen;
            }
            break;
        }
        if (bOverSize)
        {
            return -1;
        }
    }

    return nTotalSize;
}

int CRecord::UpdateRecord(CRecord *pRecord)
{
    if (NULL == pRecord)
    {
        return -1;
    }
    size_t cbLength = pRecord->GetLength();
    size_t cbLength2 = m_pFieldset->GetLength();
    DB_ASSERT(cbLength == cbLength2);
    if (cbLength != cbLength2)
    {
        return -1;
    }
    memcpy(m_pData, pRecord->m_pData, cbLength);
    return 0;
}

size_t CRecord::GetLength() const
{
    return m_pFieldset->GetLength();
}

void CRecord::Flush(CMyString &strSql)
{
    int ret = FormatSQL(m_pFieldset, m_flag, this, strSql);
    DB_ASSERT(ret>0);
}

int CRecord::Flush(byte_t *pData, size_t cbData)
{
    int ret = this->PackData(pData, cbData, ((m_pRecordSet!=NULL)? false : true), false);
    return ret;
}

//////////////////////////////////////////////////////////////////////////
//构建SQL语句
void ReplaceAll(CMyString &strSource, CMyString &strSub, CMyString &strVal)
{
    CMyString::size_type pos = strSource.find(strSub.c_str());
    size_t len = strSub.length();
    while(pos != CMyString::npos)
    {
        strSource.replace(pos, len, strVal.c_str());
        pos = strSource.find(strSub.c_str());
    }
}

static int FormatSQL(CFieldSet *pFieldset, RECORD_FLAG eFlag, 
    CRecord *pRecord, CMyString &strSQL)
{
    DB_ASSERT(pFieldset);
    DB_ASSERT(pRecord);

    CMyString strFormat;
    switch(eFlag)
    {
    case OP_INSERT:
        strFormat = pFieldset->GetInsertSql();
        break;
    case OP_UPDATE:
        strFormat = pFieldset->GetUpdateSql();
        break;
    case OP_DELETE:
        strFormat = pFieldset->GetDeleteSql();
        break;
    default:
        return 0;
        break;
    }
    if (strFormat.empty())
    {
        return 0;
    }

    CMyString strPlaceHolder;
    CMyString strValue;
    int val(0);
    size_t nFieldCount = pFieldset->GetCount();
    //必须逆序进行替换，否则会出现误操作
    for(int index=(int)(nFieldCount-1); index>=0; --index)
    {
        strPlaceHolder.Format("[:%d]", index);
        if (!strFormat.Find(strPlaceHolder.c_str(), false))
        {
            continue;
        }

        //转换成对应的值
        CField *pField = pFieldset->Field(index);
        CData *pData = pRecord->Field(index);
        DB_ASSERT(pField);
        DB_ASSERT(pData);
        switch(pField->GetType())
        {
        case TYPE_INT:
            if (pField->GetLength()==8)
            {
                //strValue.Format("%I64d", pData->AsInt64());
                strValue.Format("%ld", pData->AsInt64());
            }
            else
            {
                strValue.Format("%d", pData->AsInt32());
            }
            break;
        case TYPE_UINT:
            if (pField->GetLength() == 8)
            {
                //strValue.Format("%I64u", pData->AsUint64());
                strValue.Format("%lu", pData->AsUint64());
            }
            else
            {
                strValue.Format("%u", pData->AsUint32());
            }
            break;
        case TYPE_DECIMAL:
            {
                CMyString strTempFormat("%%10.%dlf", pField->GetPrecision());
                strValue.Format(strTempFormat.c_str(), pData->AsDouble());
            }
            break;
        case TYPE_DATETIME:
            {
                time_t tv = pData->AsTime();
                char szTime[64] = "";
                TimeToStr(tv, szTime, sizeof(szTime), "'yyyy-mm-dd HH:MM:SS'");
                strValue = szTime;
            }
            break;
        case TYPE_DATE:
            {
                time_t tv = pData->AsTime();
                char szTime[64] = "";
                TimeToStr(tv, szTime, sizeof(szTime), "'yyyy-mm-dd'");
                strValue = szTime;
            }
            break;
        case TYPE_TIME:
            {
                time_t tv = pData->AsTime();
                char szTime[64] = "";
                TimeToStr(tv, szTime, sizeof(szTime), "'HH:MM:SS'");
                strValue = szTime;
            }
        case TYPE_STRING:
            strValue.Format("'%s'", pData->AsString());
            break;
        case TYPE_BINARY:
            pData->GetBinaryStr(strValue);
            break;
        }

        if (!strValue.empty())
        {
            ReplaceAll(strFormat, strPlaceHolder, strValue);
        }
    }

    val = strFormat.length();
    strSQL.append(strFormat);
    return val;
}

bool CRecord::Reload(bool ignoreChange)
{
	if (m_pDataOperator == NULL)
	{
		return false;
	}

	return m_pDataOperator->Reload(this, ignoreChange);
}

ISimDB* CRecord::getDBConn(void)
{
	if (NULL == m_pDataOperator)
	{
		return NULL;
	}

	return m_pDataOperator->getDBConn();
}


