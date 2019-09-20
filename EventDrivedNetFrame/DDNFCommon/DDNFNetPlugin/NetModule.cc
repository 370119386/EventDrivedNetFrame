#include "NetModule.h"
#include "DDNFCommon/DDNFNetPlugin/Net.h"

using namespace DDNF;

DDNF::NetModule::NetModule(IPluginManager* p)
{
	m_pPluginManager = p;
	m_pNet = NULL;
	m_bufferSize = 0;
	nLastTime = m_pPluginManager->GetNowTime();
}

DDNF::NetModule::~NetModule()
{
	Finalize();
}

bool DDNF::NetModule::RegisterNetMsgHandler(const int nMsgID, const NET_MESSAGE_HANDLER_FUNCTOR_PTR& cb)
{
	if (m_NetMsgIdHandlerImpl.find(nMsgID) == m_NetMsgIdHandlerImpl.end())
	{
		m_NetMsgIdHandlerImpl.insert(std::make_pair(nMsgID,new std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>()));
	}

	m_NetMsgIdHandlerImpl[nMsgID]->push_back(cb);

	return true;
}

bool DDNF::NetModule::RegisterNetMsgHandler(const NET_MESSAGE_HANDLER_FUNCTOR_PTR& cb)
{
	m_kNetMsgImpl.push_back(cb);
	return true;
}

bool DDNF::NetModule::RegisterNetEventHandler(const NET_EVENT_HANDLER_FUNCTOR_PTR& cb)
{
	m_kNetEventImpl.push_back(cb);
	return true;
}

void DDNF::NetModule::UnRegisterNetMsgHandler(const int nMsgID)
{
	if (m_NetMsgIdHandlerImpl.find(nMsgID) != m_NetMsgIdHandlerImpl.end())
	{
		std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>* pkList = m_NetMsgIdHandlerImpl[nMsgID];
		if (NULL != pkList)
		{
			pkList->clear();
			delete pkList;
		}
		m_NetMsgIdHandlerImpl.erase(nMsgID);
	}
}

void DDNF::NetModule::UnRegisterNetMsgHandler(const NET_MESSAGE_HANDLER_FUNCTOR_PTR& cb)
{
	std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>::iterator it = std::find(m_kNetMsgImpl.begin(), m_kNetMsgImpl.end(), cb);
	if(it != m_kNetMsgImpl.end())
		m_kNetMsgImpl.erase(it);
}

void DDNF::NetModule::UnRegisterNetEventHandler(const NET_EVENT_HANDLER_FUNCTOR_PTR& cb)
{
	std::list<NET_EVENT_HANDLER_FUNCTOR_PTR>::iterator it = std::find(m_kNetEventImpl.begin(), m_kNetEventImpl.end(), cb);
	if (it != m_kNetEventImpl.end())
	{
		m_kNetEventImpl.erase(it);
	}
}

void DDNF::NetModule::InitAsClient(const char* strIP, const unsigned short nPort)
{
	m_pNet = DDNF_NEW Net(this, &NetModule::OnHandleNetMsg, &NetModule::OnHandleNetEvent);
	m_pNet->ExpandBufferSize(m_bufferSize);
	m_pNet->InitAsClient(strIP, nPort);
}

int DDNF::NetModule::InitAsServer(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount /*= 4*/)
{
	m_pNet = DDNF_NEW Net(this, &NetModule::OnHandleNetMsg, &NetModule::OnHandleNetEvent);
	m_pNet->ExpandBufferSize(m_bufferSize);
	return m_pNet->InitAsServer(nMaxClient, nPort, nCpuCount);
}

int DDNF::NetModule::ExpandBufferSize(const unsigned int size /*= 1024 * 1024 * 20*/)
{
	if ((int)size > m_bufferSize)
	{
		m_bufferSize = (int)size;
		if (m_pNet)
		{
			m_pNet->ExpandBufferSize(m_bufferSize);
		}
	}
	return m_bufferSize;
}

bool DDNF::NetModule::Finalize()
{
	m_pPluginManager = NULL;
	m_bufferSize = 0;
	if (m_pNet)
	{
		m_pNet->Final();
	}
	delete m_pNet;
	m_pNet = NULL;
	std::map<int, std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>*>::iterator itr = m_NetMsgIdHandlerImpl.begin();
	std::map<int, std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>*>::iterator end = m_NetMsgIdHandlerImpl.end();
	for (; itr != end; itr++)
	{
		std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>* pkList = itr->second;
		if (NULL != pkList)
		{
			pkList->clear();
			delete pkList;
		}
	}
	m_NetMsgIdHandlerImpl.clear();
	m_kNetMsgImpl.clear();
	m_kNetEventImpl.clear();
	return true;
}

void DDNF::NetModule::OnHandleNetMsg(const SOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	//m_pLogModule->LogInfo("OnReceiveNetPack " + std::to_string(nMsgID), __FUNCTION__, __LINE__);

	//NFPerformance performance;

	std::map<int, std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>*>::iterator it = m_NetMsgIdHandlerImpl.find(nMsgID);
	if (m_NetMsgIdHandlerImpl.end() != it)
	{
		std::map<int, std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>*>::value_type& pair = *it;
		std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>& xFunList = *pair.second;
		for (std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>::iterator itList = xFunList.begin(); itList != xFunList.end(); ++itList)
		{
			NET_MESSAGE_HANDLER_FUNCTOR_PTR& pFunPtr = *itList;
			NET_MESSAGE_HANDLER_FUNCTOR* pFunc = pFunPtr.get();
#if DDNF_PLATFORM != DDNF_PLATFORM_WIN
			DDNF_CRASH_TRY
#endif
			pFunc->operator()(nSockIndex, nMsgID, msg, nLen);
#if DDNF_PLATFORM != DDNF_PLATFORM_WIN
			DDNF_CRASH_END_TRY
#endif
		}
	}
	else
	{
		for (std::list<NET_MESSAGE_HANDLER_FUNCTOR_PTR>::iterator itList = m_kNetMsgImpl.begin(); itList != m_kNetMsgImpl.end(); ++itList)
		{
			NET_MESSAGE_HANDLER_FUNCTOR_PTR& pFunPtr = *itList;
			NET_MESSAGE_HANDLER_FUNCTOR* pFunc = pFunPtr.get();
#if DDNF_PLATFORM != DDNF_PLATFORM_WIN
			DDNF_CRASH_TRY
#endif
			pFunc->operator()(nSockIndex, nMsgID, msg, nLen);
#if DDNF_PLATFORM != DDNF_PLATFORM_WIN
			DDNF_CRASH_END_TRY
#endif
		}
	}

	//if (performance.CheckTimePoint(1))
	//{
	//	std::ostringstream os;
	//	os << "---------------net module performance problem------------------- ";
	//	os << performance.TimeScope();
	//	os << "---------- ";
	//	m_pLogModule->LogWarning(NFGUID(0, nMsgID), os, __FUNCTION__, __LINE__);
	//}
}

void DDNF::NetModule::OnHandleNetEvent(const SOCK nSockIndex, const NET_EVENT eEvent, INet* pNet)
{
	for (std::list<NET_EVENT_HANDLER_FUNCTOR_PTR>::iterator it = m_kNetEventImpl.begin();
		it != m_kNetEventImpl.end(); ++it)
	{
		NET_EVENT_HANDLER_FUNCTOR_PTR& pFunPtr = *it;
		NET_EVENT_HANDLER_FUNCTOR* pFunc = pFunPtr.get();
		pFunc->operator()(nSockIndex, eEvent, pNet);
	}
}