#include "SYS_systemwin32.h"

#include "DOC_document.h"

#include "CMD_commands.h"

#include "EDL_resource.h"
#include "EPL_resource.h"

#include "EWD_textwindow.h"
#include "EWD_commandline.h"
#include "EWD_dynamicinput.h"
#include "EWD_resource.h"

#include "GUI_glcontrol.h"
#include "GUI_glselection.h"

#include "resources.h"

#include "reg_util.h"

#include "fault.h"
#include "log.h"

#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.
#include <windowsx.h>

using namespace mycad;

#define MAKEPOINT(lParam) POINT({ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) });

CommandMode Command::m_commandMode = CommandMode::BOTH;

Command::Command()
	: m_lpCommandInfo(nullptr),
	m_lpTextWindow(nullptr),
	m_lpCommandLine(nullptr),
	m_lpDynamicInput(nullptr),
	m_nStep(0),
	m_bShortcutMenuLoopActive(false),
	m_ptMouseDown({ 0,0 }),
	m_ptMouseUp({ 0,0 }),
	m_ptMouseMove({ 0,0 }),
	m_c3fColour({})
{}
Command::Command(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: m_lpCommandInfo(command),
	m_lpTextWindow((TextWindow*)commandwindows[0]),
	m_lpCommandLine((CommandLine*)commandwindows[1]),
	m_lpDynamicInput((DynamicInput*)commandwindows[2]),
	m_nStep(0),
	m_bShortcutMenuLoopActive(false),
	m_ptMouseDown({ 0,0 }),
	m_ptMouseUp({ 0,0 }),
	m_ptMouseMove({ 0,0 })
{
	if ((mode == CommandMode::OUTPUT) || (mode == CommandMode::BOTH)) {
		initCommandLine();
		initTextWindow();
	}
	initDynamicInput();
}
Command::Command(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: m_lpCommandInfo(command),
	m_lpTextWindow((TextWindow*)commandwindows[0]),
	m_lpCommandLine((CommandLine*)commandwindows[1]),
	m_lpDynamicInput((DynamicInput*)commandwindows[2]),
	m_nStep(0),
	m_bShortcutMenuLoopActive(false),
	m_ptMouseDown({ 0,0 }),
	m_ptMouseUp({ 0,0 }),
	m_ptMouseMove({ 0,0 })
{
	if ((mode == CommandMode::OUTPUT) || (mode == CommandMode::BOTH)) {
		initCommandLine(text);
		initTextWindow(text);
	}
	initDynamicInput();
}
Command::Command(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, CommandMode mode)
	: m_lpCommandInfo(command),
	m_lpTextWindow((TextWindow*)commandwindows[0]),
	m_lpCommandLine((CommandLine*)commandwindows[1]),
	m_lpDynamicInput((DynamicInput*)commandwindows[2]),
	m_nStep(0),
	m_bShortcutMenuLoopActive(false),
	m_ptMouseDown(position),
	m_ptMouseUp(position),
	m_ptMouseMove(position)
{
	if ((mode == CommandMode::OUTPUT) || (mode == CommandMode::BOTH)) {
		initCommandLine();
		initTextWindow();
	}
	initDynamicInput();
}
Command::Command(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, const char* text, CommandMode mode)
	: m_lpCommandInfo(command),
	m_lpTextWindow((TextWindow*)commandwindows[0]),
	m_lpCommandLine((CommandLine*)commandwindows[1]),
	m_lpDynamicInput((DynamicInput*)commandwindows[2]),
	m_nStep(0),
	m_bShortcutMenuLoopActive(false),
	m_ptMouseDown(position),
	m_ptMouseUp(position),
	m_ptMouseMove(position)
{
	if ((mode == CommandMode::OUTPUT) || (mode == CommandMode::BOTH)) {
		initCommandLine(text);
		initTextWindow(text);
	}
	initDynamicInput();
}
Command::~Command()
{

}

void Command::setMode(CommandMode mode) {
	if (static_cast<int>(mode) > static_cast<int>(CommandMode::BOTH)) return;
	m_commandMode = mode;
}
CommandMode Command::getMode() { return m_commandMode; }

void Command::setSystemVariable(int variableid, int value)
{
	char chPath[MAX_LOADSTRING]{ 0 };
	char chCompanyName[MAX_LOADSTRING]{ 0 };
	char chApplicationName[MAX_LOADSTRING]{ 0 };
	char chRelease[MAX_LOADSTRING]{ 0 };

	::LoadString(NULL, ID_COMPANY_NAME, chCompanyName, MAX_LOADSTRING);
	::LoadString(NULL, ID_APPLICATION_NAME, chApplicationName, MAX_LOADSTRING);
	::LoadString(NULL, ID_APPLICATION_RELEASE, chRelease, MAX_LOADSTRING);

	_tcscpy_s(chPath, STRING_SIZE(chPath), "Software\\");
	_tcscat_s(chPath, STRING_SIZE(chPath), chCompanyName);
	_tcscat_s(chPath, STRING_SIZE(chPath), "\\");
	_tcscat_s(chPath, STRING_SIZE(chPath), chApplicationName);
	_tcscat_s(chPath, STRING_SIZE(chPath), "\\");
	_tcscat_s(chPath, STRING_SIZE(chPath), chRelease);
	_tcscat_s(chPath, STRING_SIZE(chPath), "\\Variables\\");

	char* szVariable = CommandManager::s_mCommandByID.at(variableid).szCommandDisplayName;
	_tcscat_s(chPath, STRING_SIZE(chPath), szVariable);

	log("Log::Command::setSystemVariable() %s, %d", szVariable, value);

	int nValue = 0x00001000 + value;
	bool bResult = SetUserValue(chPath, "Value", REG_DWORD, &nValue, sizeof(int));
	if (bResult == false) {
		ErrorHandler();
	}
}
int Command::getSystemVariable(int variableid)
{
	char chPath[MAX_LOADSTRING]{ 0 };
	char chCompanyName[MAX_LOADSTRING]{ 0 };
	char chApplicationName[MAX_LOADSTRING]{ 0 };
	char chRelease[MAX_LOADSTRING]{ 0 };

	::LoadString(NULL, ID_COMPANY_NAME, chCompanyName, MAX_LOADSTRING);
	::LoadString(NULL, ID_APPLICATION_NAME, chApplicationName, MAX_LOADSTRING);
	::LoadString(NULL, ID_APPLICATION_RELEASE, chRelease, MAX_LOADSTRING);

	_tcscpy_s(chPath, STRING_SIZE(chPath), "Software\\");
	_tcscat_s(chPath, STRING_SIZE(chPath), chCompanyName);
	_tcscat_s(chPath, STRING_SIZE(chPath), "\\");
	_tcscat_s(chPath, STRING_SIZE(chPath), chApplicationName);
	_tcscat_s(chPath, STRING_SIZE(chPath), "\\");
	_tcscat_s(chPath, STRING_SIZE(chPath), chRelease);
	_tcscat_s(chPath, STRING_SIZE(chPath), "\\Variables\\");

	char* szVariable = CommandManager::s_mCommandByID.at(variableid).szCommandDisplayName;
	_tcscat_s(chPath, STRING_SIZE(chPath), szVariable);

	int nValue = 0;
	bool bResult = GetUserValue(chPath, "Value", REG_DWORD, &nValue, sizeof(int));
	if (bResult == false) {
		ErrorHandler();
	}

	nValue = nValue - 0x00001000;

	log("Log::Command::getSystemVariable() %s, %d", szVariable, nValue);

	return nValue;
}

int Command::getID() const { return m_lpCommandInfo->nID; }

void Command::draw(POINT position, Colour3<float> colour)
{
	UNREFERENCED_PARAMETER(position);
	UNREFERENCED_PARAMETER(colour);
}

void Command::setText(const char* text)
{
	m_lpCommandLine->setText(text);
}

void Command::initTextWindow()
{
	if (TextWindow::s_nVisible == TRUE) {
		if (m_lpTextWindow != nullptr) {
			if (m_lpTextWindow->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
					m_lpTextWindow->setText(m_lpCommandInfo->szMacro);

					m_lpTextWindow->addString("\r\n");

					char szString[MAX_LOADSTRING];
					::strcpy_s(szString, "Command: ");
					::strcat_s(szString, MAX_LOADSTRING, m_lpCommandInfo->szMacro);
					m_lpTextWindow->addString(szString);

					m_lpTextWindow->setText("");
					m_lpTextWindow->setStaticText("");
				}
			}
		}
	}
}
void Command::initCommandLine()
{
	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandLine != nullptr) {
			if (m_lpCommandLine->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
					m_lpCommandLine->setText(m_lpCommandInfo->szMacro);

					m_lpCommandLine->addString("\r\n");

					char szString[MAX_LOADSTRING]{ 0 };
					::strcpy_s(szString, "Command: ");
					::strcat_s(szString, MAX_LOADSTRING, m_lpCommandInfo->szMacro);
					m_lpCommandLine->addString(szString);

					m_lpCommandLine->setText(m_lpCommandInfo->szCommandDisplayName);
				}
			}
		}
	}
}

