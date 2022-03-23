drop table `svnstat`;

CREATE TABLE `svnstat` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '自增ID',
  `name` VARCHAR (64) DEFAULT '' COMMENT 'SVN库别名',
  `rev` INT DEFAULT 0 COMMENT 'REV',
  `user` VARCHAR (64) DEFAULT '' COMMENT '程序员',
  `line` INT DEFAULT 0 COMMENT '代码行',
  `createtime` DATETIME COMMENT '创建时间',
  PRIMARY KEY (`id`),
  UNIQUE INDEX `index_by_rev` (`name`, `rev`),
  KEY `index_by_user` (`user`)
);

