#pragma once

#include "LIB_objectmanager.h"

#include "CMD_commands.h"

#include <map>

namespace mycad {
	/**
	* Declaration of CommandManager class.
	*/
	class CommandLine;
	class DynamicInput;
	class TextWindow;
	class CommandManager : public ObjectManager<Command> {
	public:
		CommandManager();
		~CommandManager() override;

		static std::vector<COMMANDINFO> s_vecCommands;
		static std::map<int, COMMANDINFO> s_mCommandByID;
		static std::map<std::string, int> s_mCommandIDByAlias;
		static std::map<std::string, int> s_mCommandIDByName;
		static std::map<std::string, int> s_mCommandIDByMacro;

		static int getIDByText(const char* text);
		static void setObjectHistory(unsigned int commandhistory);

		BOOL addObject(Command* command) override;

		Command* getActiveObject() const override;
		Command* getActiveObject(CommandContext context) const;
		BOOL setActiveObject(Command* command, CommandContext context);

		void setObjectInactive();
		void setObjectInactive(CommandContext context);
		void setObjectInactive(const Command* command, CommandContext context);

		LRESULT sendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		static unsigned int s_nObjectSize;

		static int getIDByPartialText(const char* text);

		enum { MAX_LOADSTRING = 256 };

		Command* m_lpActiveViewObject;

		POINT m_ptMouseMove;

		size_t m_cch;
		char* m_pchInputBuf;
		char m_ch;

		void wm_char_default(WPARAM wParam);

	};
}
