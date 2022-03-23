DELIMITER $$
DROP PROCEDURE IF EXISTS `pSvnLastRev`$$
CREATE PROCEDURE `pSvnLastRev`(IN param_svn VARCHAR(64) )
BEGIN
    SELECT IFNULL(MAX(`rev`), 0)+1 FROM `svnstat` WHERE `name`=param_svn;
END$$
DELIMITER ;

DELIMITER $$
DROP PROCEDURE IF EXISTS `pSvnStat`$$
CREATE PROCEDURE `pSvnStat`(
    in param_svn    varchar(64),    -- svn 库别名
    in param_rev    int,        -- rev
    in param_coder  varchar(64),    -- 程序员
    in param_line   int,        -- 代码行
    in param_datetime datetime  -- 创建时间
    )
BEGIN
    insert into `svnstat`(`name`,`rev`,`user`,`line`,`createtime`)
    values(param_svn, param_rev, param_coder, param_line, param_datetime);
END$$
DELIMITER ;
