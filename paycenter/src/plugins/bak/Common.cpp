/*
 * =====================================================================================
 *
 *       Filename:  Common.cpp
 *
 *    Description:  公用函数
 *
 *        Version:  1.0
 *        Created:  03/04/2020 06:51:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"

int32_t open_mysql(const CMyString &strCfgName, ISimDB *&pDB)
{
    DbmsCfg mycfg;
    int32_t ret = CONFIG.getDbmsByName(strCfgName, mycfg);
    if (ret < 0)
    {
        LogErr("not found dbms [%s] in \'main/dbms\'. ", strCfgName.c_str());
        return PAYFRAME_ERROR(err_DbmsNotConfig);
    }
    if (mycfg.dbms_type != dbms_mysql)
    {
        LogErr("dbms [%s] not mysql type.", strCfgName.c_str());
        return PAYFRAME_ERROR(err_DbmsNotMysql);
    }

    pDB = CreateMysqlConn(mycfg.user.c_str(), mycfg.passwd.c_str(),
            mycfg.host.c_str(), mycfg.db.c_str(), mycfg.port,
            0, 0, "utf8mb4");
    if (NULL == pDB)
    {
        LogErr("connect to mysql failure, dbms[%s]", strCfgName.c_str());
        return PAYFRAME_ERROR(err_DbmsOpenFailure);
    }

    LogDebug("DBG>load mysql [%s]: host [%s:%u], db [%s], user [%s]", 
           strCfgName.c_str(), mycfg.host.c_str(), mycfg.port, 
           mycfg.db.c_str(), mycfg.user.c_str());
    return err_Success;
}

int32_t open_mysql(const CMyString &strSect, const CMyString &strKey, ISimDB *&pDB)
{
    CMyString strCfgName;
    CXmlFile &xml = CONFIG.getXml();
    try
    {
        strCfgName = xml.GetString(strSect.c_str(), strKey.c_str());
    }
    catch(CException &e)
    {
        LogErr("get mysql config failure, %s", e.what());
        return PAYFRAME_ERROR(err_DbmsNotConfig);
    }
    catch(...)
    {
        LogErr("get mysql config failure");
        return PAYFRAME_ERROR(err_DbmsConfigFault);
    }

    return open_mysql(strCfgName, pDB);
}

int32_t open_redis(const CMyString &strCfgName, CMyRedis &redis)
{
    // open redis
    DbmsCfg recfg;
    int32_t ret = CONFIG.getDbmsByName(strCfgName, recfg);
    if (ret<0)
    {
        LogErr("can not get redis config, name = %s", strCfgName.c_str());
        return PAYFRAME_ERROR(err_DbmsNotConfig);
    }
    bool val = redis.Open(recfg.host.c_str(), recfg.port, 
            recfg.passwd.c_str(), 300);
    if (!val)
    {
        LogErr("open redis connection failure, config name [%s]", strCfgName.c_str());
        return PAYFRAME_ERROR(err_DbmsOpenFailure);
    }
    val = redis.Select(atoi(recfg.db.c_str()) );
    if (!val)
    {
        LogErr("not exists redis db %s, config name [%s]", recfg.db.c_str(), strCfgName.c_str());
        return PAYFRAME_ERROR(err_DbmsDbInvalid);
    }

    LogDebug("DBG>load redis[%s]: host [%s:%u], db [%s]", 
           strCfgName.c_str(), recfg.host.c_str(), recfg.port, 
           recfg.db.c_str());
    return err_Success;
}

int32_t open_redis(const CMyString &strSect, const CMyString &strKey, CMyRedis &redis)
{
    CMyString strCfgName;
    CXmlFile &xml = CONFIG.getXml();
    try
    {
        strCfgName = xml.GetString(strSect.c_str(), strKey.c_str());
    }
    catch(CException &e)
    {
        LogErr("get redis config failure, %s", e.what());
        return PAYFRAME_ERROR(err_DbmsNotConfig);
    }
    catch(...)
    {
        LogErr("get redis config failure");
        return PAYFRAME_ERROR(err_DbmsConfigFault);
    }

    return open_redis(strCfgName, redis);
}

bool getConfigInt(const CMyString sect_, const CMyString key_, int32_t &val)
{
    bool result(false);
    CXmlFile &xml = CONFIG.getXml();
    try
    {
        val = xml.GetLong(sect_.c_str(), key_.c_str());
        result = true;
    }
    catch(CException &e)
    {
        LogErr("read config failure, %s", e.what());
    }
    catch(...)
    {
        LogErr("read config failure, sect[%s] key[%s]", sect_.c_str(), key_.c_str());
    }
    return result;
}

bool getConfigStr(const CMyString sect_, const CMyString key_, CMyString &val)
{
    bool result(false);
    CXmlFile &xml = CONFIG.getXml();
    try
    {
        val = xml.GetString(sect_.c_str(), key_.c_str());
        result = true;
    }
    catch(CException &e)
    {
        LogErr("read config failure, %s", e.what());
    }
    catch(...)
    {
        LogErr("read config failure, sect[%s] key[%s]", sect_.c_str(), key_.c_str());
    }
    return result;
}

bool getConfigDouble(const CMyString sect_, const CMyString key_, double &val)
{
    bool result(false);
    CXmlFile &xml = CONFIG.getXml();
    try
    {
        val = xml.GetDouble(sect_.c_str(), key_.c_str());
        result = true;
    }
    catch(CException &e)
    {
        LogErr("read config failure, %s", e.what());
    }
    catch(...)
    {
        LogErr("read config failure, sect[%s] key[%s]", sect_.c_str(), key_.c_str());
    }
    return result;
}

bool getConfigBool(const CMyString sect_, const CMyString key_, bool &val)
{
    bool result(false);
    CXmlFile &xml = CONFIG.getXml();
    try
    {
        val = xml.GetBool(sect_.c_str(), key_.c_str());
        result = true;
    }
    catch(CException &e)
    {
        LogErr("read config failure, %s", e.what());
    }
    catch(...)
    {
        LogErr("read config failure, sect[%s] key[%s]", sect_.c_str(), key_.c_str());
    }
    return result;
}

CMyString getClientIp(IClient *pClient, http::CHttpRequest &req)
{
    CMyString strSource;
    CMyString result;
    bool bDeal(false);
    bool ret(false);
    do {
        ret = getConfigStr("main/clientIp", "source", strSource);
        if (!ret) break;

        strSource.MakeUpper();
        if (strSource != "NET")
        {
            CMyString strField;
            ret = getConfigStr("main/clientIp", "prop", strField);
            if (!ret) break;

            if (strSource == "HEADER")
            {
                result = req.getHeader(strField);
                bDeal = true;
            }
            else if (strSource == "PARAM")
            {
                result = req.getParam(strField);
                bDeal = true;
            }
            else if (strSource == "COOKIE")
            {
                result = req.getCookie(strField);
                bDeal = true;
            }
        }
    }while(0);
    if (!bDeal || result.empty())
    {
        if (NULL == pClient)
        {
            result = "0.0.0.0";
        }
        else
        {
            result = pClient->getIpStr();
        }
    }

    return result;
}

CMyString getClientIpEx(int32_t type, IClient *pClient, http::CHttpRequest &req, const CMyString &ipField)
{
    CMyString strIp;
    CMyString strHost;
    switch(type)
    {
    case 0:
        strIp = pClient->getIpStr();
        break;
    case 1:
        strHost = req.getHeader(ipField);
        break;
    case 2:
        strHost = req.getParam(ipField);
        break;
    case 3:
        strHost = req.getCookie(ipField);
        break;
    default:
        // 无效IP
        strIp = "0.0.0.0";
        break;
    }

    // 解析IP => 192.168.2.10:8090
    if (!strHost.empty())
    {
        std::string::size_type pos = strHost.find(':');
        if (pos != std::string::npos)
        {
            strIp = strHost.substr(0, pos-1);
        }
        else
        {
            strIp = strHost;
        }
    }
    strIp.trim();
    return strIp;
}
