#include "SYS_systemwin32.h"

#include "EWD_commandlinewindow.h"
#include "EWD_resource.h"

#include "fault.h"
#include "log.h"

#include <intsafe.h>//Required for IntPtrToInt

using namespace mycad;

int CommandLineWindow::s_nCmdHistLines{ 400 };

CommandLineWindow::CommandLineWindow(DynamicInputWindow* dynamicinput, int dropdownlines)
	: DynamicInputWindow(dropdownlines),
	m_lpCommandInput(nullptr),
	m_lpDynamicInput(dynamicinput),
	m_hHistory(0),
	m_clrBackground_History(RGB(200, 200, 200)),
	m_hbrBackground_History(0)
{
	log("Log::CommandLineWindow::CommandLineWindow()");
}
CommandLineWindow::~CommandLineWindow()
{
	log("Log::CommandLineWindow::~CommandLineWindow()");
}

void CommandLineWindow::setCommandInput(DynamicInputWindow* commandinput) { m_lpCommandInput = commandinput; }

void CommandLineWindow::addString(const char* text)
{
	::SendMessage(m_hHistory, EM_SETSEL, (WPARAM)0, (LPARAM)-1);//Select all,...
	::SendMessage(m_hHistory, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);//...unselect and stay at the end position...
	::SendMessage(m_hHistory, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)text);//...and append text to current position.
	::SendMessage(m_hHistory, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);//...and set scrollcaret to the current position.
}
int CommandLineWindow::getLineCount() { return (int)::SendMessage(m_hHistory, EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0); }

int CommandLineWindow::wm_close()
{
	if (::IsIconic(m_hWnd) == FALSE) {
		RECT rcWindow = { 0,0,0,0 };
		::GetWindowRect(m_hWnd, &rcWindow);

		m_x = rcWindow.left;
		m_y = rcWindow.top;
		m_cx = rcWindow.right - rcWindow.left;
		m_cy = rcWindow.bottom - rcWindow.top;
	}

	if (::IsWindowVisible(m_lpDropdownWindow->getHWND()) == TRUE) {
		::ShowWindow(m_lpDropdownWindow->getHWND(), SW_HIDE);
	}

	::DestroyWindow(m_lpDropdownWindow->getHWND());
	::DestroyWindow(m_hHistory);
	::DestroyWindow(m_hCommand);
	::DestroyWindow(m_hWnd);

	return 0;
}
int CommandLineWindow::wm_destroy()
{
	::DeleteObject(m_hbrBackground_History);
	::DeleteObject(m_hbrBackground_Command);

	m_system->getWindowManager().setObjectInactive(this);
	BOOL nResult = m_system->getWindowManager().removeObject(this);
	if (nResult == TRUE) {
		log("Log::CommandLine::wm_destroy() m_system->getWindowManager().removeWindow == TRUE");
	}
	else {
		log("Log::CommandLine::wm_destroy() m_system->getWindowManager().removeWindow == FALSE");
	}

	return 0;
}
int CommandLineWindow::wm_setcursor(WPARAM wParam, LPARAM lParam)//Set cursor and return FALSE for default processing.
{
	POINT ptCursor;
	::GetCursorPos(&ptCursor);
	::ScreenToClient((HWND)wParam, &ptCursor);
	//if (::PtInRect(&m_rcCommand, ptCursor) == TRUE) {
	//	::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
	//	return 0;
	//}

	return (int)::DefWindowProc(m_hWnd, WM_SETCURSOR, wParam, lParam);
}
int CommandLineWindow::wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam)
{
	HRESULT hResult = S_OK;
	INT nResult = 0;

	HDC hDC = (HDC)wParam;

	HWND hWnd = (HWND)lParam;
	int id = ::GetDlgCtrlID(hWnd);

	if (id == IDC_TEXTWINDOW_STATIC_COMMAND) return 0;

	::SetTextColor(hDC, m_clrText);
	::SetBkColor(hDC, m_clrBackground_History);
	::SetDCBrushColor(hDC, m_clrBackground_History);
	::SelectObject(hDC, m_hbrBackground_History);

	INT_PTR npBrush = (INT_PTR)m_hbrBackground_History;
	hResult = ::IntPtrToInt(npBrush, &nResult);
	if (hResult == S_OK) {
		return nResult;
	}

	return 0;
}
