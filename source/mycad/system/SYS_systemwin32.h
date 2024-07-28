#pragma once

#include "SYS_system.h"

#include "DOC_documentmanager.h"
#include "wm_windowmanager.h"

namespace mycad {
	/**
	Declaration of SystemWin32 class.
	*/
	class Application;
	class SystemWin32 : public System {
	public:
		SystemWin32(HINSTANCE hInstance);
		virtual ~SystemWin32();

		Window* getApplication();

		WindowManager& getWindowManager();
		DocumentManager& getDocumentManager();

		virtual int processEvents() override;

		int registerFrameClass(
			const char* lpClassName,
			HICON hIcon,
			COLORREF hbrBackground = GetSysColor(COLOR_WINDOW + 1),
			HCURSOR hCursor = ::LoadCursor(NULL, IDC_ARROW),
			const char* lpMenuName = NULL,
			unsigned int nStyle = NULL
		);
		int registerMDIChildClass(
			const char* lpClassName,
			HICON hIcon,
			COLORREF hbrBackground = GetSysColor(COLOR_WINDOW + 1),
			HCURSOR hCursor = ::LoadCursor(NULL, IDC_ARROW),
			const char* lpMenuName = NULL,
			unsigned int nStyle = NULL
		);
		int registerClass(
			const char* lpClassName,
			HICON hIcon,
			COLORREF hbrBackground = GetSysColor(COLOR_WINDOW + 1),
			HCURSOR hCursor = ::LoadCursor(NULL, IDC_ARROW),
			const char* lpMenuName = NULL,
			unsigned int nStyle = NULL
		);
		int registerClass(const char* lpClassname);

		int createWindow(
			const char* lpClassname,
			const char* lpWindowName,
			Window* lpWindow,
			unsigned long dwExStyle = WS_EX_CLIENTEDGE,
			unsigned long dwStyle = WS_OVERLAPPEDWINDOW,
			int X = CW_USEDEFAULT,
			int Y = CW_USEDEFAULT,
			int nWidth = CW_USEDEFAULT,
			int nHeight = CW_USEDEFAULT,
			HWND hWndParent = NULL,
			HMENU hMenu = 0,
			int nCmdShow = SW_NORMAL
		);
		/**
		Creates a window with DialogBoxParam.
		*/
		int createDialog(
			Window* lpWindow,
			UINT nDialogId,
			HWND hWndParent = NULL
		);

		int getMonitors();
		BOOL getMonitor(unsigned int index, MONITORINFOEX* mi);

	protected:
		enum { MAX_LOADSTRING = 256 };

		HINSTANCE m_hInstance;

		Application* m_lpApplication;

		WindowManager m_WindowManager;
		DocumentManager m_DocumentManager;

		virtual void registerClasses();
		virtual void registerApplicationClasses(HINSTANCE) {}
		virtual void registerDialogClasses();
		virtual void registerToolPaletteClasses(HINSTANCE hModule);
		virtual void registerCommandClasses(HINSTANCE hModule);
		virtual void registerMiscellaneousClasses();

		BOOL Is_WinXP_SP2_or_Later();
		BOOL Is_Win_Server();

		virtual BOOL createApplication();

		static std::vector<MONITORINFOEX> s_miMonitors;
		static BOOL CALLBACK s_bfnMonitorEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT lprc, LPARAM dwData);

		static LRESULT WINAPI s_lpfnFrameProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT WINAPI s_lpfnMDIChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT WINAPI s_lpfnWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		static INT_PTR CALLBACK s_npDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	};
}
