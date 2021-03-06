#include <windows.h>
#include <commctrl.h>
#include <wchar.h>
#include "stdafx.h"
#include <WinSock2.h>
#include "Client_GUI.h"
#include "StructControler.h"
#include "MessageProcess.h"
#include "CommandControler.h"
#include "UserManager.h"

char recvSocketBuffer[BUFFSIZE];
char sendSocketBuffer[BUFFSIZE];
wchar_t chatBuffer[BUFFSIZE];

RECT rcEdit;
RECT rcTab;
RECT rcUserList;
RECT rcWaitList;
RECT rcButtom;
RECT rcChatWindow;


struct tabClientStruct partnerTab[MAX_CHAT_CLIENT];
struct structClientWaiting clientWaitList[MAX_LIST_CLIENT];
struct structClientOnline clientOnlineList[MAX_LIST_CLIENT];

HWND hTab, hEdit, hUserList, hWaitList, hButtom, hTagTab, hTagEdit, hTagUserList, hTagWaitList, hChatWindow, hCurrentWindow;

#pragma comment(lib, "comctl32.lib") 

#define WM_SOCKET WM_USER + 1

HWND hwnd;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PWSTR pCmdLine, int nCmdShow) {


	programInitValues();
	userManagerInit();

	//create connection
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		MessageBox(NULL, L"Version not support!", L"Error!", MB_OK);
		return 0;
	}

	//Construct socket	
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//(optional) Set time-out for receiving
	int tv = 10000; //Time-out interval: 10000ms
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

	//Specify server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	MSG  msg;
	MyRegisterClass(hInstance);
	hwnd = CreateWindowW(L"Chat Client", L"Chat Client",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 100, 600, 550, 0, 0, hInstance, 0);

	//Request to connect server
	if (connect(client, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		swprintf_s(chatBuffer, L"Error cannot connect to server: %d", WSAGetLastError());
		MessageBox(NULL, chatBuffer, L"Error!", MB_OK);

		return 0;
	}

	//request window message for hwnd
	WSAAsyncSelect(client, hwnd, WM_SOCKET, FD_CLOSE | FD_READ);

	initWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	wchar_t text[BUFFSIZE];
	INITCOMMONCONTROLSEX icex;
	LPNMHDR	tc = (LPNMHDR)lParam;
	int ret = 0;

	switch (msg) {

	case WM_CREATE:

		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_TAB_CLASSES;
		InitCommonControlsEx(&icex);

		windowPotision(hwnd);

		SendMessage(hEdit, EM_SETLIMITTEXT, MAX_TAB_LEN, 0);

		break;

	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		case ID_BUTTOM:

			GetWindowTextW(hEdit, text, 250);

			if (lstrlenW(text) != 0) {
				processUserInput(hwnd, text);
			}
			break;
		}
		break;
	case WM_SOCKET:
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			ret = recv(client, recvSocketBuffer, BUFFSIZE, 0);
			if (ret > 0) {
				recvSocketBuffer[ret] = '\0';
				processIncomingMessage(recvSocketBuffer, ret);
			}
		case FD_CLOSE:
			exit(0);
		}
	case WM_NOTIFY:
		//Is event for tab change
		switch (tc->code)
		{
		case TCN_SELCHANGE:
			if (tc->idFrom == ID_TABCTRL) {
				int currentTabID = SendMessageW(hTab, TCM_GETCURSEL, 0, 0);
				struct tabClientStruct* tabStruct = getTabStruct(currentTabID);
				if (hCurrentWindow == NULL)
					break;
				if (hCurrentWindow != tabStruct->hwndDisplay) {
					//hide current window save selected window as current and show
					ShowWindow(hCurrentWindow, SW_HIDE);
					hCurrentWindow = tabStruct->hwndDisplay;
					//resize chat window

					SetWindowPos(hCurrentWindow, 0, rcTab.left, rcTab.top + 20 + TABMARGIN, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top - 20 - TABMARGIN, 0);

					ShowWindow(hCurrentWindow, SW_SHOW);
					break;
				}
			}
			break;
		}
		break;
	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	case WM_SIZE:
		windowPotision(hwnd);
		resizeWindow(hwnd);
		//set tag potision
		/*SetWindowPos(hTagTab, 0, rcTab.left, rcTab.top, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, 0);
		SetWindowPos(hTab, 0, rcTab.left, rcTab.top, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, 0);
		SetWindowPos(hEdit, 0, rcEdit.left, rcEdit.top, rcEdit.right - rcEdit.left, rcEdit.bottom - rcEdit.top, 0);
		SetWindowPos(hUserList, 0, rcUserList.left, rcUserList.top, rcUserList.right - rcUserList.left, rcUserList.bottom - rcUserList.top, 0);
		SetWindowPos(hButtom, 0, rcButtom.left, rcButtom.top, rcButtom.right - rcButtom.left, rcButtom.bottom - rcButtom.top, 0);*/

	}

	return(DefWindowProcW(hwnd, msg, wParam, lParam));
}

