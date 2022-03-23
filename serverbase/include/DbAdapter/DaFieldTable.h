/*
 * =====================================================================================
 *
 *       Filename:  DaFieldTable.h
 *
 *    Description:  CFieldSet 的管理器
 *
 *        Version:  1.0
 *        Created:  2014-3-27
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DA_FIELD_TABLE_H_
#define _GFX_SERVER_DA_FIELD_TABLE_H_

#include "DaFieldSet.h"

namespace my_data{

class CFieldSetMap;
class CFieldTable
{
public:
    ~CFieldTable();
    static CFieldTable& Instance();

    //装载表结构的定义,若已存在，则返回字段集指针
    CFieldSet* LoadTable(const char *pszPath, const char *pszTable);

    //获得表对应的的唯一性名称
    const char* GetUniqueName(const char *pszPath, const char *pszTable);
    //通过唯一性名称查找对应的CFieldSet
    CFieldSet* FindByUniqueName(const char *pszUniqueName);

private:
    CFieldTable();
    void Destroy();

    CFieldSet* Find(const char *pszTable);
    
private:
    CFieldSetMap*   m_pFieldsetMap;         //字段集合 与 表名 的映射关系表
    CFieldSetMap*   m_pUniqueNameMap;       //字段集合 与 表中的唯一名 的映射关系表
};

} //namespace my_data

#define FIELD_TABLE      my_data::CFieldTable::Instance()

#endif  //_GFX_SERVER_DA_FIELD_TABLE_H_

