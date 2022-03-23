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

 Date: 30/07/2021 16:40:31
*/
USE paycenter_v3;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for dict_withdrawlimit
-- ----------------------------
DROP TABLE IF EXISTS `dict_withdrawlimit`;
CREATE TABLE `dict_withdrawlimit`  (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `gameCode` varchar(16) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '游服编码，对应的gameId\n重复字段，用于不同应用使用',
  `deposit` bigint(20) NOT NULL COMMENT '充值金额',
  `withdraw` bigint(20) NOT NULL COMMENT '可提现金额: -1表示无限制',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 25 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci ROW_FORMAT = Compact;

SET FOREIGN_KEY_CHECKS = 1;