int windowPotision(HWND hwnd) {
	RECT rcWindow;
	GetClientRect(hwnd, &rcWindow);

	int windowWidth = rcWindow.right - rcWindow.left;
	int windowHigh = rcWindow.bottom - rcWindow.top;

	int buttomWidth = windowWidth / 7;
	rcUserList.left = MARGIN;
	rcUserList.top = MARGIN;
	rcUserList.right = windowWidth / 3 - MARGIN;
	rcUserList.bottom = windowHigh * 3 / 5;

	rcWaitList.left = MARGIN;
	rcWaitList.top = windowHigh * 3 / 5 + MARGIN;
	rcWaitList.right = windowWidth / 3 - MARGIN;
	rcWaitList.bottom = windowHigh - MARGIN;

	rcTab.left = windowWidth / 3;
	rcTab.top = MARGIN;
	rcTab.right = windowWidth - MARGIN;
	rcTab.bottom = 4 * windowHigh / 5;

	rcEdit.left = windowWidth / 3;
	rcEdit.top = 4 * windowHigh / 5 + 20;
	rcEdit.right = windowWidth - MARGIN - buttomWidth;
	rcEdit.bottom = windowHigh - MARGIN;

	rcButtom.left = windowWidth - buttomWidth;
	rcButtom.top = 4 * windowHigh / 5 + 20;
	rcButtom.right = windowWidth - MARGIN;
	rcButtom.bottom = windowHigh - MARGIN;

	TabCtrl_AdjustRect(hTab, FALSE, &rcChatWindow);
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

int resizeWindow(HWND hwnd) {
	int tagWaitListH = 20;
	int tagWaitListW = rcUserList.right - rcUserList.left;
	SetWindowPos(hTagUserList, 0, rcUserList.left, rcUserList.top, tagWaitListW, tagWaitListH, 0);
	SetWindowPos(hUserList, 0, rcUserList.left, rcUserList.top + tagWaitListH, rcUserList.right - rcUserList.left, rcUserList.bottom - rcUserList.top - tagWaitListH, 0);
	
	SetWindowPos(hTagWaitList, 0, rcWaitList.left, rcWaitList.top, tagWaitListW, tagWaitListH, 0);
	SetWindowPos(hWaitList, 0, rcWaitList.left, rcWaitList.top + tagWaitListH, rcWaitList.right - rcWaitList.left, rcWaitList.bottom - rcWaitList.top - tagWaitListH, 0);

	int tagTabH = 20;
	int tagTabW = rcTab.right - rcTab.left;
	SetWindowPos(hTagTab, 0, rcTab.left, rcTab.top, tagTabW, tagTabH, 0);
	SetWindowPos(hTab, 0, rcTab.left, rcTab.top + tagTabH, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top - tagTabH, 0);

	int tagEditH = 20;
	int tagEditW = rcEdit.right - rcEdit.left;
	SetWindowPos(hTagEdit, 0, rcEdit.left, rcEdit.top, tagEditW, tagEditH, 0);
	SetWindowPos(hEdit, 0, rcEdit.left, rcEdit.top + tagEditH, rcEdit.right - rcEdit.left, rcEdit.bottom - rcEdit.top - tagEditH, 0);

	int tagButtomH = 20;
	int tagButtomW = rcButtom.right - rcButtom.left;
	//CreateWindowW(L"Static", L"Send", WS_CHILD | WS_VISIBLE | SS_LEFT,
	//	rcButtom.left, rcButtom.top, tagButtomW, tagButtomH,
	//	hwnd, (HMENU)0, NULL, NULL);
	SetWindowPos(hButtom, 0, rcButtom.left, rcButtom.top, rcButtom.right - rcButtom.left, rcButtom.bottom - rcButtom.top, 0);

	SetWindowPos(hCurrentWindow, 0, rcTab.left, rcTab.top + tagTabH + TABMARGIN, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top - tagTabH - TABMARGIN, 0);

	return 0;
}

int initWindow(HWND hwnd) {
	int tagWaitListH = 20;
	int tagWaitListW = rcUserList.right - rcUserList.left;
	hTagUserList = CreateWindowW(L"Static", L"User Online", WS_CHILD | WS_VISIBLE | SS_LEFT,
		rcUserList.left, rcUserList.top, tagWaitListW, tagWaitListH,
		hwnd, (HMENU)0, NULL, NULL);

	hUserList = CreateWindowW(WC_LISTBOXW, NULL, WS_VSCROLL | WS_CHILD | WS_VISIBLE | LBS_NOTIFY,
		rcUserList.left, rcUserList.top + tagWaitListH, rcUserList.right - rcUserList.left, rcUserList.bottom - rcUserList.top - tagWaitListH,
		hwnd, (HMENU)ID_LIST, NULL, NULL);

	hTagWaitList = CreateWindowW(L"Static", L"Waiting Response", WS_CHILD | WS_VISIBLE | SS_LEFT,
		rcWaitList.left, rcWaitList.top, tagWaitListW, tagWaitListH,
		hwnd, (HMENU)0, NULL, NULL);

	hWaitList = CreateWindowW(WC_LISTBOXW, NULL, WS_VSCROLL | WS_CHILD | WS_VISIBLE | LBS_NOTIFY,
		rcWaitList.left, rcWaitList.top + tagWaitListH, rcWaitList.right - rcWaitList.left, rcWaitList.bottom - rcWaitList.top - tagWaitListH,
		hwnd, (HMENU)ID_LIST, NULL, NULL);

	int tagTabH = 20;
	int tagTabW = rcTab.right - rcTab.left;
	hTagTab = CreateWindowW(L"Static", L"Chating parner", WS_CHILD | WS_VISIBLE | SS_LEFT,
		rcTab.left, rcTab.top, tagTabW, tagTabH,
		hwnd, (HMENU)0, NULL, NULL);

	hTab = CreateWindowW(WC_TABCONTROLW, NULL, WS_CHILD | WS_VISIBLE,
		rcTab.left, rcTab.top + tagTabH, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top - tagTabH,
		hwnd, (HMENU)ID_TABCTRL, NULL, NULL);

	int tagEditH = 20;
	int tagEditW = rcEdit.right - rcEdit.left;
	hTagEdit = CreateWindowW(L"Static", L"Enter command", WS_CHILD | WS_VISIBLE | SS_LEFT,
		rcEdit.left, rcEdit.top, tagEditW, tagEditH,
		hwnd, (HMENU)0, NULL, NULL);

	hEdit = CreateWindowW(WC_EDITW, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
		rcEdit.left, rcEdit.top + tagEditH, rcEdit.right - rcEdit.left, rcEdit.bottom - rcEdit.top - tagEditH,
		hwnd, (HMENU)ID_EDIT, NULL, NULL);

	int tagButtomH = 20;
	int tagButtomW = rcButtom.right - rcButtom.left;
	//CreateWindowW(L"Static", L"Send", WS_CHILD | WS_VISIBLE | SS_LEFT,
	//	rcButtom.left, rcButtom.top, tagButtomW, tagButtomH,
	//	hwnd, (HMENU)0, NULL, NULL);

	hButtom = CreateWindowW(WC_BUTTONW, L"OK!", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		rcButtom.left, rcButtom.top , rcButtom.right - rcButtom.left, rcButtom.bottom - rcButtom.top,
		hwnd, (HMENU)ID_BUTTOM, NULL, NULL);
	return 0;
}

HWND makeNewChatWindow(void) {
	windowPotision(hwnd);

	int tagTabH = 20;
	int tagTabW = rcTab.right - rcTab.left;

	HWND returnHwnd = CreateWindowW(WC_LISTBOXW, NULL , WS_VSCROLL | WS_BORDER | WS_VISIBLE | WS_CHILD ,
		rcTab.left, rcTab.top + tagTabH + TABMARGIN, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top - tagTabH - TABMARGIN
		, hwnd, (HMENU)ID_TABCTRL, NULL, NULL);
	if (hCurrentWindow == NULL) {
		hCurrentWindow = returnHwnd;
		ShowWindow(hCurrentWindow, SW_SHOW);
	}
	ShowWindow(returnHwnd, SW_HIDE);

	return returnHwnd;
}