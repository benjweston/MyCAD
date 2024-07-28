#include "SYS_systemwin32.h"

#include "EWD_listboxwindowsubclass.h"
#include "EWD_dynamicinputwindow.h"
#include "EWD_resource.h"

#include "DOC_document.h"

#include "CMD_commandmanager.h"

#include "fault.h"
#include "log.h"

#include <intsafe.h>//Required for IntPtrToInt
#include <windowsx.h>

#define MAKEPOINT(lParam) POINT({ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) });

using namespace mycad;

CommandLineInputBase::CommandLineInputBase()
	: WindowWin32(),
	m_clrText(RGB(0, 0, 0)),
	m_clrBackground(RGB(138, 138, 138)),
	//m_clrBackground(RGB(204, 204, 204)),
	m_clrBorder(RGB(0, 0, 0)),
	m_clrTextBackground(RGB(138, 138, 138)),
	//m_clrTextBackground(RGB(204, 204, 204)),
	m_hbrBackground(::CreateSolidBrush(m_clrBackground)),
	m_hbrTextBackground(::CreateSolidBrush(m_clrTextBackground))
{
	log("Log::CommandLineInputBase::CommandLineInputBase()");
	m_hbrBackground = ::CreateSolidBrush(m_clrBackground);
	m_nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 30;
}
CommandLineInputBase::~CommandLineInputBase()
{
	log("Log::CommandLineInputBase::~CommandLineInputBase()");
}

void CommandLineInputBase::setBackgroundColour(COLORREF colour) { m_clrBackground = colour; m_hbrBackground = ::CreateSolidBrush(colour); }
void CommandLineInputBase::setBorderColour(COLORREF colour) { m_clrBorder = colour; }
void CommandLineInputBase::setTextColour(COLORREF colour) { m_clrText = colour; }
void CommandLineInputBase::setTextBackgroundColour(COLORREF colour) { m_clrTextBackground = colour; m_hbrTextBackground = ::CreateSolidBrush(colour); }



SIZE DropdownWindow::s_szSize{ 206,15 };
SIZE DropdownWindow::s_szOffset{ 0,0 };

DropdownWindow::DropdownWindow(UINT uListboxId)
	: CommandLineInputBase(),
	m_uListboxId(uListboxId),
	m_lpListboxSubclassWindow(0),
	m_hListbox(0)
{
	log("Log::DropdownWindow::DropdownWindow()");
}
DropdownWindow::~DropdownWindow()
{
	log("Log::DropdownWindow::~DropdownWindow()");
}

int DropdownWindow::getListboxID() { return m_uListboxId; }
HWND DropdownWindow::getListboxHWND() { return m_hListbox; }

