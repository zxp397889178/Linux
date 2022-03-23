/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  统计代码行
 *
 *        Version:  1.0
 *        Created:  2020年07月02日 20时03分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "BaseCode.h"
#include "MyDB.h"
#include "Security.h"

void syntax(const char *app)
{
    printf("syntax: %s <config.xml>\n", app);
}

int32_t open_mysql(CXmlFile &xml, ISimDB *&pDB);
int32_t open_redis(CXmlFile &xml, CMyRedis &redis);
int32_t get_repo_config(CXmlFile &xml, void *pItem, 
        CMyString &strUrl, CMyString &strUser, 
        CMyString &strPasswd, CMyString &strName, int32_t &style, int32_t &startRev);
int32_t decode_svn_stat(ISimDB *pDB, CMyString &strScript, CMyString &strUrl, 
        CMyString &strUser, CMyString &strPasswd, CMyString &strName, 
        int32_t rev, int32_t step);
void get_next_rev(CMyRedis &redis, const CMyString &strName, int32_t &rev);
int32_t get_next_rev_from_db(ISimDB *pDB, const CMyString &strName, int32_t startRev);
void set_next_rev(CMyRedis &redis, const CMyString &strName, int32_t rev);

int32_t main(int32_t argc, char **argv)
{
    if (argc < 2)
    {
        printf("syntax error.\n");
        syntax(argv[0]);
        return -1;
    }

    CXmlFile xml;
    if (!xml.Load(argv[1]))
    {
        printf("can not load config xml: %s\n", argv[1]);
        return -3;
    }

    // 初始化日志目录
    const char *sect1 = "main/log";
    CMyString strModule = xml.GetString(sect1, "module");
    CMyString strPath = xml.GetString(sect1, "path");
    int32_t logLevel = xml.GetLong(sect1, "level");
    mkdir(strPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); 
    CLogDaemon guard(strModule.c_str(), logLevel, strPath.c_str());

    // 打开mysql连接
    ISimDB *pDB(NULL);
    int32_t ret = open_mysql(xml, pDB);
    if (ret < 0)
    {
        printf("open mysql failure, result=%d\n", ret);
        return -5;
    }
    CAutoDB guard2(pDB);

    CMyRedis redis;
    ret = open_redis(xml, redis);
    if (ret < 0)
    {
        printf("open redis failure, result=%d\n", ret);
        return -6;
    }
    
    CMyString strScript = xml.GetString("main/repo", "script");
    if (strScript.empty())
    {
        printf("not config svn stat script.\n");
        LogErr("not config svn stat script.");
        return -7;
    }
    LogDebug("script: [%s]", strScript.c_str());

    int32_t step = xml.GetLong("main/repo", "step");
    if (step <=0)
    {
        step = 50;
    }
    
    // 循环处理repo
    void *pRoot = xml.GetElement("main/repo/svn");
    if (NULL == pRoot)
    {
        printf("not found any repo config.\n");
        LogErr("not found any repo config.");
        return -7;
    }
    CMyString strName;
    CMyString strUrl;
    CMyString strUser;
    CMyString strPasswd;
    void *pItem = pRoot;
    uint64_t t1 = Now(false);
    int32_t rev(0);
    do{
        int32_t style(0);
        int32_t startRev(0);
        ret = get_repo_config(xml, pItem, strUrl, strUser, strPasswd, strName, style, startRev);
        if (ret < 0)
        {
            printf("get repo config failure, result=%d, continue next.\n", ret);
            pItem = xml.GetNextSibling(pItem, "svn");
            continue;
        }
        
        if (style == 0)
            get_next_rev(redis, strName, rev);
        else
        {
            rev = get_next_rev_from_db(pDB, strName, startRev);
            step = 1;
        }

        ret = decode_svn_stat(pDB, strScript, strUrl, strUser, strPasswd, strName, rev, step); 
        if (ret < 0)
        {
            printf("decode svn stat failure, name:%s\n", strName.c_str());
        }

        set_next_rev(redis, strName, rev+step);

        ++rev;
        pItem = xml.GetNextSibling(pItem, "svn");
    }while(pItem != NULL);

    uint64_t t2 = Now(false);
    LogMsg("decode svn stat succeed, repo rev:%d, cost %ldms.", rev, (t2-t1));
    printf("\ndecode svn stat succeed, repo rev:%d, cost %.2lfs\n.", rev, (t2-t1)/1000.0);

    return 0;
}

