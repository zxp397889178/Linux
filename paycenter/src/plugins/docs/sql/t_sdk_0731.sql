ALTER TABLE `paycenter_v3`.`t_sdk` 
ADD COLUMN `enableDeposit` int(1) UNSIGNED ZEROFILL NOT NULL DEFAULT 1 COMMENT '是否支持充值：0-不支持，1-支持' AFTER `status`,
ADD COLUMN `enableWithdraw` int(1) UNSIGNED ZEROFILL NOT NULL DEFAULT 1 COMMENT '是否开启提现：0-不支持，1-支持' AFTER `enableDeposit`;