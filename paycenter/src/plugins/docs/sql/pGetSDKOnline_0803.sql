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

 Date: 03/08/2021 11:11:41
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Procedure structure for pGetSDKOnline
-- ----------------------------
DROP PROCEDURE IF EXISTS `pGetSDKOnline`;
delimiter ;;
CREATE PROCEDURE `pGetSDKOnline`(in p_gameCode varchar(32))
begin
	select `code`,
				 `name`
	from t_sdk
	where gameCode = p_gameCode
	  and `status` = 1 and `enableWithdraw` = 1
	order by convert(`name` using gbk) asc;
end
;;
delimiter ;

SET FOREIGN_KEY_CHECKS = 1;
