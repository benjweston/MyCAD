#include "SYS_systemwin32.h"

#include "DOC_document.h"
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

Document::Document(DynamicInputWindow* commandwindows[], const char* filename, int tabindex)
	: WindowWin32(),
	m_lpCommandWindows{ commandwindows[0],commandwindows[1],commandwindows[2] },
	m_szFileName{ 0 },
	nTabIndex(tabindex),
	m_szApplicationName{ 0 },
	m_szWindowName{ 0 },
	m_hTabcontrol_Layouts(0),
	m_hTabcontrol_Layouts_Tooltip(0),
	m_nTabconctrol_Layouts_Height(24),
	m_nHScroll(0),
	m_nVScroll(0),
	m_siHorz({ sizeof(SCROLLINFO),SIF_POS | SIF_RANGE,-100,100,100,0,0 }),
	m_siVert({ sizeof(SCROLLINFO),SIF_POS | SIF_RANGE,-100,100,100,0,0 }),
	m_mouseTrack(MouseTrackEvents(TME_LEAVE)),
	m_ptMouseDown({ 0,0 }),
	m_ptMouseUp({ 0,0 }),
	m_ptMouseMove({ 0,0 }),
	m_bMouseMoved(false),
	m_bMenuLoopActive(false),
	m_bShortcutMenuLoopActive(false),
	m_bLbuttonDblclk(false),
	m_nCharWidth(0),
	m_cch(0),
	m_nCurChar(0),
	m_pchInputBuf(0),
	m_ch(0)
{
	log("Log::Document::Document()");
	m_pchInputBuf = (LPTSTR)::GlobalAlloc(GPTR, MAX_LOADSTRING * sizeof(char));
	m_lpGLRenderWindow = new GLRenderWindow(commandwindows, filename);

	HINSTANCE hModule = ::LoadLibrary("mycaddata.dll");
	if (hModule != NULL) {
		::LoadString(hModule, ID_APPLICATION_NAME, m_szApplicationName, sizeof(m_szApplicationName) / sizeof(char));
		::FreeLibrary(hModule);
	}

	::strcpy_s(m_szFileName, filename);
	::strcpy_s(m_szWindowName, m_szFileName);
	::PathStripPath(m_szWindowName);
}
Document::~Document()
{
	log("Log::Document::~Document()");

	delete m_lpGLRenderWindow;
	m_lpGLRenderWindow = nullptr;

	m_system->getDocumentManager().removeObject(this);

	m_pchInputBuf = 0;
	GlobalFree((HGLOBAL)m_pchInputBuf);
}

void Document::setFileName(const char* filename)
{
	::strcpy_s(m_szFileName, filename);
	::strcpy_s(m_szWindowName, m_szFileName);
	::PathStripPath(m_szWindowName);
}
char* Document::getFileName() { return m_szFileName; }

void Document::setTabIndex(int tabindex) { nTabIndex = tabindex; }
int Document::getTabIndex() { return nTabIndex; }

char* Document::getWindowName() { return m_szWindowName; }

Window* Document::getRenderWindow() { return (Window*)m_lpGLRenderWindow; }

LayerManager* Document::getLayerManager() { return m_lpGLRenderWindow->getLayerManager(); }
LayoutManager* Document::getLayoutManager() { return m_lpGLRenderWindow->getLayoutManager(); }
Layout* Document::getActiveLayout()
{
	if (m_lpGLRenderWindow->getLayoutManager() != nullptr) {
		if (m_lpGLRenderWindow->getLayoutManager()->getActiveObject() != nullptr) {
			return m_lpGLRenderWindow->getLayoutManager()->getActiveObject();
		}
	}
	return nullptr;
}
ViewManager* Document::getViewManager() { return m_lpGLRenderWindow->getViewManager(); }
ViewportManager* Document::getViewports() { return m_lpGLRenderWindow->getLayoutManager()->getActiveObject()->Viewports(); }
Command* Document::getActiveCommand() { return m_CommandManager.getActiveObject(); }
CommandManager* Document::getCommandManager() { return &m_CommandManager; }

BOOL Document::setStatusbarText(HWND hstatusbar, int index)
{
	BOOL nResult = FALSE;


	switch (index) {
		case -1: {
			break;
		}
		case 0: {

			break;
		}
		case 1: {
			char szCoordinatese[MAX_LOADSTRING]{ 0 };

			Vector3<double> v3dCoordinates = m_lpGLRenderWindow->getCoordinates();

			if (v3dCoordinates.x != v3dCoordinates.x) {
				v3dCoordinates.x = 0.0;
			}

			if (v3dCoordinates.y != v3dCoordinates.y) {
				v3dCoordinates.y = 0.0;
			}

			if (v3dCoordinates.z != v3dCoordinates.z) {
				v3dCoordinates.z = 0.0;
			}

			const char* x = toChar(v3dCoordinates.x, 4);
			const char* y = toChar(v3dCoordinates.y, 4);
			const char* z = toChar(v3dCoordinates.z, 4);

			::strcpy_s(szCoordinatese, x);
			::strcat_s(szCoordinatese, MAX_LOADSTRING, ", ");
			::strcat_s(szCoordinatese, MAX_LOADSTRING, y);
			::strcat_s(szCoordinatese, MAX_LOADSTRING, ", ");
			::strcat_s(szCoordinatese, MAX_LOADSTRING, z);

			nResult = ::SendMessage(hstatusbar, SB_SETTEXT, MAKEWPARAM(1, SBT_NOBORDERS), (LPARAM)szCoordinatese);

			break;
		}
		case 2: {
			char szActiveSpace[MAX_LOADSTRING]{ 0 };

			if (getActiveLayout()->getActiveSpace()->getContext() == Space::Context::ModelSpace) {
				::LoadString(s_hInstance, ID_MODELSPACE, szActiveSpace, sizeof(szActiveSpace) / sizeof(char));
			}
			else if (getActiveLayout()->getActiveSpace()->getContext() == Space::Context::PaperSpace) {
				::LoadString(s_hInstance, ID_PAPERSPACE, szActiveSpace, sizeof(szActiveSpace) / sizeof(char));
			}

			nResult = ::SendMessage(hstatusbar, SB_SETTEXT, MAKEWPARAM(2, SBT_POPOUT), (LPARAM)szActiveSpace);

			break;
		}
		case 3: {

			break;
		}
		case 4: {

			break;
		}
		case 5: {

			break;
		}
		case 6: {

			break;
		}
		case 7: {

			break;
		}
		case 8: {

			break;
		}
		case 9: {

			break;
		}
		default: {
			break;
		}
	}

	return nResult;
}
BOOL Document::setDynamicInputStyle()
{
	if (getActiveLayout()->getActiveSpace()->getContext() == Space::Context::ModelSpace) {
		if (getActiveLayout()->getActiveViewport()->getCamera()->isViewOrthogonal()) {
			DynamicInput::s_nTintForXYZ = FALSE;
		}
		else {
			DynamicInput::s_nTintForXYZ = TRUE;
		}
		m_lpCommandWindows[2]->setBorderColour(RGB(0, 0, 0));
		m_lpCommandWindows[2]->setBackgroundColour(RGB(138, 138, 138));
		m_lpCommandWindows[2]->setTextBackgroundColour(RGB(138, 138, 138));
		m_lpCommandWindows[2]->setTextColour(RGB(0, 0, 0));
	}
	else if (getActiveLayout()->getActiveSpace()->getContext() == Space::Context::PaperSpace) {
		DynamicInput::s_nTintForXYZ = FALSE;
		m_lpCommandWindows[2]->setBorderColour(RGB(255, 255, 255));
		m_lpCommandWindows[2]->setBackgroundColour(RGB(255, 255, 255));
		m_lpCommandWindows[2]->setTextBackgroundColour(RGB(255, 255, 255));
		m_lpCommandWindows[2]->setTextColour(RGB(138, 138, 138));
	}
	//m_lpCommandWindows[2]->setActiveWindow(0);

	if (m_lpCommandWindows[2] == nullptr) return FALSE;

	m_lpCommandWindows[2]->setBorderColour(RGB(138, 138, 138));

	return TRUE;
}

