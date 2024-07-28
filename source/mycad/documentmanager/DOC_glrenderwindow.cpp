#include "SYS_systemwin32.h"

#include "DOC_glrenderwindow.h"
#include "DOC_resource.h"

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

#define MAKEPOINT(lParam) POINT({ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) });

using namespace mycad;

bool GLRenderWindow::s_bScrollbars{ false };
bool GLRenderWindow::s_bContinuousSize{ true };

GLRenderWindow::GLRenderWindow(DynamicInputWindow* commandwindows[], const char* filename)
	: WindowWin32(),
	m_lpCommandWindows{ commandwindows[0],commandwindows[1],commandwindows[2] },
	dwThreadId(0),
	m_lpGLRenderContext(nullptr),
	m_lpGLRenderThread(nullptr),
	m_lpRenderThread(nullptr),
	m_lpFrameThread(nullptr),
	m_lpFPSThread(nullptr),
	m_lpDataThread(nullptr),
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
	log("Log::GLRenderWindow::GLRenderWindow()");
	m_pchInputBuf = (LPTSTR)::GlobalAlloc(GPTR, MAX_LOADSTRING * sizeof(char));
	m_lpGLRenderThread = new GLRenderThread(&m_CommandManager);//Create new GLRenderThread class...
	::strcpy_s(m_szFileName, filename);
	//dwThreadId = GetCurrentThreadId();
}
GLRenderWindow::~GLRenderWindow()
{
	log("Log::GLRenderWindow::~GLRenderWindow()");

	delete m_lpFPSThread;
	m_lpFPSThread = nullptr;

	delete m_lpRenderThread;
	m_lpRenderThread = nullptr;

	delete m_lpDataThread;
	m_lpDataThread = nullptr;

	delete m_lpGLRenderThread;
	m_lpGLRenderThread = nullptr;

	m_pchInputBuf = 0;
	GlobalFree((HGLOBAL)m_pchInputBuf);
}

void GLRenderWindow::setSplitterThickness(long thickness) { Splitter::setThickness(thickness); }

//Set/get interface colours by context & interface element
void GLRenderWindow::setColour(Context context, InterfaceElement interfaceelement, COLORREF colour, BOOL tintforxyz)
{
	Colour3<float> c3fColour{ 0.f,0.f ,0.f };

	c3fColour.r = (float)GetRValue(colour) / 255.f;
	c3fColour.g = (float)GetGValue(colour) / 255.f;
	c3fColour.b = (float)GetBValue(colour) / 255.f;

	bool bTintforxyz = (bool)tintforxyz;

	switch (context) {
		case Context::ModelSpace2D: {
			setModelSpace2D(interfaceelement, c3fColour, bTintforxyz);
			break;
		}
		case Context::Sheet_Layout: {
			setLayout(interfaceelement, c3fColour, bTintforxyz);
			break;
		}
		case Context::ParallelProjection3D: {
			setParallelProjection3D(interfaceelement, c3fColour, bTintforxyz);
			break;
		}
		case Context::PerspectiveProjection3D: {
			setPerspectiveProjection3D(interfaceelement, c3fColour, bTintforxyz);
			break;
		}
		default: {
			break;
		}
	}
}
COLORREF GLRenderWindow::getColour(Context context, InterfaceElement interfaceelement)
{
	UNREFERENCED_PARAMETER(context);
	UNREFERENCED_PARAMETER(interfaceelement);
	return (COLORREF)RGB(0, 0, 0);
}//TODO - finish function to return colours

//Set/get system interface colours by interface element
void GLRenderWindow::setSystemColour(SystemInterfaceElement sysinterfaceelement, COLORREF colour)
{
	Colour3<float> c3fColour{ 0.f,0.f ,0.f };

	c3fColour.r = (float)GetRValue(colour) / 255.f;
	c3fColour.g = (float)GetGValue(colour) / 255.f;
	c3fColour.b = (float)GetBValue(colour) / 255.f;

	switch (sysinterfaceelement) {
		case SystemInterfaceElement::ActiveBorder: {
			ModelLayout::setBackgroundColour(c3fColour);
			break;
		}
		default: {
			break;
		}
	}
}
COLORREF GLRenderWindow::getSystemColour(SystemInterfaceElement sysinterfaceelement)
{
	UNREFERENCED_PARAMETER(sysinterfaceelement);
	return COLORREF RGB(0, 0, 0);
}//TODO - finish function to return system colours

void GLRenderWindow::setFileName(const char* filename) { ::strcpy_s(m_szFileName, filename); }
char* GLRenderWindow::getFileName() { return m_szFileName; }

Vector3<double> GLRenderWindow::getCoordinates() { return m_lpGLRenderThread->getCoordinates(); }

LayerManager* GLRenderWindow::getLayerManager() { return m_lpGLRenderThread->getLayerManager(); }
LayoutManager* GLRenderWindow::getLayoutManager() { return m_lpGLRenderThread->getLayoutManager(); }
Layout* GLRenderWindow::getActiveLayout()
{
	if (m_lpGLRenderThread->getLayoutManager() != nullptr) {
		if (m_lpGLRenderThread->getLayoutManager()->getActiveObject() != nullptr) {
			return m_lpGLRenderThread->getLayoutManager()->getActiveObject();
		}
	}
	return nullptr;
}
ViewManager* GLRenderWindow::getViewManager() { return m_lpGLRenderThread->getViewManager(); }
ViewportManager* GLRenderWindow::getViewports() { return m_lpGLRenderThread->getLayoutManager()->getActiveObject()->Viewports(); }
Command* GLRenderWindow::getActiveCommand() { return m_CommandManager.getActiveObject(); }
CommandManager* GLRenderWindow::getCommandManager() { return &m_CommandManager; }

BOOL GLRenderWindow::setStatusbarText(HWND hstatusbar, int index)
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

			Vector3<double> v3dCoordinates = m_lpGLRenderThread->getCoordinates();

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
BOOL GLRenderWindow::setDynamicInputStyle()
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

int GLRenderWindow::resumeRenderThread()
{
	log("Log::Drawing[%d]::resume()...........................................................", m_lpRenderThread->get_id());
	if (m_lpRenderThread != nullptr) {
		ResumeThread(m_lpFPSThread->native_handle());
		ResumeThread(m_lpFrameThread->native_handle());
		ResumeThread(m_lpRenderThread->native_handle());
	}

	HRESULT hResult = ErrorHandler();
	if (FAILED(hResult)) {
		return -1;
	}

	return 0;
}
int GLRenderWindow::suspendRenderThread()
{
	log("Log::Drawing[%d]::suspend()..........................................................", m_lpRenderThread->get_id());
	if (m_lpRenderThread != nullptr) {
		SuspendThread(m_lpRenderThread->native_handle());
		SuspendThread(m_lpFrameThread->native_handle());
		SuspendThread(m_lpFPSThread->native_handle());
	}

	HRESULT hResult = ErrorHandler();
	if (FAILED(hResult)) {
		return -1;
	}

	return 0;
}

