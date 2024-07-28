#pragma once

#include <windows.h>

//increases readability for c-style string comparisons (case insensitive and Unicode safe)
#define STRING_MATCH(szOne, szTwo)        (_tcsicmp(szOne, szTwo) == 0)
#define STRING_NMATCH(szOne, szTwo, nLen) (_tcsnicmp(szOne, szTwo, nLen) == 0)

//perform safe string buffer size operations regardless if we use Unicode or not
#define STRING_SIZE(szBuffer) (sizeof(szBuffer) / sizeof(TCHAR))

//simplify the testing of a minimum version of Windows requirement being met
#define MIN_VERSION(nMajor, nMinor) (((BYTE)LOBYTE(LOWORD(GetVersion())) >= nMajor) && ((BYTE)HIBYTE(LOWORD(GetVersion())) >= nMinor))

//simplify the testing of whether or not the host os is NT-based or 9x
#define IS_WINNT (GetVersion() < (DWORD)0x80000000)

//HWND_BOTTOM, HWND_TOP, etc. are already defined in Winuser.h, and we don't want to cause possible
//compatibility issues. so instead, define our own constants for use with AlignWindow().
#define ALIGN_BOTTOM	0x0001
#define ALIGN_CENTER	0x0002
#define ALIGN_LEFT		0x0004
#define ALIGN_MIDDLE	0x0008
#define ALIGN_RIGHT     0x0010
#define ALIGN_TOP		0x0020

//registry functions
bool GetUserValue(const LPCSTR szSubkey, const LPCSTR szValue, DWORD dwType, void* pDest, DWORD dwSize);
bool SetUserValue(const LPCSTR szSubkey, const LPCSTR szValue, DWORD dwType, void *pDest, DWORD dwSize);
bool GetSysValue(const LPCSTR szSubkey, const LPCSTR szValue, DWORD dwType, void *pDest, DWORD dwSize);
bool SetSysValue(const LPCSTR szSubkey, const LPCSTR szValue, DWORD dwType, void *pDest, DWORD dwSize);

//cmds line functions
bool GetCmdLineValue(const LPSTR szArg, LPSTR szDest, size_t nLen);
