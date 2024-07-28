#include "DRW_glrendercontext.h"

#include "fault.h"
#include "log.h"

#include <GL/gl.h>
#include "wglext.h"
#include "glExtension.h"

using namespace mycad;

GLRenderContext::GLRenderContext()
    : m_hWnd(0),
    m_hDC(0),
    m_nColorBits(32),
    m_nDepthBits(24),
    m_nStencilBits(8),
    m_nMsaaSamples(0)
{}

GLRenderContext::GLRenderContext(
    HWND hWnd,
    int colorBits,
    int depthBits,
    int stencilBits,
    int msaaSamples)
    : m_hWnd(hWnd),
    m_nColorBits(colorBits),
    m_nDepthBits(depthBits),
    m_nStencilBits(stencilBits),
    m_nMsaaSamples(msaaSamples)
{
    m_hDC = ::GetDC(m_hWnd);

    //validate sample value, should be power of 2
    if ((m_nMsaaSamples & (m_nMsaaSamples - 1)) != 0) {
        //round to the next highest power of 2, referenced from "Bit Twiddling Hacks"
        m_nMsaaSamples--;
        m_nMsaaSamples |= m_nMsaaSamples >> 1;
        m_nMsaaSamples |= m_nMsaaSamples >> 2;
        m_nMsaaSamples |= m_nMsaaSamples >> 4;
        m_nMsaaSamples |= m_nMsaaSamples >> 8;
        m_nMsaaSamples |= m_nMsaaSamples >> 16;
        m_nMsaaSamples++;
    }
}

GLRenderContext::~GLRenderContext()
{

}

HWND GLRenderContext::getHWND() const { return m_hWnd; };
HDC GLRenderContext::getHDC() const { return m_hDC; };
HGLRC GLRenderContext::getHGLRC() const { return m_hGLRC; };

