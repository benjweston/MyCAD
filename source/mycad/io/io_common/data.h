#pragma once

#include "libdxfrw.h"

namespace mycad {
	//class to store image data and path from DRW_ImageDef
	class DRW_DataImg : public DRW_Image {
	public:
		DRW_DataImg() {}
		DRW_DataImg(const DRW_Image& p) :DRW_Image(p) {}
		~DRW_DataImg() {}
		std::string path; //stores the image path
	};

	//container class to store entites.
	class DRW_DataBlock : public DRW_Block {
	public:
		DRW_DataBlock() {}
		DRW_DataBlock(const DRW_Block& p) :DRW_Block(p) {}
		~DRW_DataBlock() {
			for (std::list<DRW_Entity*>::const_iterator it = ent.begin(); it != ent.end(); ++it) {
				delete* it;
			}
		}
		std::list<DRW_Entity*>ent; //stores the entities list
	};

	//container class to store full dwg/dxf data.
	class DRW_Data {
	public:
		DRW_Data() {
			mBlock = new DRW_DataBlock();
		}
		virtual ~DRW_Data() {
			//cleanup,
			for (std::list<DRW_DataBlock*>::const_iterator it = blocks.begin(); it != blocks.end(); ++it) {
				delete* it;
			}
			delete mBlock;
		}

		DRW_Header headerC;                 //stores a copy of the header vars
		DRW_DataBlock* mBlock;              //container to store model entities
		std::list<DRW_DataBlock*>blocks;    //stores a copy of all blocks and contained entities
		std::list<DRW_Dimstyle>dimStyles;   //stores a copy of all dimension styles
		std::list<DRW_Layer>layers;         //stores a copy of all layers
		std::list<DRW_LType>lineTypes;      //stores a copy of all line types
		std::list<DRW_Textstyle>textStyles; //stores a copy of all text styles
		std::list<DRW_Vport>VPorts;         //stores a copy of all vports
		std::list<DRW_AppId>appIds;
		std::list<DRW_DataImg*>images;      //temporary list to find images for link with DRW_ImageDef. Do not delete it!!

	};
}
