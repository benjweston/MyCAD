#pragma once

#include "wm_windowmanager.h"

#include "DOC_document.h"
#include "DOC_documentmanager.h"

#include "DCK_pane.h"
#include "DCK_dockingmanager.h"

/**
Application defined Project Properties
These macros are defined in project properties; Properties\Configuration Properties\C / C++\Preprocessor\Preprocessor Definitions\<Edit...>
#define PROJDIR = "$(ProjectDir)."
#define SOLDIR = "$(SolutionDir)."
*/

namespace mycad {
	/**
	Declaration of Application class.
	*/
	class TextWindow;
	class CommandLine;
	class DynamicInput;
	class Tooltip;
	class Statusbar;
	class clsXMLNode;
	class Application : public WindowWin32 {
	public:
		/**
		Constructor.
		Creates a new Application window.
		\param windowmanager: pointer to a WindowManager instance.
		\param documentmanager: pointer to a DocumentManager instance.
		*/
		Application(WindowManager* windowmanager, DocumentManager* documentmanager);

		/**
		Destructor.
		Closes the window and disposes resources allocated.
		*/
		virtual ~Application();

		/**
		Get the application window state from the registry (Function is public because it is called from SystemWin32 class instance).
		\param idFrom: window id.
		\param hwndFrom: window handle.
		\param state: window visibility ( SW_NORMAL, SW_SHOWMAXIMIZED, etc... ), the retrieved value is used in the CreateWindow() function.
		*/
		void getApplicationState(RECT& position, int& visibility);

