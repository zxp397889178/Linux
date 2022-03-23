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
#include "IClient.h"
#include "pay_error.h"
#include <openssl/md5.h>
#include "MyUtility.h"
#include "MyDB.h"

#define HTTPRESP_MIN_BUFFER     2048 
#define HTTPRESP_CHUNK_SIZE     1024


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

// 判断是否为文本类型
bool IsTextType(const CMyString &strContType)
{
    if (!strncasecmp(strContType.c_str(), "text/", 5))
    {
        return true;
    }
    if (strContType == "application/x-www-form-urlencoded"
        || strContType == "application/json" 
        || strContType == "multipart/form-data")
    {
        return true;
    }
    return false;
}

int32_t send_chunked_data(IClient *pClient, byte_t *pData, size_t cbData, 
        const CMyString &ver, int32_t code, const CMyString &status, 
        const CMyString &strContType, const CMyString &strSvrInfo)
{
    CMyString strHeader("%s %d %s\r\n"
            "Date: %s\r\n"
            "Server: %s\r\n"
#ifdef HTTPRESP_ALLOW_CROS
            "Access-Control-Allow-Origin: *\r\n"            // 以下三个参数是允许CROS
            "Access-Control-Allow-Headers: *\r\n"
            "Access-Control-Allow-Methods: GET,POST\r\n"
#endif
            "Content-Type: %s;charset=utf-8\r\n"            // 按utf-8字符编码
            "Transfer-Encoding: chunked\r\n"                // 分块传输模式
            "Connection: close\r\n"                         // 连接后立即断开
#ifdef HTTPRESP_DISABLE_CACHE
            "Expires: -1\r\n"                               // 以下三个参数是禁止缓存
            "Cache-Control: no_cache\r\n"
            "Pragma: no-cache\r\n"
#endif
            ,
            ver.c_str(), code, status.c_str(),
            getGMTDate(::time(NULL)).c_str(),
            strSvrInfo.c_str(),
            strContType.c_str());
    bool bGZip(false);
    byte_t *pZData(NULL);
    uint32_t nZData(cbData);
    if (cbData>HTTPRESP_MIN_BUFFER && IsTextType(strContType))
    {
        bGZip = true;
        pZData = (byte_t*)calloc(1, nZData);
        if (pZData == NULL)
        {
            // 内存不足
            return -1;
        }
        int32_t ret = gzip::compress(pData, cbData, pZData, &nZData);
        if (ret != gzip::err_OK)
        {
            // gzip 的错误
            free(pZData);
            return ret;
        }
        // 手动添加了原始的大小
        strHeader.Append("Content-Encoding: gzip\r\nGZip-Origin-Bytes: %ld\r\n",
                cbData);
    }
    strHeader.Append("\r\n");
    pClient->SendMsg((void*)strHeader.c_str(), strHeader.size());
    LogDebug("%s", strHeader.c_str());

    CAutoArrayEx guard_zData(pZData);   // 此处自动守护压缩后的内存的释放
    int32_t totalSize((bGZip ? nZData : cbData) + strHeader.size());

    // 分块传输
    int32_t sizeLeft(bGZip ? nZData : cbData);
    char szHeader[64] = "";
    byte_t *chunk(bGZip ? pZData : pData);
    char szChunkEndl[32] = "\r\n";
    do {
        int32_t size1 = (sizeLeft>HTTPRESP_CHUNK_SIZE) ? HTTPRESP_CHUNK_SIZE : sizeLeft;
        sizeLeft -= size1;
        sprintf(szHeader, "%x\r\n", size1);
        pClient->SendMsg((void*)&szHeader[0], strlen(szHeader));
        pClient->SendMsg(chunk, size1);
        pClient->SendMsg((void*)&szChunkEndl[0], 2);
        chunk += size1;
        totalSize += strlen(szHeader)+2;
        LogDebug("send trunk size:%d", size1);
    }while(sizeLeft>0);
    // 发送结束标识
    char szChunkFinish[32] = "";
    strcpy(szChunkFinish, "0\r\n\r\n");  
    pClient->SendMsg((void*)&szChunkFinish[0], strlen(szChunkFinish));
    totalSize += 5;

    return totalSize;
}

