#include "stdafx.h"
#include "MessageProcess.h"
#include "UserManager.h"
#include "StructControler.h"
#include "CommandControler.h"
#include "Client_GUI.h"

struct userCommandChar commandSupportChar[] = {
	{ 0, "LGI", "format LGI[null space]<username>[null space]<password>: login server with username" },
	{ 1, "LGO", "LGO: log out" },
	{ 2, "UDT", "UDT[null space]<userid><status><username>[null space]... get update from server" },
	{ 3, "IVT", "IVT[null space]<senderid><receiverid>: invite user to conversation" },
	{ 4, "ACP", "ACP[null space]<senderid><receiverid>: accept invitation from user" },
	{ 5, "DNY", "DNY[null space]<senderid><receiverid>: deny invitation from user" },
	{ 6, "MSG", "MSG[null space]<senderid><receiverid><message>: send message to user" },
	{ 7, "RGT", "RGT[null space]<username>[null space]<password>: register username with server" }
};

int processIncomingMessage(char* messageReceive, int lenMessage) {
	int messagetoProcess = 0;
	char* tmpPointer = NULL;
	int senderID;
	switch (parseCmdtoCode(messageReceive))
	{
	//update message user from server
	case 2:
		tmpPointer = messageReceive + 4;
		messagetoProcess = lenMessage - 4;
		while (messagetoProcess > 0) {
			int processed = updateUserInfo(tmpPointer);
			if (messagetoProcess <= 0)
				break;
			messagetoProcess = messagetoProcess - processed;
			tmpPointer = tmpPointer + processed;
		}
		break;
	//ivitation come receive from user
	case 3:
		senderID = ((struct packetUserHeader*)messageReceive)->senderID;
		//add user to waiting list for user confirmation
		if (addWaiting(senderID) < 0)
			return -1;
		removeOnlineListStruct(senderID);
		makeWaitListStruct(senderID, STATE_WAIT_USER);
		break;

	//accept conversation coming from user
	case 4:
		senderID = ((struct packetUserHeader*)messageReceive)->senderID;
		//confirm if this user in waiting confirm state
		for (int i = 0; i < MAX_LIST_CLIENT; i++) {
			if (clientWaitList[i].chatClientID == senderID && clientWaitList[i].state == STATE_WAIT_MESSAGE) {
				//add to Chating group
				if (addChating(senderID) < 0)
					return -1;
				removeWaitListStruct(senderID);
				makeChatTabStruct(senderID);
			}
		}
		break;

	//user deny chat request return user to online list
	case 5:
		senderID = ((struct packetUserHeader*)messageReceive)->senderID;
		//confirm if this user in waiting mesaage state
		for (int i = 0; i < MAX_LIST_CLIENT; i++) {
			if (clientWaitList[i].chatClientID == senderID && clientWaitList[i].state == STATE_WAIT_MESSAGE) {
				//return  to online group
				if (refuseRequest(senderID) < 0)
					return -1;
				removeWaitListStruct(senderID);
				makeOnlineListStruct(senderID);
			}
		}
		break;

	//received message from user
	case 6:
		senderID = ((struct packetUserHeader*)messageReceive)->senderID;
		for (int i = 0; i < MAX_CHAT_CLIENT; i++) {
			if (partnerTab[i].chatClientID == senderID) {
				//return  to online group
				wsprintf(chatBuffer, L"User<%d>: %s", senderID, messageReceive + sizeof(struct packetUserHeader) );
				SendMessage(partnerTab[i].hwndDisplay, LB_ADDSTRING, 0, (LPARAM)chatBuffer);
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

int parseCmdtoCode(char* message) {
	for (int i = 0; i < sizeof commandSupportChar / sizeof(struct userCommandChar); i++) {
		if (strncmp(commandSupportChar[i].CommandName, message, 3) == 0)
			return commandSupportChar[i].CommandCode;
	}
	return -1;
}

char username[USERNAME_SIZE];
//process struct userupdate start at bufferPtr return lenght of processed string
int updateUserInfo(char* bufferPtr) {
	int ret = 0;
	int stateUser = 0;
	int userID = ((struct userUpdate*) bufferPtr)->userID;
	int userStatus = ((struct userUpdate*) bufferPtr)->status;
	int usernameLen = strlen((char*)bufferPtr + sizeof(struct userUpdate));
	if (usernameLen > USERNAME_SIZE) {
		ret = sizeof(struct userUpdate) + usernameLen;
		return ret;
	}

	strncpy_s(username, (char*)bufferPtr + sizeof(struct userUpdate), USERNAME_SIZE);
	if (userStatus == UPDATE_STATUS_ONLINE) {
		if (addUser(userID) < 0)
			return -1;
		if (makeOnlineListStruct(userID) < 0)
			return -1;
	}
	else if (userStatus == UPDATE_STATUS_OFFLINE) {
		if ((stateUser = removeUser(userID)) < 0)
			return ret;
		switch (getUserStatus(userID)) {
		case STATE_USER_ONLINE:
			removeOnlineListStruct(userID);
			break;
		case STATE_USER_WAITING:
			removeWaitListStruct(userID);
			break;
		case STATE_USER_CHATING:
			removeChatTabStruct(userID);
			break;
		default:
			MessageBox(NULL, L"Error user struct not found", L"Error!", MB_OK);
			return -1;
		}
		return sizeof(struct userUpdate);
	}
	return ret;
}