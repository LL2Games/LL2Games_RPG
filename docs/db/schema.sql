-- MySQL dump 10.13  Distrib 9.5.0, for Linux (x86_64)
--
-- Host: localhost    Database: game
-- ------------------------------------------------------
-- Server version	9.5.0

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
SET @MYSQLDUMP_TEMP_LOG_BIN = @@SESSION.SQL_LOG_BIN;
SET @@SESSION.SQL_LOG_BIN= 0;

--
-- GTID state at the beginning of the backup 
--

SET @@GLOBAL.GTID_PURGED=/*!80000 '+'*/ '0f2fec67-b96a-11f0-9ce5-0242ac110002:1-284';

--
-- Table structure for table `account`
--

DROP TABLE IF EXISTS `account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `account` (
  `account_id` varchar(64) COLLATE utf8mb4_unicode_ci NOT NULL,
  `created_at` datetime DEFAULT NULL,
  PRIMARY KEY (`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `channel`
--

DROP TABLE IF EXISTS `channel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `channel` (
  `channel_id` varchar(32) NOT NULL,
  `ip` varchar(45) NOT NULL,
  `port` int NOT NULL,
  `max_users` int NOT NULL,
  `current_users` int NOT NULL DEFAULT '0',
  `alive` tinyint(1) NOT NULL DEFAULT '1',
  `last_heartbeat` datetime NOT NULL,
  PRIMARY KEY (`channel_id`),
  KEY `idx_alive` (`alive`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character`
--

DROP TABLE IF EXISTS `character`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `character` (
  `char_id` bigint NOT NULL AUTO_INCREMENT,
  `account_id` varchar(64) COLLATE utf8mb4_unicode_ci NOT NULL,
  `name` varchar(32) COLLATE utf8mb4_unicode_ci NOT NULL,
  `level` int DEFAULT '1',
  `job` int DEFAULT NULL,
  `last_login` datetime DEFAULT NULL,
  `created_at` datetime DEFAULT NULL,
  `root_job` int DEFAULT '0' COMMENT '캐릭터 0차 직업',
  PRIMARY KEY (`char_id`),
  UNIQUE KEY `uk_account_char` (`account_id`,`name`),
  KEY `idx_account_id` (`account_id`),
  KEY `idx_character_account_id` (`account_id`),
  CONSTRAINT `fk_character_users` FOREIGN KEY (`account_id`) REFERENCES `users` (`account_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_inventory`
--

DROP TABLE IF EXISTS `character_inventory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_inventory` (
  `char_id` bigint NOT NULL,
  `inventory_type` tinyint NOT NULL,
  `slot_pos` smallint NOT NULL,
  `item_id` int NOT NULL,
  `item_count` int NOT NULL DEFAULT '0',
  `created_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`char_id`,`inventory_type`,`slot_pos`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_inventory_equip`
--

DROP TABLE IF EXISTS `character_inventory_equip`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_inventory_equip` (
  `instance_id` bigint NOT NULL AUTO_INCREMENT,
  `char_id` bigint NOT NULL,
  `item_id` int NOT NULL,
  `inventory_type` tinyint NOT NULL,
  `slot_pos` int NOT NULL,
  `enhance` int NOT NULL DEFAULT '0',
  `atk` int NOT NULL DEFAULT '0',
  `def` int NOT NULL DEFAULT '0',
  PRIMARY KEY (`instance_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_inventory_meta`
--

DROP TABLE IF EXISTS `character_inventory_meta`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_inventory_meta` (
  `char_id` bigint NOT NULL,
  `inventory_type` tinyint NOT NULL,
  `max_slot` int NOT NULL,
  `current_slot_count` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_quickslot`
--

DROP TABLE IF EXISTS `character_quickslot`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_quickslot` (
  `char_id` int NOT NULL,
  `slot_index` int NOT NULL,
  `quickslot_type` int NOT NULL,
  `skill_id` int NOT NULL DEFAULT '0',
  `inventory_type` int NOT NULL DEFAULT '0',
  `inventory_slot_pos` int NOT NULL DEFAULT '0',
  PRIMARY KEY (`char_id`,`slot_index`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_skill`
--

DROP TABLE IF EXISTS `character_skill`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_skill` (
  `char_id` bigint DEFAULT NULL,
  `skill_id` int DEFAULT NULL,
  `skill_level` int DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character_stat`
--

DROP TABLE IF EXISTS `character_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `character_stat` (
  `char_id` bigint NOT NULL COMMENT '캐릭터 ID (character 테이블 PK, 1:1)',
  `str` int NOT NULL COMMENT '힘',
  `dex` int NOT NULL COMMENT '민첩',
  `intel` int NOT NULL COMMENT '지능',
  `luk` int NOT NULL COMMENT '운',
  `max_hp` int NOT NULL COMMENT '최대 HP',
  `max_mp` int NOT NULL COMMENT '최대 MP',
  `cur_hp` int NOT NULL COMMENT '현재 HP',
  `cur_mp` int NOT NULL COMMENT '현재 MP',
  `remain_ap` int NOT NULL COMMENT '남은 능력치 포인트',
  `updated_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '마지막 갱신 시각',
  `exp` bigint NOT NULL DEFAULT '0',
  `level` int NOT NULL DEFAULT '1' COMMENT '레벨',
  PRIMARY KEY (`char_id`),
  CONSTRAINT `fk_character_stat_char` FOREIGN KEY (`char_id`) REFERENCES `character` (`char_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='캐릭터 능력치 (1:1)';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `level_exp`
--

DROP TABLE IF EXISTS `level_exp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `level_exp` (
  `level` int NOT NULL COMMENT '레벨',
  `need_exp` bigint NOT NULL COMMENT '필요한 경험치'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `users` (
  `account_id` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  `passwd` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  PRIMARY KEY (`account_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
SET @@SESSION.SQL_LOG_BIN = @MYSQLDUMP_TEMP_LOG_BIN;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-06-11 10:00:56
