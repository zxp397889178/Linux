/*
 * =====================================================================================
 *
 *       Filename:  QWFrame.cpp
 *
 *    Description:  session manager, for accept callback
 *
 *        Version:  1.0
 *        Created:  2015年09月08日 16时44分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zengyh, zengyh@quwangame.com
 *   Organization:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "QWFrameConfig.h"
#include <fstream>
CQWFrameConfig::CQWFrameConfig()
	: m_pFrameXml(NULL)
	, m_pXmlInfo(NULL)
	, m_pIptable(NULL)
	, m_timerXmlCheck(2000, false)
	, m_cfgLog()
	, m_cfgIpTable()
{
}

CQWFrameConfig::~CQWFrameConfig()
{
	SAFE_DELETE(m_pFrameXml);
	SAFE_DELETE(m_pXmlInfo);
	SAFE_DELETE(m_pIptable);
}

template<typename T>
class CObjGuard
{
public:
    CObjGuard(T *ptr)
        : m_ptr(ptr)
        , m_bGuard(true)
    {
    }
    void Guard(T *ptr)
    {
        m_ptr = ptr;
        m_bGuard = true;
    }
    ~CObjGuard()
    {
        if (NULL != m_ptr && m_bGuard)
        {
            SAFE_DELETE(m_ptr);
        }
    }
    void Release(void)
    {
        m_bGuard = true;
    }
private:
    T*      m_ptr;
    bool    m_bGuard;
};
bool CQWFrameConfig::Init(const CMyString &strFile)
{
	//m_pFrameXml = new CFileInfo("./frame.xml");
	m_pFrameXml = new CFileInfo();
	if (NULL == m_pFrameXml)
	{
		printf("ERR: can not find config file [frame.xml]\n");
		return false;
	}
    CObjGuard<CFileInfo> guard1(m_pFrameXml);
    CObjGuard<CFileInfo> guard2(m_pXmlInfo);
    if (!m_pFrameXml->Open("./frame.xml"))
    {
		printf("ERR: can not find config file [frame.xml]\n");
        return false;
    }

	if (strFile.length() > 0)
	{
		//m_pXmlInfo = new CFileInfo(strFile.c_str());
		m_pXmlInfo = new CFileInfo();
		if (NULL == m_pXmlInfo)
		{
			printf("ERR: can not find config file [%s]\n", strFile.c_str());
			return false;
		}
        guard2.Guard(m_pXmlInfo);
        if (!m_pXmlInfo->Open(strFile.c_str()))
        {
			printf("ERR: can not find config file [%s]\n", strFile.c_str());
            return false;
        }
	}
	

	if (this->ReadAllConfig())
	{
		m_timerXmlCheck.Startup();

        guard1.Release();
        guard2.Release();
		return true;
	}
	return false;
}

bool CQWFrameConfig::ReadAllConfig(void)
{
	CXmlFile xml;
	if (!xml.Load(m_pFrameXml->GetFullPath()))
	{
		LogFatal("ERR: load config file [%s] failure.", m_pFrameXml->GetFullPath());
		return false;
	}

	int32_t ret(-1);
	try
	{
		do {
			ret = this->ReadSvrCfg(xml); 			if (ret < 0) break;
			ret = this->ReadLogCfg(xml); 			if (ret < 0) break;
			ret = this->ReadIpTableCfg(xml); 		if (ret < 0) break;
			ret = this->ReadDatabaseCfg(xml); 		if (ret < 0) break;
			ret = this->ReadListenCfg(xml); 		if (ret < 0) break;
			ret = this->ReadConnectionCfg(xml); 	if (ret < 0) break;
		} while (0);
		if (ret < 0)
		{
			LogFatal("ERR: read [%s] failure.ret=%d", m_pFrameXml->GetFullPath(), ret);
			return false;
		}
	}
	catch (Bad_Config &e)
	{
		LogFatal("ERR: file[%s] bad config [%s]", m_pFrameXml->GetFullPath(), e.what());
		return false;
	}
	catch (...)
	{
		LogFatal("ERR: read config[%s] data failure.", m_pFrameXml->GetFullPath());
		return false;
	}

	return true;

}

int32_t CQWFrameConfig::ReadSvrCfg(CXmlFile &xml)
{
	const char *pszSect = "frame/svr";
	m_nSvrId = xml.GetLong(pszSect, "id");
	m_nActorId = xml.GetLong(pszSect, "actor");

	return 0;
}

int32_t CQWFrameConfig::ReadLogCfg(CXmlFile &xml)
{
	// error => -1
	const char *pszSect = "frame/log";
	m_cfgLog.strModule = xml.GetString(pszSect, "name");
	m_cfgLog.strPath = xml.GetString(pszSect, "path");
	m_cfgLog.level = xml.GetLong(pszSect, "level");

	if (Comm::OpenLog(m_cfgLog.strModule.c_str(), m_cfgLog.level, m_cfgLog.strPath.c_str()) != 0)
	{
		LogFatal("ERR: open log failure, path [%s] module [%s]", m_cfgLog.strPath.c_str(),
			m_cfgLog.strModule.c_str());
		return -1;
	}

	return 0;
}

int32_t CQWFrameConfig::ReadIpTableCfg(CXmlFile &xml)
{
	// error => -2
	m_cfgIpTable.enable = xml.GetBool("frame/iptables", "enable");
	m_iptables.Reset();
	SAFE_DELETE(m_pIptable);
	if (m_cfgIpTable.enable)
	{
		m_cfgIpTable.strFile = xml.GetString("server/iptables", "file");
		//m_pIptable = new CFileInfo(m_cfgIpTable.strFile.c_str());
		m_pIptable = new CFileInfo();
		if (NULL == m_pIptable)
		{
			LogFatal("ERR: not found iptable config file [%s]", m_cfgIpTable.strFile.c_str());
			return -2;
		}
        CObjGuard<CFileInfo> guard3(m_pIptable);
        if (!m_pIptable->Open(m_cfgIpTable.strFile.c_str()))
        {
			LogFatal("ERR: not found iptable config file [%s]", m_cfgIpTable.strFile.c_str());
			return -2;
        }
		if (!m_iptables.Init(m_cfgIpTable.strFile))
		{
			return -2;
		}
        guard3.Release();
	}
	return 0;
}

int32_t CQWFrameConfig::ReadDatabaseCfg(CXmlFile &xml)
{
	CMyString strDbFile = xml.GetString("frame/mysql", "file");
	CMyString strContent;
	this->ReadDbXmlContent(strDbFile, strContent);

	// 解密
	// this->DecryptMysqlCfg(strContent);

	CXmlFile dbXml;
	if (!dbXml.Parse(strContent.c_str(), strContent.length()))
	{
		LogFatal("ERR: prase config file [%s] failure.", strDbFile.c_str());
		return -1;
	}

	// 加载dbconfig.xml配置
	MYSQL_CFG_LIST mapMysqlCfg;
	void *pElement = dbXml.GetElement("db/mysql/conn");
	while (pElement != NULL)
	{
		MysqlCfg mysql;
		mysql.strName = dbXml.GetValue(pElement, "name");
		mysql.strHost = dbXml.GetValue(pElement, "host");
		mysql.nPort = StrToUint32(dbXml.GetValue(pElement, "port"));
		mysql.strDb = dbXml.GetValue(pElement, "db");
		mysql.strUser = dbXml.GetValue(pElement, "user");
		mysql.strPasswd = dbXml.GetValue(pElement, "passwd");
		mysql.strCharset = dbXml.GetValue(pElement, "charset");

		LogDebug("INIT:CONFIG, read db config name[%s], host[%s], db[%s], user[%s], passwd[%s], charset[%s]",
			mysql.strName.c_str(), mysql.strHost.c_str(), mysql.strDb.c_str(), mysql.strUser.c_str(), mysql.strPasswd.c_str(), mysql.strCharset.c_str());

		mapMysqlCfg.insert(make_pair(mysql.strName, mysql));
		pElement = dbXml.GetNextSibling(pElement, "conn");
	}

	// 加载frame.xml的mysql配置，根据name从dbconfig.xml找到对应的数据库账号密码等信息
	m_cfgMysqlList.clear();
	void *pConnElement = xml.GetElement("frame/mysql/conn");
	while (pConnElement != NULL)
	{
		CMyString strName = xml.GetValue(pConnElement, "name");
		uint32_t nId = StrToUint32(xml.GetValue(pConnElement, "id"));

		MYSQL_CFG_LIST::iterator iter = mapMysqlCfg.find(strName);
		if (iter == mapMysqlCfg.end())
		{
			LogFatal("ERR: frame.xml mysql conn name[%s] not found in dbconfig.xml .", strName.c_str());
			return -2;
		}
		MysqlCfg &mysql = iter->second;
		mysql.nId = nId;
		m_cfgMysqlList.insert(make_pair(mysql.strName, mysql));
		pConnElement = xml.GetNextSibling(pConnElement, "conn");
	}


	// redis配置
	m_cfgRedisList.clear();
	void *pElement1 = xml.GetElement("frame/redis/conn");
	while (pElement1 != NULL)
	{
		RedisCfg redis;
		redis.strName = xml.GetValue(pElement1, "name");
		redis.strHost = xml.GetValue(pElement1, "host");
		redis.port = StrToUint32(xml.GetValue(pElement1, "port"));
		redis.dbIndex = StrToUint32(xml.GetValue(pElement1, "db"));
		redis.timeout = StrToUint32(xml.GetValue(pElement1, "timeout"));
		redis.nId = StrToUint32(xml.GetValue(pElement1, "id"));

		m_cfgRedisList.insert(make_pair(redis.strName, redis));
		pElement1 = xml.GetNextSibling(pElement1, "conn");
	}

	return 0;
}

int32_t CQWFrameConfig::ReadListenCfg(CXmlFile &xml)
{
	m_cfgListenList.clear();
	void *pElement = xml.GetElement("frame/listener/listen");
	while (pElement != NULL)
	{
		ListenCfg listen;
		listen.strName = xml.GetValue(pElement, "name");
		listen.nActorId = StrToUint32(xml.GetValue(pElement, "actor"));
		listen.nPort = StrToUint32(xml.GetValue(pElement, "port"));
		listen.strProto = xml.GetValue(pElement, "proto");
		listen.nId = StrToUint32(xml.GetValue(pElement, "id"));	

		m_cfgListenList.insert(make_pair(listen.nActorId, listen));
		pElement = xml.GetNextSibling(pElement, "listen");
	}
	return 0;
}


int32_t CQWFrameConfig::ReadConnectionCfg(CXmlFile &xml)
{
	m_cfgConnectionList.clear();
	void *pElement = xml.GetElement("frame/connector/conn");
	while (pElement != NULL)
	{
		ConnectionCfg conn;
		conn.strName	= xml.GetValue(pElement, "name");
		conn.nId			= StrToUint32(xml.GetValue(pElement, "id"));
		conn.nActorId	= StrToUint32(xml.GetValue(pElement, "actor"));
		conn.strHost		= xml.GetValue(pElement, "host");
		conn.nPort		= StrToUint32(xml.GetValue(pElement, "port"));
		conn.nCount		= StrToUint32(xml.GetValue(pElement, "count"));

		m_cfgConnectionList.insert(make_pair(conn.nActorId, conn));
		pElement = xml.GetNextSibling(pElement, "conn");
	}
	return 0;
}

/*
void CQWFrameConfig::DecryptMysqlCfg(CMyString &strValue)
{
	// aes ecb模式
	CMyString strKey = "DfvLXtx6hLrUw31A";
	//CAesWrapper aesWrapper;
	//aesWrapper.SetKey(strKey.c_str(), strKey.length());

	//printf("strValue=[%s]\n", strValue.c_str());

	// base64解码
	void *pBuf = NULL;
	size_t cbOutput = 0;
	CBase64Wrapper::Decode(strValue.c_str(), pBuf, cbOutput);
	string strData;
	strData.assign((const char *)pBuf, cbOutput);
	if (pBuf != NULL)
	{
		free(pBuf);
	}

	//printf("strData=[%s],len=[%lu]\n", strData.c_str(), strData.length());

	//CMyString strOut;
	//aesWrapper.DecryptECB(strData, strOut);
	//strValue = strOut;
    

	//printf("db file =[%s]\n", strValue.c_str());
	return;
}
*/

