#pragma once

#ifndef MESSAGE_PROCESS
#define MESSAGE_PROCESS

#define UPDATE_STATUS_ONLINE 0
#define UPDATE_STATUS_OFFLINE 1

#define USERNAME_SIZE 64
struct userCommandChar {
	int CommandCode;
	char CommandName[4];
	char HelpMessage[100];
};

struct userUpdate {
	int userID;
	int status;
};

int parseCmdtoCode(char* message);
int updateUserInfo(char* bufferPtr);
int processIncomingMessage(char* messageReceive, int lenMessage);

#endif
