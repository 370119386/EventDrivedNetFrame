#include "INetModule.h"

using namespace DDNF;

DDNF::INetModule::INetModule(IPluginManager* p)
{
	m_pPluginManager = p;
}

DDNF::INetModule::~INetModule()
{
	m_pPluginManager = NULL;
}