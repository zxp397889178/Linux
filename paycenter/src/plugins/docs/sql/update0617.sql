ALTER TABLE `paycenter_v3`.`dict_paystyle` 
ADD COLUMN `gameId` INT NULL DEFAULT 0 COMMENT '指定游戏类型，id' AFTER `desc`;

ALTER TABLE `paycenter_v3`.`dict_paystyle` 
CHANGE COLUMN `code` `code` VARCHAR(16) NOT NULL COMMENT '支付方式代码' ;


