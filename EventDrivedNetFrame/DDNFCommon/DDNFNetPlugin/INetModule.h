#ifndef __I__NET__MODULE__H__
#define __I__NET__MODULE__H__
#include "DDNFCommon/DDNFPluginModule/IModule.h"
#include "DDNFCommon/DDNFPluginModule/IPluginManager.h"

namespace DDNF
{
	class INetModule : public IModule
	{
	public:
		INetModule(IPluginManager* p);
		virtual ~INetModule();

		template<typename BaseType>
		bool AddReceiveCallBack(const int nMsgID, BaseType* pBase, void (BaseType::* handleRecieve)(const NFSOCK, const int, const char*, const uint32_t))
		{
			NET_MESSAGE_HANDLER_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr(new NET_MESSAGE_HANDLER_FUNCTOR(functor));

			return AddReceiveCallBack(nMsgID, functorPtr);
		}

		template<typename BaseType>
		bool AddReceiveCallBack(BaseType* pBase, void (BaseType::* handleRecieve)(const NFSOCK, const int, const char*, const uint32_t))
		{
			NET_MESSAGE_HANDLER_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr(new NET_MESSAGE_HANDLER_FUNCTOR(functor));

			return AddReceiveCallBack(functorPtr);
		}

		virtual bool AddReceiveCallBack(const int nMsgID, const NET_MESSAGE_HANDLER_FUNCTOR_PTR& cb) = 0;
		virtual bool AddReceiveCallBack(const NET_MESSAGE_HANDLER_FUNCTOR_PTR& cb) = 0;

	private:
		std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR> mNetMsgHandlers;
		std::map<int, std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>> mxReceiveCallBack;

	};
}
#endif//__I__NET__MODULE__H__