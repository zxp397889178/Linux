/*
 * =====================================================================================
 *
 *       Filename:  PayBase.h
 *
 *    Description:  支付SDK封装v2.0
 *
 *        Version:  1.0
 *        Created:  2021年06月09日 16时25分50秒
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
#include "MyNet.h"
#include "MyDB.h"
#include "MyUtility.h"
#include <map>
#include <queue>

enum DepositStyle
{
    depositRequest = 1,         // 采用请求订单的方式,如三方支付等
    depositVerify,              // 采用订单token验证方式，如google、Apple
};
enum HttpRequestStyle
{
    httpRequestSync = 1,        // HTTP 同步请求
    httpRequestASync,           // HTTP 异步请求
};
enum HttpRequestMethod
{
    httpUsePost = 0,            // 采用post方法
    httpUseGet,                 // 采用get方法
};

enum PayBaseError
{
    err_PayBaseCommon = 1100,
    
    err_depositMissParameter,           // 缺少必要的参数
    err_depositParameterFault,          // 前端传过来的参数异常
    err_depositSystemCorrupted,         // 系统出现损坏
    err_depositCreateOrderFailure,      // 存入DB出错
    err_depositHostFail,                // 代收服务器失败，有可能发送过去，或者超时
    err_depositRequestDataCorrupted,    // 代收服务器返回的数据解析失败，有可能是出错
    err_depositOrderUpdateFailure,      // 支付订单DB更新失败
    err_depositVerifySignMismatch,      // 支付订单校验时，验签失败
    err_depositVerifyDuplicate,         // 支付订单校验时，订单重复

    err_depositCallbackDataCorrupted,   // 支付回调数据损坏
    err_depositDataLost,                // 订单数据非法，有可能是订单不匹配
    err_depositCallbackIpInvalid,       // 回调IP非法
    err_depositCallbackSignMismatch,    // 回调签名不合法
    err_depositDeliverHostFail,         // 发货的服务器失败，有可能是超时
    err_depositDeliverGoodsFail,        // 服务器发货失败
    err_depositOrderNotFound,           // 充值订单未发现
    err_depositOrderQueryFail,          // 查询充值订单错误，有可能是DB错误

    err_creditMissParameter,            // 缺少必要的参数 
    err_creditParameterFault,           // 前端传过来的参数异常
    err_creditSignMismatch,             // 提现订单的签名不合法
    err_creditIpInvalid,                // 提现的ip 无效
    err_creditTokenExpired,             // 提现订单的token超时，或者不存在
    err_creditRequestCorrupted,         // 提现请求数据已损坏
    err_creditTokenMismatch,            // 提现订单的token不匹配
    err_creditHostFail,                 // 提现服务器出错
    err_creditRequestFail,              // 提现结果出错
    err_creditOrderUpdateFailure,       // 提现订单更新出错

    err_creditCallbackFault,            // 提现回调结果错误
    err_creditOrderExpired,             // 提现订单超时
    err_creditOrderCorrupted,           // 提现订单数据损坏
    err_creditSystemFault,              // 提现系统出错，数据不匹配
    err_creditCallbackIpInvalid,        // 回调IP不合法
    err_creditCallbackSignMismatch,     // 提现回调验签失败

    err_queryorderMissParameter,        // 查询订单缺失参数
    err_queryorderParameterFault,       // 系统出错了
    err_queryorderSignMismatch,         // 查询的验签不匹配
    err_queryorderHostFail,
    err_queryorderResponeDataCorrupted,
    err_depositEnabled,                 // 充值功能禁用
    err_creditEnabled,                  // 提现功能禁用
    err_creditOverLimit,                // 提现金额超过限制
    err_depositBillPointCannotBuy,      // 充值计费点不可购买（购买次数限制）
    err_depositDeliverGoodsSuccess,     // 服务器已发货完成
};

enum Alarm_Level
{
    Alarm_Fatal = 1,    //非常严重
    Alarm_Err = 2,      //严重
    Alarm_General = 3,  //一般
    Alarm_Slight = 4,   //轻微
};

class IClient;
class CSdk;
class CGame;
class CSdkChannel;
class CPayBase
{
protected:
    enum DepositStatus
    {
        deposit_CreateOrder       = 0,  // 1 - 创建订单
        deposit_MoneyArrival      = 1,  // 3 - 金额到账
        deposit_MoneyFail         = 2,  // 3 - 金额未到账
        deposit_UserCancel        = 3,  // 3 - 支付取消
        deposit_Refund            = 4,  // 5 - 退款（TODO)
        deposit_DeliverSucceed    = 5,  // 4 - 发货完成
        deposit_RequestSucceed    = 6,  // 2 - 请求成功
        deposit_RequestFail       = 7,  // 2 - 请求失败
        deposit_DeliverHostFail   = 8,  // 4 - 发货服务器失败 (REDO)
        deposit_DeliverFail       = 9,  // 4 - 发货失败（TODO）
        deposit_DeliverManual     = 10, // 5 - 发货失败，人工补单（TODO)
        deposit_UserPayTimeout    = 11, // 3 - 支付超时
    };
    enum CreditStatus
    {
        credit_Pending              = 1,    // 1- 待审核
        credit_AuditReject          = 2,    // 2- 审核拒绝
        credit_AuditRejectAndFrozon = 3,    // 2- 审核拒绝并金额冻结
        credit_AuditPass            = 4,    // 2- 审核成功
        credit_LoanBySDK            = 5,    // 3- 放款审核成功-SDK放款
        credit_LoanManual           = 6,    // 3- 放款审核成功-人工放款
        credit_LoanReject           = 7,    // 3- 放款审核拒绝
        credit_LoanRejectAndFrozon  = 8,    // 3- 放款审核拒绝并冻结资金
        credit_SdkLoanSucceed       = 10,   // 4- 放款成功（SDK放款）
        credit_SdkLoanReject        = 11,   // 4- 放款被驳回（SDK放款）
        credit_SdkLoanFail          = 12,   // 4- 放款失败（SDK放款）
        credit_SdkLoanFault         = 13,   // 4- 放款订单异常（SDK放款）
    };

    // 游服充值发货完毕后返回： {"code": gameDepositStatus }
    // 依据以下值做不同的处理
    enum GameDepositStatus
    {
        deliver_Success             = 4,    // 充值成功
        deliver_Failure             = 7,    // 充值失败，需要重发
        deliver_OrderFinished       = 11,   // 订单已完成，更改状态
        deliver_OrderNotExist       = 12,   // 订单不存在，更改状态
        deliver_SystemFault         = 13,   // 系统错误，需要重发
        deliver_FeeMotMatch         = 14,   // 订单金额不匹配，更改状态
    };
    // 游服代付状态，CreditStatus需要转换成以下状态向游服发通知
    enum GameWithdrawStatus
    {
        withdraw_LoanManual      = 1,   // 成功-人工放款
        withdraw_LoanBySDK       = 2,   // 成功-SDK放款
        withdraw_RejectAndFrozen = 8,   // 失败-冻结资金
        withdraw_RejectAndRefund = 9,   // 失败-返还资金
    };
public:
    CPayBase(const CMyString &sdkFlag, enum DepositStyle depositStyle, enum HttpRequestStyle httpRequestStyle);
    virtual ~CPayBase();
    virtual int32_t Init(const CMyString &xmlSect);
    virtual void Maintance(void);

    void setCreditRequestMethod(enum HttpRequestMethod method) { m_creditRequestMethod = method; }
protected:
// ================================== 代收功能的虚函数 ==================================
    /* 向三方平台请求支付订单时构建数据请求
     * 参数: [IN]  req          前端的请求
             [IN]  pGame        游服指针，已校验过空指针
             [IN]  pSdk         SDK指针，已校验过空指针
             [IN]  pChannel     SDK渠道指针，已校验过空指针
             [IN]  orderNo      订单号
             [OUT] url          返回订单支付的URL
             [OUT] contentType  httpData的数据类型，如 application/json text/plain
             [OUT] httpData     需要向三方平台发送的数据
     */
    virtual void doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, 
            const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData) = 0;

    /* 向三方平台发起HTTP请求,若子类未重构，则由父类实现来处理
     * 参数: [IN]   net             CHttpClient对象
     *       [IN]   strUrl          URL
     *       [IN]   header_list     HTTP头
     *       [IN]   httpData        HTTP 数据
     *       [OUT]  strResp         返回结果
     *       [OUT]  strRespHeader   返回结果头部
     *  返回值：HTTP请求的返回结果
     */
    virtual int32_t doDepositHttpRequest(http::CHttpClient &net, const CMyString &strUrl, http::VEC_STRING &header_list, const CMyString &httpData, CMyString &strResp, CMyString &strRespHeader);

    /* 解析HTTP 代收请求返回的数据，并解析出depositUrlData 和 depositOrderNo
     * 如果出错，则errCode 和 errNote有值
     * 参数: [IN]   pGame           游服指针，已校验过空指针
     *       [IN]   respHeader      三方平台返回的HTTP 头
     *       [IN]   respData        三方平台返回的HTTP 数据
     *       [OUT]  depositUrlData  代收的链接url或者数据
     *       [OUT]  depositOrderNo  代收的订单号，有可能为N/A
     *       [OUT]  errNote         出错时的错误说明
     * 返回值：true-代收请求成功，
     *        false-代收请求失败
     */
    virtual bool doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, 
            CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote) = 0;


    /** callback模式下，解析充值回调的数据，解析出重要参数
     *  参数: [IN]  req             http 请求数据
     *        [OUT] orderNo         支付中心的订单号
     *        [OUT] platformOrderNo 三方平台的订单号
     *        [OUT] fee             请求充值的金额
     *        [OUT] realFee         三方平台实际到账金额
     *        [OUT] tax             手续费
     *        [OUT] depositedOK     三方平台的支付结果,TRUE成功
     *  返回值：true表示订单数据解析正确， false表示解析失败
     */
    virtual bool doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo, 
            CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &depositOK) { return false; }
    /* callback模式下，校验三方平台返回的回调数据签名是否匹配
     * 参数：[IN]  req      http请求的数据
     *       [IN]  pSdk     订单对应的SDK指针
     *       [OUT] errNote  错误说明
     * 返回值：true表示验签成功，false表示验签失败
     */
    virtual bool validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote) { return false; }

    /* callback模式下，充值回调结果的返回值
     * 参数：[IN]  bSucceed       是否成功
     *       [IN]  errCode        错误码
     *       [OUT] ext_headers    扩展的HTTP返回头, 如 Content-Type等
     *       [OUT] szResp         返回内容
     *       [OUT] cbResp         返回的内容长度
     */
    virtual void doDepositCallbackResp(bool bSucceed, int32_t errCode, 
            std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
    {
        ext_headers["Content-Type"] = "application/json";
        safe_strcpy(szResp, cbResp, "{\"code\":0,\"message\":\"OK\"}");
        cbResp = strlen(szResp);
    }

    /* callback模式下，处理三方回调先于请求回来的情况
     * 参数：[IN]  req      http请求的数据
     *       [IN]  pSdk     订单对应的SDK指针
     *       [OUT] errNote  错误说明
     * 返回值：true表示验签成功，false表示验签失败
     */
    virtual bool delayedDealDepositCallback(http::CHttpRequest& req, const CMyString& orderNo) { return false; }

    /** verify模式下，必须的参数，每个调用自行处理
     *  参数：[IN/OUT] vecParami    必不可少的参数，已有 gameCode, uin, orderNo, sign
     */
    virtual void addDepositVerifyNeededParam(std::vector<CMyString> &vecParam) {  }

    /* verify模式下，检查订单是否有重复，每个调用自行处理
     * 参数： req       HTTP请求，必不可少的参数有 gameCode, orderNo, sign, 其他的每个接口自行定义
     * 返回值： true    未发现重复订单
     *          false   发现重复订单
     */
    virtual bool checkDepositVerifyDuplicate(http::CHttpRequest &req) { return true; }

    /* verify模式下，把相关需要发送到三方平台验证的数据保存起来，在Maintance中调用
     * 参数： [IN]  pGame       游服指针，已做安全性检查
     *        [IN]  pSdk        SDK指针，已做安全性检查
     *        [IN]  req         http请求数据
     *        [IN]  data        充值的订单数据，包含以下字段
     *              ==========  ==================================================
     *              payId       订单序号，t_payorder表中的唯一ID
     *              uin         用户唯一ID
     *              gameCode    游戏编码
     *              orderNo     订单号
     *              billNo      游戏服务器的订单ID
     *              productId   产品ID，前端透传过来的
     *              channelId   支付渠道ID,t_channel中的ID，也称为configId
     *              method      支付方式编码，dict_paystyle中的code
     *              billpoint   计费点ID
     *              fee         充值金额，以分计
     */
    virtual void storeDepositVerifyData(CGame *pGame, CSdk *pSdk, http::CHttpRequest &req, Json::Value &data) {  }

    /* verify模式下，获得在 storeDepositVerifyData中存储的数据，逐条处理
     * 参数：[OUT] req_data    须返回的订单相关数据，必须包含以下字段：
     *                  => gameCode
     *                  => uin
     *                  => payId  (orderId)
     *                  => orderNo
     *                  => billpoint (bpId, 无则填0)
     *                  => fee
     *       [OUT] depositOK   充值操作结果：true表示金额到账，false表示充值未到账  
     *       [OUT] platformOrderNo  第三方平台返回的订单ID
     *       [OUT] realFee     真实到账的金额
     *       [OUT] regionCode  充值的地区（google有返回）
     *  返回值： -1 出错，在虚函数中要记录错误信息
     *            0 没有待验证的订单
     *            1 有待验证的订单，并处理成功
     *  备注： 每次只处理一条待验证的充值订单，不要做循环操作
     */
    virtual int32_t verifyDepositRequest(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode) { return 0; }
// ================================== 代付功能的虚函数 ==================================
    /* 构建代付请求的数据
     * 参数: [IN]  req          json内存数据 
     *       [IN]  pGame        CGame对象指针
     *       [IN]  pSdk         CSdk对象指针
     *       [OUT] url          代付需要调用的第三方URL
     *       [OUT] conentType   httpData的类型，如 application/json 等
     *       [OUT] httpData     向三方平台发送的代付请求数据
     */
    virtual void doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, 
            CMyString& url, CMyString &contentType, CMyString& httpData) = 0;
            
    /* 向三方平台发起HTTP请求,若子类未重构，则由父类实现来处理
     * 参数: [IN]   net             CHttpClient对象
     *       [IN]   strUrl          URL
     *       [IN]   header_list     HTTP头
     *       [IN]   httpData        HTTP 数据
     *       [OUT]  strResp         返回结果
     *       [OUT]  strRespHeader   返回结果头部
     *  返回值：HTTP请求的返回结果
     */
    virtual int32_t doCreditHttpRequest(http::CHttpClient &net, const CMyString &strUrl, http::VEC_STRING &header_list, const CMyString &httpData, CMyString &strResp, CMyString &strRespHeader);

    /* 解析代付申请返回的结果
     * 参数: [IN]   pGame           CGame对象指针
     *       [IN]   respHeader      代付请求返回的header
     *       [IN]   respData        代付请求返回的data
     *       [OUT]  creditOrderNo   解析出代付订单号
     *       [OUT]  errNote         若出错的话，返回的错误说明
     * 返回值:  true  解析成功
     *          false 解析失败，有可能是返回数据出错
     */
    virtual bool doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, 
        CMyString &creditOrderNo, CMyString &errNote) = 0;

    /* 代付回调数据解析
     * 参数: [IN]   req             代付回调返回的HTTP
     *       [OUT]  orderNo         支付中心订单号
     *       [OUT]  creditOrderNo   代付订单号
     *       [OUT]  creditOK        代付结果状态：true 成功；false 失败 
     * 参数：  true  回调数据解析成功
     *         false 回调数据解析出错
    */
    virtual bool doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, 
                int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK) = 0;

    /* 校验三方平台返回的回调数据签名是否匹配
     * 参数：[IN]  req      http请求的数据
     *       [IN]  pGame    CGame 对象指针
     *       [IN]  pSdk     订单对应的SDK指针
     *       [OUT] errNote  错误说明
     * 返回值：true表示验签成功，false表示验签失败
     */
    virtual bool validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote) = 0;

    /* 代付回调结果的返回值
     * 参数：[IN]  bSucceed       是否成功
     *       [IN]  errCode        错误码
     *       [OUT] ext_headers    扩展的HTTP返回头, 如 Content-Type等
     *       [OUT] szResp         返回内容
     *       [OUT] cbResp         返回的内容长度
     */
    virtual void doCreditCallbackResp(bool bSucceed, int32_t errCode, 
            std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp) = 0;

    /** verify模式下，必须的参数，每个调用自行处理
     *  参数：[IN/OUT] vecParami    必不可少的参数，已有 gameCode, uin, sign
     */
    virtual void addCreditVerifyNeededParam(std::vector<CMyString> &vecParam) {  }

    /* verify模式下，把相关需要发送到三方平台验证的数据保存起来，在Maintance中调用
     * 参数： [IN]  pGame       游服指针，已做安全性检查
     *        [IN]  pSdk        SDK指针，已做安全性检查
     *        [IN]  req         http请求数据
     *        [IN]  data        代付的订单数据
     */
    virtual void storeCreditVerifyData(CGame *pGame, CSdk *pSdk, http::CHttpRequest &req, Json::Value &data, Json::Value &resp_data) {  }

    /* verify模式下，获得在 storeCreditVerifyData中存储的数据，逐条处理
     * 参数：[OUT] req_data    须返回的订单相关数据，必须包含以下字段：
     *                  => gameCode
     *                  => uin
     *                  => payId  (orderId)
     *                  => orderNo
     *                  => fee
     *       [OUT] creditOK   代付操作结果：true表示成功，false表示失败  
     *       [OUT] creditOrderNo  第三方平台返回的订单ID
     *  返回值： -1 出错，在虚函数中要记录错误信息
     *            0 没有待验证的订单
     *            1 有待验证的订单，并处理成功
     *  备注： 每次只处理一条待验证的代付订单，不要做循环操作
     */
    virtual int32_t verifyCreditRequest(Json::Value &req_data, bool &creditOK, CMyString &creditOrderNo, CMyString& errNote) { return 0; }


