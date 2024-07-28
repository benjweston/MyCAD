#pragma once

#include "CMD_commands.h"

namespace mycad {
	/**
	* Declaration of dlg_DrawingUnits class.
	*/
	class dlg_DrawingUnits : public CommandDialog {
	public:
		dlg_DrawingUnits(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		dlg_DrawingUnits(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);
		~dlg_DrawingUnits();

	private:
		bool m_bClockwise;

		HWND m_hcboLengthType,
			m_hcboLengthPrecision,
			m_hcboAngleType,
			m_hcboAnglePrecision,
			m_hbtnClockwise;

		HWND m_hcboInsertionscale;

		HWND m_htxtSampleOutputLength,
			m_htxtSampleOutputAngle;

		HWND m_hcboLightingUnits;

		HWND m_hOK,
			m_hCancel,
			m_hDirection,
			m_hHelp;

		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;
		int wm_erasebkgnd(WPARAM wParam) override;
		int wm_initdialog(WPARAM wParam, LPARAM lParam) override;
		int wm_notify(WPARAM wParam, LPARAM lParam) override;
		int wm_paint() override;
		int wm_syscommand(WPARAM wParam, LPARAM lParam) override;

	};
}
