#include "wm_statusbar.h"
#include "wm_tooltip.h"

#include "fault.h"
#include "log.h"

using namespace mycad;

Statusbar::Statusbar(DWORD dwStyle,
	HWND hwndParent,
	UINT uId,
	HINSTANCE hInstance)
	: Window(hInstance),
	m_rcStatusbar({ 0,0,0,0 }),
	m_nPanels(0),
	m_nPanelPositions(0)
{
	//log("Log::Statusbar::Statusbar()");

	HRESULT hResult = S_OK;

	m_hWnd = ::CreateWindow(
		STATUSCLASSNAME,
		NULL,
		dwStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		hwndParent,
		(HMENU)(UINT_PTR)uId,
		hInstance,
		(LPVOID)0
	);

	if (m_hWnd == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)

		LONG_PTR lpStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
		lpStyle &= ~SBARS_SIZEGRIP;
		::SetWindowLongPtr(m_hWnd, GWL_STYLE, lpStyle);

		::SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		::ShowWindow(m_hWnd, SW_SHOW);

		setFont(m_hWnd);

		::SendMessage(m_hWnd, WM_SIZE, 0, 0);
		::GetClientRect(m_hWnd, &m_rcStatusbar);

#pragma warning( pop )
	}
}
Statusbar::~Statusbar()
{
	//log("Log::Statusbar::~Statusbar()");

	::DestroyWindow(m_hWnd);
}

BOOL Statusbar::setText(UINT uIndex, const char* szText, UINT uFlags) { return (BOOL)::SendMessage(m_hWnd, SB_SETTEXT, MAKEWPARAM(uIndex, uFlags), (LPARAM)szText); }

BOOL Statusbar::setTooltip(UINT uIndex, Tooltip tooltip)
{
	if (m_nPanels.size() == 0) return FALSE;
	if (uIndex > m_nPanels.size()) return FALSE;

	m_nPanels[uIndex]->lpTooltip = NULL;
	delete m_nPanels[uIndex]->lpTooltip;

	m_nPanels[uIndex]->lpTooltip = &tooltip;

	return TRUE;
}
BOOL Statusbar::setTooltip(UINT uIndex, DWORD dwStyle, UINT uFlags, UINT_PTR uId)
{
	if (m_nPanels.size() == 0) return FALSE;
	if (uIndex > m_nPanels.size()) return FALSE;

	m_nPanels[uIndex]->lpTooltip = NULL;
	delete m_nPanels[uIndex]->lpTooltip;

	m_nPanels[uIndex]->lpTooltip = new Tooltip(dwStyle, m_hWnd, s_hInstance, uFlags, uId);

	return TRUE;
}
BOOL Statusbar::setTooltip(UINT uIndex, DWORD dwStyle, UINT uFlags, UINT_PTR uId, const char* lpszText)
{
	if (m_nPanels.size() == 0) return FALSE;
	if (uIndex > m_nPanels.size()) return FALSE;

	m_nPanels[uIndex]->lpTooltip = NULL;
	delete m_nPanels[uIndex]->lpTooltip;

	m_nPanels[uIndex]->lpTooltip = new Tooltip(dwStyle, m_hWnd, s_hInstance, uFlags, uId, lpszText);

	return TRUE;
}
BOOL Statusbar::setTooltip(UINT uIndex, DWORD dwStyle, UINT uFlags, UINT_PTR uId, const char* lpszTitle, const char* lpszText)
{
	if (m_nPanels.size() == 0) return FALSE;
	if (uIndex > m_nPanels.size()) return FALSE;

	m_nPanels[uIndex]->lpTooltip = NULL;
	delete m_nPanels[uIndex]->lpTooltip;

	m_nPanels[uIndex]->lpTooltip = new Tooltip(dwStyle, m_hWnd, s_hInstance, uFlags, uId, lpszTitle, lpszText);

	return TRUE;
}
BOOL Statusbar::setTooltipText(UINT uIndex, const char* szText) { return m_nPanels[uIndex]->lpTooltip->setText(szText); }
BOOL Statusbar::setTooltipTitle(UINT uIndex, const char* szText) { return m_nPanels[uIndex]->lpTooltip->setTitle(szText); }

//template<std::size_t SIZE>
//void setParts(std::array<int, SIZE>& panels) - Template function implementation in header.
const int Statusbar::getParts() { return (int)m_nPanelPositions.size(); }
int Statusbar::getParts(int number, int nParts[])
{
	if (nParts == 0) {
		return (int)m_nPanelPositions.size();
	}

	int nNumber = number;
	if (number > m_nPanelPositions.size()) {
		nNumber = (int)m_nPanelPositions.size();
	}
	for (int i = 0; i < nNumber; i++) {
		nParts[i] = m_nPanelPositions[i];
	}
	return (int)m_nPanelPositions.size();
}

