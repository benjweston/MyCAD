#include "EWD_listboxwindowsubclass.h"
#include "EWD_resource.h"

#include "CMD_commandmanager.h"//Required for access to command tooltip text.

#include "fault.h"
#include "log.h"

#include <windowsx.h>

#define MAKEPOINT(lParam) POINT({ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) });

using namespace mycad;

ListboxWindowSubclass::ListboxWindowSubclass(HWND hWnd, HINSTANCE hInstance)
	: WindowWin32(),
	m_mouseTrackLeave(MouseTrackEvents(TME_LEAVE)),
	m_mouseTrackHover(MouseTrackEvents(TME_HOVER)),
	m_hTooltip(0),
	m_ToolInfo({ 0 })
{
	m_hWnd = hWnd;
	s_hInstance = hInstance;

	m_hTooltip = ::CreateWindow(
		TOOLTIPS_CLASS,
		NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_hWnd,
		NULL,
		s_hInstance,
		(LPVOID)0
	);

	SendMessage(m_hTooltip, TTM_SETMAXTIPWIDTH, 0, (LPARAM)400);

	m_ToolInfo.cbSize = sizeof(TOOLINFO);
	m_ToolInfo.hinst = s_hInstance;
	m_ToolInfo.hwnd = m_hWnd;
	m_ToolInfo.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
	m_ToolInfo.uId = IDC_LISTBOX_TOOLTIP_ID;

	SendMessage(m_hTooltip, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&m_ToolInfo);

	::SetWindowSubclass(m_hWnd, s_lpfnListboxSubclassProc, IDC_LISTBOX_SUBCLASS_ID, (DWORD_PTR)this);
}
ListboxWindowSubclass::~ListboxWindowSubclass()
{
	::RemoveWindowSubclass(m_hWnd, s_lpfnListboxSubclassProc, IDC_LISTBOX_SUBCLASS_ID);
	::DestroyWindow(m_hTooltip);
}

int ListboxWindowSubclass::wm_contextmenu(WPARAM wParam, LPARAM lParam)
{
	SendMessage(GetParent(m_hWnd), WM_CONTEXTMENU, wParam, lParam);
	return 0;
}
int ListboxWindowSubclass::wm_mousehover(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	m_mouseTrackHover.Reset();

	return 0;
}
int ListboxWindowSubclass::wm_mouseleave()
{
	m_mouseTrackLeave.Reset();
	SendMessage(m_hTooltip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&m_ToolInfo);

	return 0;
}
int ListboxWindowSubclass::wm_mousemove(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult{ TRUE };

	UINT nKeycode = GET_KEYSTATE_WPARAM(wParam);//Get keystate.
	if ((MK_LBUTTON & nKeycode) || (MK_RBUTTON & nKeycode)) return 0;//If left or right mouse button is pressed then exit the function.

	POINT ptMouseMove = MAKEPOINT(lParam);//Get mouse position.
	if ((ptMouseMove.x == m_ptMouseMove.x) && (ptMouseMove.y == m_ptMouseMove.y)) return 0;//If mouse position is unchanged then exit the function;

	m_ptMouseMove = ptMouseMove;//Mouse position has changed; assign new position to POINT variable ptCursor.
	m_mouseTrackLeave.OnMouseMove(m_hWnd);
	m_mouseTrackHover.OnMouseMove(m_hWnd);

	SendMessage(m_hTooltip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&m_ToolInfo);

	LPARAM lpRetVal = ::SendMessage(m_hWnd, LB_ITEMFROMPOINT, 0, lParam);

	INT nIndex = (INT)LOWORD(lpRetVal);
	BOOL nPointIsNotInClientArea = (BOOL)HIWORD(lpRetVal);

	if (nPointIsNotInClientArea == TRUE) return 0;//The specified point is not in the client area so exit the function.
	if (m_nIndex == nIndex) return 0;//If the index hasn't changed, exit. This allows the tooltip to ignore cursor movement after creation.

	m_nIndex = nIndex;

	ClientToScreen(m_hWnd, &ptMouseMove);
	ptMouseMove.y += 23;
	SendMessage(m_hTooltip, TTM_TRACKPOSITION, 0, MAKELPARAM(ptMouseMove.x, ptMouseMove.y));

	lResult = ::SendMessage(m_hWnd, LB_SETCURSEL, (WPARAM)m_nIndex, 0);
	if (lResult == LB_ERR) return 0;

	int nCommandID = (int)SendMessage(m_hWnd, LB_GETITEMDATA, (WPARAM)m_nIndex, 0);
	if (nCommandID == LB_ERR) return 0;

	m_ToolInfo.lpszText = (LPSTR)CommandManager::s_mCommandByID.at(nCommandID).szTooltip;
	SendMessage(m_hTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ToolInfo);

	return 0;
}

LRESULT CALLBACK ListboxWindowSubclass::s_lpfnListboxSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	UNREFERENCED_PARAMETER(uIdSubclass);

	ListboxWindowSubclass* window = reinterpret_cast<ListboxWindowSubclass*>(dwRefData);

	switch (uMsg) {
		case WM_CONTEXTMENU: {
			window->wm_contextmenu(wParam, lParam);
			return 0;//Avoid default edit control context menu behaviour;
		}
		case WM_MOUSEHOVER: {
			window->wm_mousehover(wParam, lParam);
			break;
		}
		case WM_MOUSELEAVE: {
			window->wm_mouseleave();
			break;
		}
		case WM_MOUSEMOVE: {
			window->wm_mousemove(wParam, lParam);
			break;
		}
		default: {
			break;
		}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
