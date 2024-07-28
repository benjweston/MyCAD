#include "SYS_systemwin32.h"

#include "DOC_mdi_document.h"
#include "DOC_resource.h"

#include "DOC_glrenderwindow.h"
#include "DRW_glrenderthread.h"

#include "EDL_dialog_drawingunits.h"
#include "EDL_dialog_draftingsettings.h"
#include "EDL_dialog_drawingwindowcolours.h"
#include "EDL_dialog_options.h"
#include "EDL_dialog_ucs.h"
#include "EDL_dialog_ucsicon.h"
#include "EDL_dialog_viewmanager.h"
#include "EDL_dialog_viewports.h"
#include "EDL_resource.h"

#include "EPL_resource.h"

#include "EWD_textwindow.h"
#include "EWD_commandline.h"
#include "EWD_dynamicinput.h"
#include "EWD_resource.h"

#include "entities.h"

#include "resources.h"

#include "char_util.h"//Required for toChar()
#include "reg_util.h"

#include "fault.h"
#include "log.h"

#include <commctrl.h>
#include <Shlwapi.h>//Required for PathStripPath()
#include <strsafe.h>
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.
#include <windowsx.h>

using namespace mycad;

MDIDocument::MDIDocument(HWND hMDIClient, DynamicInputWindow* commandwindows[], const char* filename, int tabindex)
	: Document(commandwindows, filename, tabindex),
	m_hMDIClient(hMDIClient)
{
	log("Log::MDIDocument::MDIDocument()");
}
MDIDocument::~MDIDocument()
{
	log("Log::MDIDocument::~MDIDocument()");
}

int MDIDocument::wm_create(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	::SendMessage(m_system->getApplication()->getHWND(), WM_PARENTNOTIFY, MAKEWPARAM(WM_CREATE, IDC_DOCUMENT), (LPARAM)m_hWnd);//Required to simulate DefWindowProc behaviour for WM_PARENTNOTIFY.

	wm_create_Menu();

	m_hTabcontrol_Layouts = ::CreateWindowEx(
		WS_EX_NOPARENTNOTIFY,
		WC_TABCONTROL,
		NULL,
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_TOOLTIPS | TCS_BOTTOM,
		3,
		m_cy - m_nTabconctrol_Layouts_Height,
		m_cx - 6,
		m_nTabconctrol_Layouts_Height,
		m_hWnd,
		(HMENU)IDC_TABCTRL_LAYOUTS,
		s_hInstance,
		(LPVOID)0
	);

	if (m_hTabcontrol_Layouts == NULL) {
		ErrorHandler();
	}

	setFont(m_hTabcontrol_Layouts);

	m_hTabcontrol_Layouts_Tooltip = (HWND)::SendMessage(m_hTabcontrol_Layouts, TCM_GETTOOLTIPS, 0, 0);
	if (m_hTabcontrol_Layouts_Tooltip == NULL) {
		ErrorHandler();
	}

	char szRenderWindowClassName[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, ID_RENDERWINDOW_CLASS, szRenderWindowClassName, sizeof(szRenderWindowClassName) / sizeof(char));

	DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP;
	if (GLRenderWindow::s_bScrollbars == true) {
		dwStyle |= WS_VSCROLL | WS_HSCROLL;
	}

	if (m_system->createWindow(szRenderWindowClassName,
		NULL,
		(WindowWin32*)m_lpGLRenderWindow,
		NULL,
		dwStyle,
		0,
		0,
		m_cx,
		m_cy - m_nTabconctrol_Layouts_Height,
		m_hWnd,
		(HMENU)IDC_RENDERWINDOW
	) == IDOK) {
		log("Log::Application::createDocument() TRUE");
	}
	else {
		log("Log::Application::createDocument() FALSE");
	}

	HRESULT hResult = ErrorHandler();
	if (FAILED(hResult)) {
		return -1;
	}

	HWND hStatusbar = ::GetDlgItem(m_system->getApplication()->getHWND(), IDC_STATUSBAR);
	setStatusbarText(hStatusbar, 1);

	return 0;
}
int MDIDocument::wm_destroy()
{
	::SendMessage(m_system->getApplication()->getHWND(), WM_PARENTNOTIFY, MAKEWPARAM(WM_DESTROY, IDC_DOCUMENT), (LPARAM)m_hWnd);//Required to simulate DefWindowProc behaviour for WM_PARENTNOTIFY.

	m_system->getDocumentManager().setObjectInactive(this);
	BOOL nResult = m_system->getDocumentManager().removeObject(this);

	if (nResult == TRUE) {
		log("Log::Document::wm_destroy() m_system->getDocumentManager().removeWindow == TRUE");
	}
	else {
		log("Log::Document::wm_destroy() m_system->getDocumentManager().removeWindow == FALSE");
	}

	return 0;
}
int MDIDocument::wm_mdiactivate(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)wParam == m_hWnd) {
		::SendMessage(m_lpGLRenderWindow->getHWND(), WM_ACTIVATE, MAKEWPARAM(WA_INACTIVE, 0), 0);
		setApplicationTitle(WA_INACTIVE);
		setApplicationMenu(WA_INACTIVE);
	}
	else if ((HWND)lParam == m_hWnd) {
		::SendMessage(m_lpGLRenderWindow->getHWND(), WM_ACTIVATE, MAKEWPARAM(WA_ACTIVE, 0), 0);
		setApplicationTitle(WA_ACTIVE);
		setApplicationMenu(WA_ACTIVE);
	}

	setTextWindowTitle();

	return 0;
}
int MDIDocument::wm_settext(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int MDIDocument::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);
	switch (nID) {
		case SC_CLOSE: {
			log("Log::MDIDocument::wm_syscommand() SC_CLOSE");

			return(TRUE);
		}
		case SC_MAXIMIZE: {
			log("Log::MDIDocument::wm_syscommand() SC_MAXIMIZE");
			setApplicationTitle(SC_MAXIMIZE);
			break;
		}
		case SC_MINIMIZE: {
			log("Log::MDIDocument::wm_syscommand() SC_MINIMIZE");
			setApplicationTitle(SC_MINIMIZE);
			break;
		}
		case SC_RESTORE: {
			log("Log::MDIDocument::wm_syscommand() SC_RESTORE");
			setApplicationTitle(SC_RESTORE);
			break;
		}
		default: {
			log("Log::MDIDocument::wm_syscommand() default");
			break;
		}
	}

	return 0;
}