void Command::initTextWindow(const char* text)
{
	if (TextWindow::s_nVisible == TRUE) {
		if (m_lpTextWindow != nullptr) {
			if (m_lpTextWindow->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
					m_lpTextWindow->setText(text);

					m_lpTextWindow->setText(m_lpCommandInfo->szCommandDisplayName);

					m_lpTextWindow->addString("\r\n");

					char szString[MAX_LOADSTRING];
					::strcpy_s(szString, "Command: ");
					::strcat_s(szString, MAX_LOADSTRING, text);
					m_lpTextWindow->addString(szString);

					m_lpTextWindow->addString("\r\n");

					m_lpTextWindow->addString(m_lpCommandInfo->szCommandDisplayName);

					m_lpTextWindow->setStaticText("");
					m_lpTextWindow->setText("");
				}
			}
		}
	}
}
void Command::initCommandLine(const char* text)
{
	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandLine != nullptr) {
			if (m_lpCommandLine->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
					m_lpCommandLine->setText(m_lpCommandInfo->szCommandDisplayName);

					m_lpCommandLine->addString("\r\n");

					char szString[MAX_LOADSTRING]{ 0 };
					::strcpy_s(szString, "Command: ");
					::strcat_s(szString, MAX_LOADSTRING, text);
					m_lpCommandLine->addString(szString);

					m_lpCommandLine->addString("\r\n");

					m_lpCommandLine->addString(m_lpCommandInfo->szCommandDisplayName);
				}
			}
		}
	}
}

void Command::initDynamicInput()
{
	if (DynamicInput::s_nVisible == TRUE) {
		if (m_lpDynamicInput != nullptr) {
			if (m_lpDynamicInput->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpDynamicInput->getHWND()) == TRUE) {
					::ShowWindow(m_lpDynamicInput->getHWND(), SW_HIDE);
				}
			}
		}
	}
}



