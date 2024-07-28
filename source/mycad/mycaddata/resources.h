#pragma once

#ifdef MATHLIBRARY_EXPORTS
#define MATHLIBRARY_API __declspec(dllexport)
#else
#define MATHLIBRARY_API __declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

/////////////////////////////////////////////////////////////////////////////
//
// Icons
//

#define IDI_MYCAD																				114

#define IDI_VIEW_VIEWMANAGER_CURRENT															1500
#define IDI_VIEW_VIEWMANAGER_MODELVIEWS															1510
#define IDI_VIEW_VIEWMANAGER_LAYOUTVIEWS														1520
#define IDI_VIEW_VIEWMANAGER_PRESETVIEWS														1530

#define IDI_VIEW_3DVIEWS_TOP																	12470
#define IDI_VIEW_3DVIEWS_BOTTOM																	12480
#define IDI_VIEW_3DVIEWS_LEFT																	12490
#define IDI_VIEW_3DVIEWS_RIGHT																	12500
#define IDI_VIEW_3DVIEWS_FRONT																	12510
#define IDI_VIEW_3DVIEWS_BACK																	12520
#define IDI_VIEW_3DVIEWS_SWISOMETRIC															12530
#define IDI_VIEW_3DVIEWS_SEISOMETRIC															12540
#define IDI_VIEW_3DVIEWS_NEISOMETRIC															12550
#define IDI_VIEW_3DVIEWS_NWISOMETRIC															12560

#define IDB_VIEW_3DVIEWS_TOP																	12470
#define IDB_VIEW_3DVIEWS_BOTTOM																	12480
#define IDB_VIEW_3DVIEWS_LEFT																	12490
#define IDB_VIEW_3DVIEWS_RIGHT																	12500
#define IDB_VIEW_3DVIEWS_FRONT																	12510
#define IDB_VIEW_3DVIEWS_BACK																	12520
#define IDB_VIEW_3DVIEWS_SWISOMETRIC															12530
#define IDB_VIEW_3DVIEWS_SEISOMETRIC															12540
#define IDB_VIEW_3DVIEWS_NEISOMETRIC															12550
#define IDB_VIEW_3DVIEWS_NWISOMETRIC															12560

#define IDI_DRAW_POINT_SINGLE																	56010
#define IDI_DRAW_POINT_MULTIPLE																	56011
#define IDI_DRAW_POINT_DIVIDE																	56012
#define IDI_DRAW_POINT_MEASURE																	56013
#define IDI_DRAW_LINE																			56020
#define IDI_DRAW_RAY																			56030
#define IDI_DRAW_XLINE																			56040
#define IDI_DRAW_MULTILINE																		56050
#define IDI_DRAW_POLYLINE																		56060
#define IDI_DRAW_3DPOLYLINE																		56070
#define IDI_DRAW_POLYGON																		56080
#define IDI_DRAW_RECTANGLE																		56090
#define IDI_DRAW_HELIX																			56095
#define IDI_DRAW_ARC																			56110
#define IDI_DRAW_ARC_CENTRE_START_END															56120
#define IDI_DRAW_ARC_THREE_POINTS																56130
#define IDI_DRAW_CIRCLE																			56140
#define IDI_DRAW_CIRCLE_CENTRE_RADIUS															56150
#define IDI_DRAW_CIRCLE_THREE_POINTS															56160
#define IDI_DRAW_SPLINE																			56170
#define IDI_DRAW_ELLIPSE																		56180
#define IDI_DRAW_ELLIPSE_CENTRE_AXES															56190
#define IDI_DRAW_ELLIPSE_FOUR_POINTS															56200
#define IDI_DRAW_ELLIPSE_ARC																	56210
#define IDI_DRAW_REVISION_CLOUD																	56230

/////////////////////////////////////////////////////////////////////////////
//
// Menus
//

/////////////////////////////////////////////////////////////////////////////
//
// Accelerators
//

/////////////////////////////////////////////////////////////////////////////
//
// Dialogs
//

/////////////////////////////////////////////////////////////////////////////
//
// Controls
//

#define IDC_APPLICATION																				50003
#define IDC_FAKEMENU																				50005

/////////////////////////////////////////////////////////////////////////////
//
// String Tables
//

#define ID_COMPANY_NAME																				110
#define ID_APPLICATION_CLASS																		111
#define ID_APPLICATION_NAME																			112
#define ID_APPLICATION_RELEASE																		113
#define ID_APPLICATION_VERSION_NUMBER_MAJOR															114
#define ID_APPLICATION_VERSION_NUMBER_MINOR															115
#define ID_APPLICATION_VERSION_NUMBER_PATCH															116

#define ID_FAKEMENU_CLASS																			120

// The Fibonacci recurrence relation describes a sequence F
// where F(n) is { n = 0, a
//               { n = 1, b
//               { n > 1, F(n-2) + F(n-1)
// for some initial integral values a and b.
// If the sequence is initialized F(0) = 1, F(1) = 1,
// then this relation produces the well-known Fibonacci
// sequence: 1, 1, 2, 3, 5, 8, 13, 21, 34, ...

// Initialize a Fibonacci relation sequence
// such that F(0) = a, F(1) = b.
// This function must be called before any other function.
extern "C" MATHLIBRARY_API void fibonacci_init(
    const unsigned long long a, const unsigned long long b);

// Produce the next value in the sequence.
// Returns true on success and updates current value and index;
// false on overflow, leaves current value and index unchanged.
extern "C" MATHLIBRARY_API bool fibonacci_next();

// Get the current value in the sequence.
extern "C" MATHLIBRARY_API unsigned long long fibonacci_current();

// Get the position of the current value in the sequence.
extern "C" MATHLIBRARY_API unsigned fibonacci_index();
