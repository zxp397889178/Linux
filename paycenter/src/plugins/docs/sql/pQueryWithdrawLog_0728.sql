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

 Date: 30/07/2021 18:44:49
*/
USE paycenter_v3;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Procedure structure for pQueryWithdrawLog
-- ----------------------------
DROP PROCEDURE IF EXISTS `pQueryWithdrawLog`;
delimiter ;;
CREATE PROCEDURE `pQueryWithdrawLog`(IN param_withdrawid varchar(64))
BEGIN
	SELECT *
	FROM log_withdraw_operate a
	WHERE a.withdraw_id = param_withdrawid
	ORDER BY create_time DESC;
END
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
