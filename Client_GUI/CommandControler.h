
struct userCommand {
	int CommandCode;
	wchar_t CommandName[4];
	wchar_t HelpMessage[100];
};

struct packetUserHeader {
	char CommandName[4]; //3 character command name end with '\0'
	int senderID;
	int receiverID;
};

struct userCommand commandSupport[] = {
	{ 0, L"LGI", L"format LGI[space]<username>[space]<password>: login server with username" },
	{ 1, L"LGO", L"LGO: log out" },
	{ 2, L"UDT", L"UDT: get update from server" },
	{ 3, L"IVT", L"IVT[space]<senderid><receiverid>: invite user to conversation" },
	{ 4, L"ACP", L"ACP[space]<senderid><receiverid>: accept invitation from user" },
	{ 5, L"DNY", L"DNY[space]<senderid><receiverid>: deny invitation from user" },
	{ 6, L"MSG", L"MSG[space]<senderid><receiverid><message>: send message to user" },
	{ 7, L"RGT", L"RGT[space]<username>[space]<password>: register username with server" }
};

int processUserInput(HWND hwnd, LPWSTR userInput);
int doCommand(HWND hwnd, LPWSTR userCommand);
int parseCmdtoCode(LPWSTR userCommand);
int sendChatMessage(LPWSTR userInput);