CommandWindow::CommandWindow()
	: Command(),
	m_lpGLControl(nullptr)
{

}
CommandWindow::CommandWindow(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: Command(command, commandwindows, mode),
	m_lpGLControl(nullptr)
{

}
CommandWindow::CommandWindow(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: Command(command, commandwindows, text, mode),
	m_lpGLControl(nullptr)
{

}
CommandWindow::CommandWindow(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, CommandMode mode)
	: Command(command, commandwindows, position, mode),
	m_lpGLControl(nullptr)
{

}
CommandWindow::CommandWindow(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, const char* text, CommandMode mode)
	: Command(command, commandwindows, position, text, mode),
	m_lpGLControl(nullptr)
{

}
CommandWindow::~CommandWindow()
{
	delete m_lpGLControl;
	m_lpGLControl = NULL;
}

int CommandWindow::wm_char(WPARAM wParam, LPARAM lParam)//Returns 1 while command sequence is active, 0 when the command sequence is finished.
{
	UNREFERENCED_PARAMETER(lParam);

	int nResult = 0;
	WORD vkCode = LOWORD(wParam);

	log("Log::Command::wm_char() vkCode = %d", vkCode);

	switch (vkCode) {
	case VK_RETURN: {
		nResult = wm_char_return();
		break;
	}
	case VK_ESCAPE: {
		nResult = wm_char_escape();
		break;
	}
	default: {
		break;
	}
	}

	return nResult;
}
int CommandWindow::wm_close()
{
	log("Log::Command::wm_close()");

	::DestroyWindow(m_hWnd);

	return 0;
}
int CommandWindow::wm_command(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int CommandWindow::wm_contextmenu(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int CommandWindow::wm_destroy()
{
	log("Log::Command::wm_destroy()");

	return 0;
}
int CommandWindow::wm_entermenuloop(WPARAM wParam)
{
	BOOL nTrackkPopupMenu = (BOOL)wParam;

	if (nTrackkPopupMenu == TRUE) {
		m_bShortcutMenuLoopActive = true;
	}

	return 0;
}
int CommandWindow::wm_exitmenuloop(WPARAM wParam)
{
	BOOL nTrackkPopupMenu = (BOOL)wParam;

	if (nTrackkPopupMenu == TRUE) {
		m_bShortcutMenuLoopActive = false;
	}

	return 0;
}
int CommandWindow::wm_keydown(WPARAM wParam, LPARAM lParam)//Returns 1 while command sequence is active, 0 when the command sequence is finished.
{
	int nResult = 0;

	WORD vkCode = LOWORD(wParam);									// Virtual-key code
	WORD keyFlags = HIWORD(lParam);									// Keystroke message flags

	WORD scanCode = LOBYTE(keyFlags);								// scan code
	BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;	// extended-key 

	if (isExtendedKey) {
		scanCode = MAKEWORD(scanCode, 0xE0);
	}

	BOOL wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT;			// previous key-state flag, 1 on autorepeat
	WORD repeatCount = LOWORD(lParam);								// repeat count, > 0 if several keydown messages was combined into one message

	BOOL isKeyReleased = (keyFlags & KF_UP) == KF_UP;				// transition-state flag, 1 on keyup

	UNREFERENCED_PARAMETER(wasKeyDown);
	UNREFERENCED_PARAMETER(repeatCount);
	UNREFERENCED_PARAMETER(isKeyReleased);

	log("Log::Command::wm_keydown() vkCode = %d", vkCode);

	switch (vkCode) {
	case VK_LEFT: {
		nResult = 0;
		break;
	}
	case VK_RIGHT: {
		nResult = 0;
		break;
	}
	case VK_END: {
		nResult = 0;
		break;
	}
	default: {
		break;
	}
	}

	return nResult;
}
int CommandWindow::wm_lbuttondblclk(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int CommandWindow::wm_lbuttondown(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_ptMouseDown = MAKEPOINT(lParam);
	m_ptMouseMove = m_ptMouseDown;

	return 0;
}
int CommandWindow::wm_lbuttonup(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_ptMouseUp = MAKEPOINT(lParam);

	return 0;
}
int CommandWindow::wm_mouseleave()
{
	return 0;
}
int CommandWindow::wm_mousemove(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_ptMouseMove = MAKEPOINT(lParam);

	return 0;
}
int CommandWindow::wm_mousewheel(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int CommandWindow::wm_notify(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int CommandWindow::wm_paint()
{
	if (m_lpGLControl == nullptr) {
		return 0;
	}
	else {
		m_lpGLControl->draw(m_ptMouseMove, m_c3fColour);
	}
	return 0;
}

int CommandWindow::wm_char_return()
{
	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandLine->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
				m_lpCommandLine->setText("");
			}
		}
	}

	if (TextWindow::s_nVisible == TRUE) {
		if (m_lpTextWindow->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
				m_lpTextWindow->addString("\r\n");
				m_lpTextWindow->setStaticText("Command:");
				m_lpTextWindow->setText("");
			}
		}
	}

	return 0;
}
int CommandWindow::wm_char_escape()
{
	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandLine->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
				//if (m_lpCommandInfo->enContext == FALSE) {
					m_lpCommandLine->addString("\r\n");
					m_lpCommandLine->addString("Command: *Cancel*");
				//}
				m_lpCommandLine->setText("");
			}
		}
	}

	if (TextWindow::s_nVisible == TRUE) {
		if (m_lpTextWindow->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
				//if (m_lpCommandInfo->enContext == FALSE) {
					m_lpTextWindow->addString("\r\n");
					m_lpTextWindow->addString("Command: *Cancel*");
				//}
				m_lpTextWindow->setStaticText("Command:");
				m_lpTextWindow->setText("");
			}
		}
	}

	return 0;
}
int CommandWindow::wm_char_back() { return 0; }
int CommandWindow::wm_char_default(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}



CommandDialog::CommandDialog() : Command() {}
CommandDialog::CommandDialog(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: Command(command, commandwindows, mode)
{

}
CommandDialog::CommandDialog(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: Command(command, commandwindows, text, mode)
{

}
CommandDialog::CommandDialog(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, CommandMode mode)
	: Command(command, commandwindows, position, mode)
{

}
CommandDialog::CommandDialog(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, const char* text, CommandMode mode)
	: Command(command, commandwindows, position, text, mode)
{

}
CommandDialog::~CommandDialog()
{

}



CViewBase::CViewBase(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: CommandWindow(command, commandwindows, mode)
{
	if ((mode == CommandMode::OUTPUT) || (mode == CommandMode::BOTH)) {
		initCommandLine();
		initTextWindow();
	}
}
CViewBase::CViewBase(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: CommandWindow(command, commandwindows, text, mode)
{
	if ((mode == CommandMode::OUTPUT) || (mode == CommandMode::BOTH)) {
		initCommandLine();
		initTextWindow();
	}
}

void CViewBase::initCommandLine()
{
	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandLine->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
				if (m_lpCommandInfo->vecPrompts.size() > 0) {
					if (m_lpCommandInfo->vecPrompts[0].size() > 0) {
						const char* lpszString = m_lpCommandInfo->vecPrompts[0].c_str();
						m_lpCommandLine->addString("\r\n");
						m_lpCommandLine->addString(lpszString);
					}
				}
			}
		}
	}
}
void CViewBase::initTextWindow()
{
	if (TextWindow::s_nVisible == TRUE) {
		if (m_lpTextWindow->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
				if (m_lpCommandInfo->vecPrompts.size() > 0) {
					if (m_lpCommandInfo->vecPrompts[0].size() > 0) {
						const char* lpszString = m_lpCommandInfo->vecPrompts[0].c_str();
						m_lpTextWindow->addString("\r\n");
						m_lpTextWindow->addString(lpszString);
					}
				}
			}
		}
	}
}



CView2D::CView2D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: CViewBase(command, commandwindows, mode) {}
CView2D::CView2D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: CViewBase(command, commandwindows, text, mode) {}

