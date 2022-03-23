ALTER TABLE `paycenter_v3`.`dict_paystyle` 
ADD COLUMN `gameCode` VARCHAR(64) NULL DEFAULT '' COMMENT '游服编码，对应的gameId\n重复字段，用于不同应用使用' AFTER `gameId`;


