#include <assert.h>
#include "PluginManager.h"
#include "DDNFCommon/DDNFPluginModule/IModule.h"
#include "DDNFCommon/DDNFPluginModule/IPlugin.h"
#include "DynLib.h"
#include <iostream>

using namespace DDNF;

PluginManager::PluginManager()
{
	mPluginNameDic.clear();
	mPluginLibDic.clear();
	mPluginHandleDic.clear();
}

PluginManager::~PluginManager()
{
	
}

bool DDNF::PluginManager::LoadPlugin()
{
	LoadConfig();

	for(auto& itr : mPluginNameDic)
	{
		LoadPluginLibrary(itr.first);
	}

	return true;
}

bool DDNF::PluginManager::Awake()
{
	for (PluginInstanceMap::const_iterator itr = mPluginHandleDic.begin(); itr != mPluginHandleDic.end(); ++itr)
	{
		if (nullptr != itr->second)
		{
			itr->second->Awake();
		}
	}
	return true;
}

bool DDNF::PluginManager::Init()
{
	for (PluginInstanceMap::const_iterator itr = mPluginHandleDic.begin(); itr != mPluginHandleDic.end(); ++itr)
	{
		if (nullptr != itr->second)
		{
			itr->second->Init();
		}
	}
	return true;
}

bool DDNF::PluginManager::AfterInit()
{
	for (PluginInstanceMap::const_iterator itr = mPluginHandleDic.begin(); itr != mPluginHandleDic.end(); ++itr)
	{
		if (nullptr != itr->second)
		{
			itr->second->AfterInit();
		}
	}
	return true;
}

bool DDNF::PluginManager::CheckConfig()
{
	for (PluginInstanceMap::const_iterator itr = mPluginHandleDic.begin(); itr != mPluginHandleDic.end(); ++itr)
	{
		if (nullptr != itr->second)
		{
			itr->second->CheckConfig();
		}
	}
	return true;
}

bool DDNF::PluginManager::ReadyExecute()
{
	for (PluginInstanceMap::const_iterator itr = mPluginHandleDic.begin(); itr != mPluginHandleDic.end(); ++itr)
	{
		if (nullptr != itr->second)
		{
			itr->second->ReadyExecute();
		}
	}
	return true;
}

bool DDNF::PluginManager::Execute()
{
	for (PluginInstanceMap::const_iterator itr = mPluginHandleDic.begin(); itr != mPluginHandleDic.end(); ++itr)
	{
		if (nullptr != itr->second)
		{
			itr->second->Execute();
		}
	}
	return true;
}

bool DDNF::PluginManager::BeforeShut()
{
	for (PluginInstanceMap::const_iterator itr = mPluginHandleDic.begin(); itr != mPluginHandleDic.end(); ++itr)
	{
		if (nullptr != itr->second)
		{
			itr->second->BeforeShut();
		}
	}
	return true;
}

bool DDNF::PluginManager::Shut()
{
	for (PluginInstanceMap::const_iterator itr = mPluginHandleDic.begin(); itr != mPluginHandleDic.end(); ++itr)
	{
		if (nullptr != itr->second)
		{
			itr->second->Shut();
		}
	}
	return true;
}

bool DDNF::PluginManager::Finalize()
{
	for (PluginInstanceMap::const_iterator itr = mPluginHandleDic.begin(); itr != mPluginHandleDic.end(); ++itr)
	{
		if (nullptr != itr->second)
		{
			itr->second->Finalize();
		}
	}

	for (PluginNameMap::const_iterator itr = mPluginNameDic.begin(); itr != mPluginNameDic.end(); ++itr)
	{
		UnLoadPluginLibrary(itr->first);
	}

	mPluginHandleDic.clear();
	mPluginNameDic.clear();

	return true;
}

void DDNF::PluginManager::Registered(IPlugin* plugin)
{
	const std::string& strPluginName = plugin->GetPluginName();
	if (mPluginHandleDic.find(strPluginName) == mPluginHandleDic.end())
	{
		mPluginHandleDic.insert(PluginInstanceMap::value_type(strPluginName, plugin));
		plugin->Install();
	}
	else
	{
		std::cout << strPluginName << std::endl;
		assert(0);
	}
}

void DDNF::PluginManager::UnRegistered(IPlugin* plugin)
{
	PluginInstanceMap::iterator it = mPluginHandleDic.find(plugin->GetPluginName());
	if (it != mPluginHandleDic.end())
	{
		it->second->Uninstall();
		delete it->second;
		it->second = NULL;
		mPluginHandleDic.erase(it);
	}
}

