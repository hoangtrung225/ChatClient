#pragma once

#define UPDATE_STATUS_ONLINE 0
#define UPDATE_STATUS_OFFLINE 1

#define USERNAME_SIZE 64
struct userCommand {
	int CommandCode;
	char CommandName[4];
	char HelpMessage[100];
};
struct userCommand commandSupport[] = {
	{ 0, "LGI", "format LGI[null space]<username>[null space]<password>: login server with username" },
	{ 1, "LGO", "LGO: log out" },
	{ 2, "UDT", "UDT[null space]<userid><status><username>[null space]... get update from server" },
	{ 3, "IVT", "IVT[null space]<senderid><receiverid>: invite user to conversation" },
	{ 4, "ACP", "ACP[null space]<senderid><receiverid>: accept invitation from user" },
	{ 5, "DNY", "DNY[null space]<senderid><receiverid>: deny invitation from user" },
	{ 6, "MSG", "MSG[null space]<senderid><receiverid><message>: send message to user" },
	{ 7, "RGT", "RGT[null space]<username>[null space]<password>: register username with server" }
};

struct userUpdate {
	int userID;
	int status;
};
