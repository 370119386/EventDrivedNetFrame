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

		virtual int GetAppID() const;
		virtual void SetAppID(const int nAppID);

		virtual const std::string& GetConfigPath() const;
		virtual void SetConfigPath(const std::string& rkPath);

		virtual void SetConfigName(const std::string& rkFileName);
		virtual const std::string& GetConfigName() const;

		virtual const std::string& GetAppName() const;
		virtual void SetAppName(const std::string& rkAppName);

		virtual const std::string& GetLogConfigName() const;
		virtual void SetLogConfigName(const std::string& rkName);

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

		int mAppID;

		std::string mkConfigPath;
		std::string mkConfigFileName;
		std::string mkLogConfigName;
		std::string mkAppName;
	};
}
#endif