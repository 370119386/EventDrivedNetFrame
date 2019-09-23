#include "Net.h"
#include "NetModule.h"
#include "NetClientModule.h"
#include "common/crc32.hpp"

using namespace DDNF;

NetClientModule::NetClientModule(IPluginManager* p)
{
    mnBufferSize = 0;
    m_pPluginManager = p;

	mnLastActionTime = GetPluginManager()->GetNowTime();
}

bool DDNF::NetClientModule::Init()
{
	//m_pLogModule = m_pPluginManager->FindModule<NFILogModule>();

    for (int i = 0; i < SERVER_TYPES::ST_MAX; ++i)
    {
        INetClientModule::RegisterNetEventHandler((SERVER_TYPES) i, this, &NetClientModule::OnSocketEvent);
    }

    return true;
}

bool DDNF::NetClientModule::AfterInit()
{


    return true;
}

bool DDNF::NetClientModule::BeforeShut()
{
    return true;
}

bool DDNF::NetClientModule::Shut()
{
    return true;
}

bool DDNF::NetClientModule::Execute()
{
    ProcessExecute();
    ProcessAddNetConnect();

	if (mnLastActionTime + 10 > GetPluginManager()->GetNowTime())
	{
		return true;
	}

	mnLastActionTime = GetPluginManager()->GetNowTime();

	LogServerInfo();

    return true;
}

void DDNF::NetClientModule::UnRegisterNetMsgHandler(const SERVER_TYPES eType, const uint16_t nMsgID)
{
    DDNF_SHARE_PTR<CallBack> xCallBack = mxCallBack.GetElement(eType);
    if (xCallBack)
    {
        std::map<int, std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>>::iterator it = xCallBack->mxReceiveCallBack.find(nMsgID);
        if (xCallBack->mxReceiveCallBack.end() == it)
        {
            xCallBack->mxReceiveCallBack.erase(it);
        }
    }
}

void DDNF::NetClientModule::AddServer(const ConnectData& xInfo)
{
    mxTempNetList.push_back(xInfo);
}

int DDNF::NetClientModule::ExpandBufferSize(const unsigned int size)
{
    if (size > 0)
    {
        mnBufferSize = size;
    }
    return mnBufferSize;
}

int DDNF::NetClientModule::RegisterNetMsgHandler(const SERVER_TYPES eType, const uint16_t nMsgID,
                                           NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr)
{
    DDNF_SHARE_PTR<CallBack> xCallBack = mxCallBack.GetElement(eType);
    if (!xCallBack)
    {
        xCallBack = DDNF_SHARE_PTR<CallBack>(DDNF_NEW CallBack);
        mxCallBack.AddElement(eType, xCallBack);
    }

	std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR> xList;
	xList.push_back(functorPtr);

    xCallBack->mxReceiveCallBack.insert(std::map<int, std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>>::value_type(nMsgID, xList));

    return 0;
}

int DDNF::NetClientModule::RegisterNetMsgHandler(const SERVER_TYPES eType, NET_MESSAGE_HANDLER_FUNCTOR_PTR functorPtr)
{
    DDNF_SHARE_PTR<CallBack> xCallBack = mxCallBack.GetElement(eType);
    if (!xCallBack)
    {
        xCallBack = DDNF_SHARE_PTR<CallBack>(DDNF_NEW CallBack);
        mxCallBack.AddElement(eType, xCallBack);
    }

    xCallBack->mxCallBackList.push_back(functorPtr);

    return 0;
}


int DDNF::NetClientModule::RegisterNetEventHandler(const SERVER_TYPES eType, NET_EVENT_HANDLER_FUNCTOR_PTR functorPtr)
{
    DDNF_SHARE_PTR<CallBack> xCallBack = mxCallBack.GetElement(eType);
    if (!xCallBack)
    {
        xCallBack = DDNF_SHARE_PTR<CallBack>(DDNF_NEW CallBack);
        mxCallBack.AddElement(eType, xCallBack);
    }

    xCallBack->mxEventCallBack.push_back(functorPtr);
    return 0;
}

void DDNF::NetClientModule::SendByServerIDWithOutHead(const int nServerID, const uint16_t nMsgID, const std::string & strData)
{
	DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);
	if (pServer)
	{
		DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
		if (pNetModule.get())
		{
			if (!pNetModule->SendMsgWithOutHead(nMsgID, strData, 0))
			{
				std::ostringstream stream;
				stream << " SendMsgWithOutHead failed " << nServerID;
				stream << " msg id " << nMsgID;
				m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
			}
		}
	}
	else
	{
		std::ostringstream stream;
		stream << " can't find the server " << nServerID;
		stream << " msg id " << nMsgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
	}
}