//Public window functions.
int GLRenderWindow::wm_activate(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (LOWORD(wParam)) {
		case WA_INACTIVE: {
			log("Log::GLRenderWindow::wm_activate() WA_INACTIVE");
			suspendRenderThread();
			break;
		case WA_ACTIVE:
		case WA_CLICKACTIVE: {
			log("Log::GLRenderWindow::wm_activate() WA_ACTIVE");
			resumeRenderThread();
			break;
		}
		default:
			log("Log::GLRenderWindow::wm_activate() default");
			resumeRenderThread();
			break;
		}
	}

	return 0;
}
int GLRenderWindow::wm_char(WPARAM wParam, LPARAM lParam)//Contains functions for editing the command text buffer.
{
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

	//::SetWindowPos(m_lpCommandWindows[1]->getHWND(), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	BOOL nResult = wm_char_view(vkCode);
	if (nResult == TRUE) {
		return 0;
	}

	Command* lpCommand = m_CommandManager.getActiveObject();
	if (lpCommand != nullptr) {
		LRESULT lResult = ::SendMessage(lpCommand->getHWND(), WM_CHAR, wParam, lParam);
		if (lResult == TRUE) {
			return 0;
		}
	}

	log("Log::GLRenderWindow::wm_char() vkCode = %d", vkCode);

	switch (vkCode) {
		case VK_BACK: {			// 0x08		BACKSPACE key
			wm_char_back();
			break;
		}
		case VK_RETURN: {		// 0x0D		ENTER key
			wm_char_return();
			break;
		}
		case VK_ESCAPE: {		// 0x1B		ESC key
			wm_char_escape();
			break;
		}
		case VK_END: {			// 0x23		END key
			wm_char_end();
			break;
		}
		case VK_LEFT: {			// 0x25		LEFT ARROW key
			wm_char_left();
			break;
		}
		case VK_RIGHT: {		// 0x27		RIGHT ARROW key
			wm_char_right();
			break;
		}
		default: {
			wm_char_default(wParam, lParam);
			break;
		}
	}

	log("Log::GLRenderWindow::wm_char() m_pchInputBuf = %s", m_pchInputBuf);

	return 0;
}
int GLRenderWindow::wm_close()
{
	resumeRenderThread();

	m_lpGLRenderThread->SendThreadMessage(WM_CLOSE);

	if (m_lpFPSThread) {
		m_lpFPSThread->join();
	}

	if (m_lpFrameThread) {
		m_lpFrameThread->join();
	}

	if (m_lpRenderThread) {
		m_lpRenderThread->join();
	}

	if (m_lpDataThread) {
		m_lpDataThread->join();
	}

	::DestroyWindow(m_hWnd);

	return 0;
}
int GLRenderWindow::wm_command(WPARAM wParam, LPARAM lParam)
{
	int nSource = HIWORD(wParam);

	switch (nSource) {
		//Application...
		case IDC_APPLICATION: {
			log("Log::GLRenderWindow::wm_command() nSource = IDC_APPLICATION");
			break;
		}
		//GLRenderWindow...
		case IDC_DOCUMENT: {
			log("Log::GLRenderWindow::wm_command() nSource = IDC_DOCUMENT");
			break;
		}
		//Command line windows...
		case IDC_TEXTWINDOW_EDIT_COMMAND:
		case IDC_TEXTWINDOW_DROPDOWNWINDOW_LISTBOX_COMMANDS: {
			log("Log::GLRenderWindow::wm_command() nSource = IDC_TEXTWINDOW");
			break;
		}
		case IDC_COMMANDLINE_EDIT_COMMAND:
		case IDC_COMMANDLINE_DROPDOWNWINDOW_LISTBOX_COMMANDS: {
			log("Log::GLRenderWindow::wm_command() nSource = IDC_COMMANDLINE");
			break;
		}
		case IDC_DYNAMICINPUT_EDIT_COMMAND:
		case IDC_DYNAMICINPUT_DROPDOWNWINDOW_LISTBOX_COMMANDS: {
			log("Log::GLRenderWindow::wm_command() nSource = IDC_DYNAMICINPUT");
			break;
		}
		//Tool palettes...
		case IDC_LAYERPROPERTIESMANAGER: {
			log("Log::GLRenderWindow::wm_command() nSource = IDC_LAYERPROPERTIESMANAGER");
			break;
		}
		//Dialogs...
		case IDD_DRAFTINGSETTINGS: {
			log("Log::GLRenderWindow::wm_command() nSource = IDD_DRAFTINGSETTINGS");
			break;
		}
		case IDD_DRAWINGUNITS: {
			log("Log::GLRenderWindow::wm_command() nSource = IDD_DRAWINGUNITS");
			break;
		}
		case IDD_DRAWINGWINDOWCOLOURS: {
			log("Log::GLRenderWindow::wm_command() nSource = IDD_DRAWINGWINDOWCOLOURS");
			break;
		}
		case IDD_OPTIONS: {
			log("Log::GLRenderWindow::wm_command() nSource = IDD_OPTIONS");
			break;
		}
		case IDD_UCS: {
			log("Log::GLRenderWindow::wm_command() nSource = IDD_UCS");
			break;
		}
		case IDD_UCSICON: {
			log("Log::GLRenderWindow::wm_command() nSource = IDD_UCSICON");
			break;
		}
		case IDD_VIEWMANAGER: {
			log("Log::GLRenderWindow::wm_command() nSource = IDD_VIEWMANAGER");
			break;
		}
		case IDD_VIEWPORTS: {
			log("Log::GLRenderWindow::wm_command() nSource = IDD_VIEWPORTS");
			break;
		}
		default: {
			break;
		}
	}

	wm_command_sendcommand(wParam, lParam);

	return 0;
}
int GLRenderWindow::wm_contextmenu(WPARAM wParam, LPARAM lParam)
{
	Command* lpCommand = getActiveCommand();
	if (lpCommand != nullptr) {
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		LRESULT lResult = ::SendMessage(lpCommand->getHWND(), WM_CONTEXTMENU, wParam, lParam);
		if (lResult == TRUE) {

			return 0;
		}
	}

	if (m_CommandManager.getActiveObject(CommandContext::VIEW) == nullptr) {
		if (getActiveLayout()->getActiveViewport()->getCamera()->getPerspective() == true) {

		}
	}

	HMENU hMenu = ::LoadMenu(s_hInstance, MAKEINTRESOURCE(ID_SHORTCUT_MENU_DEFAULT));
	HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
	POINT ptCursor = MAKEPOINT(lParam);

	if (ptCursor.x == -1 && ptCursor.y == -1) {//If the coordinates are (-1, -1), then the user used the keyboard - we'll pretend the user clicked at client (0, 0).
		ptCursor = { 0,0 };
	}

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
		case ID_CONTEXT_REPEAT: {

			break;
		}
		case ID_CONTEXT_RECENT_INPUT: {

			break;
		}
		case ID_CUT: {

			break;
		}
		case ID_COPY: {

			break;
		}
		case ID_COPY_WITH_BASE_POINT: {

			break;
		}
		case ID_PASTE: {

			break;
		}
		case ID_PASTE_AS_BLOCK: {

			break;
		}
		case ID_PASTETO_ORIGINAL_COORDINATES: {

			break;
		}
		case ID_CONTEXT_UNDO: {

			break;
		}
		case ID_CONTEXT_REDO: {

			break;
		}
		case ID_CONTEXT_PAN: {
			SendCommandParam(m_hWnd, IDC_DOCUMENT, IDC_PAN);
			break;
		}
		case ID_CONTEXT_ZOOM: {
			SendCommandParam(m_hWnd, IDC_DOCUMENT, IDC_ZOOM);
			break;
		}
		case ID_CONTEXT_STEERINGWHEELS: {

			break;
		}
		case ID_CONTEXT_SUBOBJECTSELECTIONFILTER: {

			break;
		}
		case ID_CONTEXT_QUICK_SELECT: {

			break;
		}
		case ID_CONTEXT_QUICKCALC: {

			break;
		}
		case ID_CONTEXT_FIND: {

			break;
		}
		case ID_CONTEXT_OPTIONS: {
			SendCommandParam(m_hWnd, IDC_DOCUMENT, IDC_OPTIONS);
			break;
		}
		default: {
			break;
		}
	}

	DestroyMenu(hMenu);

	return 0;
}
int GLRenderWindow::wm_create(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (GLRenderWindow::s_bScrollbars == true) {
		SCROLLINFO si = { sizeof(si) };
		si.fMask = SIF_POS | SIF_RANGE;
		si.nPos = 50;
		//si.nPage = 100; // number of lines in a page (i.e. rows of text in window)
		si.nMin = 0;
		si.nMax = 100;
		SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE);
		SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
	}

	dwThreadId = GetCurrentThreadId();

	if (fileOpen(m_szFileName) == false) {
		log("Log::Drawing::Drawing():[WARNING] Could not open file!");
		NMHDR nmh;
		nmh.hwndFrom = m_hWnd;
		nmh.idFrom = IDC_DOCUMENT;
		HWND hApp = m_system->getApplication()->getHWND();

		char szText[MAX_LOADSTRING]{ 0 };
		::strcpy_s(szText, "");
		nmh.code = SB_SETTEXT;
		::SendMessage(hApp, WM_NOTIFY, (WPARAM)&szText, (LPARAM)&nmh);

		nmh.code = TCN_SELCHANGE;
		::SendMessage(hApp, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
	}
	else {
		log("Log::Drawing::Drawing():m_datathread reading file");
		m_lpDataThread = new std::thread(&GLRenderWindow::readData, this);
	}

	m_lpGLRenderContext = new GLRenderContext(m_hWnd, 32, 24, 8, 2);//Create GLRenderContext class (Manages OpenGL render context)...
	if (m_lpRenderThread == nullptr) {//If the thread doesn't exist...
		if (m_lpGLRenderContext->createContextWithAttributes()) {//Create OpenGL render context with attributes...
			m_lpRenderThread = new std::thread(&GLRenderThread::renderThreadProc, m_lpGLRenderThread, m_lpGLRenderContext, SIZE({ m_cx,m_cy }));//Create the thread.
			m_lpFrameThread = new std::thread(&GLRenderThread::swapbuffersThreadProc, m_lpGLRenderThread);
			m_lpFPSThread = new std::thread(&GLRenderThread::calculateFPSThreadProc, m_lpGLRenderThread);
		}
	}

	HRESULT hResult = ErrorHandler();
	if (FAILED(hResult)) {
		return -1;
	}

	return 0;
}
int GLRenderWindow::wm_destroy()
{
	return 0;
}
int GLRenderWindow::wm_entermenuloop(WPARAM wParam)
{
	BOOL nTrackkPopupMenu = (BOOL)wParam;

	LRESULT lResult = m_CommandManager.sendMessage(WM_ENTERMENULOOP, wParam, 0);
	if (lResult == TRUE) {
		//return 0;
	}

	if (nTrackkPopupMenu == TRUE) {
		m_bShortcutMenuLoopActive = true;
	}
	else {
		m_bMenuLoopActive = true;
	}

	return 0;
}
int GLRenderWindow::wm_exitmenuloop(WPARAM wParam)
{
	BOOL nTrackkPopupMenu = (BOOL)wParam;

	LRESULT lResult = m_CommandManager.sendMessage(WM_EXITMENULOOP, wParam, 0);
	if (lResult == TRUE) {
		//return 0;
	}

	if (nTrackkPopupMenu == TRUE) {
		m_bShortcutMenuLoopActive = false;
	}
	else {
		m_bMenuLoopActive = false;
	}

	return 0;
}
int GLRenderWindow::wm_exitsizemove()
{
	if (s_bContinuousSize == false) {
		wm_size();
	}
	return 0;
}
int GLRenderWindow::wm_hscroll(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	m_siHorz.cbSize = sizeof(SCROLLINFO);
	m_siHorz.fMask = SIF_ALL;

	GetScrollInfo(m_hWnd, SB_HORZ, &m_siHorz);
	m_nHScroll = m_siHorz.nPos;

	switch (LOWORD(wParam)) {
		case SB_LINELEFT: {
			m_siHorz.nPos -= 1;
			break;
		}
		case SB_LINERIGHT: {
			m_siHorz.nPos += 1;
			break;
		}
		case SB_PAGELEFT: {
			m_siHorz.nPos -= m_siHorz.nPage;
			break;
		}
		case SB_PAGERIGHT: {
			m_siHorz.nPos += m_siHorz.nPage;
			break;
		}
		case SB_THUMBTRACK: {
			m_siHorz.nPos = m_siHorz.nTrackPos;
			break;
		}
		default: {
			break;
		}
	}

	m_siHorz.fMask = SIF_POS;
	SetScrollInfo(m_hWnd, SB_HORZ, &m_siHorz, TRUE);

	GetScrollInfo(m_hWnd, SB_HORZ, &m_siHorz);
	if (m_siHorz.nPos != m_nHScroll) {
		int dx = m_nHScroll - m_siHorz.nPos;
		//log("Log::GLRenderWindow::hScroll() dx = %d", dx);
		m_lpGLRenderThread->SendThreadMessage(WM_HSCROLL, wParam, (LPARAM)dx);
	}

	return 0;
}
int GLRenderWindow::wm_keydown(WPARAM wParam, LPARAM lParam)//Contains functions for moving the virtual carat in the command text buffer.
{
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

	//::SetWindowPos(m_lpCommandWindows[1]->getHWND(), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	//Command* lpCommand = getActiveCommand();
	//if (lpCommand != nullptr) {
	//	int nResult = lpCommand->wm_keydown(wParam, lParam);
	//	if (nResult == 1) {
	//		return 0;
	//	}
	//}

	log("Log::GLRenderWindow::wm_keydown() vkCode = %d", vkCode);

	Command* lpViewCommand = m_CommandManager.getActiveObject(CommandContext::VIEW);
	if (lpViewCommand == nullptr) {
		switch (vkCode) {
			case VK_SHIFT:												// Converts VK_SHIFT to VK_LSHIFT or VK_RSHIFT
			case VK_CONTROL:											// Converts VK_CONTROL to VK_LCONTROL or VK_RCONTROL
			case VK_MENU: {												// Converts VK_MENU to VK_LMENU or VK_RMENU
				vkCode = LOWORD(::MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX));
				break;
			}
			case VK_F1: {
				log("Log::GLRenderWindow::wm_command() VK_F1");
				::SendMessage(GetParent(m_hWnd), WM_COMMAND, MAKEWPARAM(ID_HELP_HELP, 0), 0);
				break;
			}
			default: {
				break;
			}
		}
	}

	m_lpGLRenderThread->SendThreadMessage(WM_KEYDOWN, wParam, lParam);

	return 0;
}
int GLRenderWindow::wm_killfocus(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);
	return 0;
}
int GLRenderWindow::wm_lbuttondblclk(WPARAM wParam, LPARAM lParam)
{
	if (getActiveLayout() == nullptr) return 0;

	m_CommandManager.setObjectInactive(CommandContext::COMMAND);//Cancel "Select" command from wm_lbuttondown/wm_lbuttonup events.
	if (m_CommandManager.getActiveObject(CommandContext::VIEW) != nullptr) return 0;//Make wm_lbuttondblclk event inactive if a "View" command is active.

	//Send lButtonDblClk to m_lpDrawing - this will;
	//1. Set the active space.
	//2. Set the active viewport.
	//Space::Context enActiveSpace = getActiveLayout()->getActiveSpace()->getContext();
	m_lpGLRenderThread->SendThreadMessage(WM_LBUTTONDBLCLK, wParam, lParam);
	//if (getActiveLayout()->getActiveSpace()->getContext() == enActiveSpace) return 0;//If the active space context hasn't changed, we don't need to update the UI.

	//HWND hStatusbar = ::GetDlgItem(m_hWndParent, IDC_STATUSBAR);
	//setStatusbarText(hStatusbar, 2);

	//setStatusbarText(hStatusbar, 2);
	//setDynamicInputStyle();

	return 0;
}
int GLRenderWindow::wm_lbuttondown(WPARAM wParam, LPARAM lParam)
{
	m_ptMouseDown = MAKEPOINT(lParam);
	m_ptMouseMove = m_ptMouseDown;
	m_bMouseMoved = false;

	::SetFocus(m_hWnd);

	if (getActiveLayout() == nullptr) return 0;

	m_lpGLRenderThread->SendThreadMessage(WM_LBUTTONDOWN, wParam, lParam);

	if (m_CommandManager.getActiveObject(CommandContext::VIEW) != nullptr) {
		SetCapture(m_hWnd);
		return 0;
	}

	POINT glPoint = getActiveLayout()->getActiveViewport()->getCursor(WinToGL(m_ptMouseUp));
	LRESULT lResult = m_CommandManager.sendMessage(WM_LBUTTONDOWN, wParam, MAKELPARAM(glPoint.x, glPoint.y));
	if (lResult == TRUE) {
		//return 0;
	}

	if (::PtInRect(getActiveLayout()->getActiveViewport(), WinToGL(m_ptMouseDown)) == TRUE) {
		if (m_CommandManager.getActiveObject(CommandContext::VIEW) == nullptr) {
			Command* lpCommand = m_CommandManager.getActiveObject(CommandContext::COMMAND);
			if (lpCommand == nullptr) {
				SendCommandParam(m_hWnd, IDC_DOCUMENT, IDC_SELECT);
			}
			else {
				if (lpCommand->getID() == IDC_SELECT) {
					::SendMessage(m_hWnd, WM_CHAR, (WPARAM)VK_RETURN, 0);
				}
			}
		}
	}
	else {
		if (m_CommandManager.getActiveObject(CommandContext::VIEW) == nullptr) {
			if (m_CommandManager.getActiveObject(CommandContext::COMMAND) == nullptr) {
				getActiveLayout()->setActiveViewport(WinToGL(m_ptMouseDown));
			}
		}
	}

	//m_lpGLRenderThread->SendThreadMessage(WM_LBUTTONDOWN, wParam, lParam);

	return 0;
}
int GLRenderWindow::wm_lbuttonup(WPARAM wParam, LPARAM lParam)
{
	m_ptMouseUp = MAKEPOINT(lParam);
	m_ptMouseMove = m_ptMouseUp;

	if (m_CommandManager.getActiveObject(CommandContext::VIEW) != nullptr) {
		ReleaseCapture();
	}

	if (getActiveLayout() == nullptr) return 0;

	POINT glPoint = getActiveLayout()->getActiveViewport()->getCursor(WinToGL(m_ptMouseUp));
	LRESULT lResult = m_CommandManager.sendMessage(WM_LBUTTONUP, wParam, MAKELPARAM(glPoint.x, glPoint.y));
	if (lResult == TRUE) {
		//return 0;
	}

	if (m_CommandManager.getActiveObject(CommandContext::VIEW) == nullptr) {
		Command* lpCommand = m_CommandManager.getActiveObject(CommandContext::COMMAND);
		if (lpCommand != nullptr) {
			if (lpCommand->getID() == IDC_SELECT) {
				CSelect* lpSelect = (CSelect*)m_CommandManager.getActiveObject(CommandContext::COMMAND);
				if (lpSelect->getMouseMoved() == true) {
					::SendMessage(m_hWnd, WM_CHAR, (WPARAM)VK_RETURN, 0);
				}
			}
		}
	}
	
	m_lpGLRenderThread->SendThreadMessage(WM_LBUTTONUP, wParam, lParam);

	return 0;
}
int GLRenderWindow::wm_mouseleave()
{
	m_mouseTrack.Reset();
	m_lpGLRenderThread->SendThreadMessage(WM_MOUSELEAVE);
	return 0;
}
int GLRenderWindow::wm_mousemove(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	m_ptMouseMove = MAKEPOINT(lParam);

	m_mouseTrack.OnMouseMove(m_hWnd);

	if (m_lpCommandWindows[2] != nullptr) {
		if (::IsWindowVisible(m_lpCommandWindows[2]->getHWND()) == TRUE) return 0;
	}

	if (getActiveLayout() == nullptr) return 0;

	POINT glPoint = getActiveLayout()->getActiveViewport()->getCursor(WinToGL(m_ptMouseMove));
	LRESULT lResult = m_CommandManager.sendMessage(WM_MOUSEMOVE, wParam, MAKELPARAM(glPoint.x, glPoint.y));
	if (lResult == TRUE) {
		//return 0;
	}

	m_lpGLRenderThread->SendThreadMessage(WM_MOUSEMOVE, wParam, lParam);

	HWND hStatusbar = ::GetDlgItem(m_system->getApplication()->getHWND(), IDC_STATUSBAR);
	setStatusbarText(hStatusbar, 1);

	return 0;
}
int GLRenderWindow::wm_mousewheel(WPARAM wParam, LPARAM lParam)
{
	m_lpGLRenderThread->SendThreadMessage(WM_MOUSEWHEEL, wParam, lParam);
	return 0;
}
int GLRenderWindow::wm_move(LPARAM lParam)
{
	log("Log::GLRenderWindow::wm_move()");

	m_x = GET_X_LPARAM(lParam);
	m_y = GET_Y_LPARAM(lParam);

	return 0;
}
int GLRenderWindow::wm_notify(WPARAM wParam, LPARAM lParam)
{
	m_lpGLRenderThread->SendThreadMessage(WM_NOTIFY, wParam, lParam);
	return 0;
}
int GLRenderWindow::wm_paint()
{
	log("Log::GLRenderWindow::wm_paint()");
	m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
	return 0;
}
int GLRenderWindow::wm_setcursor(WPARAM wParam, LPARAM lParam)//Child window; set cursor and return FALSE for default processing.
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	Command* lpCommand = getActiveCommand();
	if (lpCommand != nullptr) {
		LRESULT lResult = ::SendMessage(lpCommand->getHWND(), WM_SETCURSOR, wParam, lParam);
		if (lResult == TRUE) {
			return 0;
		}
	}

	if (m_bShortcutMenuLoopActive == true) {
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		return 0;
	}

	if (getActiveLayout() == nullptr) {
		return 0;
	}

	if (DynamicInput::s_nVisible == TRUE) {
		if (m_lpCommandWindows[2] != nullptr) {
			if (m_lpCommandWindows[2]->getValid() == TRUE) {
				if (::IsWindowVisible(m_lpCommandWindows[2]->getHWND()) == TRUE) {
					::SetCursor(::LoadCursor(NULL, IDC_ARROW));
					return 0;
				}
			}
		}
	}

	BOOL nPtInRect = ::PtInRect(getActiveLayout()->getActiveViewport(), WinToGL(m_ptMouseMove));
	if (nPtInRect == FALSE) {
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}
	else {
		::SetCursor(NULL);
	}

	return 0;
}
int GLRenderWindow::wm_setfocus(WPARAM wParam)
{
	UNREFERENCED_PARAMETER(wParam);
	return 0;
}
int GLRenderWindow::wm_size(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	log("Log::GLRenderWindow::wm_size()");

	m_cx = GET_X_LPARAM(lParam);
	m_cy = GET_Y_LPARAM(lParam);

	wm_size();

	return 0;
}
int GLRenderWindow::wm_timer(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	log("Log::GLRenderWindow::wm_timer()");

	return 0;
}
int GLRenderWindow::wm_vscroll(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	m_siVert.cbSize = sizeof(SCROLLINFO);
	m_siVert.fMask = SIF_ALL;

	GetScrollInfo(m_hWnd, SB_VERT, &m_siVert);
	m_nVScroll = m_siVert.nPos;

	switch (LOWORD(wParam)) {
		case SB_LINELEFT: {
			m_siVert.nPos -= 1;
			break;
		}
		case SB_LINERIGHT: {
			m_siVert.nPos += 1;
			break;
		}
		case SB_PAGELEFT: {
			m_siVert.nPos -= m_siVert.nPage;
			break;
		}
		case SB_PAGERIGHT: {
			m_siVert.nPos += m_siVert.nPage;
			break;
		}
		case SB_THUMBTRACK: {
			m_siVert.nPos = m_siVert.nTrackPos;
			break;
		}
		default: {
			break;
		}
	}

	m_siVert.fMask = SIF_POS;
	SetScrollInfo(m_hWnd, SB_VERT, &m_siVert, TRUE);

	GetScrollInfo(m_hWnd, SB_VERT, &m_siVert);
	if (m_siVert.nPos != m_nVScroll) {
		int dy = m_nVScroll - m_siVert.nPos;
		//log("Log::GLRenderWindow::vScroll() dy = %d", -dy);
		m_lpGLRenderThread->SendThreadMessage(WM_VSCROLL, wParam, (LPARAM)-dy);//Reverse sign of dy to account for reversed y-axis in OpenGL.
	}

	return 0;
}
int GLRenderWindow::wm_windowposchanged(LPARAM lParam)
{
	//UNREFERENCED_PARAMETER(lParam);

	log("Log::GLRenderWindow::wm_windowposchanged()");

	LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;

	m_x = lpwp->x;
	m_y = lpwp->y;

	m_cx = lpwp->cx;
	m_cy = lpwp->cy;

	wm_size();

	return 0;
}
int GLRenderWindow::wm_windowposchanging(LPARAM lParam)
{
	//UNREFERENCED_PARAMETER(lParam);

	log("Log::GLRenderWindow::wm_windowposchanging()");

	LPWINDOWPOS lpwp = (LPWINDOWPOS)lParam;

	m_x = lpwp->x;
	m_y = lpwp->y;

	m_cx = lpwp->cx;
	m_cy = lpwp->cy;

	wm_size();

	return 0;
}

