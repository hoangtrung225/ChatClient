#include <stdlib.h>
#define INITUALSIZE 256
int* userOnline, *userWaiting, *userChatting, *listEndPTR;
void userManagerInit(void) {
	//when initual there is no user online, waiting or chating
	userWaiting = userChatting = listEndPTR
		= userOnline = (int*)malloc(INITUALSIZE * sizeof(int));
	//list user end with -1
	listEndPTR[0] = -1;
}

int addUser(int userID) {
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
}

//return 0 on success found idUserOnline in onlineList and move it to wating  list
//return -1 if not found
int addWaiting(int idUserOnline) {
	//search userOnline for match userID
	for (int* tmpPtr = userOnline; tmpPtr < userWaiting; tmpPtr++) {
		if (tmpPtr[0] == idUserOnline) {
			//swap match value with last elem of user online list
			tmpPtr[0] = userWaiting[-1];

			//decrease size of userOnline list
			userWaiting = userWaiting - 1;

			//add idUserOnline to Waiting list
			userWaiting[0] = idUserOnline;
			return 0;
		}
	}
	return -1;
}

int addChating(int idUserWaiting) {
	//search userWaiting for match userID
	for (int* tmpPtr = userWaiting; tmpPtr < userChatting; tmpPtr++) {
		if (tmpPtr[0] == idUserWaiting) {
			//swap match value with last elem of user waiting list
			tmpPtr[0] = userChatting[-1];

			//decrease size of userWait list
			userChatting = userChatting - 1;

			//add idUserWaiting to Chating list
			userChatting[0] = idUserWaiting;
			return 0;
		}
	}
	return -1;
}

//remove Chating userID from chating list and insert back to online user
