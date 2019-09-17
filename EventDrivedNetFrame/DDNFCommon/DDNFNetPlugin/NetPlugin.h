#include "DDNFCommon/DDNFPluginModule/IPlugin.h"
#include "DDNFCommon/DDNFPluginModule/IPluginManager.h"

namespace DDNF
{
	class NetPlugin : public IPlugin
	{
	public:
		NetPlugin(IPluginManager* p)
		{
			m_pPluginManager = p;
		}

		virtual int GetPluginVersion() const override;

		virtual std::string GetPluginName() const override;

		virtual void Install() override;

		virtual void Uninstall() override;
	};
}