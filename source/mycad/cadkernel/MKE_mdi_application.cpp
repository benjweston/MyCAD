#include "SYS_mdi_systemwin32.h"

#include "wm_tooltip.h"
#include "wm_statusbar.h"

#include "CMD_commands.h"
#include "CMD_commandmanager.h"

#include "DOC_mdi_document.h"
#include "DOC_glrenderwindow.h"
#include "DOC_resource.h"

#include "MKE_mdi_application.h"
#include "MKE_commonfiledialog.h"
#include "MKE_resource.h"

#include "EDL_dialog_drawingunits.h"
#include "EDL_dialog_draftingsettings.h"
#include "EDL_dialog_drawingwindowcolours.h"
#include "EDL_dialog_options.h"
#include "EDL_dialog_ucs.h"
#include "EDL_dialog_ucsicon.h"
#include "EDL_dialog_viewmanager.h"
#include "EDL_dialog_viewports.h"
#include "EDL_resource.h"

#include "EPL_palette_layerpropertiesmanager.h"
#include "EPL_resource.h"

#include "EWD_dynamicinput.h"
#include "EWD_commandline.h"
#include "EWD_textwindow.h"
#include "EWD_resource.h"

#include "resources.h"

#include "reg_util.h"
#include "xml_util.h"

#include "fault.h"
#include "log.h"

#include <windowsx.h>

using namespace mycad;

HWND MDIApplication::s_hMDIClient(0);

MDIApplication::MDIApplication(WindowManager* windowmanager, DocumentManager* documentmanager)
	: Application(windowmanager, documentmanager)
{
	log("Log::MDIApplication::MDIApplication()");
}
MDIApplication::~MDIApplication()
{
	log("Log::SDIApplication::~SDIApplication()");
}

