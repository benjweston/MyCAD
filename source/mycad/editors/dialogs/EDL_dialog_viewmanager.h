#pragma once

#include "CMD_commands.h"

namespace mycad {
	/**
	* Declaration of dlg_ViewManager class.
	*/
	class View;
	class ViewManager;
	class dlg_ViewManager : public CommandDialog {
	public:
		dlg_ViewManager(COMMANDINFO* command, DynamicInputWindow* commandwindows[], ViewManager* viewmanager, SIZE viewport, CommandMode mode = CommandMode::BOTH);
		dlg_ViewManager(COMMANDINFO* command, DynamicInputWindow* commandwindows[], const char* text, ViewManager* viewmanager, SIZE viewport, CommandMode mode = CommandMode::BOTH);
		~dlg_ViewManager();

	private:
		ViewManager *m_lpViewManager;
		int m_nWidth;
		int m_nHeight;

		View* m_lpSelectedView;
		bool m_bSelectedView;

		HWND m_hCurrentView, m_hViews;
		HWND m_hTrvw, m_hLstvw;
		HWND m_hOK, m_hCancel, m_hApply, m_hHelp;
		HWND m_hSetCurrent, m_hNew, m_hUpdateLayers, m_hEditBoundaries, m_hDelete;

		float m_fDivider;

		void wm_initdialog_treeview_imagelist();
		void wm_initdialog_treeview();

		void wm_size();

		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;
		int wm_erasebkgnd(WPARAM wParam) override;
		int wm_getminmaxinfo(LPARAM lParam) override;
		int wm_initdialog(WPARAM wParam, LPARAM lParam) override;
		int wm_move(LPARAM lParam) override;
		int wm_notify(WPARAM wParam, LPARAM lParam) override;
		int wm_paint() override;
		int wm_showwindow(WPARAM wParam, LPARAM lParam);
		int wm_size(WPARAM wParam, LPARAM lParam) override;
		int wm_syscommand(WPARAM wParam, LPARAM lParam) override;

	};
}
