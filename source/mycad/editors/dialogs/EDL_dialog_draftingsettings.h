#pragma once

#include "EDL_dialog_tab.h"

#include "CMD_commands.h"

namespace mycad {
	/**
	* Declaration of SnapAndGridTab class.
	*/
	class SnapAndGridTab : public dlg_Tab {
	public:
		SnapAndGridTab(HWND htabcontrol);
		~SnapAndGridTab();


	private:


	};



	/**
	* Declaration of PolarTrackingTab class.
	*/
	class PolarTrackingTab : public dlg_Tab {
	public:
		PolarTrackingTab(HWND htabcontrol);
		~PolarTrackingTab();


	private:


	};



	/**
	* Declaration of ObjectSnapTab class.
	*/
	class ObjectSnapTab : public dlg_Tab {
	public:
		ObjectSnapTab(HWND htabcontrol);
		~ObjectSnapTab();


	private:


	};



	/**
	* Declaration of ObjectSnap3DTab class.
	*/
	class ObjectSnap3DTab : public dlg_Tab {
	public:
		ObjectSnap3DTab(HWND htabcontrol);
		~ObjectSnap3DTab();


	private:


	};



	/**
	* Declaration of DynamicInputTab class.
	*/
	class Tooltip;
	class DynamicInputTab : public dlg_Tab {
	public:
		DynamicInputTab(HWND htabcontrol);
		~DynamicInputTab();

		int getDynmode(int index);
		int getDynprompt(int index);
		int getDyninfotips(int index);

		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;
		int wm_initdialog(WPARAM wParam, LPARAM lParam) override;
		int wm_notify(WPARAM wParam, LPARAM lParam) override;
		int wm_size(WPARAM wParam, LPARAM lParam) override;

	private:
		HWND m_hbtnEnablePointerInput,
			m_hbtnPointerInputSettings,
			m_hbtnEnableDimensionInput,
			m_hbtnDimensionInputSettings,
			m_hbtnDraftingTooltipAppearance,
			m_hbtnShowInputNearCrosshairs,
			m_hbtnShowAdditionalTooltips;

		HWND m_hgrbPointerInput,
			m_hgrbDimensionInput,
			m_hgrbDynamicPrompts;

		Tooltip* m_lpTooltip[4];

		int m_nDynmode[2];
		int m_nDynprompt[2];
		int m_nDyninfotips[2];

		void setDynmode();
		void setDynprompt();
		void setDyninfotips();

		void wm_initdialog_getDynprompt();
		void wm_initdialog_getDyninfotips();
		void wm_initdialog_getDynmode();

		int getXMLData();

	};



	/**
	* Declaration of QuickPropertiesTab class.
	*/
	class QuickPropertiesTab : public dlg_Tab {
	public:
		QuickPropertiesTab(HWND htabcontrol);
		~QuickPropertiesTab();


	private:


	};



	/**
	* Declaration of SelectionCyclingTab class.
	*/
	class SelectionCyclingTab : public dlg_Tab {
	public:
		SelectionCyclingTab(HWND htabcontrol);
		~SelectionCyclingTab();


	private:


	};



	/**
	* Declaration of dlg_DraftingSettings class.
	*/
	class DynamicInputWindow;
	class CommandLine;
	class DynamicInput;
	class TextWindow;
	class dlg_Options;
	class dlg_DraftingSettings : public CommandDialog {
	public:
		dlg_DraftingSettings(COMMANDINFO* command, DynamicInputWindow* commandwindows[], CommandMode mode = CommandMode::BOTH);
		dlg_DraftingSettings(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, CommandMode mode = CommandMode::BOTH);
		~dlg_DraftingSettings();

	private:
		HWND m_hTabControl;
		dlg_Tab* m_lpTab;
		RECT m_rcDisplay;
		DLGTEMPLATEEX* m_lpRes;

		HWND m_hOptions, m_hOK, m_hCancel, m_hHelp;

		SnapAndGridTab* m_lpSnapAndGridTab;
		PolarTrackingTab* m_lpPolarTrackingTab;
		ObjectSnapTab* m_lpObjectSnapTab;
		ObjectSnap3DTab* m_lpObjectSnap3DTab;
		DynamicInputTab* m_lpDynamicInputTab;
		QuickPropertiesTab* m_lpQuickPropertiesTab;
		SelectionCyclingTab* m_lpSelectionCyclingTab;

		dlg_Options* m_lpOptions;

		int m_nCurSel;

		static INT_PTR CALLBACK s_nfnChildDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void createOptions();

		HRESULT wm_initdialog_tabcontrol(HWND hDlg);

		DLGTEMPLATEEX* wm_notify_tcn_selchange_LockDlgRes(LPCTSTR lpszResName);
		void wm_notify_tcn_selchange(HWND hDlg);

		void wm_size_tabcontrol();

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
