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

 Date: 31/07/2021 15:26:32
*/
USE paycenter_v3;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Procedure structure for pAPIV3_AddNewSDK
-- ----------------------------
DROP PROCEDURE IF EXISTS `pAPIV3_AddNewSDK`;
delimiter ;;
CREATE PROCEDURE `pAPIV3_AddNewSDK`(IN param_gameCode   	VARCHAR(16),  
	IN param_code       	VARCHAR(16),  
	IN param_name       	VARCHAR(64),  
	IN param_accountId  	VARCHAR(128), 
	IN param_accountKey 	VARCHAR(128), 
	
	IN param_rechargeCgi	VARCHAR(256),
	IN param_payUrl     	VARCHAR(256), 
	IN param_checkUrl   	VARCHAR(256), 
	IN param_notifyUrl  	VARCHAR(256), 
	IN param_extraData     	VARCHAR(1024), 
	IN param_validIps   	VARCHAR(256), 

	IN param_withdrawCgi	VARCHAR(256),
	IN param_withdrawUrl 	VARCHAR(256),
	IN param_withdrawCallbackUrl VARCHAR(256),
	IN param_withdrawQueryUrl VARCHAR(256),
	in param_withdrawValidIp  varchar(256),

	IN param_mgrUrl     	VARCHAR(256),
	IN param_enableDeposit INT(11),
	IN param_enableWithdraw INT(11))
BEGIN
	DECLARE var_code        INT DEFAULT 0;
	DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
	declare var_sdkId       int default 0;
	DECLARE EXIT HANDLER FOR SQLEXCEPTION
	BEGIN
		GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
		SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
		SELECT var_code, var_message, 0;
	END;

	LABEL_MAIN: BEGIN
        -- 判断游戏是否存在
        if not exists (select 1 from `t_game` where `code`=param_gameCode) then
        begin
            set var_code = -1001, var_message=concat('not found gameCode ', param_gameCode);
            leave LABEL_MAIN;
        end;
        end if;
        
        -- 判断是否存在 sdk
		IF EXISTS (SELECT * FROM `t_sdk` WHERE gameCode = param_gameCode AND `code`=param_code) THEN
		BEGIN
			SET var_code=-1003, var_message=CONCAT('already exists sdk code: ', param_code);
			LEAVE LABEL_MAIN;
		END;
		END IF;

        -- 写入数据库
		INSERT INTO `t_sdk`(`gameCode`, `code`, `name`, `accountId`, `apiKey`, 
            `recharge_cgi`, `apiPayUrl`, `apiCheckUrl`, `notifyUrl`, `extraData`, `validIps`,
            `withdraw_cgi`, `apiWithdrawUrl`, `apiWithdrawCallbackUrl`, `apiWithdrawQueryUrl`, `validIps_withdraw`,
            `mgrUrl`,`createTime`,`status`,`enableDeposit`,`enableWithdraw`)
		VALUES(param_gameCode, param_code, param_name, param_accountId, param_accountKey, 
            param_rechargeCgi, param_payUrl, param_checkUrl, param_notifyUrl, param_extraData, param_validIps,
            param_withdrawCgi, param_withdrawUrl, param_withdrawCallbackUrl,param_withdrawQueryUrl, param_withdrawValidIp,
            param_mgrUrl, NOW(), 1, param_enableDeposit, param_enableWithdraw);
		IF (ROW_COUNT() = 0) THEN
		BEGIN
			SET var_code=-1005, var_message=CONCAT('add new sdk ', param_code, '(', param_name,') FAILURE');
			LEAVE LABEL_MAIN;
		END;
		END IF;
		set var_sdkId = LAST_INSERT_ID();
		
		SET var_code=0, var_message=CONCAT('add new sdk ', param_code, '(', param_name,') SUCCEED');
	END LABEL_MAIN;

	SELECT var_code, var_message, var_sdkId;
	END
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