//Public window functions.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
int Document::wm_activate(WPARAM wParam, LPARAM lParam)
{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	UNREFERENCED_PARAMETER(lParam);

	::SendMessage(m_lpGLRenderWindow->getHWND(), WM_ACTIVATE, wParam, lParam);

	switch (LOWORD(wParam)) {
		case WA_INACTIVE: {
			log("Log::Document::wm_activate() WA_INACTIVE");
			setApplicationTitle(WA_INACTIVE);
			setApplicationMenu(WA_INACTIVE);
			break;
		case WA_ACTIVE:
		case WA_CLICKACTIVE: {
			log("Log::Document::wm_activate() WA_ACTIVE");
			setApplicationTitle(WA_ACTIVE);
			setApplicationMenu(WA_ACTIVE);
			break;
		}
		default:
			break;
		}
	}

	setTextWindowTitle();

	return 0;
}
int Document::wm_char(WPARAM wParam, LPARAM lParam)//Contains functions for editing the command text buffer.
{
	::SendMessage(m_lpGLRenderWindow->getHWND(), WM_CHAR, wParam, lParam);
	return 0;
}
int Document::wm_close()
{
	::SendMessage(m_lpGLRenderWindow->getHWND(), WM_CLOSE, 0, 0);
	::DestroyWindow(m_hWnd);

	return 0;
}
int Document::wm_command(WPARAM wParam, LPARAM lParam)
{
	::SendMessage(m_lpGLRenderWindow->getHWND(), WM_COMMAND, wParam, lParam);
	return 0;
}
int Document::wm_create(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

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
int Document::wm_destroy()
{
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
int Document::wm_exitsizemove()
{
	return 0;
}
int Document::wm_getminmaxinfo(LPARAM lParam)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
	lpMMI->ptMinTrackSize = { 150, 80 };
	return 0;
}
int Document::wm_hscroll(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int Document::wm_keydown(WPARAM wParam, LPARAM lParam)//Contains functions for moving the virtual carat in the command text buffer.
{
	::SendMessage(m_lpGLRenderWindow->getHWND(), WM_KEYDOWN, wParam, lParam);
	return 0;
}
int Document::wm_killfocus(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);

	return 0;
}
int Document::wm_lbuttondblclk(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int Document::wm_lbuttondown(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int Document::wm_lbuttonup(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int Document::wm_mouseleave()
{
	return 0;
}
int Document::wm_mousemove(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int Document::wm_mousewheel(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int Document::wm_move(LPARAM lParam)
{
	log("Log::Document::wm_move()");

	m_x = GET_X_LPARAM(lParam);
	m_y = GET_Y_LPARAM(lParam);

	return 0;
}
int Document::wm_notify(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

#define lpnmhdr (LPNMHDR(lParam))

	log("Log::Document::wm_notify() lpnmhdr->code = %d", lpnmhdr->code);

	switch (lpnmhdr->code) {
		case TTN_GETDISPINFO: {//Tool tip notifications...
			log("Log::Application::wm_notify() TTN_GETDISPINFO");

#define lpttt (LPTOOLTIPTEXT(lParam))

			wm_notify_ttn_getdispinfo(lpttt->hdr, lpttt->lpszText);
			break;
		}
		case TCN_SELCHANGE: {//Tab control notifications...
			wm_notify_tcn_selchange(lpnmhdr->idFrom);
			break;
		}
		case TCN_SELCHANGING: {//Tab control notifications...
			wm_notify_tcn_selchanging(lpnmhdr->idFrom);
			break;
		}
		default: {
			break;
		}
	}

	return 0;
}
int Document::wm_setcursor(WPARAM wParam, LPARAM lParam)//Child window; set cursor and return FALSE for default processing.
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	return 0;
}
int Document::wm_setfocus(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);

	log("Log::Document::wm_setfocus()");

	if (TextWindow::s_nVisible == TRUE) {
		if (m_lpCommandWindows[0] != nullptr) {
			if (m_lpCommandWindows[0]->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpCommandWindows[0]->getHWND()) == TRUE) {
					::SendMessage(m_lpCommandWindows[0]->getHWND(), WM_KILLFOCUS, (WPARAM)m_hWnd, 0);
				}
			}
		}
	}

	if (CommandLine::s_nVisible == TRUE) {
		if (m_lpCommandWindows[1] != nullptr) {
			if (m_lpCommandWindows[1]->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpCommandWindows[1]->getHWND()) == TRUE) {
					::SendMessage(m_lpCommandWindows[1]->getHWND(), WM_KILLFOCUS, (WPARAM)m_hWnd, 0);
				}
			}
		}
	}

	if (DynamicInput::s_nVisible == TRUE) {
		if (m_lpCommandWindows[2] != nullptr) {
			if (m_lpCommandWindows[2]->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpCommandWindows[2]->getHWND()) == TRUE) {
					::ShowWindow(m_lpCommandWindows[2]->getHWND(), SW_HIDE);
				}
			}
		}
	}

	::SetFocus(m_hWnd);//Set focus for keyboard input.

	return 0;
}
int Document::wm_syscommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int nID = LOWORD(wParam);
	switch (nID) {
		case SC_CLOSE: {
			log("Log::Document::wm_syscommand() SC_CLOSE");

			return(TRUE);
		}
		case SC_MAXIMIZE: {
			log("Log::Document::wm_syscommand() SC_MAXIMIZE");

			break;
		}
		case SC_MINIMIZE: {
			log("Log::Document::wm_syscommand() SC_MINIMIZE");

			break;
		}
		case SC_RESTORE: {
			log("Log::Document::wm_syscommand() SC_RESTORE");

			break;
		}
		default: {
			log("Log::Document::wm_syscommand() default");
			break;
		}
	}

	return 0;
}
int Document::wm_timer(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	log("Log::Document::wm_timer()");

	return 0;
}
int Document::wm_vscroll(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	//m_siVert.cbSize = sizeof(SCROLLINFO);
	//m_siVert.fMask = SIF_ALL;

	//GetScrollInfo(m_hWnd, SB_VERT, &m_siVert);
	//m_nVScroll = m_siVert.nPos;

	//switch (LOWORD(wParam)) {
	//	case SB_LINELEFT: {
	//		m_siVert.nPos -= 1;
	//		break;
	//	}
	//	case SB_LINERIGHT: {
	//		m_siVert.nPos += 1;
	//		break;
	//	}
	//	case SB_PAGELEFT: {
	//		m_siVert.nPos -= m_siVert.nPage;
	//		break;
	//	}
	//	case SB_PAGERIGHT: {
	//		m_siVert.nPos += m_siVert.nPage;
	//		break;
	//	}
	//	case SB_THUMBTRACK: {
	//		m_siVert.nPos = m_siVert.nTrackPos;
	//		break;
	//	}
	//	default: {
	//		break;
	//	}
	//}

	//m_siVert.fMask = SIF_POS;
	//SetScrollInfo(m_hWnd, SB_VERT, &m_siVert, TRUE);

	//GetScrollInfo(m_hWnd, SB_VERT, &m_siVert);
	//if (m_siVert.nPos != m_nVScroll) {
	//	int dy = m_nVScroll - m_siVert.nPos;
	//	//log("Log::Document::vScroll() dy = %d", -dy);
	//	m_lpGLRenderThread->SendThreadMessage(WM_VSCROLL, wParam, (LPARAM)-dy);//Reverse sign of dy to account for reversed y-axis in OpenGL.
	//}

	return 0;
}
int Document::wm_windowposchanged(LPARAM lParam)
{
	//UNREFERENCED_PARAMETER(lParam);

	log("Log::Document::wm_windowposchanged()");

	LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;

	m_x = lpwp->x;
	m_y = lpwp->y;

	m_cx = lpwp->cx;
	m_cy = lpwp->cy;

	wm_size();

	return 0;
}
int Document::wm_windowposchanging(LPARAM lParam)
{
	//UNREFERENCED_PARAMETER(lParam);

	log("Log::Document::wm_windowposchanging()");

	LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;

	m_x = lpwp->x;
	m_y = lpwp->y;

	m_cx = lpwp->cx;
	m_cy = lpwp->cy;

	wm_size();

	return 0;
}

