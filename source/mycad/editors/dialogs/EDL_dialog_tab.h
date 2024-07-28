#pragma once

#include "wm_windowwin32.h"

namespace mycad {
	/**
	* Declaration of dlg_Tab class.
	*/
	class dlg_Tab : public WindowWin32 {
	public:
		dlg_Tab();
		dlg_Tab(HWND htabcontrol);
		virtual ~dlg_Tab();

		virtual void setRegistryData();

		virtual int setXMLData();

		int wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam) override;
		int wm_erasebkgnd(WPARAM wParam) override;
		int wm_paint() override;

	protected:
		HWND m_hTabControl;//Tabcontrol handle.

		virtual void getRegistryData();

		virtual int getXMLData();

	};
}