//Private overrides of public window functions.
void GLRenderWindow::wm_size()
{
	DWORD dwStyle = GetWindowLongPtr(m_hWnd, GWL_STYLE);
	BOOL nHScroll = ((dwStyle & WS_HSCROLL) != FALSE);
	if (nHScroll == TRUE) {
		m_siHorz.cbSize = sizeof(SCROLLINFO);
		m_siHorz.fMask = SIF_RANGE | SIF_DISABLENOSCROLL;
		m_siHorz.nMin = -m_cx;
		m_siHorz.nMax = m_cx;

		SetScrollInfo(m_hWnd, SB_HORZ, &m_siHorz, TRUE);
	}
	BOOL nVScroll = ((dwStyle & WS_VSCROLL) != FALSE);
	if (nVScroll == TRUE) {
		m_siVert.cbSize = sizeof(SCROLLINFO);
		m_siVert.fMask = SIF_RANGE | SIF_DISABLENOSCROLL;
		m_siVert.nMin = -m_cy;
		m_siVert.nMax = m_cy;

		SetScrollInfo(m_hWnd, SB_VERT, &m_siVert, TRUE);
	}

	if (s_bContinuousSize) {
		m_lpGLRenderThread->SendThreadMessage(WM_SIZE, (WPARAM)SIZE_RESTORED, MAKELPARAM(m_cx, m_cy));
	}
}


