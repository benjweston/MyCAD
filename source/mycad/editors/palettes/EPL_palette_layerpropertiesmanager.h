#pragma once

#include "CMD_commands.h"

namespace mycad {
	/**
	* Declaration of plt_LayerPropertiesManager class.
	*/
	class plt_LayerPropertiesManager : public WindowWin32 {
	public:
		plt_LayerPropertiesManager();
		~plt_LayerPropertiesManager();

		static BOOL s_nVisible;
		static BOOL s_nDocked;
		static POINT s_ptPosition;
		static SIZE s_szSize;

	private:
		HWND m_hTrvw;
		HWND m_hLstvw;

		int m_nTreeviewWidth;
		int m_nListviewAWidth;
		int m_nToolbarHeight;
		int m_nStatusbarHeight;

		BOOL InitListViewColumns();
		BOOL InsertListViewItems(int cItems);

		int wm_close() override;
		int wm_create(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;
		int wm_erasebkgnd(WPARAM wParam) override;
		int wm_notify(WPARAM wParam, LPARAM lParam) override;
		int wm_paint() override;
		int wm_setcursor(WPARAM wParam, LPARAM lParam) override;
		int wm_size(WPARAM wParam, LPARAM lParam) override;
		int wm_syscommand(WPARAM wParam, LPARAM lParam) override;

	};
}
