#include "MKE_popupmenu.h"

#include "fault.h"
#include "log.h"

#include <windowsx.h>

using namespace mycad;

Popupmenu::Popupmenu()
	: WindowWin32()
{

}
Popupmenu::~Popupmenu()
{

}

int Popupmenu::wm_char(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	
	

	return 0;
}
int Popupmenu::wm_close()
{
	return 1;
}
int Popupmenu::wm_command(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);



	return 0;
}
int Popupmenu::wm_create(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);



	return 0;
}
int Popupmenu::wm_destroy()
{
	return 1;
}
int Popupmenu::wm_exitsizemove()
{
	return 0;
}
int Popupmenu::wm_initmenupopup(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);



	return 0;
}
int Popupmenu::wm_keydown(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);



	return 0;
}
int Popupmenu::wm_mousemove(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);



	return 0;
}
int Popupmenu::wm_notify(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);



	return 0;
}
int Popupmenu::wm_paint()
{
	return 0;
}
int Popupmenu::wm_size(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);



	return 0;
}