void DDNF::NetClientModule::SendByServerID(const int nServerID, const uint16_t nMsgID, const std::string& strData)
{
	DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);
	if (pServer)
	{
		DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
		if (pNetModule.get())
		{
			if (!pNetModule->SendMsg(nMsgID, strData, 0))
			{
				std::ostringstream stream;
				stream << " SendMsgWithOutHead failed " << nServerID;
				stream << " msg id " << nMsgID;
				m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
			}
		}
	}
	else
	{
		std::ostringstream stream;
		stream << " can't find the server " << nServerID;
		stream << " msg id " << nMsgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
	}
}

void DDNF::NetClientModule::SendByServerID(const int nServerID, const uint16_t nMsgID, const std::string & strData, const NFGUID id)
{
	DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);
	if (pServer)
	{
		DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
		if (pNetModule.get())
		{
			if (!pNetModule->SendMsg(nMsgID, strData, 0, id))
			{
				std::ostringstream stream;
				stream << " SendMsgWithOutHead failed " << nServerID;
				stream << " msg id " << nMsgID;
				m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
			}
		}
	}
	else
	{
		std::ostringstream stream;
		stream << " can't find the server " << nServerID;
		stream << " msg id " << nMsgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
	}
}

void DDNF::NetClientModule::SendToAllServerWithOutHead(const uint16_t nMsgID, const std::string & strData)
{
	DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
	while (pServer)
	{
		DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
		if (pNetModule)
		{
			if (!pNetModule->SendMsgWithOutHead(nMsgID, strData, 0))
			{
				std::ostringstream stream;
				stream << " SendMsgWithOutHead failed " << pServer->nGameID;
				stream << " msg id " << nMsgID;
				m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
			}
		}

		pServer = mxServerMap.Next();
	}
}

void DDNF::NetClientModule::SendToAllServer(const uint16_t nMsgID, const std::string& strData)
{
    DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
    while (pServer)
    {
        DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule)
        {
			if (!pNetModule->SendMsg(nMsgID, strData, 0))
			{
				std::ostringstream stream;
				stream << " SendMsgWithOutHead failed " << pServer->nGameID;
				stream << " msg id " << nMsgID;
				m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
			}
        }

        pServer = mxServerMap.Next();
    }
}

//void NetClientModule::SendToAllServer(const uint16_t nMsgID, const std::string & strData, const NFGUID id)
//{
//	DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
//	while (pServer)
//	{
//		DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
//		if (pNetModule)
//		{
//			if (!pNetModule->SendMsg(nMsgID, strData, 0, id))
//			{
//				std::ostringstream stream;
//				stream << " SendMsgWithOutHead failed " << pServer->nGameID;
//				stream << " msg id " << nMsgID;
//				m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
//			}
//		}
//
//		pServer = mxServerMap.Next();
//	}
//}

void DDNF::NetClientModule::SendToAllServerWithOutHead(const SERVER_TYPES eType, const uint16_t nMsgID, const std::string & strData)
{
	DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
	while (pServer)
	{
		DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
		if (pNetModule && eType == pServer->eServerType)
		{
			if (!pNetModule->SendMsgWithOutHead(nMsgID, strData, 0))
			{
				std::ostringstream stream;
				stream << " SendMsgWithOutHead failed " << pServer->nGameID;
				stream << " msg id " << nMsgID;
				m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
			}
		}

		pServer = mxServerMap.Next();
	}
}

void DDNF::NetClientModule::SendToAllServer(const SERVER_TYPES eType, const uint16_t nMsgID, const std::string& strData)
{
    DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
    while (pServer)
    {
        DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule && eType == pServer->eServerType)
        {
			if (!pNetModule->SendMsg(nMsgID, strData, 0))
			{
				std::ostringstream stream;
				stream << " SendMsgWithOutHead failed " << pServer->nGameID;
				stream << " msg id " << nMsgID;
				m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
			}
        }

        pServer = mxServerMap.Next();
    }
}

