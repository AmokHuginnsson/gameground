-- Cleanup database. {{{
DROP TRIGGER IF EXISTS tgg_user_session_update;
DROP TRIGGER IF EXISTS tgg_user_session_update_id;
DROP TRIGGER IF EXISTS tgg_user_session_insert;
DROP TRIGGER IF EXISTS tgg_user_session_delete;
DROP VIEW IF EXISTS v_user_session;
DROP TRIGGER IF EXISTS tgg_session_id_user_delete;
DROP TRIGGER IF EXISTS tgg_session_id_user_update;
DROP TRIGGER IF EXISTS tgg_session_id_user_insert;
DROP TRIGGER IF EXISTS tgg_stat_id_game_update;
DROP TRIGGER IF EXISTS tgg_stat_id_game_insert;
DROP TRIGGER IF EXISTS tgg_stat_id_user_update;
DROP TRIGGER IF EXISTS tgg_stat_id_user_insert;
DROP TRIGGER IF EXISTS tgg_relationship_id_relation_update;
DROP TRIGGER IF EXISTS tgg_relationship_id_relation_insert;
DROP TRIGGER IF EXISTS tgg_relationship_id_user_id_related_user_update;
DROP TRIGGER IF EXISTS tgg_relationship_id_related_user_insert;
DROP TRIGGER IF EXISTS tgg_relationship_id_user_insert;
DROP TRIGGER IF EXISTS tgg_relation_delete;
DROP TRIGGER IF EXISTS tgg_relation_update;
DROP TRIGGER IF EXISTS tgg_game_delete;
DROP TRIGGER IF EXISTS tgg_game_update;
DROP TRIGGER IF EXISTS tgg_user_recursive_delete;
DROP TRIGGER IF EXISTS tgg_user_checked_delete;
DROP TRIGGER IF EXISTS tgg_user_update;
DROP TRIGGER IF EXISTS tgg_user_insert;
DROP INDEX IF EXISTS idx_stat_id_user;
DROP INDEX IF EXISTS idx_relationship_id_user;
DROP TABLE IF EXISTS tbl_session;
DROP TABLE IF EXISTS tbl_stat;
DROP TABLE IF EXISTS tbl_relationship;
DROP TABLE IF EXISTS tbl_game;
DROP TABLE IF EXISTS tbl_relation;
DROP TABLE IF EXISTS tbl_user;
-- }}}

-- Create tables. {{{
-- Service user login information. A slow changing table.
CREATE TABLE tbl_user (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	login VARCHAR(16) UNIQUE NOT NULL,
	password VARCHAR(40),
	name VARCHAR(255),
	email VARCHAR(255),
	description VARCHAR(255),
	setup VARCHAR(255),
	registered DATETIME
);

-- Game type configuration and information.
CREATE TABLE tbl_game (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	name VARCHAR(16) UNIQUE NOT NULL
);

-- Relation type configuration and information.
CREATE TABLE tbl_relation (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	name VARCHAR(16) UNIQUE NOT NULL
);

-- Played out game statictics.
CREATE TABLE tbl_relationship (
	id_user INTEGER NOT NULL,
	id_related_user INTEGER NOT NULL,
	id_relation VARCHAR(16) NOT NULL REFERENCES tbl_relation( id )
);

-- Played out game statictics.
CREATE TABLE tbl_stat (
	id_user INTEGER NOT NULL,
	id_game INTEGER NOT NULL,
	score FLOAT
);

-- Session information should not be stored in tbl_user
-- cause session infromation changes frequently while user information
-- changes rarely.
-- There is 1 to 1 relation between tbl_user and tbl_session.
CREATE TABLE tbl_session (
	id_user INTEGER NOT NULL,
	last_activity DATETIME NOT NULL
);
CREATE INDEX idx_stat_id_user ON tbl_stat ( id_user );
CREATE INDEX idx_relationship_id_user ON tbl_relationship ( id_user );
-- }}}