//Private overrides of public window functions.
void Document::wm_size()
{
	BOOL nResult = (BOOL)::SetWindowPos(m_hTabcontrol_Layouts, NULL, 0, m_cy - m_nTabconctrol_Layouts_Height, m_cx, m_nTabconctrol_Layouts_Height, SWP_SHOWWINDOW);
	if (nResult == 0) {
		ErrorHandler();
	}

	if (m_lpGLRenderWindow == nullptr) {
		return;
	}
	if (m_lpGLRenderWindow->getValid() == NULL) {
		return;
	}

	nResult = (BOOL)::SetWindowPos(m_lpGLRenderWindow->getHWND(), HWND_TOP, 0, 0, m_cx, m_cy - m_nTabconctrol_Layouts_Height, SWP_SHOWWINDOW);
	if (nResult == 0) {
		ErrorHandler();
	}
}

void Document::setApplicationTitle(INT nState)
{
	char szApplicationTitle[MAX_LOADSTRING]{ 0 };
	::strcpy_s(szApplicationTitle, m_szApplicationName);

	if (nState == WA_ACTIVE) {
		char seperator[8] = " - ";
		::strcat_s(szApplicationTitle, 16, seperator);
		::strcat_s(szApplicationTitle, MAX_LOADSTRING, m_szWindowName);
	}

	BOOL nResult = (BOOL)::SendMessage(m_system->getApplication()->getHWND(), WM_SETTEXT, 0, (LPARAM)&szApplicationTitle);
	if (nResult != TRUE) {
		ErrorHandler();
	}
}
void Document::setApplicationMenu(BOOL nState)
{
	if (nState == WA_ACTIVE) {
		::SetMenu(m_system->getApplication()->getHWND(), m_hMenu);
	}
	else {
		HMENU hMenu = m_system->getApplication()->getMenu();
		::SetMenu(m_system->getApplication()->getHWND(), hMenu);
	}
}

BOOL Document::showCommandline(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	BOOL nResult = FALSE;

	if (CommandLine::s_nVisible == FALSE) return FALSE;

	if (m_lpCommandWindows[1]->getValid() == FALSE) return FALSE;

	if (::IsWindowVisible(m_lpCommandWindows[1]->getHWND()) == TRUE) {
		m_ch = (char)wParam;

		HWND hCommand = ::GetDlgItem(m_lpCommandWindows[1]->getHWND(), IDC_COMMANDLINE_EDIT_COMMAND);
		::SetFocus(hCommand);

		LRESULT lResult = ::SendMessage(hCommand, WM_SETTEXT, 0, (LPARAM)&m_ch);
		if (lResult == TRUE) {
			::SendMessage(hCommand, EM_SETCARETINDEX, (WPARAM)1, 0);
		}

		nResult = TRUE;
	}

	return nResult;
}
BOOL Document::showDynamicinput(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	BOOL nResult = FALSE;

	if (DynamicInput::s_nVisible == FALSE) return FALSE;
	if (m_lpCommandWindows[2] == nullptr) return FALSE;
	if (m_lpCommandWindows[2]->getValid() == FALSE) return FALSE;

	if (::IsWindowVisible(m_lpCommandWindows[2]->getHWND()) == FALSE) {
		POINT ptMouseMove = m_ptMouseMove;
		::ClientToScreen(m_hWnd, &ptMouseMove);
		::SetWindowPos(m_lpCommandWindows[2]->getHWND(),
			NULL,
			ptMouseMove.x + DynamicInput::s_szOffset.cx,
			ptMouseMove.y + DynamicInput::s_szOffset.cy,
			DynamicInput::s_szSize.cx,
			DynamicInput::s_szSize.cy,
			SWP_NOACTIVATE
		);
		::ShowWindow(m_lpCommandWindows[2]->getHWND(), SW_SHOWNA);

		if (::IsWindowVisible(m_lpCommandWindows[2]->getHWND()) == TRUE) {
			m_ch = (char)wParam;

			HWND hCommand = ::GetDlgItem(m_lpCommandWindows[2]->getHWND(), IDC_DYNAMICINPUT_EDIT_COMMAND);
			::SetFocus(hCommand);

			LRESULT lResult = ::SendMessage(hCommand, WM_SETTEXT, 0, (LPARAM)&m_ch);
			if (lResult == TRUE) {
				::SendMessage(hCommand, EM_SETCARETINDEX, (WPARAM)1, 0);
			}

			nResult = TRUE;
		}
	}

	return nResult;
}

void Document::setTextWindowTitle()
{
	if (TextWindow::s_nVisible == FALSE) return;

	if (m_lpCommandWindows[0]->getValid() == FALSE) return;

	TextWindow* lpTextWindow = (TextWindow*)m_lpCommandWindows[0];

	char szTextWindowName[MAX_LOADSTRING]{ 0 };
	::LoadString(s_hInstance, ID_TEXTWINDOW_NAME, szTextWindowName, sizeof(szTextWindowName) / sizeof(char));

	char szNewTitle[MAX_LOADSTRING]{ 0 };
	::strcpy_s(szNewTitle, szTextWindowName);
	::strcat_s(szNewTitle, MAX_LOADSTRING, " - ");
	::strcat_s(szNewTitle, MAX_LOADSTRING, m_szWindowName);

	if (lpTextWindow->getValid() == TRUE) {
		::SendMessage(lpTextWindow->getHWND(), WM_SETTEXT, 0, (LPARAM)&szNewTitle);
	}
}

POINT Document::WinToGL(POINT position) { return POINT({ position.x,m_cy - position.y }); }//Transforms Windows coordinates (Right is +x, down is +y) to OpenGL coordinates (Right is +x, up is +y).

void Document::clearCommandTextBuffer()
{
	m_pchInputBuf = 0;
	GlobalFree((HGLOBAL)m_pchInputBuf);
	m_pchInputBuf = (LPTSTR)GlobalAlloc(GPTR, MAX_LOADSTRING * sizeof(char));
	m_nCurChar = 0;
	m_cch = 0;

	log("Log::Document::clearCommandTextBuffer() m_pchInputBuf = %s", m_pchInputBuf);
}

int Document::createCommandWindow(Command* lpCommand, CommandContext nContext)//Creates a message-only window.
{
	char szCommandClassName[MAX_LOADSTRING];
	::LoadString(s_hInstance, ID_COMMAND_CLASS, szCommandClassName, sizeof(szCommandClassName) / sizeof(char));
	int nResult = m_system->createWindow(szCommandClassName,
		NULL,
		(WindowWin32*)lpCommand,
		NULL,
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		HWND_MESSAGE,
		//(HMENU)IntToPtr(lpCommand->getID())
		(HMENU)(intptr_t)(lpCommand->getID())
	);
	if (nResult == IDOK) {
		log("Log::Document::createCommandWindow() IDOK");
		m_CommandManager.addObject(lpCommand);
		m_CommandManager.setActiveObject(lpCommand, nContext);
	}

	return nResult;
}
int Document::createCommandDialog(Command* lpCommand, UINT nDialogId)//Creates a modal dialog box from a dialog box template resource.
{
	m_CommandManager.addObject(lpCommand);
	m_CommandManager.setActiveObject(lpCommand, CommandContext::COMMAND);

	int nResult = m_system->createDialog(lpCommand, nDialogId, m_system->getApplication()->getHWND());

	if (nResult >= 0) {
		if (nResult == IDOK) {
			log("Log::Document::createCommandDialog() IDOK");
		}
		else if (nResult == IDCANCEL) {
			log("Log::Document::createCommandDialog() IDCANCEL");
		}
	}

	m_CommandManager.setObjectInactive(CommandContext::COMMAND);
	clearCommandTextBuffer();

	return nResult;
}

void Document::exitCommand()
{
	if (m_CommandManager.getActiveObject(CommandContext::VIEW) != nullptr) {
		m_CommandManager.setObjectInactive(CommandContext::VIEW);
	}
	else if (m_CommandManager.getActiveObject(CommandContext::COMMAND) != nullptr) {
		m_CommandManager.setObjectInactive(CommandContext::COMMAND);
	}

	clearCommandTextBuffer();

	::SetCursor(NULL);
}

