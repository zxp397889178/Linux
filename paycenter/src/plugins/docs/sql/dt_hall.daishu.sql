/*
SQLyog Ultimate v13.1.1 (64 bit)
MySQL - 5.6.27 : Database - dt_hall
*********************************************************************
*/

/*!40101 SET NAMES utf8mb4 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`dt_hall` /*!40100 DEFAULT CHARACTER SET utf8mb4 */;

USE `dt_hall`;

/*Table structure for table `config_cash` */

DROP TABLE IF EXISTS `config_cash`;

CREATE TABLE `config_cash` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `type` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '渠道配置：1 充值；2 提现；',
  `list` varchar(30) NOT NULL DEFAULT '' COMMENT '渠道列表（用逗号隔开）：1 银联；2 支付宝；3 微信；（固定选框吧）',
  `status` tinyint(1) NOT NULL DEFAULT '0' COMMENT '状态：1 开启；0 关闭',
  `source` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '来源',
  `data_status` tinyint(1) NOT NULL DEFAULT '0' COMMENT '数据状态：1 删除 0 正常',
  `create_time` datetime NOT NULL COMMENT '创建时间',
  `update_time` datetime NOT NULL COMMENT '更新时间',
  `hall_channel` tinyint(1) NOT NULL DEFAULT '0' COMMENT '测试渠道状态：1 是，0 否',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=63 DEFAULT CHARSET=utf8mb4 COMMENT='充值/提现启用渠道配置表';

/*Data for the table `config_cash` */

LOCK TABLES `config_cash` WRITE;

insert  into `config_cash`(`id`,`type`,`list`,`status`,`source`,`data_status`,`create_time`,`update_time`,`hall_channel`) values 
(1,1,'1,2,3,5',0,1226,1,'0000-00-00 00:00:00','2020-03-06 12:36:28',0),
(5,2,'1',0,1226,0,'0000-00-00 00:00:00','2019-11-27 18:37:50',0),
(6,1,'1,3',0,1226,1,'2019-01-30 16:08:56','2020-03-06 12:36:28',0),
(7,1,'1,2,3',0,1226,1,'2019-02-01 01:10:26','2020-03-06 12:36:28',0),
(8,2,'2,3',0,1226,1,'2019-02-01 01:14:10','2019-11-27 18:37:50',0),
(9,1,'1,2,3,5',0,1226,1,'2019-02-01 01:14:20','2020-03-06 12:36:28',0),
(10,2,'2,3,5,1',0,1226,1,'2019-02-01 01:14:32','2019-11-27 18:37:50',0),
(11,1,'2,3',0,1226,1,'2019-02-01 01:28:39','2020-03-06 12:36:28',0),
(12,1,'2,1',0,1226,1,'2019-02-20 14:25:36','2020-03-06 12:36:28',0),
(13,1,'2',0,1226,1,'2019-02-20 14:48:13','2020-03-06 12:36:28',0),
(14,1,'2',0,1226,1,'2019-02-20 14:49:09','2020-03-06 12:36:28',0),
(15,1,'1,2',0,1226,0,'2019-02-20 14:58:36','2019-03-07 14:14:10',1),
(16,1,'5,3,2,1',0,1226,0,'2019-02-20 14:59:01','2019-03-07 14:14:10',1),
(17,2,'2',0,1226,1,'2019-02-20 15:15:13','2019-11-27 18:37:50',0),
(18,1,'2',0,1226,1,'2019-02-20 15:16:23','2020-03-06 12:36:28',0),
(20,2,'3',0,1226,0,'2019-02-21 13:38:51','2019-03-14 16:15:22',1),
(21,2,'1,2',1,1226,0,'2019-02-22 17:13:10','2019-03-14 16:15:23',1),
(22,2,'1',0,1226,0,'2019-02-26 20:34:27','2019-03-14 16:15:22',1),
(23,1,'1,2,3,5',0,1226,0,'2019-03-06 14:18:00','2019-03-07 14:14:10',1),
(24,2,'1,2,3,5',0,1226,0,'2019-03-06 14:18:28','2019-03-14 16:15:22',1),
(25,1,'1',0,1226,1,'2019-03-06 17:07:38','2020-03-06 12:36:28',0),
(26,1,'3',0,1226,1,'2019-03-06 17:07:51','2020-03-06 12:36:28',0),
(27,1,'5',0,1226,1,'2019-03-06 17:08:18','2020-03-06 12:36:28',0),
(28,1,'1',0,1226,0,'2019-03-06 17:51:17','2019-03-07 14:14:10',1),
(29,1,'2,3',0,1226,0,'2019-03-07 13:51:57','2019-03-07 14:14:10',1),
(30,1,'1,2',0,1226,1,'2019-03-07 13:52:50','2020-03-06 12:36:28',0),
(31,1,'5',0,1226,1,'2019-03-07 15:37:48','2019-03-07 15:37:56',1),
(32,1,'2',0,1226,1,'2019-04-08 21:21:07','2020-03-06 12:36:28',0),
(33,1,'1,2,3',0,1226,0,'2019-04-17 17:36:28','2020-03-06 12:36:28',0),
(34,2,'2',0,1226,0,'2019-05-08 16:03:28','2019-11-27 18:37:50',0),
(35,2,'2',0,1226,0,'2019-05-08 16:08:29','2019-11-27 18:37:50',0),
(36,2,'1,2',0,1226,1,'2019-05-09 15:29:41','2019-11-27 18:37:50',0),
(37,1,'2',0,1226,1,'2019-05-13 10:59:07','2020-03-06 12:36:28',0),
(38,1,'2',0,1226,0,'2019-05-13 11:02:44','2020-03-06 12:36:28',0),
(39,1,'1',0,1226,1,'2019-05-13 11:03:19','2020-03-06 12:36:28',0),
(40,1,'5',0,1226,1,'2019-05-13 11:28:57','2020-03-06 12:36:28',0),
(41,1,'5',0,1226,1,'2019-05-13 11:36:33','2020-03-06 12:36:28',0),
(42,1,'1,2,3,5',0,1226,0,'2019-05-21 18:51:28','2020-03-06 12:36:28',0),
(43,1,'1',0,1226,0,'2019-06-19 16:58:30','2020-03-06 12:36:28',0),
(44,1,'3',0,1226,0,'2019-06-20 16:37:53','2020-03-06 12:36:28',0),
(45,2,'1,2,3',1,1226,0,'2019-06-24 16:14:35','2019-11-27 18:37:51',0),
(46,1,'1,2',0,1226,0,'2019-07-02 19:59:01','2020-03-06 12:36:28',0),
(47,1,'2,3,5,1',0,1226,0,'2019-09-27 13:10:42','2020-03-06 12:36:28',0),
(48,1,'1,2,5',0,1226,0,'2019-09-27 13:48:26','2020-03-06 12:36:28',0),
(49,1,'5',0,1226,0,'2019-09-27 13:48:51','2020-03-06 12:36:28',0),
(50,1,'3,5',0,1226,1,'2019-09-30 15:49:28','2020-03-06 12:36:28',0),
(51,1,'5,2',0,1226,1,'2019-10-14 16:37:07','2020-03-06 12:36:28',0),
(52,2,'1,2',0,1226,1,'2019-11-12 14:35:22','2019-11-27 18:37:50',0),
(53,2,'1,2',0,1226,1,'2019-11-12 16:14:17','2019-11-27 18:37:50',0),
(54,2,'1,2',0,1226,0,'2019-11-12 17:23:56','2019-11-27 18:37:50',0),
(55,2,'5',0,1226,1,'2019-11-12 18:47:24','2019-11-27 18:37:50',0),
(56,1,'1,2,3,5',0,0,0,'2019-11-20 10:21:36','2020-03-06 12:36:28',0),
(57,1,'2,3,1',1,0,0,'2019-11-20 10:21:50','2020-03-26 16:02:07',0),
(58,1,'1,2',0,0,1,'2019-11-20 11:59:39','2020-03-06 12:36:28',0),
(59,1,'1,3,2',0,1226,0,'2019-11-21 10:43:45','2020-03-06 12:36:28',0),
(60,1,'1',0,0,1,'2019-11-25 11:31:14','2020-03-06 12:36:28',0),
(61,1,'2',0,0,1,'2019-11-27 18:37:23','2020-03-06 12:36:28',0),
(62,2,'5',0,0,1,'2019-11-27 18:37:44','2019-11-27 18:37:53',0);