//void NetClientModule::SendToAllServer(const SERVER_TYPES eType, const uint16_t nMsgID, const std::string & strData, const NFGUID id)
//{
//	DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
//	while (pServer)
//	{
//		DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
//		if (pNetModule && eType == pServer->eServerType)
//		{
//			if (!pNetModule->SendMsg(nMsgID, strData, 0, id))
//			{
//				std::ostringstream stream;
//				stream << " SendMsgWithOutHead failed " << pServer->nGameID;
//				stream << " msg id " << nMsgID;
//				m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
//			}
//		}
//
//		pServer = mxServerMap.Next();
//	}
//}

void DDNF::NetClientModule::SendToServerByPB(const int nServerID, const uint16_t nMsgID, const google::protobuf::Message& xData)
{
    DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);
    if (pServer)
    {
        DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule)
        {
			//if (!pNetModule->SendMsgPB(nMsgID, xData, 0))
			//{
			//	std::ostringstream stream;
			//	stream << " SendMsgPB failed " << pServer->nGameID;
			//	stream << " msg id " << nMsgID;
			//	m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
		 //  }
        }
    }
	else
	{
		std::ostringstream stream;
		stream << " can't find the server " << nServerID;
		stream << " msg id " << nMsgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
	}
}

//void NetClientModule::SendToServerByPB(const int nServerID, const uint16_t nMsgID, const google::protobuf::Message & xData, const NFGUID id)
//{
//	DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);
//	if (pServer)
//	{
//		DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
//		if (pNetModule)
//		{
//			if (!pNetModule->SendMsgPB(nMsgID, xData, 0, id))
//			{
//				std::ostringstream stream;
//				stream << " SendMsgPB failed " << pServer->nGameID;
//				stream << " msg id " << nMsgID;
//				m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
//			}
//		}
//	}
//	else
//	{
//		std::ostringstream stream;
//		stream << " can't find the server " << nServerID;
//		stream << " msg id " << nMsgID;
//		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
//	}
//}

void DDNF::NetClientModule::SendToAllServerByPB(const uint16_t nMsgID, const google::protobuf::Message& xData)
{
    DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
    while (pServer)
    {
        DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule)
        {
			//if (!pNetModule->SendMsgPB(nMsgID, xData, 0))
			//{
			//	std::ostringstream stream;
			//	stream << " SendMsgPB failed " << pServer->nGameID;
			//	stream << " msg id " << nMsgID;
			//	m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
			//}
        }

        pServer = mxServerMap.Next();
    }
}

void DDNF::NetClientModule::SendToAllServerByPB(const SERVER_TYPES eType, const uint16_t nMsgID,
                                             const google::protobuf::Message& xData)
{
    DDNF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
    while (pServer)
    {
        DDNF_SHARE_PTR<INetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule && eType == pServer->eServerType && pServer->eState == ConnectDataState::NORMAL)
        {
   //         if (!pNetModule->SendMsgPB(nMsgID, xData, 0))
			//{
			//	std::ostringstream stream;
			//	stream << " SendMsgPB failed " << pServer->nGameID;
			//	stream << " msg id " << nMsgID;
			//	m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
			//}
        }

        pServer = mxServerMap.Next();
    }
}

void DDNF::NetClientModule::SendBySuitWithOutHead(const SERVER_TYPES eType, const std::string & strHashKey, const uint16_t nMsgID, const std::string & strData)
{
	uint32_t nCRC32 = DDNF::CRC32(strHashKey);
	SendBySuitWithOutHead(eType, nCRC32, nMsgID, strData);
}

void DDNF::NetClientModule::SendBySuit(const SERVER_TYPES eType, const std::string& strHashKey, const uint16_t nMsgID,
                                    const std::string& strData)
{
    uint32_t nCRC32 = DDNF::CRC32(strHashKey);
    SendBySuit(eType, nCRC32, nMsgID, strData);
}

//void NetClientModule::SendBySuit(const SERVER_TYPES eType, const std::string & strHashKey, const uint16_t nMsgID, const std::string & strData, const NFGUID id)
//{
//	uint32_t nCRC32 = NFrame::CRC32(strHashKey);
//	SendBySuit(eType, nCRC32, nMsgID, strData, id);
//}

void DDNF::NetClientModule::SendBySuitWithOutHead(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const std::string & strData)
{
	//DDNF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxServerTypeMap.GetElement(eType);
	//if (xConnectDataMap)
	//{
	//	DDNF_SHARE_PTR<ConnectData> pConnectData = xConnectDataMap->GetElementBySuit(nHashKey32);
	//	if (pConnectData)
	//	{
	//		SendByServerIDWithOutHead(pConnectData->nGameID, nMsgID, strData);
	//	}
	//}
	//else
	//{
	//	std::ostringstream stream;
	//	stream << " can't find the server type " << eType;
	//	stream << " msg id " << nMsgID;
	//	m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
	//}
}

