#pragma once

#include "LIB_objectmanager.h"

namespace mycad {
	//! LayoutManager class.
	/*!
	*  LayoutManager class.
	*  @author B.J.W.
	*/
	class Space;
	class Layout;
	class CommandManager;
	class DataBlocks;
	class LayoutManager : public ObjectManager<Layout> {
	public:
		LayoutManager(CommandManager* commandmanager, DataBlocks* datablocks, SIZE size);

		bool setTilemode(int tilemode);
		int getTilemode();

		BOOL addObject(int index, char* name);

		Layout* setActiveLayout(unsigned int index);//Sets active layout by index, returns active layout
		Layout* setActiveLayout(const char* name);//Sets active layout by name, returns active layout
		Layout* getActiveObject() const override;//Returns active layout

		//Layout* Item(char* name);

		Space* getActiveSpace();

		void size(int width, int height);


	private:
		SIZE m_Size;

		int TILEMODE;//System Variable: PaperSpace == 0, ModelSpace == 1, Default value 1

		CommandManager* m_lpCommandManager;
		DataBlocks* m_lpDataBlocks;

		Layout* m_lpModelLayout;

	};
}
