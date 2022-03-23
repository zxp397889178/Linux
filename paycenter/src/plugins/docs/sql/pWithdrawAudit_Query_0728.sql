/*
 Navicat Premium Data Transfer

 Source Server         : 支付中心-开发服
 Source Server Type    : MySQL
 Source Server Version : 50627
 Source Host           : 192.168.2.163:3306
 Source Schema         : paycenter_v3

 Target Server Type    : MySQL
 Target Server Version : 50627
 File Encoding         : 65001

 Date: 27/07/2021 16:45:59
*/
USE paycenter_v3;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Procedure structure for pWithdrawAudit_Query
-- ----------------------------
DROP PROCEDURE IF EXISTS `pWithdrawAudit_Query`;
delimiter ;;
CREATE PROCEDURE `pWithdrawAudit_Query`(in param_gameCode       varchar(32),    -- 游戏代码
        in param_sdkCode        varchar(32),    -- SDK代码
        in param_orderNo        varchar(64))
  COMMENT '查询需要SDK转帐的基础数据'
BEGIN
        declare  var_withdrawCgi    varchar(128);
        declare  var_token          varchar(32);
        
        set var_token = CONCAT('T', UNIX_TIMESTAMP(), 'K', SUBSTRING(MD5(RAND()),1,8));
        
        SELECT `withdraw_cgi` into var_withdrawCgi
        FROM `t_sdk`
        WHERE `gameCode`=param_gameCode AND `code`=param_sdkCode AND `status`=1;
        
        SELECT `uuid` as 'uin',`bank_code` as 'bankCode',`account_no` as 'bankCard',`account_name` as 'bankName',
            `real_money` as 'fee', var_withdrawCgi as 'cgi', var_token as 'token'
        FROM `t_withdraw`
        WHERE `gameCode`=param_gameCode AND `order_id`=param_orderNo AND `status` in (4,11,12,13);
        
	END
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
