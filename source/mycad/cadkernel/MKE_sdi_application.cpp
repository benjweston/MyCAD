#include "SYS_sdi_systemwin32.h"

#include "wm_tooltip.h"
#include "wm_statusbar.h"

#include "CMD_commands.h"
#include "CMD_commandmanager.h"

#include "DOC_sdi_document.h"
#include "DOC_glrenderwindow.h"
#include "DOC_resource.h"

#include "MKE_sdi_application.h"
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

SDIApplication::SDIApplication(WindowManager* windowmanager, DocumentManager* documentmanager)
	: Application(windowmanager, documentmanager)
{
	log("Log::SDIApplication::SDIApplication()");
}
SDIApplication::~SDIApplication()
{
	log("Log::SDIApplication::~SDIApplication()");
}

int SDIApplication::wm_create(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	wm_create_Menu();

	wm_create_statusbar();

	wm_create_toolbar_standard();
	wm_create_toolbar_draw();
	wm_create_rebar_standard();

	wm_create_tabcontrol_documents();

	getWindowState(WINDOWSTATE_SHOW, &TextWindow::s_nVisible, "\\FixedProfile\\Command Line Windows", "TextWindow.Show");
	getWindowState(WINDOWSTATE_SHOW, &CommandLine::s_nVisible, "\\FixedProfile\\Command Line Windows", "CommandLine.Show");
	getWindowState(WINDOWSTATE_SHOW, &plt_LayerPropertiesManager::s_nVisible, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\LayerManager", "Show");

	getSystemVariables();

	return 0;
}

void SDIApplication::createDocument(const char* filename)
{
	int nCount = (int)::SendMessage(m_hTabcontrol_Documents, TCM_GETITEMCOUNT, 0, 0);
	SDIDocument* lpDocument = new SDIDocument(m_lpCommandWindows, filename, nCount);

	char szClassName[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, ID_DOCUMENT_CLASS, szClassName, sizeof(szClassName) / sizeof(char));

	char* szWindowName = lpDocument->getWindowName();

	DWORD dwExStyle = NULL;
	DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP;

	int nCmdShow = SW_NORMAL;

	int nResult = m_system->createWindow(szClassName,
		szWindowName,
		(WindowWin32*)lpDocument,
		dwExStyle,
		dwStyle,
		m_ptDocument.x,
		m_ptDocument.y,
		m_szDocument.cx,
		m_szDocument.cy,
		m_hWnd,
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
