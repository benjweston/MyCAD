#include "xml_util.h"
#include "fault.h"
#include "log.h"

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { printf("Error: %i\n", a_eResult); return a_eResult; }
#endif

#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)

#define MAX_LOADSTRING 256

using namespace tinyxml2;

HRESULT mycad::xmlutil::getFirstAndLastCharStripped(const char* lpszSource, char* lpszDestination)
{
	HRESULT hResult = S_OK;

	char lpszName[MAX_LOADSTRING]{ 0 };
	const char* lpszName_tmp = lpszSource;

	lpszName_tmp++;
	errno_t err = ::strcpy_s(lpszName, lpszName_tmp);
	if (err != 0) {
		hResult = ErrorHandler();
		return hResult;
	}

	lpszName[::strlen(lpszName) - 1] = 0;

	err = ::strcpy_s(lpszDestination, MAX_LOADSTRING, lpszName);
	if (err != 0) {
		hResult = ErrorHandler();
		return hResult;
	}

	return hResult;
}

//const char* mycad::xmlutil::getPath(const char* indir) {
//	const int MAX_STRING = 256;
//
//	//const char* pchSolutionDir = EXPAND(SOLDIR);
//
//	char chSolutionDir[MAX_STRING], chSolutionDirTemp[MAX_STRING];
//	::strcpy_s(chSolutionDirTemp, indir);//Copy const char* into char[].
//
//	char* substr = chSolutionDirTemp + 1;//Remove the quotes at the beginning of the string.
//
//	char *buf = substr;
//	buf[::strlen(buf) - 2] = '\0';//Remove the quotes and colon at the end of the string.
//
//	::strcpy_s(chSolutionDir, buf);
//
//	char chData[MAX_STRING] = "data\\preferences.xml";
//	::strcat_s(chSolutionDir, chData);
//
//	return chSolutionDir;
//}
XMLError mycad::xmlutil::getXMLColour(XMLElement* element, Colour3<int>& colour) {
	XMLError eResult = XML_SUCCESS;

	Colour3<int>n3Colour{ 255,255,255 };

	XMLElement* lpElement = element->FirstChildElement("Red");
	eResult = lpElement->QueryIntText(&n3Colour.r);
	if (eResult == XML_SUCCESS) {
		colour.r = n3Colour.r;
	}

	lpElement = element->FirstChildElement("Green");
	eResult = lpElement->QueryIntText(&n3Colour.g);
	if (eResult == XML_SUCCESS) {
		colour.g = n3Colour.g;
	}

	lpElement = element->FirstChildElement("Blue");
	eResult = lpElement->QueryIntText(&n3Colour.b);
	if (eResult == XML_SUCCESS) {
		colour.b = n3Colour.b;
	}

	return XML_SUCCESS;
}
XMLError mycad::xmlutil::getXMLInt(XMLElement* element, int& val) {
	XMLError eResult = XML_SUCCESS;
	eResult = element->QueryIntText(&val);
	return eResult;
}
XMLError mycad::xmlutil::getXMLBool(tinyxml2::XMLElement* element, bool& val) {
	XMLError eResult = XML_SUCCESS;
	eResult = element->QueryBoolText(&val);
	return eResult;
}
const char*  mycad::xmlutil::getXMLText(tinyxml2::XMLElement* element) {
	XMLError eResult = XML_SUCCESS;
	enum { MAX_STRING = 256 };
	const char* ch = element->GetText();
	if (ch == 0) {
		eResult = XML_ERROR_PARSING_ATTRIBUTE;
		return "";
	}
	return ch;
}
