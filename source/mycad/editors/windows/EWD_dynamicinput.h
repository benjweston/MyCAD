#pragma once

#include "EWD_dynamicinputwindow.h"

namespace mycad {
	/**
	* Declaration of DynamicInput class.
	*/
	class EditWindowSubclass;
	class DynamicInput : public DynamicInputWindow {
	public:
		DynamicInput(int dropdownlines = 7);
		~DynamicInput();

		static BOOL s_nVisible;
		static SIZE s_szOffset;
		static SIZE s_szSize;
		static BOOL s_nTintForXYZ;

		void setDynamicInputMode(int mode);
		int getDynamicInputMode();

		int wm_close() override;
		int wm_command(WPARAM wParam, LPARAM lParam) override;
		int wm_contextmenu(WPARAM wParam, LPARAM lParam) override;
		int wm_create(WPARAM wParam, LPARAM lParam) override;
		int wm_destroy() override;
		int wm_killfocus(WPARAM wParam) override;
		int wm_move(LPARAM lParam) override;
		int wm_paint() override;
		int wm_setfocus(WPARAM wParam) override;
		int wm_showwindow(WPARAM wParam, LPARAM lParam) override;
		int wm_size(WPARAM wParam, LPARAM lParam) override;

	private:
		EditWindowSubclass* m_lpEditSubclassWindow;//m_hCommand uses m_lpEditSubclassWindow.

		int m_nDYNMODE;
		int m_nDYNPROMPT;
		int m_nDYNINFOTIPS;

		void wm_command_change();

		void setDropdownPosition();

	};



	//! DynamicInput class.
	/*!
	*  DynamicInput class - container window for edit control displaying dynamic command input.
	*  @author B.J.W.
	*/
	class wnd_PointerInput : public DynamicInputWindow {
	public:
		wnd_PointerInput();
		~wnd_PointerInput();

		static BOOL s_nVisible;
		static SIZE s_szSize;



	private:


	};



	//! wnd_DimensionInput class.
	/*!
	*  wnd_DimensionInput class - container window for edit control displaying dynamic command input.
	*  @author B.J.W.
	*/
	class wnd_DimensionInput : public DynamicInputWindow {
	public:
		wnd_DimensionInput();
		~wnd_DimensionInput();

		static BOOL s_nVisible;
		static SIZE s_szSize;



	private:


	};



	//! wnd_DynamicPrompts class.
	/*!
	*  wnd_DynamicPrompts class - container window for edit control displaying dynamic command input.
	*  @author B.J.W.
	*/
	class wnd_DynamicPrompts : public DynamicInputWindow {
	public:
		wnd_DynamicPrompts();
		~wnd_DynamicPrompts();

		static BOOL s_nVisible;
		static SIZE s_szSize;



	private:


	};
}
