#pragma once

#include <windows.h>

namespace mycad {
	class RenderContext {
	public:
		RenderContext() {}
		virtual ~RenderContext() {}

		virtual bool activateContext() = 0;
		virtual bool createContext() = 0;
		virtual void releaseContext() = 0;
		virtual int swapBuffers() = 0;

	protected:
		virtual bool setPixelFormat() = 0;
		virtual int findPixelFormat() = 0;

	};
}
