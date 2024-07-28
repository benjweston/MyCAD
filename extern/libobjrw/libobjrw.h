#ifndef LIBOBJRW_H
#define LIBOBJRW_H

#include <string>
#include "drw_entities.h"
#include "drw_objects.h"
#include "drw_header.h"
#include "drw_interface.h"

//class dxfReader;
//class dxfWriter;

class objRW {
public:
	objRW(const char* name);
	~objRW();
	void setDebug(DRW::DBG_LEVEL lvl);
	///reads the file specified in constructor
	/*!
	 * An interface must be provided. It is used by the class to signal various
	 * components being added.
	 * @param interface_ the interface to use
	 * @param ext should the extrusion be applied to convert in 2D?
	 * @return true for success
	 */
	bool read(DRW_Interface *interface_, bool ext);
	void setBinary(bool b) { binFile = b; }

private:
	DRW::Version version;
	std::string fileName;
	std::string codePage;
	bool binFile;
	//dxfReader *reader;
	//dxfWriter *writer;
	DRW_Interface *iface;
	DRW_Header header;
	//   int section;
	std::string nextentity;
	int entCount;
	bool wlayer0;
	bool dimstyleStd;
	bool applyExt;
	bool writingBlock;
	int elParts;  /*!< parts munber when convert ellipse to polyline */
	std::map<std::string, int> blockMap;
	std::vector<DRW_ImageDef*> imageDef;  /*!< imageDef list */

	int currHandle;

};

#endif //LIBOBJRW_H
