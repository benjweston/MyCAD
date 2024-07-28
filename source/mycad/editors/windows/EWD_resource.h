/////////////////////////////////////////////////////////////////////////////
//
// Icons
//

/////////////////////////////////////////////////////////////////////////////
//
// Menus
//

#define ID_TEXTWINDOW																				5000
#define ID_TEXTWINDOW_RECENTCOMMANDS																5010
#define ID_TEXTWINDOW_COPY																			5020
#define ID_TEXTWINDOW_COPYHISTORY																	5030
#define ID_TEXTWINDOW_PASTE																			5040
#define ID_TEXTWINDOW_PASTETOCMDLINE																5050
#define ID_TEXTWINDOW_OPTIONS																		5060

#define ID_TEXTWINDOW_CONTEXTMENU																	6000
#define ID_TEXTWINDOW_CONTEXTMENU_RECENTCOMMANDS													6010
#define ID_TEXTWINDOW_CONTEXTMENU_COMMAND_1															6020
#define ID_TEXTWINDOW_CONTEXTMENU_COMMAND_2															6030
#define ID_TEXTWINDOW_CONTEXTMENU_COMMAND_3															6040
#define ID_TEXTWINDOW_CONTEXTMENU_INPUTSETTINGS														6050
#define ID_TEXTWINDOW_CONTEXTMENU_AUTOCOMPLETE														6060
#define ID_TEXTWINDOW_CONTEXTMENU_AUTOCORRECT														6070
#define ID_TEXTWINDOW_CONTEXTMENU_SEARCHSYSTEMVARIABLES												6080
#define ID_TEXTWINDOW_CONTEXTMENU_MIDSTRINGSEARCH													6090
#define ID_TEXTWINDOW_CONTEXTMENU_DELAYTIME															6100
#define ID_TEXTWINDOW_CONTEXTMENU_COPY																6110
#define ID_TEXTWINDOW_CONTEXTMENU_COPYHISTORY														6120
#define ID_TEXTWINDOW_CONTEXTMENU_PASTE																6130
#define ID_TEXTWINDOW_CONTEXTMENU_PASTETOCMDLINE													6140
#define ID_TEXTWINDOW_CONTEXTMENU_OPTIONS															6150

#define ID_COMMANDLINE_CONTEXTMENU																	7000
#define ID_COMMANDLINE_CONTEXTMENU_RECENTCOMMANDS													7010
#define ID_COMMANDLINE_CONTEXTMENU_COMMAND_1														7020
#define ID_COMMANDLINE_CONTEXTMENU_COMMAND_2														7030
#define ID_COMMANDLINE_CONTEXTMENU_COMMAND_3														7040
#define ID_COMMANDLINE_CONTEXTMENU_INPUTSETTINGS													7050
#define ID_COMMANDLINE_CONTEXTMENU_AUTOCOMPLETE														7060
#define ID_COMMANDLINE_CONTEXTMENU_AUTOCORRECT														7070
#define ID_COMMANDLINE_CONTEXTMENU_SEARCHSYSTEMVARIABLES											7080
#define ID_COMMANDLINE_CONTEXTMENU_SEARCHCONTENT													7090
#define ID_COMMANDLINE_CONTEXTMENU_MIDSTRINGSEARCH													7100
#define ID_COMMANDLINE_CONTEXTMENU_DELAYTIME														7110
#define ID_COMMANDLINE_CONTEXTMENU_LINESOFPROMPTHISTORY												7120
#define ID_COMMANDLINE_CONTEXTMENU_INPUTSEARCHOPTIONS												7130
#define ID_COMMANDLINE_CONTEXTMENU_CUT																7140
#define ID_COMMANDLINE_CONTEXTMENU_COPY																7150
#define ID_COMMANDLINE_CONTEXTMENU_COPYHISTORY														7160
#define ID_COMMANDLINE_CONTEXTMENU_PASTE															7170
#define ID_COMMANDLINE_CONTEXTMENU_PASTETOCMDLINE													7180
#define ID_COMMANDLINE_CONTEXTMENU_TRANSPARENCY														7190
#define ID_COMMANDLINE_CONTEXTMENU_OPTIONS															7200

