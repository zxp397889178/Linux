/*
 * =====================================================================================
 *
 *       Filename:  CField.hpp
 *
 *    Description:  字段类
 *
 *        Version:  1.0
 *        Created:  2014-03-26
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DAFIELD_INTERFACE_
#define _GFX_SERVER_DAFIELD_INTERFACE_

#include "BaseCode.h"

namespace my_data{

enum FIELD_TYPE
{
    TYPE_INT,               //整型
    TYPE_UINT,              //无符号整型
    TYPE_DECIMAL,           //数值型，有小数
    TYPE_DATE,              //日期,以time_t格式存储
    TYPE_TIME,              //时间,以time_t格式存储
    TYPE_DATETIME,          //时间戳,以time_t格式存储
    TYPE_STRING,            //字符串
    TYPE_BINARY,            //二进制
};

enum FIELD_FLAG_DEFINE
{
    FLAG_KEY    = 1,        //索引
    FLAG_READ   = 2,        //读属性
    FLAG_WRITE  = 4,        //写属性
};

struct FieldInfo
{
    uint8_t             index;          //字段索引
    char                name[65];       //字段名: mysql 字段名长度<=64
    uint32_t            length;         //长度
    uint8_t             precision;      //numeric类型的小数位
    enum FIELD_TYPE     eType;          //字段类型, 参见 FIELD_TYPE
    uint32_t            flag;           //参见 FIELD_FLAG_DEFINE
};

class CField
{
    friend CField* NewField(FieldInfo &fieldInfo);
public:
    void release() { delete this; }

public:
    int             GetIndex(void) const     { return m_fieldInfo.index; }
    const char*     GetName(void) const      { return m_fieldInfo.name; }
    enum FIELD_TYPE GetType(void) const      { return m_fieldInfo.eType; }
    uint32_t        GetLength(void) const    { return m_fieldInfo.length; }
    uint8_t         GetPrecision(void) const { return m_fieldInfo.precision; }

    //判断是否为整型字段
    bool IsNumberField() const      { return (this->GetType()==TYPE_INT); }
    //判断是否为无符号整型字段
    bool IsUnsignedField() const    { return (this->GetType()==TYPE_UINT); }
    //判断是否为带小数的字段，若是，则GetPrecision有效
    bool IsNumericField() const     { return (this->GetType()==TYPE_DECIMAL); }
    //判断是否字符串字段
    bool IsStringField() const      { return (this->GetType()==TYPE_STRING); }
    //判断是否二进制字段
    bool IsBinaryField() const      { return (this->GetType()==TYPE_BINARY); }
    //判断是否为日期格式，日期格式内部存储为uint32 (yyyy-mm-dd)
    bool IsDateField() const        { return (this->GetType()==TYPE_DATE); }
    //判断是否为时间格式，时间格式内部存储为uint32 (HH:MM:SS)
    bool IsTimeField() const        { return (this->GetType()==TYPE_DATE); }
    //判断是否为时间戳格式，时间戳格式内部存储为uint64 (yyyy-mm-dd HH:MM:SS)
    bool IsDateTimeField() const    { return (this->GetType()==TYPE_DATETIME); }

    //判断是否为索引字段
    bool IsKeyField() const         { return HAS_PROPERTY(FLAG_KEY, m_fieldInfo.flag); }
    //判断字段是否可写，缺省是可读的
    bool CanWrite() const           { return HAS_PROPERTY(FLAG_WRITE, m_fieldInfo.flag); }

private:
    explicit CField(FieldInfo &fieldInfo)
    {
        memcpy(&m_fieldInfo, &fieldInfo, sizeof(FieldInfo));
    }
    ~CField() { }

private:
    FieldInfo   m_fieldInfo;
};

} //namespace my_data

#endif  //_GFX_SERVER_DAFIELD_INTERFACE_

