#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <unistd.h>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <list>

#include "../header/server.h"
#include "../header/commandRequest.h"
#include "../header/dbconnection.h"
#include "../header/sqlStatement.h"
#include "../header/protobuf/out/command.pb.h"

#define closesocket(s) close(s)

#define HTTP_PORT 8096

#define MAXPUF 1023

using namespace std;

static void serv_request(int in, int out, char* rootpath);

static char* getValue(char* keyValue);

static const list<string> getRequestedCommands();

static const list<string> getRequestedCommands() {
	DBConnection con;
	pqxx::connection& connection = con.openConnection();

	string response;
	SqlStatement stm;
	char * sqlstmt = "Select * from command;";
	pqxx::result res = stm.execute(connection, sqlstmt);

	static list<string> dbEntryList;

	for (pqxx::result::const_iterator row = res.begin(); row != res.end(); ++row)
	{
	  // Fields within a row can be accessed by column name.
	  // You can also iterate the fields in a row, or index the row
	  // by column number just like an array.
	  // Values are stored internally as plain strings.  You access
	  // them by converting each to the desired C++ type using the
	  // "as()" function template.
	  std::cout
		<< row["tag"].as<std::string>() << "\t"
		<< row["cmd"].as<std::string>() << "\t"
		<< row["description"].as<std::string>()
		<< std::endl;
	  dbEntryList.push_front(row["description"].as<std::string>());
	  dbEntryList.push_front(row["cmd"].as<std::string>());
	  dbEntryList.push_front(row["tag"].as<std::string>());

	  //response += row["tag"].as<std::string>() + string(" ") + row["cmd"].as<std::string>() + string(" ") + row["description"].as<std::string>();
	}

	//return response.c_str();
	return dbEntryList;
}

void BlockedServer::execute(char **argv)
{
	struct sockaddr_in server, client;
	int sock, fd;
	int len;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		perror("failed to create socket");
		exit(1);
	}

	/* Erzeuge die Socketadresse des Servers
	* Sie besteht aus Typ und Portnummer */

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(HTTP_PORT);

	if(bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
		perror("cant't bind socket");
		exit(1);
	}

	listen(sock, 5);

	for(;;) {
		len = sizeof(client);
		fd = accept(sock, (struct sockaddr*)&client, (socklen_t*)&len);
		if(fd < 0) {
			perror("accept failed");
			exit(1);
		}

		serv_request(fd, fd, argv[1]);
		closesocket(fd);
	}

}

static void serv_request(int in, int out, char* rootpath)
{
	char buffer[8192];
	char *b, *l, *le;
	int count, totalcount;
	char url[256];
	char path[256];
	int fd;
	int eoh = 0;
	char *ptr;

	b = buffer;
	l = buffer;
	totalcount = 0;
	*url = 0;
	while((count = recv(in, b, sizeof(buffer) - totalcount, 0)) > 0) {
		totalcount += count;
		b += count;
		while(l < b) {
			le = l;
			while (le < b && *le != '\n' && *le != '\r') ++le;
			if('\n' == *le || '\r' == *le) {
				*le = 0;
				printf("Header line = %s\n", l);
				sscanf(l, "GET %255s HTTP/", url);
				if(strlen(l)) eoh = 1;
				l = le + 1;
			}
		}
		if(eoh) break;
	}

	if(strlen(url)) {

		char *category;
		char *searchstring;

		// url format: localhost:8096/category=xxx&search=yyy
		printf("got request: GET %s\n", url);

		category = strtok(url, "/&");
		searchstring = strtok(NULL, "&");

		CommandRequest cmdRequest(getValue(category), getValue(searchstring));

		printf("value of category: %s\n", cmdRequest.getCategory());
		printf("value of searchstring: %s\n", cmdRequest.getCommand());

		list<string> dbEntryList = getRequestedCommands();

		GOOGLE_PROTOBUF_VERIFY_VERSION;

		tutorial::Command cmd;

		int i = 0;
		for(list<string>::iterator it = dbEntryList.begin(); it != dbEntryList.end(); it++) {
			if(i == 0) cmd.set_tag((*it).data());
			if(i == 1) cmd.set_cmd((*it).data());
			if(i == 2) cmd.set_description((*it).data());
			i++;
		}

		string serializedCommand;
		cmd.SerializeToString(&serializedCommand);

		std::stringstream html;
		html << "HTTP/1.0 200 OK\nContent-Type: text/html\n\n" << "<html><body><p>" << serializedCommand << "</p></body></html>";
		string htmlString = html.str();

		//char* html = "<html><body>" + string(list) + "</body></html>";
		//sprintf(buffer, "HTTP/1.0 200 OK\nContent-Type: text/html\n\n");
		//send(out, buffer, strlen(buffer), 0);

		/*Date: Fri, 31 Dec 1999 23:59:59 GMT
		Content-Type: text/html
		Content-Length: 1354*/

		sprintf(buffer, htmlString.c_str());

		send(out, buffer, strlen(buffer), 0);
		fflush(stdout);

		google::protobuf::ShutdownProtobufLibrary();


		/*do {
			count = read(fd, buffer, sizeof(buffer));
			send(out, buffer, count, 0);
			printf(".");
			fflush(stdout);
		} while(count > 0);*/
		printf("finished request: GET %s\n", url);

		/*sprintf(path, "%s/%s", rootpath, url);
		fd = open(path, O_RDONLY);
		if(fd > 0) {
			sprintf(buffer, "HTTP/1.0 200 OK\nContent-Type: text/html\n\n");
			send(out, buffer, strlen(buffer), 0);
			do {
				count = read(fd, buffer, sizeof(buffer));
				send(out, buffer, count, 0);
				printf(".");
				fflush(stdout);
			} while(count > 0);
			close(fd);
			printf("finished request: GET %s\n", url);
		} else {
			sprintf(buffer, "HTTP/1.0 404 Not Found\n\n");
			send(out, buffer, strlen(buffer), 0);
		}*/
	}
	else {
		sprintf(buffer, "HTTP/1.0 501 Method Not Implemented\n\n");
		send(out, buffer, strlen(buffer), 0);
	}
}

char* getValue(char* keyValue) {
	char *valuePtr = strtok(keyValue, "=");
	return strtok(NULL, "=");
}

int main (int argc, char **argv) {
	BlockedServer server;
	server.execute(argv);

	/*DBConnection con;
	pqxx::connection& connection = con.openConnection();

	SqlStatement stm;
	char * sqlstmt = "Select * from command;";
	pqxx::result res = stm.execute(connection, sqlstmt);

	for (pqxx::result::const_iterator row = res.begin(); row != res.end(); ++row)
	{
	  // Fields within a row can be accessed by column name.
	  // You can also iterate the fields in a row, or index the row
	  // by column number just like an array.
	  // Values are stored internally as plain strings.  You access
	  // them by converting each to the desired C++ type using the
	  // "as()" function template.
	  std::cout
		<< row["tag"].as<std::string>() << "\t"
		<< row["cmd"].as<std::string>() << "\t"
		<< row["description"].as<std::string>()
		<< std::endl;
	}*/

	return 0;
}
