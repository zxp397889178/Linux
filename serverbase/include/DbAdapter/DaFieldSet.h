/*
 * =====================================================================================
 *
 *       Filename:  DaFieldSet
 *
 *    Description:  set of CField
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

#ifndef _GFX_SERVER_DA_FIELD_SET_H_
#define _GFX_SERVER_DA_FIELD_SET_H_

#include "DaField.hpp"

namespace my_data{

#define UNIQUE_NAME_MAXLEN      16

class CFieldVector;
class CFieldMap;
class CFieldTable;
class CFieldSet
{
    friend class CFieldTable;
public:
    void release(void) { delete this; }

    bool IsValid(void);

    //获取字段信息
    CField* operator[](int index);
    CField* Field(int nIndex);
    CField* Field(const char* pszName);
    CField* GetKeyField(void);

    //获取全部字段的长度总和
    size_t GetLength(void);
    //获取字段总数
    size_t GetCount(void);
    //获得唯一性名称，由用户在.tbl文件中定义
    const char* GetUniqueName(void) const { return m_szUniqueName; }

    inline const char* GetSelectSql(void) const { return m_pszSelect; }
    inline const char* GetUpdateSql(void) const { return m_pszUpdate; }
    inline const char* GetInsertSql(void) const { return m_pszInsert; }
    inline const char* GetDeleteSql(void) const { return m_pszDelete; }

private:
    explicit CFieldSet(const char* pszTable);
    ~CFieldSet();

private:
    CFieldVector*           m_pFieldVec;    //数据集合，真实存储字段的对象
    CFieldMap*              m_pFieldMap;    //字段名与字段的映射关系，仅映射表

    char                    m_szUniqueName[17];
    //以下为SQL语句
    char*                   m_pszSelect;
    char*                   m_pszInsert;
    char*                   m_pszUpdate;
    char*                   m_pszDelete;
};

} //namespace my_data


#endif  //_GFX_SERVER_DA_FIELD_SET_H_
