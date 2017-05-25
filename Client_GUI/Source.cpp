#include "stdafx.h"
#include <Shlwapi.h>

struct userCommand {
	int CommandCode;
	wchar_t CommandName[4];
	wchar_t HelpMessage[64];
};

struct userCommand commandSupport[] = {
	{0, L"LGI", L"LGI [username] [password]: login server with username"},
	{1, L"LGO", L"LGO: log out"},
	{2, L"UDT", L"UDT: get update from server"},
	{3, L"IVT", L"IVT [username]: invite user to conversation"},
	{4, L"ACP", L"ACP [username]: accept invitation from user"},
	{5, L"DNY", L"DNY [username]: deny invitation from user"},
	{6, L"MSG", L"MSG [username]: [message]:send message to user"},
	{7, L"RGT", L"RGT [username] [password]: register username with server"}
};
int processUserInput(HWND hwnd, LPWSTR userInput) {
	if (userInput[0] == L':')
		doCommand(userInput + 1);
	else
		sendChatMessage(userInput);
}

doCommand