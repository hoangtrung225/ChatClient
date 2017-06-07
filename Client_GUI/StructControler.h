#pragma once
struct structClientOnline *getOnlineStructList(int listIndex);
struct structClientWaiting *getWaitStructList(int listIndex);
struct tabClientStruct* getTabStruct(int tabIndex);
int programInitValues(void);
int makeChatTabStruct(int clientID, int tabNumber);
int removeChatTabStruct(int clientID);
int makeWaitListStruct(int clientID, int state);
int removeWaitListStruct(int clientID);
int makeOnlineListStruct(int clientID);
int removeOnlineListStruct(int clientID);