int CView2D::wm_contextmenu(WPARAM wParam, LPARAM lParam)
{
	HMENU hMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_CONTEXT_MENU_VIEW));
	HMENU hSubMenu = ::GetSubMenu(hMenu, 0);

	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	mii.fState = MFS_CHECKED;

	switch (m_lpCommandInfo->nID) {
		case IDC_ZOOM: {
			BOOL nResult = ::SetMenuItemInfo(hMenu, ID_CONTEXT_VIEW_ZOOM, MF_BYCOMMAND, &mii);
			if (nResult == FALSE) {
				ErrorHandler();
			}
			break;
		}
		case IDC_PAN: {
			BOOL nResult = ::SetMenuItemInfo(hMenu, ID_CONTEXT_VIEW_PAN, MF_BYCOMMAND, &mii);
			if (nResult == FALSE) {
				ErrorHandler();
			}
			break;
		}
		default: {
			break;
		}
	}

	HWND hRenderWindow = (HWND)wParam;
	POINT ptCursor = MAKEPOINT(lParam);
	UINT uReturnCmd = ::TrackPopupMenuEx(
		hSubMenu,
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
		ptCursor.x,
		ptCursor.y,
		hRenderWindow,
		NULL
	);

	::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	Document* lpDocument = m_system->getDocumentManager().getActiveObject();
	if (lpDocument == nullptr) {
		FaultHandler(__FILE__, (unsigned short)__LINE__);
		return 0;
	}
	if (lpDocument->getLayoutManager() == nullptr) {
		ErrorHandler();
		return 0;
	}
	if (lpDocument->getLayoutManager()->getSize() == 0) {
		ErrorHandler();
		return 0;
	}

	HWND hDocument = lpDocument->getHWND();
	if (hDocument == NULL) {
		ErrorHandler();
		return 0;
	}

	switch (uReturnCmd) {
		case ID_CONTEXT_VIEW_EXIT: {
			::SendMessage(hDocument, WM_CHAR, (WPARAM)VK_RETURN, 0);
			break;
		}
		case ID_CONTEXT_VIEW_PAN: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_PAN);
			break;
		}
		case ID_CONTEXT_VIEW_ZOOM: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_ZOOM);
			break;
		}
		case ID_CONTEXT_VIEW_3DORBIT: {
			Layout* lpActiveLayout = lpDocument->getLayoutManager()->getActiveObject();
			if (lpActiveLayout == nullptr) {
				ErrorHandler();
				break;
			}

			Space::Context context = lpActiveLayout->getActiveSpace()->getContext();
			if (context == Space::Context::PaperSpace) {
				::SendMessage(hDocument, WM_CHAR, (WPARAM)VK_ESCAPE, 0);
			}
			else {
				SendCommandParam(hDocument, IDC_COMMAND, IDC_3DORBIT);
			}
			break;
		}
	}

	DestroyMenu(hMenu);

	return 1;
}


CView3D::CView3D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: CViewBase(command, commandwindows, mode) {}
CView3D::CView3D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: CViewBase(command, commandwindows, text, mode) {}

