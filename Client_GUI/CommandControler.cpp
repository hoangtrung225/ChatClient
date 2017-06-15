#include "stdafx.h"
#include <Shlwapi.h>
#include "UserManager.h"
#include "StructControler.h"
#include "CommandControler.h"
#include "Client_GUI.h"
#include "wchar.h"

struct userCommand commandSupport[] = {
	{ 0, L"LGI", L"format LGI[space]<username>[space]<password>: login server with username" },
	{ 1, L"LGO", L"LGO: log out" },
	{ 2, L"UDT", L"UDT: get update from server" },
	{ 3, L"IVT", L"IVT[space]<senderid><receiverid>: invite user to conversation" },
	{ 4, L"ACP", L"ACP[space]<senderid><receiverid>: accept invitation from user" },
	{ 5, L"DNY", L"DNY[space]<senderid><receiverid>: deny invitation from user" },
	{ 6, L"MSG", L"MSG[space]<senderid><receiverid><message>: send message to user" },
	{ 7, L"RGT", L"RGT[space]<username>[space]<password>: register username with server" },
	{ 8, L"TER", L"TER[space]<senderid><receiverid>: terminate conversation"},
	{ 100, L"TST", L"TST[space]<number code>: add 5 online user for testing"},
};

int processUserInput(HWND hwnd, LPWSTR userInput) {
	if (userInput[0] == L':')
		doCommand(hwnd, userInput + 1);
	else
		sendChatMessage(userInput);
	return 0;
}

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
			headerPacket.CommandName[3] = '\0';
			
			headerPacket.senderID = thisUserId;
			if (thisUserId == -1) {
				MessageBox(NULL, L"Login to send Invite message", L"Error!", MB_OK);
				return -1;
			}

			int listCurrentSelected = (int)SendMessageW(hUserList, LB_GETCURSEL, 0, 0);
			if (listCurrentSelected == LB_ERR) {
				MessageBox(NULL, L"Select an user to send Chat request", L"Error!", MB_OK);
				return -1;
			}
			struct structClientOnline* selectedStruct = getOnlineStructList(listCurrentSelected);
			headerPacket.receiverID = selectedStruct->chatClientID;

			//check if user is in online list
			if (addWaiting(selectedStruct->chatClientID) < 0)
				return -1;

			memcpy(sendSocketBuffer, &headerPacket, sizeof headerPacket);
			if (send(client, sendSocketBuffer, sizeof headerPacket, 0) < 0) {
				MessageBox(NULL, L"Error: connection lost", L"Error!", MB_OK);
				return -1;
			}

			//add user into wait list
			if (makeWaitListStruct(selectedStruct->chatClientID, STATE_WAIT_MESSAGE) < 0)
				return -1;

			removeOnlineListStruct(selectedStruct->chatClientID);
		}
			break;

		//send Accept
		case 4: {
			struct packetUserHeader headerPacket;
			strncpy(headerPacket.CommandName, "ACP", 3);
			headerPacket.CommandName[3] = '\0';

			headerPacket.senderID = thisUserId;
			if (thisUserId == -1) {
				MessageBox(NULL, L"Login to send Accept message", L"Error!", MB_OK);
				return -1;
			}

			int listCurrentSelected = (int)SendMessageW(hWaitList, LB_GETCURSEL, 0, 0);
			if (listCurrentSelected == LB_ERR) {
				MessageBox(NULL, L"Select an user to send Accept message", L"Error!", MB_OK);
				return -1;
			}
			struct structClientWaiting* selectedStruct = getWaitStructList(listCurrentSelected);
			headerPacket.receiverID = selectedStruct->chatClientID;

			//send accept if user is waiting respond else return error
			if (selectedStruct->state != STATE_WAIT_USER) {
				MessageBox(NULL, L"Error: user accept have to in waiting queue", L"Error!", MB_OK);
				return -1;
			}

			//check if user is in waiting list
			if (addChating(selectedStruct->chatClientID) < 0)
				return -1;

			memcpy(sendSocketBuffer, &headerPacket, sizeof headerPacket);
			if (send(client, sendSocketBuffer, sizeof headerPacket, 0) < 0) {
				MessageBox(NULL, L"Error: connection lost", L"Error!", MB_OK);
				return -1;
			}

			//add user to chat tab list, remove from waiting list
			if (makeChatTabStruct(selectedStruct->chatClientID) < 0)
				return -1;
			removeWaitListStruct(selectedStruct->chatClientID);
		}
			break;

		//send DENY message
		case 5: {
			struct packetUserHeader headerPacket;
			strncpy(headerPacket.CommandName, "DNY", 3);
			headerPacket.CommandName[3] = '\0';

			headerPacket.senderID = thisUserId;
			if (thisUserId == -1) {
				MessageBox(NULL, L"Login to send Deny message", L"Error!", MB_OK);
				return -1;
			}

			int listCurrentSelected = (int)SendMessageW(hWaitList, LB_GETCURSEL, 0, 0);
			if (listCurrentSelected == LB_ERR) {
				MessageBox(NULL, L"Select an user to send Deny message", L"Error!", MB_OK);
				return -1;
			}
			struct structClientWaiting* selectedStruct = getWaitStructList(listCurrentSelected);
			headerPacket.receiverID = selectedStruct->chatClientID;

			//send accept if user is waiting respond else return error
			if (selectedStruct->state != STATE_WAIT_USER) {
				MessageBox(NULL, L"Error: user deny have to in waiting queue", L"Error!", MB_OK);
				return -1;
			}

			//check if user is in waiting list
			if (refuseRequest(selectedStruct->chatClientID) < 0)
				return -1;

			memcpy(sendSocketBuffer, &headerPacket, sizeof headerPacket);
			if (send(client, sendSocketBuffer, sizeof headerPacket, 0) < 0) {
				MessageBox(NULL, L"Error: connection lost", L"Error!", MB_OK);
				return -1;
			}
			if (makeOnlineListStruct(selectedStruct->chatClientID) < 0)
				return -1;
			//remove from waiting list, push back into online list
			removeWaitListStruct(selectedStruct->chatClientID);
				
		}
			break;
		case 6: 
			sendChatMessage(userCommand + 4);
			break;
		//regist with server
		case 7: {
			strncpy(sendSocketBuffer, "RGT", 3);
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
		//send TERMINATE message
		case 8: {
			struct packetUserHeader headerPacket;
			strncpy(headerPacket.CommandName, "TER", 3);
			headerPacket.CommandName[3] = '\0';

			headerPacket.senderID = thisUserId;
			if (thisUserId == -1) {
				MessageBox(NULL, L"Login to send Deny message", L"Error!", MB_OK);
				return -1;
			}

			int tabCurrentSelect = (int)SendMessageW(hTab, TCM_GETCURSEL, 0, 0);
			if (tabCurrentSelect == LB_ERR) {
				MessageBox(NULL, L"Select an chating user to terminate conversation", L"Error!", MB_OK);
				return -1;
			}
			struct tabClientStruct* selectedStruct = getTabStruct(tabCurrentSelect);
			headerPacket.receiverID = selectedStruct->chatClientID;

			//check if user is in waiting list
			if (endConversation(selectedStruct->chatClientID) < 0)
				return -1;

			memcpy(sendSocketBuffer, &headerPacket, sizeof headerPacket);
			if (send(client, sendSocketBuffer, sizeof headerPacket, 0) < 0) {
				MessageBox(NULL, L"Error: connection lost", L"Error!", MB_OK);
				return -1;
			}
			if (makeOnlineListStruct(selectedStruct->chatClientID) < 0)
				return -1;

			//remove from waiting list, push back into online list
			removeChatTabStruct(selectedStruct->chatClientID);
		}
			break;
		case 100: {
			if (userCommand[4] == L'0') {
				for (int i = 0; i < 5; i++) {
					addUser(i);
					makeOnlineListStruct(i);
				}
			}
			if (userCommand[4] == L'1') {
				for (int i = 100; i < 105; i++) {
					addUser(i);
					addWaiting(i);
					addChating(i);
					makeChatTabStruct(i);
				}
			}
			if (userCommand[4] == L'2') {
				for (int i = 200; i < 205; i++) {
				addUser(i);
				addWaiting(i);
				makeWaitListStruct(i, STATE_WAIT_USER);
				}
			}
		}
			break;
		default:
			break;
	}
	return 0;
}

