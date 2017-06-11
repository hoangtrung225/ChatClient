#include "stdafx.h"
#include <Shlwapi.h>
#include "UserManager.h"
#include "StructControler.h"
#include "CommandControler.h"
#include "wchar.h"

#define _CRT_SECURE_NO_WARNINGS

int processUserInput(HWND hwnd, LPWSTR userInput) {
	if (userInput[0] == L':')
		doCommand(hwnd, userInput + 1);
	else
		sendChatMessage(userInput);
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
				return;
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

			//remove from waiting list, push back into online list
			if (removeWaitListStruct(selectedStruct->chatClientID) < 0)
				return -1;
			makeOnlineListStruct(selectedStruct->chatClientID);
		}
			break;
		case 6: 
			sendChatMessage(userCommand + 1 + 4);
			break;
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
	wprintf_s(chatBuffer, L"Me: %s", userInput);

	int index = GetWindowTextLength(hEdit);
	SetFocus(hEdit); // set focus
	SendMessage(hEdit, EM_SETSEL, (WPARAM)index, (LPARAM)index); // set selection - end of text
	SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)chatBuffer); // append!
}