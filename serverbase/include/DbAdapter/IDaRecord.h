/*
 * =====================================================================================
 *
 *       Filename:  IMyRecord.h
 *
 *    Description:  内存记录
 *
 *        Version:  1.0
 *        Created:  2013-03-27
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DARECORD_INTERFACE_
#define _GFX_SERVER_DARECORD_INTERFACE_

#include "DaData.h"
namespace my_data{

enum RECORD_FLAG
{
    OP_DUMMY,       //初始状态
    OP_INSERT,      //插入新记录状态
    OP_UPDATE,      //更新记录状态
    OP_DELETE,      //记录被删除状态
};

class IDaRecord
{
public:
    virtual bool IsValid(void) const = 0;
    virtual bool Delete(void) = 0;
    virtual bool Flush(void) = 0;

    virtual size_t FieldCount(void) const = 0;
    virtual CMyData* operator[](int index) = 0;
    virtual const CMyData* operator[](int index) = 0;
    virtual CMyData*        Field(int nIndex) = 0;
    virtual const CMyData*  Field(int nIndex) = 0;

    virtual void release(void) = 0;

protected:
    IDaRecord()             { }
    virtual ~IDaRecord()    { }
};

}; //namespace my_data

#endif  //_GFX_SERVER_DARECORD_INTERFACE_