int32_t send_chunked_file(IClient *pClient, 
        const CMyString &strFile, bool bTrailer, bool bDownload, 
        const CMyString &strSvrInfo)
{ 
    FILE *fp = fopen(strFile.c_str(), "rb");
    if (NULL == fp)
    {
        // return err_FileNotFound;
        return -1;
    }
    CAutoFile guard_File(fp);

    // 获得文件扩展信息
    CFileInfo finfo;
    finfo.Open(fp);
    CMyString strContType;
    if (!GetContentType(strFile, strContType))
    {
        // 未指定的文件类型，缺省为 text/plain
        strContType = "text/plain";
    }

    // 读取数据到内存中
    uint32_t cbData(finfo.GetSize());
    if (cbData == 0)
    {
        return -3;
        // return err_FileIsEmpty;
    }
    byte_t *pData = (byte_t*)calloc(1, cbData+1);
    fseek(fp, 1, SEEK_SET);
    fread(pData, 1, cbData, fp);
    CAutoArrayEx guard_Data(pData);

    // 构建MD5信息
    MD5_CTX ctx;
    uint8_t md5[16] = {0,};
    if (bTrailer)
    {
        MD5_Init(&ctx);
    }
    
    // 发送文件头
    CMyString strHeader("HTTP/1.1 200 OK\r\n"
            "Date: %s\r\n"
            "Server: %s\r\n"
            "Content-Type: %s\r\n"
            "Transfer-Encoding: chunked\r\n"                // 分块传输模式
            "Connection: close\r\n"                         // 连接后立即断开
            ,
            getGMTDate(::time(NULL)).c_str(),
            strSvrInfo.c_str(),
            strContType.c_str());
    bool bGZip(false);
    byte_t *pZData(NULL);
    uint32_t nZData(cbData);
    if (cbData>HTTPRESP_MIN_BUFFER && IsTextType(strContType))
    {
        bGZip = true;
        pZData = (byte_t*)calloc(1, nZData);
        if (pZData == NULL)
        {
            // 内存不足
            // return err_InsufficientMemory;
            return -11;
        }
        int32_t ret = gzip::compress(pData, cbData, pZData, &nZData);
        if (ret != gzip::err_OK)
        {
            // gzip 的错误
            free(pZData);
            return ret;
        }
        // 手动添加了原始的大小
        strHeader.Append("Content-Encoding: gzip\r\nGZip-Origin-Bytes: %ld\r\n",
                cbData);
    }
    if (bDownload)
    {
        strHeader.Append("Content-Disposition: attachment; filename=\"%s\"\r\n", finfo.GetFName());
    }
    strHeader.Append("\r\n");
    pClient->SendMsg((void*)strHeader.c_str(), strHeader.size());

    CAutoArrayEx guard_ZData(pZData);   // 此处自动守护压缩后的内存的释放
    int32_t totalSize((bGZip ? nZData : cbData) + strHeader.size());

    // 分块传输
    int32_t sizeLeft(bGZip ? nZData : cbData);
    char szHeader[64] = "";
    byte_t *chunk(bGZip ? pZData : pData);
    char szChunkEndl[32] = "\r\n";
    do {
        int32_t size1 = (sizeLeft>HTTPRESP_CHUNK_SIZE) ? HTTPRESP_CHUNK_SIZE : sizeLeft;
        sizeLeft -= size1;
        sprintf(szHeader, "%x\r\n", size1);
        pClient->SendMsg((void*)&szHeader[0], strlen(szHeader));
        pClient->SendMsg(chunk, size1);
        pClient->SendMsg((void*)&szChunkEndl[0], 2);
        if (bTrailer)
        {
            MD5_Update(&ctx, chunk, size1);
        }
        chunk += size1;
        totalSize += strlen(szHeader)+2;
    }while(sizeLeft>0);
    if (bTrailer)
    {
        MD5_Final(md5, &ctx);
    }

    // 发送结束标识
    char szChunkFinish[32] = "";
    strcpy(szChunkFinish, "0\r\n\r\n");  
    pClient->SendMsg((void*)&szChunkFinish[0], strlen(szChunkFinish));
    totalSize += 5;

    //发送Trailer标识
    if (bTrailer)
    {
        char szContentMD5[128] = "";
        char *pc = &szContentMD5[0];
        strcpy(szContentMD5, "Content-MD5: ");
        pc = &szContentMD5[strlen(szContentMD5)];
        for(int32_t i(0); i<(int32_t)sizeof(md5); ++i)
        {
            uint8_t c = md5[i];
            sprintf(pc+(i*2), "%02x", c); 
        }
        LogDebug(szContentMD5);
        strcat(szContentMD5, "\r\n");
        pClient->SendMsg((void*)&szContentMD5[0], strlen(szContentMD5));
        totalSize += strlen(szContentMD5);
    }
    return totalSize;
}


int32_t Broadcast2Sibling(int32_t cmd, Json::Value &data)
{
    ReloadCfg &rc = CONFIG.getReloadCfg();
    CMyRedis redis;
    int32_t ret = open_redis(rc.redis_name, redis);
    if (ret != 0)
    {
        LogErr("can not open redis, name:%s", rc.redis_name.c_str());
        return -1;
    }

    CMyString strNote;
    ret = BroadcastCmdToSibling(redis, rc.keyformat, cmd, data, strNote);
    if (ret<0)
    {
        LogErr(strNote.c_str());
        return -3;
    }
    LogMsg(strNote.c_str());
    return ret;
}


