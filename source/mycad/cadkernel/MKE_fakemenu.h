#pragma once

#include "wm_windowwin32.h"

namespace mycad {
	/**
	* Declaration of Fakemenu class.
	*/
	class Fakemenu : public WindowWin32 {
	public:
		Fakemenu();
		~Fakemenu();

		int wm_activate(WPARAM, LPARAM) override;
		int wm_char(WPARAM wParam, LPARAM lParam);
		int wm_close() override;
		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_create(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;
		int wm_keydown(WPARAM wParam, LPARAM lParam) override;
		int wm_killfocus(WPARAM wParam) override;
		int wm_lbuttondblclk(WPARAM wParam, LPARAM lParam) override;
		int wm_lbuttondown(WPARAM wParam, LPARAM lParam) override;
		int wm_lbuttonup(WPARAM wParam, LPARAM lParam) override;
		int wm_mouseleave() override;
		int wm_mousemove(WPARAM wParam, LPARAM lParam) override;
		int wm_mousewheel(WPARAM wParam, LPARAM lParam) override;
		int wm_notify(WPARAM wParam, LPARAM lParam) override;
		int wm_paint() override;
		int wm_setcursor(WPARAM wParam, LPARAM lParam) override;
		int wm_setfocus(WPARAM wParam) override;
		int wm_size(WPARAM wParam, LPARAM lParam) override;

	private:
		static int s_nSpacing;

		enum { STATUSBARPANELCOUNT = 30 };

		POINT m_ptMouseDown;
		int m_nPosition;

		HWND m_hCheckbox[STATUSBARPANELCOUNT];

		HWND createCheckbox(UINT textid, UINT controlid);
		void createSeperator();

	};
}