int CView3D::wm_contextmenu(WPARAM wParam, LPARAM lParam)
{
	BOOL nResult = FALSE;

	HMENU hMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_CONTEXT_MENU_ORBIT));
	HMENU hSubMenu = ::GetSubMenu(hMenu, 0);

	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;
	mii.fState = MFS_CHECKED;

	switch (m_lpCommandInfo->nID) {
		case IDC_3DORBIT: {
			::SetMenuItemInfo(hMenu, ID_CONTEXT_ORBIT_CONSTRAINED_ORBIT, MF_BYCOMMAND, &mii);
			break;
		}
		case IDC_3DFORBIT: {
			::SetMenuItemInfo(hMenu, ID_CONTEXT_ORBIT_FREE_ORBIT, MF_BYCOMMAND, &mii);
			break;
		}
		case IDC_3DCORBIT: {
			::SetMenuItemInfo(hMenu, ID_CONTEXT_ORBIT_CONTINUOUS_ORBIT, MF_BYCOMMAND, &mii);
			break;
		}
		case IDC_3DDISTANCE: {
			::SetMenuItemInfo(hMenu, ID_CONTEXT_ORBIT_ADJUST_DISTANCE, MF_BYCOMMAND, &mii);
			break;
		}
		case IDC_3DSWIVEL: {
			::SetMenuItemInfo(hMenu, ID_CONTEXT_ORBIT_SWIVEL, MF_BYCOMMAND, &mii);
			break;
		}
		case IDC_3DWALK: {
			::SetMenuItemInfo(hMenu, ID_CONTEXT_ORBIT_WALK, MF_BYCOMMAND, &mii);
			break;
		}
		case IDC_3DFLY: {
			::SetMenuItemInfo(hMenu, ID_CONTEXT_ORBIT_FLY, MF_BYCOMMAND, &mii);
			break;
		}
		case IDC_ZOOM: {
			::SetMenuItemInfo(hMenu, ID_CONTEXT_ORBIT_ZOOM, MF_BYCOMMAND, &mii);
			break;
		}
		case IDC_PAN: {
			::SetMenuItemInfo(hMenu, ID_CONTEXT_ORBIT_PAN, MF_BYCOMMAND, &mii);
			break;
		}
		default: {
			break;
		}
	}

	Document* lpDocument = m_system->getDocumentManager().getActiveObject();
	if (lpDocument == nullptr) {
		FaultHandler(__FILE__, (unsigned short)__LINE__);
		return 0;
	}
	if (lpDocument->getLayoutManager() == nullptr) {
		ErrorHandler();
		return 0;
	}
	if (lpDocument->getLayoutManager()->getSize() == 0) {
		ErrorHandler();
		return 0;
	}

	Layout* lpActiveLayout = lpDocument->getLayoutManager()->getActiveObject();
	if (lpActiveLayout == nullptr) {
		ErrorHandler();
		return 0;
	}

	bool bVisible = lpActiveLayout->getActiveViewport()->getCamera()->getPerspective();
	if (bVisible == false) {
		mii.fState = MFS_CHECKED;
		nResult = ::SetMenuItemInfo(hSubMenu, ID_CONTEXT_ORBIT_ORTHOGRAPHIC, MF_BYCOMMAND, &mii);
		if (nResult == FALSE) {
			ErrorHandler();
		}
		mii.fState = MFS_UNCHECKED;
		nResult = ::SetMenuItemInfo(hSubMenu, ID_CONTEXT_ORBIT_PERSPECTIVE, MF_BYCOMMAND, &mii);
		if (nResult == FALSE) {
			ErrorHandler();
		}
	}
	else {
		mii.fState = MFS_UNCHECKED;
		nResult = ::SetMenuItemInfo(hSubMenu, ID_CONTEXT_ORBIT_ORTHOGRAPHIC, MF_BYCOMMAND, &mii);
		if (nResult == FALSE) {
			ErrorHandler();
		}
		mii.fState = MFS_CHECKED;
		nResult = ::SetMenuItemInfo(hSubMenu, ID_CONTEXT_ORBIT_PERSPECTIVE, MF_BYCOMMAND, &mii);
		if (nResult == FALSE) {
			ErrorHandler();
		}
	}
	//wm_init_contextmenu_orbit_perspective(hSubMenu);

	BOOL nVisible = lpActiveLayout->getActiveViewport()->getUCSVisible();
	if (nVisible == FALSE) {
		mii.fState = MFS_UNCHECKED;
	}
	else {
		mii.fState = MFS_CHECKED;
	}

	HMENU hSubSubMenu = ::GetSubMenu(hSubMenu, 21);
	::SetMenuItemInfo(hSubSubMenu, ID_CONTEXT_ORBIT_VISUALAIDS_UCSICON, MF_BYCOMMAND, &mii);

	HWND hRenderWindow = (HWND)wParam;
	POINT ptCursor = MAKEPOINT(lParam);
	UINT uReturnCmd = ::TrackPopupMenuEx(
		hSubMenu,
		TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
		ptCursor.x,
		ptCursor.y,
		hRenderWindow,
		NULL
	);

	::SetCursor(::LoadCursor(NULL, IDC_ARROW));

	HWND hDocument = lpDocument->getHWND();
	if (hDocument == NULL) {
		ErrorHandler();
		return 0;
	}

	switch (uReturnCmd) {
		case ID_CONTEXT_ORBIT_EXIT: {
			::SendMessage(hDocument, WM_CHAR, (WPARAM)VK_RETURN, 0);
			break;
		}
		case ID_CONTEXT_ORBIT_CONSTRAINED_ORBIT: {
			SendCommandParam(hDocument, ID_CONTEXT_MENU_ORBIT, IDC_3DORBIT);
			break;
		}
		case ID_CONTEXT_ORBIT_FREE_ORBIT: {
			SendCommandParam(hDocument, ID_CONTEXT_MENU_ORBIT, IDC_3DFORBIT);
			break;
		}
		case ID_CONTEXT_ORBIT_CONTINUOUS_ORBIT: {
			SendCommandParam(hDocument, ID_CONTEXT_MENU_ORBIT, IDC_3DCORBIT);
			break;
		}
		case ID_CONTEXT_ORBIT_ADJUST_DISTANCE: {
			SendCommandParam(hDocument, ID_CONTEXT_MENU_ORBIT, IDC_3DDISTANCE);
			break;
		}
		case ID_CONTEXT_ORBIT_SWIVEL: {
			SendCommandParam(hDocument, ID_CONTEXT_MENU_ORBIT, IDC_3DSWIVEL);
			break;
		}
		case ID_CONTEXT_ORBIT_WALK: {
			SendCommandParam(hDocument, ID_CONTEXT_MENU_ORBIT, IDC_3DWALK);
			break;
		}
		case ID_CONTEXT_ORBIT_FLY: {
			SendCommandParam(hDocument, ID_CONTEXT_MENU_ORBIT, IDC_3DFLY);
			break;
		}
		case ID_CONTEXT_ORBIT_ZOOM: {
			SendCommandParam(hDocument, ID_CONTEXT_MENU_ORBIT, IDC_ZOOM);
			break;
		}
		case ID_CONTEXT_ORBIT_PAN: {
			SendCommandParam(hDocument, ID_CONTEXT_MENU_ORBIT, IDC_PAN);
			break;
		}
		case ID_CONTEXT_ORBIT_ORTHOGRAPHIC: {
			SendCommandParam(m_system->getApplication()->getHWND(), IDC_COMMAND, IDC_SETVAR, MAKELPARAM(IDC_PERSPECTIVE, 0));
			break;
		}
		case ID_CONTEXT_ORBIT_PERSPECTIVE: {
			SendCommandParam(m_system->getApplication()->getHWND(), IDC_COMMAND, IDC_SETVAR, MAKELPARAM(IDC_PERSPECTIVE, 1));
			break;
		}
		case ID_CONTEXT_ORBIT_PRESET_VIEWS_TOP: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC__VIEW, MAKELPARAM(0, (int)View::PresetView::Top));
			break;
		}
		case ID_CONTEXT_ORBIT_PRESET_VIEWS_BOTTOM: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC__VIEW, MAKELPARAM(0, (int)View::PresetView::Bottom));
			break;
		}
		case ID_CONTEXT_ORBIT_PRESET_VIEWS_LEFT: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC__VIEW, MAKELPARAM(0, (int)View::PresetView::Left));
			break;
		}
		case ID_CONTEXT_ORBIT_PRESET_VIEWS_RIGHT: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC__VIEW, MAKELPARAM(0, (int)View::PresetView::Right));
			break;
		}
		case ID_CONTEXT_ORBIT_PRESET_VIEWS_FRONT: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC__VIEW, MAKELPARAM(0, (int)View::PresetView::Front));
			break;
		}
		case ID_CONTEXT_ORBIT_PRESET_VIEWS_BACK: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC__VIEW, MAKELPARAM(0, (int)View::PresetView::Back));
			break;
		}
		case ID_CONTEXT_ORBIT_PRESET_VIEWS_SWISOMETRIC: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC__VIEW, MAKELPARAM(0, (int)View::PresetView::SWIsometric));
			break;
		}
		case ID_CONTEXT_ORBIT_PRESET_VIEWS_SEISOMETRIC: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC__VIEW, MAKELPARAM(0, (int)View::PresetView::SEIsometric));
			break;
		}
		case ID_CONTEXT_ORBIT_PRESET_VIEWS_NEISOMETRIC: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC__VIEW, MAKELPARAM(0, (int)View::PresetView::NEIsometric));
			break;
		}
		case ID_CONTEXT_ORBIT_PRESET_VIEWS_NWISOMETRIC: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC__VIEW, MAKELPARAM(0, (int)View::PresetView::NWIsometric));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUAL_STYLES_2DWIREFRAME: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_VSCURRENT, MAKELPARAM(0, (int)View::VisualStyle::Wireframe2D));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUAL_STYLES_3DWIREFRAME: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_VSCURRENT, MAKELPARAM(0, (int)View::VisualStyle::Wireframe3D));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUAL_STYLES_2DHIDDEN: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_VSCURRENT, MAKELPARAM(0, (int)View::VisualStyle::Hidden2D));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUAL_STYLES_3DHIDDEN: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_VSCURRENT, MAKELPARAM(0, (int)View::VisualStyle::Hidden3D));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUAL_STYLES_REALISTIC: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_VSCURRENT, MAKELPARAM(0, (int)View::VisualStyle::Realistic));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUAL_STYLES_CONCEPTUAL: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_VSCURRENT, MAKELPARAM(0, (int)View::VisualStyle::Conceptual));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUAL_STYLES_SHADED: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_VSCURRENT, MAKELPARAM(0, (int)View::VisualStyle::Shaded));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUAL_STYLES_SHADEDWITHEDGES: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_VSCURRENT, MAKELPARAM(0, (int)View::VisualStyle::ShadedWithEdges));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUAL_STYLES_SHADESOFGREY: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_VSCURRENT, MAKELPARAM(0, (int)View::VisualStyle::ShadesOfGrey));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUAL_STYLES_SKETCHY: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_VSCURRENT, MAKELPARAM(0, (int)View::VisualStyle::Sketchy));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUAL_STYLES_XRAY: {
			SendCommandParam(hDocument, IDC_COMMAND, IDC_VSCURRENT, MAKELPARAM(0, (int)View::VisualStyle::XRay));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUALAIDS_GRID: {
			lpActiveLayout->getActiveViewport()->setGridVisible(!(lpActiveLayout->getActiveViewport()->getGridVisible()));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUALAIDS_GRID_XY: {
			lpActiveLayout->getActiveViewport()->setGridXYVisible(!(lpActiveLayout->getActiveViewport()->getGridXYVisible()));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUALAIDS_GRID_YZ: {
			lpActiveLayout->getActiveViewport()->setGridYZVisible(!(lpActiveLayout->getActiveViewport()->getGridYZVisible()));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUALAIDS_GRID_ZX: {
			lpActiveLayout->getActiveViewport()->setGridZXVisible(!(lpActiveLayout->getActiveViewport()->getGridZXVisible()));
			break;
		}
		case ID_CONTEXT_ORBIT_VISUALAIDS_UCSICON: {
			wm_contextmenu_orbit_visualaids_ucsicon();
			break;
		}
		default: {
			break;
		}
	}

	DestroyMenu(hMenu);

	return 1;
}

void CView3D::wm_contextmenu_orbit_visualaids_ucsicon()
{
	Document* lpDocument = m_system->getDocumentManager().getActiveObject();
	if (lpDocument == nullptr) {
		ErrorHandler();
		return;
	}
	if (lpDocument->getLayoutManager() == nullptr) {
		ErrorHandler();
		return;
	}
	if (lpDocument->getLayoutManager()->getSize() == 0) {
		ErrorHandler();
		return;
	}

	Layout* lpActiveLayout = lpDocument->getLayoutManager()->getActiveObject();
	if (lpActiveLayout == nullptr) {
		ErrorHandler();
		return;
	}

	lpActiveLayout->getActiveViewport()->setUCSVisible(!lpActiveLayout->getActiveViewport()->getUCSVisible());

	::SendMessage(lpDocument->getHWND(), WM_PAINT, 0, 0);
}



CPan::CPan(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: CView2D(command, commandwindows, mode)
{

}
CPan::CPan(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: CView2D(command, commandwindows, text, mode)
{

}

int CPan::wm_setcursor(WPARAM wParam, LPARAM lParam)//Child window; set cursor and return FALSE for default processing.
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (m_bShortcutMenuLoopActive == true) {
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}
	else {
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
	}

	return 1;
}



CPan3D::CPan3D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: CView3D(command, commandwindows, mode)
{

}
CPan3D::CPan3D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: CView3D(command, commandwindows, text, mode)
{

}

int CPan3D::wm_setcursor(WPARAM wParam, LPARAM lParam)//Child window; set cursor and return FALSE for default processing.
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (m_bShortcutMenuLoopActive == true) {
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}
	else {
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
	}

	return 1;
}



CZoom::CZoom(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: CView2D(command, commandwindows, mode)
{

}
CZoom::CZoom(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: CView2D(command, commandwindows, text, mode)
{

}

int CZoom::wm_setcursor(WPARAM wParam, LPARAM lParam)//Child window; set cursor and return FALSE for default processing.
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (m_bShortcutMenuLoopActive == true) {
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}
	else {
		::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
	}

	return 1;
}



CZoom3D::CZoom3D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: CView3D(command, commandwindows, mode)
{

}
CZoom3D::CZoom3D(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: CView3D(command, commandwindows, text, mode)
{

}

int CZoom3D::wm_setcursor(WPARAM wParam, LPARAM lParam)//Child window; set cursor and return FALSE for default processing.
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (m_bShortcutMenuLoopActive == true) {
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}
	else {
		::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
	}

	return 1;
}



C3DOrbit::C3DOrbit(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode)
	: CView3D(command, commandwindows, mode)
{

}
C3DOrbit::C3DOrbit(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
	: CView3D(command, commandwindows, text, mode)
{

}

int C3DOrbit::wm_setcursor(WPARAM wParam, LPARAM lParam)//Child window; set cursor and return FALSE for default processing.
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (m_bShortcutMenuLoopActive == true) {
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}
	else {
		::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
	}

	return 1;
}



CLine::CLine(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, CommandMode mode)
{
	m_lpTextWindow = (TextWindow*)commandwindows[0];
	m_lpCommandLine = (CommandLine*)commandwindows[1];
	m_lpDynamicInput = (DynamicInput*)commandwindows[2];
	m_lpCommandInfo = command;
	m_ptStartPoint = position;

	m_nStep = 0;

	if ((mode == CommandMode::OUTPUT) || (mode == CommandMode::BOTH)) {
		if (CommandLine::s_nVisible == TRUE) {
			if (m_lpCommandLine->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
					m_lpCommandLine->setText("Window Lasso  Press Spacebar to cycle options");
				}
			}
		}

		if (TextWindow::s_nVisible == TRUE) {
			if (m_lpTextWindow->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
					m_lpTextWindow->setStaticText("Command:");
					m_lpTextWindow->setText("Window Lasso  Press Spacebar to cycle options");
				}
			}
		}
	}

	if (DynamicInput::s_nVisible == TRUE) {
		if (m_lpDynamicInput->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpDynamicInput->getHWND()) == TRUE) {
				::ShowWindow(m_lpDynamicInput->getHWND(), SW_HIDE);
			}
		}
	}
}
CLine::CLine(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, const char* text, CommandMode mode)
	: CommandWindow(command, commandwindows, text, mode)
{
	m_ptStartPoint = position;

	if ((mode == CommandMode::OUTPUT) || (mode == CommandMode::BOTH)) {
		if (CommandLine::s_nVisible == TRUE) {
			if (m_lpCommandLine->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
					m_lpCommandLine->setText("Window Lasso  Press Spacebar to cycle options");
				}
			}
		}

		if (TextWindow::s_nVisible == TRUE) {
			if (m_lpTextWindow->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
					m_lpTextWindow->setStaticText("Command:");
					m_lpTextWindow->setText("Window Lasso  Press Spacebar to cycle options");
				}
			}
		}
	}

	if (DynamicInput::s_nVisible == TRUE) {
		if (m_lpDynamicInput->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpDynamicInput->getHWND()) == TRUE) {
				::ShowWindow(m_lpDynamicInput->getHWND(), SW_HIDE);
			}
		}
	}
}