// ================================== 充值订单查询的虚函数 ==================================
    /* 构建充值订单查询请求
     * 参数: [IN]  req              http请求的数据
     *       [IN]  pGame            CGame对象指针
     *       [IN]  pSdk             CSdk对象指针
     *       [IN]  orderNo          充值订单号
     *       [OUT] url              充值订单查询的URL
     *       [OUT] contentType      httpData的类型，如 application/json 等
     *       [OUT] httpData         HTTP请求的内容
     */
    virtual void doMakeDepositQueryRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk,
        const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData) {};
    /* 充值订单查询结果解析
     * 参数:  [IN]  pGame            CGame对象指针
     *        [IN]  respHeader       HTTP返回header
     *        [IN]  respData         HTTP返回data
     *        [OUT] resp             充值订单返回的数据：{"data":{"orderNo":"xxx","fee":100,"status":1,"desc":"充值成功"}}
     *        [OUT] errNote          错误说明
     */
    virtual bool doParseDepositQueryResp(CGame *pGame, const CMyString &respHeader, 
        const CMyString &respData, Json::Value &resp,  CMyString &errNote) { return false; };

// ================================== 提现订单查询的虚函数 ==================================
    /* 构建提现订单查询请求
     * 参数: [IN]  req              http请求的数据
     *       [IN]  pGame            CGame对象指针
     *       [IN]  pSdk             CSdk对象指针
     *       [IN]  orderNo          充值订单号
     *       [OUT] url              充值订单查询的URL
     *       [OUT] contentType      httpData的类型，如 application/json 等
     *       [OUT] httpData         HTTP请求的内容
     */
    virtual void doMakeCreditQueryRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk,
        const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData) {};
    /* 提现订单查询结果解析
     * 参数:  [IN]  pGame            CGame对象指针
     *        [IN]  respHeader       HTTP返回header
     *        [IN]  respData         HTTP返回data
     *        [OUT] resp             充值订单返回的数据：{"data":{"orderNo":"xxx","fee":100,"status":1,"desc":"充值成功"}}
     *        [OUT] errNote          错误说明
     */
    virtual bool doParseCreditQueryResp(CGame *pGame, const CMyString &respHeader, 
        const CMyString &respData, Json::Value &resp,  CMyString &errNote) { return false; };

