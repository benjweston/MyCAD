#include "SYS_systemwin32.h"

#include "CMD_commandmanager.h"

#include "EWD_editwindowsubclass.h"
#include "EWD_textwindow.h"
#include "EWD_commandline.h"
#include "EWD_dynamicinput.h"
#include "EWD_resource.h"

#include "EDL_dialog_options.h"
#include "EDL_resource.h"

#include "CMD_commandmanager.h"

#include "fault.h"
#include "log.h"

#include <commctrl.h>
#include <windowsx.h>

#define MAKEPOINT(lParam) POINT({ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) });

#define COMMANDHEIGHT 20

using namespace mycad;

BOOL CommandLine::s_nVisible{ FALSE };
BOOL CommandLine::s_nDocked{ FALSE };
BOOL CommandLine::s_nCheckBeforeClose{ FALSE };

INT_PTR CALLBACK CommandLine::s_nfnCommandLineCloseCheckProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (uMsg) {
		case WM_INITDIALOG: {
			return (INT_PTR)TRUE;
		}
		case WM_COMMAND: {
			if ((LOWORD(wParam) == IDYES) || (LOWORD(wParam) == IDNO)) {
				::EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
		}
		case WM_CTLCOLORSTATIC: {
			HDC hDC = (HDC)wParam;
			HWND hWnd = (HWND)lParam;
			int id = ::GetDlgCtrlID(hWnd);
			if (id == IDC_COMMANDLINE_CLOSECHECK_LTEXT_CLOSE) {
				//::SetBkMode(hDC, TRANSPARENT);
				COLORREF m_clrClose = RGB(0, 0, 255);
				::SetTextColor(hDC, m_clrClose);

				//HRESULT hResult = S_OK;
				//INT nResult = 0;
				//INT_PTR npBrush = (INT_PTR)::CreateSolidBrush(m_clrClose);
				//hResult = IntPtrToInt(npBrush, &nResult);
				//if (hResult != S_OK) {
				//	ErrorHandler();
				//}
			}
			break;
		}
		default: {
			break;
		}
	}

	return (INT_PTR)FALSE;
}

CommandLine::CommandLine(DynamicInputWindow* dynamicinput, int dropdownlines)
	: CommandLineWindow(dynamicinput, dropdownlines),
	m_lpEditSubclassWindow{ nullptr,nullptr },
	m_dwExStyle(0),
	m_szBorder({8,8}),
	m_mouseTrack(MouseTrackEvents(TME_LEAVE | TME_NONCLIENT)),
	m_bMouseInRect(false),
	m_clrClose(RGB(0, 0, 255))
{
	log("Log::CommandLine::CommandLine()");
	m_hbrBackground_History = ::CreateSolidBrush(m_clrBackground_History);
	m_hbrBackground_Command = ::CreateSolidBrush(m_clrBackground_Command);
}
CommandLine::~CommandLine()
{
	log("Log::CommandLine::~CommandLine()");

	m_lpEditSubclassWindow[0] = nullptr;
	m_lpEditSubclassWindow[1] = nullptr;
}

BOOL CommandLine::setText(const char* text)
{
	LRESULT lResult = FALSE;

	lResult = (LRESULT)DynamicInputWindow::setText(text);

	int cch = (int)::SendMessage(m_hCommand, WM_GETTEXTLENGTH, 0, 0);
	if (cch == 0) {
		setFont(m_hCommand, "Consolas\0", true);
	}
	else {
		setFont(m_hCommand, "Consolas\0");
	}

	return (BOOL)lResult;
}

