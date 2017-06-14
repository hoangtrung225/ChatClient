#pragma once
#ifndef COMMAND_CONTROL
#define COMMAND_CONTROL

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

int processUserInput(HWND hwnd, LPWSTR userInput);
int doCommand(HWND hwnd, LPWSTR userCommand);
int parseCmdtoCode(LPWSTR userCommand);
int sendChatMessage(LPWSTR userInput);

#endif