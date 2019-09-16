#ifndef __DDNF__NET__H__
#define __DDNF__NET__H__
#include "INet.h"
#include <thread>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <event2/event_compat.h>
#include <list>

namespace DDNF
{
	class Net : public INet
	{
	public:
		Net();

		template<typename BaseType>
		Net(BaseType* pBaseType, void (BaseType::* handleRecieve)(const SOCK, const int, const char*, const uint32_t), void (BaseType::* handleEvent)(const SOCK, const NET_EVENT, INet*), bool tcpStream = false)
		{
			mEventBase = NULL;
			listener = NULL;

			mNetMsgHandler = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			mNetEventHandler = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			mIp = "";
			mPort = 0;
			mCpuCount = 0;
			mbServer = false;
			mbWorking = false;

			mMsgDispatchedCount = 0;
			mMsgRecievedCount = 0;

			mBufferSize = 0;
			mbTCPStream = tcpStream;
		}

	public:
		virtual bool Execute() override;
		virtual void Initialization(const char* strIP, const unsigned short nPort);
		virtual int Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount = 4);

	protected:
		int InitClientNet();
		int InitServerNet();

	public:
		virtual int ExpandBufferSize(unsigned int size);
		virtual bool Final();

		virtual bool SendMsg(const char* msg, const size_t nLen, const SOCK nSockIndex);
		virtual bool SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const size_t nLen, const SOCK nSockIndex);
		virtual bool SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const size_t nLen, const std::list<SOCK>& fdList);
		virtual bool SendMsgToAllClientWithOutHead(const int16_t nMsgID, const char* msg, const size_t nLen);

		virtual bool CloseNetObject(const SOCK nSockIndex);
		virtual bool AddNetObject(const SOCK nSockIndex, NetObject* pObject);
		virtual NetObject* GetNetObject(const SOCK nSockIndex);

		virtual bool IsServer();
		virtual bool Log(int severity, const char* msg);

	protected:
		void CloseObject(const SOCK nSockIndex);
		void ExecuteClose();
		bool CloseSocketAll();

	protected:
		static void listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sa, int socklen, void* user_data);
		static void conn_readcb(struct bufferevent* bev, void* user_data);
		static void conn_writecb(struct bufferevent* bev, void* user_data);
		static void conn_eventcb(struct bufferevent* bev, short events, void* user_data);
		static void log_cb(int severity, const char* msg);

		bool Dismantle(NetObject* pObject);

	private:
		int DeCode(const char* strData, const uint32_t unAllLen, MsgHead& xHead);
		int EnCode(const uint16_t unMsgID, const char* strData, const uint32_t unDataLen, std::string& strOutData);

		bool SendMsg(const char* msg, const size_t nLen, const std::list<SOCK>& fdList);
		bool SendMsgToAllClient(const char* msg, const size_t nLen);

	private:
		std::string mIp;
		int mPort;

		struct event_base* mEventBase;
		struct evconnlistener* listener;

		struct evconnlistener* mConnectListner;
		std::map<SOCK, NetObject*> mIndexSocketDic;
		std::vector<SOCK> mNeedRemovedSockes;
		bool mbServer;
		bool mbTCPStream;
		bool mbWorking;
		INT64 mMsgDispatchedCount;
		INT64 mMsgRecievedCount;
		INT32 mBufferSize;
		int mCpuCount;
		int mMaxConnect;

		NET_MESSAGE_HANDLER_FUNCTOR mNetMsgHandler;
		NET_EVENT_HANDLER_FUNCTOR mNetEventHandler;
	};
}
#endif//__DDNF__NET__H__