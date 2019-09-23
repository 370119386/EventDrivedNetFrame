#ifndef __DDNF__NET__CLIENT__MODULE__H__
#define __DDNF__NET__CLIENT__MODULE__H__

#include <iostream>
#include <iosfwd>
#include <queue.h>
//#include "NFComm/NFCore/NFQueue.hpp"
//#include "NFComm/NFCore/NFConsistentHash.hpp"
//#include "NFComm/NFMessageDefine/NFDefine.pb.h"
#include "DDNFCommon/DDNFPluginModule/INetClientModule.h"
#include "DDNFCommon/DDNFPluginModule/INetModule.h"
#include "DDNFCommon/DDNFPluginModule/ILogModule.h"
#include "DDNFCommon/DDNFPluginModule/IPluginManager.h"

namespace DDNF
{
	class NetClientModule : public INetClientModule
	{
	public:
		NetClientModule(IPluginManager* p);

		virtual bool Init();

		virtual bool AfterInit();

		virtual bool BeforeShut();

		virtual bool Shut();

		virtual bool Execute();

		virtual void AddServer(const ConnectData& xInfo);

		virtual int ExpandBufferSize(const unsigned int size = 1024 * 1024 * 20);

		virtual int RegisterNetMsgHandler(const SERVER_TYPES eType, NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr);

		virtual int RegisterNetMsgHandler(const SERVER_TYPES eType, const uint16_t nMsgID, NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr);

		virtual int RegisterNetEventHandler(const SERVER_TYPES eType, NET_EVENT_HANDLER_FUNCTOR_PTR functorPtr);

		virtual void UnRegisterNetMsgHandler(const SERVER_TYPES eType, const uint16_t nMsgID);

		////////////////////////////////////////////////////////////////////////////////
		virtual void SendByServerIDWithOutHead(const int nServerID, const uint16_t nMsgID, const std::string& strData);

		virtual void SendByServerID(const int nServerID, const uint16_t nMsgID, const std::string& strData);
		virtual void SendByServerID(const int nServerID, const uint16_t nMsgID, const std::string& strData, const NFGUID id);

		virtual void SendToAllServerWithOutHead(const uint16_t nMsgID, const std::string& strData);

		virtual void SendToAllServer(const uint16_t nMsgID, const std::string& strData);
		//virtual void SendToAllServer(const uint16_t nMsgID, const std::string& strData, const NFGUID id);

		virtual void SendToAllServerWithOutHead(const SERVER_TYPES eType, const uint16_t nMsgID, const std::string& strData);

		virtual void SendToAllServer(const SERVER_TYPES eType, const uint16_t nMsgID, const std::string& strData);
		//virtual void SendToAllServer(const SERVER_TYPES eType, const uint16_t nMsgID, const std::string& strData, const NFGUID id);

		virtual void SendToServerByPB(const int nServerID, const uint16_t nMsgID, const google::protobuf::Message& xData);
		//virtual void SendToServerByPB(const int nServerID, const uint16_t nMsgID, const google::protobuf::Message& xData, const NFGUID id);

		virtual void SendToAllServerByPB(const uint16_t nMsgID, const google::protobuf::Message& xData);

		virtual void SendToAllServerByPB(const SERVER_TYPES eType, const uint16_t nMsgID, const google::protobuf::Message& xData);

		////////////////////////////////////////////////////////////////////////////////

		virtual void SendBySuitWithOutHead(const SERVER_TYPES eType, const std::string& strHashKey, const uint16_t nMsgID, const std::string& strData);

		virtual void SendBySuit(const SERVER_TYPES eType, const std::string& strHashKey, const uint16_t nMsgID, const std::string& strData);
		//virtual void SendBySuit(const SERVER_TYPES eType, const std::string& strHashKey, const uint16_t nMsgID, const std::string& strData, const NFGUID id);

		virtual void SendBySuitWithOutHead(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const std::string& strData);

		virtual void SendBySuit(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const std::string& strData);
		//virtual void SendBySuit(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const std::string& strData, const NFGUID id);

		virtual void SendSuitByPB(const SERVER_TYPES eType, const std::string& strHashKey, const uint16_t nMsgID, const google::protobuf::Message& xData);
		//virtual void SendSuitByPB(const SERVER_TYPES eType, const std::string& strHashKey, const uint16_t nMsgID, const google::protobuf::Message& xData, const NFGUID id);

		virtual void SendSuitByPB(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const google::protobuf::Message& xData);
		//virtual void SendSuitByPB(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const google::protobuf::Message& xData, const NFGUID id);

		////////////////////////////////////////////////////////////////////////////////

		virtual Map<int, ConnectData>& GetServerList();

		virtual DDNF_SHARE_PTR<ConnectData> GetServerNetInfo(const SERVER_TYPES eType);

		virtual DDNF_SHARE_PTR<ConnectData> GetServerNetInfo(const int nServerID);

		virtual DDNF_SHARE_PTR<ConnectData> GetServerNetInfo(const INet* pNet);

	protected:

		void InitCallBacks(DDNF_SHARE_PTR<ConnectData> pServerData);

		void ProcessExecute();

	private:
		void LogServerInfo();

		void KeepState(DDNF_SHARE_PTR<ConnectData> pServerData);

		void OnSocketEvent(const SOCK fd, const NET_EVENT eEvent, INet* pNet);

		int OnConnected(const SOCK fd, INet* pNet);

		int OnDisConnected(const SOCK fd, INet* pNet);

		void ProcessAddNetConnect();

	private:
		int64_t mnLastActionTime;
		int64_t mnBufferSize;
		//server_id, server_data
		Map<int, ConnectData> mxServerMap;
		//server_type, server_id, server_data
		Map<int, Map<int, ConnectData>> mxServerTypeMap;

		std::list<ConnectData> mxTempNetList;

		struct CallBack
		{
			//call back
			//std::map<int, NET_MESSAGE_HANDLER_FUNCTOR_PTR> mxReceiveCallBack;
			std::map<int, std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>> mxReceiveCallBack;
			std::list<NET_EVENT_HANDLER_FUNCTOR_PTR> mxEventCallBack;
			std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR> mxCallBackList;
		};

		Map<int, CallBack> mxCallBack;

		ILogModule* m_pLogModule;
	};
}

#endif//__DDNF__NET__CLIENT__MODULE__H__