int32_t CQWFrameConfig::ReadDbXmlContent(CMyString &strFile, CMyString &strContent)
{
	ifstream infile;
	infile.open(strFile.c_str());   //将文件流对象与文件连接起来 
	if (!infile.is_open())
	{
		LogFatal("ERR: can not open [%s].", strFile.c_str());
		return -1;
	}

	string str;
	while (getline(infile, str))
	{
		strContent.Append(str.c_str());
	}
	infile.close();             //关闭文件输入流 

	return 0;
}

void CQWFrameConfig::Update(int32_t interval /* = 5000 */)
{
	if (m_timerXmlCheck.ToNextTime(interval))
	{
		if (m_pFrameXml->IsModified())
		{
			bool ret = this->ReadAllConfig();
			if (!ret)
				LogFatal("ERR: mallsvr config xml reload FAIL");
			else
				LogImpt("proxy config reload SUCCEED.");
		}
	}
}

bool CQWFrameConfig::IsIpValid(uint32_t ip)
{
	return m_iptables.IsIpAllow(ip);
}

bool CQWFrameConfig::IsIpValid(const CMyString &strIp)
{
	return m_iptables.IsIpAllow(strIp);
}

bool CQWFrameConfig::IsDevValid(const CMyString &strDev)
{
	return m_iptables.IsDeviceAllow(strDev);
}