BOOL Document::wm_char_view(WORD code)
{
	BOOL nValue = FALSE;

	Command* lpViewCommand = m_CommandManager.getActiveObject(CommandContext::VIEW);

	if (lpViewCommand != nullptr) {
		int nID = lpViewCommand->getID();
		switch (nID) {
			case IDC_3DORBIT:
			case IDC_3DFORBIT:
			case IDC_3DCORBIT:
			case IDC_3DDISTANCE:
			case IDC_3DSWIVEL:
			case IDC_3DWALK:
			case IDC_3DFLY:
			case IDC_ZOOM:
			case IDC_PAN: {
				switch (code) {
					case 0x31: {//0x31 	1 key
						SendCommandParam(m_hWnd, ID_CONTEXT_MENU_ORBIT, IDC_3DORBIT);
						nValue = TRUE;
						break;
					}
					case 0x32: {//0x32 	2 key
						SendCommandParam(m_hWnd, ID_CONTEXT_MENU_ORBIT, IDC_3DFORBIT);
						nValue = TRUE;
						break;
					}
					case 0x33: {//0x33 	3 key
						SendCommandParam(m_hWnd, ID_CONTEXT_MENU_ORBIT, IDC_3DCORBIT);
						nValue = TRUE;
						break;
					}
					case 0x34: {//0x34 	4 key
						SendCommandParam(m_hWnd, ID_CONTEXT_MENU_ORBIT, IDC_3DDISTANCE);
						nValue = TRUE;
						break;
					}
					case 0x35: {//0x35 	5 key
						SendCommandParam(m_hWnd, ID_CONTEXT_MENU_ORBIT, IDC_3DSWIVEL);
						nValue = TRUE;
						break;
					}
					case 0x36: {//0x36 	6 key
						SendCommandParam(m_hWnd, ID_CONTEXT_MENU_ORBIT, IDC_3DWALK);
						nValue = TRUE;
						break;
					}
					case 0x37: {//0x37 	7 key
						SendCommandParam(m_hWnd, ID_CONTEXT_MENU_ORBIT, IDC_3DFLY);
						nValue = TRUE;
						break;
					}
					case 0x38: {//0x38 	8 key
						SendCommandParam(m_hWnd, ID_CONTEXT_MENU_ORBIT, IDC_ZOOM);
						nValue = TRUE;
						break;
					}
					case 0x39: {//0x39 	9 key
						SendCommandParam(m_hWnd, ID_CONTEXT_MENU_ORBIT, IDC_PAN);
						nValue = TRUE;
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
	}

	return nValue;
}
void Document::wm_char_back()
{
	HRESULT hResult = ::StringCchLength(m_pchInputBuf, MAX_LOADSTRING, &m_cch);
	if (FAILED(hResult)) {
		ErrorHandler();
	}
	else {
		m_nCurChar--;
		char pchInputBuf[MAX_LOADSTRING] = { 0 };
		::strcpy_s(pchInputBuf, m_pchInputBuf);
		for (unsigned int i = m_nCurChar; i < m_cch; i++) {
			if (i > MAX_LOADSTRING - 2) break;
			m_pchInputBuf[i] = pchInputBuf[i + 1];
		}
		m_pchInputBuf[m_cch] = '\0';
	}
}
void Document::wm_char_return()
{
	Command* lpCommand = m_CommandManager.getActiveObject();
	if (lpCommand == nullptr) {
		HRESULT hResult = ::StringCchLength(m_pchInputBuf, MAX_LOADSTRING, &m_cch);//Get text buffer length...
		if (FAILED(hResult)) {//If StringCchLength failed...
			ErrorHandler();
		}
		else {//If StringCchLength succeeded...
			if (m_cch > 0) {//If the text buffer is not empty,...
				SendCommandText(m_hWnd, IDC_DOCUMENT, m_pchInputBuf);//...send command by text.
			}
			else {//If the text buffer is empty,...
				Command* lpRecentCommand = m_CommandManager.getObjects().back();//...get the last command...
				if (lpRecentCommand != nullptr) {//If the last command was not null...
					int nRecentCommandID = lpRecentCommand->getID();//...get the command id...
					SendCommandParam(m_hWnd, IDC_DOCUMENT, nRecentCommandID);//...and send command by id.
				}
			}
		}
	}
	else {
		exitCommand();
	}
}
void Document::wm_char_escape()
{
	Command* lpCommand = m_CommandManager.getActiveObject();
	if (lpCommand == nullptr) {
		SendCommandParam(m_hWnd, IDC_DOCUMENT, IDC_CANCEL);
	}
	else{
		exitCommand();
	}
}
void Document::wm_char_end()
{
	HRESULT hResult = ::StringCchLength(m_pchInputBuf, MAX_LOADSTRING, &m_cch);
	if (FAILED(hResult)) {
		ErrorHandler();
	}
	else {
		m_nCurChar = (int)m_cch;
	}
}
void Document::wm_char_left()
{
	m_nCurChar--;
	if (m_nCurChar < 0) {
		m_nCurChar = 0;
	}
}
void Document::wm_char_right()
{
	m_nCurChar++;
	if (m_nCurChar > (int)m_cch + 1) {
		m_nCurChar = (int)m_cch + 1;
	}
}
void Document::wm_char_default(WPARAM wParam, LPARAM lParam)
{
	//Command* lpViewCommand = m_CommandManager.getActiveObject(TRUE);
	//if (lpViewCommand) {
	//	int nResult = lpViewCommand->wm_char(wParam, lParam);
	//	return;
	//}
	//else {
	//	Command* lpCommand = m_CommandManager.getActiveObject(FALSE);
	//	if (lpCommand) {
	//		int nResult = lpCommand->wm_char(wParam, lParam);
	//		return;
	//	}
	//}

	POINT ptCursor = { 0,0 };
	RECT rcClient = { 0,0,0,0 };

	GetCursorPos(&ptCursor);
	ScreenToClient(m_hWnd, &ptCursor);
	GetClientRect(m_hWnd, &rcClient);
	if (PtInRect(&rcClient, ptCursor) == TRUE) {
		if (showDynamicinput(wParam, lParam) == TRUE) {
			return;
		}
		else {
			if (showCommandline(wParam, lParam) == TRUE) {
				return;
			}
		}
	}
	else {
		if (showCommandline(wParam, lParam) == TRUE) {
			return;
		}
	}

	m_ch = (char)wParam;

	char pchInputBuf[MAX_LOADSTRING]{ 0 };
	::strcpy_s(pchInputBuf, m_pchInputBuf);

	for (int i = 0; i < m_nCurChar; i++) {
		m_pchInputBuf[i] = pchInputBuf[i];
	}
	m_pchInputBuf[m_nCurChar++] = m_ch;
	for (unsigned int i = m_nCurChar; i < m_cch + 1; i++) {
		m_pchInputBuf[i] = pchInputBuf[i - 1];
	}

	if (::islower(m_ch) != 0) {
		char ch = m_ch;
		m_ch = (char)::toupper((int)ch);
		m_pchInputBuf[m_nCurChar - 1] = m_ch;
	}
}

void Document::wm_command_sendcommand(WPARAM wParam, LPARAM lParam)
{
	int nID = LOWORD(wParam);

	switch (nID) {
		case WM_SETTEXT: {
			log("Log::Document::wm_command_sendcommand() WM_SETTEXT");
			wm_command_sendcommand_text(wParam, lParam);
			break;
		}
		default: {
			log("Log::Document::wm_command_sendcommand() default");
			wm_command_sendcommand_param(wParam, lParam);
			break;
		}
	}
}
void Document::wm_command_sendcommand_text(WPARAM wParam, LPARAM lParam)//Commands from the CommandLine are sent here.
{
	int nSource = HIWORD(wParam);
	if (nSource != IDC_DOCUMENT) {//If the commandline input source is NOT the active document, set the documents commandline text buffer to the value received from the source in the lParam function parameter.
		//m_pchInputBuf = (LPSTR)lParam;
		m_pchInputBuf = (char*)lParam;
	}
	//log("Log::Document::wm_command_sendcommand_text() CommandLine text = %s", m_pchInputBuf);

	int nID = CommandManager::getIDByText(m_pchInputBuf);
	//log("Log::Document::wm_command_sendcommand_text() Command ID = %d", nID);

	//When the command input is from the command line, the mouse position variables may not have 
	//the correct values because the user may have clicked on a listbox or textbox;
	//This sets all the mouse position variables to the cursor position;
	::GetCursorPos(&m_ptMouseDown);//Set m_ptMouseDown to cursor position,...
	::ScreenToClient(m_hWnd, &m_ptMouseDown);//...convert to document window coordinates,...
	m_ptMouseMove = m_ptMouseDown;//...and set m_ptMouseMove...
	m_ptMouseUp = m_ptMouseDown;//...and m_ptMouseUp to m_ptMouseDown value.

	switch (nID) {
		case IDC_3DCORBIT: {
			log("Log::Document::wm_command_sendcommand_text() Command = 3DCORBIT");
			createCommandWindow(new C3DOrbit(&CommandManager::s_mCommandByID.at(IDC_3DCORBIT), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_3DDISTANCE: {
			log("Log::Document::wm_command_sendcommand_text() Command = 3DDISTANCE");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DDISTANCE), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_3DFLY: {
			log("Log::Document::wm_command_sendcommand_text() Command = 3DFLY");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DFLY), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_3DFORBIT: {
			log("Log::Document::wm_command_sendcommand_text() Command = 3DFORBIT");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DFORBIT), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_3DO:
		case IDC_3DORBIT: {
			log("Log::Document::wm_command_sendcommand_text() Command = 3DORBIT");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DORBIT), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_3DPOLY: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_3DPOLY");
			break;
		}
		case IDC_3DSWIVEL: {
			log("Log::Document::wm_command_sendcommand_text() Command = 3DSWIVEL");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DSWIVEL), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_3DWALK: {
			log("Log::Document::wm_command_sendcommand_text() Command = 3DWALK");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DWALK), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_ABOUT: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_ABOUT");
			::SendMessage(m_system->getApplication()->getHWND(), WM_COMMAND, MAKEWPARAM(ID_HELP_ABOUT, 0), 0);
			break;
		}
		case IDC_ARC: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_ARC");
			break;
		}
		case IDC_CIRCLE: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_CIRCLE");
			break;
		}
		case IDC_DSETTINGS: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_DSETTINGS");
			createCommandDialog(new dlg_DraftingSettings(&CommandManager::s_mCommandByID.at(IDC_DSETTINGS), m_lpCommandWindows, m_pchInputBuf), IDD_DRAFTINGSETTINGS);
			break;
		}
		case IDC_ELLIPSE: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_ELLIPSE");
			break;
		}
		case IDC_EXIT: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_EXIT");
			PostQuitMessage(0);
			break;
		}
		case IDC_HELIX: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_HELIX");
			break;
		}
		case IDC_LINE: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_LINE");
			createCommandWindow(
				new CLine(
					&CommandManager::s_mCommandByID.at(IDC_LINE),
					m_lpCommandWindows,
					getActiveLayout()->getActiveViewport()->getCursor(WinToGL(m_ptMouseDown)),
					m_pchInputBuf
				)
			);
			break;
		}
		case IDC_MSPACE: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_MSPACE");
			if (getActiveLayout()) {
				getActiveLayout()->setActiveSpace(Space::Context::ModelSpace);
				//m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
				::SendMessage(m_lpGLRenderWindow->getHWND(), WM_PAINT, 0, 0);
			}
			break;
		}
		case IDC_OPTIONS: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_OPTIONS");
			createCommandDialog(new dlg_Options(&CommandManager::s_mCommandByID.at(IDC_OPTIONS), m_lpCommandWindows, m_pchInputBuf), IDD_OPTIONS);
			break;
		}
		case IDC_PAN: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_PAN");
			createCommandWindow(new CPan(&CommandManager::s_mCommandByID.at(IDC_PAN), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_POINT: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_POINT");
			break;
		}
		case IDC_PLINE: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_PLINE");
			break;
		}
		case IDC_POLYGON: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_POLYGON");
			break;
		}
		case IDC_PSPACE: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_PSPACE");
			if (getActiveLayout()) {
				getActiveLayout()->setActiveSpace(Space::Context::PaperSpace);
				//m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
				::SendMessage(m_lpGLRenderWindow->getHWND(), WM_PAINT, 0, 0);
			}
			break;
		}
		case IDC_QUIT: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_QUIT");
			PostQuitMessage(0);
			break;
		}
		case IDC_RAY: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_RAY");
			break;
		}
		case IDC_RECTANGLE: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_RECTANGLE");
			break;
		}
		case IDC_SELECT: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_SELECT");
			createCommandWindow(
				new CSelect(
					&CommandManager::s_mCommandByID.at(IDC_SELECT),
					m_lpCommandWindows,
					getActiveLayout()->getActiveViewport()->getCursor(WinToGL(m_ptMouseMove)),
					m_pchInputBuf,
					getActiveLayout()->getCrosshairColour()
				)
			);
			break;
		}
		case IDC_SETVAR: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_SETVAR");
			::SendMessage(m_system->getApplication()->getHWND(), WM_NOTIFY, wParam, lParam);
			break;
		}
		case IDC_SPLINE: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_SPLINE");
			break;
		}
		case IDC_TILEMODE: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_TILEMODE");
			break;
		}
		case IDC_UNITS: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_UNITS");
			createCommandDialog(new dlg_DrawingUnits(&CommandManager::s_mCommandByID.at(IDC_UNITS), m_lpCommandWindows, m_pchInputBuf), IDD_DRAWINGUNITS);
			break;
		}
		case IDC_UCSICON: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_UCSICON");
			dlg_UCSIcon* lpUCSIcon = new dlg_UCSIcon(
				&CommandManager::s_mCommandByID.at(IDC_UCSICON),
				m_lpCommandWindows,
				m_pchInputBuf,
				static_cast<int>(GLUCSIcon::getStyle()),
				GLUCSIcon::getLineWidth(),
				GLUCSIcon::getSize(),
				ModelspaceUCSIcon::getColour(),
				LayoutTabUCSIcon::getColour()
			);
			int nResult = createCommandDialog(lpUCSIcon, IDD_UCSICON);
			if (nResult == 1) {
				GLUCSIcon::setStyle(static_cast<GLUCSIcon::Style>(lpUCSIcon->getStyle()));//Static cast from int to GLUCSIcon::Style
				GLUCSIcon::setLineWidth(lpUCSIcon->getLinewidth());
				GLUCSIcon::setSize(lpUCSIcon->getSize());
				PSpace::setUCSIconColour(lpUCSIcon->getLayoutTabIconColour());
				MSpace::setUCSIconColour(lpUCSIcon->getModelspaceIconColour());
				if (lpUCSIcon->getModelspaceIconColour() == Colour3<float> {0.f, 0.f, 0.f}) {
					ModelSpace::setUCSIconColour(Colour3<float> { 1.f, 1.f, 1.f });
				}
				else {
					ModelSpace::setUCSIconColour(lpUCSIcon->getModelspaceIconColour());
				}
			}
			break;
		}
		case IDC_UCSMANAGER: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_UCSMANAGER");
			int nActiveTab = 0;
			//getWindowState(WINDOWSTATE_ACTIVETAB, &nActiveTab, "\\Profiles\\<<Unnamed Profile>>\\Dialogs\\Options");
			createCommandDialog(new dlg_UCS(&CommandManager::s_mCommandByID.at(IDC_UCSMANAGER), m_lpCommandWindows, m_pchInputBuf, nActiveTab), IDD_UCS);
			break;
		}
		case IDC_UNKNOWN: {
			createCommandWindow(new CUnknown(&CommandManager::s_mCommandByID.at(IDC_UNKNOWN), m_lpCommandWindows, m_pchInputBuf));
			::SendMessage(m_hWnd, WM_CHAR, (WPARAM)VK_RETURN, 0);
			break;
		}
		case IDC_VIEW: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_VIEW");

			ViewManager* lpViewManager = getViewManager();
			if (lpViewManager == nullptr) {
				ErrorHandler();
				break;
			}

			Layout* lpActiveLayout = getLayoutManager()->getActiveObject();
			if (lpActiveLayout == nullptr) {
				ErrorHandler();
				break;
			}
			if (lpActiveLayout->Viewports() == nullptr) {
				ErrorHandler();
				break;
			}

			Viewport* lpActiveViewport = lpActiveLayout->Viewports()->getActiveObject();
			if (lpActiveViewport == nullptr) {
				ErrorHandler();
				break;
			}

			createCommandDialog(new dlg_ViewManager(&CommandManager::s_mCommandByID.at(IDC_VIEW), m_lpCommandWindows, m_pchInputBuf, lpViewManager, lpActiveViewport->extents), IDD_VIEWMANAGER);

			break;
		}
		case IDC__VIEW: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC__VIEW");

			if (getActiveLayout() == nullptr) {
				break;
			}

			View::PresetView enPresetView = static_cast<View::PresetView>(HIWORD(lParam));
			getActiveLayout()->getActiveSpace()->setPresetView(enPresetView);
			//m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
			::SendMessage(m_lpGLRenderWindow->getHWND(), WM_PAINT, 0, 0);

			break;
		}
		case IDC_VPORTS: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_VPORTS");
			//if (getActiveLayout()) {
			//	ViewportManager::LPVIEWPORTDATA lpVprtConfigData = (ViewportManager::LPVIEWPORTDATA)lpcmdhdr->lParam;
			//	getActiveLayout()->getActiveSpace()->setViewportConfiguration(lpVprtConfigData);
			//	m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
			//}
			if (getActiveLayout() == nullptr) {
				break;
			}

			Layout* lpActiveLayout = getLayoutManager()->getActiveObject();
			if (lpActiveLayout == nullptr) {
				ErrorHandler();
				break;
			}

			ViewportManager* lpViewportManager = lpActiveLayout->Viewports();
			if (lpViewportManager == nullptr) {
				ErrorHandler();
				return;
			}

			int nActiveTab = (int)HIWORD(lParam);
			createCommandDialog(new dlg_Viewports(&CommandManager::s_mCommandByID.at(IDC_VPORTS), m_lpCommandWindows, m_pchInputBuf, lpViewportManager, nActiveTab), IDD_VIEWPORTS);

			break;
		}
		case IDC__VPORTS: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC__VPORTS");


			break;
		}
		case IDC_VSCURRENT: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_VSCURRENT");
			//if (getActiveLayout()) {
			//	View::VisualStyle enVisualStyle = static_cast<View::VisualStyle>(lpcmdhdr->val);
			//	getActiveLayout()->getActiveSpace()->setVisualStyle(enVisualStyle);
			//}
			break;
		}
		case IDC_XLINE: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_XLINE");
			break;
		}
		case IDC_ZOOM: {
			log("Log::Document::wm_command_sendcommand_text() Command = IDC_ZOOM");
			createCommandWindow(new CZoom(&CommandManager::s_mCommandByID.at(IDC_ZOOM), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		default: {
			break;
		}
	}
}
void Document::wm_command_sendcommand_param(WPARAM wParam, LPARAM lParam)//Commands from the UI are sent here.
{
	int nID = LOWORD(wParam);
	int nSource = HIWORD(wParam);

	//log("Log::Document::wm_command_sendcommand_param() Command ID = %d", nID);

	switch (nID) {
		case IDC_3DCORBIT: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_3DCORBIT");
			createCommandWindow(new C3DOrbit(&CommandManager::s_mCommandByID.at(IDC_3DCORBIT), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_3DDISTANCE: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_3DDISTANCE");
			createCommandWindow(new CView3D(&CommandManager::s_mCommandByID.at(IDC_3DDISTANCE), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_3DFLY: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_3DFLY");
			createCommandWindow(new CView3D(&CommandManager::s_mCommandByID.at(IDC_3DFLY), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_3DFORBIT: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_3DFORBIT");
			createCommandWindow(new CView3D(&CommandManager::s_mCommandByID.at(IDC_3DFORBIT), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_3DO:
		case IDC_3DORBIT: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_3DORBIT");
			createCommandWindow(new C3DOrbit(&CommandManager::s_mCommandByID.at(IDC_3DORBIT), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_3DPOLY: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_3DPOLY");
			break;
		}
		case IDC_3DSWIVEL: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_3DSWIVEL");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DSWIVEL), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_3DWALK: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_3DWALK");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DWALK), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_ABOUT: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_ABOUT");
			::SendMessage(m_system->getApplication()->getHWND(), WM_COMMAND, MAKEWPARAM(ID_HELP_ABOUT, 0), 0);
			break;
		}
		case IDC_ARC: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_ARC");
			break;
		}
		case IDC_CANCEL: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_CANCEL");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_CANCEL), m_lpCommandWindows));
			exitCommand();
			break;
		}
		case IDC_CIRCLE: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_CIRCLE");
			break;
		}
		case IDC_DSETTINGS: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_DSETTINGS");
			createCommandDialog(new dlg_DraftingSettings(&CommandManager::s_mCommandByID.at(IDC_DSETTINGS), m_lpCommandWindows), IDD_DRAFTINGSETTINGS);
			break;
		}
		case IDC_ELLIPSE: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_ELLIPSE");
			break;
		}
		case IDC_EXIT: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_EXIT");
			PostQuitMessage(0);
			break;
		}
		case IDC_HELIX: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_HELIX");
			break;
		}
		case IDC_LAYER: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_LAYER");
			//createCommandDialog(new plt_LayerPropertiesManager(&CommandManager::s_mCommandByID.at(IDC_LAYER), m_lpCommandWindows), IDD_OPTIONS);
			break;
		}
		case IDC_LINE: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_LINE");
			createCommandWindow(new CLine(&CommandManager::s_mCommandByID.at(IDC_LINE), m_lpCommandWindows, getActiveLayout()->getActiveViewport()->getCursor(WinToGL(m_ptMouseDown))));
			break;
		}
		case IDC_MSPACE: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_MSPACE");
			if (getActiveLayout()) {
				getActiveLayout()->setActiveSpace(Space::Context::ModelSpace);
				//m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
				::SendMessage(m_lpGLRenderWindow->getHWND(), WM_PAINT, 0, 0);
			}
			break;
		}
		case IDC_OPTIONS: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_OPTIONS");
			createCommandDialog(new dlg_Options(&CommandManager::s_mCommandByID.at(IDC_OPTIONS), m_lpCommandWindows), IDD_OPTIONS);
			break;
		}
		case IDC_PAN: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_PAN");
			if (nSource == ID_CONTEXT_MENU_ORBIT) {
				createCommandWindow(new CPan3D(&CommandManager::s_mCommandByID.at(IDC_PAN), m_lpCommandWindows), CommandContext::VIEW);
			}
			else {
				createCommandWindow(new CPan(&CommandManager::s_mCommandByID.at(IDC_PAN), m_lpCommandWindows), CommandContext::VIEW);
			}
			break;
		}
		case IDC_POINT: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_POINT");
			break;
		}
		case IDC_PLINE: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_PLINE");
			break;
		}
		case IDC_POLYGON: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_POLYGON");
			break;
		}
		case IDC_PSPACE: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_PSPACE");
			if (getActiveLayout()) {
				getActiveLayout()->setActiveSpace(Space::Context::PaperSpace);
				//m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
				::SendMessage(m_lpGLRenderWindow->getHWND(), WM_PAINT, 0, 0);
			}
			break;
		}
		case IDC_QUIT: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_QUIT");
			PostQuitMessage(0);
			break;
		}
		case IDC_RAY: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_RAY");
			break;
		}
		case IDC_RECTANGLE: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_RECTANGLE");
			break;
		}
		case IDC_SELECT: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_SELECT");
			createCommandWindow(
				new CSelect(
					&CommandManager::s_mCommandByID.at(IDC_SELECT),
					m_lpCommandWindows,
					getActiveLayout()->getActiveViewport()->getCursor(WinToGL(m_ptMouseDown)),
					getActiveLayout()->getCrosshairColour()
				)
			);
			break;
		}
		case IDC_SETVAR: {
			log("Log::Document::wm_command_sendcommand_param() Command = SETVAR");
			::SendMessage(m_system->getApplication()->getHWND(), WM_COMMAND, wParam, lParam);
			break;
		}
		case IDC_SPLINE: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_SPLINE");
			break;
		}
		case IDC_TILEMODE: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_TILEMODE");
			break;
		}
		case IDC_UNITS: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_UNITS");
			createCommandDialog(new dlg_DrawingUnits(&CommandManager::s_mCommandByID.at(IDC_UNITS), m_lpCommandWindows), IDD_DRAWINGUNITS);
			break;
		}
		case IDC_UCSICON: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_UCSICON");
			dlg_UCSIcon* lpUCSIcon = new dlg_UCSIcon(
				&CommandManager::s_mCommandByID.at(IDC_UCSICON),
				m_lpCommandWindows,
				static_cast<int>(GLUCSIcon::getStyle()),
				GLUCSIcon::getLineWidth(),
				GLUCSIcon::getSize(),
				ModelspaceUCSIcon::getColour(),
				LayoutTabUCSIcon::getColour()
			);
			int nResult = createCommandDialog(lpUCSIcon, IDD_UCSICON);
			if (nResult == 1) {
				GLUCSIcon::setStyle(static_cast<GLUCSIcon::Style>(lpUCSIcon->getStyle()));//Static cast from int to GLUCSIcon::Style
				GLUCSIcon::setLineWidth(lpUCSIcon->getLinewidth());
				GLUCSIcon::setSize(lpUCSIcon->getSize());
				PSpace::setUCSIconColour(lpUCSIcon->getLayoutTabIconColour());
				MSpace::setUCSIconColour(lpUCSIcon->getModelspaceIconColour());
				if (lpUCSIcon->getModelspaceIconColour() == Colour3<float> {0.f, 0.f, 0.f}) {
					ModelSpace::setUCSIconColour(Colour3<float> { 1.f, 1.f, 1.f });
				}
				else {
					ModelSpace::setUCSIconColour(lpUCSIcon->getModelspaceIconColour());
				}
			}
			break;
		}
		case IDC_UCSMANAGER: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_UCSMANAGER");
			createCommandDialog(new dlg_UCS(&CommandManager::s_mCommandByID.at(IDC_UCSMANAGER), m_lpCommandWindows, 0), IDD_UCS);
			break;
		}
		case IDC_VIEW: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_VIEW");

			ViewManager* lpViewManager = getViewManager();
			if (lpViewManager == nullptr) {
				ErrorHandler();
				break;
			}

			Layout* lpActiveLayout = getLayoutManager()->getActiveObject();
			if (lpActiveLayout == nullptr) {
				ErrorHandler();
				break;
			}
			if (lpActiveLayout->Viewports() == nullptr) {
				ErrorHandler();
				break;
			}

			Viewport* lpActiveViewport = lpActiveLayout->Viewports()->getActiveObject();
			if (lpActiveViewport == nullptr) {
				ErrorHandler();
				break;
			}

			createCommandDialog(new dlg_ViewManager(&CommandManager::s_mCommandByID.at(IDC_VIEW), m_lpCommandWindows, lpViewManager, lpActiveViewport->extents), IDD_VIEWMANAGER);

			break;
		}
		case IDC__VIEW: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC__VIEW");

			if (getActiveLayout() == nullptr) {
				break;
			}

			//if (nSource == IDD_VIEWMANAGER) {
			//
			//}

			View::PresetView enPresetView = static_cast<View::PresetView>(HIWORD(lParam));
			getActiveLayout()->getActiveSpace()->setPresetView(enPresetView);
			//m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
			::SendMessage(m_lpGLRenderWindow->getHWND(), WM_PAINT, 0, 0);

			break;
		}
		case IDC_VPORTS: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_VPORTS");

			if (getActiveLayout() == nullptr) {
				break;
			}

			Layout* lpActiveLayout = getLayoutManager()->getActiveObject();
			if (lpActiveLayout == nullptr) {
				ErrorHandler();
				break;
			}

			ViewportManager* lpViewportManager = lpActiveLayout->Viewports();
			if (lpViewportManager == nullptr) {
				ErrorHandler();
				return;
			}

			int nActiveTab = (int)HIWORD(lParam);
			int nResult = createCommandDialog(new dlg_Viewports(&CommandManager::s_mCommandByID.at(IDC_VPORTS), m_lpCommandWindows, lpViewportManager, nActiveTab), IDD_VIEWPORTS);
			if (nResult == IDOK) {

			}

			break;
		}
		case IDC__VPORTS: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC__VPORTS");

			if (getActiveLayout() == nullptr) {
				break;
			}

			ViewportManager::LPVIEWPORTDATA lpVprtConfigData = (ViewportManager::LPVIEWPORTDATA)lParam;
			getActiveLayout()->getActiveSpace()->setViewportConfiguration(lpVprtConfigData);
			//m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
			::SendMessage(m_lpGLRenderWindow->getHWND(), WM_PAINT, 0, 0);

			break;
		}
		case IDC_VSCURRENT: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_VSCURRENT");
			if (getActiveLayout()) {
				View::VisualStyle enVisualStyle = static_cast<View::VisualStyle>(HIWORD(lParam));
				getActiveLayout()->getActiveSpace()->setVisualStyle(enVisualStyle);
			}
			break;
		}
		case IDC_XLINE: {
			log("Log::Document::wm_command_sendcommand_param() Command = IDC_XLINE");
			break;
		}
		case IDC_ZOOM: {
			log("Log::Document::wm_command_sendcommand_param() Command = ZOOM");
			if (nSource == ID_CONTEXT_MENU_ORBIT) {
				createCommandWindow(new CZoom3D(&CommandManager::s_mCommandByID.at(IDC_ZOOM), m_lpCommandWindows, 0), CommandContext::VIEW);
			}
			else {
				createCommandWindow(new CZoom(&CommandManager::s_mCommandByID.at(IDC_ZOOM), m_lpCommandWindows), CommandContext::VIEW);
			}
			break;
		}
		default: {
			break;
		}
	}
}

