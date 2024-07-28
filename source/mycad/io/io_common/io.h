#pragma once

#include "libdxfrw.h"
#include "libdwgr.h"
#include "libobjrw.h"
#include "drw_interface.h"
#include "data.h"

namespace mycad {
	/**
	* Declaration of Interface class.
	*/
	class Interface : public DRW_Interface {
	public:
		Interface()
			: cData(0),
			currentBlock(0)
		{
			dxfW = NULL;
		}

		~Interface() {}

		bool fileImport(const std::string& fileI, DRW_Data* fData);
		//bool fileImport(const std::wstring& wfileI, DRW_Data* fData);
		bool fileExport(const std::string& file, DRW::Version v, bool binary, DRW_Data* fData);

		void writeEntity(DRW_Entity* ent);

		//reimplement virtual DRW_Interface functions

		//reader part, stores all in class dx_data
			//header
		void addHeader(const DRW_Header* data) {
			cData->headerC = *data;
		}

		//tables
		void addLType(const DRW_LType& data) {
			cData->lineTypes.push_back(data);
		}
		void addLayer(const DRW_Layer& data) {
			cData->layers.push_back(data);
		}
		void addDimStyle(const DRW_Dimstyle& data) {
			cData->dimStyles.push_back(data);
		}
		void addVport(const DRW_Vport& data) {
			cData->VPorts.push_back(data);
		}
		void addTextStyle(const DRW_Textstyle& data) {
			cData->textStyles.push_back(data);
		}
		void addAppId(const DRW_AppId& data) {
			cData->appIds.push_back(data);
		}

		//blocks
		void addBlock(const DRW_Block& data) {
			DRW_DataBlock* bk = new DRW_DataBlock(data);
			currentBlock = bk;
			cData->blocks.push_back(bk);
		}
		void endBlock() {
			currentBlock = cData->mBlock;
		}

		void setBlock(const int /*handle*/) {}//unused

		//entities
		void addPoint(const DRW_Point& data) {
			currentBlock->ent.push_back(new DRW_Point(data));
		}
		void addLine(const DRW_Line& data) {
			currentBlock->ent.push_back(new DRW_Line(data));
		}
		void addRay(const DRW_Ray& data) {
			currentBlock->ent.push_back(new DRW_Ray(data));
		}
		void addXline(const DRW_Xline& data) {
			currentBlock->ent.push_back(new DRW_Xline(data));
		}
		void addArc(const DRW_Arc& data) {
			currentBlock->ent.push_back(new DRW_Arc(data));
		}
		void addCircle(const DRW_Circle& data) {
			currentBlock->ent.push_back(new DRW_Circle(data));
		}
		void addEllipse(const DRW_Ellipse& data) {
			currentBlock->ent.push_back(new DRW_Ellipse(data));
		}
		void addLWPolyline(const DRW_LWPolyline& data) {
			currentBlock->ent.push_back(new DRW_LWPolyline(data));
		}
		void addPolyline(const DRW_Polyline& data) {
			currentBlock->ent.push_back(new DRW_Polyline(data));
		}
		void addSpline(const DRW_Spline* data) {
			currentBlock->ent.push_back(new DRW_Spline(*data));
		}
		//¿para que se usa?
		void addKnot(const DRW_Entity&) {}

		void addInsert(const DRW_Insert& data) {
			currentBlock->ent.push_back(new DRW_Insert(data));
		}
		void addTrace(const DRW_Trace& data) {
			currentBlock->ent.push_back(new DRW_Trace(data));
		}
		void add3dFace(const DRW_3Dface& data) {
			currentBlock->ent.push_back(new DRW_3Dface(data));
		}
		void addSolid(const DRW_Solid& data) {
			currentBlock->ent.push_back(new DRW_Solid(data));
		}
		void addMText(const DRW_MText& data) {
			currentBlock->ent.push_back(new DRW_MText(data));
		}
		void addText(const DRW_Text& data) {
			currentBlock->ent.push_back(new DRW_Text(data));
		}
		void addDimAlign(const DRW_DimAligned* data) {
			currentBlock->ent.push_back(new DRW_DimAligned(*data));
		}
		void addDimLinear(const DRW_DimLinear* data) {
			currentBlock->ent.push_back(new DRW_DimLinear(*data));
		}
		void addDimRadial(const DRW_DimRadial* data) {
			currentBlock->ent.push_back(new DRW_DimRadial(*data));
		}
		void addDimDiametric(const DRW_DimDiametric* data) {
			currentBlock->ent.push_back(new DRW_DimDiametric(*data));
		}
		void addDimAngular(const DRW_DimAngular* data) {
			currentBlock->ent.push_back(new DRW_DimAngular(*data));
		}
		void addDimAngular3P(const DRW_DimAngular3p* data) {
			currentBlock->ent.push_back(new DRW_DimAngular3p(*data));
		}
		void addDimOrdinate(const DRW_DimOrdinate* data) {
			currentBlock->ent.push_back(new DRW_DimOrdinate(*data));
		}
		void addLeader(const DRW_Leader* data) {
			currentBlock->ent.push_back(new DRW_Leader(*data));
		}
		void addHatch(const DRW_Hatch* data) {
			currentBlock->ent.push_back(new DRW_Hatch(*data));
		}
		void addViewport(const DRW_Viewport& data) {
			currentBlock->ent.push_back(new DRW_Viewport(data));
		}
		void addImage(const DRW_Image* data) {
			DRW_DataImg* img = new DRW_DataImg(*data);
			currentBlock->ent.push_back(new DRW_DataImg(*data));
			cData->images.push_back(img);
		}

