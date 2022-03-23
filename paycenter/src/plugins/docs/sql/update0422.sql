use paycenter_v3;

ALTER TABLE `paycenter_v3`.`t_billpoint`
  CHANGE `billPoint` `billPoint` BIGINT (20) NOT NULL COMMENT '计费点金额，以分显示，法币',
  CHANGE `extraCount` `extraCount` BIGINT (20) DEFAULT - 1 NULL COMMENT '累计可购买次数，-1表示无限制',
  ADD COLUMN `versionRange` VARCHAR (128) DEFAULT '' NULL COMMENT '版本号范围，用-表示范围值，多个版本号之间用,分隔' AFTER `bpDescId`,
  ADD COLUMN `sourceRange` VARCHAR (128) DEFAULT '' NULL COMMENT '来源号范围，用-表示来源号范围，多个来源号组用,分隔' AFTER `versionRange`,
  ADD COLUMN `vipRange` VARCHAR (128) DEFAULT '' NULL COMMENT 'VIP范围，用-表示VIP范围，多个VIP组之间用,分隔' AFTER `sourceRange`,
  ADD COLUMN `dailyCount` INT DEFAULT - 1 NULL COMMENT '每日可购买次数，-1表示不限，0表示不可购买' AFTER `vipRange`,
  ADD COLUMN `monthlyCount` INT DEFAULT - 1 NULL COMMENT '每月可购买次数，-1表示不限，0表示不可购买' AFTER `dailyCount`,
  ADD COLUMN `discount` INT DEFAULT 0 NULL COMMENT '折扣比例，百分比，实际金额为billPoint,仅用于显示之用' AFTER `monthlyCount`;

ALTER TABLE `paycenter_v3`.`t_billpoint`
  ADD COLUMN `channelRange` VARCHAR (256) DEFAULT '' NULL COMMENT '渠道号范围，空表示不限，多来渠道号用，分隔' AFTER `sourceRange`;

