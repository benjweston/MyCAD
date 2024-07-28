#include "SYS_systemwin32.h"

#include "DCK_pane.h"
#include "DCK_resource.h"

#include "reg_util.h"

#include "fault.h"
#include "log.h"

using namespace mycad;


Pane::Pane()
	: WindowWin32(false)
{
	log("Log::Pane::Pane()");
}
Pane::~Pane()
{
	log("Log::Pane::~Pane()");
}