// ======== 以下函数用于子类调用，子类直接调用相同函数就可 ========
protected:
    // 请求代收功能
    int32_t doDepositRequest(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers);
    // 代收结果回调
    int32_t doDepositCallback(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers);
    // 代收结果数据验证
    int32_t doDepositVerify(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers);

    // 查询代收订单信息
    int32_t doDepositQuery(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers);

    // 发起代付请求
    int32_t doCreditRequest(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers);
    // 代付结果回调
    int32_t doCreditCallback(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers);
    // 代付结果数据异步处理
    int32_t doCreditVerify(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers);

    // 查询代付订单信息
    int32_t doWithdrawQuery(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers);

protected:
    bool InitRedisCache(redis_cache::CFmtCache &cache, CMyRedis &redis, CXmlFile &xml, const CMyString &sect);
    bool InitRedisCache(redis_cache::CIncrCache &cache, CMyRedis &redis, CXmlFile &xml, const CMyString &sect);

private:
    // ====== 以下为 Maintance 中使用的函数 ======
    void PingMysql(void);
    void PingRedis(void);
    void dealReDeliver(void);
    void dealExpiredOrder(void);
    void dealCreditOrder(void);
    void dealDepositRequest(void);
    void dealDepositCheck(void);
    void dealDepositPendingVerify(void);
    void dealCreditPendingVerify(void);


    void dealReDeliver(const CMyString &key);
    void dealRespBack(void);