UNLOCK TABLES;

/*Table structure for table `config_pay` */

DROP TABLE IF EXISTS `config_pay`;

CREATE TABLE `config_pay` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `amounts` varchar(100) NOT NULL DEFAULT '' COMMENT '支付列表，最多6个（用逗号隔开）',
  `minAmount` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '支付最小值(分）',
  `maxAmount` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '支付最大值(分）',
  `payType` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '支付类型：1:银联 2:支付宝 3:微信 4:支付宝扫码 5:微信扫码',
  `create_time` datetime NOT NULL COMMENT '创建时间',
  `update_time` datetime NOT NULL COMMENT '更新时间',
  `status` tinyint(1) NOT NULL DEFAULT '0' COMMENT '状态：1 开启；0 关闭',
  `source` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '来源(1126）',
  `channel` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '支付渠道ID',
  `channel_name` varchar(30) NOT NULL DEFAULT '' COMMENT '支付渠道名称',
  `data_status` tinyint(1) NOT NULL DEFAULT '0' COMMENT '数据状态：1 删除 0 正常',
  `scan` tinyint(1) NOT NULL DEFAULT '0' COMMENT '状态：false 生成二维码；true 不生成二维码',
  `hall_channel` tinyint(1) NOT NULL DEFAULT '0' COMMENT '测试渠道状态：1 是，0 否',
  `guide` varchar(100) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_by_configId` (`channel`) COMMENT '主键，与支付表的configId关联',
  KEY `config_pay_create_time` (`create_time`),
  KEY `config_pay_update_time` (`update_time`)
) ENGINE=InnoDB AUTO_INCREMENT=301 DEFAULT CHARSET=utf8mb4 COMMENT='充值渠道配置表';

/*Data for the table `config_pay` */

LOCK TABLES `config_pay` WRITE;

insert  into `config_pay`(`id`,`amounts`,`minAmount`,`maxAmount`,`payType`,`create_time`,`update_time`,`status`,`source`,`channel`,`channel_name`,`data_status`,`scan`,`hall_channel`,`guide`) values 
(294,'10000,20000,50000',10000,10000,7,'2020-03-10 21:47:52','2020-03-10 21:47:52',0,1226,6,'QQ',0,0,0,''),
(295,'5000,20000,30000,50000,100000,500000,600000',5000,600000,2,'2020-03-10 21:48:43','2020-03-10 21:48:43',1,1226,7,'支付宝',0,0,0,''),
(296,'50000,80000,100000,200000',50000,290000,3,'2020-03-10 21:54:11','2020-03-18 19:40:13',1,1226,8,'微信',0,0,0,''),
(297,'80000,100000,200000,500000,800000,1000000',60000,1000000,1,'2020-03-10 22:25:27','2020-03-26 16:02:07',1,1226,9,'银联',0,0,0,''),
(300,'12300,12300,7899800',1000,20000,6,'2020-03-11 14:53:50','2020-03-11 14:53:50',0,1226,10,'京东',0,0,0,'');

UNLOCK TABLES;

/*Table structure for table `dict_pay` */

DROP TABLE IF EXISTS `dict_pay`;

CREATE TABLE `dict_pay` (
  `payId` int(11) NOT NULL COMMENT '支付ID',
  `payCode` varchar(6) NOT NULL COMMENT '支付编码',
  `payName` varchar(32) NOT NULL COMMENT '支付名称',
  PRIMARY KEY (`payId`),
  UNIQUE KEY `idx_by_code` (`payCode`),
  UNIQUE KEY `idx_by_name` (`payName`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

/*Data for the table `dict_pay` */

LOCK TABLES `dict_pay` WRITE;

insert  into `dict_pay`(`payId`,`payCode`,`payName`) values 
(1,'UN','银联'),
(2,'ALI','支付宝'),
(3,'WX','微信'),
(6,'JD','京东'),
(7,'QQ','QQ');

UNLOCK TABLES;

/* Procedure structure for procedure `pAddPayMethod` */

/*!50003 DROP PROCEDURE IF EXISTS  `pAddPayMethod` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pAddPayMethod`(
        IN param_amounts varchar(100),      -- 常用支付金额，以,分隔开
        IN param_minAmount int unsigned,
        in param_maxAmount int unsigned,
        IN param_payMethod varchar(32),     -- UN=>银联；ALI=>支付宝；WX=>微信; JD=>京东支付
        in param_qrcode int,                -- 是否生成QRCode， 0=>生成QRCode, 1=>不生成QRCode
        IN param_isTest int                 -- 是否测试渠道，1=>测试渠道，0=>正式渠道
    )
BEGIN
        declare var_code        int default 0;
        declare var_message     varchar(128) default 'OK';
        declare var_channelId   int unsigned default 1;
        declare var_payChannel  varchar(30) default '未知';
        declare var_payType     int default -1;
        declare var_cash_list   varchar(30) default '';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message, 0;
        END;
        
        select IFnull(max(`channel`), 1)+1 INTO var_channelId from `config_pay`;

