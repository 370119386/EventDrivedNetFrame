#ifndef __DDNF__I_CLUSTER__CLIENT__MODULE__H__
#define __DDNF__I_CLUSTER__CLIENT__MODULE__H__

#include <iostream>
#include "IModule.h"
#include "INetModule.h"
#include "DDNFCommon/DDNFCore/Map.hpp"

namespace DDNF
{
	enum ConnectDataState
	{
		DISCONNECT,
		CONNECTING,
		NORMAL,
		RECONNECT,
	};

	struct ConnectData
	{
		ConnectData()
		{
			nGameID = 0;
			nPort = 0;
			strName = "";
			strIP = "";
			eServerType = ST_NONE;
			eState = ConnectDataState::DISCONNECT;
			mnLastActionTime = 0;
			nWorkLoad = 0;
		}

		int nGameID;
		SERVER_TYPES eServerType;
		std::string strIP;
		int nPort;
		int nWorkLoad;
		std::string strName;
		ConnectDataState eState;
		INT64 mnLastActionTime;

		DDNF_SHARE_PTR<INetModule> mxNetModule;
	};

	class INetClientModule : public IModule
	{
	public:
		enum EConstDefine
		{
			EConstDefine_DefaultWeith = 500,
		};

		template<typename BaseType>
		bool RegisterNetMsgHandler(const SERVER_TYPES eType, const uint16_t nMsgID, BaseType* pBase, void (BaseType::* handleRecieve)(const SOCK, const int, const char*, const uint32_t))
		{
			NET_MESSAGE_HANDLER_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr(new NET_MESSAGE_HANDLER_FUNCTOR(functor));

			RegisterNetMsgHandler(eType, nMsgID, functorPtr);

			return true;
		}

		template<typename BaseType>
		int RegisterNetMsgHandler(const SERVER_TYPES eType, BaseType* pBase, void (BaseType::* handleRecieve)(const SOCK, const int, const char*, const uint32_t))
		{
			NET_MESSAGE_HANDLER_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr(new NET_MESSAGE_HANDLER_FUNCTOR(functor));

			RegisterNetMsgHandler(eType, functorPtr);

			return true;
		}

		template<typename BaseType>
		bool RegisterNetEventHandler(const SERVER_TYPES eType, BaseType* pBase, void (BaseType::* handler)(const SOCK, const NET_EVENT, INet*))
		{
			NET_MESSAGE_HANDLER_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr(new NET_MESSAGE_HANDLER_FUNCTOR(functor));

			RegisterNetEventHandler(eType, functorPtr);

			return true;
		}

		virtual int RegisterNetMsgHandler(const SERVER_TYPES eType, NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr) = 0;
		virtual int RegisterNetMsgHandler(const SERVER_TYPES eType, const uint16_t nMsgID, NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr) = 0;
		virtual int RegisterNetEventHandler(const SERVER_TYPES eType, NET_EVENT_HANDLER_FUNCTOR_PTR functorPtr) = 0;

		virtual void UnRegisterNetMsgHandler(const SERVER_TYPES eType, const uint16_t nMsgID) = 0;
		////////////////////////////////////////////////////////////////////////////////

		virtual void AddServer(const ConnectData& xInfo) = 0;
		virtual int ExpandBufferSize(const unsigned int size = 1024 * 1024 * 20) = 0;

		////////////////////////////////////////////////////////////////////////////////
		virtual void SendByServerIDWithOutHead(const int nServerID, const uint16_t nMsgID, const std::string& strData) = 0;

		virtual void SendByServerID(const int nServerID, const uint16_t nMsgID, const std::string& strData) = 0;
		//virtual void SendByServerID(const int nServerID, const uint16_t nMsgID, const std::string& strData, const NFGUID id) = 0;

		virtual void SendToAllServerWithOutHead(const uint16_t nMsgID, const std::string& strData) = 0;

		virtual void SendToAllServer(const uint16_t nMsgID, const std::string& strData) = 0;
		//virtual void SendToAllServer(const uint16_t nMsgID, const std::string& strData, const NFGUID id) = 0;

		virtual void SendToAllServerWithOutHead(const SERVER_TYPES eType, const uint16_t nMsgID, const std::string& strData) = 0;

		virtual void SendToAllServer(const SERVER_TYPES eType, const uint16_t nMsgID, const std::string& strData) = 0;
		//virtual void SendToAllServer(const SERVER_TYPES eType, const uint16_t nMsgID, const std::string& strData, const NFGUID id) = 0;

		virtual void SendToServerByPB(const int nServerID, const uint16_t nMsgID, const google::protobuf::Message& xData) = 0;
		//virtual void SendToServerByPB(const int nServerID, const uint16_t nMsgID, const google::protobuf::Message& xData, const NFGUID id) = 0;

		virtual void SendToAllServerByPB(const uint16_t nMsgID, const google::protobuf::Message& xData) = 0;

		virtual void SendToAllServerByPB(const SERVER_TYPES eType, const uint16_t nMsgID, const google::protobuf::Message& xData) = 0;

		////////////////////////////////////////////////////////////////////////////////

		virtual void SendBySuitWithOutHead(const SERVER_TYPES eType, const std::string& strHashKey, const uint16_t nMsgID, const std::string& strData) = 0;

		virtual void SendBySuit(const SERVER_TYPES eType, const std::string& strHashKey, const uint16_t nMsgID, const std::string& strData) = 0;
		//virtual void SendBySuit(const SERVER_TYPES eType, const std::string& strHashKey, const uint16_t nMsgID, const std::string& strData, const NFGUID id) = 0;

		virtual void SendBySuitWithOutHead(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const std::string& strData) = 0;

		virtual void SendBySuit(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const std::string& strData) = 0;
		//virtual void SendBySuit(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const std::string& strData, const NFGUID id) = 0;

		virtual void SendSuitByPB(const SERVER_TYPES eType, const std::string& strHashKey, const uint16_t nMsgID, const google::protobuf::Message& xData) = 0;
		//virtual void SendSuitByPB(const SERVER_TYPES eType, const std::string& strHashKey, const uint16_t nMsgID, const google::protobuf::Message& xData, const NFGUID id) = 0;

		virtual void SendSuitByPB(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const google::protobuf::Message& xData) = 0;
		//virtual void SendSuitByPB(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const google::protobuf::Message& xData, const NFGUID id) = 0;

		////////////////////////////////////////////////////////////////////////////////

		virtual Map<int, ConnectData>& GetServerList() = 0;
		virtual DDNF_SHARE_PTR<ConnectData> GetServerNetInfo(const SERVER_TYPES eType) = 0;
		virtual DDNF_SHARE_PTR<ConnectData> GetServerNetInfo(const int nServerID) = 0;
		virtual DDNF_SHARE_PTR<ConnectData> GetServerNetInfo(const INet* pNet) = 0;
	};
}
#endif//__DDNF__I_CLUSTER__CLIENT__MODULE__H__
