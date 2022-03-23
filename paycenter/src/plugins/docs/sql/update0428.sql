DELIMITER $$
USE `paycenter_v3`$$
DROP PROCEDURE IF EXISTS `pAPIV3_ChangeBillpointStatus`$$
CREATE PROCEDURE `pAPIV3_ChangeBillpointStatus`(
        IN param_bpId   INT,
        IN param_opcode INT     -- 0, 1, 99
    )
BEGIN
        DECLARE var_usedCount   INT DEFAULT 0;
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
        
        IF (param_opcode=99) THEN
        BEGIN
            IF EXISTS (SELECT 1 FROM `t_payorder` WHERE `bpId`=param_bpId AND `orderStatus` IN (5,8,9,10)) THEN
            BEGIN
                UPDATE `t_billpoint` SET `status`=0,`updateTime`=NOW() WHERE `bpId`=param_bpId;
                IF ROW_COUNT()=1 THEN
                    SET var_code=0, var_message='disable billpoint succeed';
                ELSE
                    SET var_code=0, var_message='delete billpoint fail';
                END IF;
            END;    
            ELSE
            BEGIN
                DELETE FROM `t_billpoint` WHERE `bpId`=param_bpId;
                IF ROW_COUNT()=1 THEN
                    SET var_code=99, var_message='delete billpoint succeed';
                ELSE
                    SET var_code=99, var_message='not found billpoint';
                END IF;
            END;
            END IF;
        END;
        ELSEIF (param_opcode=0 OR param_opcode=1) THEN
        BEGIN
            UPDATE `t_billpoint` SET `status`=param_opcode, `updateTime`=NOW() WHERE `bpId`=param_bpId;
            IF ROW_COUNT()=1 THEN
                SET var_code=param_opcode, var_message='update billpoint succeed';
            ELSE
                SET var_code=param_opcode, var_message='update billpoint fail';
            END IF;
        END;
        ELSE
        BEGIN
            SET var_code=-1, var_message=CONCAT('unknown opcode ', param_opcode);
        END;
        END IF;
        
        SELECT var_code, var_message;
    END$$

DELIMITER ;

DELIMITER $$

USE `paycenter_v3`$$

DROP PROCEDURE IF EXISTS `pAPIV3_ModifyBillPoint`$$

CREATE PROCEDURE `pAPIV3_ModifyBillPoint`(IN param_bpId          INT,
    IN param_bpCode         VARCHAR(64),
    IN param_billPoint      BIGINT,
    IN param_coin           BIGINT,
    IN param_extraCoin      BIGINT,
    IN param_initCount      INT,
    IN param_descId         INT,
    IN param_iconUrl        VARCHAR(256),
    IN param_itemInfo       VARCHAR(256),
    IN param_versionRange   VARCHAR(128),
    IN param_channelRange   VARCHAR(256),
    IN param_vipRange       VARCHAR(128),
    IN param_dailyCount     INT,
    IN param_monthlyCount   INT,
    IN param_discount       INT)
BEGIN
    DECLARE var_code        INT DEFAULT 0;
    DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
        SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
        SELECT var_code `code`, var_message message;
    END;
    
    LABEL_MAIN: BEGIN
    UPDATE t_billpoint 
    SET `bpCode`=param_bpCode,`billPoint`=param_billPoint,
         `coin` = param_coin,`extraCoin` = param_extraCoin, `extraCount` = param_initCount,
         `bpIcon`=param_iconUrl,`bpDescId`=param_descId, `updateTime`=NOW(),
         `itemInfo`=param_itemInfo, `versionRange`=param_versionRange,`channelRange`=param_channelRange,
         `vipRange`=param_vipRange, `dailyCount`=param_dailyCount,`monthlyCount`=param_monthlyCount, `discount`=param_discount
    WHERE `bpId`=param_bpId;
    IF (ROW_COUNT()=0) THEN
    BEGIN
        SET var_message='not change anything.';
    END;
    END IF; 
    END LABEL_MAIN;
    SELECT var_code `code`, var_message message;
    END$$

DELIMITER ;

DELIMITER $$

USE `paycenter_v3`$$

DROP PROCEDURE IF EXISTS `pAPIV3_AddBillPoint`$$

CREATE PROCEDURE `pAPIV3_AddBillPoint`(IN param_configId     INT,
    IN param_bpCode         VARCHAR(64),
    IN param_billPoint      BIGINT,
    IN param_coin           BIGINT,
    IN param_extraCoin      BIGINT,
    IN param_initCount      INT,
    IN param_descId         INT,
    IN param_iconUrl        VARCHAR(256),
    IN param_itemInfo       VARCHAR(256),
    IN param_versionRange   VARCHAR(128),
    IN param_channelRange   VARCHAR(256),
    IN param_vipRange       VARCHAR(128),
    IN param_dailyCount     INT,
    IN param_monthlyCount   INT,
    IN param_discount       INT)
    
BEGIN
    DECLARE var_code        INT DEFAULT 0;
    DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
    DECLARE var_bpId        INT DEFAULT 0; 
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
        SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
        SELECT var_code, var_message,var_bpId;
    END;
    
    LABEL_MAIN: BEGIN
        INSERT INTO `t_billpoint`(`configId`,`bpCode`,`billPoint`,`coin`,`extraCoin`,
                `extraCount`,`bpIcon`,`bpDescId`,`createTime`,`status`, `itemInfo`, 
                `versionRange`,`channelRange`,`vipRange`,`dailyCount`,`monthlyCount`,`discount`)
        VALUES (param_configId, param_bpCode, param_billPoint, param_coin, param_extraCoin, 
                param_initCount, param_iconUrl, param_descId, NOW(), 1, param_itemInfo,
                param_versionRange, param_channelRange, param_vipRange, param_dailyCount, param_monthlyCount, param_discount);
        IF (ROW_COUNT() = 0) THEN
        BEGIN
            SET var_code=-1001, var_message=CONCAT('add new billpoint ', param_billPoint, ') FAILURE');
            LEAVE LABEL_MAIN;
        END;
        END IF;
        SET var_bpId = LAST_INSERT_ID();
        
    END LABEL_MAIN;
        
        SELECT var_code `code`, var_message message,var_bpId;
    
END$$

DELIMITER ;

ALTER TABLE `paycenter_v3`.`t_billpoint`
  DROP COLUMN `sourceRange`;



