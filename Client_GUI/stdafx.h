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

extern SOCKET client;
extern int thisUserId;
extern struct tabClientStruct parnerTab[];
extern struct listClientStruct clientWaitList[];
extern int clientChatID;
extern HWND hTab, hEdit, hUserList, hWaitList, hButtom, hTagTab, hTagEdit, hTagUserList, hTagWaitList;
extern int waitNumber, chatNumber, listNumber;
struct tabClientStruct {
	int tabNumber;       // tab control 
	HWND hwndDisplay;   // current child dialog box 
	int chatClientID;
};

struct listClientStruct {
	int listNumber;
	int chatClientID;
};