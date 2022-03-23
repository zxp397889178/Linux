/*
 * =====================================================================================
 *
 *       Filename:  DbItem.h
 *
 *    Description:  数据库配置信息
 *
 *        Version:  1.0
 *        Created:  2021年08月07日 16时01分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "BaseCode.h"
#include "MyDB.h"

enum DB_TYPE 
{ 
    db_Dummy = 0,
    db_Oracle,        //oracle, 暂不支持
    db_Mysql,         //mysql
    db_Sqlite,        //sqlite, 暂不支持
    db_Redis          //redis
};

struct DbItem
{
    CMyString       name;    // 索引2
    DB_TYPE         dbType;
    CMyString       host;
    int32_t         port;
    CMyString       db;
    CMyString       user;
    CMyString       passwd;

    DbItem() : dbType(db_Dummy), port(0) 
    {
    }
};

enum DB_TYPE Str2Dbtype(const CMyString &str);
CMyString Dbtype2Str(enum DB_TYPE eType);

class CDbItemList
{
    typedef std::vector<DbItem>     VEC_DBINFO;
public:
    CDbItemList();
    ~CDbItemList();

    /* 装载数据库配置信息，
     * 返回值：<0 出错
     *         =0 没有指定数据库连接信息
     *         >0 数据库连接的数量
     */
    int32_t Init(CXmlFile &xml);

    bool getDbItem(const CMyString &name, DbItem &dbInfo);

    /* 获得数据库连接
     * 参数： name      => config.xml中配置的name
     *        pDB       => 数据库连接指针
     *        appName   => 本应用指定的appName, 用于在DBMS中查询,可以为空
     * 返回值：true     => 连接成功
     *         false    => 连接失败,请查看日志
     */
    bool getMysql(const CMyString &name, ISimDB *&pDB, const char *appName=NULL);

    bool getRedis(const CMyString &name, CMyRedis &redis);

private:
    int32_t ParseDB(CXmlFile &xml, void *pItem);
    bool IsExists(const CMyString &name);

private:
    VEC_DBINFO      m_vecDbItem;
};

