#pragma once

#include "CMD_commands.h"

namespace mycad {
	/**
	* Declaration of dlg_UCSIcon class.
	*/
	class dlg_UCSIcon : public CommandDialog {
	public:
		dlg_UCSIcon(COMMANDINFO* command, DynamicInputWindow* commandwindows[], int style, unsigned int linewidth, unsigned int size, Colour3<float> modelspaceiconcolour, Colour3<float> layouttabiconcolour, CommandMode mode = CommandMode::BOTH);
		dlg_UCSIcon(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, int style, unsigned int linewidth, unsigned int size, Colour3<float> modelspaceiconcolour, Colour3<float> layouttabiconcolour, CommandMode mode = CommandMode::BOTH);
		~dlg_UCSIcon();

		int getStyle();
		unsigned int getLinewidth();
		unsigned int getSize();
		Colour3<float> getModelspaceIconColour();
		Colour3<float> getLayoutTabIconColour();

	private:
		int m_nStyle;
		unsigned int m_nLinewidth;
		unsigned int m_nSize;
		Colour3<float> m_ModelspaceIconColour;
		Colour3<float> m_LayoutTabIconColour;
		bool m_bTintForXYZ;

		SIZE m_nRange;

		HWND m_hrbtIconStyle2D, m_hrbtIconStyle3D;
		HWND m_hcboLinewidth;
		HWND m_htxtSize, m_htrbUCSIconSize;
		HWND m_hcboModelspaceIconColour, m_hcboLayoutTabIconColour;
		HWND m_hbtnTintForXYZ;
		HWND m_hOK, m_hCancel, m_hHelp;

		void selectColour(HWND hcombobox);

		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;
		int wm_erasebkgnd(WPARAM wParam) override;
		int wm_hscroll(WPARAM wParam, LPARAM lParam) override;
		int wm_initdialog(WPARAM wParam, LPARAM lParam) override;
		int wm_notify(WPARAM wParam, LPARAM lParam) override;
		int wm_paint() override;
		int wm_syscommand(WPARAM wParam, LPARAM lParam) override;
	};
}