bool GLRenderContext::activateContext()
{
    if (::wglMakeCurrent(m_hDC, m_hGLRC) == 0) {
        return false;
    }
    else {
        return true;
    }
}
bool GLRenderContext::createContext()
{
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED,	//Flags
		PFD_TYPE_RGBA,																			//The kind of framebuffer. Colour4 or palette.
		32,																						//Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0, 0, 0,
		0, 0, 0, 0,
		24,																						//Number of bits for the depthbuffer
		8,																						//Number of bits for the stencilbuffer
		0,																						//Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int pixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	if (!SetPixelFormat(m_hDC, pixelFormat, &pfd)) {
		MessageBox(0, "Cannot set a suitable pixel format.", "Error", MB_ICONEXCLAMATION | MB_OK);
		log("Log::Error[%ld]:Failed to create OpenGL rendering context", ::GetLastError());
		ReleaseDC(m_hWnd, m_hDC);
		return false;
	}

	m_hGLRC = wglCreateContext(m_hDC);

	return true;
}
bool GLRenderContext::createContextWithAttributes()
{
    int pixelFormat = 0;
    if (m_nMsaaSamples <= 0) {
        pixelFormat = findPixelFormat();
    }
    else {
        pixelFormat = findPixelFormatWithAttributes();
    }

    if(pixelFormat <= 0) {
        MessageBox(0, "Failed to choose OpenGL pixel format.", "Error", MB_ICONEXCLAMATION | MB_OK);
        ReleaseDC(m_hWnd, m_hDC);
        return false;
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(m_hDC, pixelFormat, sizeof(pfd), &pfd);
    if(!SetPixelFormat(m_hDC, pixelFormat, &pfd)) {
        MessageBox(0, "Cannot set a suitable pixel format.", "Error", MB_ICONEXCLAMATION | MB_OK);
        ReleaseDC(m_hWnd, m_hDC);
        return false;
    }

    m_hGLRC = wglCreateContext(m_hDC);

    return true;
}
void GLRenderContext::releaseContext()
{
	if (!m_hDC || !m_hGLRC)
		return;

	//delete DC and RC
	wglMakeCurrent(m_hDC, NULL);
	wglDeleteContext(m_hGLRC);
	ReleaseDC(m_hWnd, m_hDC);

	//m_hDC = 0;
	m_hGLRC = 0;

	ReleaseDC(m_hWnd, m_hDC);

	log("Log::Closed OpenGL rendering context.");
}
int GLRenderContext::swapBuffers() { return (int)::SwapBuffers(m_hDC); }

bool GLRenderContext::setPixelFormat()
{
    PIXELFORMATDESCRIPTOR pfd;

    //find out the best matched pixel format
    int pixelFormat = findPixelFormat();
    if(pixelFormat == 0)
        return false;

    //set members of PIXELFORMATDESCRIPTOR with given mode ID
    DescribePixelFormat(m_hDC, pixelFormat, sizeof(pfd), &pfd);

    //set the pixel format
    if(!SetPixelFormat(m_hDC, pixelFormat, &pfd))
        return false;

    return true;
}
int GLRenderContext::findPixelFormat()
{
    int currMode = 0;                           //pixel format mode ID
    int bestMode = 0;                           //return value, best pixel format
    int currScore = 0;                          //points of current mode
    int bestScore = 0;                          //points of best candidate
    PIXELFORMATDESCRIPTOR pfd = { 0 };

    //search the available formats for the best mode
    for(currMode = 1; DescribePixelFormat(m_hDC, currMode, sizeof(pfd), &pfd) > 0; ++currMode) {
        //ignore if cannot support opengl
        if(!(pfd.dwFlags & PFD_SUPPORT_OPENGL))
            continue;

        //ignore if cannot render into a window
        if(!(pfd.dwFlags & PFD_DRAW_TO_WINDOW))
            continue;

        //ignore if cannot support rgba mode
        if((pfd.iPixelType != PFD_TYPE_RGBA) || (pfd.dwFlags & PFD_NEED_PALETTE))
            continue;

        //ignore if not double buffer
        if(!(pfd.dwFlags & PFD_DOUBLEBUFFER))
            continue;

        //try to find best candidate
        currScore = 0;

        //colour bits
        if(pfd.cColorBits >= m_nColorBits) ++currScore;
        if(pfd.cColorBits == m_nColorBits) ++currScore;

        //depth bits
        if(pfd.cDepthBits >= m_nDepthBits) ++currScore;
        if(pfd.cDepthBits == m_nDepthBits) ++currScore;

        //stencil bits
        if(pfd.cStencilBits >= m_nStencilBits) ++currScore;
        if(pfd.cStencilBits == m_nStencilBits) ++currScore;

        //alpha bits
        if(pfd.cAlphaBits > 0) ++currScore;

        //check if it is best mode so far
        if(currScore > bestScore) {
            bestScore = currScore;
            bestMode = currMode;
        }
    }
    return bestMode;
}
int GLRenderContext::findPixelFormat(HDC hDC)
{
    int currMode = 0;                           //pixel format mode ID
    int bestMode = 0;                           //return value, best pixel format
    int currScore = 0;                          //points of current mode
    int bestScore = 0;                          //points of best candidate
    PIXELFORMATDESCRIPTOR pfd = { 0 };

    //search the available formats for the best mode
    for (currMode = 1; DescribePixelFormat(hDC, currMode, sizeof(pfd), &pfd) > 0; ++currMode) {
        //ignore if cannot support opengl
        if (!(pfd.dwFlags & PFD_SUPPORT_OPENGL))
            continue;

        //ignore if cannot render into a window
        if (!(pfd.dwFlags & PFD_DRAW_TO_WINDOW))
            continue;

        //ignore if cannot support rgba mode
        if ((pfd.iPixelType != PFD_TYPE_RGBA) || (pfd.dwFlags & PFD_NEED_PALETTE))
            continue;

        //ignore if not double buffer
        if (!(pfd.dwFlags & PFD_DOUBLEBUFFER))
            continue;

        //try to find best candidate
        currScore = 0;

        //colour bits
        if (pfd.cColorBits >= m_nColorBits) ++currScore;
        if (pfd.cColorBits == m_nColorBits) ++currScore;

        //depth bits
        if (pfd.cDepthBits >= m_nDepthBits) ++currScore;
        if (pfd.cDepthBits == m_nDepthBits) ++currScore;

        //stencil bits
        if (pfd.cStencilBits >= m_nStencilBits) ++currScore;
        if (pfd.cStencilBits == m_nStencilBits) ++currScore;

        //alpha bits
        if (pfd.cAlphaBits > 0) ++currScore;

        //check if it is best mode so far
        if (currScore > bestScore) {
            bestScore = currScore;
            bestMode = currMode;
        }
    }
    return bestMode;
}
int GLRenderContext::findPixelFormatWithAttributes()
{
	log("Log::Temporary window class registered for WGL_ARB_pixel_format/WGL_ARB_multisample extensions.");

    HWND tmpHandle = CreateWindow("STATIC",
        "GL",
        WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0,
        0,
        64,
        64,
        (HWND)0,
        (HMENU)0,
        ::GetModuleHandle(0),
        (LPVOID)0);

    if (tmpHandle == 0) {
        MessageBox(NULL, "Fatal Error - Could not create temp window!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        ErrorHandler();
    }

	HDC tmpHdc = GetDC(tmpHandle);

    //first, set a pixel format using a traditional way
    int pixelFormat = findPixelFormat(tmpHdc);
    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(tmpHdc, pixelFormat, sizeof(pfd), &pfd);
    if(!SetPixelFormat(tmpHdc, pixelFormat, &pfd)) {
        MessageBox(0, "Cannot set a suitable pixel format for temporary window.", "Error", MB_ICONEXCLAMATION | MB_OK);
        ReleaseDC(tmpHandle, tmpHdc);             //remove device context
        if (tmpHandle != 0) {
            ::DestroyWindow(tmpHandle);
        }
        return 0;
    }

    //create a dummy opengl RC
    HGLRC tmpHglrc = wglCreateContext(tmpHdc);    //create a new OpenGL rendering context
    wglMakeCurrent(tmpHdc, tmpHglrc);

    //In WGL_ARB_pixel_format, colorBits excludes alphaBits
    int alphaBits = 0;
    if(m_nColorBits == 32) {
        m_nColorBits = 24;
        alphaBits = 8;
    }

    //attribute list for wglChoosePixelFormatARB()
    int attributes[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
        WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
        WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,     m_nColorBits,
        WGL_ALPHA_BITS_ARB,     alphaBits,
        WGL_DEPTH_BITS_ARB,     m_nDepthBits,
        WGL_STENCIL_BITS_ARB,   m_nStencilBits,
        WGL_SAMPLE_BUFFERS_ARB, 1,                  //must be 1 for MSAA
        WGL_SAMPLES_ARB,        m_nMsaaSamples,     //must be power of 2: 1,2,4,8,16
        0
    };

    //if WGL_ARB_pixel_format is not supported, you cannot use wglChoosePixelFormatARB()
    //if WGL_ARB_multisample is not supported, you cannot use MSAA
    glExtension& extension = glExtension::getInstance();
    if(extension.isSupported("WGL_ARB_pixel_format") && extension.isSupported("WGL_ARB_multisample")) {
        log("Log::WGL_ARB_pixel_format and WGL_ARB_multisample are supported.");
        log("Log::Use wglChoosePixelFormatARB() to find a pixel format with MSAA.");
        unsigned int formatCount = 0;
        BOOL result = FALSE;
        int samples = 0;
        for(samples = m_nMsaaSamples; samples > 0; samples /= 2) {
            //find pixel format with wglChoosePixelFormatARB()
            attributes[21] = samples;
            result = wglChoosePixelFormatARB(tmpHdc,        //hWnd to device context
                                             attributes,    //pointer to int attributes
                                             0,             //pointer to float attributes
                                             1,             //max pixel formats to be returned
                                             &pixelFormat,  //pointer to pixelFormat
                                             &formatCount); //returns the number of matching formats

            if(result == TRUE && formatCount > 0)
                break;
        }
        log("Log::MSAA mode: %dx", samples);
    }
    else {
        log("Log::[WARNING] WGL_ARB_pixel_format and WGL_ARB_multisample are NOT supported.");
        log("Log::[WARNING] MSAA cannot be enabled.");
    }
    log("Log::Pixel Format: %d", pixelFormat);

    //destroy the dummy opengl window
    wglMakeCurrent(0, 0);
    wglDeleteContext(tmpHglrc);
    ReleaseDC(tmpHandle, tmpHdc);

#pragma warning( push )
#pragma warning( disable : 6387)
    ::DestroyWindow(tmpHandle);
#pragma warning( pop )

    log("Log::Temporary window for WGL_ARB_pixel_format extension destroyed.");

	return pixelFormat;
}