//Set interface colours by interface element
void GLRenderWindow::setModelSpace2D(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz)
{
	UNREFERENCED_PARAMETER(colour);
	UNREFERENCED_PARAMETER(tintforxyz);

	switch (interfaceelement) {
		case InterfaceElement::UniformBackground: {
			//ModelSpace::setBackgroundColour(colour);
			break;
		}
		case InterfaceElement::Crosshairs: {
			//ModelSpace::setPointerColour(colour);
			break;
		}
		default: {
			break;
		}
	}
}
void GLRenderWindow::setLayout(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz)
{
	switch (interfaceelement) {
		case InterfaceElement::UniformBackground: {
			PaperLayout::setPaperColour(colour);
			Space::setBackgroundColour(colour);
			break;
		}
		case InterfaceElement::PaperBackground: {
			PaperLayout::setBackgroundColour(colour);
			break;
		}
		case InterfaceElement::Crosshairs: {
			PaperSpace::setCrosshairColour(colour, tintforxyz);
			break;
		}
		default: {
			break;
		}
	}
}
void GLRenderWindow::setParallelProjection3D(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz)
{
	switch (interfaceelement) {
		case InterfaceElement::UniformBackground: {
			ModelSpace::setBackgroundColour(colour);
			break;
		}
		case InterfaceElement::PaperBackground: {

			break;
		}
		case InterfaceElement::Crosshairs: {
			ModelSpace::setCrosshairColour(colour, tintforxyz, 0);
			break;
		}
		default: {
			break;
		}
	}
}
void GLRenderWindow::setPerspectiveProjection3D(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz)
{
	UNREFERENCED_PARAMETER(colour);
	UNREFERENCED_PARAMETER(tintforxyz);

	switch (interfaceelement) {
		case InterfaceElement::UniformBackground: {

			break;
		}
		case InterfaceElement::PaperBackground: {

			break;
		}
		case InterfaceElement::Crosshairs: {
			ModelSpace::setCrosshairColour(colour, tintforxyz, 1);
			break;
		}
		default: {
			break;
		}
	}
}
void GLRenderWindow::setBlockEditor(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz) { UNREFERENCED_PARAMETER(interfaceelement); UNREFERENCED_PARAMETER(colour); UNREFERENCED_PARAMETER(tintforxyz); }
void GLRenderWindow::setCommandLine(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz) { UNREFERENCED_PARAMETER(interfaceelement); UNREFERENCED_PARAMETER(colour); UNREFERENCED_PARAMETER(tintforxyz); }
void GLRenderWindow::setPlotPreview(InterfaceElement interfaceelement, Colour3<float> colour, bool tintforxyz) { UNREFERENCED_PARAMETER(interfaceelement); UNREFERENCED_PARAMETER(colour); UNREFERENCED_PARAMETER(tintforxyz); }

