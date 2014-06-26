#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <unistd.h>
#include <cstdlib>

#include "../header/commandRequest.h"


using namespace std;

CommandRequest::CommandRequest(CommandRequest &cmdRequ) {
	this->category = cmdRequ.category;
	this->command = cmdRequ.command;
}

CommandRequest::CommandRequest(char *category, char *command) {
	this->category = category;
	this->command = command;
}

char* CommandRequest::getCategory() {
	return category;
}

char* CommandRequest::getCommand() {
	return command;
}
