class CommandRequest 
{
	public:
		CommandRequest(char* category, char* command);
		CommandRequest(CommandRequest &cmdReq);
	
		char *getCategory();
		char *getCommand();
		
	private:
		char *category;
		char *command;
};
