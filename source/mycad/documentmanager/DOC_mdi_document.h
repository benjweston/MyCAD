#pragma once

#include "DOC_document.h"

namespace mycad {
	/**
	Declaration of MDIDocument class.
	*/
	class MDIDocument : public Document {
	public:
		MDIDocument(HWND hMDIClient, DynamicInputWindow* commandwindows[], const char* filename, int tabindex = 0);
		~MDIDocument();

		int wm_create(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;
		int wm_mdiactivate(WPARAM wParam, LPARAM lParam) override;
		int wm_settext(WPARAM wParam, LPARAM lParam) override;
		int wm_syscommand(WPARAM wParam, LPARAM lParam) override;

	private:
		HWND m_hMDIClient;

		void setApplicationTitle(INT nState) override;
		void setApplicationMenu(BOOL nState) override;

	};
}
