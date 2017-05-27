#include "stdafx.h"
#include <Shlwapi.h>

struct userCommand {
	int CommandCode;
	wchar_t CommandName[4];
	wchar_t HelpMessage[64];
};

struct packetUserHeader {
	char CommandName[4]; //3 character command name end with '\0'
	int senderID;
	int receiverID;
};

struct userCommand commandSupport[] = {
	{0, L"LGI", L"LGI [username] [password]: login server with username"},
	{1, L"LGO", L"LGO: log out"},
	{2, L"UDT", L"UDT: get update from server"},
	{3, L"IVT", L"IVT [username]: invite user to conversation"},
	{4, L"ACP", L"ACP [username]: accept invitation from user"},
	{5, L"DNY", L"DNY [username]: deny invitation from user"},
	{6, L"MSG", L"MSG [username] [message]:send message to user"},
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
	switch (parseCmdtoCode(userCommand))
	{
		//case 0 login
		case 0: {
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
		break;

		//case 1 logout send message logout to server
		case 1: {
			strncpy(sendSocketBuffer, "LGO", 3);
			sendSocketBuffer[3] = '\0';
			send(client, sendSocketBuffer, 4, 0);
			//remove client chat tab from hTab
			//remove waiting list from hList
		}
		break;
		//case 2 update
		case 2: {
			strncpy(sendSocketBuffer, "UDT", 3);
			sendSocketBuffer[3] = '\0';
			send(client, sendSocketBuffer, 4, 0);
		}
		break;

		//case 3 invite
		case 3:{
			struct packetUserHeader headerPacket;
			strncpy(headerPacket.CommandName, "IVT", 3);
			headerPacket.senderID = thisUserId;

			int listCurrentSelected = (int)SendMessageW(hList, LB_GETCURSEL, 0, 0);
			if (listCurrentSelected == LB_ERR) {
				MessageBox(NULL, L"Select an user to send chat request", L"Error!", MB_OK);
			}
			headerPacket.receiverID = getStructfromList(listCurrentSelected)->chatClientID;

		}
		default:
			break;
	}
}

int parseCmdtoCode(LPWSTR userCommand) {
	for (int i = 0; i < sizeof commandSupport / sizeof( struct userCommand ); i++) {
		if (StrCmpN(commandSupport[i].CommandName, userCommand, 3) == 0)
			return commandSupport[i].CommandCode;
	}
	return -1;
}

struct listClientStruct *getStructfromList(int listIndex) {
	for (int i = 0; i < listNumber; i++) {
		if (clientWaitList[i].listNumber == listIndex)
			return clientWaitList + i;
	}
	return NULL;
}