bool DDNF::PluginManager::ReLoadPlugin(const std::string & strPluginDLLName)
{
	//1.shut all module of this plugin
	//2.unload this plugin
	//3.load new plugin
	//4.init new module
	//5.tell others who has been reloaded
	PluginInstanceMap::iterator itInstance = mPluginHandleDic.find(strPluginDLLName);
	if (itInstance == mPluginHandleDic.end())
	{
		return false;
	}
	//1
	IPlugin* pPlugin = itInstance->second;
	IModule* pModule = pPlugin->First();
	while (pModule)
	{
		pModule->BeforeShut();
		pModule->Shut();
		pModule->Finalize();

		pModule = pPlugin->Next();
	}

	//2
	PluginLibMap::iterator it = mPluginLibDic.find(strPluginDLLName);
	if (it != mPluginLibDic.end())
	{
		DynLib* pLib = it->second;
		DLL_STOP_PLUGIN_FUNC pFunc = (DLL_STOP_PLUGIN_FUNC)pLib->GetSymbol("DllStopPlugin");

		if (pFunc)
		{
			pFunc(this);
		}

		pLib->UnLoad();

		delete pLib;
		pLib = NULL;
		mPluginLibDic.erase(it);
	}

	//3
	DynLib* pLib = new DynLib(strPluginDLLName);
	bool bLoad = pLib->Load();
	if (bLoad)
	{
		mPluginLibDic.insert(PluginLibMap::value_type(strPluginDLLName, pLib));

		DLL_START_PLUGIN_FUNC pFunc = (DLL_START_PLUGIN_FUNC)pLib->GetSymbol("DllStartPlugin");
		if (!pFunc)
		{
			std::cout << "Reload Find function DllStartPlugin Failed in [" << pLib->GetName() << "]" << std::endl;
			assert(0);
			return false;
		}

		pFunc(this);
	}
	else
	{
#if DDNF_PLATFORM == DDNF_PLATFORM_LINUX
		char* error = dlerror();
		if (error)
		{
			std::cout << stderr << " Reload shared lib[" << pLib->GetName() << "] failed, ErrorNo. = [" << error << "]" << std::endl;
			std::cout << "Reload [" << pLib->GetName() << "] failed" << std::endl;
			assert(0);
			return false;
		}
#elif DDNF_PLATFORM == DDNF_PLATFORM_WIN
		std::cout << stderr << " Reload DLL[" << pLib->GetName() << "] failed, ErrorNo. = [" << GetLastError() << "]" << std::endl;
		std::cout << "Reload [" << pLib->GetName() << "] failed" << std::endl;
		assert(0);
		return false;
#endif // NF_PLATFORM
	}

	//4
	PluginInstanceMap::iterator itReloadInstance = mPluginHandleDic.begin();
	for (; itReloadInstance != mPluginHandleDic.end(); itReloadInstance++)
	{
		if (strPluginDLLName != itReloadInstance->first)
		{
			itReloadInstance->second->OnReloadPlugin();
		}
	}
	return true;
}

IPlugin* DDNF::PluginManager::FindPlugin(const std::string& strPluginName)
{
	PluginInstanceMap::const_iterator itr = mPluginHandleDic.find(strPluginName);
	if (itr != mPluginHandleDic.end())
	{
		return itr->second;
	}

	return NULL;
}

IModule* DDNF::PluginManager::FindModule(const std::string& strModuleName)
{
	return NULL;
}

void DDNF::PluginManager::LoadConfig()
{
	mPluginNameDic.insert(PluginNameMap::value_type("NetPlugin",true));
}

bool DDNF::PluginManager::LoadPluginLibrary(const std::string& strPluginDLLName)
{
	PluginLibMap::iterator it = mPluginLibDic.find(strPluginDLLName);
	if (it == mPluginLibDic.end())
	{
		DynLib* pLib = new DynLib(strPluginDLLName);
		bool bLoad = pLib->Load();

		if (bLoad)
		{
			std::cout << stdout << "Load shared lib [" << pLib->GetName() << "] succeed ..." << std::endl;
			mPluginLibDic.insert(PluginLibMap::value_type(strPluginDLLName, pLib));

			DLL_START_PLUGIN_FUNC pFunc = (DLL_START_PLUGIN_FUNC)pLib->GetSymbol("DllStartPlugin");
			if (!pFunc)
			{
				std::cout << "Find function DllStartPlugin Failed in [" << pLib->GetName() << "]" << std::endl;
				assert(0);
				return false;
			}

			pFunc(this);

			return true;
		}
		else
		{
#if DDNF_PLATFORM == DDNF_PLATFORM_LINUX || DDNF_PLATFORM == DDNF_PLATFORM_APPLE
			char* error = dlerror();
			if (error)
			{
				std::cout << stderr << " Load shared lib[" << pLib->GetName() << "] failed, ErrorNo. = [" << error << "]" << std::endl;
				std::cout << "Load [" << pLib->GetName() << "] failed" << std::endl;
				assert(0);
				return false;
			}
#elif DDNF_PLATFORM == DDNF_PLATFORM_WIN
			std::cout << stderr << " Load DLL[" << pLib->GetName() << "] failed, ErrorNo. = [" << GetLastError() << "]" << std::endl;
			std::cout << "Load [" << pLib->GetName() << "] failed" << std::endl;
			assert(0);
			return false;
#endif // EM_PLATFORM
		}
	}

	return false;
}

bool DDNF::PluginManager::UnLoadPluginLibrary(const std::string& strPluginDLLName)
{
	PluginLibMap::iterator it = mPluginLibDic.find(strPluginDLLName);
	if (it != mPluginLibDic.end())
	{
		DynLib* pLib = it->second;
		DLL_STOP_PLUGIN_FUNC pFunc = (DLL_STOP_PLUGIN_FUNC)pLib->GetSymbol("DllStopPlugin");

		if (pFunc)
		{
			pFunc(this);
		}

		pLib->UnLoad();

		delete pLib;
		pLib = 0;
		mPluginLibDic.erase(it);

		return true;
	}

	return false;
}