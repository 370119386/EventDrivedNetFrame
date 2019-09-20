#include "LogPlugin.h"
#include "LogModule.h"

using namespace DDNF;

#ifdef DDNF_DYNAMIC_PLUGIN

DDNF_EXPORT void DllStartPlugin(IPluginManager* pm)
{
    CREATE_PLUGIN(pm, LogPlugin)
};

DDNF_EXPORT void DllStopPlugin(IPluginManager* pm)
{
    DESTROY_PLUGIN(pm, LogPlugin)
};

#endif

//////////////////////////////////////////////////////////////////////////

const int LogPlugin::GetPluginVersion()
{
    return 0;
}

const std::string LogPlugin::GetPluginName()
{
	return GET_CLASS_NAME(LogPlugin);
}

void NFLogPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, NFILogModule, NFLogModule)
}

void NFLogPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, NFILogModule, NFLogModule)
}