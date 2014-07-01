#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <unistd.h>
#include <cstdlib>

#include <pqxx/pqxx>

#include "../header/server.h"
#include "../header/commandRequest.h"
#include "../header/dbconnection.h"
#include "../header/sqlStatement.h"


pqxx::result SqlStatement::execute(pqxx::connection& conn, char *sqlCommand) {

	// Start a transaction.  With libpqxx we're always working in
	// a transaction.
	//pqxx::work txn(conn);
	pqxx::nontransaction ntx(conn);


	//pqxx::transaction_base &txn);

	// Execute database query and immediately retrieve results.
	pqxx::result res = ntx.exec(sqlCommand);

	//txn.commit();

	conn.disconnect();

	return res;
}
