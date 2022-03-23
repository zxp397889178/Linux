ALTER TABLE `paycenter_v3`.`t_payorder` 
    ADD INDEX `idx_payorder_time`(`createdAt`),
    MODIFY COLUMN `createdAt` timestamp(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6) COMMENT '创建时间' AFTER `thirdPartyResponse`,
    MODIFY COLUMN `updatedAt` timestamp(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6) COMMENT '更新时间' AFTER `createdAt`,
    MODIFY COLUMN `user_createtime` timestamp(0) NULL DEFAULT NULL COMMENT '用户创建时间' AFTER `create_channel`,
    ADD INDEX  `idx_payorder_orderno` (`orderNo`);