protected:
    void postAlert(const CMyString &gameCode, const CMyString &note);
    void postAlarm(int32_t level, const CMyString &content);

    // ========= 以下函数为代收时相关的子函数 ========
    void OnPayResp(Json::Value &resp, char *szResp, uint32_t& cbResp);
    void doPayFailure(const CMyString &gameCode, const CMyString &strUin, const CMyString &orderNo, int64_t fee, char* szResp, uint32_t& cbResp);

    bool createDepositOrder(http::CHttpRequest& req, CSdk* pSdk, CGame* pGame, int32_t& orderId, CMyString& orderNo);
    bool updateDepositOrder(const CMyString &gameCode, int32_t orderId, int32_t result, const CMyString &platforOrderNo);
    bool createDepositRequest(http::CHttpRequest &req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, 
            int32_t orderId, const CMyString &orderNo, CMyString& platformUrl, CMyString& platformOrderNo, int32_t &cost);
    void storeDepositOrder(http::CHttpRequest &req, int32_t orderId, const CMyString &orderNo);
    void doPayResp(CSdkChannel *pChannel, const CMyString& strUrl, const CMyString &orderNo, char* szResp, uint32_t& cbResp);

    bool checkDepositVerifySign(http::CHttpRequest &req, const CMyString &gameApiKey, Json::Value &resp_data);

    // ======== 以下函数为代收回调时的子函数 ========
    bool fetchDepositOrderData(http::CHttpRequest& req, const CMyString& orderNo, Json::Value& req_data, int32_t& orderStatus);
    bool validDepositCallbackIp(CSdk *pSdk, const CMyString& strIp);
    void getDepositWealth(CGame *pGame, int32_t bpId, int64_t fee, int64_t& coin, int64_t& ext_coin, CMyString& itemInfo);
    bool updateDepositNotify(const CMyString &gameCode, int32_t orderId, int32_t result, const CMyString &platformOrderNo, int64_t realFee, const CMyString &regionCode);
    bool postDepositResultToGamesvr(CGame *pGame, CSdk* pSdk, Json::Value &req_data);
    bool finishDepositOrder(const CMyString &gameCode, int32_t orderId, int32_t result, int32_t bpId, const CMyString &strUin);
    bool deliverGoods(const CMyString &gameCode, const CMyString &strUrl, const CMyString &strData, enum DepositStatus &result, bool &bNeedReDeliver);
    void judgeBPInfo(CGame *pGame, const int32_t& bpId, const int64_t& fee, const CMyString& strUin, Json::Value& req_data, int64_t& coin, int64_t& ext_coin, CMyString& itemInfo);

    // ======== 以下为代付申请时相关子函数 ========
    bool validCreditParam(http::CHttpRequest &req, Json::Value &resp, CGame *&pGame);
    bool validCreditIp(const CMyString &strIp);
    bool validCreditToken(const CMyString &orderNo, const CMyString &creditToken, Json::Value &req_data, Json::Value &resp);
    bool postCreditRequest(Json::Value &req, Json::Value &resp, CGame *pGame, CSdk *&pSdk, CMyString &creditOrderNo, int32_t &cost);
    void storeCreditData(CGame *pGame, CSdk *pSdk, const CMyString &strUin, const CMyString &orderNo, int64_t fee, int32_t creditCost);
    bool updateCreditOrder(const CMyString &gameCode, const CMyString &creditRemark, int64_t creditTime, const CMyString &creditOperator, 
            const CMyString &orderNo, const CMyString &creditOrderNo, int32_t result);
    bool updateCreditTax(const CMyString &gameCode, const CMyString &orderNo, int64_t &fee, int64_t &realFee, int64_t &tax);
    bool finishCreditOrder(const CMyString &gameCode, const CMyString &orderNo, const CMyString &creditOrderNo, int32_t result);

    bool checkCreditVerifySign(http::CHttpRequest &req, const CMyString &gameApiKey, Json::Value &resp_data);

    // ======== 以下函数为代付回调时的子函数 ========
    bool fetchCreditOrderData(const CMyString &orderNo, Json::Value &req_data);
    bool validCreditCallbackIp(const CMyString &strIp, CSdk *pSdk);
    bool postCreditResultToGamesvr(CGame *pGame, CSdk *pSdk, const CMyString &orderNo, int64_t fee, int64_t realFee, enum CreditStatus creditResult);

    // ======== 以下函数为查询时的子函数 ========
    bool validQueryParam(http::CHttpRequest &req, Json::Value &resp, CGame *&pGame);
    bool createDepositQueryRequest(http::CHttpRequest &req, CGame* pGame, CSdk *pSdk, Json::Value &resp, int32_t &cost);
    bool createCreditQueryRequest(http::CHttpRequest &req, CGame* pGame, CSdk *pSdk, Json::Value &resp, int32_t &cost);

    // ======== 以下函数为写入提现日志的函数 ========
    void setWithdrawLog(const CMyString& orderNo, const int& result, CMyString oper, CMyString remark, CMyString message, int64_t createtime = 0);
    void updateWithdrawLog(const CMyString& orderNo, const int& result, CMyString message);

