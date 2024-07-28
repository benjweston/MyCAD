#include "DRW_layer.h"

#include "log.h"

using namespace mycad;

Layer::Layer()
{
	log("Log::Layer::Layer()");
	::strcpy_s(m_szName, "Layer0");
}
Layer::Layer(const char *name)
{
	log("Log::Layer::Layer()");
	::strcpy_s(m_szName, name);
}
Layer::~Layer()
{

}
