DROP TABLE user;
CREATE TABLE user
	(
	id INTEGER PRIMARY KEY,
	login VARCHAR(16) NOT NULL,
	password VARCHAR(40)
	);

INSERT INTO user ( id, login, password ) VALUES ( '1', 'amok', '1c29cf0ceb89afce131e27b76c18af1e9cf7f5e3' );
INSERT INTO user ( id, login, password ) VALUES ( '2', 'aNeutrino', 'b4434d5d3638c82cf2dc25c9e4cb75b1f7110e05' );
