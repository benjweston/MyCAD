#pragma once

#include "SYS_systemwin32.h"

namespace mycad {
	/**
	Declaration of SDISystemWin32 class.
	*/
	class SDISystemWin32 : public SystemWin32 {
	public:
		SDISystemWin32(HINSTANCE hInstance);
		~SDISystemWin32();

	private:
		void registerApplicationClasses(HINSTANCE hModule) override;

	};
}
