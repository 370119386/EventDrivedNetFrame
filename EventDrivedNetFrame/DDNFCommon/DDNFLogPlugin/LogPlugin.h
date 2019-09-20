#include "DDNFCommon/DDNFPluginModule/IPlugin.h"
#include "DDNFCommon/DDNFPluginModule/IPluginManager.h"

namespace DDNF
{
	class LogPlugin : public IPlugin
	{
	public:
		LogPlugin(IPluginManager* p)
		{
			m_pPluginManager = p;
		}

		virtual int GetPluginVersion() const;

		virtual std::string GetPluginName() const;

		virtual void Install();

		virtual void Uninstall();
	};
}