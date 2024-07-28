#include "SYS_systemwin32.h"

#include "DOC_document.h"
#include "DOC_resource.h"

#include "MKE_mdi_application.h"
#include "MKE_sdi_application.h"
#include "MKE_resource.h"

#include "EDL_resource.h"
#include "EPL_resource.h"
#include "EWD_resource.h"

#include "resources.h"

#include "fault.h"
#include "log.h"

#include <commctrl.h>
#include <intsafe.h>//Required for IntPtrToInt

using namespace mycad;

RECT m_rcMonitor;

SystemWin32::SystemWin32(HINSTANCE hInstance)
	: m_hInstance(hInstance),
	m_lpApplication(nullptr)
{
	log("Log::SystemWin32::SystemWin32()");

	::SetProcessDPIAware();//Disable scaling on high DPI displays on Vista.
	::EnumDisplayMonitors(0, 0, s_bfnMonitorEnumProc, (LPARAM)&m_rcMonitor);

	INITCOMMONCONTROLSEX icce{ sizeof(icce),ICC_STANDARD_CLASSES | ICC_BAR_CLASSES | ICC_TAB_CLASSES | ICC_TREEVIEW_CLASSES | ICC_LISTVIEW_CLASSES };
	::InitCommonControlsEx(&icce);

	BOOL pvInfo = FALSE;
	::SetUserObjectInformation(::GetCurrentProcess(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &pvInfo, sizeof(BOOL));
}
SystemWin32::~SystemWin32()
{
	log("Log::SystemWin32::~SystemWin32()");
	//delete m_lpApplication;
	m_lpApplication = nullptr;
}

Window* SystemWin32::getApplication() { return m_lpApplication; }

WindowManager& SystemWin32::getWindowManager() { return m_WindowManager; }
DocumentManager& SystemWin32::getDocumentManager() { return m_DocumentManager; }

int SystemWin32::processEvents()
{
	HACCEL hAccelTable = ::LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_ACCEL_MAIN));

