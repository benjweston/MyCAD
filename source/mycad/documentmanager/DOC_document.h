#pragma once

#include "wm_windowwin32.h"

#include "CMD_commandmanager.h"

#include "DOC_documentmanager.h"

#include "DRW_layer.h"
#include "DRW_layermanager.h"
#include "DRW_layout.h"
#include "DRW_layoutmanager.h"

#include "io.h"

#include <thread>

namespace mycad {
	/**
	Declaration of Document class.
	*/
	class DynamicInputWindow;
	class CommandManager;
	class GLRenderContext;
	class GLRenderThread;
	class GLRenderWindow;
	class ViewManager;
	class Document : public WindowWin32 {
	public:
		Document(DynamicInputWindow* commandwindows[], const char* filename, int tabindex = 0);
		virtual ~Document();

		void setFileName(const char* filename);
		char* getFileName();

		void setTabIndex(int tabindex);
		int getTabIndex();

		char* getWindowName();

		Window* getRenderWindow();

		LayerManager *getLayerManager();
		LayoutManager* getLayoutManager();
		Layout* getActiveLayout();
		ViewManager* getViewManager();
		ViewportManager *getViewports();
		Command* getActiveCommand();
		CommandManager* getCommandManager();

		BOOL setStatusbarText(HWND hstatusbar, int index = -1);
		BOOL setDynamicInputStyle();

		//Public window functions.
		virtual int wm_activate(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_char(WPARAM wParam, LPARAM lParam);
		virtual int wm_close() override;
		virtual int wm_command(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_create(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_destroy() override;
		virtual int wm_exitsizemove() override;
		virtual int wm_getminmaxinfo(LPARAM lParam) override;
		virtual int wm_hscroll(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_keydown(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_killfocus(WPARAM wParam) override;
		virtual int wm_lbuttondblclk(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_lbuttondown(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_lbuttonup(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_mouseleave() override;
		virtual int wm_mousemove(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_mousewheel(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_move(LPARAM lParam) override;
		virtual int wm_notify(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_setcursor(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_setfocus(WPARAM wParam) override;
		virtual int wm_syscommand(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_timer(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_vscroll(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_windowposchanged(LPARAM lParam);
		virtual int wm_windowposchanging(LPARAM lParam);

	protected://Private overrides of public window functions.
		void wm_size();

	protected:
		DynamicInputWindow* m_lpCommandWindows[3];//Command windows 0, 1 & 2 are TextWindow, CommandLine & DynamicInput.
		char m_szFileName[MAX_LOADSTRING];
		int nTabIndex;

		char m_szApplicationName[MAX_LOADSTRING];
		char m_szWindowName[MAX_LOADSTRING];

		CommandManager m_CommandManager;
		GLRenderWindow* m_lpGLRenderWindow;

		HWND m_hTabcontrol_Layouts;
		HWND m_hTabcontrol_Layouts_Tooltip;

		int m_nTabconctrol_Layouts_Height;

		int m_nHScroll;
		int m_nVScroll;

		SCROLLINFO m_siHorz;
		SCROLLINFO m_siVert;

		MouseTrackEvents m_mouseTrack;

		POINT m_ptMouseDown;
		POINT m_ptMouseUp;
		POINT m_ptMouseMove;
		bool m_bMouseMoved;

		bool m_bMenuLoopActive;
		bool m_bShortcutMenuLoopActive;
		bool m_bLbuttonDblclk;

		int m_nCharWidth;
		size_t m_cch;
		int m_nCurChar;
		char* m_pchInputBuf;
		char m_ch;

		virtual void setApplicationTitle(INT nState);
		virtual void setApplicationMenu(BOOL nState);

		BOOL showCommandline(WPARAM wParam, LPARAM lParam);
		BOOL showDynamicinput(WPARAM wParam, LPARAM lParam);

		void setTextWindowTitle();

		POINT WinToGL(POINT pt);

		void clearCommandTextBuffer();

		int createCommandWindow(Command* lpCommand, CommandContext nContext = CommandContext::COMMAND);
		int createCommandDialog(Command* lpCommand, UINT nDialogId);

		void exitCommand();

		BOOL wm_char_view(WORD code);
		void wm_char_back();
		void wm_char_return();
		void wm_char_escape();
		void wm_char_end();
		void wm_char_left();
		void wm_char_right();
		void wm_char_default(WPARAM wParam, LPARAM lParam);

		void wm_command_sendcommand(WPARAM wParam, LPARAM lParam);
		void wm_command_sendcommand_text(WPARAM wParam, LPARAM lParam);
		void wm_command_sendcommand_param(WPARAM wParam, LPARAM lParam);

		void wm_create_Menu();

		void wm_notify_ttn_getdispinfo_Layouts(UINT_PTR idFrom, LPSTR& lpszText);
		void wm_notify_ttn_getdispinfo(NMHDR nmHDR, LPSTR& lpszText);

		void wm_notify_tcn_selchange_layouts();
		void wm_notify_tcn_selchange_document_callback();
		void wm_notify_tcn_selchange_layout_callback();
		void wm_notify_tcn_selchange(UINT_PTR idFrom);

		int wm_notify_tcn_selchanging_layouts();
		int wm_notify_tcn_selchanging_layout_callback();
		int wm_notify_tcn_selchanging(UINT_PTR idFrom);

	};
}