void MDIDocument::setApplicationTitle(INT nState)
{
	char szApplicationTitle[MAX_LOADSTRING]{ 0 };
	::strcpy_s(szApplicationTitle, m_szApplicationName);

	switch (nState) {
		case WA_ACTIVE: {
			if (!::IsZoomed(m_hWnd)) {
				char seperator[8] = " - ";
				::strcat_s(szApplicationTitle, 16, seperator);
				::strcat_s(szApplicationTitle, MAX_LOADSTRING, m_szWindowName);
			}
			break;
		}
		case SC_MAXIMIZE: {
			//char seperator[8] = " - ";
			//::strcat_s(szApplicationTitle, 16, seperator);
			//::strcat_s(szApplicationTitle, MAX_LOADSTRING, szWindowName);
			break;
		}
		case SC_MINIMIZE:
		case SC_RESTORE: {
			char seperator[8] = " - ";
			::strcat_s(szApplicationTitle, 16, seperator);
			::strcat_s(szApplicationTitle, MAX_LOADSTRING, m_szWindowName);
			break;
		}
	}

	BOOL nResult = (BOOL)::SendMessage(m_system->getApplication()->getHWND(), WM_SETTEXT, 0, (LPARAM)&szApplicationTitle);
	if (nResult != TRUE) {
		ErrorHandler();
	}
}
void MDIDocument::setApplicationMenu(BOOL nState)
{
	if (nState == WA_ACTIVE) {
		::SendMessage(m_hMDIClient, WM_MDISETMENU, (WPARAM)m_hMenu, (LPARAM)::GetSubMenu(m_hMenu, 9));
	}
	else {
		HMENU hMenu = m_system->getApplication()->getMenu();
		::SendMessage(m_hMDIClient, WM_MDISETMENU, (WPARAM)hMenu, (LPARAM)::GetSubMenu(hMenu, 2));
	}
}