label_main: begin
        IF NOT EXISTS (select * from `dict_pay` where `payCode`=param_payMethod) then
        begin
            set var_code = -1, var_message='unsupport pay method';
            leave label_main;
        end;
        end if;
        select `payId`,`payName` INTO var_payType, var_payChannel from `dict_pay` where `payCode`=param_payMethod;
       
        -- 若存在相央的method，则返回错误
        if exists (select * from `config_pay` where `payType`=var_payType and `status`=1 and `data_status`=0 and `hall_channel`=param_isTest) then
        begin
            set var_code = -3, var_message=concat('already exists pay method :', param_payMethod, '(', var_payChannel,')');
            leave label_main;
        end;
        end if;

        begin
        
        -- 写入到数据库中去  
        insert into `config_pay`(`amounts`,`minAmount`,`maxAmount`,`payType`,`create_time`,
            `update_time`, `status`,`source`,`channel`,`channel_name`,
            `data_status`,`scan`,`hall_channel`,`guide`)
        values(param_amounts, param_minAmount, param_maxAmount, var_payType, NOW(),
            NOW(), 1, 1226, var_channelId, var_payChannel,
            0, param_qrcode, param_isTest, '');
        if (row_count() <= 0) then
        begin
            set var_code=-5, var_message='add new pay method failure';
            rollback;
            leave label_main;
        end;
        end if;

        -- 获得支付列表
        select GROUP_CONCAT(`payType` SEPARATOR ',') into var_cash_list
        from `config_pay` 
        WHERE `data_status`=0 AND `status`=1 and `hall_channel`=param_isTest;
        
        -- 更新支付列表 (TODO: 排序规则？)
        if exists (select * from `config_cash` where `data_status`=0 AND `status`=1 AND `type`=1 AND `hall_channel`=param_isTest) then
        begin
            update `config_cash` 
            set `list`=var_cash_list, `update_time`=NOW() 
            where `data_status`=0 AND `status`=1 AND `type`=1 AND `hall_channel`=param_isTest;
        end;
        else
        begin
            INSERT into `config_cash`(`type`, `list`, `status`, `source`, `data_status`, `create_time`, `update_time`, `hall_channel`)
            valueS(1, var_cash_list, 1, 1226, 0, NOW(), NOW(), param_isTest);
        end;
        end if;
        IF (ROW_COUNT() <=0) THEN
        BEGIN
            set var_code=-7, var_message='update config_cash failure';
            rollback;
            leave label_main;
        END;
        END IF;
        
        commit;
        end;
        
        end label_main; 
        
        select var_code, var_message, var_channelId;
        
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pCheckAuthorize` */

/*!50003 DROP PROCEDURE IF EXISTS  `pCheckAuthorize` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pCheckAuthorize`(
				IN p_uuid			VARCHAR (40),
				IN p_gameId		VARCHAR (40)
    )
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) 	DEFAULT 'OK';
				DECLARE var_uuid				VARCHAR(40)		DEFAULT '';
        DECLARE var_secret			VARCHAR(64)		DEFAULT '';
				-- DECLARE var_diamond			bigint(20)		DEFAULT 0;
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message, 0;
        END;
        
label_main: BEGIN
				-- 查询游戏是否存在
				SELECT client_secret INTO var_secret
				FROM dt_hall.config_game 
				WHERE client_id = p_gameId AND `show` = 1 AND `open` = 1 AND data_status = 0 LIMIT 1;
				
				IF var_secret = '' THEN
				BEGIN
						SET var_code=-1, var_message='gameId not found.';
						LEAVE label_main;
				END;
				END IF;
				
				-- 查询用户
				SELECT	uuid-- , diamond 
				INTO var_uuid-- , var_diamond
				FROM dt_hall.account
				WHERE uuid = p_uuid AND data_status = 0;
END label_main; 

        SELECT var_code, var_message, var_uuid, var_secret;
END */$$
DELIMITER ;

/* Procedure structure for procedure `pCheckExchange` */

/*!50003 DROP PROCEDURE IF EXISTS  `pCheckExchange` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pCheckExchange`(IN p_uuid					VARCHAR (40),
				IN p_gameId				VARCHAR (40),
				IN p_orderId			VARCHAR (40),
				IN p_thirdOrderId	VARCHAR (40),
				IN p_type					INT(2),
				IN p_count				bigint(20))
BEGIN
        DECLARE var_code        		INT DEFAULT 0;
        DECLARE var_message     		VARCHAR(128) 	DEFAULT 'OK';
				DECLARE var_uuid						VARCHAR(40)		DEFAULT '';
        DECLARE var_secret					VARCHAR(64)		DEFAULT '';
        DECLARE var_diamond					bigint(20)		DEFAULT 0;
        DECLARE var_order_id				VARCHAR(64)		DEFAULT '';
				DECLARE var_tourist					VARCHAR(64)		DEFAULT '';
				DECLARE var_token						VARCHAR(64)		DEFAULT '';
				DECLARE var_last_id					INT						DEFAULT 0;
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message, 0;
        END;
        
label_main: BEGIN
				-- 查询游戏是否存在
				SELECT client_secret INTO var_secret
				FROM dt_hall.config_game 
				WHERE client_id = p_gameId AND `show` = 1 AND `open` = 1 AND data_status = 0 LIMIT 1;
				
				IF var_secret = '' THEN
				BEGIN
						SET var_code=-1, var_message='gameId not found.';
						LEAVE label_main;
				END;
				END IF;
				
				-- 创建订单
				IF p_type = 1 THEN
						INSERT INTO `dt_hall`.`log_exchange`(`orderId`, `uuid`, `from`, `from_rate`, `to`, `to_rate`, `count`, `exchange_count`, `type`, `third_order`, `create_time`, `update_time`)
				VALUES (p_orderId, p_uuid, '999999', 1, p_gameId, 1, p_count, p_count, 1, p_thirdOrderId, NOW(), NOW());
				ELSE
						INSERT INTO `dt_hall`.`log_exchange`(`orderId`, `uuid`, `from`, `from_rate`, `to`, `to_rate`, `count`, `exchange_count`, `type`, `third_order`, `create_time`, `update_time`)
				VALUES (p_orderId, p_uuid, p_gameId, 1, '999999', 1, p_count, p_count, 1, p_thirdOrderId, NOW(), NOW());
				END IF;
				
				SELECT LAST_INSERT_ID() INTO var_last_id;
				
				-- 更新用户信息
				IF p_count > 0 THEN
						IF p_type = 1 THEN
								UPDATE dt_hall.account SET diamond = diamond - p_count, update_time = NOW() WHERE uuid = p_uuid;
						ELSE
								UPDATE dt_hall.account SET diamond = diamond + p_count, update_time = NOW() WHERE uuid = p_uuid;
						END IF;
						IF (ROW_COUNT() <= 0) THEN
						BEGIN
								SET var_code=-3, var_message='create user uuid failure.';
								ROLLBACK;
								LEAVE label_main;
						END;
						END IF;
				END IF;
				
				-- 更新订单状态
				UPDATE dt_hall.log_exchange SET `status` = 1, update_time = NOW() WHERE orderId = p_orderId;
				IF (ROW_COUNT() <= 0) THEN
				BEGIN
						SET var_code=-4, var_message='create user uuid failure.';
						ROLLBACK;
						LEAVE label_main;
				END;
				END IF;
				
				-- 查询用户
				SELECT	uuid, diamond, tourist, token
				INTO	var_uuid, var_diamond, var_tourist, var_token
				FROM dt_hall.account
				WHERE uuid = p_uuid AND data_status = 0;
				
				COMMIT;

END label_main; 

        SELECT var_code, var_message, var_uuid, var_secret, var_diamond, var_order_id, var_tourist, var_token;
END */$$
DELIMITER ;

/* Procedure structure for procedure `pCheckInsertUser` */

