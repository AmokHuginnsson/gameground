DROP TRIGGER IF EXISTS tgg_stats_id_game_update;
DROP TRIGGER IF EXISTS tgg_stats_id_game_insert;
DROP TRIGGER IF EXISTS tgg_stats_id_user_update;
DROP TRIGGER IF EXISTS tgg_stats_id_user_insert;
DROP TRIGGER IF EXISTS tgg_game_delete;
DROP TRIGGER IF EXISTS tgg_game_update;
DROP TRIGGER IF EXISTS tgg_user_delete;
DROP TRIGGER IF EXISTS tgg_user_update;
DROP INDEX IF EXISTS idx_stats_id_user;
DROP INDEX IF EXISTS idx_stats_id_game;
DROP TABLE IF EXISTS tbl_stats;
DROP TABLE IF EXISTS tbl_game;
DROP TABLE IF EXISTS tbl_user;

CREATE TABLE tbl_user (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	login VARCHAR(16) UNIQUE NOT NULL,
	password VARCHAR(40),
	name VARCHAR(255),
	email VARCHAR(255),
	description VARCHAR(255)
);

CREATE TABLE tbl_game (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	name VARCHAR(16) UNIQUE NOT NULL
);

CREATE TABLE tbl_stats (
	id_user INTEGER NOT NULL,
	id_game INTEGER NOT NULL,
	score FLOAT
);
CREATE INDEX idx_stats_id_user ON tbl_stats ( id_user );
CREATE INDEX idx_stats_id_game ON tbl_stats ( id_game );
CREATE TRIGGER tgg_user_update BEFORE UPDATE ON tbl_user FOR EACH ROW WHEN
	NEW.id <> OLD.id
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during UPDATE on TABLE tbl_user on COLUMN id." );
END;
CREATE TRIGGER tgg_user_delete BEFORE DELETE ON tbl_user FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_stats WHERE id_user = OLD.id ) <> 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during DELETE on TABLE tbl_user on COLUMN id." );
END;
CREATE TRIGGER tgg_game_update BEFORE UPDATE ON tbl_game FOR EACH ROW WHEN
	NEW.id <> OLD.id
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during UPDATE on TABLE tbl_game on COLUMN id." );
END;
CREATE TRIGGER tgg_game_delete BEFORE DELETE ON tbl_game FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_stats WHERE id_game = OLD.id ) <> 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during DELETE on TABLE tbl_game on COLUMN id." );
END;
CREATE TRIGGER tgg_stats_id_user_insert BEFORE INSERT ON tbl_stats FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_user WHERE id = NEW.id_user ) = 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during INSERT on TABLE tbl_stats on COLUMN id_user." );
END;
CREATE TRIGGER tgg_stats_id_game_insert BEFORE INSERT ON tbl_stats FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_game WHERE id = NEW.id_game ) = 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during INSERT on TABLE tbl_stats on COLUMN id_game." );
END;
CREATE TRIGGER tgg_stats_id_user_update BEFORE UPDATE ON tbl_stats FOR EACH ROW WHEN
	OLD.id_user <> NEW.id_user
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during UPDATE on TABLE tbl_stats on COLUMN id_user." );
END;
CREATE TRIGGER tgg_stats_id_game_update BEFORE UPDATE ON tbl_stats FOR EACH ROW WHEN
	OLD.id_game <> NEW.id_game
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during UPDATE on TABLE tbl_stats on COLUMN id_game." );
END;

INSERT INTO tbl_game ( name ) VALUES ( 'go' );
INSERT INTO tbl_game ( name ) VALUES ( 'boggle' );
INSERT INTO tbl_game ( name ) VALUES ( 'galaxy' );
INSERT INTO tbl_game ( name ) VALUES ( 'gomoku' );

INSERT INTO tbl_user ( login, password, name, email ) VALUES ( 'amok', '1c29cf0ceb89afce131e27b76c18af1e9cf7f5e3', 'Marcin Konarski', 'amok13@o2.pl' );
INSERT INTO tbl_user ( login, password, name, email ) VALUES ( 'kamawanai', '0c7ebd714bbcfca66cc23ce5652569ccb48d6e48', 'Diana Blaszczyk', 'diana.blaszczyk@gmail.com' );
INSERT INTO tbl_user ( login, password ) VALUES ( 'aNeutrino', 'b4434d5d3638c82cf2dc25c9e4cb75b1f7110e05' );

