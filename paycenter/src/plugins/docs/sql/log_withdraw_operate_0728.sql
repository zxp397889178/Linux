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

 Date: 30/07/2021 16:39:49
*/
USE paycenter_v3;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for log_withdraw_operate
-- ----------------------------
DROP TABLE IF EXISTS `log_withdraw_operate`;
CREATE TABLE `log_withdraw_operate`  (
  `id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT '主键',
  `withdraw_id` varchar(50) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL COMMENT '提现订单ID',
  `status` int(11) NULL DEFAULT NULL COMMENT '状态：1- 待审核 2- 审核拒绝 3- 审核拒绝并金额冻结 4- 审核成功 5- 放款审核成功-SDK放款 6- 放款审核成功-人工放款 7- 放款审核拒绝 8- 放款审核拒绝并冻结资金 10- 放款成功（SDK放款）11- 放款被驳回（SDK放款）12- 放款失败（SDK放款）13- 放款订单异常（SDK放款）',
  `operate_user` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '审核人员',
  `remark` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '' COMMENT '审核备注',
  `message` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NULL DEFAULT NULL COMMENT '操作结果信息（如：商户账号余额不足）',
  `create_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '申请时间',
  `update_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '审核时间',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `log_withdraw_wid`(`withdraw_id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 1330 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_general_ci COMMENT = '提现审核日志表' ROW_FORMAT = COMPACT;

SET FOREIGN_KEY_CHECKS = 1;
