DELIMITER $$

USE `paycenter_v3`$$

DROP PROCEDURE IF EXISTS `pWithdrawReq`$$

CREATE PROCEDURE `pWithdrawReq`(IN param_gameCode    VARCHAR(32),    -- 游戏代码
        IN param_uin            VARCHAR(64),    -- 用户唯一ID
        IN param_bankCode       VARCHAR(32),    -- 银行编码
        IN param_bankCard       VARCHAR(64),    -- 银行账号
        IN param_accountName    VARCHAR(64),    -- 用户名
        IN param_fee            BIGINT,         -- 提现到账金额，以元计
        IN param_tax            BIGINT,         -- 提现税收，以元计
        IN param_ip             VARCHAR(16),    -- 提现时的IP
        IN param_createTime     BIGINT,         -- 提现时间
        IN param_thirdorder     VARCHAR(32))
    COMMENT '简化版本的提现订单'
BEGIN
    DECLARE   var_code          INT DEFAULT 0;
    DECLARE   var_message       VARCHAR(128) DEFAULT 'OK';
    DECLARE   var_orderNo       VARCHAR(32) DEFAULT '';
    DECLARE     var_ipdata              VARCHAR(128) DEFAULT '';
    DECLARE     var_merchant            VARCHAR(32) DEFAULT '';
    
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        GET DIAGNOSTICS CONDITION 1 @p1=RETURNED_SQLSTATE,@p2= MESSAGE_TEXT;
        SET var_code=-1000, var_message=CONCAT("SQL Fault, State:", @p1, ", Message:", @p2);
        SELECT var_code, var_message, '';
    END;

    SET var_orderNo = CONCAT('W', UNIX_TIMESTAMP(), 'R', SUBSTRING(MD5(RAND()),1,8));
    
    SELECT CONCAT(`country`,`province`,`city`,'(',`operator` ,')') INTO var_ipdata
    FROM `t_ipdata` WHERE `end`>=INET_ATON(param_ip) ORDER BY END LIMIT 1;
    /*获取提现规则*/
    SELECT `value` INTO @max_count FROM dict_param WHERE param = 'withdraw_count';
    SELECT `value` INTO @is_withdraw FROM dict_param WHERE param = 'withdraw_unpay';
    
    SELECT COUNT(*) INTO @withdraw_count FROM t_withdraw a WHERE a.uuid = param_uin AND a.create_time >= CURDATE() AND a.gameCode = param_gameCode;

    START TRANSACTION;
    LABEL_MAIN: BEGIN
    /*
    IF @withdraw_count >= @max_count THEN
        ROLLBACK;
        SET var_code=-999, var_message='Too many withdrawals'; 
        LEAVE LABEL_MAIN;
    END IF;
    
    IF @is_withdraw > 0 THEN
        IF EXISTS (SELECT 1 FROM t_payorder where uin = param_uin AND gameCode = param_gameCode AND orderStatus in(1,5,8,9,10) LIMIT 1) THEN
            ROLLBACK;
            SET var_code=-1000, var_message='No withdrawal without recharge'; 
            LEAVE LABEL_MAIN;
        END IF;
    END IF;
    */
    INSERT INTO t_withdraw(order_id,`gameCode`,UUID,
                                         create_time,pay_out_money,account_no,
                                         money, duty_money, real_money, `status`,
                                         player_ip,bank_code,account_name,player_ipaddress,third_order)
    VALUES (var_orderNo,param_gameCode,param_uin,
          FROM_UNIXTIME(param_createTime),param_fee*100,param_bankCard,
                    (param_fee+param_tax)*100,param_tax*100,param_fee*100,1,
                    param_ip,param_bankCode,param_accountName,var_ipdata,param_thirdorder);
    IF (ROW_COUNT()=0) THEN
        BEGIN
            ROLLBACK;   
            SET var_code=-1001, var_message='Create withdraw order failure'; 
            LEAVE LABEL_MAIN;
        END;
    END IF;
    
    INSERT INTO `log_withdraw`(`withdraw_id`,`status`,`create_time`,`update_time`)
    VALUES(var_orderNo, 1,FROM_UNIXTIME(param_createTime),FROM_UNIXTIME(param_createTime));
    IF (ROW_COUNT()=0) THEN
        BEGIN
            ROLLBACK;
            SET var_code=-1003, var_message='Save pay order log failure'; 
            LEAVE LABEL_MAIN;
        END;
    END IF;
    END LABEL_MAIN;
    COMMIT;
        
    SELECT var_code, var_message, var_orderNo AS 'ORDERNO';
END$$

DELIMITER ;

DELIMITER $$

USE `paycenter_v3`$$

DROP PROCEDURE IF EXISTS `pWithdrawAudit_Query`$$

CREATE PROCEDURE `pWithdrawAudit_Query`(IN param_gameCode       VARCHAR(32),    -- 游戏代码
        IN param_sdkCode        VARCHAR(32),    -- SDK代码
        IN param_orderNo        VARCHAR(64))
    COMMENT '查询需要SDK转帐的基础数据'
BEGIN
        DECLARE  var_withdrawCgi    VARCHAR(128);
        DECLARE  var_token          VARCHAR(32);
        
        SET var_token = CONCAT('T', UNIX_TIMESTAMP(), 'K', SUBSTRING(MD5(RAND()),1,8));
        
        SELECT `withdraw_cgi` INTO var_withdrawCgi
        FROM `t_sdk`
        WHERE `gameCode`=param_gameCode AND `code`=param_sdkCode AND `status`=1;
        
        SELECT `uuid` AS 'uin',`bank_code` AS 'bankCode',`account_no` AS 'bankCard',`account_name` AS 'bankName',
            `real_money` AS 'fee', var_withdrawCgi AS 'cgi', var_token AS 'token'
        FROM `t_withdraw`
        WHERE `gameCode`=param_gameCode AND `order_id`=param_orderNo AND `status`=4;
        
    END$$

DELIMITER ;

