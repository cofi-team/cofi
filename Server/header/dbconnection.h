#include <pqxx/pqxx>

class DBConnection 
{
	#define DEFAULT_DBNAME "cofi"
	#define DEFAULT_DBUSER "cofi"
	#define DEFAULT_DBPWD "cofi"
	#define DEFAULT_DBHOST "127.0.0.1"
	#define DEFAULT_DBPORT 5432

	public:	
		DBConnection();
		DBConnection(char* dbname, char* user, char* pwd, char* host, int port);
		DBConnection(DBConnection &cmdReq);
	
		void openConnection();
		void closeConnection(pqxx::connection&);
		pqxx::connection& getConnection();
		

	private:
		char *dbname;
		char *user;
		char *pwd;
		char *host;
		int port;
		pqxx::connection c;
};

class DatabaseException { };
class OpenConnectionException : public DatabaseException { };
