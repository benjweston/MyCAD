#pragma once

#include <windows.h>

namespace mycad {
	/**
	* Declaration of IWindow class.
	*/
	class IWindow {
	public:
		/**
		* Constructor.
		* Creates a new window.
		* To check if the window was created properly, use the #getValid() method.
		*/
		IWindow() {}
		/**
		* Destructor.
		* Closes the window and disposes resources allocated.
		*/
		virtual ~IWindow() {}

		virtual BOOL getActive() const = 0;
		virtual BOOL getValid() const = 0;
		virtual HWND getHWND() const = 0;
	};
}