/*!50003 DROP PROCEDURE IF EXISTS  `pCheckInsertUser` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pCheckInsertUser`(
        IN ptype			VARCHAR (20),
				IN p_tourist	VARCHAR (64),
				IN p_nick			VARCHAR (20),
				IN p_head			VARCHAR (200),
				IN p_sex			VARCHAR (2),
				IN p_phone		VARCHAR (11),
				IN p_password VARCHAR (32),
				IN p_agentId	VARCHAR (20),
				IN p_token		VARCHAR (64)
    )
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) 	DEFAULT 'OK';
        DECLARE var_uuid				VARCHAR(40)		DEFAULT '';
				DECLARE var_nick				VARCHAR(20)		DEFAULT '';
				DECLARE var_head				VARCHAR(200)	DEFAULT '';
				DECLARE var_sex					VARCHAR(2)		DEFAULT '';
				DECLARE var_agent       VARCHAR(20)		DEFAULT '';
				DECLARE var_agentPhone	VARCHAR(11)		DEFAULT '';
				DECLARE var_password		VARCHAR(32)		DEFAULT '';
				DECLARE var_last_id			INT						DEFAULT 0;
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message, 0;
        END;
        
label_main: BEGIN
				-- 查询用户是否存在
				IF ptype = 'tourist' || ptype = 'thirdPlatform' THEN
						SELECT	uuid, nick, head, sex, agent_id INTO var_uuid, var_nick, var_head, var_sex, var_agent
						FROM dt_hall.account
						WHERE tourist = p_tourist AND data_status = 0;
				ELSE
						SELECT	uuid, nick, head, sex, agent_id INTO var_uuid, var_nick, var_head, var_sex, var_agent
						FROM dt_hall.account
						WHERE phone = p_phone AND data_status = 0;
				END IF;
	
				IF ISNULL(var_uuid) THEN
						-- 查询agentId是否存在
						IF p_agentId <> "0" THEN
								SELECT phone INTO var_agentPhone FROM dt_hall.account WHERE uuid = p_agentId AND data_status = 0;
								IF var_agentPhone = '' THEN
										SET p_agentId = '0';
								END IF;
						END IF;
		
						-- 新建账号
						INSERT INTO `dt_hall`.`account`(`uuid`, `nick`, `sex`, `head`, `tourist`, `phone`, `password`, `agent_id`, `token`, `into_game_time`, `bind_phone_time`, `create_time`, `update_time`) VALUES ('000000', p_nick, p_sex, p_head, p_tourist, p_phone, p_password, p_agentId, p_token, NOW(), NOW(), NOW(), NOW());
						IF (ROW_COUNT() <= 0) THEN
						BEGIN
								SET var_code=-2, var_message='create user failure.';
								ROLLBACK;
								LEAVE label_main;
						END;
						END IF;

						-- 根据 insert ID 获取对应 uuid 并更新 account 表 uuid信息
						SELECT LAST_INSERT_ID() INTO var_last_id;
						SELECT uuid INTO var_uuid FROM dt_hall.user_uuid WHERE id = var_last_id;
						
						IF ptype = 'tourist' THEN
								SET p_nick = CONCAT('游客', var_uuid);
						END IF;
						UPDATE dt_hall.account SET uuid = var_uuid, nick = p_nick, token = p_token WHERE id = var_last_id;
						IF (ROW_COUNT() <= 0) THEN
						BEGIN
								SET var_code=-3, var_message='create user uuid failure.';
								ROLLBACK;
								LEAVE label_main;
						END;
						END IF;
						
						SET var_nick	= p_nick;
						SET var_head	= p_head;
						SET var_sex		= p_sex;
						SET var_agent = p_agentId;
						
						COMMIT;
				ELSE
						IF ptype = 'phone' THEN
								IF MD5(p_password) <> var_password THEN
										BEGIN
												SET var_code = -1, var_message='password mistake.';
												LEAVE label_main;
										END;
								END IF;
						END IF;
				END IF;
END label_main; 

        SELECT var_code, var_message, var_uuid, var_nick, var_head, var_sex, var_agent;
END */$$
DELIMITER ;

/* Procedure structure for procedure `pCheckQueryCoins` */

/*!50003 DROP PROCEDURE IF EXISTS  `pCheckQueryCoins` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pCheckQueryCoins`(
				IN p_uuid			VARCHAR (40),
				IN p_gameId		VARCHAR (40),
				IN p_enterId	VARCHAR (40)
    )
BEGIN
        DECLARE var_code        		INT DEFAULT 0;
        DECLARE var_message     		VARCHAR(128) 	DEFAULT 'OK';
				DECLARE var_uuid						VARCHAR(40)		DEFAULT '';
        DECLARE var_secret					VARCHAR(64)		DEFAULT '';
        DECLARE var_diamond					bigint(20)		DEFAULT 0;
        DECLARE var_award_diamond		bigint(20)		DEFAULT 0;
        DECLARE var_donate					bigint(20)		DEFAULT 0;
        DECLARE var_session_id			VARCHAR(64)		DEFAULT '';
        DECLARE var_enter_id				VARCHAR(64)		DEFAULT '';
				DECLARE var_tourist					VARCHAR(64)		DEFAULT '';
				DECLARE var_token						VARCHAR(64)		DEFAULT '';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message, 0;
        END;
        
label_main: BEGIN
				-- 查询游戏是否存在
				SELECT client_secret INTO var_secret
				FROM dt_hall.config_game 
				WHERE client_id = p_gameId AND `show` = 1 AND `open` = 1 AND data_status = 0 LIMIT 1;
				
				IF var_secret = '' THEN
				BEGIN
						SET var_code=-1, var_message='gameId not found.';
						LEAVE label_main;
				END;
				END IF;
				
				-- 更新用户信息
				UPDATE dt_hall.account SET enter_id = p_enterId, update_time = NOW() WHERE uuid = p_uuid;
				IF (ROW_COUNT() <= 0) THEN
				BEGIN
						SET var_code=-2, var_message='create user uuid failure.';
						ROLLBACK;
						LEAVE label_main;
				END;
				END IF;
				
				-- 查询用户
				SELECT	uuid, diamond, award_diamond, donate, session_id, enter_id, tourist, token
				INTO	var_uuid, var_diamond, var_award_diamond, var_donate, var_session_id, var_enter_id, var_tourist, var_token
				FROM dt_hall.account
				WHERE uuid = p_uuid AND data_status = 0;
				
				COMMIT;
				
END label_main; 

        SELECT var_code, var_message, var_uuid, var_secret, var_diamond, var_award_diamond, var_donate, var_session_id, var_enter_id, var_tourist, var_token;
END */$$
DELIMITER ;

/* Procedure structure for procedure `pCreateAgent2` */

