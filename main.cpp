#include <iostream>
#include <pqxx/pqxx>

#include "server.cpp"

using namespace std;
using namespace pqxx;

int main()
{
	try {
		connection C("dbname=cofi user=cofi password=cofi hostaddr=127.0.0.1 port=5432");
		
		if(C.is_open()) {
			cout << "Opened database successfully: " << C.dbname() << endl;

			BlocketServer server;
			//server.execute();
			
		} else {
			cout << "Can't open database" << endl;
			return 1;
		}
	} catch(const std::exception &e) {
		cerr << e.what() << endl;
		return 1;
	}
}
