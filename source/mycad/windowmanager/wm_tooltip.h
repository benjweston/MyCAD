#pragma once

#include "wm_window.h"

#include <commctrl.h>

#define MAX_TTM_SETTITLE 100

namespace mycad {
	/**
	* Declaration of Tooltip class.
	*/
	class Tooltip : public Window {
	public:
		/**
		* Constructor.
		* Creates a new window.
		* To check if the window was created properly, use the #getValid() method.
		*/
		Tooltip();
		Tooltip(DWORD dwStyle, HWND parent, HINSTANCE hInstance, UINT uFlags, UINT_PTR uId, UINT uMaxWidth = 0);
		Tooltip(DWORD dwStyle, HWND parent, HINSTANCE hInstance, UINT uFlags, UINT_PTR uId, const char* lpszText, UINT uMaxWidth = 0);
		Tooltip(DWORD dwStyle, HWND parent, HINSTANCE hInstance, UINT uFlags, UINT_PTR uId, const char* lpszTitle, const char* lpszText, UINT uMaxWidth = 0);

		/**
		* Destructor.
		* Closes the window and disposes resources allocated.
		*/
		virtual ~Tooltip();

		BOOL setPosition(LPRECT lprc);
		BOOL setPosition(long left, long top, long right, long bottom);

		BOOL setText(const char* lpszText);

		BOOL setTitle(const char* lpszTitle, INT nIcon = TTI_NONE);

		INT setMaxTipWidth(int nWidth);

		void trackActivate(BOOL bState);
		void trackPosition(POINT ptPosition);

		LPTOOLINFO getToolInfo();

	private:
		TOOLINFO m_ToolInfo;

		char m_szTooltipTitle[MAX_TTM_SETTITLE];
		char m_szTooltipText[MAX_LOADSTRING];

	};
}