#pragma warning( push )
#pragma warning( disable : 6387)
	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0)) {
		if (!::TranslateAccelerator(m_lpApplication->getHWND(), hAccelTable, &msg)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
#pragma warning( pop )

	return (int)msg.wParam;
}

int SystemWin32::registerFrameClass(
	const char* lpClassName,
	HICON hIcon,
	COLORREF hbrBackground,
	HCURSOR hCursor,
	const char* lpMenuName,
	unsigned int nStyle)
{
	BOOL nResult = 0;

	WNDCLASSEX wcex{ 0 };
	wcex.cbClsExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbWndExtra = 2 * sizeof(LONG_PTR);//4 bytes for (Window*) plus 4 bytes for unsigned int saved by dialog windows.
	wcex.hbrBackground = (hbrBackground == NULL) ? NULL : ::CreateSolidBrush(hbrBackground);
	wcex.hCursor = hCursor;
	wcex.hIcon = hIcon;
	wcex.hInstance = m_hInstance;
	wcex.lpfnWndProc = s_lpfnFrameProc;
	wcex.lpszClassName = lpClassName;
	wcex.lpszMenuName = lpMenuName;
	wcex.style = nStyle;

	nResult = ::GetClassInfoEx(wcex.hInstance, wcex.lpszClassName, &wcex);

	if (nResult == TRUE) return 1;

	if (::RegisterClassEx(&wcex) == 0) {
		ErrorHandler();
		nResult = 0;
	}
	else {
		nResult = 1;
	}

	return nResult;
}
int SystemWin32::registerMDIChildClass(
	const char* lpClassName,
	HICON hIcon,
	COLORREF hbrBackground,
	HCURSOR hCursor,
	const char* lpMenuName,
	unsigned int nStyle)
{
	BOOL nResult = 0;

	WNDCLASSEX wcex{ 0 };
	wcex.cbClsExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbWndExtra = 2 * sizeof(LONG_PTR);//4 bytes for (Window*) plus 4 bytes for unsigned int saved by dialog windows.
	wcex.hbrBackground = (hbrBackground == NULL) ? NULL : ::CreateSolidBrush(hbrBackground);
	wcex.hCursor = hCursor;
	wcex.hIcon = hIcon;
	wcex.hInstance = m_hInstance;
	wcex.lpfnWndProc = s_lpfnMDIChildProc;
	wcex.lpszClassName = lpClassName;
	wcex.lpszMenuName = lpMenuName;
	wcex.style = nStyle;

	nResult = ::GetClassInfoEx(wcex.hInstance, wcex.lpszClassName, &wcex);

	if (nResult == TRUE) return 1;

	if (::RegisterClassEx(&wcex) == 0) {
		ErrorHandler();
		nResult = 0;
	}
	else {
		nResult = 1;
	}

	return nResult;
}
int SystemWin32::registerClass(
	const char* lpClassName,
	HICON hIcon,
	COLORREF hbrBackground,
	HCURSOR hCursor,
	const char* lpMenuName,
	unsigned int nStyle)
{
	BOOL nResult = 0;

	WNDCLASSEX wcex{ 0 };
	wcex.cbClsExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbWndExtra = 2 * sizeof(LONG_PTR);//4 bytes for (Window*) plus 4 bytes for unsigned int saved by dialog windows.
	wcex.hbrBackground = (hbrBackground == NULL) ? NULL : ::CreateSolidBrush(hbrBackground);
	wcex.hCursor = hCursor;
	wcex.hIcon = hIcon;
	wcex.hInstance = m_hInstance;
	wcex.lpfnWndProc = s_lpfnWindowProc;
	wcex.lpszClassName = lpClassName;
	wcex.lpszMenuName = lpMenuName;
	wcex.style = nStyle;

	nResult = ::GetClassInfoEx(wcex.hInstance, wcex.lpszClassName, &wcex);

	if (nResult == TRUE) return 1;

	if (::RegisterClassEx(&wcex) == 0) {
		ErrorHandler();
		nResult = 0;
	}
	else {
		nResult = 1;
	}

	return nResult;
}
int SystemWin32::registerClass(const char* lpClassname)//Registers a system window class (#32770) with RegisterClassEx.
{
	BOOL nResult = 0;

	WNDCLASSEX wcex{ 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	if (::GetClassInfoEx(m_hInstance, "#32770", &wcex) == FALSE) {//#32770 is the system class for a dialog box.
		return 0;
	};
	wcex.lpszClassName = lpClassname;

	nResult = ::GetClassInfoEx(wcex.hInstance, wcex.lpszClassName, &wcex);

	if (nResult == TRUE) return 1;

	if (::RegisterClassEx(&wcex) == 0) {
		ErrorHandler();
		nResult = 0;
	}
	else {
		nResult = 1;
	}

	return nResult;
}

int SystemWin32::createWindow(
	const char* lpClassname,
	const char* lpWindowName,
	Window* lpWindow,
	unsigned long dwExStyle,
	unsigned long dwStyle,
	int X,
	int Y,
	int nWidth,
	int nHeight,
	HWND hWndParent,
	HMENU hMenu,
	int nCmdShow)
{
	int nResult = 0;

	HWND hWnd = ::CreateWindowEx(
		dwExStyle,
		lpClassname,
		lpWindowName,
		dwStyle,
		X,
		Y,
		nWidth,
		nHeight,
		hWndParent,
		hMenu,
		m_hInstance,
		(LPVOID)(WindowWin32*)lpWindow
	);

	if (hWnd == NULL) {
		ErrorHandler();
	}
	else {
#pragma warning( push )
#pragma warning( disable : 6387)

		if (nCmdShow >= 0) {
			::ShowWindow(hWnd, nCmdShow);
		}

#pragma warning( pop )
	}

	if (lpWindow->getValid() == TRUE) {
		log("Log::SystemWin32::createWindow(): window->getValid() == TRUE");
		nResult = 1;
	}
	else {
		log("Log::SystemWin32::createWindow(): window->getValid() == FALSE");
		delete lpWindow;
		lpWindow = NULL;
	}

	return nResult;
}
int SystemWin32::createDialog(
	Window* lpWindow,
	UINT nDialogId,
	HWND hWndParent)
{
	HRESULT hResult = S_OK;
	INT nResult = 0;

	INT_PTR npResult = ::DialogBoxParam(m_hInstance, MAKEINTRESOURCE(nDialogId), hWndParent, (DLGPROC)&s_npDialogProc, (LPARAM)lpWindow);

	hResult = ::IntPtrToInt(npResult, &nResult);
	if (hResult != S_OK) {
		ErrorHandler();
	}

	return (int)nResult;
}

int SystemWin32::getMonitors() { return (int)s_miMonitors.size(); }
BOOL SystemWin32::getMonitor(unsigned int index, MONITORINFOEX* mi)
{
	BOOL nResult = FALSE;

	if (index >= s_miMonitors.size()) return nResult;

	try {
		*mi = s_miMonitors.at(index);
		nResult = TRUE;
	}
	catch (std::out_of_range exception) {
		ErrorHandler();
	}

	return nResult;
}

void SystemWin32::registerClasses()
{
	HINSTANCE hModule = ::LoadLibrary("mycaddata.dll");
	if (hModule != NULL) {
		registerApplicationClasses(hModule);
		registerCommandClasses(hModule);
		::FreeLibrary(hModule);
	}
	registerDialogClasses();
	registerMiscellaneousClasses();
}
void SystemWin32::registerDialogClasses()
{
	BOOL nResult = FALSE;
	char szClassName[MAX_LOADSTRING]{ 0 };

	::LoadString(m_hInstance, ID_VIEWPORTS_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_VIEWPORTS_CLASS FAILED!");
	}

	::LoadString(m_hInstance, ID_VIEWMANAGER_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_VIEWMANAGER_CLASS FAILED!");
	}

	::LoadString(m_hInstance, ID_UCSICONPROPERTIES_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_UCSICONPROPERTIES_CLASS FAILED!");
	}

	::LoadString(m_hInstance, ID_DRAWINGUNITS_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_DRAWINGUNITS_CLASS FAILED!");
	}

	::LoadString(m_hInstance, ID_UCS_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_UCS_CLASS FAILED!");
	}

	::LoadString(m_hInstance, ID_DRAFTINGSETTINGS_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_DRAFTINGSETTINGS_CLASS FAILED!");
	}

	::LoadString(m_hInstance, ID_DRAWINGWINDOWCOLOURS_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_DRAWINGWINDOWCOLOURS_CLASS FAILED!");
	}

	::LoadString(m_hInstance, ID_OPTIONS_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_OPTIONS_CLASS FAILED!");
	}
}
void SystemWin32::registerToolPaletteClasses(HINSTANCE hModule)
{
	BOOL nResult = FALSE;
	char szClassName[MAX_LOADSTRING]{ 0 };

	HICON hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_MYCAD));
	HCURSOR hCursor = ::LoadCursor(NULL, IDC_ARROW);

	::LoadString(m_hInstance, ID_LAYER_PROPERTIES_MANAGER_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	//nResult = registerClass(GetSysColor(COLOR_MENU + 1), NULL, hCursor, szClassName, NULL, CS_DBLCLKS);
	nResult = registerClass(szClassName, hIcon, GetSysColor(COLOR_MENU + 1), hCursor, NULL, CS_DBLCLKS);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_LAYER_PROPERTIES_MANAGER_CLASS FAILED!");
	}
}
void SystemWin32::registerCommandClasses(HINSTANCE hModule)
{
	BOOL nResult = FALSE;
	char szClassName[MAX_LOADSTRING]{ 0 };

	HICON hIcon = ::LoadIcon(hModule, MAKEINTRESOURCE(IDI_MYCAD));
	HCURSOR hCursor = ::LoadCursor(NULL, IDC_ARROW);

	::LoadString(m_hInstance, ID_TEXTWINDOW_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName, hIcon, GetSysColor(COLOR_WINDOW + 1), hCursor, MAKEINTRESOURCE(ID_TEXTWINDOW), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_TEXTWINDOW_CLASS FAILED!");
	}

	::LoadString(m_hInstance, ID_COMMANDLINE_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName, hIcon, GetSysColor(COLOR_WINDOW + 1), hCursor, NULL, CS_HREDRAW | CS_VREDRAW/*, true*/);
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_COMMANDLINE_CLASS FAILED!");
	}
}
void SystemWin32::registerMiscellaneousClasses()
{
	BOOL nResult = FALSE;
	char szClassName[MAX_LOADSTRING]{ 0 };

	::LoadString(m_hInstance, ID_DYNAMICINPUT_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName, NULL, GetSysColor(COLOR_WINDOW + 1));
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_DYNAMICINPUT_CLASS FAILED!");
	}

	::LoadString(m_hInstance, ID_DROPDOWN_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName, NULL, GetSysColor(COLOR_WINDOW + 1));
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_DROPDOWN_CLASS FAILED!");
	}

	//Register message-only command Window.
	::LoadString(m_hInstance, ID_COMMAND_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
	nResult = registerClass(szClassName, NULL, GetSysColor(COLOR_WINDOW + 1));
	if (nResult == FALSE) {
		log("Log::SystemWin32::registerClass() ID_COMMAND_CLASS FAILED!");
	}
}

