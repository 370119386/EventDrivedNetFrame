#ifndef __IPLUGIN__H__
#define __IPLUGIN__H__
#include <string>

#include "IModule.h"
#include "DDNFCommon/DDNFCore/Map.hpp"

#define CREATE_PLUGIN(pManager, className)  IPlugin* pCreatePlugin##className = new className(pManager); pManager->Registered( pCreatePlugin##className );
#define DESTROY_PLUGIN(pManager, className) pManager->UnRegistered( pManager->FindPlugin((#className)) );

namespace DDNF
{
	class IPlugin : public IModule,
		public Map<std::string,IModule>
	{
	public:
		IPlugin()
		{

		}

		virtual ~IPlugin() {};

		virtual int GetPluginVersion() const = 0;

		virtual std::string GetPluginName() const = 0;

		virtual void Install() = 0;

		virtual bool Awake()
		{
			return true;
		}

		virtual bool Init()
		{
			return true;
		}

		virtual bool AfterInit()
		{
			return true;
		}

		virtual bool CheckConfig()
		{
			return true;
		}

		virtual bool ReadyExecute()
		{
			return true;
		}

		virtual bool Execute()
		{
			return true;
		}

		virtual bool BeforeShut()
		{
			return true;
		}

		virtual bool Shut()
		{
			return true;
		}

		virtual bool Finalize()
		{
			return true;
		}

		virtual bool OnReloadPlugin()
		{
			IModule* pModule = First();
			while (pModule)
			{
				//pPluginManager->SetCurrenModule(pModule);
				pModule->OnReloadPlugin();
				pModule = Next();
			}

			return true;
		}

		virtual void Uninstall() = 0;
	};
}
#endif// __IPLUGIN__H__