void DDNF::NetClientModule::SendBySuit(const SERVER_TYPES eType, const int nHashKey, const uint16_t nMsgID, const std::string& strData)
{
	//DDNF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxServerTypeMap.GetElement(eType);
	//if (xConnectDataMap)
	//{
	//	DDNF_SHARE_PTR<ConnectData> pConnectData = xConnectDataMap->GetElementBySuit(nHashKey);
	//	if (pConnectData)
	//	{
	//		SendByServerID(pConnectData->nGameID, nMsgID, strData);
	//	}
	//}
	//else
	//{
	//	std::ostringstream stream;
	//	stream << " can't find the server type " << eType;
	//	stream << " msg id " << nMsgID;
	//	m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
	//}
}

//void NetClientModule::SendBySuit(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const std::string & strData, const NFGUID id)
//{
//	DDNF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxServerTypeMap.GetElement(eType);
//	if (xConnectDataMap)
//	{
//		DDNF_SHARE_PTR<ConnectData> pConnectData = xConnectDataMap->GetElementBySuit(nHashKey32);
//		if (pConnectData)
//		{
//			SendByServerID(pConnectData->nGameID, nMsgID, strData, id);
//		}
//	}
//	else
//	{
//		std::ostringstream stream;
//		stream << " can't find the server type " << eType;
//		stream << " msg id " << nMsgID;
//		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
//	}
//}

void DDNF::NetClientModule::SendSuitByPB(const SERVER_TYPES eType, const std::string& strHashKey, const uint16_t nMsgID, const google::protobuf::Message& xData)
{
    uint32_t nCRC32 = DDNF::CRC32(strHashKey);
    SendSuitByPB(eType, nCRC32, nMsgID, xData);
}

//void NetClientModule::SendSuitByPB(const SERVER_TYPES eType, const std::string & strHashKey, const uint16_t nMsgID, const google::protobuf::Message & xData, const NFGUID id)
//{
//	uint32_t nCRC32 = NFrame::CRC32(strHashKey);
//	SendSuitByPB(eType, nCRC32, nMsgID, xData, id);
//}

void DDNF::NetClientModule::SendSuitByPB(const SERVER_TYPES eType, const int nHashKey, const uint16_t nMsgID, const google::protobuf::Message& xData)
{
    //DDNF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxServerTypeMap.GetElement(eType);
    //if (xConnectDataMap)
    //{
    //    DDNF_SHARE_PTR<ConnectData> pConnectData = xConnectDataMap->GetElementBySuit(nHashKey);
    //    if (pConnectData)
    //    {
    //        SendToServerByPB(pConnectData->nGameID, nMsgID, xData);
    //    }
    //}
}

//void NetClientModule::SendSuitByPB(const SERVER_TYPES eType, const int nHashKey32, const uint16_t nMsgID, const google::protobuf::Message & xData, const NFGUID id)
//{
//	DDNF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxServerTypeMap.GetElement(eType);
//	if (xConnectDataMap)
//	{
//		DDNF_SHARE_PTR<ConnectData> pConnectData = xConnectDataMap->GetElementBySuit(nHashKey32);
//		if (pConnectData)
//		{
//			SendToServerByPB(pConnectData->nGameID, nMsgID, xData, id);
//		}
//	}
//}

DDNF_SHARE_PTR<ConnectData> DDNF::NetClientModule::GetServerNetInfo(const SERVER_TYPES eType)
{
    //DDNF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxServerTypeMap.GetElement(eType);
    //if (xConnectDataMap)
    //{
    //    return xConnectDataMap->GetElementBySuitRandom();
    //}

    return nullptr;
}

DDNF_SHARE_PTR<ConnectData> DDNF::NetClientModule::GetServerNetInfo(const int nServerID)
{
    return mxServerMap.GetElement(nServerID);
}

Map<int, ConnectData>& DDNF::NetClientModule::GetServerList()
{
    return mxServerMap;
}

DDNF_SHARE_PTR<ConnectData> DDNF::NetClientModule::GetServerNetInfo(const INet* pNet)
{
    int nServerID = 0;
    for (DDNF_SHARE_PTR<ConnectData> pData = mxServerMap.First(nServerID);
         pData != NULL; pData = mxServerMap.Next(nServerID))
    {
        if (pData->mxNetModule && pNet == pData->mxNetModule->GetNet())
        {
            return pData;
        }
    }

    return DDNF_SHARE_PTR<ConnectData>(NULL);
}

