#include "libobjrw.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cassert>
#include "intern/drw_textcodec.h"
//#include "intern/dxfreader.h"
//#include "intern/dxfwriter.h"
#include "intern/drw_dbg.h"

objRW::objRW(const char* name) {
	DRW_DBGSL(DRW_dbg::LEVEL::None);
	fileName = name;
	//reader = NULL;
	//writer = NULL;
	//applyExt = false;
	//elParts = 128; //parts munber when convert ellipse to polyline
}
objRW::~objRW() {
	//if (reader != NULL)
	//	delete reader;
	//if (writer != NULL)
	//	delete writer;
	//for (std::vector<DRW_ImageDef*>::iterator it = imageDef.begin(); it != imageDef.end(); ++it)
	//	delete *it;

	//imageDef.clear();
}

void objRW::setDebug(DRW::DBG_LEVEL lvl) {
	switch (lvl) {
	case DRW::DBG_LEVEL::DEBUG:
		DRW_DBGSL(DRW_dbg::LEVEL::DEBUG);
		break;
	default:
		DRW_DBGSL(DRW_dbg::LEVEL::None);
	}
}

bool objRW::read(DRW_Interface *interface_, bool ext) {
	drw_assert(fileName.empty() == false);
	bool isOk = false;
	applyExt = ext;
	std::ifstream filestr;
	//if (interface_ == NULL)
	//	return isOk;
	//DRW_DBG("dxfRW::read 1def\n");
	//filestr.open(fileName.c_str(), std::ios_base::in | std::ios::binary);
	//if (!filestr.is_open())
	//	return isOk;
	//if (!filestr.good())
	//	return isOk;

	//char line[22];
	//char line2[22] = "AutoCAD Binary DXF\r\n";
	//line2[20] = (char)26;
	//line2[21] = '\0';
	//filestr.read(line, 22);
	//filestr.close();
	//iface = interface_;
	//DRW_DBG("dxfRW::read 2\n");
	//if (strcmp(line, line2) == 0) {
	//	filestr.open(fileName.c_str(), std::ios_base::in | std::ios::binary);
	//	binFile = true;
	//	//skip sentinel
	//	filestr.seekg(22, std::ios::beg);
	//	reader = new dxfReaderBinary(&filestr);
	//	DRW_DBG("dxfRW::read binary file\n");
	//}
	//else {
	//	binFile = false;
	//	filestr.open(fileName.c_str(), std::ios_base::in);
	//	reader = new dxfReaderAscii(&filestr);
	//}

	//isOk = processDxf();
	filestr.close();
	//delete reader;
	//reader = NULL;
	return isOk;
}
