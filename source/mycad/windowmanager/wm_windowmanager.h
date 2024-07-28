#pragma once

#include "LIB_objectmanager.h"

namespace mycad {
	/**
	* Declaration of WindowManager class.
	*/
	class Window;
	class WindowManager : public ObjectManager<Window> {
	public:
		WindowManager();

		BOOL getWindow(const char* classname, Window* window);
		Window* getWindow(const char* classname);

	private:
		enum { MAX_LOADSTRING = 256 };

	};
}