BOOL GLRenderWindow::showDynamicinput(WPARAM wParam, LPARAM lParam)
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
BOOL GLRenderWindow::showCommandline(WPARAM wParam, LPARAM lParam)
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

//File I/O
void GLRenderWindow::readEntity(DRW_Entity* ent)
{
	switch (ent->eType) {
		case DRW::ETYPE::ARC: {
			DRW_Arc* drw_entity = static_cast<DRW_Arc*>(ent);
			Arc* entity = new Arc(drw_entity);
			m_lpGLRenderThread->Entities()->ent.push_back(entity);
			break;
		}
		case DRW::ETYPE::CIRCLE: {
			DRW_Circle* drw_entity = static_cast<DRW_Circle*>(ent);
			Circle* entity = new Circle(drw_entity);
			m_lpGLRenderThread->Entities()->ent.push_back(entity);
			break;
		}
		case DRW::ETYPE::DIMALIGNED:
		case DRW::ETYPE::DIMANGULAR:
		case DRW::ETYPE::DIMANGULAR3P:
		case DRW::ETYPE::DIMDIAMETRIC:
		case DRW::ETYPE::DIMLINEAR:
		case DRW::ETYPE::DIMORDINATE:
		case DRW::ETYPE::DIMRADIAL: {

			break;
		}
		case DRW::ETYPE::ELLIPSE: {
			DRW_Ellipse* drw_entity = static_cast<DRW_Ellipse*>(ent);
			EllipsE* entity = new EllipsE(drw_entity);
			m_lpGLRenderThread->Entities()->ent.push_back(entity);
			break;
		}
		case DRW::ETYPE::HATCH: {

			break;
		}
		case DRW::ETYPE::IMAGE: {

			break;
		}
		case DRW::ETYPE::INSERT: {

			break;
		}
		case DRW::ETYPE::LEADER: {

			break;
		}
		case DRW::ETYPE::LINE: {
			DRW_Line* drw_entity = static_cast<DRW_Line*>(ent);
			Line* entity = new Line(drw_entity);
			m_lpGLRenderThread->Entities()->ent.push_back(entity);
			break;
		}
		case DRW::ETYPE::LWPOLYLINE: {
			DRW_LWPolyline* drw_entity = static_cast<DRW_LWPolyline*>(ent);
			LWPolyline* entity = new LWPolyline(drw_entity);
			m_lpGLRenderThread->Entities()->ent.push_back(entity);
			break;
		}
		case DRW::ETYPE::MTEXT: {

			break;
		}
		case DRW::ETYPE::POINT: {
			DRW_Point* drw_entity = static_cast<DRW_Point*>(ent);
			Point* entity = new Point(drw_entity);
			m_lpGLRenderThread->Entities()->ent.push_back(entity);
			break;
		}
		case DRW::ETYPE::POLYLINE: {
			DRW_Polyline* drw_entity = static_cast<DRW_Polyline*>(ent);
			Polyline* entity = new Polyline(drw_entity);
			m_lpGLRenderThread->Entities()->ent.push_back(entity);
			break;
		}
		case DRW::ETYPE::RAY: {
			DRW_Ray* drw_entity = static_cast<DRW_Ray*>(ent);
			Ray* entity = new Ray(drw_entity);
			m_lpGLRenderThread->Entities()->ent.push_back(entity);
			break;
		}
		case DRW::ETYPE::SOLID: {

			break;
		}
		case DRW::ETYPE::SPLINE: {

			break;
		}
		case DRW::ETYPE::TEXT: {

			break;
		}
		case DRW::ETYPE::XLINE: {
			DRW_Xline* drw_entity = static_cast<DRW_Xline*>(ent);
			Xline* entity = new Xline(drw_entity);
			m_lpGLRenderThread->Entities()->ent.push_back(entity);
			break;
		}
		default: {
			break;
		}
	}
}
void GLRenderWindow::readBlock(DRW_DataBlock* block)
{
	UNREFERENCED_PARAMETER(block);
}
void GLRenderWindow::readDimstyle(DRW_Dimstyle* dimstyle)
{
	UNREFERENCED_PARAMETER(dimstyle);
}
void GLRenderWindow::readLayer(DRW_Layer* lay)
{
	//DRW_Layer *drw_l = static_cast<DRW_Layer*>(lay);
	const char* n = lay->name.c_str();
	Layer* mcd_l = new Layer(n);
	m_lpGLRenderThread->getLayerManager()->addObject(mcd_l);
}
void GLRenderWindow::readLinetype(DRW_LType* linetype)
{
	UNREFERENCED_PARAMETER(linetype);
}
void GLRenderWindow::readTextstyle(DRW_Textstyle* textstyle)
{
	UNREFERENCED_PARAMETER(textstyle);
}
void GLRenderWindow::readVport(DRW_Vport* vpt)
{
	UNREFERENCED_PARAMETER(vpt);
}
void GLRenderWindow::readAppId(DRW_AppId appid)
{
	UNREFERENCED_PARAMETER(appid);
}
void GLRenderWindow::readImage(DRW_DataImg* dataimg)
{
	UNREFERENCED_PARAMETER(dataimg);
}

