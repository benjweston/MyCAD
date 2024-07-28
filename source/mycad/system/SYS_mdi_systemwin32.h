#pragma once

#include "SYS_systemwin32.h"

namespace mycad {
	/**
	Declaration of MDISystemWin32 class.
	*/
	class MDISystemWin32 : public SystemWin32 {
	public:
		MDISystemWin32(HINSTANCE hInstance);
		~MDISystemWin32();

		int processEvents() override;

	private:
		void registerApplicationClasses(HINSTANCE hModule) override;

	};
}