int DropdownWindow::wm_close()
{
	::DestroyWindow(m_hListbox);

	delete m_lpListboxSubclassWindow;
	m_lpListboxSubclassWindow = nullptr;

	return 0;
}
int DropdownWindow::wm_command(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nSource = LOWORD(wParam);
	int nID = HIWORD(wParam);
	//HWND hwndControl = (HWND)lParam;

	switch (nID) {
		case LBN_SELCHANGE: {
			//log("Log::DropdownWindow::wm_command() LBN_SELCHANGE source = %d", nSource);
			wm_command_lbn_selchange(nSource);
			::ShowWindow(m_hWnd, SW_HIDE);
			break;
		}
		default: {
			break;
		}
	}

	return 0;
}
int DropdownWindow::wm_contextmenu(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	HMENU hMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_DROPDOWN_CONTEXTMENU));
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
		case ID_DROPDOWN_CONTEXTMENU_AUTOCOMPLETE: {

			break;
		}
		case ID_DROPDOWN_CONTEXTMENU_AUTOCORRECT: {

			break;
		}
		case ID_DROPDOWN_CONTEXTMENU_SEARCHSYSTEMVARIABLES: {

			break;
		}
		case ID_DROPDOWN_CONTEXTMENU_MIDSTRINGSEARCH: {

			break;
		}
		case ID_DROPDOWN_CONTEXTMENU_DELAYTIME: {

			break;
		}
		default: {
			break;
		}
	}

	DestroyMenu(hMenu);

	return 0;
}
int DropdownWindow::wm_create(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	HRESULT hResult = S_OK;

	m_clrBackground = RGB(255, 255, 255);
	m_hbrBackground = ::CreateSolidBrush(m_clrBackground);

	RECT rcClient{ 0,0,0,0 };
	::GetClientRect(m_hWnd, &rcClient);
	m_hListbox = ::CreateWindowEx(
		WS_EX_NOPARENTNOTIFY,
		WC_LISTBOX,
		NULL,
		WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | LBS_SORT,
		rcClient.left,
		rcClient.top,
		rcClient.right,
		rcClient.bottom,
		m_hWnd,
		(HMENU)(UINT_PTR)m_uListboxId,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hListbox == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)

		m_lpListboxSubclassWindow = new ListboxWindowSubclass(m_hListbox, s_hInstance);
		setFont(m_hListbox);
		addListboxItems();

#pragma warning( pop )
	}

	return 0;
}
int DropdownWindow::wm_ctlcolorlistbox(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	HRESULT hResult = S_OK;
	INT nResult = 0;

	HDC hDC = (HDC)wParam;

	::SetTextColor(hDC, m_clrText);
	::SetBkColor(hDC, m_clrBackground);
	::SetDCBrushColor(hDC, m_clrBackground);
	::SelectObject(hDC, m_hbrBackground);

	INT_PTR npBrush = (INT_PTR)m_hbrBackground;

	hResult = ::IntPtrToInt(npBrush, &nResult);
	if (hResult == S_OK) {
		return nResult;
	}

	return 0;
}
int DropdownWindow::wm_destroy()
{
	::DeleteObject(m_hbrBackground);

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

void DropdownWindow::wm_command_lbn_selchange(int controlid)
{
	int nIndex = (int)SendMessage(m_hListbox, LB_GETCURSEL, 0, 0);
	if (nIndex == -1) {
		ErrorHandler();
		return;
	}

	int nID = (int)SendMessage(m_hListbox, LB_GETITEMDATA, (WPARAM)nIndex, 0);
	if (nID == -1) {
		ErrorHandler();
		return;
	}
	
	char chInputBuf[MAX_LOADSTRING] = { 0 };
	//::strcpy_s(chInputBuf, CommandManager::s_mCommandByID.at(nID).szAlias);
	::strcpy_s(chInputBuf, CommandManager::s_mCommandByID.at(nID).szCommandDisplayName);

	Document* lpActiveDocument = (Document*)m_system->getDocumentManager().getActiveObject();
	if (lpActiveDocument == nullptr) {
		return;
	}

	HWND hWnd = lpActiveDocument->getHWND();
	if (hWnd == NULL) {
		return;
	}

	SendCommandText(hWnd, controlid, chInputBuf);
}

void DropdownWindow::getDisplayText(std::vector<COMMANDINFO>::iterator it, char &displaytext)
{
	char* szCommandDisplayName = it->szCommandDisplayName;
	size_t cchCommandDisplayName = strlen(szCommandDisplayName);

	if (cchCommandDisplayName == 0) return;

	char* szAlias = it->szAlias;
	size_t cchAlias = strlen(szAlias);

	char chDisplayText[MAX_LOADSTRING] = { 0 };
	if (cchAlias > 0) {
		::strcpy_s(chDisplayText, szAlias);
		::strcat_s(chDisplayText, MAX_LOADSTRING, " (");
		::strcat_s(chDisplayText, MAX_LOADSTRING, szCommandDisplayName);
		::strcat_s(chDisplayText, MAX_LOADSTRING, ")");
	}
	else {
		::strcpy_s(chDisplayText, szCommandDisplayName);
	}

	::strcpy_s(&displaytext, MAX_LOADSTRING, chDisplayText);
}
HRESULT DropdownWindow::addListboxItems()
{
	HRESULT hResult = S_OK;
	LRESULT lResult{ LB_ERR };

	std::vector<COMMANDINFO>::iterator it;
	for (it = CommandManager::s_vecCommands.begin(); it != CommandManager::s_vecCommands.end(); it++) {
		char chDisplayText[MAX_LOADSTRING] = { 0 };
		getDisplayText(it, *chDisplayText);//Get display test from getDisplayText() function...
		size_t cchDisplayText = strlen(chDisplayText);

		if (cchDisplayText > 0) {
			lResult = ::SendMessage(m_hListbox, LB_ADDSTRING, 0, (LPARAM)chDisplayText);//..., add string to listbox...
			if (lResult == LB_ERR) {
				hResult = ErrorHandler();
			}
			else {
				//log("Log::DropdownWindow::addListboxItems() %s", chDisplayText);
				int nPos = (int)lResult;
				int nID = it->nID;
				lResult = ::SendMessage(m_hListbox, LB_SETITEMDATA, (WPARAM)nPos, (LPARAM)nID);///...and set the ID value to be associated with the display string.
				if (lResult == LB_ERR) {
					hResult = ErrorHandler();
				}
			}
		}
	}

	return hResult;
}



UINT DynamicInputWindow::s_nInputSearchDelay{ 300 };
UINT DynamicInputWindow::s_nLowercaseToUppercaseDelay{ 300 };

DynamicInputWindow::DynamicInputWindow(int dropdownlines)
	: CommandLineInputBase(),
	m_hCommand(0),
	m_nDropdownLines(dropdownlines),
	m_lpDropdownWindow(nullptr),
	m_clrBackground_Command(RGB(233, 233, 233)),
	m_hbrBackground_Command(0),
	m_szCueBanner("\0"),
	m_bLowercaseToUppercaseDelay(false),
	m_bIsIndirectTextInput(false)
{
	log("Log::DynamicInputWindow::DynamicInputWindow()");
	m_hbrBackground = ::CreateSolidBrush(m_clrBackground);
}
DynamicInputWindow::~DynamicInputWindow()
{
	log("Log::DynamicInputWindow::~DynamicInputWindow()");
}

void DynamicInputWindow::setDropdownLines(int lines) { m_nDropdownLines = lines; }

BOOL DynamicInputWindow::setText(const char* text)
{
	LRESULT lResult = FALSE;

	m_bIsIndirectTextInput = true;//Set m_bIsIndirectTextInput to true. This allows us to set the text without triggering any of the events associated with a change in command text.
	lResult = (LRESULT)::SendMessage(m_hCommand, WM_SETTEXT, (WPARAM)0, (LPARAM)text);
	if (lResult == FALSE) {
		log("Log::DynamicInputWindow::setText(%s) == FALSE", text);
	}
	m_bIsIndirectTextInput = false;//Return m_bIsIndirectTextInput value to false.

	return (BOOL)lResult;
}

int DynamicInputWindow::wm_close()
{
	BOOL nTimer = ::KillTimer(m_hWnd, IDT_LOWERCASETOUPPERCASEDELAY);
	if (nTimer == 0) {
		ErrorHandler();
	}

	if (::IsWindowVisible(m_lpDropdownWindow->getHWND()) == TRUE) {
		::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);
	}

	::DestroyWindow(m_lpDropdownWindow->getHWND());
	::DestroyWindow(m_hCommand);

	return 0;
}
int DynamicInputWindow::wm_killfocus(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);

	::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);

	return 0;
}
int DynamicInputWindow::wm_notify(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

#define lpnmhdr (LPNMHDR(lParam))

	switch (lpnmhdr->code) {
		case TCN_SELCHANGE: {
			log("Log::DynamicInputWindow::wm_notify() TCN_SELCHANGE");
			Document* lpActiveDocument = (Document*)m_system->getDocumentManager().getActiveObject();
			if (lpActiveDocument == nullptr) {
				ErrorHandler();
				return 0;
			}
		}
		default: {
			break;
		}
	}

	return 0;
}
int DynamicInputWindow::wm_setcursor(WPARAM wParam, LPARAM lParam)//Set cursor and return FALSE for default processing.
{
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	return (int)::DefWindowProc(m_hWnd, WM_SETCURSOR, wParam, lParam);
}
int DynamicInputWindow::wm_timer(WPARAM wParam, LPARAM lParam)
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

