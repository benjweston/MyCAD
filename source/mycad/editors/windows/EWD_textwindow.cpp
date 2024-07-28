#include "SYS_systemwin32.h"

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

#define STATICWIDTH 64
#define COMMANDHEIGHT 19

using namespace mycad;

BOOL TextWindow::s_nVisible{ FALSE };
BOOL TextWindow::s_nDocked{ FALSE };

TextWindow::TextWindow(DynamicInputWindow* dynamicinput, int dropdownlines)
	: CommandLineWindow(dynamicinput, dropdownlines),
	m_lpEditSubclassWindow{ nullptr,nullptr },
	m_hStatic(0),
	m_szStatic("\0")
{
	log("Log::TextWindow::TextWindow()");
	m_hbrBackground_History = ::CreateSolidBrush(m_clrBackground_History);
	m_hbrBackground_Command = ::CreateSolidBrush(m_clrBackground_Command);
}
TextWindow::~TextWindow()
{
	log("Log::TextWindow::~TextWindow()");

	m_lpEditSubclassWindow[0] = nullptr;
	m_lpEditSubclassWindow[1] = nullptr;
}

BOOL TextWindow::setStaticText(const char* text)
{
	LRESULT lResult = FALSE;
	lResult = (LRESULT)::SendMessage(m_hStatic, WM_SETTEXT, (WPARAM)0, (LPARAM)text);
	return (BOOL)lResult;
}

