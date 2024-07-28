#pragma once

#include "wm_windowwin32.h"

#include "CMD_commands.h"

namespace mycad {
	/**
	* Declaration of CommandLineInputBase class.
	*/
	class CommandLineInputBase : public WindowWin32 {
	public:
		CommandLineInputBase();
		virtual ~CommandLineInputBase();

		virtual void setBackgroundColour(COLORREF colour);
		virtual void setBorderColour(COLORREF colour);
		virtual void setTextColour(COLORREF colour);
		virtual void setTextBackgroundColour(COLORREF colour);

	protected:
		COLORREF m_clrBackground;
		COLORREF m_clrBorder;
		COLORREF m_clrText;
		COLORREF m_clrTextBackground;

		HBRUSH m_hbrBackground;
		HBRUSH m_hbrTextBackground;

		int m_nScreenHeight;

	};



	/**
	* Declaration of DropdownWindow class.
	*/
	class ListboxWindowSubclass;
	class DropdownWindow : public CommandLineInputBase {
	public:
		DropdownWindow(UINT uListboxId);
		virtual ~DropdownWindow();

		static SIZE s_szSize;
		static SIZE s_szOffset;

		int getListboxID();
		HWND getListboxHWND();

		int wm_close() override;
		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_contextmenu(WPARAM wParam, LPARAM lParam) override;
		int wm_create(WPARAM wParam, LPARAM lParam) override;
		int wm_ctlcolorlistbox(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;

	private:
		ListboxWindowSubclass* m_lpListboxSubclassWindow;

		UINT m_uListboxId;
		HWND m_hListbox;

		void wm_command_lbn_selchange(int controlid);

		void getDisplayText(std::vector<COMMANDINFO>::iterator it, char &displaytext);
		HRESULT addListboxItems();

	};



	/**
	Declaration of DynamicInputWindow class.
	*/
	class DynamicInputWindow : public CommandLineInputBase {
	public:
		DynamicInputWindow(int dropdownlines = 7);
		~DynamicInputWindow();

		static UINT s_nInputSearchDelay;
		static UINT s_nLowercaseToUppercaseDelay;

		virtual void setDropdownLines(int lines);

		virtual BOOL setText(const char* text);

		virtual int wm_close() override;
		virtual int wm_killfocus(WPARAM wParam) override;
		virtual int wm_notify(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_setcursor(WPARAM wParam, LPARAM lParam) override;
		virtual int wm_timer(WPARAM wParam, LPARAM lParam) override;

	protected:
		HWND m_hCommand;

		int m_nDropdownLines;
		DropdownWindow* m_lpDropdownWindow;

		COLORREF m_clrBackground_Command;
		HBRUSH m_hbrBackground_Command;

		TCHAR m_szCueBanner[MAX_LOADSTRING];

		bool m_bLowercaseToUppercaseDelay;
		bool m_bIsIndirectTextInput;

		int createDropdownWindow(RECT position, UINT uListboxId);

		virtual void wm_command_escape(int controlid);
		virtual void wm_command_return(int controlid);

		virtual void wm_timer_inputsearchdelay();
		virtual void wm_timer_lowercasetouppercasedelay();

	};
}
