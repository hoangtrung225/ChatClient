#include <windows.h>
#include <commctrl.h>
#include <wchar.h>
#include "stdafx.h"
#include <WinSock2.h>
#include "Client_GUI.h"
#include "helper.h"

#pragma comment(lib, "comctl32.lib") 

HWND hwnd;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PWSTR pCmdLine, int nCmdShow) {
	
	programInitValues();

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

	//Request to connect server
	/*if (connect(client, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		MessageBox(NULL, L"Cannot connect to server!", L"Error!", MB_OK);
		return 0;
	}*/
	MSG  msg;
	MyRegisterClass(hInstance);
	hwnd = CreateWindowW(L"Chat Client", L"Chat Client",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 100, 600, 550, 0, 0, hInstance, 0);
	initWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	TCITEMW tie;
	wchar_t text[BUFFSIZE];
	LRESULT count;
	INITCOMMONCONTROLSEX icex;
	LPNMHDR	tc = (LPNMHDR)lParam;

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

				tie.mask = TCIF_TEXT;
				tie.pszText = text;
				count = SendMessageW(hTab, TCM_GETITEMCOUNT, 0, 0);
				SendMessageW(hTab, TCM_INSERTITEMW, count, (LPARAM)(LPTCITEM)&tie);
			}
			break;
		}
		break;
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
					int tagTabH = 20;
					int tagTabW = rcTab.right - rcTab.left;
					SetWindowPos(hCurrentWindow, 0, rcTab.left, rcTab.top + tagTabH + TABMARGIN, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top - tagTabH);
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

	SetWindowPos(hCurrentWindow, 0, rcTab.left, rcTab.top + tagTabH + TABMARGIN, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top - tagTabH);

	return 0;
}

int initWindow(HWND hwnd) {
	int tagWaitListH = 20;
	int tagWaitListW = rcUserList.right - rcUserList.left;
	hTagUserList = CreateWindowW(L"Static", L"User Online", WS_CHILD | WS_VISIBLE | SS_LEFT,
		rcUserList.left, rcUserList.top, tagWaitListW, tagWaitListH,
		hwnd, (HMENU)0, NULL, NULL);

	hUserList = CreateWindowW(WC_LISTBOXW, NULL, WS_CHILD | WS_VISIBLE | LBS_NOTIFY,
		rcUserList.left, rcUserList.top + tagWaitListH, rcUserList.right - rcUserList.left, rcUserList.bottom - rcUserList.top - tagWaitListH,
		hwnd, (HMENU)ID_LIST, NULL, NULL);

	hTagWaitList = CreateWindowW(L"Static", L"Waiting Response", WS_CHILD | WS_VISIBLE | SS_LEFT,
		rcWaitList.left, rcWaitList.top, tagWaitListW, tagWaitListH,
		hwnd, (HMENU)0, NULL, NULL);

	hWaitList = CreateWindowW(WC_LISTBOXW, NULL, WS_CHILD | WS_VISIBLE | LBS_NOTIFY,
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
	
	HWND returnHwnd = CreateWindowW(WC_TABCONTROLW, NULL, WS_VSCROLL | WS_BORDER | WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY,
		rcTab.left, rcTab.top + tagTabH + TABMARGIN, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top - tagTabH,
		hwnd, (HMENU)ID_TABCTRL, NULL, NULL);
	if (hCurrentWindow == NULL)
		hCurrentWindow = returnHwnd;
	return returnHwnd;
}