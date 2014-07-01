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

DBConnection::DBConnection() : dbname(DEFAULT_DBNAME), user(DEFAULT_DBUSER), host(DEFAULT_DBHOST), pwd(DEFAULT_DBPWD), port(DEFAULT_DBPORT), c("dbname=cofi user=cofi password=cofi hostaddr=127.0.0.1 port=5432") {
}

DBConnection::DBConnection(DBConnection &dbCon) {
	this->dbname = dbCon.dbname;
	this->user = dbCon.user;
	this->host = dbCon.host;
	this->pwd = dbCon.pwd;
	this->port = dbCon.port;
}

DBConnection::DBConnection(char *dbname, char *user, char *host, char *pwd, int port) : dbname(DEFAULT_DBNAME), user(DEFAULT_DBUSER), host(DEFAULT_DBHOST), pwd(DEFAULT_DBPWD), port(DEFAULT_DBPORT), c("dbname=cofi user=cofi password=cofi hostaddr=127.0.0.1 port=5432") {
}

connection& DBConnection::getConnection() {
	return c;
}

void DBConnection::openConnection() {

	connection &con = getConnection();
	con.is_open();
	if(con.is_open()) {
			cout << "Opened database successfully: " << con.dbname() << endl;


	} else {
			cout << "Can't open database" << endl;
			throw OpenConnectionException();
	}
}

void DBConnection::closeConnection(connection &con) {
	//c.close();
}