/*!50003 DROP PROCEDURE IF EXISTS  `pCreateAgent2` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pCreateAgent2`(
        IN param_parentUuid         VARCHAR(40),  -- 父代理的UUID
        IN param_nick               VARCHAR(64),  -- 子代理的昵称,允许为空
        IN param_passwd             VARCHAR(32),  -- 子代理的密码
        IN param_ratio              double,       -- 分成比例,*10000
        IN param_ip                 VARCHAR(16)   -- 父代理操作时的IP
    )
    COMMENT '新增代理2'
BEGIN
        DECLARE var_account     VARCHAR(11);
        DECLARE var_ipaddr      VARCHAR(255) default '';
        DECLARE var_nick        VARCHAR(20) DEFAULT '';
        DECLARE var_id          INT UNSIGNED;
        DECLARE var_uuid        VARCHAR(40) DEFAULT '';
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_msg         VARCHAR(64) DEFAULT 'OK';
        DECLARE var_diamond     INT UNSIGNED DEFAULT 300;
        DECLARE var_uuid_int64  BIGINT UNSIGNED DEFAULT 0;
        DECLARE var_channel     VARCHAR(50) default '';
        DECLARE var_parentRatio double default 0.1;
        DECLARE var_ratio       double default 0.05;
        

        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1 = RETURNED_SQLSTATE, @p2 = MESSAGE_TEXT;
            ROLLBACK;
            SELECT -1999, CONCAT('execute pCreateAgent2 failure, SQLState:', @p1, ',Message:', @p2);
        END;
    
        SELECT CAST(IFNULL(MAX(phone),'12300010001')+1 AS CHAR) INTO var_account 
        FROM account 
        WHERE phone 
        LIKE '1230001%';
    
        SELECT IFNULL(CONCAT(`country`,`province`,`city`),'系统预留') INTO var_ipaddr 
        FROM ip_data 
        WHERE `end`>INET_ATON(param_ip)
        ORDER BY `end` 
        LIMIT 1;
    
        SELECT `channel` INTO var_channel FROM `account` WHERE `uuid`=param_parentUuid;
        SELECT `ratio` INTO var_parentRatio FROM `hall_dc`.`user_summary` WHERE `uuid`=param_parentUuid;
        IF param_ratio >= var_parentRatio THEN
            SET var_ratio = var_parentRatio;
        ELSE
            SET var_ratio = param_ratio;
        END IF;
        
        

        START TRANSACTION;
LABEL_MAIN: BEGIN
        -- dt_hall库
        -- 增加代理账号 替换手机号部分  12345000000
        INSERT INTO dt_hall.`account` (`uuid`,`sex`, `head`, `diamond`, `withdraw`, `profit`,
            `donate`, `pay`, `pay_time`, `flag`, `guaranteed`, `is_access`, 
            `award_diamond`, `award_exchange`, `tourist`, `phone`, `password`, `ip`, 
            `channel`, `address`, `is_online`, `auto_in`, `auto_out`, `invite`, 
            `agent_id`, `game_id`, `into_game_time`, `bind_phone_time`, `is_black`, `first`,
            `imie`, `session_id`, `enter_id`, `create_time`, `update_time`, `source`,
            `data_status`, `money`, `is_bind_withdraw`, `os`, `is_voice`, `is_suspect`, 
            `is_special`, `online_time`, `agent_type`,`nick`) 
        VALUES('0000', '2', '7.png', var_diamond, 0, 0,
            300, 0, 1576678209, 1, 0, 0,
            0, 0, '',var_account, param_passwd, param_ip,
            var_channel, var_ipaddr, 0, 1, 1, '',
            param_parentUuid, '', NOW(), NOW(), 0, 1,
            '', '', '',NOW(),NOW(), 1226,
            0, 0, 0, 'PCWEB', 0, 0,
            0, 0, 2, var_nick);
        IF (ROW_COUNT() = 0) THEN
        BEGIN
            ROLLBACK;
            SET var_code=-1000, var_msg='add new account failure.';
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        SET var_id = LAST_INSERT_ID();
        
        SELECT `uuid`,IF(param_nick ='', CONCAT('推广',`uuid`),param_nick) INTO var_uuid, var_nick
        FROM `user_uuid`
        WHERE `id`=var_id;
        IF (var_uuid = '') THEN
        BEGIN
            ROLLBACK;
            SET var_code=-1001, var_msg='can not found uuid in user_uuid table.';
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        UPDATE `account`
        SET `uuid`=var_uuid, `nick`=var_nick
        WHERE `id`=var_id;
        IF (ROW_COUNT() = 0) THEN
        BEGIN
            ROLLBACK;
            SET var_code=-1002, var_msg='update account uuid failure.';
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        -- hall_dc库
        -- 增加hall_dc数据
        INSERT INTO hall_dc.user_agent_summary (`uuid`, agent_uuid)
        VALUES(var_uuid, param_parentUuid);
        IF (ROW_COUNT() = 0) THEN
        BEGIN
            ROLLBACK;
            SET var_code=-1003, var_msg='add agent map failure.';
            LEAVE LABEL_MAIN;
        END;
        END IF;

        INSERT INTO hall_dc.`user_summary`(`uuid`, nick, create_time, create_ip, create_addr,
            create_channel, register_time, register_phone, money, ratio)
        VALUES(var_uuid, var_nick, NOW(), param_ip, var_ipaddr,
            var_channel, NOW(), var_account, var_diamond, var_ratio);
        IF (ROW_COUNT() = 0) THEN
        BEGIN
            ROLLBACK;
            SET var_code=-1005, var_msg='add agent summary failure.';
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        -- 增加父子关系
        INSERT INTO hall_dc.`agent_forward` (`uuid`, `child_uuid`, `level`, `create_time`)
        VALUES (param_parentUuid, var_uuid, 1, NOW());
        IF (ROW_COUNT() = 0) THEN
        BEGIN
            ROLLBACK;
            SET var_code=-1006, var_msg='add agent_forward map failure';
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        -- 增加祖孙关系
        INSERT INTO hall_dc.`agent_forward` (`uuid`, `child_uuid`, `level`, `create_time`)
        SELECT `uuid`, var_uuid, `level` + 1, NOW()
        FROM hall_dc.`agent_forward`
        WHERE `child_uuid` = param_parentUuid AND `uuid` != 0;
        
        COMMIT;
        
        -- 增加代理数据团队、直属数量
        CALL hall_dc.pAgentUpdateCount(var_uuid);  
END LABEL_MAIN;
        
        SELECT var_code, var_msg, var_id, var_uuid, var_account;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pDelPayMethod` */

/*!50003 DROP PROCEDURE IF EXISTS  `pDelPayMethod` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pDelPayMethod`(IN param_configId int)
BEGIN
        declare var_code        int default 0;
        declare var_message     varchar(128) default 'OK';
        declare var_isTest      int default 0;
        declare var_cash_list   varchar(30) default '';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
        
        select `hall_channel` into var_isTest from `config_pay` where `channel`=param_configId;

LABEL_MAIN: BEGIN
        
        begin   -- 启动事务
        
        delete from `config_pay` where `channel`=param_configId;
        if (row_count() <=0) then
        begin
            rollback;
            set var_code=-1, var_message='remove pay method failure';
            LEAVE LABEL_MAIN;
        end;
        end if;
        
        if exists (select * from `config_pay` where `data_status`=0 AND `status`=1 AND `hall_channel`=var_isTest) then
        begin
            -- 获得支付列表  
            SELECT GROUP_CONCAT(`payType` SEPARATOR ',') INTO var_cash_list
            FROM `config_pay` 
            WHERE `data_status`=0 AND `status`=1 AND `hall_channel`=var_isTest;
        
            UPDATE `config_cash` 
            SET `list`=var_cash_list, `update_time`=NOW() 
            WHERE `data_status`=0 AND `status`=1 AND `type`=1 AND `hall_channel`=var_isTest;
            IF (ROW_COUNT() <=0) THEN
            BEGIN
                ROLLBACK;
                SET var_code=-3, var_message='update cash list failure';
                LEAVE LABEL_MAIN;
            END;
            end if;
        end;
        else
        begin
            delete from `config_cash`
            WHERE `data_status`=0 AND `status`=1 AND `type`=1 AND `hall_channel`=var_isTest;
            IF (ROW_COUNT() <=0) THEN
            BEGIN
                ROLLBACK;
                SET var_code=-5, var_message='remove cash list failure';
                LEAVE LABEL_MAIN;
            END;
            end if;
        end;
        end if;
        
        set var_code=0, var_message='OK';
        commit;  -- 提交事务
        end;
        
        end LABEL_MAIN;
        
        select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pEnablePayMethod` */

