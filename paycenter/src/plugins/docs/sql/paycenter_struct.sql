/*
SQLyog Ultimate v13.1.1 (64 bit)
MySQL - 5.6.27 : Database - paycenter
*********************************************************************
*/

/*!40101 SET NAMES utf8mb4 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS*/`paycenter` /*!40100 DEFAULT CHARACTER SET utf8mb4 */;

USE `paycenter`;

/*Table structure for table `dict_orderstatus` */

DROP TABLE IF EXISTS `dict_orderstatus`;

CREATE TABLE `dict_orderstatus` (
  `status` int(11) NOT NULL COMMENT '订单状态',
  `note` varchar(16) DEFAULT NULL COMMENT '订单状态说明',
  `step` int(11) DEFAULT NULL COMMENT '发生的步骤序号',
  `finished` tinyint(1) DEFAULT NULL COMMENT '订单是否结束',
  `desc` varchar(128) DEFAULT '' COMMENT '订单状态说明',
  `next_status` varchar(32) DEFAULT '' COMMENT '后续步骤说明',
  `timeout` int(11) DEFAULT '0' COMMENT '超时间隔(秒)',
  `timeout_desc` varchar(128) DEFAULT '' COMMENT '超时说明',
  PRIMARY KEY (`status`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

/*Table structure for table `dict_paymethod` */

DROP TABLE IF EXISTS `dict_paymethod`;

CREATE TABLE `dict_paymethod` (
  `id` int(11) NOT NULL COMMENT 'ID',
  `method` varchar(8) NOT NULL COMMENT '支付代码',
  `name` varchar(16) NOT NULL COMMENT '支付名称',
  `desc` varchar(32) DEFAULT '' COMMENT '描述',
  PRIMARY KEY (`method`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

/*Table structure for table `game_channel_method` */

DROP TABLE IF EXISTS `game_channel_method`;

CREATE TABLE `game_channel_method` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '主键 ID',
  `gameId` int(11) NOT NULL COMMENT '游戏id',
  `channelId` int(11) NOT NULL COMMENT '渠道id',
  `method` varchar(50) NOT NULL COMMENT '支付方式',
  `configId` int(11) NOT NULL COMMENT '对应渠道配置id',
  `status` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=81 DEFAULT CHARSET=utf8mb4;

/*Table structure for table `ips` */

DROP TABLE IF EXISTS `ips`;

CREATE TABLE `ips` (
  `start` int(11) unsigned NOT NULL,
  `end` int(11) unsigned NOT NULL,
  `startip` varchar(64) NOT NULL,
  `endip` varchar(64) NOT NULL,
  `country` varchar(64) NOT NULL,
  `province` varchar(64) NOT NULL,
  `city` varchar(64) NOT NULL,
  `operator` varchar(64) NOT NULL,
  `zipcode` int(11) unsigned NOT NULL,
  `areacode` int(11) unsigned NOT NULL,
  UNIQUE KEY `index_by_ip` (`start`,`end`),
  UNIQUE KEY `index_by_end` (`end`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8mb4;

/*Table structure for table `report_sdkstat` */

DROP TABLE IF EXISTS `report_sdkstat`;

CREATE TABLE `report_sdkstat` (
  `stat_date` varchar(8) NOT NULL DEFAULT '' COMMENT '统计日期',
  `game_id` varchar(50) NOT NULL DEFAULT '' COMMENT '游戏ID',
  `channel` varchar(50) NOT NULL DEFAULT '' COMMENT '玩家渠道',
  `channelid` varchar(50) NOT NULL DEFAULT '' COMMENT '玩家渠道ID',
  `method` varchar(50) NOT NULL DEFAULT '' COMMENT '支付方式',
  `SDK` varchar(50) NOT NULL DEFAULT '' COMMENT '支付SDK',
  `req_cnt` int(11) NOT NULL DEFAULT '0' COMMENT '支付调起次数',
  `sucess_cnt` int(11) NOT NULL DEFAULT '0' COMMENT '成功次数',
  `fail_cnt` int(11) NOT NULL DEFAULT '0' COMMENT '失败次数',
  `req_money` int(11) NOT NULL DEFAULT '0' COMMENT '支付发起金额',
  `sucess_money` int(11) NOT NULL DEFAULT '0' COMMENT '支付成功金额',
  `real_money` int(11) NOT NULL DEFAULT '0' COMMENT '实际到账金额',
  `fail_money` int(11) NOT NULL DEFAULT '0' COMMENT '支付失败金额',
  `insert_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

/*Table structure for table `tchannel` */

DROP TABLE IF EXISTS `tchannel`;

CREATE TABLE `tchannel` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '序号',
  `group` varchar(16) NOT NULL DEFAULT '' COMMENT '隶属厂商',
  `code` varchar(32) DEFAULT NULL COMMENT '支付渠道码',
  `name` varchar(128) DEFAULT NULL COMMENT '支付渠道说明',
  `status` int(11) DEFAULT '0' COMMENT '状态：0-不可用，1-正常',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_channel_code` (`code`),
  UNIQUE KEY `idx_channel_group` (`group`,`code`)
) ENGINE=InnoDB AUTO_INCREMENT=20 DEFAULT CHARSET=utf8mb4;

/*Table structure for table `tcity` */

DROP TABLE IF EXISTS `tcity`;

CREATE TABLE `tcity` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '主键 ID',
  `cityCode` varchar(20) NOT NULL COMMENT '城市编码',
  `cityName` varchar(50) NOT NULL COMMENT '城市名称',
  `parentCode` varchar(20) DEFAULT NULL COMMENT '上级城市',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=432 DEFAULT CHARSET=utf8mb4;

/*Table structure for table `tgame` */

DROP TABLE IF EXISTS `tgame`;

CREATE TABLE `tgame` (
  `id` int(11) unsigned NOT NULL,
  `code` varchar(32) NOT NULL COMMENT '游戏代码',
  `name` varchar(64) DEFAULT NULL COMMENT '游戏名称',
  `notify_url` varchar(256) NOT NULL DEFAULT '' COMMENT '支付回调地址',
  `transfer_url` varchar(256) DEFAULT '' COMMENT '提现回调地址',
  `desc` varchar(128) DEFAULT '' COMMENT '描述',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_code` (`code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

/*Table structure for table `tlogpayorder` */

DROP TABLE IF EXISTS `tlogpayorder`;

CREATE TABLE `tlogpayorder` (
  `logId` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '日志id,仅作主键用',
  `payId` int(10) unsigned NOT NULL COMMENT 'tPayOrder表的ID',
  `game_req` varchar(1024) NOT NULL COMMENT '游服发过来的请求',
  `game_req_time` datetime NOT NULL COMMENT '游服请求的时间',
  `req_data` varchar(1024) DEFAULT '' COMMENT '向平台请求的数据',
  `req_time` datetime DEFAULT NULL COMMENT '请求的时间',
  `resp_header` varchar(1024) DEFAULT '' COMMENT '平台返回时的header, base64编码',
  `resp_data` varchar(1024) DEFAULT '' COMMENT '平台返回的数据所在磁盘的文件名',
  `resp_time` datetime DEFAULT NULL COMMENT '返回的时间',
  `notify_data` varchar(1024) DEFAULT '' COMMENT '平台异步通知的数据',
  `notify_time` datetime DEFAULT NULL COMMENT '异常通知的时间',
  `note` varchar(256) DEFAULT '' COMMENT '备注',
  PRIMARY KEY (`logId`)
) ENGINE=MyISAM AUTO_INCREMENT=194 DEFAULT CHARSET=utf8mb4;

/*Table structure for table `tpaychannel` */

DROP TABLE IF EXISTS `tpaychannel`;

CREATE TABLE `tpaychannel` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `sdk_code` varchar(16) NOT NULL DEFAULT '' COMMENT '对应SDK的编码',
  `method_code` varchar(16) NOT NULL DEFAULT '' COMMENT '支付方式，大的渠道',
  `channel_code` varchar(32) NOT NULL COMMENT '支付渠道，外健，与tChannel表的code关联',
  `game_code` varchar(32) NOT NULL COMMENT '游戏ID，外键，每个游戏的支付渠道得单独配置',
  `game_channel_id` varchar(64) DEFAULT '' COMMENT '游戏渠道ID，允许每个游戏不同渠道有不同的支付，暂时不支持',
  `flag` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '渠道用途，0支付，1提现',
  `name` varchar(64) NOT NULL COMMENT '支付通道名称',
  `desc` varchar(128) DEFAULT '' COMMENT '支付通道描述',
  `keydata` varchar(1024) NOT NULL COMMENT '支付通道关键数据',
  `active_time` varchar(256) NOT NULL COMMENT '可启用时间段，以 hh:mm:ss - hh:mm:ss来表示，用;分隔多个支付时间段',
  `money_range` varchar(128) NOT NULL COMMENT '支付金额范围，以 100-2999表示，仅只持一组',
  `weight` int(11) unsigned NOT NULL COMMENT '支付通道的权重，权重越大优先级越高,0表示不被调起，值为1-100',
  `interval` int(10) unsigned NOT NULL COMMENT '两次支付的调起间隔，以秒计算，外部可以折算成每分种几次之类的',
  `cgi` varchar(128) NOT NULL COMMENT 'cgi服务接口',
  `status` int(11) NOT NULL DEFAULT '1' COMMENT '状态，1表示启用，其他值表示停用状态',
  `create_time` datetime DEFAULT CURRENT_TIMESTAMP COMMENT '增加时间',
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `idx_gameid` (`game_code`),
  KEY `idx_paychannel` (`game_code`),
  KEY `fk_channel_code` (`channel_code`),
  CONSTRAINT `fk_channel_code` FOREIGN KEY (`channel_code`) REFERENCES `tchannel` (`code`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `fk_game_code` FOREIGN KEY (`game_code`) REFERENCES `tgame` (`code`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=66 DEFAULT CHARSET=utf8mb4;

/*Table structure for table `tpayorder` */

DROP TABLE IF EXISTS `tpayorder`;

CREATE TABLE `tpayorder` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '表的主键 ID',
  `uin` varchar(150) NOT NULL COMMENT '玩家 UIN',
  `gameId` int(11) NOT NULL DEFAULT '0' COMMENT '游戏编码，tGame中的code',
  `game_code` varchar(16) DEFAULT '' COMMENT '游戏的速记码',
  `sdk_code` varchar(16) NOT NULL DEFAULT '' COMMENT 'tSDK中的code',
  `method_code` varchar(16) NOT NULL DEFAULT '' COMMENT '充值的渠道，tChannel中的method',
  `channel_code` varchar(32) NOT NULL DEFAULT '' COMMENT '充值的细化渠道，tPayChannel中的channel_code',
  `orderNo` varchar(150) NOT NULL COMMENT '订单号',
  `gameOrder` varchar(50) NOT NULL COMMENT '游服订单号',
  `thirdPartyOrder` varchar(50) DEFAULT NULL COMMENT '第三方订单号',
  `totalFee` int(11) NOT NULL COMMENT '总金额',
  `productInfo` varchar(255) DEFAULT NULL COMMENT '商品信息',
  `gameNorifyUrl` varchar(255) DEFAULT NULL COMMENT '支付回调通知游服地址',
  `orderStatus` int(11) NOT NULL COMMENT '订单状态',
  `thirdPartyResponse` varchar(1024) DEFAULT NULL COMMENT '第三方返回数据',
  `createdAt` datetime(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6) COMMENT '创建时间',
  `updatedAt` datetime(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6) COMMENT '更新时间',
  `realAmount` int(11) DEFAULT NULL COMMENT '实际支付金额 单位：分',
  `productId` varchar(64) DEFAULT NULL COMMENT '商品 ID',
  `playerIP` varchar(128) DEFAULT NULL COMMENT '玩家 IP,含地址信息',
  `userName` varchar(32) DEFAULT '' COMMENT '用户名称',
  `create_channel` varchar(32) DEFAULT '' COMMENT '创建渠道号',
  `user_createtime` datetime DEFAULT NULL COMMENT '用户创建时间',
  `configId` int(11) DEFAULT NULL COMMENT '渠道配置 ID',
  `clientType` varchar(64) DEFAULT '' COMMENT '前端的平台类型',
  `remark` varchar(64) DEFAULT '' COMMENT '前端请求发过来的remark',
  `channelId` int(11) DEFAULT '0' COMMENT '渠道ID',
  `channelName` varchar(50) DEFAULT '' COMMENT '渠道中文名称',
  PRIMARY KEY (`id`),
  UNIQUE KEY `IDX_9e116d4adfd60229dc662a81b0` (`orderNo`) USING BTREE,
  UNIQUE KEY `IDX_56898578266f75e4496c88b1ae` (`gameOrder`) USING BTREE,
  KEY `FK_365d925992c98ee5384c1a13640` (`game_code`) USING BTREE,
  KEY `FK_592c78aa6eda1985b07692a4d20` (`channel_code`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=2197 DEFAULT CHARSET=utf8mb4;

/*Table structure for table `trole` */

DROP TABLE IF EXISTS `trole`;

CREATE TABLE `trole` (
  `id` int(11) NOT NULL COMMENT '表主键 ID',
  `name` varchar(10) NOT NULL COMMENT '角色名称',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

/*Table structure for table `tsdk` */

DROP TABLE IF EXISTS `tsdk`;

CREATE TABLE `tsdk` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'SDK ID',
  `code` varchar(16) NOT NULL COMMENT 'SDK代码',
  `name` varchar(64) NOT NULL COMMENT 'SDK名称',
  `url` varchar(128) NOT NULL COMMENT '支付网关',
  `account` varchar(128) NOT NULL COMMENT '商户号',
  `apikey` varchar(128) NOT NULL COMMENT '商户APIKey',
  `extra_data` varchar(256) DEFAULT '' COMMENT '扩展数据',
  `mgrurl` varchar(128) DEFAULT '' COMMENT '管理的URL',
  `note` varchar(128) DEFAULT '' COMMENT '备注',
  `create_time` datetime NOT NULL,
  `update_time` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_sdk_code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=21 DEFAULT CHARSET=utf8mb4;

/*Table structure for table `tuser` */

DROP TABLE IF EXISTS `tuser`;

CREATE TABLE `tuser` (
  `id` int(11) NOT NULL AUTO_INCREMENT COMMENT '表的主键 ID',
  `username` varchar(20) NOT NULL COMMENT '用户账户名称',
  `nickname` varchar(10) NOT NULL COMMENT '用户昵称',
  `password` varchar(100) NOT NULL COMMENT '密码',
  `roleId` int(11) DEFAULT NULL COMMENT '角色 ID',
  `createdAt` datetime(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6),
  `updatedAt` datetime(6) NOT NULL DEFAULT CURRENT_TIMESTAMP(6),
  PRIMARY KEY (`id`),
  KEY `FK_368e146b785b574f42ae9e53d5e` (`roleId`) USING BTREE,
  CONSTRAINT `tuser_ibfk_1` FOREIGN KEY (`roleId`) REFERENCES `trole` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4;

/*!50106 set global event_scheduler = 1*/;

/* Event structure for event `job_pStatPayMethodSDK` */

/*!50106 DROP EVENT IF EXISTS `job_pStatPayMethodSDK`*/;

DELIMITER $$

/*!50106 CREATE EVENT `job_pStatPayMethodSDK` ON SCHEDULE EVERY 1 MINUTE STARTS '2020-03-16 00:00:34' ON COMPLETION NOT PRESERVE ENABLE DO begin
declare v_stat_date 		varchar(8);
declare v_time					varchar(8);

set v_stat_date=date_format(date_add(now(),interval -1 day),'%Y%m%d');
set v_time=date_format(now(),'%H-%i-%s');

if v_time>='00-00-00' and v_time<='00-30-00' then
	call pStatPayMethodSDK(v_stat_date);
end if;

end */$$
DELIMITER ;

/* Procedure structure for procedure `pAddChannelDict` */

/*!50003 DROP PROCEDURE IF EXISTS  `pAddChannelDict` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pAddChannelDict`(
        IN param_method     varchar(16),
        in param_channel    varchar(32),
        in param_desc       varchar(128)
    )
BEGIN
        declare var_code        int default 0;
        declare var_message     varchar(128) default 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        begin
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            set var_code=-1000, var_message=concat("SQL Fault, State:", @p1, ", Message:", @p2);
            select var_code, var_message;
        end;
    
LABEL_MAIN: BEGIN        
        -- 判断是否存在 method+channel的数据，若存在，则退出
        IF EXISTS (SELECT * from `tchannel` where `group`=param_method and `code`=param_channel) then
        begin
            SET var_code=-3, var_message=concat('already exists ',param_method,':', param_channel);
            LEAVE LABEL_MAIN;
        end;
        end if;
        
        insert into `tchannel`(`group`,`code`,`name`,`status`)
        values(param_method, param_channel, param_desc, 1);
        if (row_count()<=0) then
        begin
            SET var_code=-5, var_message=concat('add pay channel ', param_method,':', param_channel, ' FAILURE');
            leave LABEL_MAIN;
        end;
        end if;
        SET var_code=0, var_message=CONCAT('add pay channel ', param_method,':', param_channel, ' SUCCEED');
        END LABEL_MAIN;
        
        select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pAddNewSDK` */

/*!50003 DROP PROCEDURE IF EXISTS  `pAddNewSDK` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pAddNewSDK`(
        in param_code       varchar(16),
        in param_name       varchar(64),
        in param_url        varchar(128),
        in param_account    varchar(128),
        in param_apiKey     varchar(128),
        in param_extraData  varchar(256),
        in param_mgrUrl     varchar(128)
    )
BEGIN
        declare var_code        int default 0;
        declare var_message     varchar(128) default 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
        
LABEL_MAIN: BEGIN
        if exists (select * from `tsdk` where `code`=param_code) then
        begin
            set var_code=-1, var_message=concat('already exists sdk code: ', param_code);
            leave LABEL_MAIN;
        end;
        end if;
        
        insert into `tsdk`(`code`,`name`,`url`,`account`,`apikey`,`extra_data`,`mgrurl`, `create_time`)
        values(param_code, param_name, param_url, param_account, param_apiKey, param_extraData, param_mgrUrl, NOW());
        if (row_count() <= 0) then
        begin
            set var_code=-3, var_message=CONCAT('add new sdk ', param_code, '(', param_name,') FAILURE');
            leave LABEL_MAIN;
        end;
        end if;
        SET var_code=0, var_message=CONCAT('add new sdk ', param_code, '(', param_name,') SUCCEED');
        END LABEL_MAIN;
        
        select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pAddPayChannel` */

/*!50003 DROP PROCEDURE IF EXISTS  `pAddPayChannel` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pAddPayChannel`(
        in param_gameCode       varchar(32),
        in param_sdkCode        varchar(16),
        in param_channel        varchar(32),
        in param_channelName    varchar(64),
        in param_activeTime     varchar(256),
        in param_moneyRange     varchar(128),
        in param_weight         int,
        in param_cgi            varchar(128),
        in param_keyData        varchar(1024)
    )
BEGIN
        declare var_code        int default 0;
        declare var_message     varchar(128) default 'OK';
        declare var_method      varchar(16);
        declare var_channelDesc varchar(128) default '';
        declare var_sdkName     varchar(32) default '';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
        
LABEL_MAIN: BEGIN
        IF NOT EXISTS (select * from `tsdk` where `code`=param_sdkCode) then
        begin
            set var_code=-1, var_message=concat('not found sdk ', param_sdkCode);
            leave LABEL_MAIN;
        end;
        end if;
        
        if not exists (select * from `tchannel` WHERE `code`=param_channel AND `status`=1) THEN
        BEGIN
            SET var_code=-3, var_message=concat('not found pay channel ', param_channel, ', please refer to `tchannel`');
            leave LABEL_MAIN;
        END;
        END IF;
        
        select `name` INTO var_sdkName from `tSDK` where `code`=param_sdkCode;
        
        select `group`, concat(param_gameCode, '-', var_sdkName, '-', `name`) into var_method, var_channelDesc 
        from `tchannel` where `code`=param_channel AND `status`=1;
        
        -- 不检查状态
        if exists (select * from `tpaychannel` 
            where `sdk_code`=param_sdkCode AND `method_code`=var_method AND `channel_code`=param_channel AND `game_code`=param_gameCode) then
        begin
            set var_code=-5, var_message=concat('already exists pay channel ', param_channel);
            leave LABEL_MAIN;
        end;
        end if;
        
        insert into `tpaychannel`(`sdk_code`,`method_code`,`channel_code`,`game_code`,`game_channel_id`,`flag`,
            `name`,`desc`,`keydata`,`active_time`,`money_range`,`weight`,
            `interval`,`cgi`,`status`,`create_time`)
        values(param_sdkCode, var_method, param_channel, param_gameCode, '', 0,
            param_channelName, var_channelDesc,  param_keyData, param_activeTime, param_moneyRange, param_weight, 
            60, param_cgi, 1, now());
        if (row_count() <= 0) then
        begin
            set var_code=-7, var_message=concat('add pay channel ', var_method, ':', param_channel,' FAILURE');
            leave LABEL_MAIN;
        end;
        end if;
        
        set var_code = 0, var_message=concat('add pay channel ', var_method, ':', param_channel,' SUCCEEED');
        
        END LABEL_MAIN;
        
        select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pAddPayMethod` */

/*!50003 DROP PROCEDURE IF EXISTS  `pAddPayMethod` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pAddPayMethod`(
        IN param_gameId  int,
        in param_method  varchar(50),
        in param_configId int
    )
BEGIN
        declare var_code        int default 0;
        declare var_message     varchar(128) default 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
        
LABEL_MAIN: BEGIN
        -- 未启用的方法，也不允许重复添加
        if exists (select * from `game_channel_method` where `gameId`=param_gameId and `method`=param_method) then
        begin
            set var_code = -1, var_message=CONCAT('already exists pay method ', param_method);
            leave LABEL_MAIN;
        end;
        end if;
        
        INSERT INTO `game_channel_method`(`gameId`,`channelId`,`method`,`configId`,`status`)
        VALUE(param_gameId, 0, param_method, param_configId, 1);
        if (row_count() <= 0) then
        begin
            set var_code =-3, var_message='insert new pay method failure';
            leave LABEL_MAIN;
        end;
        end if;
        
        set var_code=0, var_message=concat('add new method ', param_method, ' SUCCEED');
        end LABEL_MAIN;
        
        SELECT var_code, var_message;
        
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pCheckAuthorize` */

/*!50003 DROP PROCEDURE IF EXISTS  `pCheckAuthorize` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pCheckAuthorize`(
				IN p_uuid		VARCHAR (40),
				IN p_gameId		VARCHAR (40))
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) 	DEFAULT 'OK';
				DECLARE var_uuid				VARCHAR(40)		DEFAULT '';
        DECLARE var_secret			VARCHAR(64)		DEFAULT '';
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
				SELECT	uuid INTO var_uuid
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
				SELECT	uuid, diamond 
				INTO	var_uuid, var_diamond
				FROM dt_hall.account
				WHERE uuid = p_uuid AND data_status = 0;
				
				COMMIT;

END label_main; 

        SELECT var_code, var_message, var_uuid, var_secret, var_diamond, var_order_id;
END */$$
DELIMITER ;

/* Procedure structure for procedure `pCheckInsertUser` */

/*!50003 DROP PROCEDURE IF EXISTS  `pCheckInsertUser` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pCheckInsertUser`(IN ptype			VARCHAR (20),
				IN p_tourist	VARCHAR (64),
				IN p_nick			VARCHAR (20),
				IN p_head			VARCHAR (200),
				IN p_sex			VARCHAR (2),
				IN p_phone		VARCHAR (11),
				IN p_password VARCHAR (32),
				IN p_agentId	VARCHAR (20),
				IN p_token		VARCHAR (64))
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
	
				IF var_uuid = '' THEN
						-- 查询agentId是否存在
						IF p_agentId <> "0" THEN
								SELECT phone INTO var_agentPhone FROM dt_hall.account WHERE uuid = p_agentId AND data_status = 0;
								IF var_agentPhone = '' THEN
										SET p_agentId = '0';
								END IF;
						END IF;
		
						-- 新建账号
						INSERT INTO `dt_hall`.`account`(`uuid`, `nick`, `sex`, `head`, `tourist`, `phone`, `password`, `agent_id`, `into_game_time`, `bind_phone_time`, `create_time`, `update_time`) VALUES ('000000', p_nick, p_sex, p_head, p_tourist, p_phone, p_password, p_agentId, NOW(), NOW(), NOW(), NOW());
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
						UPDATE dt_hall.account SET uuid = var_uuid, nick = p_nick WHERE id = var_last_id;
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
				SELECT	uuid, diamond, award_diamond, donate, session_id, enter_id 
				INTO	var_uuid, var_diamond, var_award_diamond, var_donate, var_session_id, var_enter_id
				FROM dt_hall.account
				WHERE uuid = p_uuid AND data_status = 0;
				
				COMMIT;
				
END label_main; 

        SELECT var_code, var_message, var_uuid, var_secret, var_diamond, var_award_diamond, var_donate, var_session_id, var_enter_id;
END */$$
DELIMITER ;

/* Procedure structure for procedure `pDelChannelDict` */

/*!50003 DROP PROCEDURE IF EXISTS  `pDelChannelDict` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pDelChannelDict`(
        IN param_method     VARCHAR(16),
        IN param_channel    VARCHAR(32)
    )
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;

LABEL_MAIN: BEGIN        
        -- 判断是否存在 method+channel的数据，若存在，则退出
        IF NOT EXISTS (SELECT * FROM `tchannel` WHERE `group`=param_method AND `code`=param_channel) THEN
        BEGIN
            SET var_code=-1, var_message=CONCAT('not exists ',param_method,':', param_channel);
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        delete from `tchannel` WHERE `group`=param_method AND `code`=param_channel;
        if (ROW_COUNT() <=0) then
        begin
            SET var_code=-3, var_message=CONCAT('remove ',param_method,':', param_channel, ' FAILURE');
            LEAVE LABEL_MAIN;
        end;
        end if;
        
        SET var_code=0, var_message=CONCAT('remove ',param_method,':', param_channel, ' SUCCEED');
        END LABEL_MAIN;
        
        select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pDelPayChannel` */

/*!50003 DROP PROCEDURE IF EXISTS  `pDelPayChannel` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pDelPayChannel`(
        in param_sdk varchar(16),
        IN param_channel varchar(32)
    )
BEGIN
        declare var_code        int default 0;
        declare var_message     varchar(128) default 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
        
LABEL_MAIN: begin
        -- 不论是否启用，均可以删除
        IF NOT EXISTS (SELECT * from `tpaychannel` where `sdk_code`=param_sdk and `channel_code`=param_channel ) then
        begin
            set var_code=-1, var_message=concat('not found pay channel ', param_sdk, ':', param_channel );
            leave LABEL_MAIN;
        end;
        end if;
        
        delete from `tpaychannel` WHERE `sdk_code`=param_sdk AND `channel_code`=param_channel;
        if (row_count() <=0) then
        begin
            SET var_code=-3, var_message=CONCAT('delete pay channel ', param_sdk, ':', param_channel , ' failure' );
            LEAVE LABEL_MAIN;
        end;
        end if;
        
        SET var_code=0, var_message=CONCAT('delete pay channel ', param_sdk, ':', param_channel , ' SUCCEED' );
        
        END LABEL_MAIN;
        
        select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pDelPayMethod` */

/*!50003 DROP PROCEDURE IF EXISTS  `pDelPayMethod` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pDelPayMethod`(IN param_gameId int, in param_configId int )
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;

LABEL_MAIN:begin        
        if not exists (SELECT * from `game_channel_method` WHERE `gameId`=param_gameId AND `configId`=param_configId) then
        begin
            set var_code=-1, var_message=CONCAT('not exists method, configId ', param_configId); 
            leave LABEL_MAIN;
        end;
        end if;
        
        delete from `game_channel_method` where `gameId`=param_gameId and `configId`=param_configId;
        if (ROW_COUNT() <=0) THEN
        BEGIN
            SET var_code=-3, var_message=CONCAT('delete method FAILURE, configId ', param_configId); 
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        SET var_code=0, var_message=CONCAT('delete method SUCCEED, configId ', param_configId); 
        
        END LABEL_MAIN;
        select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pDelSDK` */

/*!50003 DROP PROCEDURE IF EXISTS  `pDelSDK` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pDelSDK`(in param_code varchar(16) )
BEGIN
        declare var_code    int default 0;
        declare var_message varchar(128) default 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
        
LABEL_MAIN: BEGIN
        if not exists (select * from `tsdk` where `code`=param_code) then
        begin
            set var_code=-1, var_message=concat('not exists sdk:', param_code);
            leave LABEL_MAIN;
        end;
        end if;
        
        begin  -- 启动事务
        delete from `tsdk` where `code`=param_code;
        if (row_count() <= 0) then
        begin
            rollback;
            SET var_code=-3, var_message=CONCAT('remove sdk ', param_code, ' failure');
            LEAVE LABEL_MAIN;
        end;
        end if;
        
        delete from `tpaychannel` where `sdk_code`=param_code;
        commit;
        end;    -- 事务结束
        
        set var_code=0, var_message=CONCAT('remove sdk', param_code, ' SUCCEED');
        
        END LABEL_MAIN;
        
        select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pEnableChannelDict` */

/*!50003 DROP PROCEDURE IF EXISTS  `pEnableChannelDict` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pEnableChannelDict`(
        IN param_method     VARCHAR(16),
        IN param_channel    VARCHAR(32),
        in param_enable     int   -- 1=>启用   0=>停用
    )
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
        declare var_status      int;
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;

LABEL_MAIN: BEGIN        
        -- 判断是否存在 method+channel的数据，若存在，则退出
        IF NOT EXISTS (SELECT * FROM `tchannel` WHERE `group`=param_method AND `code`=param_channel) THEN
        BEGIN
            SET var_code=-1, var_message=CONCAT('not exists ',param_method,':', param_channel);
            LEAVE LABEL_MAIN;
        END;
        END IF;

        select `status` INTO var_status from `tchannel` WHERE `group`=param_method AND `code`=param_channel;
        if (var_status = param_enable) then
        begin
            SET var_code=0, var_message=CONCAT(param_method,':', param_channel, ' status not need change');
            LEAVE LABEL_MAIN;
        end;
        end if;
        
        if (param_enable <> 1 and param_enable <> 0) then
        begin
            SET var_code=-5, var_message=CONCAT('param_enable onlye allow 0, 1');
            LEAVE LABEL_MAIN;
        end;
        end if;
        
        update `tchannel` set `status`=param_enable WHERE `group`=param_method AND `code`=param_channel;
        IF (ROW_COUNT() <=0) THEN
        BEGIN
            SET var_code=-3, var_message=CONCAT('change ',param_method,':', param_channel, ' status FAILURE');
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        SET var_code=0, var_message=CONCAT('change ',param_method,':', param_channel, ' status SUCCEED');
        END LABEL_MAIN;
        
        SELECT var_code, var_message;

	END */$$
DELIMITER ;

/* Procedure structure for procedure `pEnablePayChannel` */

/*!50003 DROP PROCEDURE IF EXISTS  `pEnablePayChannel` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pEnablePayChannel`(
        IN param_gameCode       VARCHAR(32),
        IN param_sdkCode        VARCHAR(16),
        IN param_channel        VARCHAR(32),
        in param_enable         int             -- 1 启用； 0 停用
    )
BEGIN
        declare var_code        int default 0;
        declare var_message     varchar(128) default  'OK';
        DECLARE var_method      VARCHAR(16);
        declare var_status      int;
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
        
LABEL_MAIN: BEGIN        
        if not exists (select * from `tChannel` where `code`=param_channel and `status`=1) then
        begin
            set var_code=-1, var_message=CONCAT('not found pay channel ', param_channel, ', please refer to `tchannel`');
            LEAVE LABEL_MAIN;
        end;
        end if;

        -- 查找支付方式
        SELECT `group` INTO var_method 
        FROM `tchannel` WHERE `code`=param_channel AND `status`=1;
        
        -- 查找支付状态
        select `status` INTO var_status
        from `tpaychannel` 
        where `sdk_code`=param_sdkCode and `method_code`=var_method and `channel_code`=param_channel and `game_code`=param_gameCode;

        -- 若状态相同，则不需要修改
        if (param_enable = var_status) then
        begin
            set var_code=0, var_message=CONCAT('not need to change pay channel ', param_channel, ' status');
            LEAVE LABEL_MAIN;
        end;
        end if;
        
        UPDATE `tpaychannel`
        SET `status`=param_enable, `update_time`=NOW()
        WHERE `sdk_code`=param_sdkCode AND `method_code`=var_method AND `channel_code`=param_channel AND `game_code`=param_gameCode;
        IF (ROW_COUNT() <= 0) THEN
        BEGIN
            SET var_code=-3, var_message=CONCAT('modify pay channel ', var_method, ':', param_channel,' status FAILURE');
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        SET var_code = 0, var_message=CONCAT('modify pay channel ', var_method, ':', param_channel,' status SUCCEEED');
        
        END LABEL_MAIN;
        
        SELECT var_code, var_message;
        
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pEnablePayMethod` */

/*!50003 DROP PROCEDURE IF EXISTS  `pEnablePayMethod` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pEnablePayMethod`(
        IN param_gameId INT, 
        IN param_configId INT,
        in param_enable int   -- 1=>启用 0=>停用
    )
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
        declare var_status      int;
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;

LABEL_MAIN: BEGIN
        IF NOT EXISTS (SELECT * from `game_channel_method` where `gameId`=param_gameId and `configId`=param_configId) then
        begin
            set var_code=-1, var_message=CONCAT('not exists pay method, configId ', param_configId);
            leave LABEL_MAIN;
        end;
        end if;
        
        select `status` into var_status
        FROM `game_channel_method` WHERE `gameId`=param_gameId and `channelId`=param_configId;
        
        if (param_enable = var_status) then
        begin
            SET var_code=0, var_message=CONCAT('configId ', param_configId, ' not need change status' );
            LEAVE LABEL_MAIN;
        end;
        end if;
        
        if (param_enable <0 || param_enable>1) then
        begin
            SET var_code=-5, var_message='param_enable value must be 0 and 1 ';
            LEAVE LABEL_MAIN;
        end;
        end if;
        
        update `game_channel_method`
        set `status` = param_enable
        WHERE `gameId`=param_gameId AND `configId`=param_configId;
        IF (ROW_COUNT() <= 0) then
        begin
            SET var_code=-7, var_message=CONCAT('update configId ', param_configId, ' status FAILURE' );
            LEAVE LABEL_MAIN;
        end;
        end if;
        
        SET var_code=0, var_message=CONCAT('update configId ', param_configId, ' status SUCCEED' );
        
        END LABEL_MAIN;
        select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pGetMethodAll` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetMethodAll` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetMethodAll`()
begin
	select * from dict_paymethod;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetPayChannel` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetPayChannel` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetPayChannel`(IN param_gameId  int,in p_type varchar(20),in p_record_start int,in p_record_num int)
begin
	select a.`name` paychannel_name,
				 a.channel_code,
				 b.`code` sdkcode,
				 b.`name` sdkname,
				 a.method_code,
				 case when method_code = 'WX' then '微信'
							when method_code = 'UN' then '云闪付'
							when method_code = 'JD' then '京东'
							when method_code = 'ALI' then '支付宝'
							when method_code = 'QQ' then 'QQ'
							when method_code = 'BANK' then '银联'
				 else a.method_code end method_name,
				 c.`name` channel_name,
				 a.money_range,
				 a.active_time,
				 a.weight,
				 a.`status`,
				 a.`id`,
				 a.cgi pay_addr,
				 a.keydata oper_addr
	from tpaychannel a
	inner join tsdk b
	  on a.sdk_code = b.`code`
	inner join tchannel c
	  on a.channel_code = c.code
	where a.method_code = p_type
	limit p_record_start,p_record_num;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetPayChannelCNT` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetPayChannelCNT` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetPayChannelCNT`(IN param_gameId  int,in p_type varchar(20))
begin
	select count(*) record_cnt 
	from tpaychannel a
	inner join tsdk b
	  on a.sdk_code = b.`code`
	where a.method_code = p_type;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetPayMethodAll` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetPayMethodAll` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetPayMethodAll`(in p_methodcode varchar(20))
begin
	select `code` methodid,
				 `name` methodname
	from tchannel a 
	where `status` = 1
	  and a.`group` = p_methodcode
	order by `group`;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportChannel` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportChannel` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportChannel`(in p_begin_time datetime,in p_end_time datetime,in p_gameid varchar(50),in p_record_start int,in p_record_num int)
begin
	
	select b.`name` channel_code,
				 count(*) orders_cnt,
				 sum(case when a.orderStatus in (1,5,8,9,10) then 1 else 0 end) success_cnt,
				 round(sum(case when a.orderStatus in (1,5,8,9,10) then 1 else 0 end)/count(*)*100,2) success_rate,
				 round(sum(case when a.orderStatus in (1,5,8,9,10) then a.totalFee else 0 end)/100,2) recharge_money
	from tpayorder a
	inner join tchannel b
	  on a.channel_code = b.`code` and b.`status` = 1
	where a.createdAt >= p_begin_time
	  and a.createdAt <= p_end_time
	group by channel_code
	limit p_record_start,p_record_num;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportChannelCNT` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportChannelCNT` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportChannelCNT`(in p_begin_time datetime,in p_end_time datetime,in p_gameid varchar(50))
begin
	select count(1) record_cnt from (
	select channel_code,
				 count(*) orders_cnt,
				 sum(case when a.orderStatus in (1,5,8,9,10) then 1 else 0 end) success_cnt,
				 round(sum(case when a.orderStatus in (1,5,8,9,10) then 1 else 0 end)/count(*)*100,2) success_rate,
				 round(sum(case when a.orderStatus in (1,5,8,9,10) then a.totalFee else 0 end)/100,2) recharge_money
	from tpayorder a
	where a.createdAt >= p_begin_time
	  and a.createdAt <= p_end_time
	group by channel_code) x;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportLatelyMethod` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportLatelyMethod` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportLatelyMethod`(in p_rolechannels longtext,in p_record_start int,in p_record_num int)
begin
	/*实时统计，近xx小时支付统计-支付方式:默认近24小时*/
	declare v_begin_time	timestamp;
	declare v_totalcnt		bigint default 0;
	set v_begin_time = date_sub(now(),interval 24 hour);
	
	drop temporary table if exists tmp_pmethod;
	create temporary table tmp_pmethod
	as
	select a.MethodName,
				 count(*) req_cnt,
				 count(case when a.`status` in (1,5,8,9,10) then payId end) sucess_cnt,
				 sum(case when a.`status` in (1,5,8,9,10) then a.totalFee end) totalFee
	from vpayorder a
	where a.create_time >= 	v_begin_time
	  and find_in_set(a.channelId,p_rolechannels) > 0
	group by a.MethodName;
	
	select sum(req_cnt) into v_totalcnt from tmp_pmethod;
	
	select x.MethodName,
				 x.req_cnt,
				 round(x.req_cnt/v_totalcnt*100,2) req_rate,
				 x.sucess_cnt,
				 round(x.sucess_cnt/x.req_cnt*100,2) sucess_rate,
				 totalFee
	from tmp_pmethod x
	limit p_record_start,p_record_num;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportLatelyMethodCNT` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportLatelyMethodCNT` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportLatelyMethodCNT`(in p_rolechannels longtext)
begin
	/*实时统计，近xx小时支付统计-支付方式*/
	declare v_begin_time	timestamp;
	declare v_totalcnt		bigint default 0;
	set v_begin_time = date_sub(now(),interval 24 hour);
	
	drop temporary table if exists tmp_pmethod;
	create temporary table tmp_pmethod
	as
	select a.MethodName,
				 count(*) req_cnt,
				 count(case when a.`status` in (1,5,8,9,10) then payId end) sucess_cnt,
				 sum(case when a.`status` in (1,5,8,9,10) then a.totalFee end) totalFee
	from vpayorder a
	where a.create_time >= 	v_begin_time
	  and find_in_set(a.channelId,p_rolechannels) > 0
	group by a.MethodName;
	
	select sum(req_cnt) into v_totalcnt from tmp_pmethod;
	
	select count(*) record_cnt
	from tmp_pmethod x;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportLatelyOrders` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportLatelyOrders` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportLatelyOrders`(in p_rolechannels longtext,in p_record_start int,in p_record_num int)
begin
	/*实时统计，近xx小时支付统计-支付方式:默认近24小时*/
	declare v_begin_time	timestamp;
	set v_begin_time = date_sub(now(),interval 24 hour);
	
	select a.uin,
				 a.nick,
				 a.ip,
				 a.SDKName,
				 a.MethodName,
				 a.platformOrderNo,
				 a.orderNo,
				 a.totalFee,
				 concat(a.statusNote,'(',a.`status`,')') statusNote,
				 a.realFee,
				 date_format(a.create_time,'%Y-%m-%d %H:%i:%s') create_time,
				 date_format(a.last_updatetime,'%Y-%m-%d %H:%i:%s') last_updatetime,
				 a.payId,
				 a.channelName,
				 date_format(a.user_createtime,'%Y-%m-%d %H:%i:%s') user_createtime,
				 concat(b.`name`,'(',a.channel_code,')') pay_channel,
				 a.gameOrderNo,
				 concat(a.statusNote,'(',a.`status`,')') order_status,
				 c.mgrurl
	from vpayorder a
	inner join tchannel b
	  on a.channel_code = b.`code` and b.`status` = 1
	inner join tsdk c
	  on a.sdk = c.`code`
	where a.create_time>=v_begin_time
		and find_in_set(a.channelId,p_rolechannels)>0
	order by create_time desc
	limit p_record_start,p_record_num;
	
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportLatelyOrdersCNT` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportLatelyOrdersCNT` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportLatelyOrdersCNT`(in p_rolechannels longtext)
begin
	/*实时统计，近xx小时支付统计-支付方式:默认近24小时*/
	declare v_begin_time	timestamp;
	set v_begin_time = date_sub(now(),interval 24 hour);
	
	select count(*) record_cnt
	from vpayorder a
	where create_time>=v_begin_time
		and find_in_set(a.channelId,p_rolechannels)>0;
	
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportLatelySDK` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportLatelySDK` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportLatelySDK`(in p_rolechannels longtext,in p_record_start int,in p_record_num int)
begin
	/*实时统计，近xx小时支付统计-支付方式:默认近24小时*/
	declare v_begin_time	timestamp;
	declare v_totalcnt		bigint default 0;
	set v_begin_time = date_sub(now(),interval 24 hour);
	
	drop temporary table if exists tmp_pmethod;
	create temporary table tmp_pmethod
	as
	select a.SDKName,
				 count(*) req_cnt,
				 count(case when a.`status` in (1,5,8,9,10) then payId end) sucess_cnt,
				 sum(case when a.`status` in (1,5,8,9,10) then a.totalFee end) totalFee
	from vpayorder a
	where a.create_time >= 	v_begin_time
	  and find_in_set(a.channelId,p_rolechannels) > 0
	group by a.SDKName;
	
	select sum(req_cnt) into v_totalcnt from tmp_pmethod;
	
	select x.SDKName,
				 x.req_cnt,
				 round(x.req_cnt/v_totalcnt*100,2) req_rate,
				 x.sucess_cnt,
				 round(x.sucess_cnt/x.req_cnt*100,2) sucess_rate,
				 totalFee
	from tmp_pmethod x
	limit p_record_start,p_record_num;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportLatelySDKCNT` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportLatelySDKCNT` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportLatelySDKCNT`(in p_rolechannels longtext)
begin
	/*实时统计，近xx小时支付统计-支付方式:默认近24小时*/
	declare v_begin_time	timestamp;
	declare v_totalcnt		bigint default 0;
	set v_begin_time = date_sub(now(),interval 24 hour);
	
	drop temporary table if exists tmp_pmethod;
	create temporary table tmp_pmethod
	as
	select a.SDKName,
				 count(*) req_cnt,
				 count(case when a.`status` in (1,5,8,9,10) then payId end) sucess_cnt,
				 sum(case when a.`status` in (1,5,8,9,10) then a.totalFee end) totalFee
	from vpayorder a
	where a.create_time >= 	v_begin_time
	  and find_in_set(a.channelId,p_rolechannels) > 0
	group by a.SDKName;
	
	select sum(req_cnt) into v_totalcnt from tmp_pmethod;
	
	select count(*) record_cnt
	from tmp_pmethod x;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportMethod` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportMethod` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportMethod`(in p_begin_time datetime,in p_end_time datetime,in p_gameid varchar(50),in p_record_start int,in p_record_num int)
begin
	
	select b.`name` method_code,
				 count(*) orders_cnt,
				 sum(case when a.orderStatus in (1,5,8,9,10) then 1 else 0 end) success_cnt,
				 round(sum(case when a.orderStatus in (1,5,8,9,10) then 1 else 0 end)/count(*)*100,2) success_rate,
				 round(sum(case when a.orderStatus in (1,5,8,9,10) then a.totalFee else 0 end)/100,2) recharge_money
	from tpayorder a
	inner join dict_paymethod b
	  on a.method_code = b.method
	where a.createdAt >= p_begin_time
	  and a.createdAt <= p_end_time
	group by method_code
	limit p_record_start,p_record_num;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportMethodCNT` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportMethodCNT` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportMethodCNT`(in p_begin_time datetime,in p_end_time datetime,in p_gameid varchar(50))
begin
	select count(1) record_cnt from (
	select method_code,
				 count(*) orders_cnt,
				 sum(case when a.orderStatus in (5,8) then 1 else 0 end) success_cnt,
				 round(sum(case when a.orderStatus in (5,8) then 1 else 0 end)/count(*)*100,2) success_rate,
				 round(sum(case when a.orderStatus in (5,8) then a.totalFee else 0 end)/100,2) recharge_money
	from tpayorder a
	where a.createdAt >= p_begin_time
	  and a.createdAt <= p_end_time
	group by method_code) x;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportOrders` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportOrders` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportOrders`(in p_begin_date datetime,in p_end_date datetime,in p_sdk varchar(50),in p_method varchar(50),in p_order_status varchar(50),in p_channel varchar(50),in p_uin varchar(50),in p_rolechannels longtext,in p_record_start int,in p_record_num int)
begin
	declare v_next_date		varchar(8);
	declare v_sql					blob;
	set v_next_date = date_format(date_add(p_end_date,interval 1 day),'%Y%m%d');
	
	set v_sql = concat('
	select a.payid,
				 a.uin,
				 a.nick,
				 a.ip,
				 a.SDKName,
				 a.MethodName,
				 a.channel_code,
				 a.totalFee,
				 a.statusNote,
				 a.realFee,
				 date_format(a.create_time,"%Y-%m-%d %H:%i:%s") create_time,
				 a.gameOrderNo,
				 a.platformOrderNo,
				 a.orderNo,
				 date_format(a.last_updatetime,"%Y-%m-%d %H:%i:%s") last_updatetime,
				 a.channel,
				 date_format(a.user_createtime,"%Y-%m-%d %H:%i:%s") user_createtime
	from vpayorder a
	where create_time>=''',p_begin_date,'''
	  and create_time<''',v_next_date,'''
		and find_in_set(a.channelId,''',p_rolechannels,''')>0');
		
	if p_sdk is not null then
		set v_sql = concat(v_sql,'
		  and a.sdk = ''',p_sdk,'''');
	end if;
	
	if p_method is not null then
		set v_sql = concat(v_sql,'
		  and a.method = ''',p_method,'''');
	end if;
	
	if p_order_status is not null then
		set v_sql = concat(v_sql,'
		  and a.`status` = ''',p_order_status,'''');
	end if;
	
	if p_channel is not null then
		set v_sql = concat(v_sql,'
		  and a.`channel` = ''',p_channel,'''');
	end if;
	
	if p_uin is not null then
		set v_sql = concat(v_sql,'
		  and a.`uin` = ''',p_uin,'''');
	end if;
	
	set v_sql = concat(v_sql,'
	order by create_time desc
	limit ',p_record_start,',',p_record_num);
	
	#SELECT CONCAT(V_SQL);
	SET @V_SQL = v_sql;
	PREPARE EXE_SQL FROM @V_SQL;
	EXECUTE EXE_SQL;
	DEALLOCATE PREPARE EXE_SQL;
	
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportOrdersCNT` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportOrdersCNT` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportOrdersCNT`(in p_begin_date datetime,in p_end_date datetime,in p_sdk varchar(50),in p_method varchar(50),in p_order_status varchar(50),in p_channel varchar(50),in p_uin varchar(50),in p_rolechannels longtext)
begin
	declare v_next_date		varchar(8);
	declare v_sql					blob;
	set v_next_date = date_format(date_add(p_end_date,interval 1 day),'%Y%m%d');
	
	set v_sql = concat('
	select count(*) record_cnt
	from vpayorder a
	where create_time>=''',p_begin_date,'''
	  and create_time<''',v_next_date,'''
		and find_in_set(a.channelId,''',p_rolechannels,''')>0');
		
	if p_sdk is not null then
		set v_sql = concat(v_sql,'
		  and a.sdk = ''',p_sdk,'''');
	end if;
	
	if p_method is not null then
		set v_sql = concat(v_sql,'
		  and a.method = ''',p_method,'''');
	end if;
	
	if p_order_status is not null then
		set v_sql = concat(v_sql,'
		  and a.`status` = ''',p_order_status,'''');
	end if;
	
	if p_channel is not null then
		set v_sql = concat(v_sql,'
		  and a.`channel` = ''',p_channel,'''');
	end if;
	
	if p_uin is not null then
		set v_sql = concat(v_sql,'
		  and a.`uin` = ''',p_uin,'''');
	end if;
	
	#SELECT CONCAT(V_SQL);
	SET @V_SQL = v_sql;
	PREPARE EXE_SQL FROM @V_SQL;
	EXECUTE EXE_SQL;
	DEALLOCATE PREPARE EXE_SQL;
	
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportRechargeDate` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportRechargeDate` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportRechargeDate`(in p_begin_date datetime,in p_end_date datetime,in p_rolechannels longtext)
begin
	declare v_begin_date	varchar(8);
	declare v_end_date		varchar(8);
	declare v_next_date		varchar(8);
	declare v_today				varchar(8);
	
	set v_begin_date = DATE_FORMAT(p_begin_date,'%Y%m%d');
	set v_end_date = DATE_FORMAT(p_end_date,'%Y%m%d');
	set v_next_date = DATE_FORMAT(date_add(p_end_date,interval 1 day),'%Y%m%d');
	set v_today = DATE_FORMAT(curdate(),'%Y%m%d');
	
	drop temporary table if exists tmp_rmethod;
	create temporary table tmp_rmethod
	(
		stat_date varchar(8),
		channelid varchar(50),
		req_cnt		int,
		sucess_cnt int,
		totalFee int
	);
	#如果有查询当天,则将当天数据插入临时表并将查询
	if v_end_date >= v_today then
		insert into tmp_rmethod(stat_date,channelid,req_cnt,sucess_cnt,totalFee)
		select v_today stat_date,
					 a.channelid,
					 count(*) req_cnt,
					 count(case when a.orderStatus in (1,5,8,9,10) then a.id end) sucess_cnt,
					 ifnull(sum(case when a.orderStatus in (1,5,8,9,10) then a.totalfee end),0) recharge_money
		from tpayorder a
		where a.createdAt >= v_today
		group by a.channelid;
	end if;
	#插入查询时间内的数据
	insert into tmp_rmethod(stat_date,channelid,req_cnt,sucess_cnt,totalFee)
	select a.stat_date,
				 a.channelid,
				 ifnull(sum(a.req_cnt),0) req_cnt,
				 ifnull(sum(a.sucess_cnt),0) sucess_cnt,
				 ifnull(sum(a.sucess_money),0) sucess_money
	from report_sdkstat a
	where a.stat_date >= v_begin_date
	  and a.stat_date <= v_end_date
		and a.stat_date <> v_today
	group by a.stat_date,a.channelid;
	
	drop temporary table if exists tmp_rmethodall;
	create temporary table tmp_rmethodall
	as
	select sum(req_cnt) req_cnt,
				 sum(sucess_cnt) sucess_cnt,
				 sum(totalFee) sucess_money
	from tmp_rmethod a
	where find_in_set(a.channelid,p_rolechannels)>0;
	
	select a.stat_date,
				 sum(a.req_cnt) req_cnt,
				 round(sum(a.req_cnt)/max(c.req_cnt)*100,2) req_rate,
				 sum(a.sucess_cnt) sucess_cnt,
				 round(sum(a.sucess_cnt)/sum(a.req_cnt)*100,2) sucess_rate,
				 round(sum(a.totalFee)/100,2) sucess_money,
				 round(sum(a.totalFee)/max(c.sucess_money)*100,2) sucess_money_rate,
				 round(sum(a.totalFee)/sum(a.sucess_cnt)/100,2) avg_money,
				 round((round(sum(a.totalFee)/100,2)-round(sum(a.totalFee)/sum(a.sucess_cnt)/100,2))/max(c.sucess_money)*100,2) avg_money_rate
	from tmp_rmethod a
	inner join tmp_rmethodall c
	  on 1 = 1
	where find_in_set(a.channelid,p_rolechannels)>0
	group by a.stat_date
	order by stat_date desc;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportRechargeMethod` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportRechargeMethod` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportRechargeMethod`(in p_begin_date datetime,in p_end_date datetime,in p_rolechannels longtext)
begin
	declare v_begin_date	varchar(8);
	declare v_end_date		varchar(8);
	declare v_next_date		varchar(8);
	declare v_today				varchar(8);
	
	set v_begin_date = DATE_FORMAT(p_begin_date,'%Y%m%d');
	set v_end_date = DATE_FORMAT(p_end_date,'%Y%m%d');
	set v_next_date = DATE_FORMAT(date_add(p_end_date,interval 1 day),'%Y%m%d');
	set v_today = DATE_FORMAT(curdate(),'%Y%m%d');
	
	drop temporary table if exists tmp_rmethod;
	create temporary table tmp_rmethod
	(
		stat_date varchar(8),
		channelid varchar(50),
		method		varchar(50),
		req_cnt		int,
		sucess_cnt int,
		totalFee int
	);
	#如果有查询当天,则将当天数据插入临时表并将查询
	if v_end_date >= v_today then
		insert into tmp_rmethod(stat_date,channelid,method,req_cnt,sucess_cnt,totalFee)
		select v_today stat_date,
					 a.channelid,
					 a.method_code method,
					 count(*) req_cnt,
					 count(case when a.orderStatus in (1,5,8,9,10) then a.id end) sucess_cnt,
					 ifnull(sum(case when a.orderStatus in (1,5,8,9,10) then a.totalfee end),0) recharge_money
		from tpayorder a
		where a.createdAt >= v_today
		group by a.channelid,a.method_code;
	end if;
	#插入查询时间内的数据
	insert into tmp_rmethod(stat_date,channelid,method,req_cnt,sucess_cnt,totalFee)
	select a.stat_date,
				 a.channelid,
				 a.method,
				 ifnull(sum(a.req_cnt),0) req_cnt,
				 ifnull(sum(a.sucess_cnt),0) sucess_cnt,
				 ifnull(sum(a.sucess_money),0) sucess_money
	from report_sdkstat a
	where a.stat_date >= v_begin_date
	  and a.stat_date <= v_end_date
		and a.stat_date <> v_today
	group by a.stat_date,a.channelid,a.method;
	
	drop temporary table if exists tmp_rmethodall;
	create temporary table tmp_rmethodall
	as
	select stat_date,
				 sum(req_cnt) req_cnt,
				 sum(sucess_cnt) sucess_cnt,
				 sum(totalFee) sucess_money
	from tmp_rmethod a
	where find_in_set(a.channelid,p_rolechannels)>0
	group by a.stat_date;
	
	select a.stat_date,
				 b.`name` methodname,
				 sum(a.req_cnt) req_cnt,
				 round(sum(a.req_cnt)/max(c.req_cnt)*100,2) req_rate,
				 sum(a.sucess_cnt) sucess_cnt,
				 round(sum(a.sucess_cnt)/sum(a.req_cnt)*100,2) sucess_rate,
				 round(sum(a.totalFee)/100,2) sucess_money,
				 round(sum(a.totalFee)/max(c.sucess_money)*100,2) sucess_money_rate,
				 round(sum(a.totalFee)/max(a.sucess_cnt)/100,2) avg_money,
				 round((sum(a.totalFee)/sum(a.sucess_cnt))/(max(c.sucess_money)/max(c.sucess_cnt))*100 - 100,2) avg_money_rate
	from tmp_rmethod a
	inner join tmp_rmethodall c
	  on a.stat_date = c.stat_date
	inner join dict_paymethod b
	  on a.method=b.method
	where find_in_set(a.channelid,p_rolechannels)>0
	group by a.stat_date,b.`name`
	order by stat_date desc;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportRechargeSDK` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportRechargeSDK` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportRechargeSDK`(in p_begin_date datetime,in p_end_date datetime,in p_rolechannels longtext)
begin
	declare v_begin_date	varchar(8);
	declare v_end_date		varchar(8);
	declare v_next_date		varchar(8);
	declare v_today				varchar(8);
	
	set v_begin_date = DATE_FORMAT(p_begin_date,'%Y%m%d');
	set v_end_date = DATE_FORMAT(p_end_date,'%Y%m%d');
	set v_next_date = DATE_FORMAT(date_add(p_end_date,interval 1 day),'%Y%m%d');
	set v_today = DATE_FORMAT(curdate(),'%Y%m%d');
	
	drop temporary table if exists tmp_rmethod;
	create temporary table tmp_rmethod
	(
		stat_date varchar(8),
		channelid varchar(50),
		SDK		varchar(50),
		req_cnt		int,
		sucess_cnt int,
		totalFee int
	);
	#如果有查询当天,则将当天数据插入临时表并将查询
	if v_end_date >= v_today then
		insert into tmp_rmethod(stat_date,channelid,SDK,req_cnt,sucess_cnt,totalFee)
		select v_today stat_date,
					 a.channelid,
					 a.sdk_code SDK,
					 count(*) req_cnt,
					 count(case when a.orderStatus in (1,5,8,9,10) then a.id end) sucess_cnt,
					 ifnull(sum(case when a.orderStatus in (1,5,8,9,10) then a.totalfee end),0) recharge_money
		from tpayorder a
		where a.createdAt >= v_today
		group by a.channelid,a.sdk_code;
	end if;
	#插入查询时间内的数据
	insert into tmp_rmethod(stat_date,channelid,SDK,req_cnt,sucess_cnt,totalFee)
	select a.stat_date,
				 a.channelid,
				 a.SDK,
				 ifnull(sum(a.req_cnt),0) req_cnt,
				 ifnull(sum(a.sucess_cnt),0) sucess_cnt,
				 ifnull(sum(a.sucess_money),0) sucess_money
	from report_sdkstat a
	where a.stat_date >= v_begin_date
	  and a.stat_date <= v_end_date
		and a.stat_date <> v_today
	group by a.stat_date,a.channelid,a.SDK;
	
	drop temporary table if exists tmp_rmethodall;
	create temporary table tmp_rmethodall
	as
	select stat_date,
				 sum(req_cnt) req_cnt,
				 sum(sucess_cnt) sucess_cnt,
				 sum(totalFee) sucess_money
	from tmp_rmethod a
	where find_in_set(a.channelid,p_rolechannels)>0
	group by a.stat_date;
	
	select a.stat_date,
				 b.`name` SDKNAME,
				 sum(a.req_cnt) req_cnt,
				 round(sum(a.req_cnt)/max(c.req_cnt)*100,2) req_rate,
				 sum(a.sucess_cnt) sucess_cnt,
				 round(sum(a.sucess_cnt)/sum(a.req_cnt)*100,2) sucess_rate,
				 round(sum(a.totalFee)/100,2) sucess_money,
				 round(sum(a.totalFee)/max(c.sucess_money)*100,2) sucess_money_rate,
				 round(sum(a.totalFee)/sum(a.sucess_cnt)/100,2) avg_money,
				 round((sum(a.totalFee)/sum(a.sucess_cnt))/(max(c.sucess_money)/max(c.sucess_cnt))*100 - 100,2) avg_money_rate
	from tmp_rmethod a
	inner join tmp_rmethodall c
	  on a.stat_date = c.stat_date
	inner join tsdk b
	  on a.SDK=b.`code`
	where find_in_set(a.channelid,p_rolechannels)>0
	group by a.stat_date,b.`name`
	order by stat_date desc;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportSDK` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportSDK` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportSDK`(in p_begin_time datetime,in p_end_time datetime,in p_gameid varchar(50),in p_record_start int,in p_record_num int)
begin
	
	select b.`name` sdk_code,
				 count(*) orders_cnt,
				 sum(case when a.orderStatus in (1,5,8,9,10) then 1 else 0 end) success_cnt,
				 round(sum(case when a.orderStatus in (1,5,8,9,10) then 1 else 0 end)/count(*)*100,2) success_rate,
				 round(sum(case when a.orderStatus in (1,5,8,9,10) then a.totalFee else 0 end)/100,2) recharge_money
	from tpayorder a
	inner join tsdk b
	  on a.sdk_code = b.`code`
	where a.createdAt >= p_begin_time
	  and a.createdAt <= p_end_time
	group by sdk_code
	limit p_record_start,p_record_num;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetReportSDKCNT` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetReportSDKCNT` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetReportSDKCNT`(in p_begin_time datetime,in p_end_time datetime,in p_gameid varchar(50))
begin
	select count(1) record_cnt from (
	select sdk_code,
				 count(*) orders_cnt,
				 sum(case when a.orderStatus in (1,5,8,9,10) then 1 else 0 end) success_cnt,
				 round(sum(case when a.orderStatus in (1,5,8,9,10) then 1 else 0 end)/count(*)*100,2) success_rate,
				 round(sum(case when a.orderStatus in (1,5,8,9,10) then a.totalFee else 0 end)/100,2) recharge_money
	from tpayorder a
	where a.createdAt >= p_begin_time
	  and a.createdAt <= p_end_time
	group by sdk_code) x;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetSDK` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetSDK` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetSDK`(in p_record_start int,in p_record_num int)
begin
	select `name` sdkname,
				 `code` sdkcode,
				 `account` businessid,
				 `apikey` businesskey,
				 `url` payurl,
				 `extra_data` extra_data,
				 `mgrurl` mgrurl,
				 `note` note,
				 `id`
	from tsdk
	limit p_record_start,p_record_num;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetSDKAll` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetSDKAll` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetSDKAll`()
begin
	select `name` sdkname,
				 `code` sdkcode,
				 `account` businessid,
				 `apikey` businesskey,
				 `url` payurl,
				 `extra_data` extra_data,
				 `mgrurl` mgrurl,
				 `note` note,
				 `id`
	from tsdk;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pGetSDKCNT` */

/*!50003 DROP PROCEDURE IF EXISTS  `pGetSDKCNT` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pGetSDKCNT`()
begin
	select count(*) record_cnt
	from tsdk;
end */$$
DELIMITER ;

/* Procedure structure for procedure `pModifyChannelDict` */

/*!50003 DROP PROCEDURE IF EXISTS  `pModifyChannelDict` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pModifyChannelDict`(
        IN param_method     VARCHAR(16),
        IN param_channel    VARCHAR(32),
        IN param_desc       VARCHAR(128)
    )
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
    
LABEL_MAIN: BEGIN        
        -- 判断是否存在 method+channel的数据，若存在，则退出
        IF NOT EXISTS (SELECT * FROM `tchannel` WHERE `group`=param_method AND `code`=param_channel) THEN
        BEGIN
            SET var_code=-3, var_message=CONCAT('not exists ',param_method,':', param_channel);
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        UPDATE `tchannel` set `name`=param_desc
        where `group`=param_method and `code`=param_channel;
        IF (ROW_COUNT()<=0) THEN
        BEGIN
            SET var_code=-5, var_message=CONCAT('change ', param_method,':', param_channel, ' desc name FAILURE');
            LEAVE LABEL_MAIN;
        END;
        END IF;
        SET var_code=0, var_message=CONCAT('change ', param_method,':', param_channel, ' desc name SUCCEED');
        END LABEL_MAIN;
        
        SELECT var_code, var_message;

	END */$$
DELIMITER ;

/* Procedure structure for procedure `pModifyPayChannel` */

/*!50003 DROP PROCEDURE IF EXISTS  `pModifyPayChannel` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pModifyPayChannel`(
        IN param_gameCode       VARCHAR(32),
        IN param_sdkCode        VARCHAR(16),
        IN param_channel        VARCHAR(32),
        IN param_channelName    VARCHAR(64),
        IN param_activeTime     VARCHAR(256),
        IN param_moneyRange     VARCHAR(128),
        IN param_weight         INT,
        IN param_cgi            VARCHAR(128),
        IN param_keyData        VARCHAR(1024)
    )
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
        DECLARE var_method      VARCHAR(16);
        DECLARE var_channelDesc VARCHAR(128) DEFAULT '';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
        
LABEL_MAIN: BEGIN
        IF NOT EXISTS (SELECT * FROM `tsdk` WHERE `code`=param_sdkCode) THEN
        BEGIN
            SET var_code=-1, var_message=CONCAT('not found sdk ', param_sdkCode);
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        IF NOT EXISTS (SELECT * FROM `tchannel` WHERE `code`=param_channel AND `status`=1) THEN
        BEGIN
            SET var_code=-3, var_message=CONCAT('not found pay channel ', param_channel, ', please refer to `tchannel`');
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        SELECT `group`, CONCAT(param_gameCode, '-',`name`) INTO var_method, var_channelDesc 
        FROM `tchannel` WHERE `code`=param_channel AND `status`=1;
        
        -- 允许对未启用的渠道进行修改
        IF NOT EXISTS (SELECT * FROM `tpaychannel` 
            WHERE `sdk_code`=param_sdkCode AND `method_code`=var_method AND `channel_code`=param_channel AND `game_code`=param_gameCode) THEN
        BEGIN
            SET var_code=-5, var_message=CONCAT('not found pay channel ', param_sdkCode, ':', param_channel);
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        -- 更新信息，不论是否已启用
        update `tpaychannel`
        set `name`=param_channelName, `keydata`=param_keyData, `active_time`=param_activeTime,
            `money_range`=param_moneyRange, `weight`=param_weight, `cgi`=param_cgi, `update_time`=NOW()
        where `sdk_code`=param_sdkCode and `method_code`=var_method and `channel_code`=param_channel and `game_code`=param_gameCode;
        IF (ROW_COUNT() <= 0) THEN
        BEGIN
            SET var_code=-7, var_message=CONCAT('modify pay channel ', var_method, ':', param_channel,' failure');
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        SET var_code = 0, var_message=CONCAT('modify pay channel ', var_method, ':', param_channel,' SUCCEEED');
        
        END LABEL_MAIN;
        
        SELECT var_code, var_message;

	END */$$
DELIMITER ;

/* Procedure structure for procedure `pModifySDK` */

/*!50003 DROP PROCEDURE IF EXISTS  `pModifySDK` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pModifySDK`(
        IN param_code       VARCHAR(16),
        IN param_name       VARCHAR(64),
        IN param_url        VARCHAR(128),
        IN param_account    VARCHAR(128),
        IN param_apiKey     VARCHAR(128),
        IN param_extraData  VARCHAR(256),
        IN param_mgrUrl     VARCHAR(128)
    )
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
        
LABEL_MAIN: BEGIN
        IF not EXISTS (SELECT * FROM `tsdk` WHERE `code`=param_code) THEN
        BEGIN
            SET var_code=-1, var_message=CONCAT('not found sdk code: ', param_code);
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        update `tsdk` 
        set `name`=param_name, `url`=param_url, `account`=param_account, `apikey`=param_apiKey,
            `extra_data`=param_extraData, `mgrurl`=param_mgrUrl, `update_time`=NOW()
        where `code`=param_code;
        IF (ROW_COUNT() <= 0) THEN
        BEGIN
            SET var_code=-3, var_message=concat('update sdk ', param_code, '(', param_name, ') FAILURE');
            LEAVE LABEL_MAIN;
        END;
        END IF;
        
        SET var_code=0, var_message=CONCAT('update sdk ', param_code, '(', param_name, ') SUCCEED' );
        END LABEL_MAIN;
        
        SELECT var_code, var_message;

	END */$$
DELIMITER ;

/* Procedure structure for procedure `pNewOrder` */

/*!50003 DROP PROCEDURE IF EXISTS  `pNewOrder` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pNewOrder`(
        IN param_uin        varchar(64),    -- 1 UIN
        IN param_gameId     INT,            -- 2 HTTP 传过来的gameId           
        IN param_payChannel int,            -- 3 tPayChannel中对应的ID
        IN param_gameOrder  VARCHAR(50),    -- 4 游服发过来的订单号
        IN param_totalFee   INT,            -- 5 订单金额（以分计算）
        IN param_notifyUrl  VARCHAR(255),   -- 6 通知的URL
        IN param_productId  varchar(64),    -- 7 产品ID
        IN param_userIp     varchar(18),    -- 8 用户IP
        IN param_configId   INT,            -- 9 http 发送过来的configId
        IN param_clientType VARCHAR(64),    -- 10 前端类型
        in param_remark     varchar(128),   -- 11 remark
        IN param_req        varchar(1024)   -- 12 请求的URL
        )
BEGIN
        DECLARE   var_orderNo       varchar(32) default '';
        DECLARE   var_productInfo   varchar(256) default '';
        declare   var_gamecode      varchar(16) default '';
        declare   var_ipdata        varchar(128) default '';
        DECLARE   var_payId         int unsigned default 0;
        
        -- 生成平台订单
        SET var_orderNo = CONCAT('P', UNIX_TIMESTAMP(), 'R', SUBSTRING(MD5(RAND()),1,8));
        -- 生成产品信息
        SET var_productInfo = CONCAT('pay', '/', param_clientType);
        -- 查询出游戏编码
        select `code` into var_gamecode FROM tGame where `id`=param_gameId;
        -- 查询出IP数据
        select CONCAT(param_userIp,'/',`country`,`province`,`city`,'(',`operator` ,')') INTO var_ipdata
        FROM ips WHERE `end`>=INET_ATON(param_userIp) ORDER BY END LIMIT 1;
        
        -- 插入到数据库中
        INSERT INTO `tpayorder`(`uin`,`gameId`, `game_code`,`sdk_code`,`method_code`,`channel_code`,
            `orderNo`, `gameOrder`, `thirdPartyOrder`, `totalFee`, `productInfo`,
            `gameNorifyUrl`,`orderStatus`,`createdAt`,`updatedAt`,
            `realAmount`,`productId`,`playerIP`,
            `configId`, `clientType`,`remark`)
        SELECT param_uin, param_gameId, var_gamecode, `sdk_code`,`method_code`,`channel_code`,
            var_orderNo, param_gameOrder,'',param_totalFee, var_productInfo,
            param_notifyUrl, 0, now(), now(),
            0, param_productId, var_ipdata, 
            param_configId, param_clientType, param_remark
        FROM tPayChannel
        WHERE `id`=param_payChannel;
        
        -- 返回序号ID
        SELECT LAST_INSERT_ID() into var_payId;
        -- 生成日志
        INSERT INTO `tlogpayorder`(`payId`,`game_req`,`game_req_time`)
        VALUES(var_payId, param_req, NOW());

        -- 返回订单信息
        SELECT var_payId as 'ID', var_orderNo as 'ORDERNO';
        
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pOrderResult` */

/*!50003 DROP PROCEDURE IF EXISTS  `pOrderResult` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pOrderResult`(
        IN param_id                 int,            
        in param_platform_orderId   varchar(50),    
        IN param_respone            varchar(1024),  
        IN param_realFee            int             
        )
BEGIN
        update `orders` set `thirdPartyOrder`= param_platform_orderId, 
            `orderStatus`=2,
            `thirdPartyResponse`=param_respone, 
            `updatedAt`=Now(),
            `realAmount`=param_readlFee 
        where `id`=param_id;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pQueryGameChannel` */

/*!50003 DROP PROCEDURE IF EXISTS  `pQueryGameChannel` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pQueryGameChannel`(
        IN param_gameId     int
    )
BEGIN
        SELECT `configId`,`method` FROM game_channel_method WHERE gameId=param_gameId AND `status`=1;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pQueryIp` */

/*!50003 DROP PROCEDURE IF EXISTS  `pQueryIp` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pQueryIp`(in param_ip varchar(16) )
BEGIN
        declare var_ip int unsigned;
        
        set var_ip = inet_aton(param_ip);
        
        SELECT * FROM ips WHERE END>=var_ip ORDER BY END LIMIT 1;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pQueryMethodMoneyRange` */

/*!50003 DROP PROCEDURE IF EXISTS  `pQueryMethodMoneyRange` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pQueryMethodMoneyRange`(
	in param_channel	varchar(32)
    )
BEGIN
	    DECLARE var_method  	varchar(32);
	    declare var_methodId 	int;
	    declare var_moneyRange 	varchar(100);
	    
	    SELECT `group` into var_method FROM `tchannel` WHERE `code`=param_channel;
	    select `id` into var_methodId from `dict_paymethod` where `method`=var_method;
	    if exists (select * from `tPayChannel` WHERE `status`=1 AND `method_code`=var_method) then
	    begin
		SELECT `money_range`,var_methodId FROM `tPayChannel` WHERE `status`=1 AND `method_code`=var_method;
	    end;
	    else
	    begin
		select '', var_methodId;
	    end;
	    end if;
	    
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pQueryPayChannel` */

/*!50003 DROP PROCEDURE IF EXISTS  `pQueryPayChannel` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pQueryPayChannel`(
        in param_gameCode   varchar(32)
    )
BEGIN
        SELECT `id`,`method_code`,`sdk_code`,`channel_code`,`cgi`,`active_time`,`money_range`,`weight`,`interval` 
        FROM `tpaychannel` 
        WHERE `status`=1 AND `flag`=0 AND `game_code`=param_gameCode;
        -- flag     : 0=>支付, 1=>提现
        -- status   : 0=>停用, 1=>启用
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pQueryPayChannelKeyData` */

/*!50003 DROP PROCEDURE IF EXISTS  `pQueryPayChannelKeyData` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pQueryPayChannelKeyData`(
        IN param_gameCode   varchar(32),    -- 游戏 代码
        IN param_sdkCode    varchar(32)     -- SDK 代码
    )
BEGIN
        SELECT t1.`id` as `payId`, t2.`account`, t2.`apikey`, t2.`url`, t1.`keydata`, t2.`extra_data`
        FROM tPayChannel t1, tSDK t2 
        WHERE t1.game_code=param_gameCode AND t1.status=1 AND t1.flag=0 AND t1.`sdk_code`=param_sdkCode AND t1.sdk_code=t2.code;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pQueryPayChannelKeyDataByID` */

/*!50003 DROP PROCEDURE IF EXISTS  `pQueryPayChannelKeyDataByID` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pQueryPayChannelKeyDataByID`(in param_gamecode VARCHAR(16), IN param_payId int )
BEGIN
        SELECT t1.id, t2.account, t2.apikey, t2.url, t1.keydata, t2.extra_data
        FROM tPayChannel t1, tSDK t2
        WHERE t1.id =param_payId and  t1.game_code=param_gamecode AND t1.status=1 AND t1.flag=0 AND t1.sdk_code=t2.code;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pStatPayMethodSDK` */

/*!50003 DROP PROCEDURE IF EXISTS  `pStatPayMethodSDK` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pStatPayMethodSDK`(in p_stat_date varchar(8))
begin
	declare v_next_date		varchar(8);
	set v_next_date = date_format(date_add(p_stat_date,interval 1 day),'%Y%m%d');
	
	drop temporary table if exists tmp_order;
	create temporary table tmp_order
	as
	select a.channelName channel,
				 a.channelid,
				 a.gameId game_id,
				 a.sdk_code,
				 a.method_code,
				 a.totalFee,
				 a.realAmount,
				 a.orderstatus
	from tpayorder a
	where createdAt>=p_stat_date
	  and createdAt<v_next_date;
		
	delete from report_sdkstat where stat_date = p_stat_date;
		
	insert into report_sdkstat 
		(stat_date,channel,channelid,game_id,method,SDK,req_cnt,sucess_cnt,fail_cnt,req_money,sucess_money,real_money,fail_money)
	select p_stat_date stat_date,
				 a.channel,
				 a.channelid,
				 a.game_id,
				 a.method_code method,
				 a.sdk_code SDK,
				 count(*) req_cnt,
				 ifnull(sum(case when orderstatus in (1,5,8,9,10) then 1 else 0 end),0) sucess_cnt,
				 ifnull(sum(case when orderstatus in (2,3,4,7) then 1 else 0 end),0) fail_cnt,
				 ifnull(sum(a.totalFee),0) req_money,
				 ifnull(sum(case when orderstatus in (1,5,8,9,10) then a.totalFee else 0 end),0) sucess_money,
				 ifnull(sum(case when orderstatus in (1,5,8,9,10) then a.realAmount else 0 end),0) real_money,
				 ifnull(sum(case when orderstatus in (2,3,4,7) then a.totalFee else 0 end),0) fail_money
	from tmp_order a
	group by a.method_code,a.sdk_code,a.channel,a.game_id;
	
end */$$
DELIMITER ;

/* Procedure structure for procedure `pUpdateOrderNotify` */

/*!50003 DROP PROCEDURE IF EXISTS  `pUpdateOrderNotify` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pUpdateOrderNotify`(
        IN param_orderId        int unsigned,   -- 订单ID
        in param_status         int,            -- 订单状态
        IN param_platformOrder  varchar(256),   -- 平台订单号
        in param_realFee        int,            -- 实际支付金额
        in param_notify         varchar(1024),  -- 订单回调数据
        in param_notify_time    datetime        -- 回调时间
    )
BEGIN
        DECLARE var_code    INT DEFAULT 0;
        DECLARE var_message VARCHAR(128) DEFAULT 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;

LABEL_MAIN: BEGIN
        BEGIN
            UPDATE tPayOrder 
            SET `thirdPartyOrder`=param_platformOrder,`orderStatus`=param_status,
                `realAmount`=param_realFee, `updatedAt`=NOW() 
            WHERE `id`=param_orderId;
            if (ROW_COUNT() = 0) then
            begin
                rollback;
                set var_code=-1, var_message='update pay order status failure.';
                leave LABEL_MAIN;
            end;
            end if;
            
            update tLOgPayOrder 
            set `notify_data`=param_notify, `notify_time`=param_notify_time
            where `payId`=param_orderId;
            IF (ROW_COUNT() = 0) THEN
            BEGIN
                ROLLBACK;
                SET var_code=-3, var_message='update pay order log failure.';
                LEAVE LABEL_MAIN;
            END;
            END IF;
            
            COMMIT;
        end;
        
        SET var_code=0, var_message=concat('update pay order ', param_orderId, ' status SUCCEED');
        end LABEL_MAIN;
        
        select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pUpdateOrderStatus` */

/*!50003 DROP PROCEDURE IF EXISTS  `pUpdateOrderStatus` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pUpdateOrderStatus`(
        IN param_orderId        int unsigned,       -- 订单ID
        in param_status         int,                -- 订单状态
        in param_req_data       varchar(1024),      -- 请求数据
        IN param_req_time       datetime,           -- 请求时间
        in param_resp_header    varchar(1024),      -- 返回头
        in param_resp_data      varchar(1024),       -- 返回数据保存的文件的路径
        IN param_resp_time      DATETIME,            -- 返回时间(主要是为了与 param_req_time 同步)
        IN param_platformOrderNo varchar(50)         -- 平台方的订单ID，有可能为空
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

LABEL_MAIN: begin
        begin
            UPDATE tPayOrder 
            SET `orderStatus`=param_status, `updatedAt`=Now(), `thirdPartyOrder`=param_platformOrderNo 
            where `id`=param_orderId;
            IF (ROW_COUNT() = 0) then
            begin
                set var_code=-1, var_message='update payorder status failure.';
                rollback;
                leave LABEL_MAIN;
            end;
            end if;
        
            UPDATE tLogPayOrder set `req_data`=param_req_data, `req_time`=param_req_time,
                `resp_header`=param_resp_header, `resp_data`=param_resp_data, `resp_time`=param_resp_time
            where `payId` = param_orderId;
            if (ROW_COUNT() = 0) THEN
            BEGIN
                SET var_code=-3, var_message='update payorder log failure.';
                rollback;
                leave LABEL_MAIN;
            END;
            END IF;
            COMMIT;
        END;
        SET var_code=0, var_message=concat('update payorder ', param_orderId,' SUCCEED');
        end LABEL_MAIN;
        
        select var_code, var_message;
	END */$$
DELIMITER ;

/* Procedure structure for procedure `pUpdatePayOrderInfo` */

/*!50003 DROP PROCEDURE IF EXISTS  `pUpdatePayOrderInfo` */;

DELIMITER $$

/*!50003 CREATE PROCEDURE `pUpdatePayOrderInfo`(
        IN param_payId  int unsigned,
        IN param_nickName varchar(32),
        in param_createTime datetime,
        in param_channelId int unsigned,
        in param_channel varchar(32),
        in param_channelName varchar(50)
    )
BEGIN
        DECLARE var_code        INT DEFAULT 0;
        DECLARE var_message     VARCHAR(128) DEFAULT 'OK';
        DECLARE EXIT HANDLER FOR SQLEXCEPTION
        BEGIN
            GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
            SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
            SELECT var_code, var_message;
        END;
	
        update `tpayorder`
        set `userName`=param_nickName, `user_createtime`=param_createTime,
            `channelId`=param_channelId, `create_channel`=param_channel, `channelName`=param_channelName
        where `id`=param_payId;
        if (row_count() = 0) then
        begin
            set var_code=-1, var_message=concat('update tPayOrder user info SUCCEED, payId ', param_payId);
        end;
        end if;
        
        select var_code, var_message;
        
	END */$$
DELIMITER ;

/*Table structure for table `vlogpayorder` */

DROP TABLE IF EXISTS `vlogpayorder`;

/*!50001 DROP VIEW IF EXISTS `vlogpayorder` */;
/*!50001 DROP TABLE IF EXISTS `vlogpayorder` */;

/*!50001 CREATE TABLE  `vlogpayorder`(
 `payId` int(10) unsigned ,
 `game_req` blob ,
 `game_req_time` datetime ,
 `req` blob ,
 `req_time` datetime ,
 `resp_header` blob ,
 `resp` blob ,
 `resp_time` datetime ,
 `notify` blob ,
 `notify_time` datetime 
)*/;

/*Table structure for table `vpaychannel` */

DROP TABLE IF EXISTS `vpaychannel`;

/*!50001 DROP VIEW IF EXISTS `vpaychannel` */;
/*!50001 DROP TABLE IF EXISTS `vpaychannel` */;

/*!50001 CREATE TABLE  `vpaychannel`(
 `id` int(11) unsigned ,
 `sdk_code` varchar(16) ,
 `game_code` varchar(32) ,
 `desc` varchar(128) ,
 `keydata` varchar(1024) ,
 `active_time` varchar(256) ,
 `money_range` varchar(128) ,
 `weight` int(11) unsigned ,
 `interval` int(10) unsigned 
)*/;

/*Table structure for table `vpayorder` */

DROP TABLE IF EXISTS `vpayorder`;

/*!50001 DROP VIEW IF EXISTS `vpayorder` */;
/*!50001 DROP TABLE IF EXISTS `vpayorder` */;

/*!50001 CREATE TABLE  `vpayorder`(
 `payId` int(11) ,
 `uin` varchar(150) ,
 `nick` varchar(32) ,
 `channel` varchar(32) ,
 `user_createtime` datetime ,
 `IP` varchar(128) ,
 `sdk` varchar(16) ,
 `SDKName` varchar(64) ,
 `method` varchar(16) ,
 `MethodName` varchar(16) ,
 `channel_code` varchar(32) ,
 `gameOrderNo` varchar(50) ,
 `orderNo` varchar(150) ,
 `platformOrderNo` varchar(50) ,
 `totalFee` decimal(14,2) ,
 `status` int(11) ,
 `statusNote` varchar(16) ,
 `create_time` datetime(6) ,
 `last_updatetime` datetime(6) ,
 `realFee` decimal(14,2) ,
 `channelId` int(11) ,
 `channelName` varchar(50) 
)*/;

/*View structure for view vlogpayorder */

/*!50001 DROP TABLE IF EXISTS `vlogpayorder` */;
/*!50001 DROP VIEW IF EXISTS `vlogpayorder` */;

/*!50001 CREATE ALGORITHM=UNDEFINED SQL SECURITY DEFINER VIEW `vlogpayorder` AS (select `tlogpayorder`.`payId` AS `payId`,from_base64(`tlogpayorder`.`game_req`) AS `game_req`,`tlogpayorder`.`game_req_time` AS `game_req_time`,from_base64(`tlogpayorder`.`req_data`) AS `req`,`tlogpayorder`.`req_time` AS `req_time`,from_base64(`tlogpayorder`.`resp_header`) AS `resp_header`,from_base64(`tlogpayorder`.`resp_data`) AS `resp`,`tlogpayorder`.`resp_time` AS `resp_time`,from_base64(`tlogpayorder`.`notify_data`) AS `notify`,`tlogpayorder`.`notify_time` AS `notify_time` from `tlogpayorder`) */;

/*View structure for view vpaychannel */

/*!50001 DROP TABLE IF EXISTS `vpaychannel` */;
/*!50001 DROP VIEW IF EXISTS `vpaychannel` */;

/*!50001 CREATE ALGORITHM=UNDEFINED SQL SECURITY DEFINER VIEW `vpaychannel` AS (select `tpaychannel`.`id` AS `id`,`tpaychannel`.`sdk_code` AS `sdk_code`,`tpaychannel`.`game_code` AS `game_code`,`tpaychannel`.`desc` AS `desc`,`tpaychannel`.`keydata` AS `keydata`,`tpaychannel`.`active_time` AS `active_time`,`tpaychannel`.`money_range` AS `money_range`,`tpaychannel`.`weight` AS `weight`,`tpaychannel`.`interval` AS `interval` from `tpaychannel` where (`tpaychannel`.`status` <> 0) order by `tpaychannel`.`game_code`,`tpaychannel`.`sdk_code`,`tpaychannel`.`method_code`) */;

/*View structure for view vpayorder */

/*!50001 DROP TABLE IF EXISTS `vpayorder` */;
/*!50001 DROP VIEW IF EXISTS `vpayorder` */;

/*!50001 CREATE ALGORITHM=UNDEFINED SQL SECURITY DEFINER VIEW `vpayorder` AS select `t1`.`id` AS `payId`,`t1`.`uin` AS `uin`,`t1`.`userName` AS `nick`,`t1`.`create_channel` AS `channel`,`t1`.`user_createtime` AS `user_createtime`,`t1`.`playerIP` AS `IP`,`t1`.`sdk_code` AS `sdk`,`t3`.`name` AS `SDKName`,`t1`.`method_code` AS `method`,`t4`.`name` AS `MethodName`,`t1`.`channel_code` AS `channel_code`,`t1`.`gameOrder` AS `gameOrderNo`,`t1`.`orderNo` AS `orderNo`,`t1`.`thirdPartyOrder` AS `platformOrderNo`,round((`t1`.`totalFee` / 100.0),2) AS `totalFee`,`t1`.`orderStatus` AS `status`,`t2`.`note` AS `statusNote`,`t1`.`createdAt` AS `create_time`,`t1`.`updatedAt` AS `last_updatetime`,round((`t1`.`realAmount` / 100.0),2) AS `realFee`,`t1`.`channelId` AS `channelId`,`t1`.`channelName` AS `channelName` from (((`tpayorder` `t1` join `dict_orderstatus` `t2`) join `tsdk` `t3`) join `dict_paymethod` `t4`) where ((`t1`.`orderStatus` = `t2`.`status`) and (`t1`.`sdk_code` = `t3`.`code`) and (`t1`.`method_code` = `t4`.`method`)) */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
