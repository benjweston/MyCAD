#pragma once

#include "wm_iwindow.h"

namespace mycad {
	/**
	* Declaration of Window class.
	*/
	class SystemWin32;
	class Window : public IWindow {
	public:
		/**
		* Constructor.
		* Creates a new window.
		* To check if the window was created properly, use the #getValid() method.
		*/
		Window();
		Window(HINSTANCE hInstance);
		/**
		* Destructor.
		* Closes the window and disposes resources allocated.
		*/
		virtual ~Window();

		Window* getWindow();

		BOOL getActive() const override;

		BOOL getValid() const override;

		void setHWND(HWND hwnd);
		HWND getHWND() const override;

		void setMenu(HMENU hMenu);
		HMENU getMenu() const;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       

		void setFont(HWND hWnd);
		void setFont(HWND hWnd, const char* text, bool italic = false);

		void setSystem(SystemWin32* system);
		virtual SystemWin32* getSystem() const;

		virtual int wm_activate(WPARAM wParam, LPARAM lParam);
		virtual int wm_char(WPARAM wParam, LPARAM lParam);
		virtual int wm_childactivate();
		virtual int wm_close();
		virtual int wm_command(WPARAM wParam, LPARAM lParam);
		virtual int wm_command(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual int wm_contextmenu(WPARAM wParam, LPARAM lParam);
		virtual int wm_create(WPARAM wParam, LPARAM lParam);
		virtual int wm_ctlcolordlg(WPARAM wParam, LPARAM lParam);
		virtual int wm_ctlcoloredit(WPARAM wParam, LPARAM lParam);
		virtual int wm_ctlcolorlistbox(WPARAM wParam, LPARAM lParam);
		virtual int wm_ctlcolorstatic(WPARAM wParam, LPARAM lParam);
		virtual int wm_destroy();
		virtual int wm_dpichanged(WPARAM wParam, LPARAM lParam);
		virtual int wm_enable(WPARAM wParam, LPARAM lParam);
		virtual int wm_entermenuloop(WPARAM wParam);
		virtual int wm_entersizemove();
		virtual int wm_erasebkgnd(WPARAM wParam);//Erase background = 0, don't erase background != 0
		virtual int wm_exitmenuloop(WPARAM wParam);
		virtual int wm_exitsizemove();
		virtual int wm_getminmaxinfo(LPARAM lParam);
		virtual int wm_hscroll(WPARAM wParam, LPARAM lParam);
		virtual int wm_initdialog(WPARAM wParam, LPARAM lParam);
		virtual int wm_initmenu(WPARAM wParam);
		virtual int wm_initmenupopup(WPARAM wParam, LPARAM lParam);
		virtual int wm_keydown(WPARAM wParam, LPARAM lParam);
		virtual int wm_keyup(WPARAM wParam, LPARAM lParam);
		virtual int wm_killfocus(WPARAM wParam);
		virtual int wm_lbuttondblclk(WPARAM wParam, LPARAM lParam);
		virtual int wm_lbuttondown(WPARAM wParam, LPARAM lParam);
		virtual int wm_lbuttonup(WPARAM wParam, LPARAM lParam);
		virtual int wm_mbuttondown(WPARAM wParam, LPARAM lParam);
		virtual int wm_mbuttonup(WPARAM wParam, LPARAM lParam);
		virtual int wm_mdiactivate(WPARAM wParam, LPARAM lParam);
		virtual int wm_menuchar(WPARAM wParam, LPARAM lParam);
		virtual int wm_menucommand(WPARAM wParam, LPARAM lParam);
		virtual int wm_move(LPARAM lParam);
		virtual int wm_moving(LPARAM lParam);
		virtual int wm_mousehover(WPARAM wParam, LPARAM lParam);
		virtual int wm_mouseleave();
		virtual int wm_mousemove(WPARAM wParam, LPARAM lParam);
		virtual int wm_mousewheel(WPARAM wParam, LPARAM lParam);
		virtual int wm_ncactivate(WPARAM wParam, LPARAM lParam);
		virtual int wm_nccalcsize(WPARAM wParam, LPARAM lParam);
		virtual int wm_nccreate(LPARAM lParam);
		virtual int wm_ncmousehover(WPARAM wParam, LPARAM lParam);
		virtual int wm_ncmouseleave();
		virtual int wm_ncmousemove(WPARAM wParam, LPARAM lParam);
		virtual int wm_ncpaint(WPARAM wParam);
		virtual int wm_notify(WPARAM wParam, LPARAM lParam);
		virtual int wm_paint();
		virtual int wm_parentnotify(WPARAM wParam, LPARAM lParam);
		virtual int wm_rbuttondown(WPARAM wParam, LPARAM lParam);
		virtual int wm_rbuttonup(WPARAM wParam, LPARAM lParam);
		virtual int wm_setcursor(WPARAM wParam, LPARAM lParam);
		virtual int wm_setfocus(WPARAM wParam);
		virtual int wm_setfont(WPARAM wParam, LPARAM lParam);
		virtual int wm_settext(WPARAM wParam, LPARAM lParam);
		virtual int wm_showwindow(WPARAM wParam, LPARAM lParam);
		virtual int wm_size(WPARAM wParam, LPARAM lParam);
		virtual int wm_sizing(WPARAM wParam, LPARAM lParam);
		virtual int wm_syschar(WPARAM wParam, LPARAM lParam);
		virtual int wm_syscommand(WPARAM wParam, LPARAM lParam);
		virtual int wm_syskeydown(WPARAM wParam, LPARAM lParam);
		virtual int wm_syskeyup(WPARAM wParam, LPARAM lParam);
		virtual int wm_timer(WPARAM wParam, LPARAM lParam);
		virtual int wm_uninitmenupopup(WPARAM wParam, LPARAM lParam);
		virtual int wm_vscroll(WPARAM wParam, LPARAM lParam);
		virtual int wm_windowposchanged(LPARAM lParam);
		virtual int wm_windowposchanging(LPARAM lParam);

	protected:
		enum { MAX_LOADSTRING = 256 };

		static HINSTANCE s_hInstance;

		UINT m_nActive;
		HWND m_hWnd;
		HMENU m_hMenu;
		SystemWin32* m_system;

	};
}