#include <windows.h>
#include <commctrl.h>
#include <wchar.h>
#include "stdafx.h"
#include <WinSock2.h>

#pragma comment(lib, "comctl32.lib") 

#define ID_TABCTRL 1
#define ID_EDIT 2
#define ID_LIST 3
#define ID_BUTTOM 4
#define MAX_TAB_LEN 15
#define MARGIN 10
#define BUFFSIZE 512
#define SERVER_PORT 8080
#define SERVER_ADDR "127.0.0.1"

int windowPotision(HWND hwnd);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE hInstance);

struct tabClient {
	HWND hwndTab;       // tab control 
	HWND hwndDisplay;   // current child dialog box 
	int chatClientID;

};

HWND hTab, hEdit, hList , hButtom;
RECT rcEdit;
RECT rcTab;
RECT rcList;
RECT rcButtom;


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PWSTR pCmdLine, int nCmdShow) {

	//create connection
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		MessageBox(NULL, L"Version not support!", L"Error!", MB_OK);
		return 0;
	}

	//Construct socket	
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//(optional) Set time-out for receiving
	int tv = 10000; //Time-out interval: 10000ms
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

	//Specify server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	//Request to connect server
	/*if (connect(client, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		MessageBox(NULL, L"Cannot connect to server!", L"Error!", MB_OK);
		return 0;
	}*/
	MSG  msg;
	MyRegisterClass(hInstance);
	CreateWindowW(L"Chat Client", L"Chat Client",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 100, 380, 230, 0, 0, hInstance, 0);

	while (GetMessage(&msg, NULL, 0, 0)) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
	WPARAM wParam, LPARAM lParam) {
	TCITEMW tie;
	wchar_t text[BUFFSIZE];
	LRESULT count, id;
	INITCOMMONCONTROLSEX icex;

	switch (msg) {

	case WM_CREATE:

		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_TAB_CLASSES;
		InitCommonControlsEx(&icex);

		windowPotision(hwnd);
		hList = CreateWindowW(WC_LISTBOXW, NULL, WS_CHILD
			| WS_VISIBLE | LBS_NOTIFY, rcList.left, rcList.top, rcList.right - rcList.left, rcList.bottom - rcList.top, hwnd,
			(HMENU)ID_LIST, NULL, NULL);
		
		hTab = CreateWindowW(WC_TABCONTROLW, NULL, WS_CHILD | WS_VISIBLE,
			rcTab.left, rcTab.top, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, hwnd, (HMENU)ID_TABCTRL, NULL, NULL);

		hEdit = CreateWindowW(WC_EDITW, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
			rcEdit.left, rcEdit.top, rcEdit.right - rcEdit.left, rcEdit.bottom - rcEdit.top, hwnd, (HMENU)ID_EDIT, NULL, NULL);

		hButtom = CreateWindowW(WC_BUTTONW, L"OK!", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			rcButtom.left, rcButtom.top, rcButtom.right - rcButtom.left, rcButtom.bottom - rcButtom.top, hwnd, (HMENU)ID_BUTTOM, NULL, NULL);

		SendMessage(hEdit, EM_SETLIMITTEXT, MAX_TAB_LEN, 0);

		break;

	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		case ID_BUTTOM:

			GetWindowTextW(hEdit, text, 250);

			if (lstrlenW(text) != 0) {

				tie.mask = TCIF_TEXT;
				tie.pszText = text;
				count = SendMessageW(hTab, TCM_GETITEMCOUNT, 0, 0);
				SendMessageW(hTab, TCM_INSERTITEMW, count,
					(LPARAM)(LPTCITEM)&tie);
			}
			break;

		
		}
		break;

	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	case WM_SIZE:
		windowPotision(hwnd);

		SetWindowPos(hTab, 0, rcTab.left, rcTab.top, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, 0);
		SetWindowPos(hEdit, 0, rcEdit.left, rcEdit.top, rcEdit.right - rcEdit.left, rcEdit.bottom - rcEdit.top, 0);
		SetWindowPos(hList, 0, rcList.left, rcList.top, rcList.right - rcList.left, rcList.bottom - rcList.top, 0);
		SetWindowPos(hButtom, 0, rcButtom.left, rcButtom.top, rcButtom.right - rcButtom.left, rcButtom.bottom - rcButtom.top, 0);

	}

	return(DefWindowProcW(hwnd, msg, wParam, lParam));
}

int windowPotision(HWND hwnd) {
	RECT rcWindow;
	GetClientRect(hwnd, &rcWindow);

	int windowWidth = rcWindow.right - rcWindow.left;
	int windowHigh = rcWindow.bottom - rcWindow.top;

	int buttomWidth = windowWidth / 7;
	rcList.left = MARGIN;
	rcList.top = MARGIN;
	rcList.right = windowWidth / 3 - 20;
	rcList.bottom = windowHigh - 2 * MARGIN;

	rcTab.left = windowWidth / 3;
	rcTab.top = MARGIN;
	rcTab.right = windowWidth - MARGIN;
	rcTab.bottom = 2 * windowHigh / 3;

	rcEdit.left = windowWidth / 3;
	rcEdit.top = 2 * windowHigh / 3 + 20;
	rcEdit.right = windowWidth - MARGIN - buttomWidth;
	rcEdit.bottom = windowHigh - MARGIN;

	rcButtom.left = windowWidth - buttomWidth;
	rcButtom.top = 2 * windowHigh / 3 + 20;
	rcButtom.right = windowWidth - MARGIN;
	rcButtom.bottom = windowHigh - MARGIN;
	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"Chat Client";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	return RegisterClassW(&wc);
}