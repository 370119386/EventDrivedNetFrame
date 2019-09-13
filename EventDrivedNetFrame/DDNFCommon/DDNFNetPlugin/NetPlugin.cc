#include "NetPlugin.h"
#include "DDNFCommon/DDNFPluginModule/IPlugin.h"
//#include "NFNetModule.h"
//#include "NFNetClientModule.h"
//#include "NFHttpClientModule.h"
//#include "NFHttpServerModule.h"
//#include "NFWSModule.h"
#include "DDNFCommon/DDNFPluginModule/IPluginManager.h"

using namespace DDNF;

#ifdef DDNF_DYNAMIC_PLUGIN

DDNF_EXPORT void DllStartPlugin(DDNF::IPluginManager* pm)
{
    CREATE_PLUGIN(pm, NetPlugin)
};

DDNF_EXPORT void DllStopPlugin(DDNF::IPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NetPlugin)
};

#endif


//////////////////////////////////////////////////////////////////////////

int DDNF::NetPlugin::GetPluginVersion() const
{
    return 0;
}

std::string DDNF::NetPlugin::GetPluginName() const
{
	return GET_CLASS_NAME(NetPlugin);
}

void DDNF::NetPlugin::Install()
{
    //REGISTER_MODULE(pPluginManager, NFINetModule, NFNetModule)
	//REGISTER_MODULE(pPluginManager, NFIWSModule, NFWSModule)
	//REGISTER_MODULE(pPluginManager, NFIHttpServerModule, NFHttpServerModule)
    //REGISTER_MODULE(pPluginManager, NFINetClientModule, NFNetClientModule)
	//REGISTER_MODULE(pPluginManager, NFIHttpClientModule, NFHttpClientModule)
}

void DDNF::NetPlugin::Uninstall()
{
	//UNREGISTER_MODULE(pPluginManager, NFIHttpClientModule, NFHttpClientModule)
    //UNREGISTER_MODULE(pPluginManager, NFINetClientModule, NFNetClientModule)
	//UNREGISTER_MODULE(pPluginManager, NFIHttpServerModule, NFHttpServerModule)
	//UNREGISTER_MODULE(pPluginManager, NFIWSModule, NFWSModule)
    //UNREGISTER_MODULE(pPluginManager, NFINetModule, NFNetModule)
}