int parseCmdtoCode(LPWSTR userCommand) {
	for (int i = 0; i < sizeof commandSupport / sizeof( struct userCommand ); i++) {
		if (wcsncmp(commandSupport[i].CommandName, userCommand, 3) == 0)
			return commandSupport[i].CommandCode;
	}
	return -1;
}

int sendChatMessage(LPWSTR userInput) {
	struct packetUserHeader headerPacket;
	strncpy(headerPacket.CommandName, "MSG", 3);
	headerPacket.CommandName[3] = '\0';

	headerPacket.senderID = thisUserId;
	if (thisUserId == -1) {
		MessageBox(NULL, L"Login to send Chat message", L"Error!", MB_OK);
		return -1;
	}

	int tabCurrentSelected = (int)SendMessageW(hTab, TCM_GETCURSEL, 0, 0);
	if (tabCurrentSelected == LB_ERR) {
		MessageBox(NULL, L"Select an user to send Chat message", L"Error!", MB_OK);
		return -1;
	}
	struct tabClientStruct* selectedStruct = getTabStruct(tabCurrentSelected);
	headerPacket.receiverID = selectedStruct->chatClientID;

	//copy send header and message to buffer
	memcpy(sendSocketBuffer, &headerPacket, sizeof headerPacket);
	int messageLen = wcslen(userInput);
	wcstombs(sendSocketBuffer + sizeof headerPacket, userInput, messageLen);
	if (send(client, sendSocketBuffer, sizeof headerPacket + messageLen, 0) < 0) {
		MessageBox(NULL, L"Error: connection lost", L"Error!", MB_OK);
		return -1;
	}

	//add message to Chat window 
	wsprintf(chatBuffer, L"Me: %s", userInput);

	SendMessage(hCurrentWindow, LB_ADDSTRING, 0, (LPARAM)chatBuffer);
	ShowWindow(hCurrentWindow, SW_SHOW);

	return 0;
}