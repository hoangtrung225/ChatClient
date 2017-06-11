#include "stdafx.h"
#include "MessageProcess.h"
#include "UserManager.h"
#include "StructControler.h"

int processIncomingMessage(char* messageReceive) {
	switch (parseCmdtoCode(messageReceive))
	{
	//update message user from server
	case 2:
	default:
		break;
	}
}

int parseCmdtoCode(char* message) {
	for (int i = 0; i < sizeof commandSupport / sizeof(struct userCommand); i++) {
		if (strncmp(commandSupport[i].CommandName, message, 3) == 0)
			return commandSupport[i].CommandCode;
	}
	return -1;
}

char username[USERNAME_SIZE];
char* updateUserInfo(void* bufferPtr) {
	char* ret = NULL;
	int stateUser = 0;
	int userID = ((struct userUpdate*) bufferPtr)->userID;
	int userStatus = ((struct userUpdate*) bufferPtr)->status;
	int usernameLen = strlen((char*)bufferPtr + sizeof(struct userUpdate));
	if (usernameLen > USERNAME_SIZE) {
		ret = (char*)bufferPtr + sizeof(struct userUpdate) + usernameLen;
		return ret;
	}
	strncpy(username, (char*)bufferPtr + sizeof(struct userUpdate), USERNAME_SIZE);
	if (userStatus == UPDATE_STATUS_ONLINE) {
		if (addUser(userID) < 0)
			return ret;
		if (makeOnlineListStruct(userID) < 0)
			return ret;
	}
	else if (userStatus == UPDATE_STATUS_OFFLINE) {
		if ((stateUser = removeUser(userID)) < 0)
			return ret;
		
	}
}