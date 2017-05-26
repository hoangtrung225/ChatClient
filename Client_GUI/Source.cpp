#include "stdafx.h"
#include <Shlwapi.h>

struct userCommand {
	int CommandCode;
	wchar_t CommandName[4];
	wchar_t HelpMessage[64];
};

struct userCommand commandSupport[] = {
	{0, L"LGI", L"LGI [username] [password]: login server with username"},
	{1, L"LGO", L"LGO: log out"},
	{2, L"UDT", L"UDT: get update from server"},
	{3, L"IVT", L"IVT [username]: invite user to conversation"},
	{4, L"ACP", L"ACP [username]: accept invitation from user"},
	{5, L"DNY", L"DNY [username]: deny invitation from user"},
	{6, L"MSG", L"MSG [username]: [message]:send message to user"},
	{7, L"RGT", L"RGT [username] [password]: register username with server"}
};
int processUserInput(HWND hwnd, LPWSTR userInput) {
	if (userInput[0] == L':')
		doCommand(hwnd, userInput + 1);
	else
		sendChatMessage(userInput);
}

char sendSocketBuffer[512];
char recvSocketBuffer[512];
int doCommand(HWND hwnd, LPWSTR userCommand) {
	for (int i = 0; i < sizeof commandSupport / sizeof(struct userCommand); i++) {
		//send message logout to server
		if (StrCmpN(userCommand, L"LGO", 3) == 0) {
			strncpy(sendSocketBuffer, "LGO", 3);
			sendSocketBuffer[3] = '\0';
			send(client, sendSocketBuffer, 4, 0);
			//remove client chat tab from hTab
			//remove waiting list from hList
		}

		if (StrCmpN(userCommand, L"LGI", 3) == 0) {
			strncpy(sendSocketBuffer, "LGI", 3);
			sendSocketBuffer[3] = '\0';
			//get username length
			int usernameLen = wcslen(userCommand + 4);
			wcstombs(sendSocketBuffer + 4, userCommand + 4, usernameLen);
			sendSocketBuffer[4 + usernameLen] = '\0';

			//get user password
			int passwordLen = wcslen(userCommand + 4 + usernameLen + 1);
			wcstombs(sendSocketBuffer + 4 + usernameLen + 1, userCommand + 4 + usernameLen + 1, passwordLen);
			sendSocketBuffer[4 + usernameLen + 1 + passwordLen] = '\0';

			send(client, sendSocketBuffer, 4 + usernameLen + 1 + passwordLen + 1, 0);
		}

		if (StrCmpN(userCommand, L"UDT", 3) == 0) {
			strncpy(sendSocketBuffer, "UDT", 3);
			sendSocketBuffer[3] = '\0';
			send(client, sendSocketBuffer, 4, 0);
		}
	}
}