int TextWindow::wm_activate(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	m_nActive = LOWORD(wParam);

	switch (LOWORD(wParam)) {
		case WA_ACTIVE:
		case WA_CLICKACTIVE: {
			log("Log::TextWindow::wm_activate() WA_ACTIVE");
			::SetWindowPos(m_system->getApplication()->getHWND(), m_hWnd, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
			break;
		}
		case WA_INACTIVE: {
			log("Log::TextWindow::wm_activate() WA_INACTIVE");

			break;
		default:
			break;
		}
	}

	return 0;
}
int TextWindow::wm_close()
{
	RECT rcWindow = { 0,0,0,0 };
	GetWindowRect(m_hWnd, &rcWindow);
	setWindowState(WINDOWSTATE_RECT, (LPARAM)&rcWindow, "\\FixedProfile\\Command Line Windows", "TextWindow.Position");

	CommandLineWindow::wm_close();

	delete m_lpEditSubclassWindow[0];
	delete m_lpEditSubclassWindow[1];

	return 0;
}
int TextWindow::wm_command(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	LRESULT lResult = FALSE;

	int nSource = LOWORD(wParam);
	int nID = HIWORD(wParam);

	switch (nSource) {
		case IDC_TEXTWINDOW_EDIT_HISTORY: {
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
		case IDC_TEXTWINDOW_EDIT_COMMAND: {
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
					wm_command_escape(IDC_TEXTWINDOW_EDIT_COMMAND);
					break;
				}
				case VK_RETURN: {
					if (m_lpDropdownWindow == nullptr) break;
					wm_command_return(IDC_TEXTWINDOW_EDIT_COMMAND);
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		case ID_TEXTWINDOW_COPY: {

			break;
		}
		case ID_TEXTWINDOW_COPYHISTORY: {

			break;
		}
		case ID_TEXTWINDOW_PASTE: {

			break;
		}
		case ID_TEXTWINDOW_PASTETOCMDLINE: {

			break;
		}
		case ID_TEXTWINDOW_OPTIONS: {
			createOptions();
			break;
		}
		default: {
			break;
		}
	}

	return 0;
}
int TextWindow::wm_contextmenu(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	HMENU hMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_TEXTWINDOW_CONTEXTMENU));
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
		case ID_TEXTWINDOW_CONTEXTMENU_INPUTSETTINGS: {

			break;
		}
		case ID_TEXTWINDOW_CONTEXTMENU_AUTOCOMPLETE: {

			break;
		}
		case ID_TEXTWINDOW_CONTEXTMENU_AUTOCORRECT: {

			break;
		}
		case ID_TEXTWINDOW_CONTEXTMENU_SEARCHSYSTEMVARIABLES: {

			break;
		}
		case ID_TEXTWINDOW_CONTEXTMENU_MIDSTRINGSEARCH: {

			break;
		}
		case ID_TEXTWINDOW_CONTEXTMENU_DELAYTIME: {

			break;
		}
		case ID_TEXTWINDOW_CONTEXTMENU_COPY: {

			break;
		}
		case ID_TEXTWINDOW_CONTEXTMENU_COPYHISTORY: {

			break;
		}
		case ID_TEXTWINDOW_CONTEXTMENU_PASTE: {

			break;
		}
		case ID_TEXTWINDOW_CONTEXTMENU_PASTETOCMDLINE: {

			break;
		}
		case ID_TEXTWINDOW_CONTEXTMENU_OPTIONS: {
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
int TextWindow::wm_create(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	HRESULT hResult = S_OK;
	LRESULT lResult = FALSE;

	::strcpy_s(m_szStatic, "Command: ");
	::strcpy_s(m_szCueBanner, "");

	RECT rcClient{ 0,0,0,0 };
	::GetClientRect(m_hWnd, &rcClient);

	m_hStatic = ::CreateWindow(
		WC_STATIC,
		NULL,
		WS_CHILD | SS_LEFT | WS_TABSTOP,
		0,
		m_cy - COMMANDHEIGHT + 2,
		STATICWIDTH,
		COMMANDHEIGHT - 2,
		m_hWnd,
		(HMENU)IDC_TEXTWINDOW_STATIC_COMMAND,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hStatic == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)

		setFont(m_hStatic, "Consolas\0");

		lResult = (LRESULT)::SendMessage(m_hStatic, WM_SETTEXT, (WPARAM)10, (LPARAM)m_szStatic);
		if (lResult == FALSE) {
			ErrorHandler();
		}

#pragma warning( pop )
	}

	m_hCommand = ::CreateWindow(
		WC_EDIT,
		NULL,
		WS_CHILD | WS_TABSTOP,
		rcClient.left,
		rcClient.bottom - COMMANDHEIGHT + 2,
		rcClient.right,
		COMMANDHEIGHT - 2,
		m_hWnd,
		(HMENU)IDC_TEXTWINDOW_EDIT_COMMAND,
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

		setFont(m_hCommand, "Consolas\0");

		RECT rcPosition = { 0,0,0,0 };
		::GetWindowRect(m_hWnd, &rcPosition);

		createDropdownWindow(
			{ rcPosition.left,rcPosition.bottom + DropdownWindow::s_szOffset.cy,DropdownWindow::s_szSize.cx,m_nDropdownLines * DropdownWindow::s_szSize.cy + 2 },
			IDC_TEXTWINDOW_DROPDOWNWINDOW_LISTBOX_COMMANDS
		);

#pragma warning( pop )
	}

	m_hHistory = ::CreateWindow(
		WC_EDIT,
		NULL,
		WS_BORDER | WS_CHILD | ES_MULTILINE | ES_READONLY | WS_TABSTOP | WS_VSCROLL,
		//WS_BORDER | WS_CHILD | ES_MULTILINE | WS_TABSTOP | WS_VSCROLL,
		rcClient.left + 1,
		rcClient.top + 1,
		rcClient.right + 10,
		rcClient.bottom - COMMANDHEIGHT,
		m_hWnd,
		(HMENU)IDC_TEXTWINDOW_EDIT_HISTORY,
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

		//setFont(m_hHistory, "Microsoft Sans Serif\0");
		setFont(m_hHistory, "Consolas\0");

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
int TextWindow::wm_initmenupopup(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int TextWindow::wm_killfocus(WPARAM wParam)
{
	HWND hWndFocus = (HWND)wParam;

	if (hWndFocus == m_lpDropdownWindow->getHWND()) return 0;

	if (::IsWindowVisible(m_lpDropdownWindow->getHWND()) == TRUE) {
		::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);
	}

	return 0;
}
int TextWindow::wm_move(LPARAM lParam)
{
	m_x = GET_X_LPARAM(lParam);
	m_y = GET_Y_LPARAM(lParam);

	setDropdownPosition();

	return 0;
}
int TextWindow::wm_setfocus(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);

	log("Log::TextWindow::wm_setfocus()");

	//Brings application window to top when TextWindow gets focus.
	//if (m_system->getApplication()->getActive() == WA_INACTIVE) {
		//::ShowWindow(m_system->getApplication()->getHWND(), SW_SHOWNA);
	//}
	//::SetWindowPos(m_system->getApplication()->getHWND(), m_hWnd, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);

	//Kills focus for CommandLine if CommandLine is visible.
	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandInput != nullptr) {
			if (m_lpCommandInput->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpCommandInput->getHWND()) == TRUE) {
					::SendMessage(m_lpCommandInput->getHWND(), WM_KILLFOCUS, (WPARAM)m_hWnd, 0);
				}
			}
		}
	}

	//Hides DynamicInput if DynamicInput is visible.
	if (DynamicInput::s_nVisible == TRUE) {
		if (m_lpDynamicInput != nullptr) {
			if (m_lpDynamicInput->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpDynamicInput->getHWND()) == TRUE) {
					::ShowWindow(m_lpDynamicInput->getHWND(), SW_HIDE);
				}
			}
		}
	}

	SetFocus(m_hCommand);

	return 0;
}
int TextWindow::wm_size(WPARAM wParam, LPARAM lParam)
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

	::SetWindowPos(m_hStatic,
		NULL,
		0,
		m_cy - COMMANDHEIGHT + 2,
		STATICWIDTH,
		COMMANDHEIGHT - 2,
		SWP_SHOWWINDOW
	);

	::SetWindowPos(m_hCommand,
		NULL,
		STATICWIDTH,
		m_cy - COMMANDHEIGHT + 2,
		m_cx - STATICWIDTH,
		COMMANDHEIGHT - 2,
		SWP_SHOWWINDOW
	);

	setDropdownPosition();

	return 0;
}
int TextWindow::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	log("Log::TextWindow::wm_syscommand()");

	int nID = LOWORD(wParam);
	switch (nID) {
		case SC_CLOSE: {
			log("Log::TextWindow::wm_syscommand() SC_CLOSE");
			s_nVisible = FALSE;
			break;
		}
		case SC_MAXIMIZE: {
			log("Log::TextWindow::wm_syscommand() SC_MAXIMIZE");
			break;
		}
		case SC_MINIMIZE: {
			log("Log::TextWindow::wm_syscommand() SC_MINIMIZE");
			break;
		}
		case SC_RESTORE: {
			log("Log::TextWindow::wm_syscommand() SC_RESTORE");
			break;
		}
		default: {
			log("Log::TextWindow::wm_syscommand() default");
			//::SetFocus(m_hWnd);
			break;
		}
	}

	return 0;
}
int TextWindow::wm_timer(WPARAM wParam, LPARAM lParam)
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