int CommandLine::wm_close()
{
	//if (s_nCheckBeforeClose == TRUE) {
	//	if (DialogBox(s_hInstance, MAKEINTRESOURCE(IDD_COMMANDLINE_CLOSECHECK), m_hWnd, (DLGPROC)s_nfnCommandLineCloseCheckProc) == IDYES) {
	//		s_nCheckBeforeClose = FALSE;
	//	}
	//	else {
	//		return 1;
	//	}
	//}

	RECT rcWindow = { 0,0,0,0 };
	GetWindowRect(m_hWnd, &rcWindow);
	setWindowState(WINDOWSTATE_RECT, (LPARAM)&rcWindow, "\\FixedProfile\\Command Line Windows", "CommandLine.Position");

	CommandLineWindow::wm_close();

	delete m_lpEditSubclassWindow[0];
	delete m_lpEditSubclassWindow[1];

	return 0;
}
int CommandLine::wm_command(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	LRESULT lResult = FALSE;

	int nSource = LOWORD(wParam);
	int nID = HIWORD(wParam);

	switch (nSource) {
		case IDC_COMMANDLINE_EDIT_HISTORY: {
			switch (nID) {
				//case EN_CHANGE: {
				//	SetFocus(m_hCommand);
				//	break;
				//}
				case EN_SETFOCUS: {
					SetFocus(m_hCommand);
					if (m_lpDropdownWindow == nullptr) break;
					if (::IsWindowVisible(m_lpDropdownWindow->getHWND()) == TRUE) {
						::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);
					}
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		case IDC_COMMANDLINE_EDIT_COMMAND: {
			switch (nID) {
				case EN_CHANGE: {
					if (m_lpDropdownWindow == nullptr) break;
					wm_command_change();
					break;
				}
				case VK_ESCAPE: {
					char chInputBuf[1] = "";
					lResult = (LRESULT)::SendMessage(m_hCommand, WM_SETTEXT, (WPARAM)0, (LPARAM)&chInputBuf);
					if (lResult == FALSE) {
						ErrorHandler();
					}
					wm_command_escape(IDC_COMMANDLINE_EDIT_COMMAND);
					break;
				}
				case VK_RETURN: {
					if (m_lpDropdownWindow == nullptr) break;
					wm_command_return(IDC_COMMANDLINE_EDIT_COMMAND);
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		default: {
			break;
		}
	}

	return 0;
}
int CommandLine::wm_contextmenu(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	HMENU hMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_COMMANDLINE_CONTEXTMENU));
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
		case ID_COMMANDLINE_CONTEXTMENU_INPUTSETTINGS: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_AUTOCOMPLETE: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_AUTOCORRECT: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_SEARCHSYSTEMVARIABLES: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_SEARCHCONTENT: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_MIDSTRINGSEARCH: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_DELAYTIME: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_LINESOFPROMPTHISTORY: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_INPUTSEARCHOPTIONS: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_CUT: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_COPY: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_COPYHISTORY: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_PASTE: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_PASTETOCMDLINE: {

			break;
		}
		case ID_COMMANDLINE_CONTEXTMENU_OPTIONS: {
			createOptions();
			break;
		}
		default: {
			break;
		}
	}

	DestroyMenu(hMenu);

	return 0;
}
int CommandLine::wm_create(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	HRESULT hResult = S_OK;

	m_dwExStyle = (DWORD)::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);//Get the windows GWL_EXSTYLE so we can add/remove WS_EX_LAYERED.

	::strcpy_s(m_szCueBanner, "Type a command ");

	RECT rcClient{ 0,0,0,0 };
	::GetClientRect(m_hWnd, &rcClient);

	m_hCommand = ::CreateWindow(
		WC_EDIT,
		NULL,
		WS_CHILD | WS_TABSTOP,
		rcClient.left,
		rcClient.bottom - COMMANDHEIGHT + 2,
		rcClient.right,
		COMMANDHEIGHT - 2,
		m_hWnd,
		(HMENU)IDC_COMMANDLINE_EDIT_COMMAND,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hCommand == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)

		m_lpEditSubclassWindow[0] = new EditWindowSubclass(m_hCommand);

		setFont(m_hCommand, "Consolas", true);

		RECT rcPosition = { 0,0,0,0 };
		::GetWindowRect(m_hWnd, &rcPosition);

		createDropdownWindow(
			{ rcPosition.left - 1,rcPosition.bottom + DropdownWindow::s_szOffset.cy,DropdownWindow::s_szSize.cx,m_nDropdownLines * DropdownWindow::s_szSize.cy + 2 },
			IDC_COMMANDLINE_DROPDOWNWINDOW_LISTBOX_COMMANDS
		);

		//m_lpDropdownWindow->setBackgroundColour(RGB(33, 40, 48));
		m_lpDropdownWindow->setBackgroundColour(RGB(73, 79, 105));
		m_lpDropdownWindow->setTextColour(RGB(255, 255, 255));

#pragma warning( pop )
	}

	m_hHistory = ::CreateWindow(
		WC_EDIT,
		NULL,
		WS_BORDER | WS_CHILD | ES_MULTILINE | ES_READONLY | WS_TABSTOP | WS_VSCROLL,
		rcClient.left,
		rcClient.top,
		rcClient.right,
		rcClient.bottom - COMMANDHEIGHT,
		m_hWnd,
		(HMENU)IDC_COMMANDLINE_EDIT_HISTORY,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hHistory == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)

		m_lpEditSubclassWindow[1] = new EditWindowSubclass(m_hHistory);

		setFont(m_hHistory, "Consolas");

		WCHAR wszCueBanner[MAX_LOADSTRING]{ 0 };
		MultiByteToWideChar(CP_ACP, 0, m_szCueBanner, -1, wszCueBanner, _countof(wszCueBanner));
		BOOL nResult = (BOOL)::SendMessage(m_hCommand, EM_SETCUEBANNER, (WPARAM)FALSE, (LPARAM)&wszCueBanner);
		if (nResult == FALSE) {
			ErrorHandler();
		}

#pragma warning( pop )
	}

	return 0;
}
int CommandLine::wm_initmenupopup(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int CommandLine::wm_killfocus(WPARAM wParam)
{
	HWND hWndFocus = (HWND)wParam;

	if (hWndFocus == m_lpDropdownWindow->getHWND()) return 0;

	if (::IsWindowVisible(m_lpDropdownWindow->getHWND()) == TRUE) {
		::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);
	}

	return 0;
}
int CommandLine::wm_move(LPARAM lParam)
{
	m_x = GET_X_LPARAM(lParam);
	m_y = GET_Y_LPARAM(lParam);

	setDropdownPosition();

	return 0;
}
/*int CommandLine::wm_ncactivate(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	//RedrawWindow(m_hWnd, NULL, NULL, RDW_UPDATENOW);

	return 0;
}*/
/*int CommandLine::wm_nccalcsize(WPARAM wParam, LPARAM lParam)
{
	BOOL nResult = (BOOL)wParam;
	if (nResult == FALSE) {
		RECT* lprcClient = (RECT*)lParam;
		rcClient = *lprcClient;

		::InflateRect(lprcClient, -m_szBorder.cx, -m_szBorder.cy);
		lprcClient->top += 31;

		lParam = (LPARAM)&lprcClient;

		return 0;
	}
	else {
		NCCALCSIZE_PARAMS* nccalcsize_params = (NCCALCSIZE_PARAMS*)lParam;
		RECT rgrc[3];

		rgrc[0] = nccalcsize_params->rgrc[0];
		rcClient = rgrc[0];

		::InflateRect(&rgrc[0], -m_szBorder.cx, -m_szBorder.cy);
		rgrc[0].top += 31;

		rgrc[1] = nccalcsize_params->rgrc[1];
		rgrc[2] = nccalcsize_params->rgrc[2];

		nccalcsize_params->rgrc[0] = rgrc[0];
		lParam = (LPARAM)&nccalcsize_params;

		return (int)(long)(LRESULT)WVR_REDRAW;
	}
}*/
int CommandLine::wm_nccreate(LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	::SetWindowLongPtr(m_hWnd, GWL_STYLE, ::GetWindowLongPtr(m_hWnd, GWL_STYLE) & ~WS_MINIMIZEBOX);
	::SetWindowLongPtr(m_hWnd, GWL_STYLE, ::GetWindowLongPtr(m_hWnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);

	return 0;
}
int CommandLine::wm_ncmouseleave()
{
	m_mouseTrack.Reset();

	POINT ptCursor;
	::GetCursorPos(&ptCursor);
	
	RECT rcWindow = { 0,0,0,0 };
	GetWindowRect(m_hWnd, &rcWindow);

	if (::PtInRect(&rcWindow, ptCursor) == FALSE) {
		m_bMouseInRect = false;
		m_dwExStyle |= WS_EX_LAYERED;//Add WS_EX_LAYERED style attribute.
		::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, m_dwExStyle);
		SetLayeredWindowAttributes(m_hWnd, 0, (255 * 75) / 100, LWA_ALPHA);
	}

	return 0;
}
int CommandLine::wm_ncmousemove(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	m_mouseTrack.OnMouseMove(m_hWnd);

	if (m_bMouseInRect == false) {
		m_dwExStyle &= ~WS_EX_LAYERED;//Remove WS_EX_LAYERED style attribute.
		::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, m_dwExStyle);
		m_bMouseInRect = true;
	}

	return 0;
}
/*int CommandLine::wm_ncpaint(WPARAM wParam)
{
	//BOOL nResult = FALSE;

	HDC hdc = ::GetDCEx(m_hWnd, (HRGN)wParam, DCX_WINDOW | DCX_INTERSECTRGN);

	//nResult = ::DrawCaption(m_hWnd, hdc, &rcWindow, DC_BUTTONS | DC_TEXT);
	//if (nResult == FALSE) {
	//	ErrorHandler();
	//	log("Log::CommandLine::wm_ncpaint() DrawCaption FALSE");
	//}

	::ReleaseDC(m_hWnd, hdc);

	return 0;
}*/
//int CommandLine::wm_setcursor(WPARAM wParam, LPARAM lParam)//Set cursor and return FALSE for default processing.
//{
//	RECT rcHistory;
//	GetClientRect(m_hHistory, &rcHistory);
//
//	POINT ptCursor;
//	::GetCursorPos(&ptCursor);
//	::ScreenToClient((HWND)wParam, &ptCursor);
//
//	if (::PtInRect(&rcHistory, ptCursor) == TRUE) {
//		::SetCursor(NULL);
//	}
//
//	return (int)::DefWindowProc(m_hWnd, WM_SETCURSOR, wParam, lParam);
//}
int CommandLine::wm_setfocus(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);

	if (TextWindow::s_nVisible == TRUE) {
		if (m_lpCommandInput != nullptr) {
			if (m_lpCommandInput->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpCommandInput->getHWND()) == TRUE) {
					::SendMessage(m_lpCommandInput->getHWND(), WM_KILLFOCUS, (WPARAM)m_hWnd, 0);
				}
			}
		}
	}

	if (DynamicInput::s_nVisible == TRUE) {
		if (m_lpDynamicInput != nullptr) {
			if (m_lpDynamicInput->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpDynamicInput->getHWND()) == TRUE) {
					::ShowWindow(m_lpDynamicInput->getHWND(), SW_HIDE);
				}
			}
		}
	}

	if (::IsWindowVisible(m_lpDropdownWindow->getHWND()) == TRUE) {
		char chInputBuf[1] = "";
		LRESULT lResult = (LRESULT)::SendMessage(m_hCommand, WM_SETTEXT, (WPARAM)0, (LPARAM)&chInputBuf);
		if (lResult == FALSE) {
			ErrorHandler();
		}
		::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);
	}

	return 0;
}
int CommandLine::wm_size(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	::SetWindowPos(m_hHistory,
		NULL,
		0,
		0,
		m_cx,
		m_cy - COMMANDHEIGHT,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hCommand,
		NULL,
		0,
		m_cy - COMMANDHEIGHT + 2,
		m_cx,
		COMMANDHEIGHT - 2,
		SWP_SHOWWINDOW
	);

	setDropdownPosition();

	return 0;
}
int CommandLine::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);

	switch (nID) {
		case SC_CLOSE: {
			log("Log::CommandLine::wm_syscommand() SC_CLOSE");
			if (s_nCheckBeforeClose == TRUE) {
				if (DialogBox(s_hInstance, MAKEINTRESOURCE(IDD_COMMANDLINE_CLOSECHECK), m_hWnd, (DLGPROC)s_nfnCommandLineCloseCheckProc) == IDYES) {

				}
				else {

				}
			}
			else {

			}
			s_nVisible = FALSE;
			break;
		}
		case SC_MAXIMIZE: {

			break;
		}
		case SC_MINIMIZE: {

			break;
		}
		case SC_RESTORE: {

			break;
		}
		default: {
			::SetFocus(m_hWnd);
			break;
		}
	}

	return 0;
}
int CommandLine::wm_timer(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nTimer = (int)wParam;

	switch (nTimer) {
		case IDT_INPUTSEARCHDELAY: {
			wm_timer_inputsearchdelay();
			break;
		}
		case IDT_LOWERCASETOUPPERCASEDELAY: {
			wm_timer_lowercasetouppercasedelay();
			break;
		}
		default: {
			break;
		}
	}

	return 0;
}

