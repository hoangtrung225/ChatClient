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
#include <wchar.h>
#include <commctrl.h>
#include <stdio.h> 

// TODO: reference additional headers your program requires here
#include <WinSock2.h>
#define BUFFSIZE 512

extern SOCKET client;
extern int thisUserId;

extern int clientChatID;
extern int waitNumber, chatNumber, listNumber;


#define STATE_WAIT_USER 0
#define STATE_WAIT_MESSAGE 1