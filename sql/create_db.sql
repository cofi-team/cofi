DROP DATABASE IF EXISTS cofi;
DROP USER IF EXISTS cofi;

CREATE USER cofi WITH PASSWORD 'cofi';
CREATE DATABASE cofi;
GRANT ALL PRIVILEGES ON DATABASE cofi to cofi;
ALTER DATABASE cofi OWNER TO cofi;
