/*
 * =====================================================================================
 *
 *       Filename:  IMyData.h
 *
 *    Description:  数据适配
 *
 *        Version:  1.0
 *        Created:  2014－03－27
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DADATA_H_
#define _GFX_SERVER_DADATA_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <typeinfo>
#include <string.h>
#include <assert.h>
#include "DaField.hpp"
#include "BitArray.h"

namespace my_data{

class CRecord;
class CData
{
    friend class CRecord;
    friend class CBitArray;
    friend inline std::ostream& operator<<(std::ostream &os, CData &data)
    {
        os << data.AsString();
        return os;
    }
    NON_COPYABLE(CData)
public:
    virtual ~CData();
    //virtual void release();

    size_t GetLength(void) const    { return m_cbData; }
    size_t GetMaxLength(void) const { return m_pField->GetLength(); }

public:
    bool Set(byte_t* pData, size_t cbData);
	bool Set(char val);
	bool Set(uint8_t val);
	bool Set(uint16_t val);
	bool Set(uint32_t val);
	bool Set(uint64_t val);
	bool Set(int8_t val);
	bool Set(int16_t val);
	bool Set(int32_t val);
	bool Set(int64_t val);
	bool Set(float val);
	bool Set(double val);
	bool Set(const char *val);

	bool Inc(int32_t val);
	bool Dec(int32_t val);

public:
    const char* AsString(void) const;
    int32_t  AsInt32(void) const;
    int64_t  AsInt64(void) const;
    uint32_t AsUint32(void) const;
    uint64_t AsUint64(void) const;
    byte_t* AsBinary(void) const;
    CBitArray AsBitArray(void);
    double  AsDouble(void)  const;
    time_t  AsTime(void)    const;

    void GetBinaryStr(CMyString &str);
    
//    template<typename T> T GetValue(void) const
//    {
//        //注意空间大小必须保持一致
//		return my_data::Cast<T>(m_pData);
//    }
//
    template<typename T> T Value()    const
    {
        T ret = 0;
        switch (m_pField->GetType())
        {
        case TYPE_INT:
            ret = static_cast<T>(this->AsInt64());
            break;
        case TYPE_UINT:
            ret = static_cast<T>(this->AsUint64());
            break;
        case TYPE_DECIMAL:
            ret = this->AsDouble();
            break;
        case TYPE_DATETIME:
        case TYPE_DATE:
        case TYPE_TIME:
            ret = this->AsTime();
        }
        return ret;
    }

private:
    //CData()
	//	: m_pField(NULL)
	//	, m_pData(NULL)
	//	, m_cbData(0)
	//	, m_pField(NULL) 
	//{
	//}
    CData(CField* pField, byte_t* pData, size_t cbData, CRecord *pOwner);

    bool CanWrite(void);

protected:
    CField* const       m_pField;   //对应的字段信息
    byte_t* const       m_pData;    //数据指针，注意本类内
    size_t              m_cbData;   //数据的实际长度
    CRecord*            m_pRecord;  //父记录
};

//http://bbs.chinaunix.net/thread-1010445-2-1.html
// 但好象不能够用

} //namespace my_data

#endif  //_GFX_SERVER_DADATA_H_

