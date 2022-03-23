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

    	// ��ȡ�ֶ���󳤶�
    	uint32_t nFieldLen(m_pField->GetLength());
    	if (cbLen > nFieldLen && m_pField->IsStringField())
    	{
    	    // �ַ�����ֵ �����ֶγ��� ����ʧ��
    	    return false;
    	}

    	bool ret(false);
    	if (0 == cbLen)
    	{
    	    // �ֶ�����, ע�����ڲ���״̬dataOpInsertʱ����������
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
    	    case dataOpInsert:  //�����״̬
    	        break;
    	    default:
    	        m_opFlag = dataOpUpdate;
    	        break;
    	    }
    	    
    	    // �ֶθ��£� ע�����ڲ���״̬dataOpInsertʱ�����ɸ���
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
    byte_t              m_buff[32];     //�����ݳ�����32���ڣ���ʹ�����õĻ�����
    byte_t*             m_pData;        //������ָ��
    uint16_t            m_nDataLen;     //����ʵ�ʳ��ȣ�������'\0'
    uint8_t             m_bAllocBuff;   //�Ƿ��з����ڴ�
    uint8_t             m_opFlag:7;     //���ݵĲ�������
    uint8_t             m_bReadOnly:1;  //�Ƿ�ֻ��
    //enum MyDataOpFlag   m_opFlag;       //���ݵĲ�������
    IMyField*           m_pField;       //��Ӧ���ֶ���Ϣ
};


#endif  //_GFX_MYDB_MYDATA_H_

