#pragma once

#include "CMD_commands.h"

namespace mycad {
	/**
	* Declaration of dlg_Viewports class.
	*/
	class Viewport;
	class ViewportManager;
	class dlg_Viewports : public CommandDialog {
	public:
		dlg_Viewports(COMMANDINFO* command, DynamicInputWindow* commandwindows[], ViewportManager* viewportmanager, int tabindex, CommandMode mode = CommandMode::BOTH);
		dlg_Viewports(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, ViewportManager* viewportmanager, int tabindex, CommandMode mode = CommandMode::BOTH);
		~dlg_Viewports();
		
		ViewportManager::LPVIEWPORTDATA getViewportData();

	private:
		static ViewportManager::VIEWPORTDATA m_VprtConfigData;

		ViewportManager *m_lpViewportManager;

		HWND m_hOK, m_hCancel, m_hHelp;
		int m_nCurSel;

		static void s_OnListboxNewViewportsInit(HWND hDlg);
		static void s_OnComboboxContextInit(HWND hDlg);
		static void s_OnComboboxSetupInit(HWND hDlg);
		static void s_OnComboboxViewInit(HWND hDlg);
		static void s_OnComboboxVisualStyleInit(HWND hDlg);

		RECT getMaxRect(DLGTEMPLATEEX rcRects[], int size);
		HRESULT setCurSel(HWND htabcontrol);

		static VOID WINAPI s_OnChildDialogInit(HWND hDlg);
		static INT_PTR CALLBACK s_nfnChildDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

		HRESULT wm_initdialog_tabcontrol(HWND hDlg);

		DLGTEMPLATEEX* wm_notify_tcn_selchange_LockDlgRes(LPCTSTR lpszResName);
		void wm_notify_tcn_selchange(HWND hDlg);

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
