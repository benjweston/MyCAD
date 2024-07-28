#pragma once

#include "wm_windowwin32.h"

namespace mycad {
	/**
	* Declaration of System class.
	*/
	class Popupmenu : public WindowWin32 {
	public:
		Popupmenu();//Private constructor to prevent more than one instantiation of class.
		~Popupmenu();

		int wm_char(WPARAM wParam, LPARAM lParam) override;
		int wm_close() override;
		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_create(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;
		int wm_exitsizemove() override;
		int wm_initmenupopup(WPARAM wParam, LPARAM lParam) override;
		int wm_keydown(WPARAM wParam, LPARAM lParam) override;
		int wm_mousemove(WPARAM wParam, LPARAM lParam) override;
		int wm_notify(WPARAM wParam, LPARAM lParam) override;
		int wm_paint() override;
		int wm_size(WPARAM wParam, LPARAM lParam) override;

	private:


	};
}