void TextWindow::wm_command_change()
{
	int nResult{ -1 };

	if (m_bIsIndirectTextInput == true) return;

	char chInputBuf[MAX_LOADSTRING]{ 0 };

	int cch = (int)::SendMessage(m_hCommand, WM_GETTEXTLENGTH, 0, 0);
	if (cch == 0) {
		::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);
		m_lpCommandInput->setText("\0");
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

	log("Log::TextWindow::wm_command_change() chInputBuf = %s", chInputBuf);

	::SetFocus(m_hCommand);
}
void TextWindow::wm_timer_lowercasetouppercasedelay()
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

	//Copy text to CommandLine
	m_lpCommandInput->setText(chInputBuf);
}

void TextWindow::setDropdownPosition()
{
	RECT rcPosition = { 0,0,0,0 };
	::GetWindowRect(m_hWnd, &rcPosition);

	int nTop = rcPosition.bottom + DropdownWindow::s_szOffset.cy;//Default dropdown window position = below CommandLine edit control
	int nHeight = m_nDropdownLines * DropdownWindow::s_szSize.cy + 2;
	if (nTop + nHeight > m_nScreenHeight) {
		nTop = rcPosition.bottom - nHeight - COMMANDHEIGHT;
	}

	::SetWindowPos(m_lpDropdownWindow->getHWND(),
		NULL,
		rcPosition.left + 8 + STATICWIDTH,
		nTop - 8,
		0,
		0,
		SWP_NOACTIVATE | SWP_NOSIZE
	);
}

void TextWindow::createOptions()
{
	DynamicInputWindow* m_lpCommandWindows[3] = { (DynamicInputWindow*)this, m_lpCommandInput, m_lpDynamicInput };
	dlg_Options* lpOptions = new dlg_Options(&CommandManager::s_mCommandByID.at(IDC_OPTIONS), m_lpCommandWindows, 1, CommandMode::NONE);//Default tab is Display(1).
	if (m_system->createDialog(lpOptions, IDD_OPTIONS, m_system->getApplication()->getHWND()) == IDOK) {
		log("Log::dlg_DraftingSettings::createOptions() IDOK");
	}

	if (lpOptions) {
		delete lpOptions;
		lpOptions = NULL;
	}
}
