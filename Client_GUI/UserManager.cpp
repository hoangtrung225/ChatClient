#include <stdlib.h>
#include "stdafx.h"
#include "UserManager.h"
#include "Client_GUI.h"
#define LISTSIZE 256
int* userOnline, *userWaiting, *userChatting, *listEndPTR;
void* mutex;

void Unlock();
void Lock();

void userManagerInit(void) {
	//when initual there is no user online, waiting or chating
	userWaiting = userChatting = listEndPTR
		= userOnline = (int*)malloc(LISTSIZE * sizeof(int));
	//list user end with -1
	listEndPTR[0] = -1;

	//create mutex
	mutex = CreateMutex(
		NULL,              // default security attributes
		false,             // initially not owned
		L"LockTest");      // named mutex

	if (mutex == NULL)
	{
		MessageBox(NULL, L"Error creating mutex handle", L"Error!", MB_OK);
		return;
	}
}

int addUser(int userID) {
	//lock
	Lock();
	int numberUser = listEndPTR - userOnline;
	
	if (numberUser > 0 && (numberUser % 256 == 0)) {
		int* newAlloc = (int*)realloc(userOnline, numberUser + LISTSIZE);
		if (newAlloc == NULL)
			return -1;
		if (newAlloc != userOnline) {
			int offset = newAlloc - userOnline;
			userWaiting = userWaiting + offset;
			userChatting = userChatting + offset;
			listEndPTR = listEndPTR + offset;
		}
	}
	//swap first chatting elem to tail increase list user size by 1
	listEndPTR[0] = userChatting[0];
	listEndPTR = listEndPTR + 1;
	listEndPTR[0] = -1;

	//swap first waiting elem over write first chatting elem move userChatting PTR
	userChatting[0] = userWaiting[0];

	//over write first waiting elem with new online elem
	userWaiting[0] = userID;

	//increase userChatting PTR
	userChatting = userChatting + 1;
	Unlock();
	return 0;
}

//return 0 on success found online user and move it to wating  list
//return -1 if not found
int addWaiting(int idUserOnline) {
	//search userOnline for match userID
	for (int* tmpPtr = userOnline; tmpPtr < userWaiting; tmpPtr++) {
		if (tmpPtr[0] == idUserOnline) {
			Lock();
			//swap match value with last elem of user online list
			tmpPtr[0] = userWaiting[-1];

			//decrease size of userOnline list
			userWaiting = userWaiting - 1;

			//add idUserOnline to Waiting list
			userWaiting[0] = idUserOnline;
			Unlock();
			return 0;
		}
	}
	return -1;
}

//add user from waiting to chatting list
int addChating(int idUserWaiting) {
	//search userWaiting for match userID
	for (int* tmpPtr = userWaiting; tmpPtr < userChatting; tmpPtr++) {
		if (tmpPtr[0] == idUserWaiting) {
			Lock();
			//swap match value with last elem of user waiting list
			tmpPtr[0] = userChatting[-1];

			//decrease size of userWait list
			userChatting = userChatting - 1;

			//add idUserWaiting to Chating list
			userChatting[0] = idUserWaiting;
			Unlock();
			return 0;
		}
	}
	return -1;
}

//remove Chatting userID from chating list and push back to online user
int endConversation(int idUserChatting) {
	int* tmpPtr;
	for (tmpPtr = userChatting; tmpPtr < listEndPTR; tmpPtr++) {
		if (tmpPtr[0] == idUserChatting) {
			Lock();
			//swap found id with first elem of Chating list
			tmpPtr[0] = userChatting[0];
			userChatting[0] = idUserChatting;
			
			//decrease size of chating list
			userChatting = userChatting + 1;

			//swap last elem of waiting list first first elem
			userChatting[-1] = userWaiting[0];
			userWaiting[0] = idUserChatting;

			//decrease size of waiting list
			userWaiting = userWaiting + 1;
			Unlock();
			return 0;
		}
	}
	return -1;
}

//push an waiting user back to online user list
int refuseRequest(int waitingRequest) {
	for (int* tmpPtr = userWaiting; tmpPtr < userChatting; tmpPtr++) {
		if (tmpPtr[0] == waitingRequest) {
			Lock();
			tmpPtr[0] = userWaiting[0];
			userWaiting[0] = waitingRequest;

			//decrease size of waiting list
			userWaiting = userWaiting + 1;
			Unlock();
			return 0;
		}
	}
	// not found user
	return -1;
}

int removeUser(int usertoRemove) {
	for (int* tmpPtr = userOnline; tmpPtr < listEndPTR; tmpPtr++)
		//found user to remove
		if (tmpPtr[0] == usertoRemove) {
			Lock();
			// remove an online user
			if (tmpPtr < userWaiting) {
				//push user to chatting list
				addWaiting(usertoRemove);
				addChating(usertoRemove);

				//push user to romove to last of chat list
				userChatting[0] = listEndPTR[-1];

				listEndPTR = listEndPTR - 1;
				listEndPTR[0] = -1;
			}
			else if (userWaiting < tmpPtr && tmpPtr < userChatting) {
				addChating(usertoRemove);

				//push user to romove to last of chat list
				userChatting[0] = listEndPTR[-1];

				listEndPTR = listEndPTR - 1;
				listEndPTR[0] = -1;
			}
			else if (userChatting < tmpPtr && tmpPtr < listEndPTR) {
				tmpPtr[0] = listEndPTR[-1];
				listEndPTR = listEndPTR - 1;
				listEndPTR[0] = -1;
			}
			else
				return -1;

			Unlock();
		}
	//not found user to remove
	return -1;
}

void Lock()
{
	if (mutex != NULL)
		WaitForSingleObject(mutex, INFINITE);
}

void Unlock()
{
	if (mutex != NULL)
		ReleaseMutex(mutex);
}