void GLRenderWindow::readHeader()
{
	//m_DRW_Data.headerC
}
void GLRenderWindow::readEntities()
{
	DRW_DataBlock* lpmBlock = m_DRW_Data.mBlock;
	for (std::list<DRW_Entity*>::const_iterator it = lpmBlock->ent.begin(); it != lpmBlock->ent.end(); ++it) {
		readEntity(*it);
	}
}
void GLRenderWindow::readBlocks()
{
	for (std::list<DRW_DataBlock*>::const_iterator it = m_DRW_Data.blocks.begin(); it != m_DRW_Data.blocks.end(); ++it) {
		readBlock(*it);
	}
}
void GLRenderWindow::readDimstyles()
{
	for (std::list<DRW_Dimstyle>::const_iterator it = m_DRW_Data.dimStyles.begin(); it != m_DRW_Data.dimStyles.end(); ++it) {
		//readDimstyle(*it);
	}
}
void GLRenderWindow::readLayers()
{
	for (std::list<DRW_Layer>::iterator it = m_DRW_Data.layers.begin(); it != m_DRW_Data.layers.end(); ++it) {
		readLayer(&(*it));
	}
}
void GLRenderWindow::readLinetypes()
{
	for (std::list<DRW_LType>::iterator it = m_DRW_Data.lineTypes.begin(); it != m_DRW_Data.lineTypes.end(); ++it) {
		//readLinetype(*it);
	}
}
void GLRenderWindow::readTextstyles()
{
	for (std::list<DRW_Textstyle>::iterator it = m_DRW_Data.textStyles.begin(); it != m_DRW_Data.textStyles.end(); ++it) {
		//readTextstyle(*it);
	}
}
void GLRenderWindow::readVports()
{
	for (std::list<DRW_Vport>::iterator it = m_DRW_Data.VPorts.begin(); it != m_DRW_Data.VPorts.end(); ++it) {
		readVport(&(*it));
	}
}
void GLRenderWindow::readAppIds()
{
	for (std::list<DRW_AppId>::iterator it = m_DRW_Data.appIds.begin(); it != m_DRW_Data.appIds.end(); ++it) {
		//readAppId(&(*it));
	}
}
void GLRenderWindow::readImages()
{
	for (std::list<DRW_DataImg*>::iterator it = m_DRW_Data.images.begin(); it != m_DRW_Data.images.end(); ++it) {
		readImage(*it);
	}
}

void GLRenderWindow::readData()
{
	char szText[MAX_LOADSTRING]{ 0 };
	::strcpy_s(szText, "Reading Header");
	//char szText2[] = "Reading Header";

	NMHDR nmh;
	nmh.hwndFrom = m_hWnd;
	nmh.idFrom = IDC_DOCUMENT;
	HWND hApp = m_system->getApplication()->getHWND();

	nmh.code = SB_SETTEXT;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)&szText, (LPARAM)&nmh);
	nmh.code = PBM_SETPOS;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)10, (LPARAM)&nmh);

	readHeader();

	::strcpy_s(szText, "Reading Entities");

	nmh.code = SB_SETTEXT;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)&szText, (LPARAM)&nmh);
	nmh.code = PBM_SETPOS;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)20, (LPARAM)&nmh);

	readEntities();

	::strcpy_s(szText, "Reading Blocks");

	nmh.code = SB_SETTEXT;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)&szText, (LPARAM)&nmh);
	nmh.code = PBM_SETPOS;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)30, (LPARAM)&nmh);

	readBlocks();

	::strcpy_s(szText, "Reading Dimstyles");

	nmh.code = SB_SETTEXT;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)&szText, (LPARAM)&nmh);
	nmh.code = PBM_SETPOS;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)40, (LPARAM)&nmh);

	readDimstyles();

	::strcpy_s(szText, "Reading Layers");

	nmh.code = SB_SETTEXT;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)&szText, (LPARAM)&nmh);
	nmh.code = PBM_SETPOS;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)50, (LPARAM)&nmh);

	readLayers();

	::strcpy_s(szText, "Reading Linetypes");

	nmh.code = SB_SETTEXT;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)&szText, (LPARAM)&nmh);
	nmh.code = PBM_SETPOS;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)60, (LPARAM)&nmh);

	readLinetypes();

	::strcpy_s(szText, "Reading Textstyles");

	nmh.code = SB_SETTEXT;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)&szText, (LPARAM)&nmh);
	nmh.code = PBM_SETPOS;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)70, (LPARAM)&nmh);

	readTextstyles();

	::strcpy_s(szText, "Reading Vports");

	nmh.code = SB_SETTEXT;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)&szText, (LPARAM)&nmh);
	nmh.code = PBM_SETPOS;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)80, (LPARAM)&nmh);

	readVports();

	::strcpy_s(szText, "Reading AppIds");

	nmh.code = SB_SETTEXT;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)&szText, (LPARAM)&nmh);
	nmh.code = PBM_SETPOS;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)90, (LPARAM)&nmh);

	readAppIds();

	::strcpy_s(szText, "Reading Images");

	nmh.code = SB_SETTEXT;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)&szText, (LPARAM)&nmh);
	nmh.code = PBM_SETPOS;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)100, (LPARAM)&nmh);

	readImages();

	::strcpy_s(szText, "");

	nmh.code = SB_SETTEXT;
	::SendMessage(hApp, WM_NOTIFY, (WPARAM)&szText, (LPARAM)&nmh);

	nmh.code = TCN_SELCHANGE;
	BOOL nResult = (BOOL)::SendMessage(hApp, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
	//BOOL nResult = (BOOL)::PostMessage(hApp, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
	//BOOL nResult = (BOOL)::PostThreadMessage(dwThreadId, WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);
	if (nResult == FALSE) {
		ErrorHandler();
	}
}
bool GLRenderWindow::fileOpen(const char* fileName)
{
	bool badState{ false };

	std::ifstream ifs;
	ifs.open(fileName, std::ifstream::in);
	badState = ifs.fail();
	ifs.close();
	if (badState) {
		//std::cout << L"Error can't open " << fileName << std::endl;
		return false;
	}

	Interface* input = new Interface();
	badState = input->fileImport(fileName, &m_DRW_Data);
	if (badState == false) {
		//std::cout << L"Error reading file " << fileName << std::endl;
		return false;
	}

	return true;
}

POINT GLRenderWindow::WinToGL(POINT position) { return POINT({ position.x,m_cy - position.y }); }//Transforms Windows coordinates (Right is +x, down is +y) to OpenGL coordinates (Right is +x, up is +y).

void GLRenderWindow::clearCommandTextBuffer()
{
	m_pchInputBuf = 0;
	GlobalFree((HGLOBAL)m_pchInputBuf);
	m_pchInputBuf = (LPTSTR)GlobalAlloc(GPTR, MAX_LOADSTRING * sizeof(char));
	m_nCurChar = 0;
	m_cch = 0;

	log("Log::GLRenderWindow::clearCommandTextBuffer() m_pchInputBuf = %s", m_pchInputBuf);
}

