#include "Bucket.h"
#include "DaFieldTable.h"
#include <string>
#include <map>
#include <fstream>
#include <iostream>


extern  int CompressData(byte_t *&pDest, size_t &cbDest, const byte_t *pSource, size_t cbSource);
extern  int UncompressData(byte_t *&pDest, size_t &cbDest, const byte_t *pSource, size_t cbSource);

//////////////////////////////////////////////////////////////////////////
namespace my_data{

class CRecordMap : public std::map< std::string, my_data::CRecord* >
{
};

class CRecordsetMap : public std::map< std::string, my_data::CRecordSet* >
{
};

}   //namespace my_data


using namespace my_data;
//////////////////////////////////////////////////////////////////////////
CBucket::CBucket()
    : m_pRecordMap(new CRecordMap() )
    , m_pRecordSetMap(new CRecordsetMap())
    , m_pEvent(NULL)
    , m_ID(0)
    , m_bKeepRecordFlag(false)
{
    DB_ASSERT(m_pRecordMap);
    DB_ASSERT(m_pRecordSetMap);
}

CBucket::~CBucket()
{
    for(CRecordMap::iterator it(m_pRecordMap->begin());
        it != m_pRecordMap->end(); ++it)
    {
        CRecord* pRecord = it->second;
        SAFE_RELEASE(pRecord);
    }
    SAFE_DELETE(m_pRecordMap);

    for(CRecordsetMap::iterator it(m_pRecordSetMap->begin());
        it != m_pRecordSetMap->end(); ++it)
    {
        CRecordSet* pRecordSet = it->second;
        SAFE_DELETE(pRecordSet);
    }
    m_pRecordSetMap->clear();
    SAFE_DELETE(m_pRecordSetMap);
}

bool CBucket::Init(IBucketEvent *pEvent, const char *pszConfig)
{
    // 获取绝对路径
    char szPath[256] = "";
	char szFile[256] = "";
	GetFullPath(szFile, sizeof(szFile), pszConfig);
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
            const char* pszTableName = (const char*)&buf[8];
            CRecord *pRecord = new CRecord(szPath, pszTableName);
            DB_ASSERT(pRecord);
            if (pRecord)
            {
                std::string strName(FIELD_TABLE.GetUniqueName(szPath, pszTableName));
                m_pRecordMap->insert(std::make_pair(strName, pRecord));
            }
        }
        else if (!strnicmp(buf, "[RCDSET]", 8))
        {
            const char* pszTableName = (const char*)&buf[8];
            CRecordSet *pRecordSet = new CRecordSet(szPath, pszTableName);
            DB_ASSERT(pRecordSet);
            if (pRecordSet)
            {
				//LogDebug("path :%s, tablename %s", szPath, pszTableName);
				const char *pName = FIELD_TABLE.GetUniqueName(szPath, pszTableName);
				if (pName == NULL)
				{
					LogFatal2("INIT", 0, "record set path[%s] name[%s] not found", szPath, pszTableName);
					return false;
				}
				std::string strName(pName);
                m_pRecordSetMap->insert(std::make_pair(strName, pRecordSet));
            }
        }
    }

    if (m_pRecordMap->empty() && m_pRecordSetMap->empty())
    {
        //没有装载入任何数据
        return false;
    }

    //分配回调事件
    m_pEvent = pEvent;
    DB_ASSERT(m_pEvent);
    return true;
}

CRecord* CBucket::QueryRecord(const char *pszName)
{
    CRecordMap::iterator it = m_pRecordMap->find(pszName);
    if (it != m_pRecordMap->end())
    {
        return it->second;
    }
    return NULL;
}

CRecordSet* CBucket::QueryRecordSet(const char *pszName)
{
    CRecordsetMap::iterator it = m_pRecordSetMap->find(pszName);
    if (it != m_pRecordSetMap->end())
    {
        return it->second;
    }
    return NULL;
}