int DynamicInputWindow::createDropdownWindow(RECT position, UINT uListboxId)
{
	m_lpDropdownWindow = new DropdownWindow(uListboxId);

	char szDropdownClassName[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, ID_DROPDOWN_CLASS, szDropdownClassName, sizeof(szDropdownClassName) / sizeof(char));

	m_system->createWindow(
		szDropdownClassName,
		NULL,
		(WindowWin32*)m_lpDropdownWindow,
		WS_EX_TOPMOST | WS_EX_NOACTIVATE,
		WS_POPUP | WS_BORDER,
		position.left,
		position.top,
		position.right,
		position.bottom,
		m_hWnd,
		NULL,
		SW_HIDE
	);

	if (m_lpDropdownWindow->getValid() == TRUE) {
		log("DynamicInput::createDropdownWindow(): window valid");
	}
	else {
		log("DynamicInput::createDropdownWindow(): window invalid");
		delete m_lpDropdownWindow;
		m_lpDropdownWindow = NULL;
		return 0;
	}

	return IDOK;
}

void DynamicInputWindow::wm_command_escape(int controlid)
{
	Document* lpActiveDocument = (Document*)m_system->getDocumentManager().getActiveObject();
	if (lpActiveDocument == nullptr) {
		return;
	}

	HWND hWnd = lpActiveDocument->getHWND();
	if (hWnd == NULL) {
		return;
	}

	SendCommandParam(hWnd, controlid, IDC_CANCEL);
}
void DynamicInputWindow::wm_command_return(int controlid)
{
	char chInputBuf[MAX_LOADSTRING]{ 0 };

	if (::IsWindowVisible(m_lpDropdownWindow->getHWND()) == TRUE) {
		::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);
	}

	int cch = (int)::SendMessage(m_hCommand, WM_GETTEXTLENGTH, 0, 0);//Get text length in characters...
	cch++;//..., increment by 1 to include the terminating null character...
	int nResult = (int)::SendMessage(m_hCommand, WM_GETTEXT, (WPARAM)cch, (LPARAM)&chInputBuf);//...and get text in buffer.

	if (nResult != cch - 1) return;

		Document* lpActiveDocument = (Document*)m_system->getDocumentManager().getActiveObject();
	if (lpActiveDocument == nullptr) {
		return;
	}

	HWND hWnd = lpActiveDocument->getHWND();
	if (hWnd == NULL) {
		return;
	}

	SendCommandText(hWnd, controlid, chInputBuf);
}

void DynamicInputWindow::wm_timer_inputsearchdelay()
{
	if (::IsWindowVisible(m_lpDropdownWindow->getHWND()) == FALSE) {
		::ShowWindow(m_lpDropdownWindow->getHWND(), SW_SHOWNA);
	}

	BOOL nTimer = ::KillTimer(m_hWnd, IDT_INPUTSEARCHDELAY);
	if (nTimer == 0) {
		ErrorHandler();
	}
}
void DynamicInputWindow::wm_timer_lowercasetouppercasedelay()
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
}
