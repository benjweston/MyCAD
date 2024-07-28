#pragma once

#include "EDL_dialog_tab.h"

#include "CMD_commands.h"

namespace mycad {
	/**
	* Declaration of FilesTab class.
	*/
	class FilesTab : public dlg_Tab {
	public:
		FilesTab(HWND htabcontrol);
		~FilesTab();


	private:


	};



	/**
	* Declaration of DisplayTab class.
	*/
	class DisplayTab : public dlg_Tab {
	public:
		DisplayTab(HWND htabcontrol);
		~DisplayTab();

		void setRegistryData() override;

		int setXMLData() override;

	private:
		HWND m_htxtColourTheme;

		HWND m_hbtnDisplayscrollbarsindrawingwindow,
			m_hbtnUselargebuttonsforToolbars,
			m_hbtnResizeribboniconstostandardsizes,
			m_hbtnShowToolTips,
			m_hedtNumberofsecondsbeforedisplay,
			m_hbtnShowshortcutkeysinToolTips,
			m_hbtnShowextendedToolTips;

		void getRegistryData() override;

		int getXMLData() override;

		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_initdialog(WPARAM wParam, LPARAM lParam) override;
		int wm_notify(WPARAM wParam, LPARAM lParam) override;
		int wm_size(WPARAM wParam, LPARAM lParam) override;

	};



	/**
	* Declaration of OpenAndSaveTab class.
	*/
	class OpenAndSaveTab : public dlg_Tab {
	public:
		OpenAndSaveTab(HWND htabcontrol);
		~OpenAndSaveTab();


	private:


	};



	/**
	* Declaration of PlotAndPublishTab class.
	*/
	class PlotAndPublishTab : public dlg_Tab {
	public:
		PlotAndPublishTab(HWND htabcontrol);
		~PlotAndPublishTab();


	private:


	};



	/**
	* Declaration of SystemTab class.
	*/
	class SystemTab : public dlg_Tab {
	public:
		SystemTab(HWND htabcontrol);
		~SystemTab();


	private:


	};



	/**
	* Declaration of UserPreferencesTab class.
	*/
	class UserPreferencesTab : public dlg_Tab {
	public:
		UserPreferencesTab(HWND htabcontrol);
		~UserPreferencesTab();


	private:


	};



	/**
	* Declaration of DraftingTab class.
	*/
	class dlg_DrawingWindowColours;
	class DraftingTab : public dlg_Tab {
	public:
		DraftingTab(HWND htabcontrol);
		~DraftingTab();


	private:
		HWND m_hAutoSnapSettings,
			m_hAutoTrackSettings,
			m_hAlignmentPointAcquisition,
			m_hAutoSnapMarkerSize,
			m_hApertureSize,
			m_hObjectSnapOptions;

		HWND m_hbtnMarker,
			m_hbtnMagnet,
			m_hbtnDisplayAutoSnaptooltip,
			m_hbtnDisplayAutoSnapaperturebox,
			m_hbtnColours;

		HWND m_hbtnDisplaypolartrackingvector,
			m_hbtnDisplayfull_screentrackingvector,
			m_hbtnDisplayAutoTrackingtooltip,
			m_hbtnAutomaticPointAquisitionAutomatic,
			m_hbtnAutomaticPointAquisitionShiftToAcquire;

		//unsigned int m_nMarkerSize, m_nMarkerSizeMin, m_nMarkerSizeMax;
		//unsigned int m_nApertureSize, m_nApertureSizeMin, m_nApertureSizeMax;

		HWND m_hctlAutoSnapMarkerSize,
			m_htrbAutoSnapMarkerSize,
			m_hctlApertureSize,
			m_htrbApertureSize;

		HWND m_hbtnIgnorehatchobjects,
			m_hbtnIgnoredimensionextensionlines,
			m_hbtnIgnorenegativeZobjectsnapsforDynamicUCS,
			m_hbtnReplaceZvaluewithcurrentelevation;

		HWND m_hbtnDraftingTooltipSettings,
			m_hbtnLightsGlyphSettings,
			m_hbtnCamerasGlyphSettings;

		dlg_DrawingWindowColours* m_lpDrawingWindowColours;

		void createDrawingWindowColours();

		int getXMLData() override;

		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_initdialog(WPARAM wParam, LPARAM lParam) override;
		int wm_notify(WPARAM wParam, LPARAM lParam) override;
		int wm_size(WPARAM wParam, LPARAM lParam) override;

	};



	/**
	* Declaration of ThreeDModelingTab class.
	*/
	class ThreeDModelingTab : public dlg_Tab {
	public:
		ThreeDModelingTab(HWND htabcontrol);
		~ThreeDModelingTab();


	private:


	};



	/**
	* Declaration of SelectionTab class.
	*/
	class SelectionTab : public dlg_Tab {
	public:
		SelectionTab(HWND htabcontrol);
		~SelectionTab();


	private:


	};



	/**
	* Declaration of ProfilesTab class.
	*/
	class ProfilesTab : public dlg_Tab {
	public:
		ProfilesTab(HWND htabcontrol);
		~ProfilesTab();


	private:


	};



	/**
	* Declaration of dlg_Options class.
	*/
	class dlg_Options : public CommandDialog {
	public:
		dlg_Options(COMMANDINFO* command, DynamicInputWindow* commandwindows[], int tabindex = -1, CommandMode mode = CommandMode::BOTH);
		dlg_Options(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, int tabindex = -1, CommandMode mode = CommandMode::BOTH);
		virtual ~dlg_Options();

	private:
		HWND m_hTabControl;
		dlg_Tab* m_lpTab;
		RECT m_rcDisplay;
		DLGTEMPLATEEX* m_lpRes;

		HWND m_hCurrentProfile, m_hCurrentDrawing;
		HWND m_hOK, m_hCancel, m_hApply, m_hHelp;

		int m_nCurSel[2];

		static INT_PTR CALLBACK s_nfnChildDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

		HRESULT wm_initdialog_tabcontrol(HWND hDlg);

		DLGTEMPLATEEX* wm_notify_tcn_selchange_LockDlgRes(LPCTSTR lpszResName);
		void wm_notify_tcn_selchange(HWND hDlg);

		void wm_size_tabcontrol();
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
