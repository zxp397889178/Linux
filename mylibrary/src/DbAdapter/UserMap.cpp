#include "UserMap.h"
#include "UserData.h"
#include <map>

namespace my_data{

    class CDaUserMap : public std::map< uint32_t, CUserData* >
    {

    };

    class CDaIdMap : public std::map< uint32_t, uint32_t >
    {

    };

}   //namespace my_data

using namespace my_data;
//////////////////////////////////////////////////////////////////////////
//CUserMap

CUserMap::CUserMap()
    : m_dbUser()
    , m_pUserIdMap(new CDaUserMap())
    , m_eSide(AS_SERVER)
    , m_pEvent(NULL)
	//, m_nUIN(100000)
{
    ASSERT(m_pUserIdMap);
    memset(m_szConfig, 0, sizeof(m_szConfig));
}

CUserMap& CUserMap::Instance()
{
    static CUserMap* s_pObject = new CUserMap();

    return *s_pObject;
}

CUserMap::~CUserMap()
{
    CDaUserMap::iterator it(m_pUserIdMap->begin());
    for(; it != m_pUserIdMap->end(); ++it)
    {
        CUserData* pUser = it->second;
        ASSERT(pUser);
        SAFE_DELETE(pUser);
    }
    m_pUserIdMap->clear();
    SAFE_DELETE(m_pUserIdMap);
}

bool CUserMap::Init(IBucketEvent *pEvent, const char *pszConfig, ISimDB *pDB)
{
    //static const char s_szIdMapSQL[] = "SELECT UIN FROM tAccount;";

    m_eSide = AS_SERVER;
    m_pEvent = pEvent;
    safe_strcpy(m_szConfig, sizeof(m_szConfig), pszConfig);

    //初始化数据库结构
    bool ret = m_dbUser.Init(m_szConfig, pDB);
    ASSERT(ret);
    if (!ret)
    {
        return false;
    }

	if (!m_pEvent->Init())
	{
		return false;
	}

/* 
    pDB->Cmd(s_szIdMapSQL);
    while(pDB->More())
    {
        m_nUIN = pDB->GetUInt(0);
    }
*/

    return true;
}

bool CUserMap::Init(IBucketEvent *pEvent, const char *pszConfig)
{
    m_eSide = AS_CLIENT;
    m_pEvent = pEvent;
    safe_strcpy(m_szConfig, sizeof(m_szConfig), pszConfig);

    return true;
}

CUserData* CUserMap::Load(uint32_t userId, bool bAutoNewUser)
{
	//printf("enter %s\n", __FUNCTION__);
	try
	{
    	if (!this->IsServerSide() || 0 == userId)
    	{
    	    return NULL;
    	}

		//printf("try to find user %u\n", userId);
    	CUserData *pUser = this->Find(userId);
    	if (NULL != pUser)
    	{
    	    return pUser;
    	}

		//printf("allocate user data.\n");
    	pUser = new CUserData();
    	if (NULL == pUser)
    	{
    	    return NULL;
    	}

		//printf("init user data.\n");
    	bool ret = pUser->Init(m_pEvent, m_szConfig);
    	if (!ret)
    	{
    	    SAFE_DELETE(pUser);
    	    return NULL;
    	}

		//printf("load data from database.\n");
    	ret = m_dbUser.LoadUser(userId, *pUser, bAutoNewUser);
    	if (!ret)
    	{
    	    SAFE_DELETE(pUser);
    	    return NULL;
    	}

		//printf("data load ok.\n");
    	uint32_t idUser = pUser->GetUserID();

    	pUser->SetBucketID(userId);
    	pUser->SetKeepRecordFlag( this->IsServerSide() );
    	m_pUserIdMap->insert(std::make_pair(idUser, pUser));

		LogDebug("SERVER DA_USERMAP.Load UserID: %u", userId);

    	return pUser;
	}
	catch(...)
	{
		LogErr("load user %u data failure", userId);
		return NULL;
	}
}

CUserData* CUserMap::LoadEx(uint32_t userId, byte_t *pData, size_t cbData, bool bCompressed/* = false*/)
{
    if (!this->IsServerSide())
    {
		LogErr("ERR: [%s:%d] not server side.", __FILE__, __LINE__);
        return NULL;
    }
    bool bNewUser(false);
    CUserData *pUser = this->Find(userId);
    if (NULL == pUser)
    {
        pUser = new CUserData();
        ASSERT(pUser);
        if (NULL == pUser)
        {
			LogErr("ERR: [%s:%d] create CUserData object failure.", __FILE__, __LINE__);
            return NULL;
        }
        if (!pUser->Init(m_pEvent, m_szConfig))
        {
			LogErr("ERR: [%s:%d] init user config [%s] failure.", __FILE__, __LINE__, m_szConfig);
            SAFE_DELETE(pUser);
            ASSERT(0);
            return NULL;
        }
        bNewUser = true;
    }

    //解包数据
    int ret(0);
    if (bCompressed)
    {
        ret = pUser->UnpackDataEx(pData, cbData);
    }
    else
    {
        ret = pUser->UnpackData(pData, cbData);
    }
    if (ret < 0)
    {
		LogErr("ERR: [%s:%d] UnpackData failure.", __FILE__, __LINE__);
        if (bNewUser)
        {
            SAFE_DELETE(pUser);
            return NULL;
        }
    }

	pUser->BindDataSource(&m_dbUser, userId);

    //增加映射关系
    if (bNewUser)
    {
        m_pUserIdMap->insert(std::make_pair(pUser->GetUserID(), pUser));
    }
    pUser->SetBucketID(userId);
    pUser->SetKeepRecordFlag( this->IsServerSide() );

	LogDebug("SERVER DA_USERMAP.Load UserID: %u", userId);

    return pUser;
}