/*!50003 DROP PROCEDURE IF EXISTS  `pEnablePayMethod` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pEnablePayMethod`(IN param_configId int, in param_enable int )
BEGIN
        declare var_code        int default 0;
        declare var_message     varchar(128) default 'OK';
        declare var_cash_list   varchar(128);
        declare var_isTest      int default 0;
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message, 0;
        END;

LABEL_MAIN: begin        
        update `config_pay` set `status`=param_enable where `channel`=param_configId;
        IF (ROW_COUNT()<=0) then
        begin
            set var_code=-1, var_message='enable/disable pay method failure';
            leave LABEL_MAIN;
        end;
        end if;
        
        -- 获得支付列表
        SELECT GROUP_CONCAT(`payType` SEPARATOR ',') INTO var_cash_list
        FROM `config_pay` 
        WHERE `data_status`=0 AND `status`=1 AND `hall_channel`=var_isTest;
        
        -- 更新支付列表 (TODO: 排序规则？)
        IF EXISTS (SELECT * FROM `config_cash` WHERE `data_status`=0 AND `status`=1 AND `type`=1 AND `hall_channel`=var_isTest) THEN
        BEGIN
            UPDATE `config_cash` 
            SET `list`=var_cash_list, `update_time`=NOW() 
            WHERE `data_status`=0 AND `status`=1 AND `type`=1 AND `hall_channel`=var_isTest;
        END;
        ELSE
        BEGIN
            INSERT INTO `config_cash`(`type`, `list`, `status`, `source`, `data_status`, `create_time`, `update_time`, `hall_channel`)
            VALUES(1, var_cash_list, 1, 1226, 0, NOW(), NOW(), var_isTest);
        END;
        END IF;
        IF (ROW_COUNT() <=0) THEN
        BEGIN
            SET var_code=-3, var_message='update config_cash failure';
            ROLLBACK;
            LEAVE label_main;
        END;
        END IF;
        
        SELECT concat(channel, case `status` when 1 then ' Enable SUCCEED' else ' Stopped' end) into var_message  
        from `config_pay` where `channel`=param_configId;
        end LABEL_MAIN;
        
        select var_code, var_message;
        
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pGetDictPay` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetDictPay` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetDictPay`()
BEGIN
        select `payId` as `id`,`payCode` as `code`,`payName` as `name` from `dict_pay` order by `payId`;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pGetPayMethod` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetPayMethod` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetPayMethod`(in p_record_start int,in p_record_num int)
begin
	select distinct a.channel,
				 a.channel_name,
				 a.amounts,
				 a.minAmount,
				 a.maxAmount,
				 `status`,
				 a.`id`,
				 b.paycode
	from dt_hall.config_pay a
	inner join dt_hall.dict_pay b
	  on a.payType = b.payid
	where data_status = 0
	limit p_record_start,p_record_num;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetPayMethodCNT` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetPayMethodCNT` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetPayMethodCNT`()
begin
	select count(*) from(
	select distinct a.channel,
				 a.channel_name,
				 a.amounts,
				 minAmount,
				 maxAmount,
				 `status`,
				 a.id
	from dt_hall.config_pay a 
	where data_status = 0
	) x;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetUserInfo` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetUserInfo` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetUserInfo`(in param_uuid varchar(40))
BEGIN
        select `uuid`,`nick`,`channel`,`head`,`diamond`,`withdraw`,`phone`,`create_time`,`bind_phone_time` as 'register_time','' as 'version','' as 'hot_version', 'chs' as 'lang',`id`
        from `account`
        where `uuid`=param_uuid;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pGetUserInfoForPaycenter` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetUserInfoForPaycenter` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetUserInfoForPaycenter`(in param_uin varchar(40) )
BEGIN
        SELECT `nick`,`channel`,`create_time` FROM `account` WHERE `uuid`=param_uin;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pModifyAgent2Bank` */

/*!50003 DROP PROCEDURE IF EXISTS  `pModifyAgent2Bank` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pModifyAgent2Bank`(
        IN param_uuid         VARCHAR(40),       -- 代理的UUID
        IN param_accountName  VARCHAR(40),       -- 账户名
        IN param_bankName     VARCHAR(40),       -- 银行名
        IN param_bankCode     VARCHAR(40),       -- 银行编码
        IN param_cardCode     VARCHAR(40),       -- 银行卡号
        IN param_province     varchar(10),       -- 银行卡省份
        IN param_city         varchar(10)        -- 银行卡城市
    )
    COMMENT '修改银行卡'
BEGIN
  DECLARE var_uuid        varchar(40);
  DECLARE var_code        INT DEFAULT 0;
  DECLARE var_msg         VARCHAR(64) DEFAULT 'OK';
--   DECLARE var_identity    varchar(30) DEFAULT '';
--   DECLARE var_mobile      varchar(15) DEFAULT '';
  DECLARE var_bank_code   varchar(10); 


  DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        GET DIAGNOSTICS CONDITION 1 @p1 = RETURNED_SQLSTATE, @p2 = MESSAGE_TEXT;
        ROLLBACK;
        SELECT -1999, CONCAT('execute pModifyAgent2Bank failure, SQLState:', @p1, ',Message:', @p2);
    END;
  
  set var_uuid = param_uuid;

  START TRANSACTION;
  LABEL_MAIN: BEGIN
      IF EXISTS(SELECT 1 FROM `dt_hall`.`withdraw_bind` WHERE uuid = var_uuid) then
        -- 修改
        UPDATE `dt_hall`.`withdraw_bind`
          SET account_name = param_accountName
              ,bank_name = param_bankName
              ,bank_code = param_bankCode
              ,account_no = param_cardCode
              ,province = param_province
              ,city = param_city
              ,update_time = now()
        WHERE UUID = var_uuid;
        IF (ROW_COUNT() = 0) THEN
        BEGIN
            ROLLBACK;
            SET var_code=-1002, var_msg='update withdraw_bind failure.';
            LEAVE LABEL_MAIN;
        END;
        END IF;
     ELSE
        -- 新增
        INSERT INTO `dt_hall`.`withdraw_bind`(uuid,type,identity,mobile,bank_code,bank_name,card_type,account_type,account_name,account_no,province,city,create_time,update_time,source,data_status)
        VALUES(param_uuid,1,'','',param_bankCode,param_bankName,'debit','B2C',param_accountName,param_cardCode,param_province,param_city,NOW(),now(),0,0);
        IF (ROW_COUNT() = 0) THEN
        BEGIN
            ROLLBACK;
            SET var_code=-1002, var_msg='insert withdraw_bind failure.';
            LEAVE LABEL_MAIN;
        END;
        END IF;
      END IF;
    COMMIT;
  END LABEL_MAIN;
  SELECT var_code, var_msg;

END */$$
DELIMITER ;

/* Procedure structure for procedure `pModifyAgent2Ratio` */

