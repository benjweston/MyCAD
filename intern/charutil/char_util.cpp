#include "char_util.h"

#include <cwchar>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <string>


using std::stringstream;
using std::wstringstream;
using std::string;
using std::wstring;

const int WCHAR_MAX_COUNT = 16;                                 //max number of string buffers
const int WCHAR_MAX_LENGTH = 2048;                              //max string length per buffer
static wchar_t wchar_wideStr[WCHAR_MAX_COUNT][WCHAR_MAX_LENGTH];//circular buffer for wchar_t*
static char wchar_str[WCHAR_MAX_COUNT][WCHAR_MAX_LENGTH];       //circular buffer for char*
static int wchar_indexWchar = 0;                                //current index of circular buffer
static int wchar_indexChar = 0;                                 //current index of circular buffer

/********************************************************************************************/
/* convert char* string to wchar_t* string													*/
/********************************************************************************************/

const wchar_t* toWchar(const char *src) {
	wchar_indexWchar = (++wchar_indexWchar) % WCHAR_MAX_COUNT;  //circulate index

	//mbstowcs(wchar_wideStr[wchar_indexWchar], src, WCHAR_MAX_LENGTH); //copy string as wide char
	size_t len = wcslen(wchar_wideStr[wchar_indexWchar]);
	mbstowcs_s(&len, wchar_wideStr[wchar_indexWchar], src, WCHAR_MAX_LENGTH); //copy string as wide char

	wchar_wideStr[wchar_indexWchar][WCHAR_MAX_LENGTH - 1] = L'\0';      //in case when source exceeded max length

	return wchar_wideStr[wchar_indexWchar];                     //return string as wide char
}

/********************************************************************************************/
/* convert a number to wchar_t* string														*/
/********************************************************************************************/
const wchar_t* toWchar(double number, int precision) {
	wchar_indexWchar = (++wchar_indexWchar) % WCHAR_MAX_COUNT;  //circulate index

	//convert a number to string
	wstring wstr;
	wstringstream wss;
	if (precision >= 0) {
		wss << std::fixed << std::setprecision(precision);
	}
	wss << number;
	wstr = wss.str();

	wcscpy_s(wchar_wideStr[wchar_indexWchar], wstr.c_str());      //copy it to circular buffer

	//unset floating format
	wss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

	return wchar_wideStr[wchar_indexWchar];                     //return string as wide char
}
const wchar_t* toWchar(float number, int precision) {
	return toWchar((double)number, precision);
}
const wchar_t* toWchar(long number) {
	wchar_indexWchar = (++wchar_indexWchar) % WCHAR_MAX_COUNT;  //circulate index

	//convert a number to string
	wstring wstr;
	wstringstream wss;
	wss << number;
	wstr = wss.str();

	wcscpy_s(wchar_wideStr[wchar_indexWchar], wstr.c_str());      //copy it to circular buffer

	return wchar_wideStr[wchar_indexWchar];                     //return string as wide char
}
const wchar_t* toWchar(int number) {
	return toWchar((long)number);
}

/********************************************************************************************/
/* convert wchar_t* string to char* string													*/
/********************************************************************************************/
const char* toChar(const wchar_t* src) {
	wchar_indexChar = (++wchar_indexChar) % WCHAR_MAX_COUNT;    //circulate index

	//wcstombs(wchar_str[wchar_indexChar], src, WCHAR_MAX_LENGTH);//copy string as char
	size_t len = ::strlen(wchar_str[wchar_indexChar]);
	wcstombs_s(&len, wchar_str[wchar_indexChar], src, WCHAR_MAX_LENGTH);//copy string as char
	wchar_str[wchar_indexChar][WCHAR_MAX_LENGTH - 1] = '\0';      //in case when source exceeded max length

	return wchar_str[wchar_indexChar];                          //return string as char
}

/********************************************************************************************/
/* convert a number to char* string															*/
/********************************************************************************************/
const char* toChar(double number, int precision) {
	wchar_indexChar = (++wchar_indexChar) % WCHAR_MAX_COUNT;    //circulate index

	//convert a number to string
	string s;
	stringstream ss;
	if (precision >= 0) {
		ss << std::fixed << std::setprecision(precision);
	}
	ss << number;
	s = ss.str();

	::strcpy_s(wchar_str[wchar_indexChar], s.c_str());              //copy to circular buffer

	//unset floating format
	ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

	return wchar_str[wchar_indexChar];                          //return string as char
}
const char* toChar(float number, int precision) {
	return toChar((double)number, precision);
}
const char* toChar(long number) {
	wchar_indexChar = (++wchar_indexChar) % WCHAR_MAX_COUNT;    //circulate index

	//convert a number to string
	string s;
	stringstream ss;
	ss << number;
	s = ss.str();

	::strcpy_s(wchar_str[wchar_indexChar], s.c_str());              //copy to circular buffer

	return wchar_str[wchar_indexChar];                          //return string as char
}
const char* toChar(int number) {
	return toChar((long)number);
}

bool parseFloat(const char *in, float& res) {
	try {
		char* end;
		res = strtof(in, &end);
		if (errno == ERANGE) {
			errno = 0;
			return false;
		}
	}
	catch (std::invalid_argument) {
		return false;
	}
	return true;
}
bool parseDouble(const char *in, double& res) {
	try {
		char* end;
		res = strtod(in, &end);
		if (errno == ERANGE) {
			errno = 0;
			return false;
		}
	}
	catch (std::invalid_argument) {
		return false;
	}
	return true;
}

const int toInt(const char *str) { return atoi(str); }

const float toFloat(const char *str) {
	float valuef;
	bool validInput = parseFloat(str, valuef);
	if (validInput == false) {
		return 0.f;
	}
	return valuef; //return (float)atof(chText);
}

const double toDouble(const char *str) {
	double valued;
	bool validInput = parseDouble(str, valued);
	if (validInput == false) {
		return 0.0;
	}
	return valued; //return atof(chText);
}
