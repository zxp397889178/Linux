/*
 * =====================================================================================
 *
 *       Filename:  PayManager.cpp
 *
 *    Description:  支付渠道选择
 *
 *        Version:  1.0
 *        Created:  01/01/2020 01:05:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "PayManager.h"
#include "MyDB.h"
#include "Security.h"
#include "Config.h"
#include "Common.h"
#include <algorithm>
#include "pay_error.h"
#include "BillPointList.h"
#include "WithdrawLimitData.h"
#include "ChannelGroup.h"

CPayManager::CPayManager()
    : m_bNeedSign(false)
    , m_pDB(NULL)
    , m_workerId(0)
{
}

CPayManager::~CPayManager()
{
    SAFE_RELEASE(m_pDB);
}

int32_t CPayManager::Init(int32_t workerId)
{
    m_workerId = workerId;
    CMyString strDbCfg;
    if (!getConfigStr("main/paycenter/db", "main", strDbCfg))
    {
        return PAYFRAME_ERROR(err_ConfigMissingParam);
    }
    if (!getConfigBool("main/paycenter", "sign", m_bNeedSign))
    {
        m_bNeedSign = false;
    }

    int32_t ret = open_mysql(strDbCfg, m_pDB); 
    if (ret < 0)
    {
        return PAYFRAME_ERROR(err_MysqlOpenFailure);
    }
    LogInit("load payframe database succeed.");
    ISimDB *pDB(m_pDB);

    // 获得游服列表信息
    ret = GAME_LIST.Load(pDB);
    if (ret != 0)
    {
        LogDebug("load t_game return %d", ret);
        return -1;
    }
    LogWarn("load t_game succeed.");
    LogInit("load game list succeed.");

    // 获得SDK列表信息
    ret = SDK_LIST.Load(pDB);
    if (ret != 0)
    {
        return -3;
    }
    LogInit("load sdk list succeed.");

    // 获得渠道列表信息
    ret = CHANNEL_LIST.Load(pDB);
    if (ret != 0)
    {
        return -5;
    }
    LogInit("load pay channel list succeed.");

    //获得计费点列表
    ret = BILLPOINT_LIST.Load(pDB);
    if (ret != 0)
    {
        return -7;
    }
    LogInit("load pay billpoint list succeed.");

    // 获得每个UIN计费点使用情况列表
    ret = BPDATA_LIST.Load(pDB);
    if (ret != 0)
    {
        return -9;
    }
    LogInit("load user billpoint used data succeed.");

    // 获得SDK子渠道列表信息
    ret = SDKCHANNEL_LIST.Load(pDB);
    if (ret != 0)
    {
        return -11;
    }
    LogInit("load sdk channel list succeed.");

    // 获得VIP列表的信息
    ret = VIP_LIST.Load(pDB);
    if (ret != 0)
    {
        return -13;
    }
    LogInit("load vip list succeed.");

    ret = DICT_CLIENTTYPE.Load(pDB);
    if (ret != 0)
    {
        return -15;
    }
    LogInit("load client type data succeed.");

    ret = DICT_PAYSTYLE.Load(pDB);
    if (ret != 0)
    {
        return -16;
    }
    LogInit("load pay style data succeed.");

    ret = WITHDRAWLIMIT_DATA.Load(pDB);
    if (ret != 0)
    {
        return -17;
    }
    LogInit("load withdraw limit data succeed.");

    ret = CHANNELGROUP_LIST.Load(pDB);
    if (ret != 0)
    {
        return -18;
    }
    LogInit("load channel group data succeed.");

    return 0;
}

CSdkChannel* CPayManager::selectChannel(std::vector<CSdkChannel*> &vecChannel, int32_t totalWeight)
{
    // TODO: 此段代码需要校验下，怀疑不能达到预期
    int32_t rand_val = (int32_t)RandGet(0, (uint32_t)totalWeight, true);
    std::vector<CSdkChannel*>::iterator it(vecChannel.begin());
    for(; it != vecChannel.end(); ++it)
    {
        CSdkChannel *pc = *it;
        int32_t nWeight = pc->getWeight();
        if (rand_val <= nWeight)
        {
            return pc;
            break;
        }
        rand_val -= nWeight;
    }
    return NULL;
}

void CPayManager::OnPayFailure(int32_t code, const char *msg, char *szResp, uint32_t &cbResp)
{
    Json::Value resp;
    resp["code"] = code;
    resp["message"] = CMyString("error :%d, message %s", code, msg);
    Json::Value data;
    data["tradeNo"] = "";
    data["url"] = "";
    data["sdkCode"] = "";
    data["payChannel"] = "";
    data["payMethod"] = "";
    resp["data"] = data;
    CJsonWrapper writer(resp);
    CMyString strResp = writer.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

int32_t CPayManager::VerifySign(http::CHttpRequest &req, char *szResp, uint32_t &cbResp)
{
    if (!m_bNeedSign)
    {
        return 0;
    }

    int32_t err(-1);
    // 获得游戏的apiKey
    CMyString gameCode = req.getParam("gameCode");
    CGame *pGame = GAME_LIST.FindByCode(gameCode);
    if (NULL == pGame)
    {
        err = PAYFRAME_ERROR(err_NotValidGame);
        CMyString strErr("unknown game %s", gameCode.c_str());
        LogErr(strErr.c_str());
        this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }
    CMyString strKey = pGame->getApiKey();

    // 检查sign是否正确
    std::vector<CMyString> vecIgnore;
    vecIgnore.push_back("sign");
    http::CHttpRespMaker reqVerify;
    req.BuildRespMaker_Except(vecIgnore, reqVerify);
    CMyString str1 = reqVerify.http_str();
    CMyString str1New;
    CHttpWrapper::urlDecode(str1, str1New);
    LogDebug("str1=> %s", str1.c_str());
    LogDebug("str1New=> %s", str1New.c_str());
    CMyString str2("%s&merchantKey=%s", str1New.c_str(), strKey.c_str());
    CMyString strMD5 = CMD5Wrapper::encode(str2.c_str());
    CMyString strSign = req.getParam("sign");
    if (strcasecmp(strMD5.c_str(), strSign.c_str()))  // 不考虑大小写
    {
        err = PAYFRAME_ERROR(err_SignMismatch);
        LogErr("origin text[%s]", str2.c_str());
        CMyString strErr("sign mismatch, req:sign[%s], verify:md5[%s]", strSign.c_str(), strMD5.c_str());
        LogErr(strErr.c_str());
        this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }

    return 0;
}

#define HTTP_REQ_INT(var)   int32_t var = req.getParamInt(#var)
#define HTTP_REQ_INT64(var) int64_t var = req.getParamInt64(#var)
#define HTTP_REQ_STR(var)   CMyString var = req.getParam(#var)
int32_t CPayManager::GetChannel(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, IClient *pClient)
{
    CMyString strErr;
    int32_t err(PAYFRAME_ERROR(err_SystemFault));

    // 检查是否缺少必要的参数
    std::vector<CMyString> vecParam;
    vecParam.push_back("uin");
    vecParam.push_back("gameCode");
    vecParam.push_back("clientType");
    if (m_bNeedSign)
    {
        vecParam.push_back("sign");
    }
    if (!req.ValidParam(vecParam, strErr))
    {
        err = PAYFRAME_ERROR(err_MissingParameters);
        LogErr(strErr.c_str());
        this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }

    // 验签
    int32_t ret = this->VerifySign(req, szResp, cbResp);
    if (ret != 0)
    {
        return ret;
    }

    // 构建返回结果
    CMyString strUin = req.getParam("uin");
    //CMyString gameCode = req.getParam("gameCode");
    //CMyString clientType = req.getParam("clientType");
    HTTP_REQ_STR(gameCode);
    HTTP_REQ_STR(clientType);
    HTTP_REQ_INT64(version);
    HTTP_REQ_STR(channelCode);
    HTTP_REQ_INT(vipLevel);
    HTTP_REQ_STR(category);
    HTTP_REQ_INT(configId);

    CGame* pGame = GAME_LIST.FindByCode(gameCode);

    Json::Value resp;
    resp["code"] = 0;
    resp["message"] = "OK";
    resp["gameCode"] = gameCode;
    resp["uin"] = strUin;
    resp["exchangeRate"] = (pGame==NULL) ? 1 : pGame->getExchangeRate();

    CChannelGroup* pChannelGroup = CHANNELGROUP_LIST.FindByChannel(channelCode);
    if (pChannelGroup != NULL)
    {
        LogWarn("[%s:%s:%d] channelCode=%s belong to channelGroup:%d groupName:%s", __FILE__, __func__, __LINE__, 
                channelCode.c_str(), pChannelGroup->getGroupId(), pChannelGroup->getGroupName().c_str());
        CHANNEL_LIST.toJson(gameCode, strUin, clientType, version, channelCode, vipLevel, category, configId, pChannelGroup->getGroupId(), resp);
    }
    else
    {
        LogWarn("[%s:%s:%d] channelCode=%s not has support channel group", __FILE__, __func__, __LINE__, channelCode.c_str());
        CHANNEL_LIST.toJson(gameCode, strUin, clientType, version, channelCode, vipLevel, category, configId, resp);
    }

    CJsonWrapper wr(resp);
    CMyString strResp = wr.toString();

    // 发送到前端
    if (strResp.size() < cbResp)
    {
        safe_strcpy(szResp, cbResp, strResp.c_str());
        cbResp = strlen(szResp);
        return 0;
    }

    // 超长，分包发送，不再通过上层应用分发
    int32_t sendSize = send_chunked_data(pClient,
            (byte_t*)strResp.c_str(), strResp.size(), 
            req.getVersion(), 200, "OK", "application/json", "payframe/2");
    LogDebug("query getChannel, send %d bytes.", sendSize);
    cbResp = 0;
    return 0;
}

int32_t CPayManager::GetWealthStat(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, IClient *pClient)
{
    CMyString strErr;
    int32_t err(PAYFRAME_ERROR(err_SystemFault));

    // 检查是否缺少必要的参数
    std::vector<CMyString> vecParam;
    vecParam.push_back("uin");
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    if (m_bNeedSign)
    {
        vecParam.push_back("sign");
    }
    if (!req.ValidParam(vecParam, strErr))
    {
        err = PAYFRAME_ERROR(err_MissingParameters);
        LogErr(strErr.c_str());
        this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }

    // 验签
    int32_t ret = this->VerifySign(req, szResp, cbResp);
    if (ret != 0)
    {
        return ret;
    }

    // 构建返回结果
    CMyString strUin = req.getParam("uin");
    HTTP_REQ_STR(gameCode);
    Json::Value resp;
    WITHDRAWLIMIT_DATA.toJsonEx(m_pDB, gameCode,strUin, resp);
    CJsonWrapper writer;
    writer.Load(resp);
    CMyString strResp = writer.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
    return 0;

}

int32_t CPayManager::QueryCgi(http::CHttpRequest &req, 
        char *szResp, uint32_t &cbResp, CMyString &strCgiNew)
{
    CMyString strErr;
    int32_t err(PAYFRAME_ERROR(err_SystemFault));

    // 检查是否缺少必要的参数
    std::vector<CMyString> vecParam;
    vecParam.push_back("configId");
    vecParam.push_back("bpId");     // 计费点ID必选
    vecParam.push_back("gameCode");
    vecParam.push_back("uin");
    vecParam.push_back("billNo");
    vecParam.push_back("amount");
    vecParam.push_back("productId");
    vecParam.push_back("playerIp");
    vecParam.push_back("clientType");
    // 以下个字段为新增的，为统计方便之用
    vecParam.push_back("userName");
    vecParam.push_back("channelId");
    vecParam.push_back("channelCode");
    vecParam.push_back("channelName");
    vecParam.push_back("createTime");
    if (m_bNeedSign)
    {
        vecParam.push_back("sign");
    }
    if (!req.ValidParam(vecParam, strErr))
    {
        err = PAYFRAME_ERROR(err_MissingParameters);
        LogErr(strErr.c_str());
        this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }

    // 验证签名
    int32_t ret = this->VerifySign(req, szResp, cbResp);
    if (ret != 0)
    {
        return ret;
    }

    CMyString uin = req.getParam("uin");
    int32_t configId = req.getParamInt("configId");
    //CMyString gameCode = req.getParam("gameCode");
    int64_t money = req.getParamInt64("amount");
    CMyString clientType = req.getParam("clientType");
    //int32_t clientFlag = DICT_CLIENTTYPE.getTypeId(clientType);
    LogDebug("DEBUG>uin:%s, configId:%d, money:%ld", uin.c_str(), configId, money);

    // 获得configId 对应的支付渠道列表
    std::vector<CSdkChannel*> vecSdkChannel;
    int32_t totalWeight(0);
    int32_t cnt = SDKCHANNEL_LIST.GetChannel(configId, money, clientType, vecSdkChannel, totalWeight);
    if (cnt <= 0)
    if (vecSdkChannel.empty())
    {
        err = PAYFRAME_ERROR(err_NotAvaiablePayChannel);
        strErr.Format("not avaiable pay channel, money :%.2lf", money/100.0);
        LogFatal(strErr.c_str());
        this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }
    LogDebug("DBGPAY>have %lu channel to choose.", vecSdkChannel.size());

    // 从可用的侯选中随机一个支付渠道
    CSdkChannel *pChannel = this->selectChannel(vecSdkChannel, totalWeight);

    // 获得参数
    CSdk *pSdk = pChannel->getSdk();
    if (pSdk==NULL)
    {
        LogErr("sdk-channel %s has not sdk.", pChannel->getName().c_str());
        err = PAYFRAME_ERROR(err_GetSdkChannelFailure);
        strErr.Format("system configure failure, please contact Administrator");
        this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }
    CPayChannel *pPayChannel = pChannel->getPayChannel();
    if (pPayChannel == NULL)
    {
        LogErr("sdk-channel %s has not payChannel.", pChannel->getName().c_str());
        err = PAYFRAME_ERROR(err_GetSdkChannelFailure);
        strErr.Format("system configure failure, please contact Administrator");
        this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }
    strCgiNew = pSdk->getPayCgi();
    pChannel->IncCall();

    // 获得计费点ID
    int32_t bpId = req.getParamInt("bpId");     // 若未指定，则为0
    if (bpId == 0)
    {
        bpId = this->selectBillpoint(configId, money, uin);
        if (-1 == bpId)
        {
            // 不需要计费点
            bpId = 0;
        }
    }
    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    if (NULL != pBP)
    {
        if (!pBP->CanBuy(uin))
        {
            LogErr("purchase times of billpoint %d for user %s up to the limit", bpId, uin.c_str());
            err = PAYFRAME_ERROR(err_PurchaseTimeUpToLimit);
            strErr.Format("Purchase times of billpoint %d up to the limit", bpId);
            this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
            return err;
        }
    }

    // 增加标识，用于识别渠道数据
    int32_t channelId = pChannel->getId();
    CMyString payStyle = pPayChannel->getPayStyle();
    req.setParam("paycenter_channelId", channelId);
    req.setParam("paycenter_bpId", bpId);
    req.setParam("paycenter_method", payStyle);

    CMyString txt[10];
    LogDebug("DBGPAY>choose pay channel=> uin [%s] configId[%s] billpointId[%s] sdk[%s] channelId[%s] method[%s] bank_code[%s] new cgi[%s]",
            uin.c_str(),
            color_int(_color_green, configId, txt[0]), 
            color_int(_color_green, bpId, txt[6]), 
            color_text(_color_red, pSdk->getCode().c_str(), txt[2]),
            color_int(_color_green, channelId, txt[1]),
            color_text(_color_red, payStyle.c_str(), txt[3]),
            color_text(_color_red, pChannel->getBankCode().c_str(), txt[4]),
            color_text(_color_green, strCgiNew.c_str(), txt[5])
            );
    return PAYFRAME_ERROR(err_OK);
}

int32_t CPayManager::QuerySignCgi(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, CMyString &strCgiNew)
{
    CMyString strErr;
    int32_t err(PAYFRAME_ERROR(err_SystemFault));

    if (!m_pDB)
    {
        return err;
    }

    int32_t nConfigId = 0;
    CMyString strOrderNo = req.getParam("orderNo");
    if (strOrderNo.empty())
    {
        strOrderNo = req.getParam("cpOrderNumber");
    }
    if (strOrderNo.empty())
    {
        strOrderNo = req.getParam("trade_no");
    }
    if (strOrderNo.empty())
    {
        strOrderNo = req.getParam("attach");
    }
    if (strOrderNo.empty())
    {
        bool bRet = req.getParam("configId", nConfigId);
        if (!bRet)
        {
            err = PAYFRAME_ERROR(err_PayOrderNotFound);
            strErr.Format("not found order number.");
            LogErr("[%s:%s:%d] not found order number.", __FILE__, __func__, __LINE__);
            this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
            return err;
        }
    }

    if (strOrderNo.empty() && nConfigId == 0)
    {
        err = PAYFRAME_ERROR(err_MissingParameters);
        LogErr("[%s:%s:%d] missing parameters.", __FILE__, __func__, __LINE__);
        strErr.Format("missing parameters.");
        this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }

    if (!strOrderNo.empty())
    {
        bool bFind(false);
        int32_t nChannelId = 0;
        m_pDB->Cmd("call pQueryOrderInfo_v3('%s')", strOrderNo.c_str());
        while(m_pDB->More())
        {
            bFind = true;
            nChannelId = m_pDB->GetInt(7);
        }

        if (!bFind)
        {
            err = PAYFRAME_ERROR(err_PayOrderDataNotFound);
            LogErr("[%s:%s:%d] order %s not found.", __FILE__, __func__, __LINE__, strOrderNo.c_str());
            strErr.Format("order %s not found.", strOrderNo.c_str());
            this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
            return err;
        }

        CSdkChannel *pChannel = SDKCHANNEL_LIST.Find(nChannelId);
        if (!pChannel)
        {
            err = PAYFRAME_ERROR(err_InvalidChannelId);
            LogErr("[%s:%s:%d] channel id %d is invalid.", __FILE__, __func__, __LINE__, nChannelId);
            strErr.Format("channel id %d is invalid.", nChannelId);
            this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
            return err;
        }

        // 获得参数
        CSdk *pSdk = pChannel->getSdk();
        if (pSdk==NULL)
        {
            LogErr("sdk-channel %s has not sdk.", pChannel->getName().c_str());
            err = PAYFRAME_ERROR(err_GetSdkChannelFailure);
            strErr.Format("system configure failure, please contact Administrator");
            this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
            return err;
        }
        strCgiNew = pSdk->getSignCgi();
        pChannel->IncCall();
    }
    else if (nConfigId > 0)
    {
        // 获得configId 对应的支付渠道列表
        std::vector<CSdkChannel*> vecSdkChannel;
        SDKCHANNEL_LIST.GetChannel(nConfigId, vecSdkChannel);
        if (vecSdkChannel.empty())
        {
            err = PAYFRAME_ERROR(err_NotAvaiablePayChannel);
            strErr.Format("not avaiable pay channel");
            LogErr(strErr.c_str());
            this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
            return err;
        }
        LogDebug("DBGPAY>have %lu channel to choose.", vecSdkChannel.size());

        // 从可用的侯选中随机一个支付渠道
        CSdkChannel* pChannel = vecSdkChannel[0];
        if (NULL == pChannel)
        {
            err = PAYFRAME_ERROR(err_NotAvaiablePayChannel);
            strErr.Format("channel is not avaiable in system");
            LogErr("channelId is not avaiable in system.");
            this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
            return err;
        }

        // 获得参数
        CSdk *pSdk = pChannel->getSdk();
        if (pSdk==NULL)
        {
            LogErr("sdk-channel %s has not sdk.", pChannel->getName().c_str());
            err = PAYFRAME_ERROR(err_GetSdkChannelFailure);
            strErr.Format("system configure failure, please contact Administrator");
            this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
            return err;
        }
        strCgiNew = pSdk->getSignCgi();
        pChannel->IncCall();
    }
    else
    {
        err = PAYFRAME_ERROR(err_MissingParameters);
        return err;
    }

    return PAYFRAME_ERROR(err_OK);
}

int32_t CPayManager::QueryVerifyCgi(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, CMyString &strCgiNew)
{
    CMyString strErr;
    int32_t err(PAYFRAME_ERROR(err_SystemFault));

    if (!m_pDB)
    {
        return err;
    }

    bool bFind(false);
    CMyString strOrderNo = req.getParam("orderNo");
    int32_t nChannelId = 0;
    m_pDB->Cmd("call pQueryOrderInfo_v3('%s')", strOrderNo.c_str());
    while(m_pDB->More())
    {
        bFind = true;
        nChannelId = m_pDB->GetInt(7);
    }

    if (!bFind)
    {
        err = PAYFRAME_ERROR(err_PayOrderDataNotFound);
        LogErr("[%s:%s:%d] order %s not found.", __FILE__, __func__, __LINE__, strOrderNo.c_str());
        strErr.Format("order %s not found.", strOrderNo.c_str());
        this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }

    CSdkChannel *pChannel = SDKCHANNEL_LIST.Find(nChannelId);
    if (!pChannel)
    {
        err = PAYFRAME_ERROR(err_InvalidChannelId);
        LogErr("[%s:%s:%d] channel id %d is invalid.", __FILE__, __func__, __LINE__, nChannelId);
        strErr.Format("channel id %d is invalid.", nChannelId);
        this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }

    // 获得参数
    CSdk *pSdk = pChannel->getSdk();
    if (pSdk==NULL)
    {
        LogErr("sdk-channel %s has not sdk.", pChannel->getName().c_str());
        err = PAYFRAME_ERROR(err_GetSdkChannelFailure);
        strErr.Format("system configure failure, please contact Administrator");
        this->OnPayFailure(err, strErr.c_str(), szResp, cbResp);
        return err;
    }
    strCgiNew = pSdk->getVerifyCgi();
    pChannel->IncCall();

    return PAYFRAME_ERROR(err_OK);
}

int32_t CPayManager::selectBillpoint(int32_t configId, int64_t money, const CMyString &uin)
{
    std::vector<CBillPoint*> vecBillpoint;
    int32_t ret = BILLPOINT_LIST.Find(configId, money, vecBillpoint);
    if (ret <= 0)
    {
        // 无合适的计费点
        return 0;
    }

    int32_t result(-1);
    // 从中选择一个可用的计费用
    std::vector<CBillPoint*>::iterator it(vecBillpoint.begin());
    for(; it != vecBillpoint.end(); ++it)
    {
        CBillPoint *pBP = *it;
        int32_t bpId = pBP->getId();
        int32_t usedCount = BPDATA_LIST.getBillCount(uin, bpId);
        int32_t leftCount = pBP->getInitCount() - usedCount;
        if (leftCount > 0)
        {
            result = bpId;
            break;
        }
    }

/*    
    // 无优惠，选第一个计费点
    if (-1 == result)
    {
        CBillPoint *pBP = vecBillPoint[0];
        result = pDB->getId();
    }
*/

    return result;
}