/*!50003 DROP PROCEDURE IF EXISTS  `pModifyAgent2Ratio` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pModifyAgent2Ratio`(
        IN param_parentUuid         VARCHAR(40),  -- 父级代理的UUID
        IN param_uuid         VARCHAR(40),  -- 代理的UUID
        IN param_ratio              double       -- 分成比例,*10000
    )
    COMMENT '修改分成比例'
BEGIN
  DECLARE var_uuid        varchar(40);
  DECLARE var_parentUuid        varchar(40);
  DECLARE var_parentRatio double default 0.1;
  DECLARE var_ratio       double default 0.05;
  DECLARE var_code        INT DEFAULT 0;
  DECLARE var_msg         VARCHAR(64) DEFAULT 'OK';


  DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        GET DIAGNOSTICS CONDITION 1 @p1 = RETURNED_SQLSTATE, @p2 = MESSAGE_TEXT;
        ROLLBACK;
        SELECT -1999, CONCAT('execute pModifyAgent2Ratio failure, SQLState:', @p1, ',Message:', @p2);
    END;
  
  set var_uuid = param_uuid;

 -- 获取当前代理父级代理分成比例

  -- SELECT agent_uuid INTO var_parentUuid FROM `hall_dc`.`user_agent_summary` WHERE UUID = var_uuid;

  SELECT ratio INTO var_parentRatio FROM `hall_dc`.`user_summary` WHERE UUID = param_parentUuid;

  IF param_ratio >= var_parentRatio THEN
      SET var_ratio = var_parentRatio;
  ELSE
      SET var_ratio = param_ratio;
  END IF;


  START TRANSACTION;
  LABEL_MAIN: BEGIN
        
        -- 更新分成比例
        UPDATE hall_dc.`user_summary`
        SET `ratio` = var_ratio
        WHERE `uuid`=var_uuid;
--         IF (ROW_COUNT() = 0) THEN
--           SELECT ROW_COUNT() aa;
--         BEGIN
--             ROLLBACK;
--             SET var_code=-1002, var_msg='update user_summary ratio failure.';
--             LEAVE LABEL_MAIN;
--         END;
--        END IF;
    COMMIT;
  END LABEL_MAIN;
  SELECT var_code, var_msg, var_uuid,var_ratio;
END */$$
DELIMITER ;

/* Procedure structure for procedure `pModifyPayMethod` */

/*!50003 DROP PROCEDURE IF EXISTS  `pModifyPayMethod` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pModifyPayMethod`(
        IN param_configId       int,
        in param_amounts        varchar(100),
        in param_minAmount      int,
        in param_maxAmount      int,
        in param_qrcode         int,
        in param_channelName    varchar(30)
    )
BEGIN
        declare var_code    int default 0;
        declare var_message varchar(128) default 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
        
        update `config_pay`
        set `amounts`=param_amounts, `minAmount`=param_minAmount, `maxAmount`=param_maxAmount, `scan`=param_qrcode, `channel_name`=param_channelName
        where `channel`=param_configId;
        if (row_count() <= 0) then
        begin
            set var_code=-1, var_message=concat('not found configId ', param_configId);
        end;
        end if;
        
        select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pOnEnterGame` */

/*!50003 DROP PROCEDURE IF EXISTS  `pOnEnterGame` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pOnEnterGame`(
	IN param_clientId 	VARCHAR(40)
    )
BEGIN
		SELECT `client_secret`
		FROM `config_game`
		WHERE `client_id`=param_clientId AND `show`=1 AND `open`=1;
		
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pOnUserLogin` */

/*!50003 DROP PROCEDURE IF EXISTS  `pOnUserLogin` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pOnUserLogin`(
	IN param_channel varchar(54), 
	in param_userId varchar(40),
	in param_ip varchar(18), 
	in param_isFirst int,  -- 1=>first time; 0 or other -> not first time
	in param_loginType varchar(50),
	in param_sessionId varchar(64),
	in param_phone varchar(11) )
BEGIN
	    declare var_ipaddr     varchar(128) default '';
	    declare var_code       int;
	    declare var_message    varchar(128);
	    
	    SELECT CONCAT(`country`,`province`,`city`,'-',`operator`) into var_ipaddr
	    FROM `ip_data`
            WHERE INET_ATON(param_ip)<=`end` LIMIT 1;
            
	    -- 更新 account
	    if (param_isFirst = 1) then
	    begin 
            update `account` 
            set `auto_in`=1, `auto_out`=1,`channel`=param_channel, 
                `ip`=param_ip, `address`=var_ipaddr,`is_online`=1,`first`=1
            WHERE `uuid`=param_userId;
		
	        -- 更新手机绑定时间
            update `account`
            set `bind_phone_time`=NOW()
            where `uuid`=param_userId and param_phone <> '';

            -- TODO: 送金币，查config_configure表中的 `bind_coin`，`tourist_coin`
	    end;
	    end if;
	
        -- 生成online表信息
        REPLACE into `online`(`uuid`,`third_id`,`nick`,`mobile`,`coin`,`login_time`,`login_ip`,`create_channel`,`session_id`)
        select `uuid`,`tourist`,`nick`,`phone`,`diamond`,NOW(), var_ipaddr, `channel`, param_sessionId
        from `account`
        where `uuid`=param_userId;

/*        
	    -- 更新 sessionId
	    update `account` 
	    set `session_id`=param_sessionId, `update_time`=NOW()
	    WHERE `uuid`=param_userId;
*/	    
		
	    set var_code = 0, var_message = 'OK';
	    
	    -- 生成登陆日志
	    insert into `log_login`(`uuid`,`ip`,`port`,`proxy_ip`,`proxy_port`,`protocol`,`type`,`address`,`login_time`)
	    value(param_userId, param_ip, 0, '', 0, 'tcp', param_loginType, var_ipaddr, now());
	    
	    select var_code, var_message, last_insert_id();
	    
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pOnUserLogout` */

/*!50003 DROP PROCEDURE IF EXISTS  `pOnUserLogout` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pOnUserLogout`(
	in param_userId varchar(32),
	in param_loginId int unsigned
    )
BEGIN
		-- update `account` set `is_online`=0 where `uuid`=param_userId;
		delete from `online` where `uuid`=param_userId;
		
		update `log_login` set `logout_time`=NOW() where `id`=param_loginId;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pQueryGame` */

/*!50003 DROP PROCEDURE IF EXISTS  `pQueryGame` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pQueryGame`()
BEGIN
		SELECT `client_name`,`client_id`,`game_type`,`open`,`pos`,`pname`,`show`,`tab` ,`access_mode`,`server_host`,`server_port`
		FROM `config_game` 
		WHERE `show`=1 AND `data_status`=0;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pQueryNotice` */

/*!50003 DROP PROCEDURE IF EXISTS  `pQueryNotice` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pQueryNotice`()
BEGIN
		SELECT `type`,`content`,`label`,`title` 
		FROM `config_notice` 
		WHERE `status`=1 AND `type`=1 AND `data_status`=0 
		ORDER BY `sort`;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pQueryUser` */

/*!50003 DROP PROCEDURE IF EXISTS  `pQueryUser` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pQueryUser`(in param_uuid varchar(32) )
BEGIN
		-- 用于重构版本的用户登陆信息获取
		
		SELECT `id`,`uuid`,`nick`,`sex`,`head`,`diamond`,`phone`,`first`
		FROM `account`
		WHERE `uuid`=param_uuid AND `data_status`=0;

	END */$$
DELIMITER ;

/* Procedure structure for procedure `pQueryUserInfoForPaycenter` */

/*!50003 DROP PROCEDURE IF EXISTS  `pQueryUserInfoForPaycenter` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pQueryUserInfoForPaycenter`(in param_userId varchar(40) )
BEGIN
        SELECT t1.`nick`,t1.`channel` AS `channelCode`,t1.`create_time`,t2.`id` AS `channelId`,t2.`channel_name`
        FROM `account` t1, `config_channel` t2
        WHERE t1.`uuid`=param_userId  AND t1.`channel`=t2.`channel`;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `prc_get_audit_status` */

