#ifndef __PLUGIN__MANAGER__H__
#define __PLUGIN__MANAGER__H__
#include "DDNFCommon/DDNFPluginModule/Platform.h"
#include "DDNFCommon/DDNFCore/Singleton.hpp"
#include "DDNFCommon/DDNFPluginModule/IPluginManager.h"
#include <map>
#include <string>

namespace DDNF
{
	class IPlugin;
	class DynLib;
	class PluginManager : public IPluginManager,public Singleton<PluginManager>
	{
	public:
		PluginManager();
		~PluginManager();

		bool LoadPlugin();
		bool Awake();
		bool Init();
		bool AfterInit();
		bool CheckConfig();
		bool ReadyExecute();
		bool Execute();
		bool BeforeShut();
		bool Shut();
		bool Finalize();

	public:
		void Registered(IPlugin* plugin);
		void UnRegistered(IPlugin* plugin);
		bool ReLoadPlugin(const std::string & strPluginDLLName);
		IPlugin* FindPlugin(const std::string& strPluginName);
		IModule* FindModule(const std::string& strModuleName);

		virtual INT64 GetInitTime() const override;
		virtual INT64 GetNowTime() const override;

	protected:
		bool LoadPluginLibrary(const std::string& strPluginDLLName);
		bool UnLoadPluginLibrary(const std::string& strPluginDLLName);
		void LoadConfig();

	private:
		typedef std::map<std::string, bool> PluginNameMap;
		typedef std::map<std::string, DynLib*> PluginLibMap;
		typedef std::map<std::string, IPlugin*> PluginInstanceMap;

		typedef void(*DLL_START_PLUGIN_FUNC)(PluginManager* pm);
		typedef void(*DLL_STOP_PLUGIN_FUNC)(PluginManager* pm);

	private:
		PluginNameMap mPluginNameDic;
		PluginLibMap mPluginLibDic;
		PluginInstanceMap mPluginHandleDic;

		INT64 mInitTime;
		INT64 mNowTime;
	};
}
#endif