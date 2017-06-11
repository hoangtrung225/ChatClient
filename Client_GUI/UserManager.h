#pragma once

#define STATE_USER_ONLINE 1
#define STATE_USER_WAITING 2
#define STATE_USER_CHATING 3

void userManagerInit(void);
int addUser(int userID);
int addWaiting(int idUserOnline);
int addChating(int idUserWaiting);
int endConversation(int idUserChatting);
int refuseRequest(int waitingRequest);
int removeUser(int usertoRemove);
int getUserStatus(int userID); 
