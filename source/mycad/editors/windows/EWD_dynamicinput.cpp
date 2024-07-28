#include "SYS_systemwin32.h"

#include "EWD_editwindowsubclass.h"
#include "EWD_dynamicinput.h"
#include "EWD_resource.h"

#include "DOC_document.h"

#include "fault.h"
#include "log.h"

#include <commctrl.h>
#include <windowsx.h>

#define MAKEPOINT(lParam) POINT({ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) });

#define COMMANDHEIGHT 19

using namespace mycad;                        

BOOL DynamicInput::s_nVisible{ TRUE };
SIZE DynamicInput::s_szOffset{ 17,17 };
SIZE DynamicInput::s_szSize{ 40,19 };
BOOL DynamicInput::s_nTintForXYZ{ FALSE };

DynamicInput::DynamicInput(int dropdownlines)
	: DynamicInputWindow(dropdownlines),
	m_lpEditSubclassWindow(nullptr),
	m_nDYNMODE(0),
	m_nDYNPROMPT(0),
	m_nDYNINFOTIPS(0)
{
	log("Log::DynamicInput::DynamicInput()");
	m_hbrBackground_Command = ::CreateSolidBrush(m_clrBackground_Command);
}
DynamicInput::~DynamicInput()
{
	log("Log::DynamicInput::~DynamicInput()");

	m_lpEditSubclassWindow = nullptr;
}

void DynamicInput::setDynamicInputMode(int mode) { m_nDYNMODE = mode; }
int DynamicInput::getDynamicInputMode() { return m_nDYNMODE; }