int MDIApplication::wm_create(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	wm_create_Menu();

	wm_create_statusbar();

	wm_create_toolbar_standard();
	wm_create_toolbar_draw();
	wm_create_rebar_standard();

	wm_create_tabcontrol_documents();

	wm_create_mdiclient((CREATESTRUCT*)lParam);

	getWindowState(WINDOWSTATE_SHOW, &TextWindow::s_nVisible, "\\FixedProfile\\Command Line Windows", "TextWindow.Show");
	getWindowState(WINDOWSTATE_SHOW, &CommandLine::s_nVisible, "\\FixedProfile\\Command Line Windows", "CommandLine.Show");
	getWindowState(WINDOWSTATE_SHOW, &plt_LayerPropertiesManager::s_nVisible, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\LayerManager", "Show");

	getSystemVariables();

	return 0;
}

void MDIApplication::createDocument(const char* filename)
{
	int nCount = (int)::SendMessage(m_hTabcontrol_Documents, TCM_GETITEMCOUNT, 0, 0);
	MDIDocument* lpDocument = new MDIDocument(s_hMDIClient, m_lpCommandWindows, filename, nCount);

	char szClassName[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, ID_DOCUMENT_CLASS, szClassName, sizeof(szClassName) / sizeof(char));

	char* szWindowName = lpDocument->getWindowName();

	DWORD dwExStyle = WS_EX_MDICHILD;
	DWORD dwStyle = MDIS_ALLCHILDSTYLES;

	int nCmdShow = SW_NORMAL;
	HWND hChild = (HWND)SendMessage(s_hMDIClient, WM_MDIGETACTIVE, 0, 0);
	if (::IsZoomed(hChild) == TRUE) {
		dwStyle |= SW_MAXIMIZE;
		nCmdShow = SW_MAXIMIZE;
	}

	BOOL nResult = m_system->createWindow(szClassName,
		szWindowName,
		(WindowWin32*)lpDocument,
		dwExStyle,
		dwStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		//m_ptDocument.x,
		//m_ptDocument.y,
		//m_szDocument.cx,
		//m_szDocument.cy,
		s_hMDIClient,
		(HMENU)IDC_DOCUMENT,
		nCmdShow
	);

	if (nResult == FALSE) return;

	m_lpDocumentManager->addObject(lpDocument);
	m_lpDocumentManager->setActiveObject(lpDocument);

	TCITEM tcItem{ TCIF_TEXT | TCIF_PARAM, 0, 0, (LPSTR)szWindowName, 0, 0, (LPARAM)lpDocument };//Set tab item mask, text & lparam...
	int nIndex = (int)::SendMessage(m_hTabcontrol_Documents, TCM_INSERTITEM, (WPARAM)nCount, (LPARAM)&tcItem);

	nIndex = (int)::SendMessage(m_hTabcontrol_Documents, TCM_SETCURSEL, (WPARAM)nCount, 0);
	if (nIndex == -1) {
		ErrorHandler();
	}

	wm_notify_tcn_selchange_documents();
}

void MDIApplication::wm_command_Window_Close()
{
	int nItemCount = (int)::SendMessage(m_hTabcontrol_Documents, TCM_GETITEMCOUNT, 0, 0);
	int itemIndex = ::SendMessage(m_hTabcontrol_Documents, TCM_GETCURSEL, 0, 0);

	Document* lpDocument = (Document*)m_lpDocumentManager->getActiveObject();

	if (lpDocument == nullptr) {
		return;
	}

	::SendMessage(lpDocument->getHWND(), WM_CLOSE, 0, 0);
	::SendMessage(m_hTabcontrol_Documents, TCM_DELETEITEM, (WPARAM)itemIndex, 0);

	delete lpDocument;
	lpDocument = NULL;

	if (nItemCount > 1) {
		if (itemIndex == nItemCount - 1) {
			int nItemSum = nItemCount - 2;
			::SendMessage(m_hTabcontrol_Documents, TCM_SETCURSEL, (WPARAM)nItemSum, 0);
		}
		else {
			::SendMessage(m_hTabcontrol_Documents, TCM_SETCURSEL, (WPARAM)itemIndex, 0);
		}

		TCITEM tcItem{ TCIF_PARAM };
		itemIndex = ::SendMessage(m_hTabcontrol_Documents, TCM_GETCURSEL, 0, 0);
		::SendMessage(m_hTabcontrol_Documents, TCM_GETITEM, (WPARAM)itemIndex, (LPARAM)&tcItem);
		lpDocument = (Document*)tcItem.lParam;
		if (lpDocument != nullptr) {
			m_lpDocumentManager->setActiveObject(lpDocument);
			HWND hWnd = lpDocument->getHWND();
			::SetWindowPos(hWnd, NULL, m_ptDocument.x, m_ptDocument.y, m_szDocument.cx, m_szDocument.cy, SWP_SHOWWINDOW);
			::SendMessage(hWnd, WM_ACTIVATE, MAKEWPARAM(WA_ACTIVE, 0), (LPARAM)hWnd);
			::SetFocus(hWnd);
		}
	}
	else {
		m_lpDocumentManager->setActiveObject(nullptr);
	}

	wm_notify_tcn_selchange_documents_callback();
}
void MDIApplication::wm_command_Window_CloseAll()
{
	m_lpDocumentManager->setActiveObject(nullptr);

	::SendMessage(m_hTabcontrol_Documents, TCM_DELETEALLITEMS, 0, 0);

	char szClassName[MAX_LOADSTRING]{ 0 };
	::LoadStringA(s_hInstance, ID_DOCUMENT_CLASS, szClassName, sizeof(szClassName) / sizeof(char));

	MSG mMsg;
	mMsg.message = WM_CLOSE;
	mMsg.lParam = (LPARAM)szClassName;

	::EnumChildWindows(m_hWnd, s_bfnEnumChildWindowsProc, (LPARAM)&mMsg);//EnumChildWindows doesn't return a value.
}
void MDIApplication::wm_command_Window_Cascade() { ::SendMessage(s_hMDIClient, WM_MDICASCADE, 0, 0); }
void MDIApplication::wm_command_Window_TileHorizontally() { ::SendMessage(s_hMDIClient, WM_MDITILE, MDITILE_HORIZONTAL, 0); }
void MDIApplication::wm_command_Window_TileVertically() { ::SendMessage(s_hMDIClient, WM_MDITILE, MDITILE_VERTICAL, 0); }
void MDIApplication::wm_command_Window_ArrangeIcons() { ::SendMessage(s_hMDIClient, WM_MDIICONARRANGE, 0, 0); }

int MDIApplication::wm_command_Default(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	log("Log::MDIApplication::wm_command() default");
	if (LOWORD(wParam) >= ID_MDI_FIRSTCHILD) {
		return ::DefFrameProc(m_hWnd, s_hMDIClient, uMsg, wParam, lParam);
	}
	else {
		HWND hChild = (HWND)::SendMessage(s_hMDIClient, WM_MDIGETACTIVE, 0, 0);
		if (hChild != NULL) {
			::SendMessage(hChild, WM_COMMAND, wParam, lParam);
			return 0;
		}
	}
	return ::DefFrameProc(m_hWnd, s_hMDIClient, uMsg, wParam, lParam);
}

void MDIApplication::wm_create_mdiclient(CREATESTRUCT* lpcs)
{
	RECT rcRebar;
	GetClientRect(m_hRebar_Standard, &rcRebar);

	RECT rcStatusbar;
	GetClientRect(m_lpStatusbar->getHWND(), &rcStatusbar);

	RECT rcClient = { 0,rcRebar.bottom + Application::s_nTabconctrol_Documents_Height,lpcs->cx,lpcs->cy - rcRebar.bottom - Application::s_nTabconctrol_Documents_Height - rcStatusbar.bottom };

	rcClient.right -= 20;
	rcClient.bottom -= 63;

	CLIENTCREATESTRUCT ccs{ 0 };
	ccs.hWindowMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_WINDOW));
	ccs.idFirstChild = ID_MDI_FIRSTCHILD;

	//DWORD dwExStyle = WS_EX_CLIENTEDGE;
	DWORD dwExStyle = NULL;
	DWORD dwStyle = WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE;

	s_hMDIClient = ::CreateWindowEx(
		dwExStyle,
		"MDICLIENT",
		NULL,
		dwStyle,
		rcClient.left,
		rcClient.top,
		rcClient.right,
		rcClient.bottom,
		m_hWnd,
		m_hMenu,
		s_hInstance,
		(LPVOID)&ccs
	);

	if (s_hMDIClient == NULL) {
		ErrorHandler();
	}

