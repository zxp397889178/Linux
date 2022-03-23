#ifndef _GFX_MYDB_MYDATA_H_
#define _GFX_MYDB_MYDATA_H_

#include <iostream>
#include <stdint.h>
#include <sys/types.h>
#include "BaseCode.h"
#include "IMyData.h"
#include "IMyField.h"
#include "IMyRecord.h"



class CMyData : public IMyData
{
	NON_COPYABLE(CMyData)
public:
    CMyData();
    explicit CMyData(IMyField* pField);
    CMyData(byte_t* data_ptr, size_t data_len, IMyField *pField);
    ~CMyData();

    void SetReadOnly(bool val) { m_bReadOnly = (uint8_t)val; }
    virtual void release(void);

public:
    virtual const char* toString(void) const;
    virtual int32_t toInt(void) const;
    virtual uint32_t toUInt(void) const;
    virtual uint64_t toUInt64(void) const;
    virtual int64_t toInt64(void) const;
    virtual float toFloat(void) const;
    virtual double toDouble(void) const;
    virtual time_t toTime() const;

public:
    virtual bool setString(const char *val);
    virtual bool setShort(int16_t val);
    virtual bool setInt(int32_t val);
    virtual bool setUInt(uint32_t val);
    virtual bool setUInt64(uint64_t val);
    virtual bool setFloat(float val);
    virtual bool setDouble(double val);
    virtual bool setTime(time_t val);

    virtual void Delete(void)                   { this->setData<const char*>(NULL, 0); }
    virtual enum MyDataOpFlag getFlag(void) const { return (enum MyDataOpFlag)m_opFlag; }
    virtual void setFlag(enum MyDataOpFlag flag = dataOpSelect)  { m_opFlag = flag; }

    virtual size_t getDataLen(void) const       { return m_nDataLen; }
    virtual size_t getFieldLen(void) const      { return m_pField->GetPrecision(); }

protected:
    template<typename T> T toData(void) const
	{
		return *(T*)&m_pData;
	}
	bool setInt64(int64_t val);

protected:
    template<typename T> bool setData(T pData, size_t cbLen)
	{
    	if (m_bReadOnly)
    	{
    	    return false;
    	}

    	if (NULL==m_pField || NULL==m_pData)
    	    return false;

    	// 获取字段最大长度
    	uint32_t nFieldLen(m_pField->GetLength());
    	if (cbLen > nFieldLen && m_pField->IsStringField())
    	{
    	    // 字符串赋值 超出字段长度 返回失败
    	    return false;
    	}

    	bool ret(false);
    	if (0 == cbLen)
    	{
    	    // 字段清零, 注：仅在插入状态dataOpInsert时主键可清零
    	    if (m_pField->IsPrimaryKeyField())
    	        ret = (m_opFlag == (uint8_t)dataOpInsert);
    	    else
    	        ret = true;

    	    if (ret)
    	    {
    	        memset(m_pData, 0, nFieldLen);
    	        //m_opFlag = dataOpDelete;
    	        m_opFlag = dataOpUpdate;
    	        m_nDataLen = 0;

    	        return true;
    	    }
    	    return false;
    	}

    	if (memcmp(m_pData, pData, cbLen) != 0)
    	{
    	    switch(m_opFlag)
    	    {
    	    case dataOpDummy:
    	        m_opFlag = dataOpInsert;
    	        break;
    	    case dataOpInsert:  //不变更状态
    	        break;
    	    default:
    	        m_opFlag = dataOpUpdate;
    	        break;
    	    }
    	    
    	    // 字段更新， 注：仅在插入状态dataOpInsert时主键可更新
    	    if (m_pField->IsPrimaryKeyField())
    	        ret = (m_opFlag == dataOpInsert);
    	    else
    	        ret = true;

    	    if (ret)
    	    {
    	        m_nDataLen = (uint16_t)cbLen;
    	        memset(m_pData, 0, nFieldLen);
    	        memcpy(m_pData, pData, cbLen);
    	    }
    	}
    	return ret;
	}

protected:
    byte_t              m_buff[32];     //若数据长度在32以内，则使用内置的缓冲区
    byte_t*             m_pData;        //缓冲区指针
    uint16_t            m_nDataLen;     //数据实际长度，不包含'\0'
    uint8_t             m_bAllocBuff;   //是否有分配内存
    uint8_t             m_opFlag:7;     //数据的操作类型
    uint8_t             m_bReadOnly:1;  //是否只读
    //enum MyDataOpFlag   m_opFlag;       //数据的操作类型
    IMyField*           m_pField;       //对应的字段信息
};


#endif  //_GFX_MYDB_MYDATA_H_

