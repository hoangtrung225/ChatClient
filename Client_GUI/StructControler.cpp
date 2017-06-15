#include "stdafx.h"
#include "UserManager.h"
#include "Client_GUI.h"
#include "StructControler.h"

wchar_t textLabel[64];

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
	for (int i = 0; i < MAX_CHAT_CLIENT; i++) {
		if (partnerTab[i].tabNumber == tabIndex)
			return partnerTab + i;
	}
	return NULL;
}

int programInitValues(void) {
	hCurrentWindow = NULL;
	memset(partnerTab, -1, MAX_CHAT_CLIENT * sizeof(tabClientStruct));
	memset(clientWaitList, -1,  MAX_LIST_CLIENT * sizeof(structClientWaiting));
	memset(clientOnlineList, -1,  MAX_LIST_CLIENT * sizeof(structClientOnline));

	return 0;
}

//add an chat tab client struct to list partner tab
int makeChatTabStruct(int clientID) {
	for (int i = 0; i < MAX_CHAT_CLIENT; i++) {
		if (partnerTab[i].chatClientID == -1) {
			partnerTab[i].chatClientID = clientID;
			partnerTab[i].tabNumber = SendMessageW(hTab, TCM_GETITEMCOUNT, 0, 0);

			TCITEMW tie;
			tie.mask = TCIF_TEXT;
			swprintf_s(textLabel, L"%d[partner]", clientID);
			tie.pszText = textLabel;

			SendMessageW(hTab, TCM_INSERTITEMW, partnerTab[i].tabNumber, (LPARAM)(LPTCITEM)&tie);

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
			//delete tab item of struct offset other tab struct 
			for (int j = 0; j < MAX_CHAT_CLIENT; j++)
				if (partnerTab[j].tabNumber > partnerTab[i].tabNumber)
					partnerTab[j].tabNumber--;
			SendMessageW(hTab, TCM_DELETEITEM, partnerTab[i].tabNumber, 0);
			partnerTab[i].tabNumber = -1;
			DestroyWindow(partnerTab[i].hwndDisplay);
			partnerTab[i].hwndDisplay = NULL;

			return 0;
		}
	}
	MessageBox(NULL, L"Error: tab structs item not found!", L"Error!", MB_OK);
	return -1;
}

int makeWaitListStruct(int clientID, int state) {
	for (int i = 0; i < MAX_LIST_CLIENT; i++) {
		if (clientWaitList[i].chatClientID == -1) {
			if (state == STATE_WAIT_USER)
				swprintf_s(textLabel, L"%d[waiting confirmation]", clientID);
			else if (state == STATE_WAIT_MESSAGE)
				swprintf_s(textLabel, L"%d[waiting response]", clientID);
			else
				return -1;

			clientWaitList[i].chatClientID = clientID;
			clientWaitList[i].state = state;
			clientWaitList[i].listNumber = SendMessageW(hWaitList, LB_GETCOUNT, 0, 0);
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
			//delete wait list item and offset other wait list struct
			for (int j = 0; j < MAX_LIST_CLIENT; j++)
				if (clientWaitList[i].listNumber < clientWaitList[j].listNumber)
					clientWaitList[j].listNumber--;
			SendMessageW(hWaitList, LB_DELETESTRING, clientWaitList[i].listNumber, 0);
			clientWaitList[i].listNumber = -1;

			return 0;
		}
	}
	MessageBox(NULL, L"Error: wait list item not found!", L"Error!", MB_OK);
	return -1;
}

int makeOnlineListStruct(int clientID) {
	for (int i = 0; i < MAX_LIST_CLIENT; i++) {
		if (clientOnlineList[i].chatClientID == -1) {
			clientOnlineList[i].chatClientID = clientID;
			clientOnlineList[i].listNumber = SendMessageW(hUserList, LB_GETCOUNT, 0, 0);
			swprintf_s(textLabel, L"%d[Online]", clientID);
			SendMessageW(hUserList, LB_INSERTSTRING, clientOnlineList[i].listNumber, (LPARAM)textLabel);

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
			//delete wait list item and offset other wait list struct
			for (int j = 0; j < MAX_LIST_CLIENT; j++)
				if (clientOnlineList[i].listNumber < clientOnlineList[j].listNumber)
					clientOnlineList[j].listNumber--;
			SendMessageW(hUserList, LB_DELETESTRING, clientOnlineList[i].listNumber, 0);
			clientOnlineList[i].listNumber = -1;

			return 0;
		}
	}
	MessageBox(NULL, L"Error: online list item not found!", L"Error!", MB_OK);
	return -1;
}