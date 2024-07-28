#include "io.h"

#include <iostream>
#include <algorithm>

#include <locale>
#include <codecvt>

using namespace mycad;

bool Interface::fileImport(const std::string& fileI, DRW_Data *fData){
    unsigned int found = (unsigned int)fileI.find_last_of(".");
    std::string fileExt = fileI.substr(found+1);
    std::transform(fileExt.begin(), fileExt.end(),fileExt.begin(), ::toupper);
    cData = fData;
    currentBlock = cData->mBlock;

    if (fileExt == "DXF"){//loads dxf
        dxfRW* dxf = new dxfRW(fileI.c_str());
        bool success = dxf->read(this, false);
        delete dxf;
        return success;
    }
	else if (fileExt == "DWG"){//loads dwg
        dwgR* dwg = new dwgR(fileI.c_str());
        bool success = dwg->read(this, false);
        delete dwg;
        return success;
	}
	else if (fileExt == "OBJ") {//loads obj
		objRW* obj = new objRW(fileI.c_str());
		bool success = obj->read(this, false);
		delete obj;
		return success;
    }
    std::cout << "file extension can be dxf or dwg" << std::endl;
    return false;
}
//bool Interface::fileImport(const std::wstring& wfileI, DRW_Data* fData) {
//	using convert_type = std::codecvt_utf8<wchar_t>;
//	std::wstring_convert<convert_type, wchar_t> converter;
//	std::string fileI = converter.to_bytes(wfileI);
//
//	unsigned int found = (unsigned int)fileI.find_last_of(".");
//	std::string fileExt = fileI.substr(found + 1);
//	std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::toupper);
//	cData = fData;
//	currentBlock = cData->mBlock;
//
//	if (fileExt == "DXF") {//loads dxf
//		dxfRW* dxf = new dxfRW(fileI.c_str());
//		bool success = dxf->read(this, false);
//		delete dxf;
//		return success;
//	}
//	else if (fileExt == "DWG") {//loads dwg
//		dwgR* dwg = new dwgR(fileI.c_str());
//		bool success = dwg->read(this, false);
//		delete dwg;
//		return success;
//	}
//	else if (fileExt == "OBJ") {//loads obj
//		objRW* obj = new objRW(fileI.c_str());
//		bool success = obj->read(this, false);
//		delete obj;
//		return success;
//	}
//	std::cout << "file extension can be dxf or dwg" << std::endl;
//	return false;
//}
bool Interface::fileExport(const std::string& file, DRW::Version v, bool binary, DRW_Data *fData){
    cData = fData;
    dxfW = new dxfRW(file.c_str());
    bool success = dxfW->write(this, v, binary);
    delete dxfW;
    return success;
}

void Interface::writeEntity(DRW_Entity* ent){
    switch (ent->eType) {
		case DRW::ETYPE::ARC: {
			dxfW->writeArc(static_cast<DRW_Arc*>(ent));
			break;
		}
		case DRW::ETYPE::CIRCLE: {
			dxfW->writeCircle(static_cast<DRW_Circle*>(ent));
			break;
		}
		case DRW::ETYPE::DIMLINEAR:
		case DRW::ETYPE::DIMALIGNED:
		case DRW::ETYPE::DIMANGULAR:
		case DRW::ETYPE::DIMANGULAR3P:
		case DRW::ETYPE::DIMRADIAL:
		case DRW::ETYPE::DIMDIAMETRIC:
		case DRW::ETYPE::DIMORDINATE: {
			dxfW->writeDimension(static_cast<DRW_Dimension*>(ent));
			break;
		}
		case DRW::ETYPE::ELLIPSE: {
			dxfW->writeEllipse(static_cast<DRW_Ellipse*>(ent));
			break;
		}
		case DRW::ETYPE::HATCH: {
			dxfW->writeHatch(static_cast<DRW_Hatch*>(ent));
			break;
		}
		case DRW::ETYPE::IMAGE: {
			dxfW->writeImage(static_cast<DRW_Image*>(ent), static_cast<DRW_DataImg*>(ent)->path);
			break;
		}
		case DRW::ETYPE::INSERT: {
			dxfW->writeInsert(static_cast<DRW_Insert*>(ent));
			break;
		}
		case DRW::ETYPE::LEADER: {
			dxfW->writeLeader(static_cast<DRW_Leader*>(ent));
			break;
		}
		case DRW::ETYPE::LINE: {
			dxfW->writeLine(static_cast<DRW_Line*>(ent));
			break;
		}
		case DRW::ETYPE::LWPOLYLINE: {
			dxfW->writeLWPolyline(static_cast<DRW_LWPolyline*>(ent));
			break;
		}
		case DRW::ETYPE::MTEXT: {
			dxfW->writeMText(static_cast<DRW_MText*>(ent));
			break;
		}
		case DRW::ETYPE::POINT: {
			dxfW->writePoint(static_cast<DRW_Point*>(ent));
			break;
		}
		case DRW::ETYPE::POLYLINE: {
			dxfW->writePolyline(static_cast<DRW_Polyline*>(ent));
			break;
		}
		case DRW::ETYPE::RAY: {
			dxfW->writeRay(static_cast<DRW_Ray*>(ent));
			break;
		}
		case DRW::ETYPE::SOLID: {
			dxfW->writeSolid(static_cast<DRW_Solid*>(ent));
			break;
		}
		case DRW::ETYPE::SPLINE: {
			dxfW->writeSpline(static_cast<DRW_Spline*>(ent));
			break;
		}
		case DRW::ETYPE::TEXT: {
			dxfW->writeText(static_cast<DRW_Text*>(ent));
			break;
		}
		case DRW::ETYPE::XLINE: {
			dxfW->writeXline(static_cast<DRW_Xline*>(ent));
			break;
		}
		default: {
			break;
		}
    }
}