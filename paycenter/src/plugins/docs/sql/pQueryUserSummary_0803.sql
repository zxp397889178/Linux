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

 Date: 05/08/2021 16:23:23
*/
USE paycenter_v3;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Procedure structure for pQueryUserSummary
-- ----------------------------
DROP PROCEDURE IF EXISTS `pQueryUserSummary`;
delimiter ;;
CREATE PROCEDURE `pQueryUserSummary`(IN param_gameCode varchar(32),IN param_uin varchar(150))
BEGIN
	SELECT IFNULL(t1.recharge_total,0) recharge_total,IFNULL(t2.withdraw_total,0) withdraw_total FROM
	(
	SELECT SUM(`realAmount`) recharge_total FROM `t_payorder` WHERE uin = param_uin AND `orderStatus` IN(1,5,8,9,10) AND `gameCode` = param_gameCode
	) t1,
	(
	SELECT SUM(`money`) withdraw_total FROM `t_withdraw` WHERE `gameCode` = param_gameCode AND `uuid` = param_uin AND `status` IN(1,4,5,6,10)
	) t2;
END
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