		//Public window functions.
		virtual int wm_activate(WPARAM, LPARAM) override;
		virtual int wm_char(WPARAM wParam, LPARAM lParam);
		virtual int wm_close() override;
		virtual int wm_command(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		virtual int wm_destroy() override;
		virtual int wm_erasebkgnd(WPARAM wParam) override;
		virtual int wm_exitmenuloop(WPARAM wParam) override;
		virtual int wm_exitsizemove() override;
		virtual int wm_getminmaxinfo(LPARAM lParam) override;
		virtual int wm_initmenu(WPARAM wParam);
		virtual int wm_initmenupopup(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_keydown(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_move(LPARAM lParam) override;
		virtual int wm_notify(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_paint() override;
		virtual int wm_parentnotify(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_setcursor(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_setfocus(WPARAM wParam) override;
		virtual int wm_syscommand(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_uninitmenupopup(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_windowposchanged(LPARAM lParam);
		virtual int wm_windowposchanging(LPARAM lParam);

	private:
		

	protected:
		static BOOL s_nNewDocumentOnStartup;
		static int s_nTabconctrol_Documents_Height;

		WindowManager* m_lpWindowManager;
		DocumentManager* m_lpDocumentManager;

		const char* m_szFilter;

		enum { STATUSBARPANELCOUNT = 31 };
		Statusbar* m_lpStatusbar;

		TextWindow* m_lpTextWindow;
		CommandLine* m_lpCommandLine;
		DynamicInput* m_lpDynamicInput;

		DynamicInputWindow* m_lpCommandWindows[3];

		HWND m_hToolbar_File;
		HWND m_hToolbar_Draw;
		HWND m_hRebar_Standard;
		HWND m_hTabcontrol_Documents;
		HWND m_hTabcontrol_Documents_Tooltip;

		POINT m_ptDocument;
		SIZE m_szDocument;

		unsigned int m_nMinFrameDuration;
		unsigned int m_nDocumentCounter;
		bool m_bThreadSuspended;

		void setApplicationState();

		void setSystemVariable(int variableid, int value);
		void getSystemVariables();

		int gsmGetScreenSize();

		void spiGetMouseHoverSize(UINT cx, UINT cy);

		void calculateWindowPosition(HWND hwnd, int x, int y, int cx, int cy, POINT* lppt);

		virtual void createDocument(const char* filename);
		virtual void createDocument(const char* filename, DWORD style, DWORD exstyle);

		void createTextWindow();
		void createCommandLine();
		void createDynamicInput();

		void createLayerPropertiesManager();

		void statusbar_Menu_Coordinates();
		void statusbar_Menu_Grid();
		void statusbar_Menu_SnapMode();
		void statusbar_Menu_DynamicInput();
		void statusbar_Menu_Customization_Init(HMENU hMenu);
		void statusbar_Menu_Customization();

		void wm_command_File_New();
		void wm_command_File_Open();
		void wm_command_File_Import();
		void wm_command_File_Close();

		void wm_command_View_Display_UCSIcon_On();
		void wm_command_View_Display_UCSIcon_Position();

		void wm_command_Insert_NewLayout();

		virtual void wm_command_Window_Close();
		virtual void wm_command_Window_CloseAll();
		virtual void wm_command_Window_Cascade();
		virtual void wm_command_Window_TileHorizontally();
		virtual void wm_command_Window_TileVertically();
		virtual void wm_command_Window_ArrangeIcons();
		virtual void wm_command_Window_Windows();

		virtual int wm_command_Default(UINT uMsg, WPARAM wParam, LPARAM lParam);

		void wm_create_Menu();
		void wm_create_toolbar_standard();
		void wm_create_toolbar_draw();
		void wm_create_rebar_standard();
		void wm_create_tabcontrol_documents();
		void wm_create_statusbar_tooltips();
		void wm_create_statusbar();

		void wm_exitsizemove_statusbar_tooltips();

		void wm_initmenupopup_View(HMENU hmenu);

		void wm_notify_nm_click_toolbar_File(DWORD_PTR dwItemSpec);
		void wm_notify_nm_click_toolbar_Draw(DWORD_PTR dwItemSpec);

		void wm_notify_nm_click_statusbar(DWORD_PTR dwItemSpec);
		void wm_notify_nm_click(UINT_PTR idFrom, DWORD_PTR dwItemSpec);

		void wm_notify_nm_rclick_statusbar(DWORD_PTR dwItemSpec);
		void wm_notify_nm_rclick(UINT_PTR idFrom, DWORD_PTR dwItemSpec);

		void wm_notify_rbn_heightchange();
		void wm_notify_tbn_dropdown(HWND hwndFrom, int iItem);
		
		void wm_notify_ttn_getdispinfo_File(UINT_PTR idFrom, LPSTR& lpszText);
		void wm_notify_ttn_getdispinfo_Draw(UINT_PTR idFrom, LPSTR& lpszText);
		void wm_notify_ttn_getdispinfo_Documents(UINT_PTR idFrom, LPSTR& lpszText);
		void wm_notify_ttn_getdispinfo(NMHDR nmHDR, LPSTR& lpszText);

		virtual void wm_notify_tcn_selchange_documents();
		virtual void wm_notify_tcn_selchange_documents_callback();
		void wm_notify_tcn_selchange(UINT_PTR idFrom);

		virtual int wm_notify_tcn_selchanging_documents();
		virtual int wm_notify_tcn_selchanging_documents_callback();
		int wm_notify_tcn_selchanging(UINT_PTR idFrom);

		void wm_parentnotify_create(UINT_PTR id, HWND hwnd);
		void wm_parentnotify_destroy(UINT_PTR id, HWND hwnd);

		void wm_size_rebar();
		void wm_size_documents();
		void wm_size_statusbar();
		virtual void wm_size();

		void show_statusbar();
		void show_statusbar(const char* filename);

		//Static dialog procedures
		static INT_PTR CALLBACK s_nfnHelpProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static INT_PTR CALLBACK s_nfnAboutProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static INT_PTR CALLBACK s_nfnWindowsProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

		static BOOL CALLBACK s_bfnEnumWindowsProc(HWND hWnd, LPARAM lParam);
		static BOOL CALLBACK s_bfnEnumChildWindowsProc(HWND hWnd, LPARAM lParam);

		//XML data functions
		int xmlGetModelspace2D(LONG_PTR element);
		int xmlGetSheetLayout(LONG_PTR element);
		int xmlGetParallelProjection3D(LONG_PTR element);
		int xmlGetPerspectiveProjection3D(LONG_PTR element);
		int xmlGetBlockEditor(LONG_PTR element);
		int xmlGetCommandLine(LONG_PTR element);
		int xmlGetPlotPreview(LONG_PTR element);

		int xmlGetDisplay(LONG_PTR root);
		int xmlGetColours(LONG_PTR root);

		int xmlGetOptions();

		int xmlGetUCSIconProperties();

		HRESULT xmlGetCommand_Prompt(LONG_PTR lpNode, COMMANDINFO* lpCmdInfo);
		int xmlGetCommand_Advanced(LONG_PTR node, COMMANDINFO* lpCmdInfo);
		int xmlGetCommand_Command(LONG_PTR node, COMMANDINFO* lpCmdInfo);

		/**
		* Loop through child nodes to retrieve command attributes.
		* \param LONG_PTR node: The current node.
		*/
		int xmlGetCommand(LONG_PTR node);

		/**
		* Loop through XML child nodes in root node.
		*/
		int xmlGetCommands();

	};
}