int CBucket::PackData(byte_t *pData, size_t cbData, 
    bool bPacked /* = false */, bool bChangedOnly /*= false*/)
{
    byte_t *pc = pData;
    int nLeft = (int)cbData;
    int nTotalSize(0);
    for(CRecordMap::iterator it(m_pRecordMap->begin()); it != m_pRecordMap->end(); ++it)
    {
        CRecord *pRecord = it->second;
        DB_ASSERT(pRecord);
        if (bChangedOnly && !pRecord->IsChanged())
        {
            continue;
        }
        int ret = pRecord->PackData(pc, (size_t)nLeft, true, bPacked);
        if (ret<0)
        {
            return -1;
        }
        pc += ret;
        nLeft -= ret;
        nTotalSize += ret;
    }

    for(CRecordsetMap::iterator it(m_pRecordSetMap->begin()); it != m_pRecordSetMap->end(); ++it)
    {
        CRecordSet *pRecordset = it->second;
        int ret = pRecordset->PackData(pc, (size_t)nLeft, bPacked, bChangedOnly);
        if (ret < 0)
        {
            return -1;
        }
        pc += ret;
        nLeft -= ret;
        nTotalSize += ret;
    }
    return nTotalSize;
}

int CBucket::PackDataEx(byte_t *&pData, size_t &cbData, bool &bCompressed, bool bChangedOnly /* = false */)
{
    static const int BUFF_SIZE = 524288; // 512 * 1024;
    static const int PACKED_MIN_BUFF = 1000;

	//typedef TScopedBuffer<524288> CUserDataBuffer;
	CTempBuffer tmpBuf(BUFF_SIZE);
	byte_t *pBuff = (byte_t*)tmpBuf.data();
    size_t cbBuff = BUFF_SIZE;

    // ret 为未压缩状态下 数据包的总大小
    int ret = this->PackData(pBuff, cbBuff, false, bChangedOnly);
    if (-1 == ret)
    {
        return -1;
    }
    else if (0 == ret)
    {
        return 0;
    }

    if (ret >= PACKED_MIN_BUFF)
    {
        bCompressed = true;
        // pData的内存在CompressData内分配
        int ret2 = CompressData(pData, cbData, pBuff, ret);
        if (-1 == ret2)
        {
            return -1;
        }
        return ret2;
    }
    else
    {
        //字节太小，不予压缩，直接导出pBuff （注意，外部需要自行释放）
        bCompressed = false;
        cbData = ret;
		pData = (byte_t*)calloc(sizeof(byte_t), cbData+8);
		memcpy(pData, pBuff, cbData);
        return ret;
    }
}

int CBucket::UnpackData(byte_t *pData, size_t cbData, bool bPacked /* = false */)
{
    byte_t *pc = pData;
    int nLeft(cbData);

    bool bOverSize(false);
    int ret(0);
    char c;
    do{
        c = (char)pc[0];
        pc += sizeof(char);
        nLeft -= sizeof(char);

        if (c == 'S')
        {
            //记录集
            ret = UnpackRecordSet(pc, nLeft, bPacked);
        }
        else
        {
            //单条记录
            ret = this->UnpackRecord(c, pc, nLeft, bPacked);
        }

        if (ret<0)
        {
            bOverSize = true;
            break;
        }

        pc += ret;
        nLeft -= ret;
    } while(nLeft>0);

    if (bOverSize)
    {
        return -1;
    }

    return 0;
}

int CBucket::UnpackDataEx(byte_t *pData, size_t cbData)
{
    byte_t *pBuff(NULL);
    size_t cbBuff(0);
    int ret = UncompressData(pBuff, cbBuff, pData, cbData);
    if (-1 == ret)
    {
        return -1;
    }
    int ret2 = this->UnpackData(pBuff, cbBuff, false);
    free(pBuff);
    if (-1 == ret2)
    {
        return -1;
    }
    return 0;
}