int32_t open_mysql(CXmlFile &xml, ISimDB *&pDB)
{
    const char *sect = "main/dbms_mysql";
    CMyString strHost;
    CMyString strDb;
    CMyString strUser;
    CMyString strPasswd;
    int32_t nPort;
    try
    {
        strHost = xml.GetString(sect, "host");
        strDb = xml.GetString(sect, "db");
        strUser = xml.GetString(sect, "user");
        strPasswd = xml.GetString(sect, "passwd");
        nPort = xml.GetLong(sect, "port");
    }
    catch(CException &e)
    {
        LogErr("read mysql config failure, %s", e.what());
        return -3;
    }
    catch(...)
    {
        LogErr("read mysql config failure.");
        return -5;
    }

    // 解密密码
    CMyString strText;
    try
    {
        int32_t ret = CPasswdWrapper::Decode(strPasswd, strText);
        if (ret < 0)
        {
            LogErr("mysql password incorrect");
            return -7;
        }
    }
    catch(CException &e)
    {
        printf("decode mysql password failure, %s\n", e.what());
        LogErr("decode mysql password failure, %s", e.what());
        return -11;
    }
    catch(...)
    {
        printf("decode mysql password failure\n");
        LogErr("decode mysql password failure");
        return -13;
    }
    strPasswd = strText;

    //printf("user:%s, passwd:%s, host:%s, db:%s, port:%d\n",
    //        strUser.c_str(), strPasswd.c_str(), strHost.c_str(), strDb.c_str(), nPort);
    pDB = CreateMysqlConn(strUser.c_str(), strPasswd.c_str(),
            strHost.c_str(), strDb.c_str(), nPort,
            0, 0, "utf8mb4");
    if (NULL == pDB)
    {   
        LogErr("connect to mysql failure");
        return -9;
    }
    
    LogDebug("connect to mysql succeed: host [%s:%u], db [%s], user [%s]",
           strHost.c_str(), nPort, strDb.c_str(), strUser.c_str());
    return 0;
}

int32_t open_redis(CXmlFile &xml, CMyRedis &redis)
{
    const char *sect = "main/dbms_redis";
    CMyString strHost;
    CMyString strDb;
    CMyString strUser;
    CMyString strPasswd;
    int32_t nPort;
    try
    {
        strHost = xml.GetString(sect, "host");
        strDb = xml.GetString(sect, "db");
        strUser = xml.GetString(sect, "user");
        strPasswd = xml.GetString(sect, "passwd");
        nPort = xml.GetLong(sect, "port");
    }
    catch(CException &e)
    {
        LogErr("read redis config failure, %s", e.what());
        return -3;
    }
    catch(...)
    {
        LogErr("read redis config failure.");
        return -5;
    }

    // 解密密码
    if (!strPasswd.empty())
    {
        CMyString strText;
        try
        {
            int32_t ret = CPasswdWrapper::Decode(strPasswd, strText);
            if (ret < 0)
            {
                LogErr("mysql password incorrect");
                return -7;
            }
        }
        catch(CException &e)
        {
            printf("decode mysql password failure, %s\n", e.what());
            LogErr("decode mysql password failure, %s", e.what());
            return -11;
        }
        catch(...)
        {
            printf("decode mysql password failure\n");
            LogErr("decode mysql password failure");
            return -13;
        }
        strPasswd = strText;
    }

    // 打开redis
    redis.Close();
    if (!redis.Open(strHost.c_str(), nPort, strPasswd.c_str(), 1000))
    {   
        LogErr("connect to redis failure");
        return -9;
    }
    redis.Select(strDb.asInt());
    
    LogDebug("connect to redis succeed: host [%s:%u], db [%s]",
           strHost.c_str(), nPort, strDb.c_str());
    return 0;
}

int32_t get_repo_config(CXmlFile &xml, void *pItem, 
        CMyString &strUrl, CMyString &strUser, 
        CMyString &strPasswd, CMyString &strName,
        int32_t &style, int32_t &startRev)
{
    int32_t result(-1);
    try
    {
        do{
            if (!xml.GetValue(pItem, "name", strName)) break;
            if (!xml.GetValue(pItem, "url", strUrl)) break;
            if (!xml.GetValue(pItem, "user", strUser)) break;
            if (!xml.GetValue(pItem, "passwd", strPasswd)) break; 
            if (!xml.GetValue(pItem, "style", style))
                style = 0;
            if (!xml.GetValue(pItem, "start_rev", startRev))
                startRev = 1;
            result = 0;
        }while(false);
    }
    catch(CException &e)
    {
        LogErr("read repo config failure, %s", e.what());
    }
    catch(...)
    {
        LogErr("read repo config failure.");
    }

    if (result < 0)
    {
        LogErr("missing config in repo.");
        return -1;
    }

    // 解密密码
    CMyString strText;
    try
    {
        int32_t ret = CPasswdWrapper::Decode(strPasswd, strText);
        if (ret < 0)
        {
            LogErr("repo password incorrect");
            return -3;
        }
    }
    catch(CException &e)
    {
        LogErr("decode password failure, %s", e.what());
        return -5;
    }
    catch(...)
    {
        LogErr("decode password failure.");
        return -7;
    }
    strPasswd = strText;

    return 0;
}