BOOL Statusbar::setPanelVisibility(UINT uIndex, BOOL nVisibility)
{
	if (m_nPanels.size() == 0) return FALSE;
	if (uIndex > m_nPanels.size()) return FALSE;

	m_nPanels[uIndex]->nVisible = nVisibility;

	return TRUE;
}
BOOL Statusbar::getPanelVisibility(UINT uIndex)
{
	if (m_nPanels.size() == 0) return FALSE;
	if (uIndex > m_nPanels.size()) return FALSE;

	return m_nPanels[uIndex]->nVisible;
}

HRESULT Statusbar::showProgressbar(UINT uIndex)
{
	HRESULT hResult = S_OK;

	if (m_nPanels.size() == 0) return E_FAIL;
	if (uIndex > m_nPanels.size()) return E_FAIL;

	if (m_nPanels[uIndex]->hwndProgress == NULL) {
		m_nPanels[uIndex]->hwndProgress = ::CreateWindow(
			PROGRESS_CLASS,
			NULL,
			WS_VISIBLE | WS_CHILD,
			0,
			0,
			0,
			0,
			m_hWnd,
			(HMENU)(INT_PTR)uIndex,
			s_hInstance,
			(LPVOID)0
		);
	}

	if (m_nPanels[uIndex]->hwndProgress == NULL) {
		hResult = ErrorHandler();
	}

	if (SUCCEEDED(hResult)) {
#pragma warning( push )
#pragma warning( disable : 6387)
		::SetWindowPos(m_nPanels[uIndex]->hwndProgress, HWND_TOP, m_nPanelPositions[uIndex], 0, m_nPanels[uIndex]->uWidth, m_rcStatusbar.bottom, SWP_SHOWWINDOW);
#pragma warning( pop )
	}

	return hResult;
}
int Statusbar::showProgressbar(UINT uIndex, int nPosition)
{
	int nResult = 0;

	if (m_nPanels.size() == 0) return E_FAIL;
	if (uIndex > m_nPanels.size()) return E_FAIL;

	if (m_nPanels[uIndex]->hwndProgress != NULL) {
		nResult = (int)::SendMessage(m_nPanels[uIndex]->hwndProgress, PBM_SETPOS, (WPARAM)nPosition, 0);
	}

	return nResult;
}
void Statusbar::hideProgressbar(UINT uIndex)
{
	::ShowWindow(m_nPanels[uIndex]->hwndProgress, SW_HIDE);
	::DestroyWindow(m_nPanels[uIndex]->hwndProgress);
}

//LPRECT Statusbar::getClientRect() { return &m_rcStatusbar; }

int Statusbar::wm_exitsizemove()
{
	BOOL nResult = FALSE;

	if (m_nPanelPositions.size() == 0) return 0;

	for (unsigned int i = 1; i < (int)m_nPanelPositions.size(); i++) {
		if (m_nPanels[i - 1]->nVisible == TRUE) {
			if (m_nPanels[i - 1]->lpTooltip != nullptr) {
				nResult = m_nPanels[i - 1]->lpTooltip->setPosition(m_nPanelPositions[i - 1], m_rcStatusbar.top, m_nPanelPositions[i], m_rcStatusbar.bottom);
			}
			if (nResult == FALSE) {
				ErrorHandler();
			}
		}
	}

	return 0;
}
int Statusbar::wm_size()
{
	BOOL nResult = FALSE;

	if (m_nPanelPositions.size() < 2) return 0;

	::SendMessage(m_hWnd, WM_SIZE, 0, 0);
	::GetClientRect(m_hWnd, &m_rcStatusbar);

	m_nPanelPositions[m_nPanelPositions.size() - 1] = m_rcStatusbar.right - m_nPanels[m_nPanelPositions.size() - 1]->uWidth;
	for (int i = (int)m_nPanelPositions.size() - 2; i >= 0; i--) {
		if (m_nPanels[i]->nVisible == TRUE) {
			m_nPanelPositions[i] = m_nPanelPositions[i + 1] - m_nPanels[i]->uWidth;
		}
		else {
			m_nPanelPositions[i] = m_nPanelPositions[i + 1];
		}
		if (m_nPanels[i]->hwndProgress != NULL) {
			::SetWindowPos(m_nPanels[i]->hwndProgress,
				HWND_TOP,
				m_nPanelPositions[i],
				0,
				m_nPanels[i]->uWidth,
				m_rcStatusbar.bottom,
				SWP_SHOWWINDOW
			);
		}
	}

	nResult = (BOOL)::SendMessage(m_hWnd, SB_SETPARTS, (WPARAM)m_nPanelPositions.size(), (LPARAM)m_nPanelPositions.data());

	if (nResult == FALSE) {
		ErrorHandler();
	}

	return 0;
}
