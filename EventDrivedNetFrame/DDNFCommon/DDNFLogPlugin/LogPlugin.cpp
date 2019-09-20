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

int LogPlugin::GetPluginVersion() const
{
    return 0;
}

std::string LogPlugin::GetPluginName() const
{
	return GET_CLASS_NAME(LogPlugin);
}

void LogPlugin::Install()
{
    //REGISTER_MODULE(pPluginManager, NFILogModule, NFLogModule)
}

void LogPlugin::Uninstall()
{
    //UNREGISTER_MODULE(pPluginManager, NFILogModule, NFLogModule)
}