#define ID_DYNAMICINPUT_CONTEXTMENU																	8000
#define ID_DYNAMICINPUT_CONTEXTMENU_UNDO															8010
#define ID_DYNAMICINPUT_CONTEXTMENU_CUT																8020
#define ID_DYNAMICINPUT_CONTEXTMENU_COPY															8030
#define ID_DYNAMICINPUT_CONTEXTMENU_PASTE															8040
#define ID_DYNAMICINPUT_CONTEXTMENU_DELETE															8050
#define ID_COMMANDLINE_CONTEXTMENU_SELECTALL														8060
#define ID_DYNAMICINPUT_CONTEXTMENU_RIGHTTOLEFTREADINGORDER											8070
#define ID_DYNAMICINPUT_CONTEXTMENU_SHOWUNICODECONTROLCHARACTERS									8080
#define ID_DYNAMICINPUT_CONTEXTMENU_INSERTUNICODECONTROLCHARACTERS									8090
#define ID_DYNAMICINPUT_CONTEXTMENU_OPENIME															8100
#define ID_DYNAMICINPUT_CONTEXTMENU_RECONVERSION													8110

#define ID_DROPDOWN_CONTEXTMENU																		9000
#define ID_DROPDOWN_CONTEXTMENU_AUTOCOMPLETE														9010
#define ID_DROPDOWN_CONTEXTMENU_AUTOCORRECT															9020
#define ID_DROPDOWN_CONTEXTMENU_SEARCHSYSTEMVARIABLES												9030
#define ID_DROPDOWN_CONTEXTMENU_MIDSTRINGSEARCH														9040
#define ID_DROPDOWN_CONTEXTMENU_DELAYTIME															9050

/////////////////////////////////////////////////////////////////////////////
//
// Accelerators
//

/////////////////////////////////////////////////////////////////////////////
//
// Dialogs
//

#define IDD_COMMANDLINE_CLOSECHECK																	8000

/////////////////////////////////////////////////////////////////////////////
//
// Controls
//

#define IDC_LISTBOX_SUBCLASS_ID																		2000
#define IDC_EDIT_SUBCLASS_ID																		2010
#define IDC_LISTBOX_TOOLTIP_ID																		2020

#define IDC_COMMANDLINE																				2100
#define IDC_COMMANDLINE_EDIT_HISTORY																2110
#define IDC_COMMANDLINE_EDIT_COMMAND																2120
#define IDC_COMMANDLINE_DROPDOWNWINDOW_LISTBOX_COMMANDS												2130

#define IDC_COMMANDLINE_CLOSECHECK_LTEXT_CLOSE														2140
#define IDC_COMMANDLINE_CLOSECHECK_LTEXT_DISPLAY													2150
#define IDC_COMMANDLINE_CLOSECHECK_ALWAYSCLOSE														2160

#define IDC_DYNAMICINPUT																			2200
#define IDC_DYNAMICINPUT_EDIT_COMMAND																2210
#define IDC_DYNAMICINPUT_DROPDOWNWINDOW_LISTBOX_COMMANDS											2220

#define IDC_TEXTWINDOW																				2300
#define IDC_TEXTWINDOW_EDIT_HISTORY																	2310
#define IDC_TEXTWINDOW_STATIC_COMMAND																2320
#define IDC_TEXTWINDOW_EDIT_COMMAND																	2330
#define IDC_TEXTWINDOW_DROPDOWNWINDOW_LISTBOX_COMMANDS												2340

/////////////////////////////////////////////////////////////////////////////
//
// Timers
//

#define IDT_INPUTSEARCHDELAY																		130
#define IDT_LOWERCASETOUPPERCASEDELAY																131

/////////////////////////////////////////////////////////////////////////////
//
// String Tables
//

#define ID_TEXTWINDOW_CLASS																			121
#define ID_TEXTWINDOW_NAME																			122

#define ID_COMMANDLINE_CLASS																		123
#define ID_COMMANDLINE_NAME																			124

#define ID_DYNAMICINPUT_CLASS																		125
#define ID_DYNAMICINPUT_NAME																		126

#define ID_DROPDOWN_CLASS																			128

#ifndef IDC_STATIC
#define IDC_STATIC																					-1
#endif

//Next default values for new objects
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS

#define _APS_NO_MFC																					130
#define _APS_NEXT_RESOURCE_VALUE																	129
#define _APS_NEXT_COMMAND_VALUE																		32771
#define _APS_NEXT_CONTROL_VALUE																		1000
#define _APS_NEXT_SYMED_VALUE																		110
#endif
#endif
