/*
 * =====================================================================================
 *
 *       Filename:  BillPointList.cpp
 *
 *    Description:  计费点列表管理
 *
 *        Version:  1.0
 *        Created:  2021年03月04日 23时29分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "BillPointList.h"
#include "BillpointDataList.h"
#include "MyNet.h"
#include "Config.h"

CBillPoint::CBillPoint(int32_t bpId, int32_t configId, const CMyString &bpCode,
            int64_t billpoint, int64_t coin, int64_t extCoin, const CMyString &itemInfo,
            int32_t initCount, const CMyString &bpIconUrl, int32_t bpNameId,
            const CMyString &versionRange, const CMyString &channelRange, const CMyString &vipRange,
            int32_t dailyCount, int32_t monthlyCount, int32_t discount, const CMyString &category,
            int32_t pos, const CMyString &extraInfo)
    : m_bpId(bpId)
    , m_configId(configId)
    , m_bpCode(bpCode)
    , m_initCount(initCount)
    , m_billPoint(billpoint)
    , m_coin(coin)
    , m_extCoin(extCoin)
    , m_category(category)
    , m_extraInfo(extraInfo)
    , m_dailyCount(dailyCount)
    , m_monthlyCount(monthlyCount)
    , m_totalCount(initCount)
{
    // 解析itemInfo
    m_data["items"] = itemInfo;
    m_bpItemData = itemInfo;

    m_data["bpId"] = bpId;
    m_data["configId"] = configId;
    m_data["bpCode"] = bpCode;
    m_data["billPoint"] = (Json::Int64)billpoint;
    m_data["coin"] = (Json::Int64)coin;
    m_data["extraCoin"] = (Json::Int64)extCoin;
    m_data["initCount"] = initCount;
    // m_data["usedCount"] = 0;    // 需要后续重置
    m_data["bpIcon"] = bpIconUrl;
    m_data["bpDescId"] = bpNameId;
    m_data["category"] = m_category;

    // 限制
    m_version.Load(versionRange);
    m_vip.Load(vipRange);
    m_channel.Load(channelRange);
    Json::Value limit;
    limit["versionRange"] = versionRange;    // 版本范围
    limit["channelRange"] = channelRange;    // 渠道范围
    limit["vipRange"]     = vipRange;            // VIP 范围
    limit["dailyCount"]   = dailyCount;        // 每日次数
    limit["monthlyCount"] = monthlyCount;    // 每月次数
    limit["totalCount"]   = initCount;         // 总次数
    m_data["limit"] = limit;
    // 折扣，仅用于显示
    m_data["discount"] = discount;      // 折扣率，*100, 仅显示用
    m_data["pos"] = pos;                // 计费点显示排序
    m_data["extraInfo"] = extraInfo;    // 扩展信息，以json格式显示

    LogDebug("bpId:%d, version:%s, channel:%s, vip:%s", 
            bpId, m_version.toString().c_str(), 
            m_channel.toString().c_str(), m_vip.toString().c_str() );
}

CBillPoint::~CBillPoint()
{
}

bool CBillPoint::toJson(const CMyString &uin, Json::Value &billpoint_array)
{
    Json::Value data = m_data;
    BPDATA_LIST.ToJson(uin, m_bpId, data);
    data["usedCount"] = data["used_stat"]["daily"];
    billpoint_array.append(data);
    return true;
}

bool CBillPoint::IsValid(int64_t version, const CMyString &channel, int32_t vip)
{
    // 检查version
    if (!m_version.Valid(version))
    {
        return false;
    }

    // 检查channel
    if (!m_channel.Valid(channel))
    {
        return false;
    }

    // 检查vip
    if (!m_vip.Valid(vip))
    {
        return false;
    }

#if 0    
    // 检查总次数是否已用完
    int32_t usedCount = BPDATA_LIST.getBillCount(uin, m_bpId);
    if (m_initCount !=-1 && usedCount > m_initCount)
    {
        return false;
    }

    // 获得uin 使用该计费点的次数，当天次数，本月次数
    int32_t dailyUsed = BPDATA_LIST.getBillStat(uin, m_bpId, CPeriodicStat::cycleDay);
    if (m_dailyCount !=-1 && dailyUsed>m_dailyCount)
    {
        return false;
    }

    int32_t monthlyUsed = BPDATA_LIST.getBillStat(uin, m_bpId, CPeriodicStat::cycleWeek);
    if (m_monthlyCount!=-1 && monthlyUsed>m_monthlyCount)
    {
        return false;
    }
#endif

    return true;
}

bool CBillPoint::CanBuy(const CMyString &uin)
{
    // 检查总次数是否已用完
    int32_t usedCount = BPDATA_LIST.getBillCount(uin, m_bpId);
    if (m_initCount !=-1 && usedCount >= m_initCount)
    {
        return false;
    }

    // 获得uin 使用该计费点的次数，当天次数，本月次数
    int32_t dailyUsed = BPDATA_LIST.getBillStat(uin, m_bpId, CPeriodicStat::cycleDay);
    if (m_dailyCount !=-1 && dailyUsed>=m_dailyCount)
    {
        return false;
    }

    int32_t monthlyUsed = BPDATA_LIST.getBillStat(uin, m_bpId, CPeriodicStat::cycleMonth);
    if (m_monthlyCount!=-1 && monthlyUsed>=m_monthlyCount)
    {
        return false;
    }

    return true;

}

//////////////////////////////////////////////////////////////////////
//CBillPointList
CBillPointList::CBillPointList()
{

}

CBillPointList::~CBillPointList()
{

}

CBillPointList& CBillPointList::InstanceGet()
{
    static CBillPointList s_obj;
    return s_obj;
}

int32_t CBillPointList::Load(ISimDB *pDB)
{
    m_vecBillPoint.clear();
    Json::Value billPoint_array;

    pDB->Cmd("SELECT `bpId`,`configId`,`bpCode`,`billPoint`,`coin`,"\
            "`extraCoin`,`extraCount`,`bpIcon`,`bpDescId`,`itemInfo`, "\
            "`versionRange`,`channelRange`,`vipRange`,"\
            "`dailyCount`,`monthlyCount`,`discount`,`category`,`pos`,`extraInfo`,`status` " );
    pDB->Cmd("FROM `t_billpoint` order by `configId`, `billPoint` ");
    while(pDB->More())
    {
        if (pDB->GetInt(19) == 1)
        {
            CBillPoint bp(pDB->GetInt(0),   // billpointId
                    pDB->GetInt(1),     // configId
                    pDB->GetString(2),  // billpointCode
                    pDB->GetInt64(3),   // billpoint
                    pDB->GetInt64(4),   // coin
                    pDB->GetInt64(5),   // extraCoin
                    pDB->GetString(9),  // itemInfo
                    pDB->GetInt(6),     // initCount
                    pDB->GetString(7),  // billpointIconUrl
                    pDB->GetInt(8),     // billpointDescId (langId)
                    pDB->GetString(10),  // version range 
                    pDB->GetString(11), // channel range
                    pDB->GetString(12), // vip range
                    pDB->GetInt(13),    // daily count
                    pDB->GetInt(14),    // monthly count
                    pDB->GetInt(15),    // discount
                    pDB->GetString(16), // category
                    pDB->GetInt(17),    // pos
                    pDB->GetString(18)  // extraInfo
            );
            m_vecBillPoint.push_back(bp);
        }

        Json::Value data;
        data["bpId"] = pDB->GetInt(0);
        data["configId"] = pDB->GetInt(1);
        data["bpCode"] = pDB->GetString(2);
        data["billPoint"] = pDB->GetInt64(3);
        data["coin"] = pDB->GetInt64(4);
        data["extraCoin"] = pDB->GetInt64(5);
        data["extraCount"] = pDB->GetInt(6);
        data["bpIcon"] = pDB->GetString(7);
        data["bpDescId"] = pDB->GetInt(8);
        data["itemInfo"] = pDB->GetString(9);
        data["versionRange"] = pDB->GetString(10);
        data["channelRange"] = pDB->GetString(11);
        data["vipRange"] = pDB->GetString(12);
        data["dailyCount"] = pDB->GetInt(13);
        data["monthlyCount"] = pDB->GetInt(14);
        data["discount"] = pDB->GetInt(15);
        data["category"] = pDB->GetString(16);
        data["pos"] = pDB->GetInt(17);
        data["extraInfo"] = pDB->GetString(18);
        data["status"] = pDB->GetInt(19);

        billPoint_array.append(data);
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_billpoint data failure, error %s", szError);
        return -1;
    }

    if (CONFIG.getProcessWorkerId() == CONFIG.getWorkerId())
    {
        this->SyncBillPoint(billPoint_array);
    }


    return 0;
}

CBillPoint* CBillPointList::Find(int32_t bpId)
{
    CBillPoint *pBillPoint(NULL);
    VEC_BILLPOINT::iterator it(m_vecBillPoint.begin());
    for(; it != m_vecBillPoint.end(); ++it)
    {
        CBillPoint &bp = *it;
        if (bp.getId() == bpId)
        {
            pBillPoint = &bp;
            break;
        }
    }
    return pBillPoint;
}

int32_t CBillPointList::toJson(int32_t configId, const CMyString &uin, int64_t version,
            const CMyString &channelCode, int32_t vipLevel, const CMyString &category,
            Json::Value &billpoint_array)
{
    int32_t cnt(0);
    VEC_BILLPOINT::iterator it(m_vecBillPoint.begin());
    for(; it != m_vecBillPoint.end(); ++it)
    {
        CBillPoint &bp = *it;
        if (bp.getConfigId() != configId)
        {
            continue;
        }
        if (!category.empty())
        {
            if (bp.getCategory() != category)
            {
                continue;
            }
        }
        if (bp.IsValid(version, channelCode, vipLevel))
        {
            bp.toJson(uin, billpoint_array);
            ++cnt;
        }
    }
    return cnt>0;

}

int32_t CBillPointList::Find(int32_t configId, int64_t money, std::vector<CBillPoint*> &vecBillPoint)
{
    VEC_BILLPOINT::iterator it(m_vecBillPoint.begin());
    for(; it != m_vecBillPoint.end(); ++it)
    {
        CBillPoint &bp = *it;
        if (bp.getConfigId() == configId && bp.getBillPoint() == money)
        {
            vecBillPoint.push_back(&bp);
        }
    }
    return static_cast<int32_t>(vecBillPoint.size());
}

#if 0
int32_t CBillPointList::GetCount(int32_t configId, const CMyString &uin)
{
    int32_t cnt(0);
    VEC_BILLPOINT::iterator it(m_vecBillPoint.begin());
    for(; it != m_vecBillPoint.end(); ++it)
    {
        CBillPoint &bp = *it;
        if (bp.getConfigId() == configId && bp.IsValid(uin));
        {
            ++cnt;
        }
    }
    return cnt;
}
#endif

bool CBillPointList::IsValid(int32_t configId, const CMyString &uin, int64_t version, const CMyString &channelCode, int32_t vipLevel)
{
    int32_t cnt(0);
    VEC_BILLPOINT::iterator it(m_vecBillPoint.begin());
    for(; it != m_vecBillPoint.end(); ++it)
    {
        CBillPoint &bp = *it;
        if (bp.getConfigId() != configId)
        {
            continue;
        }
        if (bp.IsValid(version, channelCode, vipLevel))
        {
            ++cnt;
        }
    }
    return cnt>0;
}

CBillPoint* CBillPointList::FindByCode(int32_t configId, const CMyString &bpCode)
{
    VEC_BILLPOINT::iterator it(m_vecBillPoint.begin());
    for(; it != m_vecBillPoint.end(); ++it)
    {
        CBillPoint &bp = *it;
        if (bp.getConfigId()==configId && bp.getBpCode()==bpCode);
        {
            return &bp;
        }
    }
    return NULL;
}

CBillPoint* CBillPointList::FindById(int32_t bpId)
{
    VEC_BILLPOINT::iterator it(m_vecBillPoint.begin());
    for(; it != m_vecBillPoint.end(); ++it)
    {
        CBillPoint &bp = *it;
        if (bp.getId() == bpId);
        {
            return &bp;
        }
    }
    return NULL;
}

void  CBillPointList::SyncBillPoint(Json::Value& array)
{
    if (array.empty())
    {
        return;
    }

    CMyString strUrl;
    CXmlFile& xml = CONFIG.getXml();
    CMyString strSect("main/service/syncbillpoint");
    xml.getString(strSect, "url", strUrl);
    if (strUrl.empty())
    {
        return;
    }

    int32_t nCount = 0;
    CMyString strSQL = "REPLACE INTO `t_billpoint_gs` (`bpId`, `configId`, `bpCode`, `category`, `billPoint`, `coin`, `extraCoin`, \
                        `extraCount`, `itemInfo`, `bpIcon`, `bpDescId`, `versionRange`, `channelRange`, `vipRange`, `dailyCount`, \
                        `monthlyCount`, `discount`, `status`, `pos`, `extraInfo`, `ware_id`) VALUES ";

    Json::Value::iterator iter = array.begin();
    for (; iter != array.end(); ++iter)
    {
        Json::Value& data = *iter;

        int32_t nWareId = 0;
        CMyString strExtraInfo = data["extraInfo"].asString();  
        if (!strExtraInfo.empty())
        {        
            size_t nPos = strExtraInfo.find("ware_id");
            if (nPos != std::string::npos)
            {
                CJsonWrapper extra;
                if (extra.Load(strExtraInfo))
                {
                    nWareId = extra.getInt("ware_id");
                }
            }
        }
        
        if (nCount < 100)
        {
            CMyString strTemp("(%d, %d, '%s', '%s', %ld, %ld, %ld, %d, '%s', '%s', %d, '%s', '%s', '%s', %d, %d, %d, %d, %d, '%s', %d),", 
                                data["bpId"].asInt(),data["configId"].asInt(),data["bpCode"].asString().c_str(),data["category"].asString().c_str(),
                                data["billPoint"].asInt64(),data["coin"].asInt64(),data["extraCoin"].asInt64(), data["extraCount"].asInt(), data["itemInfo"].asString().c_str(),
                                data["bpIcon"].asString().c_str(),data["bpDescId"].asInt(),data["versionRange"].asString().c_str(), data["channelRange"].asString().c_str(),
                                data["vipRange"].asString().c_str(),data["dailyCount"].asInt(),data["monthlyCount"].asInt(),data["discount"].asInt(),
                                data["status"].asInt(),data["pos"].asInt(),data["extraInfo"].asString().c_str(), nWareId);
            strSQL += strTemp;               
            nCount ++;   
        }
        else
        {
            CMyString strData = strSQL.substr(0, strSQL.length() - 1);
            this->SyncBillPointToGamesvr(strUrl, strData);          

            strSQL = "REPLACE INTO `t_billpoint_gs` (`bpId`, `configId`, `bpCode`, `category`, `billPoint`, `coin`, `extraCoin`, \
                        `extraCount`, `itemInfo`, `bpIcon`, `bpDescId`, `versionRange`, `channelRange`, `vipRange`, `dailyCount`, \
                        `monthlyCount`, `discount`, `status`, `pos`, `extraInfo`, `ware_id`) VALUES ";
            nCount = 0;
        }
    }

    if (nCount > 0)
    {
        CMyString strData = strSQL.substr(0, strSQL.length() - 1);
        this->SyncBillPointToGamesvr(strUrl, strData);
    }
}

bool CBillPointList::SyncBillPointToGamesvr(CMyString& strUrl, CMyString& strData)
{
    if (strUrl.empty())
    {
        return true;
    }

    Json::Value data;
    data["data"] = strData;
    CJsonWrapper wr(data);
    CMyString strHttpData = wr.toString();

    CMyString strHeader = "Content-Type: application/json";
    http::VEC_STRING header_list;
    header_list.push_back(strHeader);
    CMyString strResp;
    CMyString strRespHeader;
    http::CHttpClient net;
    
    int ret = net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list, strHttpData.c_str());
    if (ret != 0)
    {
        CMyString strErrNote;
        net.getError(ret, strErrNote);  
        LogFatal("[%s:%s:%d]sync billpoint data to game svr failed, url:%s, data:%s err:%d, msg:%s",
            __FILE__, __func__, __LINE__, strUrl.c_str(), strHttpData.c_str(), ret, strErrNote.c_str());
        return false;
    }

    return true;
}