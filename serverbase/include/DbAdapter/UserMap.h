/*
 * =====================================================================================
 *
 *       Filename:  UserMap
 *
 *    Description:  用户对照表
 *
 *        Version:  1.0
 *        Created:  2014-4-11
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_DA_USERMAP_H_
#define _GFX_SERVER_DA_USERMAP_H_

#include "UserDbData.h"
//注意：必须在ISimDB* 及 IClientSocket* 之前释放此UserMap，否则可能会导致出错
namespace my_data{

enum ADAPTER_SIDE
{
    AS_SERVER,
    AS_CLIENT,
};

class CDaUserMap;
class CDaIdMap;
class CUserMap
{
public:
    CUserMap();
    ~CUserMap();

    static CUserMap& Instance();
    void release() { delete this; }

    //Server端初始化
    bool Init(IBucketEvent *pEvent, const char *pszConfig, ISimDB *pDB);
	/*  Server端装载,若不存在userId, 则创建之
	 *  param:	userId			需要从数据库中装载的用户ID
	 *  		bAutoNewUser	true-若数据库中不存在该用户，则自动创建之；false-不自动创建新用户
	 * */
    CUserData* Load(uint32_t userId, bool bAutoNewUser = false );
	CUserData* LoadEx(uint32_t userId, byte_t *pData, size_t cbData, bool bCompressed = false);

    //Client端初始化
    bool Init(IBucketEvent *pEvent, const char *pszConfig);
    //Client端装载
    CUserData* Load(uint32_t userId, byte_t *pData, size_t cbData, bool bCompressed = false);

public:
    bool Unload(uint32_t userId, bool ignoreSave);

    CUserData* Find(uint32_t userId);

    int Flush(uint32_t userId);
    void FlushAll();

    inline bool IsServerSide(void) const { return m_eSide == AS_SERVER; }
    inline bool IsClientSide(void) const { return m_eSide == AS_CLIENT; }

    size_t GetCount(void) const;
    size_t GetAll(std::vector<CUserData*> &arObjs);


private:
    CDBUserData         m_dbUser;
    CDaUserMap*         m_pUserIdMap;   // userId ---> UserData*

    enum ADAPTER_SIDE   m_eSide;
    char                m_szConfig[128];
    IBucketEvent*       m_pEvent;

	//uint32_t			m_nUIN;
};

}   //namespace my_data

#define DA_USERMAP        my_data::CUserMap::Instance()
#define FREE_DA_USERMAP   { my_data::CUserMap::Instance().release(); }

#endif  //_GFX_SERVER_DA_USERMAP_H_

