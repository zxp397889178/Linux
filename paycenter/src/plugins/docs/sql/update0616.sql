ALTER TABLE `paycenter_v3`.`t_billpoint`
  ADD COLUMN `category` VARCHAR (16) DEFAULT '' NULL COMMENT '计费点分类名称' AFTER `bpCode`;

DELIMITER $$

USE `paycenter_v3`$$

DROP PROCEDURE IF EXISTS `pAPIV3_AddBillPoint`$$

CREATE PROCEDURE `pAPIV3_AddBillPoint`(IN param_configId		INT,
	IN param_bpCode	        VARCHAR(64),
	IN param_billPoint		BIGINT,
	IN param_coin			BIGINT,
	IN param_extraCoin		BIGINT,
	IN param_initCount		INT,
	IN param_descId         INT,
	IN param_iconUrl        VARCHAR(256),
	IN param_itemInfo       VARCHAR(256),
	IN param_versionRange   VARCHAR(128),
	IN param_channelRange   VARCHAR(256),
	IN param_vipRange       VARCHAR(128),
	IN param_dailyCount     INT,
	IN param_monthlyCount   INT,
	IN param_discount       INT,
	IN param_category       VARCHAR(16) )
BEGIN
	DECLARE var_code        INT DEFAULT 0;
	DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
	DECLARE var_bpId		INT DEFAULT 0; 
	DECLARE EXIT HANDLER FOR SQLEXCEPTION
	BEGIN
		GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
		SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
		SELECT var_code, var_message,var_bpId;
	END;
	
	LABEL_MAIN: BEGIN
        INSERT INTO `t_billpoint`(`configId`,`bpCode`,`billPoint`,`coin`,`extraCoin`,
                `extraCount`,`bpIcon`,`bpDescId`,`createTime`,`status`, `itemInfo`, 
                `versionRange`,`channelRange`,`vipRange`,`dailyCount`,
                `monthlyCount`,`discount`, `category`)
		VALUES (param_configId, param_bpCode, param_billPoint, param_coin, param_extraCoin, 
                param_initCount, param_iconUrl, param_descId, NOW(), 1, param_itemInfo,
                param_versionRange, param_channelRange, param_vipRange, param_dailyCount, 
                param_monthlyCount, param_discount, param_category);
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

DELIMITER $$

USE `paycenter_v3`$$

DROP PROCEDURE IF EXISTS `pAPIV3_ModifyBillPoint`$$

CREATE PROCEDURE `pAPIV3_ModifyBillPoint`(IN param_bpId 	        INT,
	IN param_bpCode         VARCHAR(64),
	IN param_billPoint		BIGINT,
	IN param_coin			BIGINT,
	IN param_extraCoin		BIGINT,
	IN param_initCount		INT,
	IN param_descId         INT,
	IN param_iconUrl        VARCHAR(256),
	IN param_itemInfo       VARCHAR(256),
	IN param_versionRange   VARCHAR(128),
	IN param_channelRange   VARCHAR(256),
	IN param_vipRange       VARCHAR(128),
	IN param_dailyCount     INT,
	IN param_monthlyCount   INT,
	IN param_discount       INT,
	IN param_category       VARCHAR(16) )
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
         `vipRange`=param_vipRange, `dailyCount`=param_dailyCount,`monthlyCount`=param_monthlyCount, 
         `discount`=param_discount, `category`=param_category
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
