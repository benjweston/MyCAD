#pragma once

#include "tinyxml2.h"
#include "LIB_colours.h"

#include <windows.h>

namespace mycad {
	namespace xmlutil {
		/**
		Strips leading and trailing \\" characters from xml strings.
		\param lpszSource: pointer to a char array containing the source text.
		\param lpszDestination: pointer to a char array used to store the altered text
		*/
		HRESULT getFirstAndLastCharStripped(const char* lpszSource, char* lpszDestination);

		//const char* getPath(const char* indir);
		tinyxml2::XMLError getXMLColour(tinyxml2::XMLElement* element, Colour3<int>& colour);
		tinyxml2::XMLError getXMLInt(tinyxml2::XMLElement* element, int& val);
		tinyxml2::XMLError getXMLBool(tinyxml2::XMLElement* element, bool& val);
		const char* getXMLText(tinyxml2::XMLElement* element);
	}
}
