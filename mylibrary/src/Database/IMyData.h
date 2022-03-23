/*
 * =====================================================================================
 *
 *       Filename:  IMyData.h
 *
 *    Description:  Data Interface 
 *
 *        Version:  1.0
 *        Created:  2015/04/06 20时21分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
*/

#ifndef _GFX_MYDB_MYDATA_INTERFACE_
#define _GFX_MYDB_MYDATA_INTERFACE_

#include <stdint.h>
#include <sys/types.h>
#include <iostream>

enum MyDataOpFlag
{
    dataOpDummy = 0,    //IMyData 未赋过值
    dataOpSelect,       //IMyData 已赋过值
    dataOpInsert,       //插入
    dataOpUpdate,       //更新
    //dataOpDelete,       //删除
};

class IMyData
{
    friend inline std::ostream& operator<<(std::ostream &os, IMyData &data)
    {
        os << data.toString();
        return os;
    }

public:
    virtual void release(void) = 0;
    IMyData& operator=(const char* rhs) { this->setString(rhs); return *this; }
    IMyData& operator=(int32_t val)     { this->setInt(val);   return *this; }
    IMyData& operator=(uint32_t val)    { this->setUInt(val);  return *this; }
    IMyData& operator=(uint64_t val)    { this->setUInt64(val); return *this; }
    IMyData& operator=(float val)       { this->setFloat(val);  return *this; }
    IMyData& operator=(double val)      { this->setDouble(val); return *this; }
    IMyData& operator=(time_t val)      { this->setTime(val);   return *this; }

//读取操作相关
public:
    virtual const char* toString(void) const = 0;
    virtual int32_t     toInt(void) const = 0;
    virtual uint32_t 	toUInt(void) const = 0;
    virtual uint64_t 	toUInt64(void) const = 0;
    virtual int64_t 	toInt64(void) const = 0;
    virtual float 		toFloat(void) const = 0;
    virtual double 		toDouble(void) const = 0;
    virtual time_t 		toTime() const = 0;

//写操作相关
public:
    virtual bool setString(const char *val) = 0;
    virtual bool setInt(int32_t val) = 0;
    virtual bool setUInt(uint32_t val) = 0;
    virtual bool setUInt64(uint64_t val) = 0;
    virtual bool setFloat(float val) = 0;
    virtual bool setDouble(double val) = 0;
    virtual bool setTime(time_t val) = 0;

    virtual void Delete(void) = 0;
    virtual enum MyDataOpFlag getFlag(void) const = 0;
    virtual void setFlag(enum MyDataOpFlag flag = dataOpSelect) = 0;


public:
    virtual size_t getDataLen(void) const = 0;
    virtual size_t getFieldLen(void) const  = 0;

protected:
    IMyData() {}
    virtual ~IMyData() {};
};


#endif  //_GFX_MYDB_MYDATA_INTERFACE_