int CLine::wm_lbuttondown(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int CLine::wm_lbuttonup(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int CLine::wm_mousemove(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}

int CLine::wm_char_return()
{
	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandLine->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
				m_lpCommandLine->addString("\r\n");
				m_lpCommandLine->addString("Window Lasso  Press Spacebar to cycle options");
				m_lpCommandLine->setText("");
			}
		}
	}

	if (TextWindow::s_nVisible == TRUE) {
		if (m_lpTextWindow->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
				m_lpTextWindow->addString("\r\n");
				m_lpTextWindow->addString("Window Lasso  Press Spacebar to cycle options");
				m_lpTextWindow->setStaticText("Command:");
				m_lpTextWindow->setText("");
			}
		}
	}

	return 0;
}



SIZE CSelect::s_szMouseHoverSize({ 4,4 });

CSelect::CSelect(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, Colour3<float> colour, CommandMode mode)
	: m_enSelectionType(SelectionType::Window),
	m_enSelectionMethod(SelectionMethod::Window),
	m_bMouseMoved(false),
	m_rcMouseHoverSize({ 0 ,0 ,0 ,0 }),
	m_hrMouseHoverSize(NULL)
{
	m_lpTextWindow = (TextWindow*)commandwindows[0];
	m_lpCommandLine = (CommandLine*)commandwindows[1];
	m_lpDynamicInput = (DynamicInput*)commandwindows[2];
	m_lpCommandInfo = command;
	m_ptMouseDown = position;
	m_ptMouseUp = position;
	m_ptMouseMove = position;
	m_c3fColour = colour;

	if ((mode == CommandMode::OUTPUT) || (mode == CommandMode::BOTH)) {
		if (CommandLine::s_nVisible == TRUE) {
			if (m_lpCommandLine != nullptr) {
				if (m_lpCommandLine->getValid() == TRUE) {
					if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
						m_lpCommandLine->setText("Specify opposite corner or [Fence/WPolygon/CPolygon]");
					}
				}
			}
		}

		if (TextWindow::s_nVisible == TRUE) {
			if (m_lpTextWindow != nullptr) {
				if (m_lpTextWindow->getValid() == TRUE) {
					if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
						m_lpTextWindow->setStaticText("Command:");
						m_lpTextWindow->setText("Specify opposite corner or [Fence/WPolygon/CPolygon]");
					}
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

	createMouseHoverSizeRegion();

	m_lpGLControl = new GLWindow(position);
}
CSelect::CSelect(COMMANDINFO* command, DynamicInputWindow* commandwindows[], POINT position, const char* text, Colour3<float> colour, CommandMode mode)
	: CommandWindow(command, commandwindows, position, text, mode),
	m_enSelectionType(SelectionType::Window),
	m_enSelectionMethod(SelectionMethod::Window),
	m_bMouseMoved(false),
	m_rcMouseHoverSize({ 0 ,0 ,0 ,0 }),
	m_hrMouseHoverSize(NULL)
{
	m_c3fColour = colour;

	if ((mode == CommandMode::OUTPUT) || (mode == CommandMode::BOTH)) {
		if (CommandLine::s_nVisible == TRUE) {
			if (m_lpCommandLine->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
					m_lpCommandLine->setText("Specify opposite corner or [Fence/WPolygon/CPolygon]:");
				}
			}
		}

		if (TextWindow::s_nVisible == TRUE) {
			if (m_lpTextWindow->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
					m_lpTextWindow->setStaticText("Command:");
					m_lpTextWindow->setText("Specify opposite corner or [Fence/WPolygon/CPolygon]:");
				}
			}
		}
	}

	if (DynamicInput::s_nVisible == TRUE) {
		if (m_lpDynamicInput->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpDynamicInput->getHWND()) == TRUE) {
				::ShowWindow(m_lpDynamicInput->getHWND(), SW_HIDE);
			}
		}
	}

	createMouseHoverSizeRegion();

	m_lpGLControl = new GLWindow(position);
}

