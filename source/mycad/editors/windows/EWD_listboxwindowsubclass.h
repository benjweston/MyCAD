#pragma once

#include "wm_windowwin32.h"

#include <commctrl.h>//Required in header for TOOLINFO #define

namespace mycad {
	/**
	* Declaration of ListboxWindowSubclass class.
	*/
	class ListboxWindowSubclass : public WindowWin32 {
	public:
		ListboxWindowSubclass(HWND hWnd, HINSTANCE hInstance);
		~ListboxWindowSubclass();

		int wm_contextmenu(WPARAM wParam, LPARAM lParam) override;
		int wm_mousehover(WPARAM wParam, LPARAM lParam) override;
		int wm_mouseleave() override;
		int wm_mousemove(WPARAM wParam, LPARAM lParam) override;

	private:
		static LRESULT CALLBACK s_lpfnListboxSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

		MouseTrackEvents m_mouseTrackLeave;
		MouseTrackEvents m_mouseTrackHover;

		HWND m_hTooltip;
		TOOLINFO m_ToolInfo;

		POINT m_ptMouseMove = { 0,0 };
		int m_nIndex = 0;

	};
}
