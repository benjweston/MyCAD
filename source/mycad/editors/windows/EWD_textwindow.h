#pragma once

#include "EWD_commandlinewindow.h"

namespace mycad {
	/**
	* Declaration of TextWindow class.
	*/
	class EditWindowSubclass;
	class TextWindow : public CommandLineWindow {
	public:
		TextWindow(DynamicInputWindow* dynamicinput, int dropdownlines = 7);//DynamicInputWindow* commandinput is a pointer to the CommandLine instance.
		~TextWindow();

		static BOOL s_nVisible;
		static BOOL s_nDocked;

		BOOL setStaticText(const char* text);

		int wm_activate(WPARAM wParam, LPARAM lParam) override;
		int wm_close() override;
		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_contextmenu(WPARAM wParam, LPARAM lParam) override;
		int wm_create(WPARAM wParam, LPARAM lParam) override;
		int wm_initmenupopup(WPARAM wParam, LPARAM lParam) override;
		int wm_killfocus(WPARAM wParam) override;
		int wm_move(LPARAM lParam) override;
		int wm_setfocus(WPARAM wParam) override;
		int wm_size(WPARAM wParam, LPARAM lParam) override;
		int wm_syscommand(WPARAM wParam, LPARAM lParam) override;
		int wm_timer(WPARAM wParam, LPARAM lParam) override;

	private:
		EditWindowSubclass* m_lpEditSubclassWindow[2];//m_hHistory uses m_lpEditSubclassWindow[0], m_hCommand uses m_lpEditSubclassWindow[1].

		HWND m_hStatic;
		char m_szStatic[MAX_LOADSTRING];

		void wm_command_change();
		void wm_timer_lowercasetouppercasedelay() override;

		void setDropdownPosition();

		void createOptions();

	};
}