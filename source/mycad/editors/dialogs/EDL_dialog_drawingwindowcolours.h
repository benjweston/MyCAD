#pragma once

#include "CMD_commands.h"

namespace mycad {
	/**
	* Declaration of dlg_DrawingWindowColours class.
	*/
	class dlg_DrawingWindowColours : public CommandDialog {
	public:
		dlg_DrawingWindowColours();
		~dlg_DrawingWindowColours();

	private:
		HWND m_hContext, m_hInterfaceElement, m_hColour;
		HWND m_hOK, m_hCancel, m_hHelp;

		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;
		int wm_erasebkgnd(WPARAM wParam) override;
		int wm_initdialog(WPARAM wParam, LPARAM lParam) override;
		int wm_notify(WPARAM wParam, LPARAM lParam) override;
		int wm_paint() override;
		int wm_size(WPARAM wParam, LPARAM lParam) override;
		int wm_syscommand(WPARAM wParam, LPARAM lParam) override;

	};
}