void DDNF::NetClientModule::InitCallBacks(DDNF_SHARE_PTR<ConnectData> pServerData)
{
	std::ostringstream stream;
	stream << "AddServer Type: " << pServerData->eServerType << " Server ID: " << pServerData->nGameID << " State: "
		<< pServerData->eState << " IP: " << pServerData->strIP << " Port: " << pServerData->nPort;

	m_pLogModule->LogInfo(stream.str());

    DDNF_SHARE_PTR<CallBack> xCallBack = mxCallBack.GetElement(pServerData->eServerType);
    if (!xCallBack)
    {
        xCallBack = DDNF_SHARE_PTR<CallBack>(DDNF_NEW CallBack);
        mxCallBack.AddElement(pServerData->eServerType, xCallBack);
    }

    //add msg callback
    std::map<int, std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>>::iterator itReciveCB = xCallBack->mxReceiveCallBack.begin();
    for (; xCallBack->mxReceiveCallBack.end() != itReciveCB; ++itReciveCB)
    {
		std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>& xList = itReciveCB->second;
		for (std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>::iterator itList = xList.begin(); itList != xList.end(); ++itList)
		{
			pServerData->mxNetModule->RegisterNetMsgHandler(itReciveCB->first, *itList);
		}
    }

    //add event callback
    std::list<NET_EVENT_HANDLER_FUNCTOR_PTR>::iterator itEventCB = xCallBack->mxEventCallBack.begin();
    for (; xCallBack->mxEventCallBack.end() != itEventCB; ++itEventCB)
    {
        pServerData->mxNetModule->RegisterNetEventHandler(*itEventCB);
    }

    std::list<NET_EVENT_HANDLER_FUNCTOR_PTR>::iterator itCB = xCallBack->mxCallBackList.begin();
    for (; xCallBack->mxCallBackList.end() != itCB; ++itCB)
    {
        pServerData->mxNetModule->RegisterNetMsgHandler(*itCB);
    }
}

void DDNF::NetClientModule::ProcessExecute()
{
    DDNF_SHARE_PTR<ConnectData> pServerData = mxServerMap.First();
    while (pServerData)
    {
        switch (pServerData->eState)
        {
            case ConnectDataState::DISCONNECT:
            {
                if (NULL != pServerData->mxNetModule)
                {
                    pServerData->mxNetModule = nullptr;
                    pServerData->eState = ConnectDataState::RECONNECT;
                }
            }
                break;
            case ConnectDataState::CONNECTING:
            {
                if (pServerData->mxNetModule)
                {
                    pServerData->mxNetModule->Execute();
                }
            }
                break;
            case ConnectDataState::NORMAL:
            {
                if (pServerData->mxNetModule)
                {
                    pServerData->mxNetModule->Execute();

                    KeepState(pServerData);
                }
            }
                break;
            case ConnectDataState::RECONNECT:
            {
                if ((pServerData->mnLastActionTime + 10) >= GetPluginManager()->GetNowTime())
                {
                    break;
                }

                if (nullptr != pServerData->mxNetModule)
                {
                    pServerData->mxNetModule = nullptr;
                }

                pServerData->eState = ConnectDataState::CONNECTING;
                pServerData->mxNetModule = DDNF_SHARE_PTR<INetModule>(DDNF_NEW NFNetModule(m_pPluginManager));

				pServerData->mxNetModule->Awake();
				pServerData->mxNetModule->Init();
				pServerData->mxNetModule->AfterInit();
				pServerData->mxNetModule->ReadyExecute();

                pServerData->mxNetModule->InitAsClient(pServerData->strIP.c_str(), pServerData->nPort);

                InitCallBacks(pServerData);
            }
                break;
            default:
                break;
        }

        pServerData = mxServerMap.Next();
    }
}

void DDNF::NetClientModule::LogServerInfo()
{
	m_pLogModule->LogInfo("This is a client, begin to print Server Info-------------------");

    ConnectData* pServerData = mxServerMap.First();
    while (nullptr != pServerData)
    {
        std::ostringstream stream;
        stream << "Type: " << pServerData->eServerType << " Server ID: " << pServerData->nGameID << " State: "
               << pServerData->eState << " IP: " << pServerData->strIP << " Port: " << pServerData->nPort;

		if (pServerData->eState == ConnectDataState::NORMAL)
		{
			m_pLogModule->LogInfo(stream.str());
		}
		else
		{
			m_pLogModule->LogError(stream.str());
		}

        pServerData = mxServerMap.Next();
    }

	m_pLogModule->LogInfo("This is a client, end to print Server Info---------------------");
}