BOOL SystemWin32::Is_WinXP_SP2_or_Later()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;
	int op = VER_GREATER_EQUAL;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 1;
	osvi.wServicePackMajor = 2;
	osvi.wServicePackMinor = 0;

	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMINOR, op);

	return VerifyVersionInfo(
		&osvi,
		VER_MAJORVERSION | VER_MINORVERSION |
		VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
		dwlConditionMask);
}
BOOL SystemWin32::Is_Win_Server()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;
	int op = VER_GREATER_EQUAL;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 0;
	osvi.wServicePackMajor = 0;
	osvi.wServicePackMinor = 0;
	osvi.wProductType = VER_NT_SERVER;

	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMINOR, op);
	VER_SET_CONDITION(dwlConditionMask, VER_PRODUCT_TYPE, VER_EQUAL);

	return VerifyVersionInfo(
		&osvi,
		VER_MAJORVERSION | VER_MINORVERSION |
		VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR |
		VER_PRODUCT_TYPE,
		dwlConditionMask);
}

BOOL SystemWin32::createApplication()
{
	char szClassName[MAX_LOADSTRING]{ 0 };
	char szWindowName[MAX_LOADSTRING]{ 0 };

	HINSTANCE hModule = ::LoadLibrary("mycaddata.dll");
	if (hModule != NULL) {
		::LoadString(hModule, ID_APPLICATION_CLASS, szClassName, sizeof(szClassName) / sizeof(char));
		::LoadString(hModule, ID_APPLICATION_NAME, szWindowName, sizeof(szWindowName) / sizeof(char));
		::FreeLibrary(hModule);
	}

	DWORD dwExStyle = WS_EX_CLIENTEDGE;
	DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;

	if (Is_WinXP_SP2_or_Later()) {
		//dwExStyle |= WS_EX_COMPOSITED;
	}

	RECT rcPosition = { 0,0,800,500 };
	int nCmdShow = SW_NORMAL;
	m_lpApplication->getApplicationState(rcPosition, nCmdShow);

	HMENU hMenu = ::CreateMenu();
	HMENU hFile = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(ID_FILE));
	HMENU hWindow = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(ID_WINDOW));
	HMENU hHelp = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(ID_HELP));

	::AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFile, "File");
	::AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hWindow, "Window");
	::AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelp, "Help");

	int nResult = createWindow(
		szClassName,
		szWindowName,
		m_lpApplication,
		dwExStyle,
		dwStyle,
		rcPosition.left,
		rcPosition.top,
		rcPosition.right,
		rcPosition.bottom,
		NULL,
		hMenu,
		nCmdShow
	);

	return nResult;
}

