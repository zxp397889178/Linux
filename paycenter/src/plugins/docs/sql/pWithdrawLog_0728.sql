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

 Date: 29/07/2021 10:03:52
*/
USE paycenter_v3;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Procedure structure for pWithdrawLog
-- ----------------------------
DROP PROCEDURE IF EXISTS `pWithdrawLog`;
delimiter ;;
CREATE PROCEDURE `pWithdrawLog`(IN param_withdrawId        	varchar(50),     	
    IN param_status 	int,      
    IN param_operUser varchar(32),       	
    IN param_remark   varchar(255),        	
    IN param_message  varchar(255),   	       	
    IN param_time  	  bigint)
  COMMENT '提现日志'
BEGIN
		INSERT INTO `log_withdraw_operate`(`withdraw_id`,`status`,operate_user,remark,message,`create_time`,`update_time`)
		VALUES(param_withdrawId, param_status,param_operUser,param_remark,FROM_BASE64(param_message),FROM_UNIXTIME(param_time),NOW());

END
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