		void linkImage(const DRW_ImageDef* data) {
			duint32 handle = data->handle;
			std::string path(data->name);
			for (std::list<DRW_DataImg*>::iterator it = cData->images.begin(); it != cData->images.end(); ++it) {
				if ((*it)->ref == handle) {
					DRW_DataImg* img = *it;
					img->path = path;
				}
			}
		}

		//writer part, send all in class dx_data to writer
		void addComment(const char* /*cmdsent*/) {}

		void writeHeader(DRW_Header& data) {
			//complete copy of header vars:
			data = cData->headerC;
			//or copy one by one:
	//       for (std::map<std::string,DRW_Variant*>::iterator it=cData->headerC.vars.begin(); it != cData->headerC.vars.end(); ++it)
	//           data.vars[it->first] = new DRW_Variant( *(it->second) );
		}

		void writeBlocks() {
			//write each block
			for (std::list<DRW_DataBlock*>::iterator blk = cData->blocks.begin(); blk != cData->blocks.end(); ++blk) {
				DRW_DataBlock* bk = *blk;
				dxfW->writeBlock(bk);
				//and write each entity in block
				for (std::list<DRW_Entity*>::const_iterator ent = bk->ent.begin(); ent != bk->ent.end(); ++ent)
					writeEntity(*ent);
			}
		}
		//only send the name, needed by the reader to prepare handles of blocks & blockRecords
		void writeBlockRecords() {
			for (std::list<DRW_DataBlock*>::iterator it = cData->blocks.begin(); it != cData->blocks.end(); ++it)
				dxfW->writeBlockRecord((*it)->name);
		}
		//write entities of model space and first paper_space
		void writeEntities() {
			for (std::list<DRW_Entity*>::const_iterator it = cData->mBlock->ent.begin(); it != cData->mBlock->ent.end(); ++it)
				writeEntity(*it);
		}
		void writeLTypes() {
			for (std::list<DRW_LType>::iterator it = cData->lineTypes.begin(); it != cData->lineTypes.end(); ++it)
				dxfW->writeLineType(&(*it));
		}
		void writeLayers() {
			for (std::list<DRW_Layer>::iterator it = cData->layers.begin(); it != cData->layers.end(); ++it)
				dxfW->writeLayer(&(*it));
		}
		void writeTextstyles() {
			for (std::list<DRW_Textstyle>::iterator it = cData->textStyles.begin(); it != cData->textStyles.end(); ++it)
				dxfW->writeTextstyle(&(*it));
		}
		void writeVports() {
			for (std::list<DRW_Vport>::iterator it = cData->VPorts.begin(); it != cData->VPorts.end(); ++it)
				dxfW->writeVport(&(*it));
		}
		void writeDimstyles() {
			for (std::list<DRW_Dimstyle>::iterator it = cData->dimStyles.begin(); it != cData->dimStyles.end(); ++it)
				dxfW->writeDimstyle(&(*it));
		}
		void writeAppId() {
			for (std::list<DRW_AppId>::iterator it = cData->appIds.begin(); it != cData->appIds.end(); ++it)
				dxfW->writeAppId(&(*it));
		}

		dxfRW* dxfW; //pointer to writer, needed to send data
		DRW_Data* cData; //class to store or read data
		DRW_DataBlock* currentBlock;
	};
}
