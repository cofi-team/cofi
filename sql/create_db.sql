DROP TABLE IF EXISTS command;
DROP DATABASE IF EXISTS cofi;
DROP USER IF EXISTS cofi;

CREATE USER cofi WITH PASSWORD 'cofi';
CREATE DATABASE cofi;
GRANT ALL PRIVILEGES ON DATABASE cofi to cofi;
ALTER DATABASE cofi OWNER TO cofi;

\c cofi;

CREATE TABLE command (
	tag text,
	cmd text CONSTRAINT cmd_key PRIMARY KEY,
	description text
);