class CScriptGuard
{
public:
    explicit CScriptGuard(FILE *_fp)
        : m_fp(_fp)
    {
    }
    ~CScriptGuard()
    {
        if (NULL!=m_fp)
        {
            pclose(m_fp);
            m_fp = NULL;
        }
    }
private:
    FILE *m_fp;
};

void ltrim2(char* &text)
{
    int32_t step(0);
    size_t cnt = strlen(text);
    for(size_t i(0); i<cnt; ++i)
    {
        unsigned char c = text[i];
        if (isspace(c))
        {
            ++step;
        }
    }
    char *pc = &text[step];
    text = pc;
}

void trim_endl(char* pc)
{
    size_t cnt = strlen(pc);
    int32_t step = cnt-1;
    for(size_t i(cnt); i>0; --i)
    {
        uint8_t c = pc[i-1];
        if (c==0x0D || c==0x0A)
        {
            pc[i-1]='\0';
            --step;
        }
        break;
    }
}

int32_t decode_svn_stat(ISimDB *pDB, CMyString &strScript, CMyString &strUrl, 
        CMyString &strUser, CMyString &strPasswd, CMyString &strName, int32_t rev, int32_t step)
{
    int32_t nNextRev(rev);
    // 构建script 脚本
    CMyString strCmd("%s -x %s -s %d -e %d -u %s -p %s", 
            strScript.c_str(), strUrl.c_str(), 
            nNextRev, nNextRev+step, 
            strUser.c_str(), strPasswd.c_str());
    LogImpt("begin to stat codeline, name[%s] rev:[%d-%d]", 
            strName.c_str(), nNextRev, nNextRev+step);
    //printf("%s\n", strCmd.c_str());
    FILE *fp = popen(strCmd.c_str(), "r");
    if (NULL == fp)
    {
        LogErr("can not open scipt %s", strScript.c_str());
        return -1;
    }
    CScriptGuard guard(fp);

    int32_t count(0);
    char *pc(NULL);
    do{
        char buffer[1024] = "";
        pc = fgets(buffer, sizeof(buffer), fp);
        if (NULL == pc)
        {
            // 全部获取完毕
            break;
        }
        trim_endl(buffer);
        char *pc = buffer;
        CStrSpliter css;
        css.Split(pc, ' ');
        if (css.size() < 5)
        {
            // 解析不正确，退出
            LogWarn("can not decode string [%s], cnt=%d", buffer, css.size());
            continue;
        }
        LogDebug("begin to decode project:%s, line[%s]", strName.c_str(), pc);
        
        CMyString rev = css[0];
        CMyString user = css[1];
        CMyString line = css[2];
        CMyString date("%s %s", css[3].c_str(), css[4].c_str());

        // 写入数据库
        pDB->Cmd("call pSvnStat('%s', %s, '%s', %s, '%s')",
                strName.c_str(), rev.c_str(), user.c_str(), line.c_str(), date.c_str());
        int32_t ret = pDB->Exec();
        if (ret < 0)
        {
            LogErr("save svn stat for %s failure.", strName.c_str());
            break;
        }
        ++count;
    }while(true);

    if (count > 0)
    {
        LogMsg("save %d svn stat on %s.", count, strName.c_str());
    }
    return 0;
}

void get_next_rev(CMyRedis &redis, const CMyString &strName, int32_t &rev)
{
    int64_t val(0);
    CMyString strKey("codestat_repo_%s", strName.c_str());
    int32_t ret = redis.Get(strKey.c_str(), val);
    if (ret <= 0)
    {
        redis.Set(strKey.c_str(), val);
    }
    rev = val;
}

int32_t get_next_rev_from_db(ISimDB *pDB, const CMyString &strName, int32_t startRev)
{
    int32_t rev(0);
    pDB->Cmd("SELECT MIN(`rev`) FROM `svnstat` WHERE `name`='%s' AND `line`=-1 AND `rev`>=%d;", strName.c_str(), startRev);
    while(pDB->More())
    {
        rev = pDB->GetInt(0);
    }
    return rev;
}
void set_next_rev(CMyRedis &redis, const CMyString &strName, int32_t rev)
{
    int64_t val(rev);
    CMyString strKey("codestat_repo_%s", strName.c_str());
    redis.Set(strKey.c_str(), val);
}