void CSelect::setSelectionType(SelectionType selectiontype)
{
	m_enSelectionType = selectiontype;
	switch (selectiontype) {
		case SelectionType::Pick: {
			log("Log::CSelect::setSelectionType() SelectionType::Pick");
			//m_enSelectionType = SelectionType::Pick;
			break;
		}
		case SelectionType::Window: {
			log("Log::CSelect::setSelectionType() SelectionType::Window");

			//m_enSelectionType = SelectionType::Window;

			delete m_lpGLControl;
			m_lpGLControl = new GLWindow(m_ptMouseDown);

			log("Log::CSelect::setSelectionType() SelectionType::Window");

			break;
		}
		case SelectionType::Lasso: {
			log("Log::CSelect::setSelectionType() SelectionType::Lasso");

			//m_enSelectionType = SelectionType::Lasso;

			delete m_lpGLControl;
			m_lpGLControl = new GLLasso(m_ptMouseDown, m_c3fColour);

			log("Log::CSelect::setSelectionType() SelectionType::Lasso");

			if (CommandLine::s_nVisible == TRUE) {
				if (m_lpCommandLine->getValid() == TRUE) {
					if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
						m_lpCommandLine->setText("Window Lasso  Press Spacebar to cycle options");
					}
				}
			}

			if (TextWindow::s_nVisible == TRUE) {
				if (m_lpTextWindow->getValid() == TRUE) {
					if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
						m_lpTextWindow->setStaticText("Command:");
						m_lpTextWindow->setText("Window Lasso  Press Spacebar to cycle options");
					}
				}
			}

			break;
		}
		default: {
			break;
		}
	}
}
CSelect::SelectionType CSelect::getSelectionType() { return m_enSelectionType; }