#define STEP_BUFFER(ptr, left, used, size)  { left-=size;used+=size;ptr+=size; DB_ASSERT(left>=0); }
int CBucket::UnpackRecordSet(byte_t *pData, size_t cbData, 
    bool bPacked /* = false */)
{
    char szUniqueName[UNIQUE_NAME_MAXLEN+1] = "";
    byte_t* pc = pData;
    int nUsedSize(0);
    int nLeft(cbData);

    safe_strcpy(szUniqueName, sizeof(szUniqueName), (const char*)pc);
    STEP_BUFFER(pc, nLeft, nUsedSize, UNIQUE_NAME_MAXLEN);

    uint16_t nCount = *(uint16_t*)pc;
    STEP_BUFFER(pc, nLeft, nUsedSize, sizeof(uint16_t));

    CRecordSet *pRecordSet = this->QueryRecordSet(szUniqueName);
    DB_ASSERT(pRecordSet);
    if (NULL == pRecordSet)
    {
		LogErr("ERR: [%s:%d] %s not found", __FILE__, __LINE__, szUniqueName);
        return -2;
    }
    for(uint16_t index=0; index<nCount; ++index)
    {
        int ret = this->UnpackRecord(pc, nLeft, pRecordSet, bPacked);
        if (ret<0)
        {
            return -1;
        }
        STEP_BUFFER(pc, nLeft, nUsedSize, ret);
    }

    return nUsedSize;
}

int CBucket::UnpackRecord(byte_t *pData, size_t cbData, 
    CRecordSet *pRecordSet, bool bPacked /* = false */)
{
    byte_t *pc = pData;
    char cOp = (char)pc[0];
    pc+=sizeof(char);
    int nUsedSize = sizeof(char);

    CFieldSet *pFieldSet = pRecordSet->GetFieldSet();
    CRecord *pRecord = new CRecord(pFieldSet, pRecordSet);
    int nStepLen = pRecord->Init(pc, cbData-1, bPacked);
    if (-1 == nStepLen)
    {
		LogErr("ERR: [%s:%d] init new Record failure", __FILE__, __LINE__);
        SAFE_RELEASE(pRecord);
        return -1;
    }

    switch(cOp)
    {
    case 'N':   //空白状态，新增记录集
        pRecordSet->AddRecord(pRecord);
        break;
    case 'U':   //修改记录数据，原pRecord失效
        pRecordSet->UpdateRecord(pRecord, (m_bKeepRecordFlag&&(cOp=='U')) );
        //在UpdateRecord内已处理，不需要再做Release调用
        break;
    case 'D':   //删除记录
        {
            CData *pKey = pRecord->GetKey();
            DB_ASSERT(pKey);
            if (NULL != pKey)
            {
                pRecordSet->DeleteRecord(pKey->AsInt32());
            }
            SAFE_RELEASE(pRecord);
        }
        break;
    case 'I':   //增加记录
        pRecordSet->AddRecord(pRecord);
        if (m_bKeepRecordFlag) pRecord->SetInsertFlag();
        break;
    default:
        SAFE_RELEASE(pRecord);
        DB_ASSERT(0);
        break;
    }
    return nUsedSize + nStepLen;
}

int CBucket::UnpackRecord(char cType, byte_t *pData, size_t cbData,
    bool bPacked /* = false */)
{
    char szUniqueName[UNIQUE_NAME_MAXLEN+1] = "";
    byte_t* pc = pData;
    int nUsedSize(0);
    int nLeft(cbData);

    safe_strcpy(szUniqueName, sizeof(szUniqueName), (const char*)pc);
    STEP_BUFFER(pc, nLeft, nUsedSize, UNIQUE_NAME_MAXLEN);

    CRecord *pOldRecord = this->QueryRecord(szUniqueName);
    DB_ASSERT(pOldRecord);
    CFieldSet *pFieldSet = pOldRecord->GetFieldSet();
    DB_ASSERT(pFieldSet);

    CRecord *pRecord = new CRecord(pFieldSet);
    int nStepLen = pRecord->Init(pc, cbData-1, bPacked);
    if (-1 == nStepLen)
    {
        SAFE_RELEASE(pRecord);
        return -1;
    }

    switch(cType)
    {
    case 'U':   //更新装载
    case 'N':   //初始化装载
        DB_ASSERT(pOldRecord);
        pOldRecord->UpdateRecord(pRecord);
        if (m_bKeepRecordFlag && cType=='U') pOldRecord->Update();
        SAFE_RELEASE(pRecord);
        break;
    case 'I':
        DB_ASSERT(pOldRecord);
        m_pRecordMap->insert(std::make_pair(szUniqueName, pRecord));
        if (m_bKeepRecordFlag) pRecord->SetInsertFlag();
        break;
    case 'D':
    default:
        //对于单条的Record，不允许删除
        SAFE_RELEASE(pRecord);
        DB_ASSERT(0);
        break;
    }
    return nUsedSize+nStepLen;
}

