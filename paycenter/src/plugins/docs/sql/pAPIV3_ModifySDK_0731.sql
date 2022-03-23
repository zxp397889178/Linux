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

 Date: 31/07/2021 15:27:09
*/
USE paycenter_v3;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Procedure structure for pAPIV3_ModifySDK
-- ----------------------------
DROP PROCEDURE IF EXISTS `pAPIV3_ModifySDK`;
delimiter ;;
CREATE PROCEDURE `pAPIV3_ModifySDK`(IN param_gameCode   	varchar(16),  
    IN param_sdkId        int,
		IN param_code       	varchar(16),  
		IN param_name       	varchar(64),  
		IN param_account    	varchar(128), 
		IN param_apiKey     	varchar(128), 
		IN param_rechargeCgi	varchar(256),
		IN param_payUrl     	varchar(256), 
		IN param_checkUrl   	varchar(256), 
		IN param_notifyUrl  	varchar(256), 
		IN param_extraData  	varchar(2048),
		IN param_validIps		varchar(256), 
		IN param_withdrawCgi	     varchar(256),
		IN param_withdrawUrl	     varchar(256),
		IN param_withdrawCallbackUrl varchar(256),
		IN param_withdrawQueryUrl    varchar(256),
		IN param_withdrawValidIps    varchar(256),
		IN param_mgrUrl     	varchar(256),
	  IN param_enableDeposit INT(11),
	  IN param_enableWithdraw INT(11))
  COMMENT 'SDK修改'
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
					GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
					SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
					SELECT var_code, var_message;
        END;
        
LABEL_MAIN: BEGIN
	
	update `t_sdk`
	set `code`=param_code, `name`=param_name, `accountId`= param_account, `apiKey`=param_apiKey, 
        `recharge_cgi` = param_rechargeCgi, `apiPayUrl`= param_payUrl, `apiCheckUrl` = param_checkUrl, 
        `notifyUrl`=param_notifyUrl,`extraData`=param_extraData,  `validIps`=param_validIps,
        `withdraw_cgi` = param_withdrawCgi,`apiWithdrawUrl`=param_withdrawUrl,
        `apiWithdrawQueryUrl` = param_withdrawQueryUrl,`apiWithdrawCallbackUrl` = param_withdrawCallbackUrl,
        `validIps_withdraw`=param_withdrawValidIps, `mgrUrl`=param_mgrUrl, `enableDeposit`=param_enableDeposit, `enableWithdraw`=param_enableWithdraw,
	    `updateTime`=NOw()
	WHERE `sdkId`= param_sdkId;
	
    IF (ROW_COUNT() <= 0) THEN
    BEGIN
        SET var_code=-1002, var_message=CONCAT('modify sdk ', param_code, '(', param_name,') FAILURE');
        LEAVE LABEL_MAIN;
    END;
    END IF;
    SET var_code=0, var_message=CONCAT('modify sdk ', param_code, '(', param_name,') SUCCEED');
    END LABEL_MAIN;
    
    SELECT var_code, var_message;
END
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
