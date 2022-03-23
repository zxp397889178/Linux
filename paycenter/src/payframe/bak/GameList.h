/*
 * =====================================================================================
 *
 *       Filename:  GameList.h
 *
 *    Description:  游戏列表定义
 *
 *        Version:  1.0
 *        Created:  2020年07月23日 23时15分48秒
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
#include <vector>

class CGame
{
public:
    CGame(int32_t gameId, const CMyString& gameCode, 
            const CMyString& gameName, const CMyString& account, 
            const CMyString& apiKey, const CMyString& apiNotifyUrl,
            const CMyString& apiTransferUrl, const CMyString& callbackUrl,
            const CMyString& alterData)
        : m_gameId(gameId)
        , m_gameCode(gameCode)
        , m_gameName(gameName)
        , m_account(account)
        , m_apiKey(apiKey)
        , m_apiNotifyUrl(apiNotifyUrl)
        , m_apiTransferUrl(apiTransferUrl)
        , m_callbackUrl(callbackUrl)
    {
        if (!alterData.empty())
        {
            Json::Reader reader;
            reader.parse(alterData.c_str(), m_alterData, false);
        }
    }

    ~CGame()
    {
    }

public:
    int32_t getId(void)               { return m_gameId; }
    CMyString& getCode(void)          { return m_gameCode; }
    CMyString& getName(void)          { return m_gameName; }
    CMyString& getAccount(void)       { return m_account; }
    CMyString& getApiKey(void)        { return m_apiKey; }
    CMyString& getApiNotifyUrl(void)  { return m_apiNotifyUrl; }
    CMyString& getApiTranferUrl(void) { return m_apiTransferUrl; }
    CMyString& getCallbackUrl(void)   { return m_callbackUrl; }
    Json::Value& getAlter(void)       { return m_alterData; }
    void getAlter(CJsonReader &jr)    { jr.Init(m_alterData); }

private:
    int32_t     m_gameId;
    CMyString   m_gameCode;
    CMyString   m_gameName;
    CMyString   m_account;              // 与游服通讯的账号，   (游服提供)
    CMyString   m_apiKey;               // 与游服通讯的key,     (游服提供)
    CMyString   m_apiNotifyUrl;         // 充值通知URL，        (游服提供)
    CMyString   m_apiTransferUrl;       // 提现通知URL，        (游服提供)
    CMyString   m_callbackUrl;          // 充值完成后回退的URL，(游服提供)
    Json::Value m_alterData;            // 警告数据
};

class CGameList
{
public:
    static CGameList& InstanceGet();
    ~CGameList();

    int32_t Load(ISimDB *pDB);

    CGame* Find(int32_t gameId);
    CGame* FindByCode(const CMyString& gameCode);

private:
    CGameList();

private:
    std::vector<CGame>  m_vecGame;
};

#define GAME_LIST        CGameList::InstanceGet()


