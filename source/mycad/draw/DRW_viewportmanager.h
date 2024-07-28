#pragma once

#include "LIB_objectmanager.h"

#include "DRW_view.h"
#include "LIB_colours.h"

namespace mycad {
	/**
	* Declaration of ViewportManager class.
	*/
	class Splitter;
	class MViewport;
	class ViewportManager : public ObjectManager<MViewport> {
	public:
		ViewportManager();
		virtual ~ViewportManager();

		enum class Configuration : int {
			ActiveModelConfiguration,
			One,
			TwoVertical,
			TwoHorizontal,
			ThreeRight,
			ThreeLeft,
			ThreeAbove,
			ThreeBelow,
			ThreeVertical,
			ThreeHorizontal,
			FourEqual,
			FourRight,
			FourLeft
		};

		enum class Context : int {
			Display,
			CurrentViewport
		};

		enum class Setup : int {
			_2D,
			_3D
		};

		typedef struct tagViewportData {
			tagViewportData() {}
			tagViewportData(ViewportManager::Configuration configuration, ViewportManager::Context context, ViewportManager::Setup setup, View::VisualStyle visualstyle)
				: enConfiguration(configuration), enContext(context), enSetup(setup), enVisualStyle(visualstyle) {}
			ViewportManager::Configuration enConfiguration{ ViewportManager::Configuration::ActiveModelConfiguration };
			ViewportManager::Context enContext{ ViewportManager::Context::Display };
			ViewportManager::Setup enSetup{ ViewportManager::Setup::_2D };
			View::VisualStyle enVisualStyle{ View::VisualStyle::Wireframe2D };
		} VIEWPORTDATA, *LPVIEWPORTDATA;

		virtual void setViewportConfiguration(Configuration configuration);
		virtual Configuration getViewportConfiguration() const;

		virtual const std::vector<MViewport*> &getViewports() const;
		virtual const std::vector<Splitter*> &getSplitters() const;

		virtual void size(long x, long y, long width, long height);
		virtual void size(POINT position, SIZE dimensions);
		virtual void size(long width, long height);
		virtual void size(SIZE dimensions);

	private:
		Configuration m_enViewportConfiguration;
		std::vector<Splitter*> m_Splitters;

	};
}
