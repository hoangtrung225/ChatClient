// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#pragma comment(lib, "comctl32.lib") 
#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <commctrl.h>

// TODO: reference additional headers your program requires here
#include <WinSock2.h>

#define MAX_CHAT_CLIENT 20
#define MAX_LIST_CLIENT 100

extern SOCKET client;
extern int thisUserId;

extern struct tabClientStruct partnerTab[MAX_CHAT_CLIENT];
extern struct structClientWaiting clientWaitList[MAX_LIST_CLIENT];
extern struct structClientOnline clientOnlineList[MAX_LIST_CLIENT];

extern int clientChatID;
extern HWND hTab, hEdit, hUserList, hWaitList, hButtom, hTagTab, hTagEdit, hTagUserList, hTagWaitList, hChatWindow, hCurrentWindow;
extern int waitNumber, chatNumber, listNumber;
struct tabClientStruct {
	int tabNumber;       // tab control 
	HWND hwndDisplay;   // current child dialog box 
	int chatClientID;
};

struct structClientOnline {
	int listNumber;
	int chatClientID;
};

struct structClientWaiting {
	int listNumber;
	int chatClientID;
	int state;
};

#define STATE_WAIT_USER 0
#define STATE_WAIT_MESSAGE 1

char sendSocketBuffer[512];
char Buffer[512];