-- Create triggers {{{
-- Automatically create session information while creating user data.
CREATE TRIGGER tgg_user_insert AFTER INSERT ON tbl_user FOR EACH ROW
BEGIN
	INSERT INTO tbl_session ( id_user, last_activity ) VALUES ( NEW.id, datetime( 'now', 'localtime' ) );
END;

-- Make tbl_user.id read only.
CREATE TRIGGER tgg_user_update BEFORE UPDATE ON tbl_user FOR EACH ROW WHEN
	NEW.id <> OLD.id
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during UPDATE on TABLE tbl_user on COLUMN id." );
END;

-- Prevent removal of user record if related records exists in other tables.
CREATE TRIGGER tgg_user_checked_delete BEFORE DELETE ON tbl_user FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_stat WHERE id_user = OLD.id ) <> 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during DELETE on TABLE tbl_user on COLUMN id." );
END;

-- Automatically remove related record from tbl_session when user record is removed.
CREATE TRIGGER tgg_user_recursive_delete AFTER DELETE ON tbl_user FOR EACH ROW
BEGIN
	DELETE FROM tbl_session WHERE id_user = OLD.id;
	DELETE FROM tbl_relationship WHERE id_user = OLD.id OR id_related_user = OLD.id;
	DELETE FROM tbl_stat WHERE id_user = OLD.id;
END;

-- Make tbl_game.id read only.
CREATE TRIGGER tgg_game_update BEFORE UPDATE ON tbl_game FOR EACH ROW WHEN
	NEW.id <> OLD.id
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during UPDATE on TABLE tbl_game on COLUMN id." );
END;

-- Prevent removal of game record if related records exists in other tables.
CREATE TRIGGER tgg_game_delete BEFORE DELETE ON tbl_game FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_stat WHERE id_game = OLD.id ) <> 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during DELETE on TABLE tbl_game on COLUMN id." );
END;

-- Make tbl_relation.id read only.
CREATE TRIGGER tgg_relation_update BEFORE UPDATE ON tbl_relation FOR EACH ROW WHEN
	NEW.id <> OLD.id
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during UPDATE on TABLE tbl_relation on COLUMN id." );
END;

-- Prevent removal of relation record if related records exists in other tables.
CREATE TRIGGER tgg_relation_delete BEFORE DELETE ON tbl_relation FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_relationship WHERE id_relation = OLD.id ) <> 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during DELETE on TABLE tbl_relation on COLUMN id." );
END;

-- Prevent creation of relationship record with no related relation record.
CREATE TRIGGER tgg_relationship_id_relation_insert BEFORE INSERT ON tbl_relationship FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_relation WHERE id = NEW.id_relation ) = 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during INSERT on TABLE tbl_relationship on COLUMN id_relation." );
END;

-- Prevent creation of relationship record with no related user record.
CREATE TRIGGER tgg_relationship_id_user_insert BEFORE INSERT ON tbl_relationship FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_user WHERE id = NEW.id_user ) = 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during INSERT on TABLE tbl_relationship on COLUMN id_user." );
END;

-- Prevent creation of relationship record with no related user (related) record.
CREATE TRIGGER tgg_relationship_id_related_user_insert BEFORE INSERT ON tbl_relationship FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_user WHERE id = NEW.id_related_user ) = 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during INSERT on TABLE tbl_relationship on COLUMN id_related_user." );
END;

-- Make tbl_relationship.id_user and tbl_relationship.id_related_user read only.
CREATE TRIGGER tgg_relationship_id_user_id_related_user_update BEFORE UPDATE ON tbl_relationship FOR EACH ROW WHEN
	OLD.id_user <> NEW.id_user OR OLD.id_related_user <> NEW.id_related_user
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during UPDATE on TABLE tbl_relationship on COLUMN id_user OR id_related_user." );
END;

-- Prevent creation of stat record with no related user record.
CREATE TRIGGER tgg_stat_id_user_insert BEFORE INSERT ON tbl_stat FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_user WHERE id = NEW.id_user ) = 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during INSERT on TABLE tbl_stat on COLUMN id_user." );
END;