void Document::wm_create_Menu()
{
	m_hMenu = ::CreateMenu();

	HMENU hFile = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_FILE));
	HMENU hEdit = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_EDIT));
	HMENU hView = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_VIEW));
	HMENU hInsert = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_INSERT));
	HMENU hFormat = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_FORMAT));
	HMENU hTools = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_TOOLS));
	HMENU hDraw = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_DRAW));
	HMENU hDimension = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_DIMENSION));
	HMENU hModify = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_MODIFY));
	HMENU hWindow = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_WINDOW));
	HMENU hHelp = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_HELP));
	HMENU hExpress = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_EXPRESS));
	HMENU hParametric = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_PARAMETRIC));
	HMENU hRaster = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_RASTER));

	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hFile, "&File");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hEdit, "&Edit");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hView, "&View");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hInsert, "&Insert");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hFormat, "F&ormat");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hTools, "&Tools");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hDraw, "&Draw");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hDimension, "Dime&nsion");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hModify, "&Modify");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hWindow, "&Window");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hHelp, "&Help");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hExpress, "E&xpress");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hParametric, "&Parametric");
	::AppendMenu(m_hMenu, MF_POPUP, (UINT_PTR)hRaster, "R&aster");
}

void Document::wm_notify_ttn_getdispinfo_Layouts(UINT_PTR idFrom, LPSTR& lpszText)
{
	BOOL nResult = FALSE;

	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT;
	tcItem.pszText = lpszText;
	tcItem.cchTextMax = MAX_LOADSTRING;
	nResult = (BOOL)::SendMessage(m_hTabcontrol_Layouts, TCM_GETITEM, (WPARAM)(unsigned int)idFrom, (LPARAM)&tcItem);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	//if (getLayoutManager() == nullptr) return;
	//if (getLayoutManager()->getSize() == 0) return;//Exit if there are no layouts...

	//Layout* lpLayout = getLayoutManager()->getObjects()[(unsigned int)idFrom];

	//lpszText = lpLayout->getName();
	//lpszText = tcItem.pszText;
}
void Document::wm_notify_ttn_getdispinfo(NMHDR nmHDR, LPSTR& lpszText)
{
	if (nmHDR.hwndFrom == m_hTabcontrol_Layouts_Tooltip) {
		wm_notify_ttn_getdispinfo_Layouts(nmHDR.idFrom, lpszText);
	}
}

