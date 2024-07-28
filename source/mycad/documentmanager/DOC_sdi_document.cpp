#include "SYS_systemwin32.h"

#include "DOC_sdi_document.h"
#include "DOC_resource.h"

#include "DOC_glrenderwindow.h"
#include "DRW_glrenderthread.h"

#include "EDL_dialog_drawingunits.h"
#include "EDL_dialog_draftingsettings.h"
#include "EDL_dialog_drawingwindowcolours.h"
#include "EDL_dialog_options.h"
#include "EDL_dialog_ucs.h"
#include "EDL_dialog_ucsicon.h"
#include "EDL_dialog_viewmanager.h"
#include "EDL_dialog_viewports.h"
#include "EDL_resource.h"

#include "EPL_resource.h"

#include "EWD_textwindow.h"
#include "EWD_commandline.h"
#include "EWD_dynamicinput.h"
#include "EWD_resource.h"

#include "entities.h"

#include "resources.h"

#include "char_util.h"//Required for toChar()
#include "reg_util.h"

#include "fault.h"
#include "log.h"

#include <commctrl.h>
#include <Shlwapi.h>//Required for PathStripPath()
#include <strsafe.h>
#include <tchar.h>//Required header for _tcscat_s, _tcscpy_s.
#include <windowsx.h>

using namespace mycad;

SDIDocument::SDIDocument(DynamicInputWindow* commandwindows[], const char* filename, int tabindex)
	: Document(commandwindows, filename, tabindex)
{
	log("Log::SDIDocument::SDIDocument()");
}
SDIDocument::~SDIDocument()
{
	log("Log::SDIDocument::~SDIDocument()");
}