int DynamicInput::wm_close()
{
	DynamicInputWindow::wm_close();

	delete m_lpEditSubclassWindow;

	return 0;
}
int DynamicInput::wm_command(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = HIWORD(wParam);

	switch (nID) {
		case EN_CHANGE: {
			if (m_lpDropdownWindow == nullptr) break;
			wm_command_change();
			break;
		}
		case VK_ESCAPE: {
			wm_command_escape(IDC_DYNAMICINPUT_EDIT_COMMAND);
			::ShowWindow(m_hWnd, SW_HIDE);
			break;
		}
		case VK_RETURN: {
			wm_command_return(IDC_DYNAMICINPUT_EDIT_COMMAND);
			::ShowWindow(m_hWnd, SW_HIDE);
			break;
		}
		default: {
			break;
		}
	}

	return 0;
}
int DynamicInput::wm_contextmenu(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	HMENU hMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_DYNAMICINPUT_CONTEXTMENU));
	HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
	POINT ptCursor = MAKEPOINT(lParam);
	UINT uReturnCmd = ::TrackPopupMenuEx(
		hSubMenu,
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
		ptCursor.x,
		ptCursor.y,
		m_hWnd,
		NULL
	);

	::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	switch (uReturnCmd) {
		case ID_DYNAMICINPUT_CONTEXTMENU: {

			break;
		}
		case ID_DYNAMICINPUT_CONTEXTMENU_CUT: {

			break;
		}
		case ID_DYNAMICINPUT_CONTEXTMENU_COPY: {

			break;
		}
		case ID_DYNAMICINPUT_CONTEXTMENU_PASTE: {

			break;
		}
		case ID_DYNAMICINPUT_CONTEXTMENU_DELETE: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_SELECTALL: {

			break;
		}
		case ID_DYNAMICINPUT_CONTEXTMENU_RIGHTTOLEFTREADINGORDER: {

			break;
		}
		case ID_DYNAMICINPUT_CONTEXTMENU_SHOWUNICODECONTROLCHARACTERS: {

			break;
		}
		case ID_DYNAMICINPUT_CONTEXTMENU_INSERTUNICODECONTROLCHARACTERS: {

			break;
		}
		case ID_DYNAMICINPUT_CONTEXTMENU_OPENIME: {

			break;
		}
		case ID_DYNAMICINPUT_CONTEXTMENU_RECONVERSION: {

			break;
		}
		default: {
			break;
		}
	}

	DestroyMenu(hMenu);

	return 0;
}
int DynamicInput::wm_create(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	HRESULT hResult = S_OK;

	::strcpy_s(m_szCueBanner, "");

	m_clrBackground = RGB(255, 255, 255);
	m_clrBorder = RGB(0, 0, 0);
	m_clrTextBackground = RGB(255, 255, 255);

	RECT rcClient{ 0,0,0,0 };
	::GetClientRect(m_hWnd, &rcClient);

	m_hCommand = ::CreateWindow(
		WC_EDIT,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP/* | ES_UPPERCASE*/,
		rcClient.left + 1,
		rcClient.top + 1,
		rcClient.right + 10,
		rcClient.bottom - 1,
		m_hWnd,
		(HMENU)IDC_DYNAMICINPUT_EDIT_COMMAND,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hCommand == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)

		m_lpEditSubclassWindow = new EditWindowSubclass(m_hCommand);

		setFont(m_hCommand, "Consolas\0");

		WCHAR wszCueBanner[MAX_LOADSTRING]{ 0 };
		MultiByteToWideChar(CP_ACP, 0, m_szCueBanner, -1, wszCueBanner, _countof(wszCueBanner));
		BOOL nResult = (BOOL)::SendMessage(m_hCommand, EM_SETCUEBANNER, (WPARAM)FALSE, (LPARAM)&wszCueBanner);
		if (nResult == FALSE) {
			ErrorHandler();
		}

		RECT rcPosition = { 0,0,0,0 };
		::GetWindowRect(m_hWnd, &rcPosition);

		createDropdownWindow(
			{ rcPosition.left,rcPosition.bottom + DropdownWindow::s_szOffset.cy,DropdownWindow::s_szSize.cx,m_nDropdownLines * DropdownWindow::s_szSize.cy + 2 },
			IDC_DYNAMICINPUT_DROPDOWNWINDOW_LISTBOX_COMMANDS
		);

		//m_lpDropdownWindow->setBackgroundColour(RGB(33, 40, 48));
		m_lpDropdownWindow->setBackgroundColour(RGB(73, 79, 105));
		m_lpDropdownWindow->setTextColour(RGB(255, 255, 255));

#pragma warning( pop )
	}

	return 0;
}
int DynamicInput::wm_destroy()
{
	::DeleteObject(m_hbrBackground_Command);

	m_system->getWindowManager().setObjectInactive(this);
	BOOL nResult = m_system->getWindowManager().removeObject(this);
	if (nResult == TRUE) {
		log("Log::DynamicInput::wm_destroy() m_system->getWindowManager().removeWindow == TRUE");
	}
	else {
		log("Log::DynamicInput::wm_destroy() m_system->getWindowManager().removeWindow == FALSE");
	}

	return 0;
}
int DynamicInput::wm_killfocus(WPARAM wParam)
{
	HWND hWndFocus = (HWND)wParam;

	if (hWndFocus == m_lpDropdownWindow->getHWND()) return 0;

	if (::IsWindowVisible(m_lpDropdownWindow->getHWND()) == TRUE) {
		::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);
	}

	return 0;
}
int DynamicInput::wm_move(LPARAM lParam)
{
	m_x = GET_X_LPARAM(lParam);
	m_y = GET_Y_LPARAM(lParam);

	setDropdownPosition();

	return 0;
}
int DynamicInput::wm_paint()
{
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint(m_hWnd, &ps);
	ps.rcPaint.left--;
	ps.rcPaint.top--;
	::FillRect(hDC, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
	//::FrameRect(hDC, &ps.rcPaint, (HBRUSH)COLOR_3DSHADOW);
	::FrameRect(hDC, &ps.rcPaint, (HBRUSH)CreateSolidBrush((COLORREF)RGB(138, 138, 138)));
	//::FrameRect(hDC, &ps.rcPaint, (HBRUSH)CreateSolidBrush(m_clrBorder));
	::EndPaint(m_hWnd, &ps);
	::ReleaseDC(m_hWnd, hDC);
	return 0;
}
int DynamicInput::wm_setfocus(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);

	::SetFocus(m_hCommand);

	return 0;
}
int DynamicInput::wm_showwindow(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	BOOL bState = (BOOL)wParam;
	if (bState == FALSE) {
		BOOL nTimer = ::KillTimer(m_hWnd, IDT_LOWERCASETOUPPERCASEDELAY);
		if (nTimer == 0) {
			ErrorHandler();
		}
		::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);
	}

	return 0;
}
int DynamicInput::wm_size(WPARAM wParam, LPARAM lParam)//We use WM_SIZE and WM_MOVE, not WM_WINDOWPOSCHANGED, because we need the client window size.
{
	UNREFERENCED_PARAMETER(wParam);

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	::SetWindowPos(m_hCommand,
		NULL,
		1,
		1,
		m_cx + 10,
		m_cy - 1,
		SWP_SHOWWINDOW
	);

	return 0;
}