void CommandLine::wm_command_change()
{
	int nResult{ -1 };

	if (m_bIsIndirectTextInput == true) return;

	char chInputBuf[MAX_LOADSTRING]{ 0 };

	int cch = (int)::SendMessage(m_hCommand, WM_GETTEXTLENGTH, 0, 0);
	if (cch == 0) {
		::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);
		setFont(m_hCommand, "Consolas\0", true);
		m_lpCommandInput->setText("\0");
		return;
	}
	else if (cch == 1) {
		setFont(m_hCommand, "Consolas\0");
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

	m_lpCommandInput->setText(chInputBuf);

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

	nResult = (int)::SendMessage(m_lpDropdownWindow->getListboxHWND(), LB_SELECTSTRING, (WPARAM)-1, (LPARAM)&chInputBuf);
	if (nResult == LB_ERR) {
		ErrorHandler();
	}

	log("Log::CommandLine::wm_command_change() chInputBuf = %s", chInputBuf);

	::SetFocus(m_hCommand);
}
void CommandLine::wm_timer_lowercasetouppercasedelay()
{
	LRESULT lResult = FALSE;
	int nResult = 0;

	char chInputBuf[MAX_LOADSTRING]{ 0 };

	BOOL nTimer = ::KillTimer(m_hWnd, IDT_LOWERCASETOUPPERCASEDELAY);
	if (nTimer == 0) {
		ErrorHandler();
	}
	else {
		m_bLowercaseToUppercaseDelay = false;
	}

	int cch = (int)::SendMessage(m_hCommand, WM_GETTEXTLENGTH, 0, 0);
	cch++;
	nResult = (int)::SendMessage(m_hCommand, WM_GETTEXT, (WPARAM)cch, (LPARAM)&chInputBuf);
	if (nResult != cch - 1) {
		ErrorHandler();
		return;
	}

	for (int i = 0; i < cch; i++) {//Change all characters to uppercase.
		char ch = chInputBuf[i];
		char ch_upper = (char)::toupper((int)ch);
		chInputBuf[i] = ch_upper;
	}
	cch--;

	//Select string in dropdown list.
	lResult = (LRESULT)::SendMessage(m_hCommand, WM_SETTEXT, (WPARAM)cch, (LPARAM)&chInputBuf);
	if (lResult == TRUE) {
		::SendMessage(m_hCommand, EM_SETCARETINDEX, (WPARAM)cch, 0);
	}

	//Copy text to TextWindow
	m_lpCommandInput->setText(chInputBuf);
}