int GLRenderWindow::createCommandWindow(Command* lpCommand, CommandContext nContext)//Creates a message-only window.
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
		log("Log::GLRenderWindow::createCommandWindow() IDOK");
		m_CommandManager.addObject(lpCommand);
		m_CommandManager.setActiveObject(lpCommand, nContext);
	}

	return nResult;
}
int GLRenderWindow::createCommandDialog(Command* lpCommand, UINT nDialogId)//Creates a modal dialog box from a dialog box template resource.
{
	m_CommandManager.addObject(lpCommand);
	m_CommandManager.setActiveObject(lpCommand, CommandContext::COMMAND);

	int nResult = m_system->createDialog(lpCommand, nDialogId, m_system->getApplication()->getHWND());

	if (nResult >= 0) {
		if (nResult == IDOK) {
			log("Log::GLRenderWindow::createCommandDialog() IDOK");
		}
		else if (nResult == IDCANCEL) {
			log("Log::GLRenderWindow::createCommandDialog() IDCANCEL");
		}
	}

	m_CommandManager.setObjectInactive(CommandContext::COMMAND);
	clearCommandTextBuffer();

	return nResult;
}

void GLRenderWindow::exitCommand()
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

BOOL GLRenderWindow::wm_char_view(WORD code)
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
void GLRenderWindow::wm_char_back()
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
void GLRenderWindow::wm_char_return()
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
void GLRenderWindow::wm_char_escape()
{
	Command* lpCommand = m_CommandManager.getActiveObject();
	if (lpCommand == nullptr) {
		SendCommandParam(m_hWnd, IDC_DOCUMENT, IDC_CANCEL);
	}
	else{
		exitCommand();
	}
}
void GLRenderWindow::wm_char_end()
{
	HRESULT hResult = ::StringCchLength(m_pchInputBuf, MAX_LOADSTRING, &m_cch);
	if (FAILED(hResult)) {
		ErrorHandler();
	}
	else {
		m_nCurChar = (int)m_cch;
	}
}
void GLRenderWindow::wm_char_left()
{
	m_nCurChar--;
	if (m_nCurChar < 0) {
		m_nCurChar = 0;
	}
}
void GLRenderWindow::wm_char_right()
{
	m_nCurChar++;
	if (m_nCurChar > (int)m_cch + 1) {
		m_nCurChar = (int)m_cch + 1;
	}
}
void GLRenderWindow::wm_char_default(WPARAM wParam, LPARAM lParam)
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

