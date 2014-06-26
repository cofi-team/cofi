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

#include "server.h"

#define closesocket(s) close(s)

#define HTTP_PORT 8096

#define MAXPUF 1023	

using namespace std;

static void serv_request(int in, int out, char* rootpath);



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
		printf("got request: GET %s\n", url);
		sprintf(path, "%s/%s", rootpath, url);
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
		}
	}
	else {
		sprintf(buffer, "HTTP/1.0 501 Method Not Implemented\n\n");
		send(out, buffer, strlen(buffer), 0);
	}
}

int main (int argc, char **argv) {
	BlockedServer server;
	server.execute(argv);
	return 0;
}
