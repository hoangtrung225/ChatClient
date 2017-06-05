#include "stdafx.h"
#include "UserManager.h"

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