#include "CBTHook.h"
#include <stdlib.h>
#include <sstream>
#include <string>

HHOOK CBT_HOOK_ID;
HWND hWnd;

std::wstring StringToWString(const std::string& s)
{
	std::wstring temp(s.length(), L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}

void AddItem(LPCSTR text) {
	//SendDlgItemMessage(hWnd, 4000, LB_ADDSTRING, 0, (LPARAM)text);
}

void AddHWND(WPARAM hwnd) {
	std::ostringstream oss;
	oss << std::hex << (int)hwnd;
	std::string Message = " ---Window Handle: 0x" + oss.str();
	AddItem(Message.c_str());
}

//void AddRECT(RECT rect) {
//	std::string out;
//	std::ostringstream oss;
//	oss << rect.top;
//	out = oss.str();
//	oss << rect.bottom;
//	out = out + ", " + oss.str();
//	oss << rect.left;
//	out = out + ", " + oss.str();
//	oss << rect.right;
//	out = out + ", " + oss.str();
//	std::string Message = "Window Moved/Resized: " + out;
//}

static LRESULT CALLBACK CBTHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode > 0) {
		CBT_CREATEWND* CBTHOOKCREATE;
		RECT* CBTRECTPTR;
		RECT CBTRECT;
		std::string Message;

		switch (nCode) {
			//case HCBT_ACTIVATE:
			//	AddItem("Window Activated");
			//case HCBT_CREATEWND:
			//	CBTHOOKCREATE = (CBT_CREATEWND*)lParam;
			//	AddItem("Window Created");
			//	Message = " ---Window Name: ";
			//	if (!IsBadReadPtr(CBTHOOKCREATE->lpcs, 1)) {
			//		if (!IsBadReadPtr(CBTHOOKCREATE->lpcs->lpszName, 1))
			//			Message = Message + CBTHOOKCREATE->lpcs->lpszName;
			//	}
			//	AddItem(Message.c_str());

			//	Message = " ---Window Class: ";
			//	if (!IsBadReadPtr(CBTHOOKCREATE->lpcs, 1)) {
			//		if (!IsBadReadPtr(CBTHOOKCREATE->lpcs->lpszClass, 1))
			//			Message = Message + CBTHOOKCREATE->lpcs->lpszClass;
			//	}
			//	AddItem(Message.c_str());
			//	AddHWND(wParam);
			case HCBT_DESTROYWND: {
				//AddItem("Window Destroyed");
				if ((HWND)wParam == hWnd) {
					return 1; // Return 1 to prevents closing window.
				}
			}
			//case HCBT_MINMAX:
			//	switch (lParam) {
			//		case SW_HIDE:
			//			AddItem("Window Hidden");
			//			break;
			//		case SW_MAXIMIZE:
			//			AddItem("Window Maximized");
			//			break;
			//		case SW_MINIMIZE:
			//			AddItem("Window Minimized");
			//			break;
			//		case SW_RESTORE:
			//			AddItem("Window Restored");
			//			break;
			//		case SW_SHOW:
			//			AddItem("Window Shown");
			//			break;
			//		case SW_SHOWDEFAULT:
			//			AddItem("Window Shown Default");
			//			break;
			//		case SW_SHOWMINIMIZED:
			//			AddItem("Window Shown Minimized");
			//			break;
			//		case SW_SHOWMINNOACTIVE:
			//			AddItem("Window Shown Minimized (Not Active)");
			//			break;
			//		case SW_SHOWNA:
			//			AddItem("Window Shown (Not Active)");
			//			break;
			//		case SW_SHOWNOACTIVATE:
			//			AddItem("Window Shown (Not Active)");
			//			break;
			//		case SW_SHOWNORMAL:
			//			AddItem("Window Shown");
			//			break;
			//	}
			//	AddHWND(wParam);
			//case HCBT_MOVESIZE:
			//	//CBTRECTPTR = (RECT*) lParam;
			//	//memcpy(&CBTRECT, CBTRECTPTR, sizeof(RECT));
			//	//AddRECT(CBTRECT);
			//	break;
			default: {
				break;
			}
		}
	}
	return CallNextHookEx(CBT_HOOK_ID, nCode, wParam, lParam);
}

bool SetCBTHook(DWORD ThreadID, HWND ThisMainWindow) {
	hWnd = ThisMainWindow;
	HHOOK CBT_HOOK_ID = ::SetWindowsHookEx(WH_CBT, CBTHookProc, 0, ThreadID);
	//MessageBox(NULL, "Hey", "There", 1);
	if (CBT_HOOK_ID)
		return true;
	else
		return false;
}

void RemoveCBTHook() {
	UnhookWindowsHookEx(CBT_HOOK_ID);
}