void CSelect::setSelectionMethod(SelectionMethod selectionmethod)
{
	m_enSelectionMethod = selectionmethod;
}
CSelect::SelectionMethod CSelect::getSelectionMethod() { return m_enSelectionMethod; }

bool CSelect::getMouseMoved() { return m_bMouseMoved; }

int CSelect::wm_lbuttondown(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_ptMouseDown = MAKEPOINT(lParam);
	m_ptMouseMove = m_ptMouseDown;
	m_bMouseMoved = false;

	return 0;
}
int CSelect::wm_lbuttonup(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_ptMouseUp = MAKEPOINT(lParam);
	m_ptMouseMove = m_ptMouseUp;

	return 0;
}
int CSelect::wm_mousemove(WPARAM wParam, LPARAM lParam)
{
	UINT nKeycode = GET_KEYSTATE_WPARAM(wParam);
	m_ptMouseMove = MAKEPOINT(lParam);

	if (MK_LBUTTON & nKeycode) {
		if (::PtInRegion(m_hrMouseHoverSize, m_ptMouseMove.x, m_ptMouseMove.y) == FALSE) {
			m_bMouseMoved = true;
			if (m_enSelectionType == SelectionType::Window) {
				setSelectionType(SelectionType::Lasso);
			}
		}
	}

	return 0;
}

int CSelect::wm_char_return()
{
	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandLine->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
				m_lpCommandLine->addString("\r\n");
				m_lpCommandLine->addString("Command: Specify opposite corner or [Fence/WPolygon/CPolygon]:");
				m_lpCommandLine->setText("");
			}
		}
	}

	if (TextWindow::s_nVisible == TRUE) {
		if (m_lpTextWindow->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
				m_lpTextWindow->addString("\r\n");
				m_lpTextWindow->addString("Command: Specify opposite corner or [Fence/WPolygon/CPolygon]:");
				m_lpTextWindow->setStaticText("Command:");
				m_lpTextWindow->setText("");
			}
		}
	}

	return 0;
}

void CSelect::draw(POINT position, Colour3<float> colour) { m_lpGLControl->draw(position, colour); }

void CSelect::createMouseHoverSizeRegion()
{
	::SetRect(&m_rcMouseHoverSize, -s_szMouseHoverSize.cx, -s_szMouseHoverSize.cy, s_szMouseHoverSize.cx, s_szMouseHoverSize.cy);
	::OffsetRect(&m_rcMouseHoverSize, m_ptMouseDown.x, m_ptMouseDown.y);
	m_hrMouseHoverSize = ::CreateRectRgn(m_rcMouseHoverSize.left, m_rcMouseHoverSize.top, m_rcMouseHoverSize.right, m_rcMouseHoverSize.bottom);
}



CUnknown::CUnknown(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode)
{
	m_lpTextWindow = (TextWindow*)commandwindows[0];
	m_lpCommandLine = (CommandLine*)commandwindows[1];
	m_lpDynamicInput = (DynamicInput*)commandwindows[2];
	m_lpCommandInfo = command;

	if ((mode == CommandMode::OUTPUT) || (mode == CommandMode::BOTH)) {
		initCommandLine(text);
		initTextWindow(text);
	}
	initDynamicInput();
}

void CUnknown::initTextWindow(const char* text)
{
	if (TextWindow::s_nVisible == TRUE) {
		if (m_lpTextWindow->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpTextWindow->getHWND()) == TRUE) {
				m_lpTextWindow->addString("\r\n");

				char szString[MAX_LOADSTRING];
				::strcpy_s(szString, "Command: ");
				::strcat_s(szString, MAX_LOADSTRING, text);
				m_lpTextWindow->addString(szString);

				m_lpTextWindow->addString("\r\n");

				::strcpy_s(szString, "Unknown command ");
				::strcat_s(szString, MAX_LOADSTRING, "\"");//Append quotatation marks.
				::strcat_s(szString, MAX_LOADSTRING, text);
				::strcat_s(szString, MAX_LOADSTRING, "\"");//Append quotatation marks.
				::strcat_s(szString, MAX_LOADSTRING, ".  Press F1 for help.");
				m_lpTextWindow->addString(szString);

				m_lpTextWindow->setStaticText("");
				m_lpTextWindow->setText("");

				m_lpTextWindow->addString("\r\n");
				m_lpTextWindow->setStaticText("Command:");
				m_lpTextWindow->setText("");
			}
		}
	}
}
void CUnknown::initCommandLine(const char* text)
{
	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandLine->getValid() == TRUE) {
			if (::IsWindowVisible(m_lpCommandLine->getHWND()) == TRUE) {
				m_lpCommandLine->addString("\r\n");

				char szString[MAX_LOADSTRING]{ 0 };
				::strcpy_s(szString, "Command: ");
				::strcat_s(szString, MAX_LOADSTRING, text);
				m_lpCommandLine->addString(szString);

				m_lpCommandLine->addString("\r\n");

				::strcpy_s(szString, "Unknown command ");
				::strcat_s(szString, MAX_LOADSTRING, "\"");//Append quotatation marks.
				::strcat_s(szString, MAX_LOADSTRING, text);
				::strcat_s(szString, MAX_LOADSTRING, "\"");//Append quotatation marks.
				::strcat_s(szString, MAX_LOADSTRING, ".  Press F1 for help.");
				m_lpCommandLine->addString(szString);

				m_lpCommandLine->setText("");
			}
		}
	}
}