void CommandLine::setDropdownPosition()
{
	RECT rcWindow = { 0,0,0,0 };
	::GetWindowRect(m_hWnd, &rcWindow);

	//int nTop = rcWindow.bottom + DropdownWindow::s_szOffset.cy;//Default dropdown window position = below CommandLine edit control
	//int nHeight = m_nDropdownLines * DropdownWindow::s_szSize.cy + 2;
	//if (nTop + nHeight > m_nScreenHeight) {
	//	nTop = rcWindow.bottom - nHeight - COMMANDHEIGHT;
	//}

	int nHeight = m_nDropdownLines * DropdownWindow::s_szSize.cy + 2;//Default dropdown window position = above CommandLine edit control
	int nTop = rcWindow.bottom - nHeight - COMMANDHEIGHT;
	if (nTop < 0) {
		nTop = rcWindow.bottom + DropdownWindow::s_szOffset.cy;
	}

	::SetWindowPos(m_lpDropdownWindow->getHWND(),
		NULL,
		rcWindow.left + 8,
		nTop - 8,
		0,
		0,
		SWP_NOACTIVATE | SWP_NOSIZE
	);
}

void CommandLine::createOptions()
{
	DynamicInputWindow* m_lpCommandWindows[3] = { m_lpCommandInput, (DynamicInputWindow*)this, m_lpDynamicInput };
	dlg_Options* lpOptions = new dlg_Options(&CommandManager::s_mCommandByID.at(IDC_OPTIONS), m_lpCommandWindows, 1, CommandMode::NONE);//Default tab is Display(1).
	if (m_system->createDialog(lpOptions, IDD_OPTIONS, m_system->getApplication()->getHWND()) == IDOK) {
		log("Log::dlg_DraftingSettings::createOptions() IDOK");
	}

	if (lpOptions) {
		delete lpOptions;
		lpOptions = NULL;
	}
}
