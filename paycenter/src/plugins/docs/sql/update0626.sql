ALTER TABLE `paycenter_v3`.`t_billpoint` 
ADD COLUMN `pos` INT NULL DEFAULT 0 COMMENT '计费点排序/位置' AFTER `status`,
ADD COLUMN `extraInfo` VARCHAR(512) NULL DEFAULT '' COMMENT '扩展信息，以json格式存储' AFTER `pos`;