bool CQWFrameConfig::IsUinValid(uint32_t uin)
{
	return m_iptables.IsUinAllow(uin);
}


ListenCfg *CQWFrameConfig::GetListenCfg(uint32_t port)
{
	for (LISTEN_CFG_LIST::iterator iter = m_cfgListenList.begin(); iter != m_cfgListenList.end(); ++iter)
	{
		if (iter->second.nPort == port)
		{
			return &iter->second;
		}
	}

	return NULL;
}

MysqlCfg *CQWFrameConfig::GetMysqlCfg(const char *dbName)
{
	MYSQL_CFG_LIST::iterator iter = m_cfgMysqlList.find(dbName);
	if (iter == m_cfgMysqlList.end())
	{
		return NULL;
	}

	return &iter->second;
}
RedisCfg *CQWFrameConfig::GetRedisCfg(const char *dbName)
{
	REDIS_CFG_LIST::iterator iter = m_cfgRedisList.find(dbName);
	if (iter == m_cfgRedisList.end())
	{
		return NULL;
	}

	return &iter->second;
}

ConnectionCfg *CQWFrameConfig::GetConnectionCfg(uint32_t actorId)
{
	CONNECTION_CFG_LIST::iterator iter = m_cfgConnectionList.find(actorId);
	if (iter == m_cfgConnectionList.end())
	{
		return NULL;
	}

	return &iter->second;
}







