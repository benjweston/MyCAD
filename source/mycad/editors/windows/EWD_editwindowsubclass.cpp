#include "EWD_editwindowsubclass.h"
#include "EWD_resource.h"

#include "fault.h"
#include "log.h"

#include <commctrl.h>//Required for SetWindowSubclass/RemoveWindowSubclass
#include <windowsx.h>

#define MAKEPOINT(lParam) POINT({ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) });

using namespace mycad;

EditWindowSubclass::EditWindowSubclass(HWND hWnd)
	: WindowWin32()
{
	m_hWnd = hWnd;
	::SetWindowSubclass(m_hWnd, s_lpfnEditSubclassProc, IDC_EDIT_SUBCLASS_ID, (DWORD_PTR)this);
}
EditWindowSubclass::~EditWindowSubclass()
{
	::RemoveWindowSubclass(m_hWnd, s_lpfnEditSubclassProc, IDC_EDIT_SUBCLASS_ID);
}

int EditWindowSubclass::wm_contextmenu(WPARAM wParam, LPARAM lParam)
{
	SendMessage(GetParent(m_hWnd), WM_CONTEXTMENU, wParam, lParam);
	return 0;
}

LRESULT CALLBACK EditWindowSubclass::s_lpfnEditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	UNREFERENCED_PARAMETER(uIdSubclass);

	EditWindowSubclass* window = reinterpret_cast<EditWindowSubclass*>(dwRefData);

	switch (uMsg) {
		case WM_CHAR: {//TODO: Should be using WM_KEYDOWN for virtual key-codes, not WM_CHAR for character codes. PROBLEM: WM_KEYDOWN issues MessageBeep.
			log("Log::EditWindowSubclass::s_lpfnEditSubclassProc() WM_KEYDOWN");
			WORD vkCode = LOWORD(wParam);
			switch (vkCode) {
				case VK_ESCAPE: {
					log("Log::EditWindowSubclass::s_lpfnEditSubclassProc() VK_ESCAPE");
					SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), VK_ESCAPE), (LPARAM)hWnd);
					return 0;//Avoid MessageBeep.
				}
				case VK_RETURN: {
					log("Log::EditWindowSubclass::s_lpfnEditSubclassProc() VK_RETURN");
					SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), VK_RETURN), (LPARAM)hWnd);
					return 0;//Avoid MessageBeep.
				}
				case VK_TAB: {
					log("Log::EditWindowSubclass::s_lpfnEditSubclassProc() VK_TAB");
					SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), VK_RETURN), (LPARAM)hWnd);
					return 0;//Avoid MessageBeep.
				}
				default: {
					break;
				}
			}
			break;
		}
		case WM_CONTEXTMENU: {
			window->wm_contextmenu(wParam, lParam);
			return 0;//Avoid default edit control context menu behaviour;
		}
		//case WM_KEYDOWN: {
		//	log("Log::EditWindowSubclass::s_lpfnEditSubclassProc() WM_KEYDOWN");
		//	WORD vkCode = LOWORD(wParam);
		//	switch (vkCode) {
		//		case VK_ESCAPE: {
		//			log("Log::EditWindowSubclass::s_lpfnEditSubclassProc() VK_ESCAPE");
		//			SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), VK_ESCAPE), (LPARAM)hWnd);
		//			return 0;//Avoid MessageBeep.
		//		}
		//		case VK_RETURN: {
		//			log("Log::EditWindowSubclass::s_lpfnEditSubclassProc() VK_RETURN");
		//			SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), VK_RETURN), (LPARAM)hWnd);
		//			return 0;//Avoid MessageBeep.
		//		}
		//		case VK_TAB: {
		//			log("Log::EditWindowSubclass::s_lpfnEditSubclassProc() VK_TAB");
		//			SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), VK_RETURN), (LPARAM)hWnd);
		//			return 0;//Avoid MessageBeep.
		//		}
		//		default: {
		//			return 0;
		//		}
		//	}
		//	break;
		//}
		default: {
			break;
		}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