/*!50003 DROP PROCEDURE IF EXISTS  `prc_get_audit_status` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `prc_get_audit_status`(IN `p_type` int)
BEGIN 
set @typestr='';
	IF p_type = 1 THEN
		set @typestr='银联-';
	ELSE
		set @typestr='支付宝-';
	END IF;

	SET @sql0='SELECT id,channel_name,`status` ';
	SET @sql1 = CONCAT('SELECT 4 id ,CONCAT(\'同意【\',@typestr,channel_name,\'】\') as channel_name,`status` FROM dt_hall.config_withdraw where data_status = 0 AND hall_channel = 0 and withdraw_type=',p_type,' and status=1 and channel_name != \'人工转账\'');
	SET @sql2 = '';
	
  SET @cnt = (select count(*) FROM  dt_hall.config_withdraw where data_status = 0 AND hall_channel = 0 and withdraw_type=p_type and `status`=1 and channel_name = '人工转账');
	IF @cnt >= 1 THEN
		SET @sql2 = CONCAT('SELECT convert(-id, signed) id ,CONCAT(\'同意【\',@typestr,channel_name,\'】\') as channel_name,`status` FROM dt_hall.config_withdraw where data_status = 0 AND hall_channel = 0 and withdraw_type=',p_type,' and status=1 and channel_name = \'人工转账\'  limit 1');	
	ELSE
		SET @sql2 = CONCAT('SELECT convert(-id, signed) id ,CONCAT(\'同意【\',@typestr,channel_name,\'】\') as channel_name,`status` FROM dt_hall.config_withdraw where data_status = 0 AND hall_channel = 0 and withdraw_type=',p_type,' and status=0 and channel_name = \'人工转账\'  limit 1');	
	end IF; 
	
  SET @sql3=CONCAT(' select 0 as id,\'拒绝\' as channel_name,\'0\' as `status` from DUAL ');
	SET @sqlStmt = CONCAT(@sql0,' from (',@sql1,' UNION ALL ',@sql2,' UNION ALL ',@sql3,') a' );	
	PREPARE stmt FROM @sqlStmt;
	EXECUTE stmt;
	DEALLOCATE PREPARE stmt; 
END */$$
DELIMITER ;

/* Procedure structure for procedure `pUpdateConfigPay` */

/*!50003 DROP PROCEDURE IF EXISTS  `pUpdateConfigPay` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pUpdateConfigPay`(
	in param_payType 	int,
	in param_minCoin 	int,
	in param_maxCoin 	int,
	in param_coinList varchar(100)
    )
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
        DECLARE var_cash_list   VARCHAR(128);
        DECLARE var_isTest      INT DEFAULT 0;
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message, 0;
        END;
	
LABEL_MAIN: BEGIN
	    begin  -- 启动事务
	    -- 若支付金币为空，则禁止此支付方式
	    if (param_coinList = '') then
	    begin
		update `config_pay` 
		set `status`=0,`update_time`=NOW()
		WHERE `payType`=param_payType AND `status`=1 AND `data_status`=0 AND `hall_channel`=var_isTest;
	    end;
	    else
	    begin
		update `config_pay`
		set `amounts`=param_coinList, `minAmount`=param_minCoin, `maxAmount`=param_maxCoin, `status`=1, `update_time`=NOW()
		where `payType`=param_payType and `data_status`=0 and `hall_channel`=var_isTest;
	    end;
	    end if;
	    if (ROW_COUNT()=0) then
	    begin
		rollback;
		set var_code=-1, var_message='update config_pay failure';
		leave LABEL_MAIN;
	    end;
	    end if;
	    
	    -- 获得支付列表
            SELECT GROUP_CONCAT(`payType` SEPARATOR ',') INTO var_cash_list
            FROM `config_pay` 
            WHERE `data_status`=0 AND `status`=1 AND `hall_channel`=var_isTest;
	    
            -- 更新支付列表 (TODO: 排序规则？)
            IF EXISTS (SELECT * FROM `config_cash` WHERE `data_status`=0 AND `status`=1 AND `type`=1 AND `hall_channel`=var_isTest) THEN
            BEGIN
                UPDATE `config_cash` 
                SET `list`=var_cash_list, `update_time`=NOW() 
                WHERE `data_status`=0 AND `status`=1 AND `type`=1 AND `hall_channel`=var_isTest;
            END;
            ELSE
            BEGIN
                INSERT INTO `config_cash`(`type`, `list`, `status`, `source`, `data_status`, `create_time`, `update_time`, `hall_channel`)
                VALUES(1, var_cash_list, 1, 1226, 0, NOW(), NOW(), var_isTest);
            END;
            END IF;
	    IF (ROW_COUNT() <=0) THEN
            BEGIN
                SET var_code=-3, var_message='update config_cash failure';
                ROLLBACK;
                LEAVE label_main;
            END;
            END IF;
        
            commit;
            end;
            set var_code=0, var_message='update config_pay SUCCEED';
        end LABEL_MAIN;
	
	select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pUpgradeAgent2` */

/*!50003 DROP PROCEDURE IF EXISTS  `pUpgradeAgent2` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pUpgradeAgent2`(
        IN param_parentUuid         VARCHAR(40),  -- 父级代理的UUID
        IN param_uuid         VARCHAR(40),        -- 代理的UUID
        IN param_ratio              double        -- 分成比例,*10000
    )
    COMMENT '升级代理'
BEGIN
  DECLARE var_uuid        varchar(40);
  DECLARE var_parentUuid  varchar(40);
  DECLARE var_parentRatio double default 0.1;
  DECLARE var_ratio       double default 0.05;
  DECLARE var_code        INT DEFAULT 0;
  DECLARE var_msg         VARCHAR(64) DEFAULT 'OK';


  DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        GET DIAGNOSTICS CONDITION 1 @p1 = RETURNED_SQLSTATE, @p2 = MESSAGE_TEXT;
        ROLLBACK;
        SELECT -1999, CONCAT('execute pUpgradeAgent2 failure, SQLState:', @p1, ',Message:', @p2);
    END;
  
  set var_uuid = param_uuid;

  -- 获取当前代理父级代理分成比例

  -- SELECT agent_uuid INTO var_parentUuid FROM `hall_dc`.`user_agent_summary` WHERE UUID = var_uuid;

  SELECT ratio INTO var_parentRatio FROM `hall_dc`.`user_summary` WHERE UUID = param_parentUuid;

  IF param_ratio >= var_parentRatio THEN
      SET var_ratio = var_parentRatio;
  ELSE
      SET var_ratio = param_ratio;
  END IF;


  START TRANSACTION;
  LABEL_MAIN: BEGIN
        -- 更新代理标识
        UPDATE dt_hall.`account`
        SET `agent_type` = 2
        WHERE `uuid`=var_uuid;
--         IF (ROW_COUNT() = 0) THEN
--         BEGIN
--             ROLLBACK;
--             SET var_code=-1002, var_msg='update account agent_type failure.';
--             LEAVE LABEL_MAIN;
--         END;
--         END IF;
        
        -- 更新分成比例
        UPDATE hall_dc.`user_summary`
        SET `ratio` = var_ratio
        WHERE `uuid`=var_uuid;
--         IF (ROW_COUNT() = 0) THEN
--         BEGIN
--             ROLLBACK;
--             SET var_code=-1002, var_msg='update user_summary ratio failure.';
--             LEAVE LABEL_MAIN;
--         END;
--         END IF;

      COMMIT;
  END LABEL_MAIN;
  
  -- 返回结果

  SELECT var_code, var_msg, var_uuid, var_ratio;

END */$$
DELIMITER ;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