void DynamicInput::wm_command_change()
{
	int nResult = 0;

	char chInputBuf[MAX_LOADSTRING] = "\0";

	int cch = (int)::SendMessage(m_hCommand, WM_GETTEXTLENGTH, 0, 0);
	if (cch == 0) {
		::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);
		return;
	}

	if (::IsWindowVisible(m_lpDropdownWindow->getHWND()) == FALSE) {
		UINT_PTR nTimer = ::SetTimer(m_hWnd, IDT_INPUTSEARCHDELAY, DynamicInputWindow::s_nInputSearchDelay, NULL);
		if (nTimer == 0) {
			ErrorHandler();
		}
	}

	cch++;
	nResult = (int)::SendMessage(m_hCommand, WM_GETTEXT, (WPARAM)cch, (LPARAM)&chInputBuf);
	if (nResult != cch - 1) {
		ErrorHandler();
		return;
	}

	SIZE m_szChar = { 0,0 };
	HDC hDC = ::GetDC(m_hWnd);
	::GetTextExtentPoint32(hDC, chInputBuf, cch, &m_szChar);
	::ReleaseDC(m_hWnd, hDC);

	int nWidth = m_szChar.cx + 8;
	if (nWidth < DynamicInput::s_szSize.cx) {
		nWidth = DynamicInput::s_szSize.cx;
	}

	::SetWindowPos(m_hWnd,
		HWND_TOP,
		m_x,
		m_y,
		nWidth,
		DynamicInput::s_szSize.cy,
		SWP_NOACTIVATE | SWP_NOMOVE
	);
	::InvalidateRect(m_hWnd, NULL, TRUE);
	::SetFocus(m_hCommand);

	if (cch > 1) {
		char ch = chInputBuf[cch - 2];
		if (::islower(ch) != 0) {
			if (m_bLowercaseToUppercaseDelay == false) {//If an IDT_LOWERCASETOUPPERCASEDELAY timer is not running, start one.
				UINT_PTR nTimer = ::SetTimer(m_hWnd, IDT_LOWERCASETOUPPERCASEDELAY, DynamicInputWindow::s_nLowercaseToUppercaseDelay, NULL);
				if (nTimer == 0) {
					ErrorHandler();
				}
				else {
					m_bLowercaseToUppercaseDelay = true;
				}
				return;//Don't select a string in the dropdown box until it has been set to uppercase.
			}
		}
	}

	//Select string in dropdown list.
	nResult = (int)::SendMessage(m_lpDropdownWindow->getListboxHWND(), LB_SELECTSTRING, (WPARAM)-1, (LPARAM)&chInputBuf);
	if (nResult == LB_ERR) {
		ErrorHandler();
	}

	log("Log::DynamicInput::wm_command_change() chInputBuf = %s", chInputBuf);

	::SetFocus(m_hCommand);
}

void DynamicInput::setDropdownPosition()
{
	//Find the height of the monitor that the lower-left corner of the dynamic input window is in.
	//int nMonitors = m_system->getMonitors();
	//
	//POINT pt = { m_x,m_y + m_cy };
	//RECT rc = { 0,0,0,0 };
	//int nScreenHeight = 0;
	//for (int i = 0; i < nMonitors; i++) {
	//	MONITORINFOEX mi;
	//	mi.cbSize = sizeof(mi);
	//	if (m_system->getMonitor(i, &mi) == TRUE) {
	//		if (PtInRect(&mi.rcWork, pt) == TRUE) {
	//			nScreenHeight = mi.rcWork.bottom;
	//		}
	//	}
	//}

	RECT rcPosition = { 0,0,0,0 };
	::GetWindowRect(m_hWnd, &rcPosition);

	int nTop = rcPosition.bottom + DropdownWindow::s_szOffset.cy;//Default dropdown window position = below CommandLine edit control
	int nHeight = m_nDropdownLines * DropdownWindow::s_szSize.cy + 2;
	if (nTop + nHeight > m_nScreenHeight) {
		nTop = rcPosition.bottom - nHeight - COMMANDHEIGHT;
	}

	::SetWindowPos(m_lpDropdownWindow->getHWND(),
		NULL,
		rcPosition.left,
		nTop,
		0,
		0,
		SWP_NOACTIVATE | SWP_NOSIZE
	);
}



BOOL wnd_PointerInput::s_nVisible{ FALSE };
SIZE wnd_PointerInput::s_szSize{ 40,19 };

wnd_PointerInput::wnd_PointerInput()
	: DynamicInputWindow()
{
	log("Log::wnd_PointerInput::wnd_PointerInput()");
}
wnd_PointerInput::~wnd_PointerInput()
{
	log("Log::wnd_PointerInput::~DynamicInput()");
}



BOOL wnd_DimensionInput::s_nVisible{ FALSE };
SIZE wnd_DimensionInput::s_szSize{ 40,19 };

wnd_DimensionInput::wnd_DimensionInput()
	: DynamicInputWindow()
{
	log("Log::wnd_DimensionInput::wnd_DimensionInput()");
}
wnd_DimensionInput::~wnd_DimensionInput()
{
	log("Log::wnd_DimensionInput::~wnd_DimensionInput()");
}



BOOL wnd_DynamicPrompts::s_nVisible{ FALSE };
SIZE wnd_DynamicPrompts::s_szSize{ 40,19 };

wnd_DynamicPrompts::wnd_DynamicPrompts()
	: DynamicInputWindow()
{
	log("Log::wnd_DynamicPrompts::wnd_DynamicPrompts()");
}
wnd_DynamicPrompts::~wnd_DynamicPrompts()
{
	log("Log::wnd_DynamicPrompts::~wnd_DynamicPrompts()");
}
