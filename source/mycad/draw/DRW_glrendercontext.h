#pragma once

#include "DRW_rendercontext.h"

#include <windows.h>

namespace mycad {
	/**
	* Declaration of GLRenderContext class.
	*/
	class GLRenderContext : RenderContext {
	public:
		GLRenderContext();
		GLRenderContext(HWND hWnd,
			int colorBits = 32,
			int depthBits = 24,
			int stencilBits = 8,
			int msaaSamples = 1);
		~GLRenderContext();

		HWND getHWND() const;
		HDC getHDC() const;
		HGLRC getHGLRC() const;

		bool activateContext() override;
		bool createContext() override;
		bool createContextWithAttributes();
		void releaseContext() override;
		int swapBuffers() override;

	private:
		HWND m_hWnd;
		HDC m_hDC;
		HGLRC m_hGLRC{ 0 };

		int m_nColorBits;
		int m_nDepthBits;
		int m_nStencilBits;
		int m_nMsaaSamples;

		bool setPixelFormat() override;
		int findPixelFormat() override;
		int findPixelFormat(HDC hDC);
		int findPixelFormatWithAttributes();

	};
}
