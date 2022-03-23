/*
 * =====================================================================================
 *
 *       Filename:  pay_error.h
 *
 *    Description:  错误定义
 *
 *        Version:  1.0
 *        Created:  2020年07月29日 16时26分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

enum payframe_error
{
    err_OK = 0, 
    err_Success = 0,
    err_Succeed = 0,
    err_NoError = 0,

    err_HttpOK = 200,

    // 系统错误
    err_SystemBegin = 1000,
    err_SystemFault,                    // 系统错误
    err_SystemError,                    // 系统错误
    err_InsufficientMemory,             // 内存不足
    err_FileNotFound,                   // 文件不存在
    err_FileIsEmpty,                    // 空文件
    

    // 客户端错误
    err_ClientBegin = 1400,
    err_MissingParameters,              // 缺少必要的参数
    err_SignMismatch,                   // 签名出错
    err_InvalidParam,                   // 参数不合法
    err_PurchaseTimeUpToLimit,          // 购买次数达到上限


    // 服务端错误
    err_ServerBegin = 1700,
    err_NotValidGame,                   // 游戏代码不合法
    err_NotAvaiablePayChannel,          // 没有可用的支付渠道
    err_ConfigMissingParam,             // config.xml中有缺失参数
    err_NotExistGamesvr,                // 没有可用的游戏服务器
    err_CreateNewOrderFailure,          // 创建新订单失败
    err_DecodeUrlFailure,
    err_GeneratePayOrderFailure,
    err_InvalidLocationInfo,
    err_MerchantBankIpInvalid,
    err_MissConfig_GameId,
    err_MissConfig_GameCode,
    err_MissConfig_GameNotifyUrl,
    err_MissConfig_GameSignKey,
    err_MissConfig_PaycenterNotify,
    err_MissConfig_PaycenterNotifyURI,
    err_MissConfig_PaySDK,
    err_MissConfig_SDKValidIp,
    err_MissPaychannelId,    
    err_NotLocationUrl,
    err_PaychannelKeydataFault,
    err_PayDataSendtoGameFailure,
    err_PayOrderDataNotFound,
    err_PayOrderFinished,
    err_PayOrderIdNotFound,
    err_PayOrderNoInvalid,
    err_PayOrderNotFound,
    err_SavePayCallbackDataFailure,
    err_UpdatePayOrderStatusFailure,
    err_GetGameListFailure,
    err_GetSdkChannelFailure,
    err_MissPayChannelId,
    err_InvalidChannelId,
    err_UnkownGamesvr,
    err_InvalidParamter,
    err_InvalidGamesvr,
    err_callbackIpInvalid,

    // 数据库错误
    err_DatabaseBegin = 2500,
    err_MysqlOpenFailure,               // mysql 打开失败
    err_MysqlQueryFault,                // mysql 查询语句错误
    err_DbmsConfigFault, 
    err_DbmsDbInvalid,
    err_DbmsMysqlFault,
    err_DbmsNotConfig,
    err_DbmsNotMysql,
    err_DbmsOpenFailure,
    err_ServerDBError,

    // 第三方平台的错误
    err_3rdBegin = 3000,
    err_orderPayResp,                   // 第三方支付返回信息错误
    err_payoutResp,
    err_invalidGameCode,

};

#define PAYFRAME_ERROR(err)     -(err)

