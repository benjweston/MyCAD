#include "wm_window.h"

#include "SYS_systemwin32.h"

#include "fault.h"
#include "log.h"

using namespace mycad;

HINSTANCE Window::s_hInstance(0);

Window::Window()
	: IWindow(),
	m_nActive(3),//Set m_nActive value above WA_INACTIVE(0), WA_ACTIVE(1) & WA_CLICKACTIVE(2), and use WM_ACTIVATE event to open default document on application start.
	m_hWnd(0),
	m_system(0)
{
	//log("Log::Window::Window()");
}
Window::Window(HINSTANCE hInstance)
	: IWindow(),
	m_nActive(3),
	m_hWnd(0),
	m_system(0)
{
	//log("Log::Window::Window()");
	s_hInstance = hInstance;
}
Window::~Window()
{
	//log("Log::Window::~Window()");

}

Window* Window::getWindow() { return this; }

BOOL Window::getActive() const { return m_nActive; }

BOOL Window::getValid() const { return ::IsWindow(m_hWnd); }//Class function wrapper for BOOL IsWindow(HWND hWnd) function.

void Window::setHWND(HWND hwnd) { m_hWnd = hwnd; }
HWND Window::getHWND() const { return m_hWnd; }

void Window::setMenu(HMENU hMenu) { m_hMenu = hMenu; }
HMENU Window::getMenu() const { return m_hMenu; }

void Window::setFont(HWND hWnd)
{
	//HFONT DeafultFont = CreateFont(-11, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "Microsoft Sans Serif");
	//::SendMessage(hWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)TRUE);

	NONCLIENTMETRICS nclim;
	nclim.cbSize = sizeof(NONCLIENTMETRICS);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &nclim, 0);
	HFONT mFont = ::CreateFontIndirect(&nclim.lfMessageFont);

	::SendMessage(hWnd, WM_SETFONT, (WPARAM)mFont, (LPARAM)TRUE);
}
void Window::setFont(HWND hWnd, const char* text, bool italic)
{
	LOGFONT logfont;
	ZeroMemory(&logfont, sizeof(LOGFONT));

	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfHeight = 14;
	logfont.lfWidth = 6;
	logfont.lfWeight = FW_MEDIUM;
	logfont.lfItalic = italic;
	*logfont.lfFaceName = *text;

	HFONT hFont = CreateFontIndirect(&logfont);
	::SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
}

void Window::setSystem(SystemWin32* system) { m_system = system; }
SystemWin32* Window::getSystem() const { return m_system; }

int Window::wm_activate(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_char(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_childactivate() { return 0; }
int Window::wm_close() { return 0; }
int Window::wm_command(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_command(UINT uMsg, WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(uMsg); UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_contextmenu(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_create(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_ctlcolordlg(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_ctlcoloredit(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_ctlcolorlistbox(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_destroy() { return 0; }
int Window::wm_dpichanged(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_enable(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_entermenuloop(WPARAM wParam) { UNREFERENCED_PARAMETER(wParam); return 0; }
int Window::wm_entersizemove() { return 0; }
int Window::wm_erasebkgnd(WPARAM wParam) { UNREFERENCED_PARAMETER(wParam); return 0; }
int Window::wm_exitmenuloop(WPARAM wParam) { UNREFERENCED_PARAMETER(wParam); return 0; }
int Window::wm_exitsizemove() { return 0; }
int Window::wm_getminmaxinfo(LPARAM lParam) { UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_hscroll(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_initdialog(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_initmenu(WPARAM wParam) { UNREFERENCED_PARAMETER(wParam); return 0; }
int Window::wm_initmenupopup(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_keydown(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_keyup(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_killfocus(WPARAM wParam) { UNREFERENCED_PARAMETER(wParam); return 0; }
int Window::wm_lbuttondblclk(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_lbuttondown(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_lbuttonup(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_mbuttondown(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_mbuttonup(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_mdiactivate(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_menuchar(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_menucommand(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_move(LPARAM lParam) { UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_moving(LPARAM lParam) { UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_mousehover(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_mouseleave() { return 0; }
int Window::wm_mousemove(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_mousewheel(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_ncactivate(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_nccalcsize(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_nccreate(LPARAM lParam) { UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_ncmousehover(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_ncmouseleave() { return 0; }
int Window::wm_ncmousemove(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_ncpaint(WPARAM wParam) { UNREFERENCED_PARAMETER(wParam); return 0; }
int Window::wm_notify(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_paint() { return 0; }
int Window::wm_parentnotify(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_rbuttondown(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_rbuttonup(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_setcursor(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_setfocus(WPARAM wParam) { UNREFERENCED_PARAMETER(wParam); return 0; }
int Window::wm_setfont(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam);  return 0; }
int Window::wm_settext(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam);  return 0; }
int Window::wm_showwindow(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_size(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_sizing(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_syschar(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_syscommand(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_syskeydown(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_syskeyup(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_timer(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_uninitmenupopup(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_vscroll(WPARAM wParam, LPARAM lParam) { UNREFERENCED_PARAMETER(wParam); UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_windowposchanged(LPARAM lParam) { UNREFERENCED_PARAMETER(lParam); return 0; }
int Window::wm_windowposchanging(LPARAM lParam) { UNREFERENCED_PARAMETER(lParam); return 0; }
