//TODO: Remove this code and add these to Properties/Configuration Properties/Linker/Input/Additional Dependencies.
#pragma comment(lib, "comctl32.lib")//Win32 Common Controls.
#pragma comment(lib, "shlwapi.lib")//Required for PathStripPath().
#pragma comment(lib, "propsys.lib")//Required for IFileDialog interface.
#pragma comment(lib, "freetype.lib")//Required for rendering TrueType fonts with OpenGL.
#pragma comment(lib, "opengl32.lib")//OpenGL libraray.
#pragma comment(lib, "glu32.lib")//OpenGL utility library.
#pragma comment(lib, "uxtheme.lib")//Required for SetWindowTheme.

#include "SYS_system.h"

#include "fault.h"
#include "log.h"

#include <SDKDDKVer.h>
#include <uxtheme.h>

//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>

//$(SolutionDir)extern\assimp\include
//#include <assimp/cimport.h>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>

//#define _WIN32_WINNT 0x0502
//#define _WIN32_IE 0x0600
#define _WINVER 0x0502 
#define ISOLATION_AWARE_ENABLED 1
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define WIN32_LEAN_AND_MEAN

#define BOTH

static HANDLE g_system = NULL;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

#ifdef NONE
	logMode(LOG_MODE_NONE);
#endif

#ifdef FILE
	logMode(LOG_MODE_FILE);
#endif

#ifdef OUTPUT
	logMode(LOG_MODE_OUTPUT);
#endif

#ifdef BOTH
	logMode(LOG_MODE_BOTH);
#endif

	::SetThemeAppProperties(STAP_ALLOW_NONCLIENT | STAP_ALLOW_CONTROLS | STAP_ALLOW_WEBCONTENT);

	mycad::System* system = mycad::System::createSystem(hInstance);
	g_system = (HANDLE)system;

	system->processEvents();

	return 0;
}