void DDNF::NetClientModule::KeepState(DDNF_SHARE_PTR<ConnectData> pServerData)
{
    if (pServerData->mnLastActionTime + 10 > GetPluginManager()->GetNowTime())
    {
        return;
    }

    pServerData->mnLastActionTime = GetPluginManager()->GetNowTime();

	//send message
}

void DDNF::NetClientModule::OnSocketEvent(const SOCK fd, const NET_EVENT eEvent, INet* pNet)
{
    if (eEvent & NET_EVENT::NET_EVENT_CONNECTED)
    {
        OnConnected(fd, pNet);
    }
	else
    {
        OnDisConnected(fd, pNet);
    }
}

int DDNF::NetClientModule::OnConnected(const SOCK fd, INet* pNet)
{
    //DDNF_SHARE_PTR<ConnectData> pServerInfo = GetServerNetInfo(pNet);
    //if (pServerInfo)
    //{
    //    /////////////////////////////////////////////////////////////////////////////////////
    //    //AddServerWeightData(pServerInfo);
    //    pServerInfo->eState = ConnectDataState::NORMAL;

    //    //for type--suit
    //    DDNF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxServerTypeMap.GetElement(pServerInfo->eServerType);
    //    if (!xConnectDataMap)
    //    {
    //        xConnectDataMap = DDNF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>>(DDNF_NEW NFConsistentHashMapEx<int, ConnectData>());
    //        mxServerTypeMap.AddElement(pServerInfo->eServerType, xConnectDataMap);
    //    }

    //    xConnectDataMap->AddElement(pServerInfo->nGameID, pServerInfo);
    //}

    return 0;
}

int DDNF::NetClientModule::OnDisConnected(const SOCK fd, INet* pNet)
{
   // DDNF_SHARE_PTR<ConnectData> pServerInfo = GetServerNetInfo(pNet);
   // if (nullptr != pServerInfo)
   // {
   //     /////////////////////////////////////////////////////////////////////////////////////
   //     //RemoveServerWeightData(pServerInfo);
   //     pServerInfo->eState = ConnectDataState::DISCONNECT;
   //     pServerInfo->mnLastActionTime = GetPluginManager()->GetNowTime();

   //     //for type--suit
   //     DDNF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxServerTypeMap.GetElement(pServerInfo->eServerType);
   //     if (xConnectDataMap)
   //     {
			//xConnectDataMap->RemoveElement(pServerInfo->nGameID);
   //     }
   // }

    return 0;
}

void DDNF::NetClientModule::ProcessAddNetConnect()
{
    std::list<ConnectData>::iterator it = mxTempNetList.begin();
    for (; it != mxTempNetList.end(); ++it)
    {
        const ConnectData& xInfo = *it;
        DDNF_SHARE_PTR<ConnectData> xServerData = mxServerMap.GetElement(xInfo.nGameID);
        if (nullptr == xServerData)
        {
            xServerData = DDNF_SHARE_PTR<ConnectData>(DDNF_NEW ConnectData());

            xServerData->nGameID = xInfo.nGameID;
            xServerData->eServerType = xInfo.eServerType;
            xServerData->strIP = xInfo.strIP;
            xServerData->strName = xInfo.strName;
            xServerData->eState = ConnectDataState::CONNECTING;
            xServerData->nPort = xInfo.nPort;
            xServerData->mnLastActionTime = GetPluginManager()->GetNowTime();

            xServerData->mxNetModule = DDNF_SHARE_PTR<INetModule>(DDNF_NEW NFNetModule(m_pPluginManager));

			xServerData->mxNetModule->Awake();
			xServerData->mxNetModule->Init();
			xServerData->mxNetModule->AfterInit();
			xServerData->mxNetModule->ReadyExecute();

            xServerData->mxNetModule->InitAsServer(xServerData->strIP.c_str(), xServerData->nPort);
            xServerData->mxNetModule->ExpandBufferSize(mnBufferSize);

            InitCallBacks(xServerData);

            mxServerMap.AddElement(xInfo.nGameID, xServerData);
        }
		else
		{
			xServerData->nWorkLoad = xInfo.nWorkLoad;
		}
    }

    mxTempNetList.clear();
}