protected:
    ISimDB*                     m_pDB;          // 主数据库指针
    CMyRedis                    m_redis;        // 主redis对象
    CMyString                   m_sdkCode;      // SDK 代码，必须与DB中一致
    const CMyString             m_sdkTag;               // sdk标签

    int32_t                     m_errCode;
    CMyString                   m_errNote;

private:
    const enum DepositStyle     m_depositStyle;         // 代收款的模式
    const enum HttpRequestStyle m_httpRequestStyle;     // HTTP 请求的模式

private:
    // 以下为超时订单的处理
    redis_cache::CFmtCache      m_cacheDepositOrder;
    CMyTimer                    m_timerDepositOrder;

    std::vector<int32_t>        m_reDeliverTimes;     // 重新向游服申请发货的时间序列
    redis_cache::CIncrCache     m_cacheDeliverGoods;  // 向gamesvr发送通知的cache
    CMyTimer                    m_timerDeliverGoods;  // 定时检查发货缓存

    redis_cache::CFmtCache      m_cacheCreditReq;     // 代付请求时缓存KEY
    redis_cache::CFmtCache      m_cacheCreditCB;      // 向三方平台发起代付
    redis_cache::CFmtCache      m_cacheCreditRespBack;     // 代付请求时缓存KEY
    std::queue<CMyString>       m_qRespBackParam;