int CBucket::Flush(void)
{
    DB_ASSERT(m_pEvent);
    if (!m_pEvent)
    {
        return -1;
    }

    size_t cbData = 0;
    byte_t *pData = NULL;
    bool bCompressed = false;
    int ret = this->PackDataEx(pData, cbData, bCompressed, true);
    if (-1 == ret)
    {
        free(pData);
        return -1;
    }
    if (0 == ret)
    {
        //无记录改变
        free(pData);
        return 0;
    }

    //调用回调函数，用于处理发送数据
    int val = m_pEvent->OnFlush2Serve(m_ID, pData, cbData, bCompressed);
    free(pData);
    DB_ASSERT(val==0);
    if (val < 0)
    {
        //刷写到服务器失败
        return -1;
    }

    //更新记录状态
    this->OnFlushOK();
    return val;
}


int CBucket::Flush2DB(void)
{
    DB_ASSERT(m_pEvent);
    if (!m_pEvent)
    {
        return -1;
    }

    CMyString strSql;

    for(CRecordMap::iterator it(m_pRecordMap->begin()); it != m_pRecordMap->end(); ++it)
    {
        CRecord *pRecord = it->second;
        DB_ASSERT(pRecord);
        if (!pRecord->IsChanged())
        {
            continue;
        }
        pRecord->Flush(strSql);
    }

    for(CRecordsetMap::iterator it(m_pRecordSetMap->begin()); it != m_pRecordSetMap->end(); ++it)
    {
        CRecordSet *pRecordset = it->second;
        DB_ASSERT(pRecordset);
        pRecordset->Flush(strSql);
    }

    if (0 == strSql.length())
    {
        //无改变的记录
        return 0;
    }

    //将结果写入到
    int ret = m_pEvent->OnFlush2DB(m_ID, strSql.c_str(), strSql.length());
    if (ret <0)
    {
        //刷写数据库失败
        return -1;
    }

    //更新记录状态
    this->OnFlushOK();
    return ret;
}

void CBucket::OnFlushOK(void)
{
    for(CRecordMap::iterator it(m_pRecordMap->begin()); it != m_pRecordMap->end(); ++it)
    {
        CRecord *pRecord = it->second;
        DB_ASSERT(pRecord);
        pRecord->ClearFlag();
    }

    for(CRecordsetMap::iterator it(m_pRecordSetMap->begin()); it != m_pRecordSetMap->end(); ++it)
    {
        CRecordSet *pRecordset = it->second;
        DB_ASSERT(pRecordset);
        pRecordset->ClearFlag();
    }
}

void CBucket::BindDataSource(IDataOperator *pDbSource, uint32_t userId)
{
    for(CRecordMap::iterator it(m_pRecordMap->begin()); it != m_pRecordMap->end(); ++it)
    {
        CRecord *pRecord = it->second;
        DB_ASSERT(pRecord);
		pRecord->BindDataOperator(pDbSource);
		
		CFieldSet *pField = pRecord->GetFieldSet();
		if (NULL != pField)
		{
			CMyString strSQL(pField->GetSelectSql(), userId);
			pRecord->SetSelectSQL(strSQL.c_str());
		}
    }

    for(CRecordsetMap::iterator it(m_pRecordSetMap->begin()); it != m_pRecordSetMap->end(); ++it)
    {
        CRecordSet *pRecordset = it->second;
        DB_ASSERT(pRecordset);
		pRecordset->BindDataOperator(pDbSource);

		CFieldSet *pField = pRecordset->GetFieldSet();
		if (NULL != pField)
		{
			CMyString strSQL(pField->GetSelectSql(), userId);
			pRecordset->SetSelectSQL(strSQL.c_str());
		}
    }
}



