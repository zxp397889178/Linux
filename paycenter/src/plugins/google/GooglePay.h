/*
 * =====================================================================================
 *
 *       Filename:  GooglePay.h
 *
 *    Description:  谷歌支付充值接口
 *
 *        Version:  1.0
 *        Created:  2020年10月21日 15时14分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once
#include "../PayWrapper.h"
#include "IWebPlugin.h"
#include "PyWrapper.h"

class CPluginGooglePay : public CPayWrapper
{
public:
    CPluginGooglePay();
    virtual ~CPluginGooglePay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t doResp(int32_t code, int32_t result, const CMyString &errNote, 
        const CMyString &uin, const CMyString &orderNo, const CMyString &googleOrderNo, 
        char *szResp, uint32_t &cbResp);
    void doResp(Json::Value &resp, char *szResp, uint32_t &cbResp);

    // 处理google返回的结果
    // return: 1  - 订单已确认
    //         0  - 订单待确认，有可能是失败
    //         -1 - google返回串不是合法的json
    //         -3 - 订单被取消
    //         -5 - google返回的订单订购状态未定义
    int32_t dealGoogleResp(const CMyString &orderNo, const CMyString &resp, CMyString &googleOrderNo, CMyString &regionCode);

    void dealGoogleError(const CMyString &orderNo, const CMyString &resp, CMyString &googleError);
    bool IsDuplicateOrder(const CMyString &gameCode, const CMyString &uin, const CMyString &productId, const CMyString &token);

    void dealPendingAck(void);
    // 处理google延迟确认操作
    // 返回值：0 - 不需要后续操作
    //         1 - 继续下一次循环确认
    int32_t dealGoogleAck(const CMyString &json_str);

private:
    CMyString   m_script;        // python 脚本
    CMyString   m_function;      // python 函数
    // TODO: 必须将 secret.json， 包名等放到web页的配置中去
    CMyString   m_secret;        // 客户端加密文件
    CMyString   m_packageName;   // 包名

    CPyWrapper  m_python;
    CMyString   m_pyhome;
    CMyTimer    m_ackTimer;
};