private:
    enum HttpRequestMethod      m_creditRequestMethod;
    //告警
    CMyString                   m_strAlarmUrl;
    CMyString                   m_strNodeName;
    int32_t                     m_nGroup;
    bool                        m_bOldAlt;
};

// 充值虚函数接口定义
#define PAYBASE_DEPOSIT_FUNCTIONS_DECLARE   virtual void doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData);\
                                            virtual bool doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData,CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote);\
                                            virtual bool doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo,CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool& depositedOK); \
                                            virtual bool validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote); \
                                            virtual void doDepositCallbackResp(bool bSucceed, int32_t errCode,std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp);

// 提现虚拟函数接口定义
#define PAYBASE_CREDIT_FUNCTIONS_DECLARE    virtual void doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData); \
    										virtual bool doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote); \
    										virtual bool doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK); \
    										virtual bool validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote); \
    										virtual void doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp);

// 充值订单查询接口定义
#define PAYBASE_DEPOSITQRY_FUNCTION_DECLARE virtual void doMakeDepositQueryRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData); \
	    									virtual bool doParseDepositQueryResp(CGame *pGame, const CMyString &respHeader, const CMyString &respData, Json::Value &resp,  CMyString &errNote);

// 提现订单查询接口定义	    									
#define PAYBASE_CREDITQRY_FUNCTION_DECLARE	virtual void doMakeCreditQueryRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData); \
	    									virtual bool doParseCreditQueryResp(CGame *pGame, const CMyString &respHeader, const CMyString &respData, Json::Value &resp,  CMyString &errNote);



// 快捷访问定义
#define HTTP_REQ_STR(var, param)            CMyString var=req.getParam(#param)
#define HTTP_REQ_INT32(var, param)          int32_t var=req.getParamInt(#param)
#define HTTP_REQ_INT64(var, param)          int64_t var=req.getParamInt64(#param)

#define HTTP_JSON_STR(param1, param2)       data[#param1] = req.getParam(#param2)
#define HTTP_JSON_INT32(param1, param2)     data[#param1] = req.getParamInt(#param2)
#define HTTP_JSON_INT64(param1, param2)     data[#param1] = (Json::Int64)req.getParamInt64(#param2)

#define MYSQL_JSON_STR(param, fieldIdx)     req_data[#param] = m_pDB->GetString(fieldIdx)
#define MYSQL_JSON_INT32(param, fieldIdx)   req_data[#param] = m_pDB->GetInt(fieldIdx)
#define MYSQL_JSON_INT64(param, fieldIdx)   req_data[#param] = (Json::Int64)m_pDB->GetInt64(fieldIdx)

