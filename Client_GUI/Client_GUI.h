#pragma once

#define ID_TABCTRL 1
#define ID_EDIT 2
#define ID_LIST 3
#define ID_BUTTOM 4
#define MAX_TAB_LEN 15
#define MARGIN 10
#define TABMARGIN 40
#define TABMARGIN 40 
#define BUFFSIZE 512
#define SERVER_PORT 8080
#define SERVER_ADDR "127.0.0.1"

int windowPotision(HWND hwnd);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE hInstance);
int initWindow(HWND hwnd);
int resizeWindow(HWND hwnd);
HWND makeNewChatWindow(void);
RECT rcEdit;
RECT rcTab;
RECT rcUserList;
RECT rcWaitList;
RECT rcButtom;