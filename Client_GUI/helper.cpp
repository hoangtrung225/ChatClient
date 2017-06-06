#include "stdafx.h"
#include "UserManager.h"
#include "Client_GUI.h"
struct structClientOnline *getOnlineStructList(int listIndex) {
	for (int i = 0; i < MAX_LIST_CLIENT; i++) {
		if (clientOnlineList[i].listNumber == listIndex)
			return clientOnlineList + i;
	}
	return NULL;
}

struct structClientWaiting *getWaitStructList(int listIndex) {
	for (int i = 0; i < MAX_LIST_CLIENT; i++) {
		if (clientWaitList[i].listNumber == listIndex)
			return clientWaitList + i;
	}
	return NULL;
}

struct tabClientStruct* getTabStruct(int tabIndex) {
	for (int i = 0; i < MAX_CHAT_CLIENT, i++) {
		if (partnerTab[i].tabNumber == tabIndex)
			return partnerTab + i;
	}
	return NULL;
}

int programInitValues(void) {
	hCurrentWindow = NULL;
	memset(partnerTab, -1, sizeof partnerTab);
	memset(clientWaitList, -1, sizeof clientWaitList);
	memset(clientOnlineList, -1, sizeof clientOnlineList);
}

//add an chat tab client struct to list partner tab
int makeChatTabStruct(int clientID, int tabNumber) {
	for (int i = 0; i < MAX_CHAT_CLIENT; i++) {
		if (partnerTab[i].chatClientID == -1) {
			partnerTab[i].chatClientID = clientID;
			partnerTab[i].tabNumber = tabNumber;
			partnerTab[i].hwndDisplay = makeNewChatWindow();

			return 0;
		}
	}
	MessageBox(NULL, L"Error: too many tab opened !", L"Error!", MB_OK);
	return -1;
}

int removeChatTabStruct(int clientID) {
	for (int i = 0; i < MAX_CHAT_CLIENT; i++) {
		if (partnerTab[i].chatClientID == clientID) {
			partnerTab[i].chatClientID = -1;
			partnerTab[i].tabNumber = -1;
			DestroyWindow(partnerTab[i].hwndDisplay);
			partnerTab[i].hwndDisplay = -1;

			return 0;
		}
	}
	MessageBox(NULL, L"Error: cannot remove tab structs!", L"Error!", MB_OK);
	return -1;
}
wchar_t textLabel[64];
int makeWaitListStruct(int clientID, int state) {
	for (int i = 0; i < MAX_LIST_CLIENT; i++) {
		if (clientWaitList[i].chatClientID == -1) {
			if (state == STATE_WAIT_USER)
				wprintf_s(textLabel, L"%d[waiting confirmation]", clientID);
			else if (state == STATE_WAIT_MESSAGE)
				wprintf_s(textLabel, L"%d[waiting response]", clientID);
			else
				return -1;

			clientWaitList[i].chatClientID = clientID;
			clientWaitList[i].state = state;
			clientWaitList[i].listNumber = SendMessageW(hWaitList, TCM_GETITEMCOUNT, 0, 0);
			SendMessageW(hWaitList, LB_INSERTSTRING, clientWaitList[i].listNumber, (LPARAM)textLabel);
			return 0;
		}
	}
	MessageBox(NULL, L"Error: too many waiting list items!", L"Error!", MB_OK);
	return -1;
}

int removeWaitListStruct(int clientID) {
	for (int i = 0; i < MAX_LIST_CLIENT; i++) {
		if (clientWaitList[i].chatClientID == clientID) {
			clientWaitList[i].chatClientID = -1;
			clientWaitList[i].state = -1;
			SendMessageW(hWaitList, LB_DELETESTRING, clientWaitList[i].listNumber, 0);
			clientWaitList[i].listNumber = -1;

			return 0;
		}
	}
	MessageBox(NULL, L"Error: cannot remove tab structs!", L"Error!", MB_OK);
	return -1;
}

int makeOnlineListStruct(int clientID) {
	for (int i = 0; i < MAX_LIST_CLIENT; i++) {
		if (clientOnlineList[i].chatClientID == -1) {

			clientOnlineList[i].chatClientID = clientID;
			clientOnlineList[i].listNumber = SendMessageW(hUserList, TCM_GETITEMCOUNT, 0, 0);
			wprintf_s(textLabel, L"%d[Online]", clientID);
			SendMessageW(hUserList, LB_INSERTSTRING, clientWaitList[i].listNumber, (LPARAM)textLabel);
			return 0;
		}
	}
	MessageBox(NULL, L"Error: too many online list items!", L"Error!", MB_OK);
	return -1;
}

int removeOnlineListStruct(int clientID) {
	for (int i = 0; i < MAX_LIST_CLIENT; i++) {
		if (clientOnlineList[i].chatClientID == clientID) {
			clientOnlineList[i].chatClientID = -1;
			SendMessageW(hUserList, LB_DELETESTRING, clientOnlineList[i].listNumber, 0);
			clientOnlineList[i].listNumber = -1;

			return 0;
		}
	}
	MessageBox(NULL, L"Error: cannot remove tab structs!", L"Error!", MB_OK);
	return -1;
}