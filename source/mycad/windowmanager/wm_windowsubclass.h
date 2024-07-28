#pragma once

#include "wm_window.h"

namespace mycad {
	/**
	* Declaration of WindowSubclass class.
	*/
	/*!
	Base class for subclassed controls.
	*/
	class WindowSubclass : public Window {
	public:
		WindowSubclass(HWND hWnd, UINT_PTR uIdSubclass);
		WindowSubclass(HWND hWnd, HINSTANCE hInstance, UINT_PTR uIdSubclass);
		virtual ~WindowSubclass();

	protected:
		UINT_PTR m_uIdSubclass;

	};
}