#pragma warning( push )
#pragma warning( disable : 6387)
	::ShowWindow(s_hMDIClient, SW_SHOW);
#pragma warning( pop )
}

void MDIApplication::wm_notify_tcn_selchange_documents()
{
	int itemIndex = ::SendMessage(m_hTabcontrol_Documents, TCM_GETCURSEL, 0, 0);

	TCITEM tcItem{ TCIF_PARAM };
	::SendMessage(m_hTabcontrol_Documents, TCM_GETITEM, (WPARAM)itemIndex, (LPARAM)&tcItem);

	Document* lpDocument = (Document*)tcItem.lParam;
	if (lpDocument == nullptr) {
		m_lpDocumentManager->setActiveObject(nullptr);
	}
	else {
		m_lpDocumentManager->setActiveObject(lpDocument);//Set active document,...

		HWND hWnd = lpDocument->getHWND();
		//::SetWindowPos(hWnd, HWND_TOP, m_ptDocument.x, m_ptDocument.y, m_szDocument.cx, m_szDocument.cy, SWP_SHOWWINDOW);
		::SendMessage(s_hMDIClient, WM_MDIACTIVATE, (WPARAM)hWnd, 0);

#pragma warning( push )
#pragma warning( disable : 26454 )
		NMHDR nmh;
		nmh.hwndFrom = m_hWnd;
		nmh.idFrom = IDC_TABCTRL_DOCUMENTS;
		nmh.code = TCN_SELCHANGE;
		::SendMessage(lpDocument->getRenderWindow()->getHWND(), WM_NOTIFY, (WPARAM)IDC_TABCTRL_DOCUMENTS, (LPARAM)&nmh);

		//NMHDR nmh{ m_hWnd,IDC_TABCTRL_DOCUMENTS,TCN_SELCHANGE };//Send WM_NOTIFY message to command windows to let them know the active document has changed.
#pragma warning( pop )

		if (m_lpTextWindow != nullptr) {
			::SendMessage(m_lpTextWindow->getHWND(), WM_NOTIFY, (WPARAM)IDC_TABCTRL_DOCUMENTS, (LPARAM)&nmh);
		}
		if (m_lpCommandLine != nullptr) {
			::SendMessage(m_lpCommandLine->getHWND(), WM_NOTIFY, (WPARAM)IDC_TABCTRL_DOCUMENTS, (LPARAM)&nmh);
		}
		if (m_lpDynamicInput != nullptr) {
			::SendMessage(m_lpDynamicInput->getHWND(), WM_NOTIFY, (WPARAM)IDC_TABCTRL_DOCUMENTS, (LPARAM)&nmh);
		}

		::SetFocus(hWnd);//...set focus for keyboard input.
	}
}

int MDIApplication::wm_notify_tcn_selchanging_documents()
{
	int nResult = 0;

	Document* lpActiveDocument = (Document*)m_lpDocumentManager->getActiveObject();
	if (lpActiveDocument == nullptr) {
		nResult = 1;
		return nResult;
	}

	HWND hWnd = lpActiveDocument->getHWND();
	::SendMessage(hWnd, WM_CHAR, (WPARAM)VK_ESCAPE, 0);//Cancel any active commands...
	//::SendMessage(hWnd, WM_ACTIVATE, MAKEWPARAM(WA_INACTIVE, 0), (LPARAM)hWnd);//...and suspend active drawing thread.

	return nResult;
}

void MDIApplication::wm_size()
{
	wm_size_rebar();

	RECT rcRebar;
	GetClientRect(m_hRebar_Standard, &rcRebar);

	::SetWindowPos(m_hTabcontrol_Documents, HWND_TOP, 3, rcRebar.bottom + 3, m_cx - 6, s_nTabconctrol_Documents_Height, SWP_SHOWWINDOW);

	if (m_lpStatusbar == nullptr) {
		return;
	}

	wm_size_statusbar();
	
	RECT rcTabcontrol_Documents = { 0,0,0,0 };
	::GetClientRect(m_hTabcontrol_Documents, &rcTabcontrol_Documents);

	RECT rcStatusbar = { 0,0,0,0 };
	::GetClientRect(m_lpStatusbar->getHWND(), &rcStatusbar);

	::SetWindowPos(
		s_hMDIClient,
		NULL,
		0,
		rcRebar.bottom + 3 + rcTabcontrol_Documents.bottom,
		m_cx,
		m_cy - rcRebar.bottom - 3 - rcTabcontrol_Documents.bottom - rcStatusbar.bottom,
		SWP_NOZORDER
	);
}