-- Prevent creation of stat record with no related game record.
CREATE TRIGGER tgg_stat_id_game_insert BEFORE INSERT ON tbl_stat FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_game WHERE id = NEW.id_game ) = 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during INSERT on TABLE tbl_stat on COLUMN id_game." );
END;

-- Make tbl_stat.id_user read only.
CREATE TRIGGER tgg_stat_id_user_update BEFORE UPDATE ON tbl_stat FOR EACH ROW WHEN
	OLD.id_user <> NEW.id_user
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during UPDATE on TABLE tbl_stat on COLUMN id_user." );
END;

-- Make tbl_stat.id_game read only.
CREATE TRIGGER tgg_stat_id_game_update BEFORE UPDATE ON tbl_stat FOR EACH ROW WHEN
	OLD.id_game <> NEW.id_game
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during UPDATE on TABLE tbl_stat on COLUMN id_game." );
END;

-- Prevent creation of session record with no related user record.
CREATE TRIGGER tgg_session_id_user_insert BEFORE INSERT ON tbl_session FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_user WHERE id = NEW.id_user ) = 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during INSERT on TABLE tbl_session on COLUMN id_user." );
END;

-- Make tbl_session.id_user read only.
CREATE TRIGGER tgg_session_id_user_update BEFORE UPDATE ON tbl_session FOR EACH ROW WHEN
	OLD.id_user <> NEW.id_user
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during UPDATE on TABLE tbl_session on COLUMN id_user." );
END;

-- Prevent removal of session record if related user record still exists.
CREATE TRIGGER tgg_session_id_user_delete BEFORE DELETE ON tbl_session FOR EACH ROW WHEN
	( SELECT COUNT(*) FROM tbl_user WHERE id = OLD.id_user ) <> 0
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during DELETE on TABLE tbl_session on COLUMN id_user." );
END;

CREATE VIEW v_user_session AS SELECT tbl_user.*, tbl_session.last_activity FROM tbl_user LEFT JOIN tbl_session ON tbl_session.id_user = tbl_user.id;
CREATE TRIGGER tgg_user_session_delete INSTEAD OF DELETE ON v_user_session
BEGIN
	DELETE FROM tbl_user WHERE id = OLD.id;
END;

CREATE TRIGGER tgg_user_session_insert INSTEAD OF INSERT ON v_user_session
BEGIN
	INSERT INTO tbl_user ( id, login, password, name, email, description, registered )
		VALUES ( NEW.id, NEW.login, NEW.password, NEW.name, NEW.email, NEW.description, NEW.registered );
END;

-- Make v_user_session.id read-only.
CREATE TRIGGER tgg_user_session_update_id INSTEAD OF UPDATE ON v_user_session WHEN
	OLD.id <> NEW.id
BEGIN
	SELECT RAISE ( FAIL, "Relation broken during UPDATE on VIEW v_user_session on COLUMN id." );
END;

CREATE TRIGGER tgg_user_session_update INSTEAD OF UPDATE ON v_user_session
BEGIN
	UPDATE tbl_user SET login = NEW.login, password = NEW.password,
		name = NEW.name, email = NEW.email, description = NEW.description,
		registered = NEW.registered WHERE id = OLD.id;
	UPDATE tbl_session SET last_activity = NEW.last_activity WHERE id_user = OLD.id;
END;

-- }}}

-- Insert data. {{{
INSERT INTO tbl_game ( name ) VALUES ( 'go' );
INSERT INTO tbl_game ( name ) VALUES ( 'boggle' );
INSERT INTO tbl_game ( name ) VALUES ( 'galaxy' );
INSERT INTO tbl_game ( name ) VALUES ( 'gomoku' );

INSERT INTO tbl_relation ( name ) VALUES ( 'friend' );
INSERT INTO tbl_relation ( name ) VALUES ( 'blocked' );
-- }}}

