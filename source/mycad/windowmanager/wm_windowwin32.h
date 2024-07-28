#pragma once

#include "wm_window.h"

//#include <commctrl.h>

namespace mycad {
	/**
	* DLGTEMPLATE struct is defined in winuser.h
	*/
	/*typedef struct {
		DWORD style;
		DWORD dwExtendedStyle;
		WORD  cdit;
		short x;
		short y;
		short cx;
		short cy;
	} DLGTEMPLATE, * LPDLGTEMPLATE;*/
	
	/**
	* The DLGTEMPLATEEX structure is not defined in any standard header file.
	*/
	typedef struct {
		WORD	dlgVer;
		WORD	signature;
		DWORD	helpID;
		DWORD	exStyle;
		DWORD	style;
		WORD	cDlgItems;
		short	x;
		short	y;
		short	cx;
		short	cy;
		//sz_Or_Ord menu;
		//sz_Or_Ord windowClass;
		//WCHAR	title[titleLen];
		//WORD	pointsize;
		//WORD	weight;
		//BYTE	italic;
		//BYTE	charset;
		//WCHAR	typeface[stringLen];
	} DLGTEMPLATEEX, * LPDLGTEMPLATEEX;//Not defined in any standard header file

	/**
	* Declaration of MouseTrackEvents class.
	*/
	class MouseTrackEvents {
		bool m_bMouseTracking;

	public:
		/**
		* Constructor.
		* Creates a new MouseTrackEvents instance.
		* \param flags: the services requested.
		*/
		MouseTrackEvents(DWORD flags = TME_LEAVE) : dwFlags(flags), m_bMouseTracking(false) {}
		void OnMouseMove(HWND hWnd)
		{
			if (!m_bMouseTracking) {
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.hwndTrack = hWnd;
				tme.dwFlags = dwFlags;
				tme.dwHoverTime = HOVER_DEFAULT;
				TrackMouseEvent(&tme);
				m_bMouseTracking = true;
			}
		}
		void Reset()
		{
			m_bMouseTracking = false;
		}

	private:
		DWORD dwFlags{ TME_LEAVE };

	};

	/**
	* Declaration of WindowWin32 class.
	*/
	class SystemWin32;
	class WindowWin32 : public Window {
	public:
		/**
		* Constructor.
		* Creates a new window.
		* To check if the window was created properly, use the #getValid() method.
		* \param dialog: The type of window to create.
		*/
		WindowWin32();
		WindowWin32(HINSTANCE hInstance);
		/**
		* Destructor.
		* Closes the window and disposes resources allocated.
		*/
		virtual ~WindowWin32();

		DWORD getExStyle();

		void setClassName(const char* lpszClassName);
		char* getClassName();

		//void setWindowName(const char* lpszWindowName);
		//char* getWindowName();

		LONG getStyle();

		void setX(int x);
		int getX() const;

		void setY(int y);
		int getY() const;

		void setCX(int cx);
		int getCX() const;

		void setCY(int cy);
		int getCY() const;

		HWND getParentHWND() const;

		int getID();

		void setIcon(HWND hWnd, LPCSTR lpszResName);

		virtual int wm_size(WPARAM wParam, LPARAM lParam);

	protected:
		enum { WINDOWSTATE_ACTIVETAB = 0, WINDOWSTATE_POSITION, WINDOWSTATE_RECT, WINDOWSTATE_SCROLLBARS, WINDOWSTATE_SHOW, WINDOWSTATE_SIZE };

		char m_szClassName[MAX_LOADSTRING];
		//char m_szWindowName[MAX_LOADSTRING];
		int m_x;
		int m_y;
		int m_cx;
		int m_cy;

		virtual void wm_size();

		/**
		* Wrapper for SendMessage.
		* \param hWnd: handle to the destination window.
		* \param idFrom: id of the source window.
		* \param text: command text to be sent to destination window.
		* \return LRESULT value.
		*/
		LRESULT SendCommandText(HWND hWnd, UINT_PTR idFrom, const char* text);
		/**
		* Wrapper for SendMessage.
		* \param hWnd: handle to the destination window.
		* \param idFrom: id of the source window.
		* \param code: command id to be sent to destination window.
		* \param lParam: pointer to any additional data.
		* \return LRESULT value.
		*/
		LRESULT SendCommandParam(HWND hWnd, UINT_PTR idFrom, int code, LPARAM lParam = 0);

		void setWindowState(int var, LPARAM lparam, const char* path, const char* key = 0);
		void getWindowState(int var, void* pDest, const char* path, const char* key = 0);
	};
}
