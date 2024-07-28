#pragma once

#include "CMD_commands.h"

namespace mycad {
	/**
	* Declaration of dlg_UCS class.
	*/
	class dlg_UCS : public CommandDialog {
	public:
		dlg_UCS(COMMANDINFO* command, DynamicInputWindow* commandwindows[], int tabindex, CommandMode mode = CommandMode::BOTH);
		dlg_UCS(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, int tabindex, CommandMode mode = CommandMode::BOTH);
		~dlg_UCS();

	private:
		HWND m_hOK, m_hCancel, m_hHelp;
		int m_nCurSel;

		static VOID WINAPI s_OnChildDialogInit(HWND hDlg);
		static INT_PTR CALLBACK s_nfnChildDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

		RECT getMaxRect(DLGTEMPLATEEX rcRects[], int size);
		HRESULT setCurSel(HWND htabcontrol);

		HRESULT wm_initdialog_tabcontrol(HWND hDlg);

		DLGTEMPLATEEX * wm_notify_tcn_selchange_LockDlgRes(LPCTSTR lpszResName);
		void wm_notify_tcn_selchange(HWND hDlg);

		void wm_size();

		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;
		int wm_erasebkgnd(WPARAM wParam) override;
		int wm_getminmaxinfo(LPARAM lParam) override;
		int wm_initdialog(WPARAM wParam, LPARAM lParam) override;
		int wm_notify(WPARAM wParam, LPARAM lParam) override;
		int wm_paint() override;
		int wm_size(WPARAM wParam, LPARAM lParam) override;
		int wm_syscommand(WPARAM wParam, LPARAM lParam) override;

	};
}
