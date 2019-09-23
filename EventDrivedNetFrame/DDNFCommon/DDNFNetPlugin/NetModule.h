#ifndef __NET__MODULE__H__
#define __NET__MODULE__H__
#include "DDNFCommon/DDNFPluginModule/INetModule.h"

#ifdef _MSC_VER
#pragma warning(disable: 4244 4267)
#endif

namespace DDNF
{
	class NetModule : public INetModule
	{
	public:
		NetModule(IPluginManager* p);
		virtual ~NetModule();

		virtual bool RegisterNetMsgHandler(const int nMsgID, const NET_MESSAGE_HANDLER_FUNCTOR_PTR& cb);
		virtual bool RegisterNetMsgHandler(const NET_MESSAGE_HANDLER_FUNCTOR_PTR& cb);
		virtual bool RegisterNetEventHandler(const NET_EVENT_HANDLER_FUNCTOR_PTR& cb);

		virtual void UnRegisterNetMsgHandler(const int nMsgID);
		virtual void UnRegisterNetMsgHandler(const NET_MESSAGE_HANDLER_FUNCTOR_PTR& cb);
		virtual void UnRegisterNetEventHandler(const NET_EVENT_HANDLER_FUNCTOR_PTR& cb);

		virtual void InitAsClient(const char* strIP, const unsigned short nPort);
		virtual int InitAsServer(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount = 4);
		virtual int ExpandBufferSize(const unsigned int size = 1024 * 1024 * 20);

		virtual INet* GetNet();

		virtual bool Finalize() override;

	protected:
		void OnHandleNetMsg(const SOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
		void OnHandleNetEvent(const SOCK nSockIndex, const NET_EVENT eEvent, INet* pNet);

	private:
		INet* m_pNet;
		int m_bufferSize;
		INT64 nLastTime;
		std::map<int, std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>*> m_NetMsgIdHandlerImpl;
		std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR> m_kNetMsgImpl;
		std::list<NET_EVENT_HANDLER_FUNCTOR_PTR> m_kNetEventImpl;

	};
}
#endif//__NET__MODULE__H__