CUserData* CUserMap::Load(uint32_t userId, byte_t *pData, size_t cbData, bool bCompressed/* = false*/)
{
    if (!this->IsClientSide())
    {
		LogErr("ERR: [%s:%d] not client side.", __FILE__, __LINE__);
        return NULL;
    }
    bool bNewUser(false);
    CUserData *pUser = this->Find(userId);
    if (NULL == pUser)
    {
        pUser = new CUserData();
        ASSERT(pUser);
        if (NULL == pUser)
        {
			LogErr("ERR: [%s:%d] create CUserData object failure.", __FILE__, __LINE__);
            return NULL;
        }
        if (!pUser->Init(m_pEvent, m_szConfig))
        {
			LogErr("ERR: [%s:%d] init user config [%s] failure.", __FILE__, __LINE__, m_szConfig);
            SAFE_DELETE(pUser);
            ASSERT(0);
            return NULL;
        }
        bNewUser = true;
    }

    //解包数据
    int ret(0);
    if (bCompressed)
    {
        ret = pUser->UnpackDataEx(pData, cbData);
    }
    else
    {
        ret = pUser->UnpackData(pData, cbData);
    }
    if (ret < 0)
    {
		LogErr("ERR: [%s:%d] UnpackData failure.", __FILE__, __LINE__);
        if (bNewUser)
        {
            SAFE_DELETE(pUser);
            return NULL;
        }
    }

    //增加映射关系
    if (bNewUser)
    {
        m_pUserIdMap->insert(std::make_pair(pUser->GetUserID(), pUser));
    }
    pUser->SetBucketID(userId);
    pUser->SetKeepRecordFlag( this->IsServerSide() );

	LogDebug("CLIENT DA_USERMAP.Load UserID: %u", userId);

    return pUser;
}

bool CUserMap::Unload(uint32_t userId, bool ignoreSave)
{
    CDaUserMap::iterator it(m_pUserIdMap->find(userId));
    if (it == m_pUserIdMap->end())
    {
        return false;
    }
    CUserData *pUser = it->second;
    ASSERT(pUser);
    if (NULL == pUser)
    {
        return false;
    }

    //回写数据
    int ret(0);
	if (!ignoreSave)
	{
	    if (this->IsServerSide())
	    {
	        ret = pUser->Flush2DB();
	    }
	    else
	    {
	        ret = pUser->Flush();
	    }
	    ASSERT(ret!=-1);
	}

    SAFE_DELETE(pUser);
    m_pUserIdMap->erase(it);

    LogDebug("DA_USERMAP.Unload UserID: %u", userId);

    return true;

}

CUserData* CUserMap::Find(uint32_t userId)
{
    CDaUserMap::iterator it(m_pUserIdMap->find(userId));
    if (it == m_pUserIdMap->end())
    {
        return NULL;
    }
    return it->second;
}

int CUserMap::Flush(uint32_t userId)
{
    CUserData *pUser = this->Find(userId);
    if (NULL == pUser)
    {
        return 0;   //未发现玩家
    }
    int ret(-1);
    if (this->IsServerSide())
    {
        ret = pUser->Flush2DB();
    }
    else
    {
        ret = pUser->Flush();
    }

    return ret;
}

void CUserMap::FlushAll()
{
    //typedef int(CUserMap::*FLUSH_FUNC)(void);

    CDaUserMap::iterator it(m_pUserIdMap->begin());
    for(; it != m_pUserIdMap->end(); ++it)
    {
        CUserData* pUser = it->second;
        ASSERT(pUser);
        if (this->IsServerSide())
            pUser->Flush2DB();
        else
            pUser->Flush();
    }
}

size_t CUserMap::GetCount(void) const
{
    return m_pUserIdMap->size();
}

size_t CUserMap::GetAll(std::vector<CUserData*> &arObjs)
{
    size_t cnt(0);
    for(CDaUserMap::iterator it(m_pUserIdMap->begin()); it != m_pUserIdMap->end(); ++it)
    {
        arObjs.push_back(it->second);
        ++cnt;
    }
    return cnt;
}

