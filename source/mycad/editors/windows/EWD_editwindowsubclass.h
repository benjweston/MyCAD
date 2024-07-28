#pragma once

#include "wm_windowwin32.h"

namespace mycad {
	/**
	* Declaration of EditWindowSubclass class.
	*/
	/*!
	Class container for subclassed edit controls.
	The WC_EDIT control class implementation in commoncontrol.dll calls MessageBeep when the text limit is exceeded, an
	invalid character is entered, or when the VK_ESCAPE, VK_RETURN or VK_TAB key is pressed in the WM_CHAR event. This
	function handles these key press events and then returns 0, thereby preventing the call to the MessageBeep function.
	*/
	class EditWindowSubclass : public WindowWin32 {
	public:
		EditWindowSubclass(HWND hWnd);
		~EditWindowSubclass();

		int wm_contextmenu(WPARAM wParam, LPARAM lParam) override;

	private:
		static LRESULT CALLBACK s_lpfnEditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	};
}
