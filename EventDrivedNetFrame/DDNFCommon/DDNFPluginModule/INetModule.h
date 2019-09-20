#ifndef __I__NET__MODULE__H__
#define __I__NET__MODULE__H__
#include "DDNFCommon/DDNFPluginModule/IModule.h"
#include "DDNFCommon/DDNFPluginModule/IPluginManager.h"
#include "DDNFCommon/DDNFNetPlugin/INet.h"
#include <list>

namespace DDNF
{
	class INetModule : public IModule
	{
	public:
		template<typename BaseType>
		bool RegisterNetMsgHandler(const int nMsgID, BaseType* pBase, void (BaseType::* handleRecieve)(const SOCK, const int, const char*, const uint32_t))
		{
			NET_MESSAGE_HANDLER_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr(new NET_MESSAGE_HANDLER_FUNCTOR(functor));

			return RegisterNetMsgHandler(nMsgID, functorPtr);
		}

		template<typename BaseType>
		bool RegisterNetMsgHandler(BaseType* pBase, void (BaseType::* handleRecieve)(const SOCK, const int, const char*, const uint32_t))
		{
			NET_MESSAGE_HANDLER_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr(new NET_MESSAGE_HANDLER_FUNCTOR(functor));

			return RegisterNetMsgHandler(functorPtr);
		}

		virtual bool RegisterNetMsgHandler(const int nMsgID, const NET_MESSAGE_HANDLER_FUNCTOR_PTR& cb) = 0;
		virtual bool RegisterNetMsgHandler(const NET_MESSAGE_HANDLER_FUNCTOR_PTR& cb) = 0;
		virtual bool RegisterNetEventHandler(const NET_EVENT_HANDLER_FUNCTOR_PTR& cb) = 0;

		virtual void UnRegisterNetMsgHandler(const int nMsgID) = 0;
		virtual void UnRegisterNetMsgHandler(const NET_MESSAGE_HANDLER_FUNCTOR_PTR& cb) = 0;
		virtual void UnRegisterNetEventHandler(const NET_EVENT_HANDLER_FUNCTOR_PTR& cb) = 0;

		virtual void InitAsClient(const char* strIP, const unsigned short nPort) = 0;
		virtual int InitAsServer(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount = 4) = 0;
		virtual int ExpandBufferSize(const unsigned int size = 1024 * 1024 * 20) = 0;
	};
}
#endif//__I__NET__MODULE__H__