DELIMITER $$

USE `paycenter_ex`$$

DROP PROCEDURE IF EXISTS `pQueryChannelAll`$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `pQueryChannelAll`()
BEGIN
        SELECT t2.`id` AS 'payId', t1.`id` AS 'configId', t2.`gameCode`,t2.`sdkCode`,
               t2.`channelCode`,t2.`payStyleCode` AS 'methodCode',t2.`name`,
               t2.`activeTime`,t2.`moneyRange`,t2.`weight`,t2.`interval`,t2.`cgi`,t2.`clientFlag`
        FROM `t_channel` t1, `t_sdkchannel` t2
        WHERE t2.`channelCode`=t1.`code` AND t2.`gameCode`=t1.`gameCode` AND t1.`status`=1 AND t2.`status`=1;
    END$$

DELIMITER ;

