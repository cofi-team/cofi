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
#include "../header/protobuf/out/command_java.pb.h"

#define closesocket(s) close(s)

#define HTTP_PORT 8096

#define MAXPUF 1023

using namespace std;

static void serv_request(int in, int out, char* rootpath);

static char* getValue(char* keyValue);

static const list<string> getRequestedCommands(char*, char*);

static const list<string> getRequestedCommands(char* categorie, char* tag) {
	DBConnection con;

	con.openConnection();

	string response;
	SqlStatement stm;
	std::stringstream sqlstmtString;
	sqlstmtString << "Select * from command where category like '%" << categorie << "%' and tag like '%" << tag << "%';";

	//string sqlstmtString = "Select * from command where categorie like '%" + categorie + "%' and tag like '" + tag + "'";
	string s = sqlstmtString.str();

	char * sqlstmt  = new char[s.length()];
	strcpy(sqlstmt, s.c_str());

	pqxx::result res = stm.execute(con.getConnection(), sqlstmt);

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
	}

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

		char* category = NULL;
		char* searchstring = NULL;

		// url format: localhost:8096/category=xxx&search=yyy
		printf("got request: GET %s\n", url);
		char *cofi = strtok(url, "/?&");

		category = strtok(NULL, "?&");
		searchstring = strtok(NULL, "&");

		if(category == NULL || searchstring == NULL) {
			// send http response with clear information for user
			return;
		}

		CommandRequest cmdRequest(getValue(category), getValue(searchstring));

		printf("value of category: %s\n", cmdRequest.getCategory());
		printf("value of searchstring: %s\n", cmdRequest.getCommand());

		list<string> dbEntryList = getRequestedCommands(cmdRequest.getCategory(), cmdRequest.getCommand());

		GOOGLE_PROTOBUF_VERIFY_VERSION;

		tutorial::Command cmd;

		int i = 0;
		for(list<string>::iterator it = dbEntryList.begin(); it != dbEntryList.end(); it++) {
			if(i == 0) cmd.set_tag((*it).data());
			if(i == 1) cmd.set_cmd((*it).data());
			if(i == 2) cmd.set_description((*it).data());
			i++;
		}

		cout << "Command: " << "Tag: " << cmd.tag() << " - Cmd: " << cmd.cmd() << " - Desc.: " << cmd.description() << std::endl;

		//string serializedCommand;
		int byteSize = cmd.ByteSize();
		char byteCmd[byteSize];
		bool serialized = cmd.SerializeToArray(byteCmd, byteSize);
		//string serialized = cmd.SerializeAsString();

		//string newLine = "\n";
		//byteCmd[byteSize-1] = n;

		//bool stringSerialized = cmd.SerializeToString(&serializedCommand);
		//cmd.SerializeToString(&serializedCommand);

		cout << "serialized: " << serialized << std::endl;

		//char *t = "HTTP/1.0 200 OK\nContent-Type: text/html\n";
		//strcat(buffer, t);

		int contentLength = byteSize;

		std::stringstream html;
		//html << "HTTP/1.0 200 OK\nContent-Type: text/html\n" << "Content-Length:" << contentLength << "\n\n"; // << serializedCommand;
		html << "HTTP/1.0 200 OK\nContent-Type: text/html\n"; // << serialized;
		string htmlString = html.str();

		sprintf(buffer, htmlString.c_str());
		send(out, buffer, strlen(buffer), 0);

		sprintf(buffer, byteCmd, byteSize);
		send(out, buffer, strlen(buffer), 0);

		//sprintf(buffer, serialized.data());
		//send(out, buffer, serialized.size(), 0);

		//std::stringstream debugOutput;
		//html << "<html><body>Command: tag = " << cmd.tag() << " cmd = " << cmd.cmd() << " description = " << cmd.description() << "</body></html>";
		//string serializedCommand = html.str();
		//sprintf(buffer, serializedCommand.c_str());

		//send(out, buffer, strlen(buffer), 0);
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
