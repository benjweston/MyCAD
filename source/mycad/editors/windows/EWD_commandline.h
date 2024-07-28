#pragma once

#include "EWD_commandlinewindow.h"

namespace mycad {
	/**
	* Declaration of CommandLine class.
	*/
	class EditWindowSubclass;
	class CommandLine : public CommandLineWindow {
	public:
		CommandLine(DynamicInputWindow* dynamicinput, int dropdownlines = 9);//DynamicInputWindow* commandinput is a pointer to the TextWindow instance.
		~CommandLine();

		static BOOL s_nVisible;
		static BOOL s_nDocked;
		static BOOL s_nCheckBeforeClose;

		static INT_PTR CALLBACK s_nfnCommandLineCloseCheckProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

		BOOL setText(const char* text) override;

		int wm_close() override;
		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_contextmenu(WPARAM wParam, LPARAM lParam) override;
		int wm_create(WPARAM wParam, LPARAM lParam) override;
		int wm_initmenupopup(WPARAM wParam, LPARAM lParam) override;
		int wm_killfocus(WPARAM wParam) override;
		int wm_move(LPARAM lParam) override;
		//int wm_ncactivate(WPARAM wParam, LPARAM lParam) override;
		//int wm_nccalcsize(WPARAM wParam, LPARAM lParam) override;
		int wm_nccreate(LPARAM lParam) override;
		int wm_ncmouseleave() override;
		int wm_ncmousemove(WPARAM wParam, LPARAM lParam) override;
		//int wm_ncpaint(WPARAM wParam) override;
		//int wm_setcursor(WPARAM wParam, LPARAM lParam) override;
		int wm_setfocus(WPARAM wParam) override;
		int wm_size(WPARAM wParam, LPARAM lParam) override;
		int wm_syscommand(WPARAM wParam, LPARAM lParam) override;
		int wm_timer(WPARAM wParam, LPARAM lParam) override;

	private:
		EditWindowSubclass* m_lpEditSubclassWindow[2];//m_hHistory uses m_lpEditSubclassWindow[0], m_hCommand uses m_lpEditSubclassWindow[1].

		DWORD m_dwExStyle;
		MouseTrackEvents m_mouseTrack;
		SIZE m_szBorder;
		bool m_bMouseInRect;

		COLORREF m_clrClose;

		void wm_command_change();
		void wm_timer_lowercasetouppercasedelay() override;

		void setDropdownPosition();

		void createOptions();

	};
}
