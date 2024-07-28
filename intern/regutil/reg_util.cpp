#include "reg_util.h"

#include <tchar.h>//Required header for _tcschr, _tcscpy_s, _tcsdec, _tcsinc, _tcslen, _tcsncpy_s, _tcsninc, _tcspbrk.

#define MAX_LOADSTRING 128

static bool __getRegValue(const HKEY hReg, const LPCSTR szSubkey, const LPCSTR szValue, DWORD dwType, void *pDest, DWORD dwSize);
static bool __setRegValue(const HKEY hReg, const LPCSTR szSubkey, const LPCSTR szValue, DWORD dwType, void *pDest, DWORD dwSize);

bool GetUserValue(const LPCSTR szSubkey, const LPCSTR szValue, DWORD dwType, void* pDest, DWORD dwSize) {
	return __getRegValue(HKEY_CURRENT_USER, szSubkey, szValue, dwType, pDest, dwSize);
}

bool SetUserValue(const LPCSTR szSubkey, const LPCSTR szValue, DWORD dwType, void *pDest, DWORD dwSize) {
	return __setRegValue(HKEY_CURRENT_USER, szSubkey, szValue, dwType, pDest, dwSize);
}

bool GetSysValue(const LPCSTR szSubkey, const LPCSTR szValue, DWORD dwType, void *pDest, DWORD dwSize) {
	//since this is a wrapper function, call the real one to get the data
	return __getRegValue(HKEY_LOCAL_MACHINE, szSubkey, szValue, dwType, pDest, dwSize);
}

bool SetSysValue(const LPCSTR szSubkey, const LPCSTR szValue, DWORD dwType, void *pDest, DWORD dwSize) {
	//since this is a wrapper function, call the real one to get the data
	return __setRegValue(HKEY_LOCAL_MACHINE, szSubkey, szValue, dwType, pDest, dwSize);
}

bool GetCmdLineValue(const LPSTR szArg, LPSTR szDest, size_t nLen) {
	LPCSTR szCmd = NULL;          //pointer to the cmds line
	LPCSTR szTest = NULL;         //worker variable
	bool bRetVal = false;       //return value

	szCmd = GetCommandLine();   //unicode safe

	//validate our data before continuing
	if (((szArg != NULL) && (_tcslen(szArg) > 0)) && ((szCmd != NULL) && (_tcslen(szCmd) > 0))) {
		//look for the / or - delimiters and move back on char in the buffer, this is because we must test
		//for a space before the argument so we can safely assume it's real and not contained in another string
		szTest = _tcspbrk(szCmd, _T("/-"));
		//validate and also make sure this isn't the last char in the string
		if (szTest != NULL && _tcslen(szTest) != 0) {
			//if we have a space before the delimiter, then we have an argument, so test it
			szTest = _tcsdec(szCmd, szTest);
			if (STRING_NMATCH(szTest, _T(" "), 1)) {
				//pass up the space and delimiter to test the argument name
				szTest = _tcsinc(szTest);
				szTest = _tcsinc(szTest);
				if (STRING_NMATCH(szArg, szTest, _tcslen(szArg))) {
					//we have a match, now we need to also check to see to see if the argument has a
					//value associated with it, so check to see if there is an = after it
					szTest = _tcsninc(szTest, _tcslen(szArg));
					if (szTest != NULL && STRING_NMATCH(szTest, _T("="), 1)) {
						//we have one, see what data (until the next space) is there
						szTest = _tcsinc(szTest);
						if (szTest != NULL) {
							LPCSTR szTemp = NULL;
							szTemp = (LPCSTR)_tcschr(szTest, _T(' '));
							if (szTemp != NULL) {
								//take everything the buffer will hold, up until the space
								if (nLen > (size_t)(szTemp - szTest))
									_tcsncpy_s(szDest, nLen, szTest, szTemp - szTest);
								else
									_tcsncpy_s(szDest, nLen, szTest, nLen);
							}
							else {
								 //take everything the buffer will hold
								_tcsncpy_s(szDest, nLen, szTest, nLen);
							}
						}
					}
					bRetVal = true;
				}
			}
		}
	}
	return bRetVal;
}

static bool __getRegValue(const HKEY hReg, const LPCSTR szSubkey, const LPCSTR szValue, DWORD dwType, void *pDest, DWORD dwSize) {
	HKEY hKey = NULL;
	bool bReturn = false;
	DWORD dwRealType = 0;

	//open the key in the registry
	if ((RegOpenKeyEx(hReg, szSubkey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) && (hKey != NULL)) {
		if (pDest != NULL) {
			//do not forget, the buffer size for string data must also include a terminating zero
			if (RegQueryValueEx(hKey, szValue, NULL, &dwRealType, (LPBYTE)pDest, &dwSize) == ERROR_SUCCESS) {
				//here we add our own bit of type safety (b/c the API really doesn't), if the returned type
				//doesn't match the type passed, we wipe the buffer and return false to avoid a casting err
				if (dwRealType != dwType) {
					SecureZeroMemory(pDest, dwSize);
				}
				else {
					bReturn = true;
				}
			}
		}
		//clean up
		RegCloseKey(hKey);
	}
	return bReturn;
}

static bool __setRegValue(const HKEY hReg, const LPCSTR szSubkey, const LPCSTR szValue, DWORD dwType, void *pDest, DWORD dwSize) {
	HKEY hKey = NULL;
	bool bReturn = false;

	//open (or create if it doesn't exist) the key in the registry
	if ((RegCreateKeyEx(hReg, szSubkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) && (hKey != NULL)) {
		if (pDest != NULL) {
			//do not forget, the buffer size for string data must also include a terminating zero
			if (RegSetValueEx(hKey, szValue, 0, dwType, (LPBYTE)pDest, dwSize) == ERROR_SUCCESS)
				bReturn = true;
		}
		//clean up
		RegCloseKey(hKey);
	}
	return bReturn;
}
