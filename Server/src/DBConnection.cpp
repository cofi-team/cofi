#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <unistd.h>
#include <cstdlib>

#include <iostream>
#include <pqxx/pqxx>

#include "../header/dbconnection.h"

using namespace std;
using namespace pqxx;
using namespace std;

DBConnection::DBConnection() {
	this->dbname = DEFAULT_DBNAME;
	this->user = DEFAULT_DBUSER;
	this->host = DEFAULT_DBHOST;
	this->pwd = DEFAULT_DBPWD;
	this->port = DEFAULT_DBPORT;
}

DBConnection::DBConnection(DBConnection &dbCon) {
	this->dbname = dbCon.dbname;
	this->user = dbCon.user;
	this->host = dbCon.host;
	this->pwd = dbCon.pwd;
	this->port = dbCon.port;
}

DBConnection::DBConnection(char *dbname, char *user, char *host, char *pwd, int port) {
	this->dbname = dbname;
	this->user = user;
	this->host = host;
	this->pwd = pwd;
	this->port = port;
}

connection& DBConnection::openConnection() {

	static connection c("dbname=cofi user=cofi password=cofi hostaddr=127.0.0.1 port=5432");

	c.is_open();
	if(c.is_open()) {
			cout << "Opened database successfully: " << c.dbname() << endl;


	} else {
			cout << "Can't open database" << endl;
			throw OpenConnectionException();
	}

	return c;
}

