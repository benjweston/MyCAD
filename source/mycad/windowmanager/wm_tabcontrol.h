#pragma once

#include "wm_window.h"

#include "wm_tooltip.h"

#include <commctrl.h>
#include <vector>
#include <array>

namespace mycad {
	/**
	* Declaration of Tabcontrol class.
	*/
	class Tabcontrol : public Window {
	public:
		/**
		* Constructor.
		* Creates a new window.
		* To check if the window was created properly, use the #getValid() method.
		*/
		Tabcontrol(DWORD dwStyle,
			int x,
			int y,
			int nWidth,
			int nHeight,
			HWND hwndParent,
			UINT uId,
			HINSTANCE hInstance
		);
		Tabcontrol(DWORD dwExStyle,
			DWORD dwStyle,
			int x,
			int y,
			int nWidth,
			int nHeight,
			HWND hwndParent,
			UINT uId,
			HINSTANCE hInstance
		);
		/**
		* Destructor.
		* Closes the window and disposes resources allocated.
		*/
		virtual ~Tabcontrol();

		int wm_windowposchanged(LPARAM lParam);
		int wm_windowposchanging(LPARAM lParam);

	private:
		UINT m_uIdSubclass;

		static LRESULT CALLBACK s_lpfnTabcontrolSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	};
}