std::vector<MONITORINFOEX> SystemWin32::s_miMonitors;
BOOL CALLBACK SystemWin32::s_bfnMonitorEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT lprc, LPARAM pData)
{
	UNREFERENCED_PARAMETER(hdc);
	UNREFERENCED_PARAMETER(lprc);
	UNREFERENCED_PARAMETER(pData);

	MONITORINFOEX mi;
	mi.cbSize = sizeof(mi);

#pragma warning( push )
#pragma warning( disable : 6387)
	BOOL nResult = ::GetMonitorInfo((HMONITOR)hMonitor, (LPMONITORINFOEX)&mi);
#pragma warning( pop )

	if (nResult == TRUE) {
		s_miMonitors.push_back(mi);
		log("Log::SystemWin32::s_bfnMonitorEnumProc() Rect = { %d, %d, %d, %d }", mi.rcWork.left, mi.rcWork.top, mi.rcWork.right, mi.rcWork.bottom);
	}
	else {
		ErrorHandler();
	}

	return IDOK;
}

LRESULT WINAPI SystemWin32::s_lpfnFrameProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = FALSE;

	WindowWin32* window = (WindowWin32*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (uMsg == WM_NCCREATE) {
		CREATESTRUCT* lpCreateStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		window = reinterpret_cast<WindowWin32*>(lpCreateStruct->lpCreateParams);
		window->setHWND(hWnd);
		window->setClassName(lpCreateStruct->lpszClass);
		//window->setWindowName(lpCreateStruct->lpszName);
		window->setX(lpCreateStruct->x);
		window->setY(lpCreateStruct->y);
		window->setCX(lpCreateStruct->cx);
		window->setCY(lpCreateStruct->cy);
		window->setSystem((SystemWin32*)getSystem());
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
	}

	if (window == nullptr) {
		return ::DefFrameProc(hWnd, MDIApplication::s_hMDIClient, uMsg, wParam, lParam);
	}

	switch (uMsg) {
		case WM_ACTIVATE: {
			lResult = window->wm_activate(wParam, lParam);
			break;
		}
		case WM_CHAR: {
			lResult = window->wm_char(wParam, lParam);
			break;
		}
		case WM_CLOSE: {
			lResult = window->wm_close();
			break;
		}
		case WM_COMMAND: {
			window->wm_command(uMsg, wParam, lParam);
			lResult = ::DefFrameProc(hWnd, MDIApplication::s_hMDIClient, uMsg, wParam, lParam);
			break;
		}
		case WM_CONTEXTMENU: {
			lResult = window->wm_contextmenu(wParam, lParam);
			break;
		}
		case WM_CREATE: {
			lResult = window->wm_create(wParam, lParam);
			break;
		}
		case WM_CTLCOLOREDIT: {
			lResult = window->wm_ctlcoloredit(wParam, lParam);
			break;
		}
		case WM_CTLCOLORLISTBOX: {
			lResult = window->wm_ctlcolorlistbox(wParam, lParam);
			break;
		}
		case WM_CTLCOLORSTATIC: {
			lResult = window->wm_ctlcolorstatic(wParam, lParam);
			break;
		}
		case WM_DESTROY: {
			lResult = window->wm_destroy();
			break;
		}
		case WM_ENABLE: {
			lResult = window->wm_enable(wParam, lParam);
			break;
		}
		case WM_ENTERMENULOOP: {
			lResult = window->wm_entermenuloop(wParam);
			break;
		}
		case WM_ENTERSIZEMOVE: {
			lResult = window->wm_entersizemove();
			break;
		}
		case WM_ERASEBKGND: {
			lResult = window->wm_erasebkgnd(wParam);
			break;
		}
		case WM_EXITMENULOOP: {
			lResult = window->wm_exitmenuloop(wParam);
			break;
		}
		case WM_EXITSIZEMOVE: {
			lResult = window->wm_exitsizemove();
			break;
		}
		case WM_GETMINMAXINFO: {
			lResult = window->wm_getminmaxinfo(lParam);
			break;
		}
		case WM_HSCROLL: {
			lResult = window->wm_hscroll(wParam, lParam);
			break;
		}
		case WM_INITMENU: {
			lResult = window->wm_initmenu(wParam);
			break;
		}
		case WM_INITMENUPOPUP: {
			lResult = window->wm_initmenupopup(wParam, lParam);
			break;
		}
		case WM_KEYDOWN: {
			lResult = window->wm_keydown(wParam, lParam);
			break;
		}
		case WM_KEYUP: {
			lResult = window->wm_keyup(wParam, lParam);
			break;
		}
		case WM_KILLFOCUS: {
			lResult = window->wm_killfocus(wParam);
			break;
		}
		case WM_LBUTTONDBLCLK: {
			lResult = window->wm_lbuttondblclk(wParam, lParam);
			break;
		}
		case WM_LBUTTONDOWN: {
			lResult = window->wm_lbuttondown(wParam, lParam);
			break;
		}
		case WM_LBUTTONUP: {
			lResult = window->wm_lbuttonup(wParam, lParam);
			break;
		}
		case WM_MBUTTONDOWN: {
			lResult = window->wm_mbuttondown(wParam, lParam);
			break;
		}
		case WM_MBUTTONUP: {
			lResult = window->wm_mbuttonup(wParam, lParam);
			break;
		}
		case WM_MDIACTIVATE: {
			lResult = window->wm_mdiactivate(wParam, lParam);
			break;
		}
		case WM_MENUCHAR: {
			window->wm_menuchar(wParam, lParam);
			lResult = ::DefFrameProc(hWnd, MDIApplication::s_hMDIClient, uMsg, wParam, lParam);
			break;
		}
		case WM_MENUCOMMAND: {
			lResult = window->wm_menucommand(wParam, lParam);
			break;
		}
		case WM_MOVE: {
			lResult = window->wm_move(lParam);
			break;
		}
		case WM_MOVING: {
			lResult = window->wm_moving(lParam);
			break;
		}
		case WM_MOUSEHOVER: {
			lResult = window->wm_mousehover(wParam, lParam);
			break;
		}
		case WM_MOUSELEAVE: {
			lResult = window->wm_mouseleave();
			break;
		}
		case WM_MOUSEMOVE: {
			lResult = window->wm_mousemove(wParam, lParam);
			break;
		}
		case WM_MOUSEWHEEL: {
			lResult = window->wm_mousewheel(wParam, lParam);
			break;
		}
		case WM_NCMOUSEHOVER: {
			lResult = window->wm_ncmousehover(wParam, lParam);
			break;
		}
		case WM_NCMOUSELEAVE: {
			lResult = window->wm_ncmouseleave();
			break;
		}
		case WM_NCMOUSEMOVE: {
			lResult = window->wm_ncmousemove(wParam, lParam);
			break;
		}
		case WM_NOTIFY: {
			lResult = window->wm_notify(wParam, lParam);
			break;
		}
		case WM_PAINT: {
			window->wm_paint();
			lResult = ::DefFrameProc(hWnd, MDIApplication::s_hMDIClient, uMsg, wParam, lParam);
			break;
		}
		case WM_PARENTNOTIFY: {
			lResult = window->wm_parentnotify(wParam, lParam);
			break;
		}
		case WM_RBUTTONDOWN: {
			lResult = window->wm_rbuttondown(wParam, lParam);
			break;
		}
		case WM_SETCURSOR: {
			window->wm_setcursor(wParam, lParam);
			lResult = ::DefFrameProc(hWnd, MDIApplication::s_hMDIClient, uMsg, wParam, lParam);
			break;
		}
		case WM_SETFOCUS: {
			window->wm_setfocus(wParam);
			lResult = ::DefFrameProc(hWnd, MDIApplication::s_hMDIClient, uMsg, wParam, lParam);
			break;
		}
		case WM_SETTEXT: {
			window->wm_settext(wParam, lParam);
			lResult = ::DefFrameProc(hWnd, MDIApplication::s_hMDIClient, uMsg, wParam, lParam);
			break;
		}
		case WM_SHOWWINDOW: {
			lResult = window->wm_showwindow(wParam, lParam);
			break;
		}
		case WM_SIZE: {
			lResult = window->wm_size(wParam, lParam);
			//lResult = ::DefFrameProc(hWnd, MDIApplication::s_hMDIClient, uMsg, wParam, lParam);
			break;
		}
		case WM_SIZING: {
			lResult = window->wm_sizing(wParam, lParam);
			break;
		}
		case WM_SYSCHAR: {
			lResult = window->wm_syschar(wParam, lParam);
			break;
		}
		case WM_SYSCOMMAND: {
			window->wm_syscommand(wParam, lParam);
			lResult = ::DefFrameProc(hWnd, MDIApplication::s_hMDIClient, uMsg, wParam, lParam);
			break;
		}
		case WM_SYSKEYDOWN: {
			lResult = window->wm_syskeydown(wParam, lParam);
			break;
		}
		case WM_SYSKEYUP: {
			lResult = window->wm_syskeyup(wParam, lParam);
			break;
		}
		case WM_TIMER: {
			lResult = window->wm_timer(wParam, lParam);
			break;
		}
		case WM_UNINITMENUPOPUP: {
			lResult = window->wm_uninitmenupopup(wParam, wParam);
			break;
		}
		case WM_VSCROLL: {
			lResult = window->wm_vscroll(wParam, lParam);
			break;
		}
		default: {
			lResult = ::DefFrameProc(hWnd, MDIApplication::s_hMDIClient, uMsg, wParam, lParam);
		}
	}

	return lResult;
}
LRESULT WINAPI SystemWin32::s_lpfnMDIChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = FALSE;

	WindowWin32* window = (WindowWin32*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (uMsg == WM_NCCREATE) {
		CREATESTRUCT* lpCreateStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		MDICREATESTRUCT* lpMDICreateStruct = (MDICREATESTRUCT*)lpCreateStruct->lpCreateParams;
		window = reinterpret_cast<WindowWin32*>(lpMDICreateStruct->lParam);
		window->setHWND(hWnd);
		window->setClassName(lpCreateStruct->lpszClass);
		//window->setWindowName(lpCreateStruct->lpszName);
		window->setX(lpCreateStruct->x);
		window->setY(lpCreateStruct->y);
		window->setCX(lpCreateStruct->cx);
		window->setCY(lpCreateStruct->cy);
		window->setSystem((SystemWin32*)getSystem());
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
	}

	if (window == nullptr) {
		return ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
	}

	switch (uMsg) {
		case WM_CHAR: {
			lResult = window->wm_char(wParam, lParam);
			break;
		}
		case WM_CHILDACTIVATE: {
			window->wm_childactivate();
			lResult = ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_CLOSE: {
			lResult = window->wm_close();
			break;
		}
		case WM_COMMAND: {
			lResult = window->wm_command(wParam, lParam);
			break;
		}
		case WM_CONTEXTMENU: {
			lResult = window->wm_contextmenu(wParam, lParam);
			break;
		}
		case WM_CREATE: {
			lResult = window->wm_create(wParam, lParam);
			break;
		}
		case WM_CTLCOLOREDIT: {
			lResult = window->wm_ctlcoloredit(wParam, lParam);
			break;
		}
		case WM_CTLCOLORLISTBOX: {
			lResult = window->wm_ctlcolorlistbox(wParam, lParam);
			break;
		}
		case WM_CTLCOLORSTATIC: {
			lResult = window->wm_ctlcolorstatic(wParam, lParam);
			break;
		}
		case WM_DESTROY: {
			lResult = window->wm_destroy();
			break;
		}
		case WM_ENABLE: {
			lResult = window->wm_enable(wParam, lParam);
			break;
		}
		case WM_ENTERMENULOOP: {
			lResult = window->wm_entermenuloop(wParam);
			break;
		}
		case WM_ENTERSIZEMOVE: {
			lResult = window->wm_entersizemove();
			break;
		}
		case WM_ERASEBKGND: {
			lResult = window->wm_erasebkgnd(wParam);
			break;
		}
		case WM_EXITMENULOOP: {
			lResult = window->wm_exitmenuloop(wParam);
			break;
		}
		case WM_EXITSIZEMOVE: {
			lResult = window->wm_exitsizemove();
			break;
		}
		case WM_GETMINMAXINFO: {
			window->wm_getminmaxinfo(lParam);
			lResult = ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_HSCROLL: {
			lResult = window->wm_hscroll(wParam, lParam);
			break;
		}
		case WM_INITMENU: {
			lResult = window->wm_initmenu(wParam);
			break;
		}
		case WM_INITMENUPOPUP: {
			lResult = window->wm_initmenupopup(wParam, lParam);
			break;
		}
		case WM_KEYDOWN: {
			lResult = window->wm_keydown(wParam, lParam);
			break;
		}
		case WM_KEYUP: {
			lResult = window->wm_keyup(wParam, lParam);
			break;
		}
		case WM_KILLFOCUS: {
			lResult = window->wm_killfocus(wParam);
			break;
		}
		case WM_LBUTTONDBLCLK: {
			lResult = window->wm_lbuttondblclk(wParam, lParam);
			break;
		}
		case WM_LBUTTONDOWN: {
			lResult = window->wm_lbuttondown(wParam, lParam);
			break;
		}
		case WM_LBUTTONUP: {
			lResult = window->wm_lbuttonup(wParam, lParam);
			break;
		}
		case WM_MBUTTONDOWN: {
			lResult = window->wm_mbuttondown(wParam, lParam);
			break;
		}
		case WM_MBUTTONUP: {
			lResult = window->wm_mbuttonup(wParam, lParam);
			break;
		}
		case WM_MDIACTIVATE: {
			lResult = window->wm_mdiactivate(wParam, lParam);
			break;
		}
		case WM_MENUCHAR: {
			window->wm_menuchar(wParam, lParam);
			lResult = ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_MENUCOMMAND: {
			lResult = window->wm_menucommand(wParam, lParam);
			break;
		}
		case WM_MOVE: {
			window->wm_move(lParam);
			lResult = ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_MOVING: {
			lResult = window->wm_moving(lParam);
			break;
		}
		case WM_MOUSEHOVER: {
			lResult = window->wm_mousehover(wParam, lParam);
			break;
		}
		case WM_MOUSELEAVE: {
			lResult = window->wm_mouseleave();
			break;
		}
		case WM_MOUSEMOVE: {
			lResult = window->wm_mousemove(wParam, lParam);
			break;
		}
		case WM_MOUSEWHEEL: {
			lResult = window->wm_mousewheel(wParam, lParam);
			break;
		}
		case WM_NCMOUSEHOVER: {
			lResult = window->wm_ncmousehover(wParam, lParam);
			break;
		}
		case WM_NCMOUSELEAVE: {
			lResult = window->wm_ncmouseleave();
			break;
		}
		case WM_NCMOUSEMOVE: {
			lResult = window->wm_ncmousemove(wParam, lParam);
			break;
		}
		case WM_NOTIFY: {
			lResult = window->wm_notify(wParam, lParam);
			break;
		}
		case WM_PAINT: {
			window->wm_paint();
			lResult = ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_PARENTNOTIFY: {
			lResult = window->wm_parentnotify(wParam, lParam);
			break;
		}
		case WM_RBUTTONDOWN: {
			lResult = window->wm_rbuttondown(wParam, lParam);
			break;
		}
		case WM_SETCURSOR: {
			window->wm_setcursor(wParam, lParam);
			lResult = ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_SETFOCUS: {
			window->wm_setfocus(wParam);
			lResult = ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_SETTEXT: {
			window->wm_settext(wParam, lParam);
			lResult = ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_SHOWWINDOW: {
			lResult = window->wm_showwindow(wParam, lParam);
			break;
		}
		case WM_SIZE: {
			window->wm_size(wParam, lParam);
			lResult = ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_SIZING: {
			lResult = window->wm_sizing(wParam, lParam);
			break;
		}
		case WM_SYSCHAR: {
			lResult = window->wm_syschar(wParam, lParam);
			break;
		}
		case WM_SYSCOMMAND: {
			window->wm_syscommand(wParam, lParam);
			lResult = ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_SYSKEYDOWN: {
			lResult = window->wm_syskeydown(wParam, lParam);
			break;
		}
		case WM_SYSKEYUP: {
			lResult = window->wm_syskeyup(wParam, lParam);
			break;
		}
		case WM_TIMER: {
			lResult = window->wm_timer(wParam, lParam);
			break;
		}
		case WM_UNINITMENUPOPUP: {
			lResult = window->wm_uninitmenupopup(wParam, wParam);
			break;
		}
		case WM_VSCROLL: {
			lResult = window->wm_vscroll(wParam, lParam);
			break;
		}
		default: {
			lResult = ::DefMDIChildProc(hWnd, uMsg, wParam, lParam);
		}
	}

	return lResult;
}
LRESULT WINAPI SystemWin32::s_lpfnWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = FALSE;

	WindowWin32* window = (WindowWin32*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (uMsg == WM_NCCREATE) {
		CREATESTRUCT* lpCreateStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		window = reinterpret_cast<WindowWin32*>(lpCreateStruct->lpCreateParams);
		window->setHWND(hWnd);
		window->setClassName(lpCreateStruct->lpszClass);
		//window->setWindowName(lpCreateStruct->lpszName);
		window->setX(lpCreateStruct->x);
		window->setY(lpCreateStruct->y);
		window->setCX(lpCreateStruct->cx);
		window->setCY(lpCreateStruct->cy);
		window->setSystem((SystemWin32*)getSystem());
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
	}

	if (window == nullptr) {
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	switch (uMsg) {
		case WM_ACTIVATE: {
			lResult = window->wm_activate(wParam, lParam);
			break;
		}
		case WM_CHAR: {
			lResult = window->wm_char(wParam, lParam);
			break;
		}
		case WM_CLOSE: {
			lResult = window->wm_close();
			break;
		}
		case WM_COMMAND: {
			lResult = window->wm_command(wParam, lParam);
			lResult = window->wm_command(uMsg, wParam, lParam);
			break;
		}
		case WM_CONTEXTMENU: {
			lResult = window->wm_contextmenu(wParam, lParam);
			break;
		}
		case WM_CREATE: {
			lResult = window->wm_create(wParam, lParam);
			break;
		}
		case WM_CTLCOLOREDIT: {
			lResult = window->wm_ctlcoloredit(wParam, lParam);
			break;
		}
		case WM_CTLCOLORLISTBOX: {
			lResult = window->wm_ctlcolorlistbox(wParam, lParam);
			break;
		}
		case WM_CTLCOLORSTATIC: {
			lResult = window->wm_ctlcolorstatic(wParam, lParam);
			break;
		}
		case WM_DESTROY: {
			lResult = window->wm_destroy();
			break;
		}
		case WM_ENABLE: {
			lResult = window->wm_enable(wParam, lParam);
			break;
		}
		case WM_ENTERMENULOOP: {
			lResult = window->wm_entermenuloop(wParam);
			break;
		}
		case WM_ENTERSIZEMOVE: {
			lResult = window->wm_entersizemove();
			break;
		}
		case WM_ERASEBKGND: {
			lResult = window->wm_erasebkgnd(wParam);
			break;
		}
		case WM_EXITMENULOOP: {
			lResult = window->wm_exitmenuloop(wParam);
			break;
		}
		case WM_EXITSIZEMOVE: {
			lResult = window->wm_exitsizemove();
			break;
		}
		case WM_GETMINMAXINFO: {
			lResult = window->wm_getminmaxinfo(lParam);
			break;
		}
		case WM_HSCROLL: {
			lResult = window->wm_hscroll(wParam, lParam);
			break;
		}
		case WM_INITMENU: {
			lResult = window->wm_initmenu(wParam);
			break;
		}
		case WM_INITMENUPOPUP: {
			lResult = window->wm_initmenupopup(wParam, lParam);
			break;
		}
		case WM_KEYDOWN: {
			lResult = window->wm_keydown(wParam, lParam);
			break;
		}
		case WM_KEYUP: {
			lResult = window->wm_keyup(wParam, lParam);
			break;
		}
		case WM_KILLFOCUS: {
			lResult = window->wm_killfocus(wParam);
			break;
		}
		case WM_LBUTTONDBLCLK: {
			lResult = window->wm_lbuttondblclk(wParam, lParam);
			break;
		}
		case WM_LBUTTONDOWN: {
			lResult = window->wm_lbuttondown(wParam, lParam);
			break;
		}
		case WM_LBUTTONUP: {
			lResult = window->wm_lbuttonup(wParam, lParam);
			break;
		}
		case WM_MBUTTONDOWN: {
			lResult = window->wm_mbuttondown(wParam, lParam);
			break;
		}
		case WM_MBUTTONUP: {
			lResult = window->wm_mbuttonup(wParam, lParam);
			break;
		}
		case WM_MDIACTIVATE: {
			lResult = window->wm_mdiactivate(wParam, lParam);
			break;
		}
		case WM_MENUCHAR: {
			lResult = window->wm_menuchar(wParam, lParam);
			break;
		}
		case WM_MENUCOMMAND: {
			lResult = window->wm_menucommand(wParam, lParam);
			break;
		}
		case WM_MOVE: {
			lResult = window->wm_move(lParam);
			break;
		}
		case WM_MOVING: {
			lResult = window->wm_moving(lParam);
			break;
		}
		case WM_MOUSEHOVER: {
			lResult = window->wm_mousehover(wParam, lParam);
			break;
		}
		case WM_MOUSELEAVE: {
			lResult = window->wm_mouseleave();
			break;
		}
		case WM_MOUSEMOVE: {
			lResult = window->wm_mousemove(wParam, lParam);
			break;
		}
		case WM_MOUSEWHEEL: {
			lResult = window->wm_mousewheel(wParam, lParam);
			break;
		}
		case WM_NCMOUSEHOVER: {
			lResult = window->wm_ncmousehover(wParam, lParam);
			break;
		}
		case WM_NCMOUSELEAVE: {
			lResult = window->wm_ncmouseleave();
			break;
		}
		case WM_NCMOUSEMOVE: {
			lResult = window->wm_ncmousemove(wParam, lParam);
			break;
		}
		case WM_NOTIFY: {
			lResult = window->wm_notify(wParam, lParam);
			break;
		}
		case WM_PAINT: {
			window->wm_paint();
			lResult = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_PARENTNOTIFY: {
			lResult = window->wm_parentnotify(wParam, lParam);
			break;
		}
		case WM_RBUTTONDOWN: {
			lResult = window->wm_rbuttondown(wParam, lParam);
			break;
		}
		case WM_SETCURSOR: {
			lResult = window->wm_setcursor(wParam, lParam);
			break;
		}
		case WM_SETFOCUS: {
			lResult = window->wm_setfocus(wParam);
			break;
		}
		case WM_SETTEXT: {
			window->wm_settext(wParam, lParam);
			lResult = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_SHOWWINDOW: {
			lResult = window->wm_showwindow(wParam, lParam);
			break;
		}
		case WM_SIZE: {
			lResult = window->wm_size(wParam, lParam);
			break;
		}
		case WM_SIZING: {
			lResult = window->wm_sizing(wParam, lParam);
			break;
		}
		case WM_SYSCHAR: {
			lResult = window->wm_syschar(wParam, lParam);
			break;
		}
		case WM_SYSCOMMAND: {
			window->wm_syscommand(wParam, lParam);
			lResult = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		case WM_SYSKEYDOWN: {
			lResult = window->wm_syskeydown(wParam, lParam);
			break;
		}
		case WM_SYSKEYUP: {
			lResult = window->wm_syskeyup(wParam, lParam);
			break;
		}
		case WM_TIMER: {
			lResult = window->wm_timer(wParam, lParam);
			break;
		}
		case WM_UNINITMENUPOPUP: {
			lResult = window->wm_uninitmenupopup(wParam, wParam);
			break;
		}
		case WM_VSCROLL: {
			lResult = window->wm_vscroll(wParam, lParam);
			break;
		}
		default: {
			lResult = ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	return lResult;
}

INT_PTR CALLBACK SystemWin32::s_npDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = FALSE;

	WindowWin32* window = (WindowWin32*)GetWindowLongPtr(hDlg, DWLP_USER);
	if (uMsg == WM_INITDIALOG) {
		window = (WindowWin32*)lParam;
		window->setHWND(hDlg);
		window->setSystem((SystemWin32*)getSystem());
		::SetWindowLongPtr(hDlg, DWLP_USER, lParam);
	}

	if (window == nullptr) {
		return FALSE;
	}

	switch (uMsg) {
		case WM_COMMAND: {
			lResult = window->wm_command(wParam, lParam);
			break;
		}
		case WM_CLOSE: {
			lResult = window->wm_close();
			break;
		}
		case WM_CTLCOLORDLG: {
			lResult = window->wm_ctlcolordlg(wParam, lParam);
			break;
		}
		case WM_CTLCOLOREDIT: {
			lResult = window->wm_ctlcoloredit(wParam, lParam);
			break;
		}
		case WM_CTLCOLORLISTBOX: {
			lResult = window->wm_ctlcolorlistbox(wParam, lParam);
			break;
		}
		case WM_CTLCOLORSTATIC: {
			return window->wm_ctlcolorstatic(wParam, lParam);
		}
		case WM_DESTROY: {
			lResult = window->wm_destroy();
			break;
		}
		case WM_ERASEBKGND: {
			lResult = window->wm_erasebkgnd(wParam);
			break;
		}
		case WM_GETMINMAXINFO: {
			lResult = window->wm_getminmaxinfo(lParam);
			break;
		}
		case WM_INITDIALOG: {
			lResult = window->wm_initdialog(wParam, lParam);
			break;
		}
		case WM_MOVE: {
			lResult = window->wm_move(lParam);
			break;
		}
		case WM_NOTIFY: {
			lResult = window->wm_notify(wParam, lParam);
			break;
		}
		case WM_PAINT: {
			window->wm_paint();
			lResult = ::DefWindowProc(hDlg, uMsg, wParam, lParam);
			break;
		}
		case WM_SETCURSOR: {
			lResult = window->wm_setcursor(wParam, lParam);
			break;
		}
		case WM_SETFONT: {
			lResult = window->wm_setfont(wParam, lParam);
			break;
		}
		case WM_SETTEXT: {
			window->wm_settext(wParam, lParam);
			lResult = ::DefWindowProc(hDlg, uMsg, wParam, lParam);
			break;
		}
		case WM_SIZE: {
			lResult = window->wm_size(wParam, lParam);
			break;
		}
		case WM_SYSCOMMAND: {
			lResult = window->wm_syscommand(wParam, lParam);
			break;
		}
		default: {
			lResult = FALSE;
			break;
		}
	}

	return lResult;
}
