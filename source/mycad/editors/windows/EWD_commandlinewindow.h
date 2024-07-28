#pragma once

#include "EWD_dynamicinputwindow.h"

namespace mycad {
	/**
	* Declaration of CommandLineWindow class.
	*/
	class CommandLineWindow : public DynamicInputWindow {
	public:
		CommandLineWindow(DynamicInputWindow* dynamicinput, int dropdownlines = 7);
		~CommandLineWindow();

		static int s_nCmdHistLines;

		virtual void setCommandInput(DynamicInputWindow* commandinput);

		void addString(const char* text);
		int getLineCount();

		virtual int wm_close() override;
		//virtual int wm_char(WPARAM wParam, LPARAM lParam);
		virtual int wm_destroy() override;
		virtual int wm_setcursor(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam) override;

	protected:
		DynamicInputWindow* m_lpCommandInput;
		DynamicInputWindow* m_lpDynamicInput;

		HWND m_hHistory;

		COLORREF m_clrBackground_History;
		HBRUSH m_hbrBackground_History;

	};
}
