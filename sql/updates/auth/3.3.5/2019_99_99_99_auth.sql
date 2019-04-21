ALTER TABLE `account` ADD COLUMN `timezone_bias` INT(10) DEFAULT 0 NOT NULL COMMENT 'Timezone difference from UTC in seconds without counting daylight saving' AFTER `recruiter`;