void GLRenderWindow::wm_command_sendcommand(WPARAM wParam, LPARAM lParam)
{
	int nID = LOWORD(wParam);

	switch (nID) {
		case WM_SETTEXT: {
			log("Log::GLRenderWindow::wm_command_sendcommand() WM_SETTEXT");
			wm_command_sendcommand_text(wParam, lParam);
			break;
		}
		default: {
			log("Log::GLRenderWindow::wm_command_sendcommand() default");
			wm_command_sendcommand_param(wParam, lParam);
			break;
		}
	}
}
void GLRenderWindow::wm_command_sendcommand_text(WPARAM wParam, LPARAM lParam)//Commands from the CommandLine are sent here.
{
	int nSource = HIWORD(wParam);
	if (nSource != IDC_DOCUMENT) {//If the commandline input source is NOT the active document, set the documents commandline text buffer to the value received from the source in the lParam function parameter.
		//m_pchInputBuf = (LPSTR)lParam;
		m_pchInputBuf = (char*)lParam;
	}
	//log("Log::GLRenderWindow::wm_command_sendcommand_text() CommandLine text = %s", m_pchInputBuf);

	int nID = CommandManager::getIDByText(m_pchInputBuf);
	//log("Log::GLRenderWindow::wm_command_sendcommand_text() Command ID = %d", nID);

	//When the command input is from the command line, the mouse position variables may not have 
	//the correct values because the user may have clicked on a listbox or textbox;
	//This sets all the mouse position variables to the cursor position;
	::GetCursorPos(&m_ptMouseDown);//Set m_ptMouseDown to cursor position,...
	::ScreenToClient(m_hWnd, &m_ptMouseDown);//...convert to document window coordinates,...
	m_ptMouseMove = m_ptMouseDown;//...and set m_ptMouseMove...
	m_ptMouseUp = m_ptMouseDown;//...and m_ptMouseUp to m_ptMouseDown value.

	switch (nID) {
		case IDC_3DCORBIT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = 3DCORBIT");
			createCommandWindow(new C3DOrbit(&CommandManager::s_mCommandByID.at(IDC_3DCORBIT), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_3DDISTANCE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = 3DDISTANCE");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DDISTANCE), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_3DFLY: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = 3DFLY");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DFLY), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_3DFORBIT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = 3DFORBIT");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DFORBIT), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_3DO:
		case IDC_3DORBIT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = 3DORBIT");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DORBIT), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_3DPOLY: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_3DPOLY");
			break;
		}
		case IDC_3DSWIVEL: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = 3DSWIVEL");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DSWIVEL), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_3DWALK: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = 3DWALK");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DWALK), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_ABOUT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_ABOUT");
			::SendMessage(GetParent(m_hWnd), WM_COMMAND, MAKEWPARAM(ID_HELP_ABOUT, 0), 0);
			break;
		}
		case IDC_ARC: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_ARC");
			break;
		}
		case IDC_CIRCLE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_CIRCLE");
			break;
		}
		case IDC_DSETTINGS: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_DSETTINGS");
			createCommandDialog(new dlg_DraftingSettings(&CommandManager::s_mCommandByID.at(IDC_DSETTINGS), m_lpCommandWindows, m_pchInputBuf), IDD_DRAFTINGSETTINGS);
			break;
		}
		case IDC_ELLIPSE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_ELLIPSE");
			break;
		}
		case IDC_EXIT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_EXIT");
			PostQuitMessage(0);
			break;
		}
		case IDC_HELIX: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_HELIX");
			break;
		}
		case IDC_LINE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_LINE");
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
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_MSPACE");
			if (getActiveLayout()) {
				getActiveLayout()->setActiveSpace(Space::Context::ModelSpace);
				m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
			}
			break;
		}
		case IDC_OPTIONS: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_OPTIONS");
			createCommandDialog(new dlg_Options(&CommandManager::s_mCommandByID.at(IDC_OPTIONS), m_lpCommandWindows, m_pchInputBuf), IDD_OPTIONS);
			break;
		}
		case IDC_PAN: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_PAN");
			createCommandWindow(new CPan(&CommandManager::s_mCommandByID.at(IDC_PAN), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		case IDC_POINT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_POINT");
			break;
		}
		case IDC_PLINE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_PLINE");
			break;
		}
		case IDC_POLYGON: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_POLYGON");
			break;
		}
		case IDC_PSPACE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_PSPACE");
			if (getActiveLayout()) {
				getActiveLayout()->setActiveSpace(Space::Context::PaperSpace);
				m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
			}
			break;
		}
		case IDC_QUIT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_QUIT");
			PostQuitMessage(0);
			break;
		}
		case IDC_RAY: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_RAY");
			break;
		}
		case IDC_RECTANGLE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_RECTANGLE");
			break;
		}
		case IDC_SELECT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_SELECT");
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
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_SETVAR");
			::SendMessage(GetParent(m_hWnd), WM_NOTIFY, wParam, lParam);
			break;
		}
		case IDC_SPLINE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_SPLINE");
			break;
		}
		case IDC_TILEMODE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_TILEMODE");
			break;
		}
		case IDC_UNITS: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_UNITS");
			createCommandDialog(new dlg_DrawingUnits(&CommandManager::s_mCommandByID.at(IDC_UNITS), m_lpCommandWindows, m_pchInputBuf), IDD_DRAWINGUNITS);
			break;
		}
		case IDC_UCSICON: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_UCSICON");
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
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_UCSMANAGER");
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
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_VIEW");

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
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC__VIEW");

			if (getActiveLayout() == nullptr) {
				break;
			}

			View::PresetView enPresetView = static_cast<View::PresetView>(HIWORD(lParam));
			getActiveLayout()->getActiveSpace()->setPresetView(enPresetView);
			m_lpGLRenderThread->SendThreadMessage(WM_PAINT);

			break;
		}
		case IDC_VPORTS: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_VPORTS");
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
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC__VPORTS");


			break;
		}
		case IDC_VSCURRENT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_VSCURRENT");
			//if (getActiveLayout()) {
			//	View::VisualStyle enVisualStyle = static_cast<View::VisualStyle>(lpcmdhdr->val);
			//	getActiveLayout()->getActiveSpace()->setVisualStyle(enVisualStyle);
			//}
			break;
		}
		case IDC_XLINE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_XLINE");
			break;
		}
		case IDC_ZOOM: {
			log("Log::GLRenderWindow::wm_command_sendcommand_text() Command = IDC_ZOOM");
			createCommandWindow(new CZoom(&CommandManager::s_mCommandByID.at(IDC_ZOOM), m_lpCommandWindows, m_pchInputBuf), CommandContext::VIEW);
			break;
		}
		default: {
			break;
		}
	}
}
void GLRenderWindow::wm_command_sendcommand_param(WPARAM wParam, LPARAM lParam)//Commands from the UI are sent here.
{
	int nID = LOWORD(wParam);
	int nSource = HIWORD(wParam);

	//log("Log::GLRenderWindow::wm_command_sendcommand_param() Command ID = %d", nID);

	switch (nID) {
		case IDC_3DCORBIT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_3DCORBIT");
			createCommandWindow(new C3DOrbit(&CommandManager::s_mCommandByID.at(IDC_3DCORBIT), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_3DDISTANCE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_3DDISTANCE");
			createCommandWindow(new CView3D(&CommandManager::s_mCommandByID.at(IDC_3DDISTANCE), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_3DFLY: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_3DFLY");
			createCommandWindow(new CView3D(&CommandManager::s_mCommandByID.at(IDC_3DFLY), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_3DFORBIT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_3DFORBIT");
			createCommandWindow(new CView3D(&CommandManager::s_mCommandByID.at(IDC_3DFORBIT), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_3DO:
		case IDC_3DORBIT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_3DORBIT");
			createCommandWindow(new C3DOrbit(&CommandManager::s_mCommandByID.at(IDC_3DORBIT), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_3DPOLY: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_3DPOLY");
			break;
		}
		case IDC_3DSWIVEL: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_3DSWIVEL");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DSWIVEL), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_3DWALK: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_3DWALK");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_3DWALK), m_lpCommandWindows), CommandContext::VIEW);
			break;
		}
		case IDC_ABOUT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_ABOUT");
			::SendMessage(GetParent(m_hWnd), WM_COMMAND, MAKEWPARAM(ID_HELP_ABOUT, 0), 0);
			break;
		}
		case IDC_ARC: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_ARC");
			break;
		}
		case IDC_CANCEL: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_CANCEL");
			createCommandWindow(new CommandWindow(&CommandManager::s_mCommandByID.at(IDC_CANCEL), m_lpCommandWindows));
			exitCommand();
			break;
		}
		case IDC_CIRCLE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_CIRCLE");
			break;
		}
		case IDC_DSETTINGS: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_DSETTINGS");
			createCommandDialog(new dlg_DraftingSettings(&CommandManager::s_mCommandByID.at(IDC_DSETTINGS), m_lpCommandWindows), IDD_DRAFTINGSETTINGS);
			break;
		}
		case IDC_ELLIPSE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_ELLIPSE");
			break;
		}
		case IDC_EXIT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_EXIT");
			PostQuitMessage(0);
			break;
		}
		case IDC_HELIX: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_HELIX");
			break;
		}
		case IDC_LAYER: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_LAYER");
			//createCommandDialog(new plt_LayerPropertiesManager(&CommandManager::s_mCommandByID.at(IDC_LAYER), m_lpCommandWindows), IDD_OPTIONS);
			break;
		}
		case IDC_LINE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_LINE");
			createCommandWindow(new CLine(&CommandManager::s_mCommandByID.at(IDC_LINE), m_lpCommandWindows, getActiveLayout()->getActiveViewport()->getCursor(WinToGL(m_ptMouseDown))));
			break;
		}
		case IDC_MSPACE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_MSPACE");
			if (getActiveLayout()) {
				getActiveLayout()->setActiveSpace(Space::Context::ModelSpace);
				m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
			}
			break;
		}
		case IDC_OPTIONS: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_OPTIONS");
			createCommandDialog(new dlg_Options(&CommandManager::s_mCommandByID.at(IDC_OPTIONS), m_lpCommandWindows), IDD_OPTIONS);
			break;
		}
		case IDC_PAN: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_PAN");
			if (nSource == ID_CONTEXT_MENU_ORBIT) {
				createCommandWindow(new CPan3D(&CommandManager::s_mCommandByID.at(IDC_PAN), m_lpCommandWindows), CommandContext::VIEW);
			}
			else {
				createCommandWindow(new CPan(&CommandManager::s_mCommandByID.at(IDC_PAN), m_lpCommandWindows), CommandContext::VIEW);
			}
			break;
		}
		case IDC_POINT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_POINT");
			break;
		}
		case IDC_PLINE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_PLINE");
			break;
		}
		case IDC_POLYGON: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_POLYGON");
			break;
		}
		case IDC_PSPACE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_PSPACE");
			if (getActiveLayout()) {
				getActiveLayout()->setActiveSpace(Space::Context::PaperSpace);
				m_lpGLRenderThread->SendThreadMessage(WM_PAINT);
			}
			break;
		}
		case IDC_QUIT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_QUIT");
			PostQuitMessage(0);
			break;
		}
		case IDC_RAY: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_RAY");
			break;
		}
		case IDC_RECTANGLE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_RECTANGLE");
			break;
		}
		case IDC_SELECT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_SELECT");
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
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = SETVAR");
			::SendMessage(GetParent(m_hWnd), WM_COMMAND, wParam, lParam);
			break;
		}
		case IDC_SPLINE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_SPLINE");
			break;
		}
		case IDC_TILEMODE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_TILEMODE");
			break;
		}
		case IDC_UNITS: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_UNITS");
			createCommandDialog(new dlg_DrawingUnits(&CommandManager::s_mCommandByID.at(IDC_UNITS), m_lpCommandWindows), IDD_DRAWINGUNITS);
			break;
		}
		case IDC_UCSICON: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_UCSICON");
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
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_UCSMANAGER");
			createCommandDialog(new dlg_UCS(&CommandManager::s_mCommandByID.at(IDC_UCSMANAGER), m_lpCommandWindows, 0), IDD_UCS);
			break;
		}
		case IDC_VIEW: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_VIEW");

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
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC__VIEW");

			if (getActiveLayout() == nullptr) {
				break;
			}

			//if (nSource == IDD_VIEWMANAGER) {
			//
			//}

			View::PresetView enPresetView = static_cast<View::PresetView>(HIWORD(lParam));
			getActiveLayout()->getActiveSpace()->setPresetView(enPresetView);
			m_lpGLRenderThread->SendThreadMessage(WM_PAINT);

			break;
		}
		case IDC_VPORTS: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_VPORTS");

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
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC__VPORTS");

			if (getActiveLayout() == nullptr) {
				break;
			}

			ViewportManager::LPVIEWPORTDATA lpVprtConfigData = (ViewportManager::LPVIEWPORTDATA)lParam;
			getActiveLayout()->getActiveSpace()->setViewportConfiguration(lpVprtConfigData);
			m_lpGLRenderThread->SendThreadMessage(WM_PAINT);

			break;
		}
		case IDC_VSCURRENT: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_VSCURRENT");
			if (getActiveLayout()) {
				View::VisualStyle enVisualStyle = static_cast<View::VisualStyle>(HIWORD(lParam));
				getActiveLayout()->getActiveSpace()->setVisualStyle(enVisualStyle);
			}
			break;
		}
		case IDC_XLINE: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = IDC_XLINE");
			break;
		}
		case IDC_ZOOM: {
			log("Log::GLRenderWindow::wm_command_sendcommand_param() Command = ZOOM");
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
