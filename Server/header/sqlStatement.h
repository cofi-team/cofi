#include <iostream>
#include <pqxx/pqxx>

class SqlStatement
{
	public:
		pqxx::result execute(pqxx::connection& conn, char *);
};