void Document::wm_notify_tcn_selchange_layouts()
{
	int itemIndex = ::SendMessage(m_hTabcontrol_Layouts, TCM_GETCURSEL, 0, 0);

	if (getLayoutManager() == nullptr) return;
	if (getLayoutManager()->getSize() == 0) return;

	if (itemIndex >= getLayoutManager()->getSize()) {
		return;
	}

	//Set current active layout...
	//TCITEM tcItem{ TCIF_TEXT | TCIF_PARAM };
	//::SendMessage(m_hTabcontrol_Layouts, TCM_GETITEM, (WPARAM)itemIndex, (LPARAM)&tcItem);
	//Layout* lpActiveLayout = (Layout*)tcItem.lParam;
	Layout* lpActiveLayout = getLayoutManager()->setActiveLayout(itemIndex);
	if (lpActiveLayout == nullptr) return;

	//Paint active document...
	NMHDR nmh;
	nmh.hwndFrom = m_hWnd;
	nmh.idFrom = IDC_TABCTRL_LAYOUTS;
	nmh.code = TCN_SELCHANGE;
	SendMessage(m_lpGLRenderWindow->getHWND(), WM_NOTIFY, (WPARAM)IDC_TABCTRL_LAYOUTS, (LPARAM)&nmh);
}
void Document::wm_notify_tcn_selchange_document_callback()
{
	INT nResult = 0;

	if (getLayoutManager() == nullptr) return;
	if (getLayoutManager()->getSize() == 0) return;

	Layout* lpLayout = nullptr;
	TCITEM tcItem{ TCIF_TEXT | TCIF_PARAM, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < getLayoutManager()->getSize(); i++) {
		lpLayout = getLayoutManager()->getObjects()[i];//Get layout by index...
		if (lpLayout != nullptr) {
			tcItem.pszText = (LPSTR)lpLayout->getName();//Set tab item text...
			tcItem.lParam = (LPARAM)lpLayout;//Set tab item lparam...
			nResult = (INT)::SendMessage(m_hTabcontrol_Layouts, TCM_INSERTITEM, (WPARAM)i, (LPARAM)&tcItem);//Insert tab (returns index of new tab if successful, or -1 otherwise).
			if (nResult == -1) {
				ErrorHandler();
			}
		}
	}

	//RECT rcTab = { 0,0,0,0 };
	//nResult = (INT)::SendMessage(m_hTabcontrol_Layouts, TCM_GETITEMRECT, 0, (LPARAM)&rcTab);
	//if (nResult == TRUE) {
	//	DWORD dwResult = (DWORD)::SendMessage(m_hTabcontrol_Layouts, TCM_SETITEMSIZE, 0, MAKELPARAM(10, rcTab.bottom - rcTab.top));
	//	int nWidth = LOWORD(dwResult);
	//	int nHeight = HIWORD(dwResult);
	//}
	
	tcItem.pszText = (LPSTR)"+";//Set tab item text...
	nResult = (INT)::SendMessage(m_hTabcontrol_Layouts, TCM_INSERTITEM, (WPARAM)getLayoutManager()->getSize(), (LPARAM)&tcItem);//Insert tab (returns index of new tab if successful, or -1 otherwise).
	if (nResult == -1) {
		ErrorHandler();
	}

	nResult = (INT)::SendMessage(m_hTabcontrol_Layouts, TCM_SETCURSEL, 0, 0);//Set first (Model) tab as current (returns index of new tab if successful, or -1 otherwise).
	if (nResult == -1) {
		ErrorHandler();
	}
}
void Document::wm_notify_tcn_selchange_layout_callback()
{
	BOOL nResult = FALSE;

	if (getLayoutManager() == nullptr) return;
	if (getLayoutManager()->getSize() == 0) return;

	Layout* lpActiveLayout = getLayoutManager()->getActiveObject();
	if (lpActiveLayout == nullptr) return;

	::SendMessage(m_lpGLRenderWindow->getHWND(), WM_PAINT, 0, 0);

	//Set state of menu items based on Model/Paper layout...
	int itemIndex = ::SendMessage(m_hTabcontrol_Layouts, TCM_GETCURSEL, 0, 0);

	HMENU hMenu = ::GetMenu(m_system->getApplication()->getHWND());
	HMENU hSubMenu = ::GetSubMenu(hMenu, 2);
	HMENU hSubSubMenu = ::GetSubMenu(hSubMenu, 12);

	MENUITEMINFO mii{ 0 };
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;

	if (itemIndex != 0) {
		mii.fState = MFS_ENABLED;
	}
	else {
		mii.fState = MFS_DISABLED;
	}

	nResult = (BOOL)::SetMenuItemInfo(hSubSubMenu, ID_VIEW_VIEWPORTS_POLYGONAL_VIEWPORTS, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	nResult = (BOOL)::SetMenuItemInfoA(hSubSubMenu, ID_VIEW_VIEWPORTS_OBJECT, MF_BYCOMMAND, &mii);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	nResult = ::DrawMenuBar(m_hWnd);
	if (nResult == FALSE) {
		ErrorHandler();
	}

	HWND hStatusbar = ::GetDlgItem(m_system->getApplication()->getHWND(), IDC_STATUSBAR);
	//setStatusbarText(hStatusbar, 1);
	setStatusbarText(hStatusbar, 2);
	setDynamicInputStyle();
}
void Document::wm_notify_tcn_selchange(UINT_PTR idFrom)
{
	switch (idFrom) {
		case IDC_TABCTRL_LAYOUTS: {
			log("Log::Document::wm_notify() TCN_SELCHANGE lpnm->idFrom = IDC_TABCTRL_LAYOUTS");
			wm_notify_tcn_selchange_layouts();
			break;
		}
		case IDC_DOCUMENT: {//Receives IDC_DOCUMENT from render thread on create.
			log("Log::Document::wm_notify() TCN_SELCHANGE lpnm->idFrom = IDC_DOCUMENT");
			wm_notify_tcn_selchange_document_callback();
			break;
		}
		case IDC_LAYOUT: {//Render thread returns WM_NOTIFY on receipt of WM_NOTIFY message so that the document knows the renderthread is readu to paint.
			log("Log::Document::wm_notify() TCN_SELCHANGE lpnm->idFrom = IDC_LAYOUT");
			wm_notify_tcn_selchange_layout_callback();
			break;
		}
		default: {
			log("Log::Document::wm_notify() TCN_SELCHANGE lpnm->idFrom = default");
			break;
		}
	}
}

int Document::wm_notify_tcn_selchanging_layouts()
{
	int nResult = 0;

	if ((m_CommandManager.getActiveObject(CommandContext::COMMAND) != nullptr) || (m_CommandManager.getActiveObject(CommandContext::VIEW) != nullptr)) {
		nResult = 1;
	}

	int itemIndex = ::SendMessage(m_hTabcontrol_Layouts, TCM_GETCURSEL, 0, 0);
	if (itemIndex >= getLayoutManager()->getSize() - 1) {
		nResult = 1;
	}

	return nResult;
}
int Document::wm_notify_tcn_selchanging_layout_callback()
{
	return 0;
}
int Document::wm_notify_tcn_selchanging(UINT_PTR idFrom)//Prevents the layout tabcontrol from reacting to inputs if a command is active.
{
	int nResult = 0;
	switch (idFrom) {
		case IDC_TABCTRL_LAYOUTS: {
			log("Log::Document::wm_notify() TCN_SELCHANGING lpnm->idFrom = IDC_TABCTRL_LAYOUTS");
			nResult = wm_notify_tcn_selchanging_layouts();
			break;
		}
		case IDC_LAYOUT: {
			log("Log::Document::wm_notify() TCN_SELCHANGING lpnm->idFrom = IDC_LAYOUT");
			nResult = wm_notify_tcn_selchanging_layout_callback();
			break;
		}
		default: {
			log("Log::Document::wm_notify() TCN_SELCHANGING lpnm->idFrom = default");
			break;
		}
	}
	return nResult;
}
