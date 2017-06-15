#pragma once
#ifndef CLIENT_GUI
#define CLIENT_GUI

#define ID_TABCTRL 1
#define ID_EDIT 2
#define ID_LIST 3
#define ID_BUTTOM 4

#define MAX_TAB_LEN 15
#define MARGIN 10
#define TABMARGIN 40

#define SERVER_PORT 8080
#define SERVER_ADDR "127.0.0.1"

#define MAX_CHAT_CLIENT 20
#define MAX_LIST_CLIENT 100

int windowPotision(HWND hwnd);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE hInstance);
int initWindow(HWND hwnd);
int resizeWindow(HWND hwnd);
HWND makeNewChatWindow(void);

extern RECT rcEdit;
extern RECT rcTab;
extern RECT rcUserList;
extern RECT rcWaitList;
extern RECT rcButtom;
extern RECT rcChatWindow;

extern SOCKET client;
extern int thisUserId;

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

extern struct tabClientStruct partnerTab[];
extern struct structClientWaiting clientWaitList[];
extern struct structClientOnline clientOnlineList[];

extern char recvSocketBuffer[];
extern char sendSocketBuffer[];
extern wchar_t chatBuffer[];

extern HWND hTab, hEdit, hUserList, hWaitList, hButtom, hTagTab, hTagEdit, hTagUserList, hTagWaitList, hChatWindow, hCurrentWindow;

#endif