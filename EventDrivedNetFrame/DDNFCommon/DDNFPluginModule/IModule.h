#ifndef __IMODULE__H__
#define __IMODULE__H__
#include "DDNFCommon/DDNFPluginModule/IPluginManager.h"

namespace DDNF
{
	class IModule
	{
	public:
		IModule()
		{

		}

		virtual ~IModule() {};

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
			return true;
		}

		virtual IPluginManager* GetPluginManager() const
		{
			return m_pPluginManager;
		}

	protected:
		IPluginManager* m_pPluginManager = 0;

	};
}
#endif// __IMODULE__H__