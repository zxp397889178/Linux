DELIMITER $$

USE `paycenter_v3`$$

DROP PROCEDURE IF EXISTS `pUserBillpointStat_v3`$$

CREATE PROCEDURE `pUserBillpointStat_v3`(
        IN param_today DATETIME,    -- 查询的日期
        IN param_style INT)
BEGIN
        DECLARE var_dateBegin   DATETIME;
        DECLARE var_dateEnd     DATETIME;
        
        IF (param_style = 0) THEN
        BEGIN
            SELECT `uin`,`bpId`,`dealCount` FROM `t_billpoint_stat` 
            ORDER BY `uin`;
        END;
        ELSEIF (param_style = 1) THEN
        BEGIN
            SET var_dateBegin = param_today;
            SET var_dateEnd = DATE_ADD(param_today, INTERVAL 1 DAY);
            
            SELECT `uin`, `bpId`, COUNT(*)
            FROM `t_payorder`
            WHERE `orderStatus` IN (5,8,9,10) AND (`createdAt`>=var_dateBegin AND `createdAt`<var_dateEnd)
            GROUP BY `uin`,`bpId`;
        END;
        ELSEIF (param_style = 2) THEN
        BEGIN
            SET var_dateBegin = DATE_SUB(param_today, INTERVAL WEEKDAY(param_today)+1 DAY);
            SET var_dateEnd = DATE_ADD(var_dateBegin, INTERVAL 1 WEEK);

            SELECT `uin`,`bpId`,COUNT(*)
            FROM `t_payorder` 
            WHERE `orderStatus` IN (5,8,9,10) AND `createdAt`>=var_dateBegin  AND `createdAt`< var_dateEnd 
            GROUP BY `uin`,`bpId`;
        END;
        ELSEIF (param_style = 3) THEN
        BEGIN
            SET var_dateEnd = DATE_ADD(LAST_DAY(param_today), INTERVAL 1 DAY);
            SET var_dateBegin = DATE_SUB(var_dateEnd, INTERVAL 1 MONTH);
            
            SELECT `uin`,`bpId`,COUNT(*)
            FROM `t_payorder` 
            WHERE `orderStatus` IN (5,8,9,10) AND `createdAt`>=var_dateBegin  AND `createdAt`<var_dateEnd 
            GROUP BY `uin`,`bpId`;
        END;
        END IF;
        SELECT var_dateBegin, var_dateEnd;
    END$$

DELIMITER ;

