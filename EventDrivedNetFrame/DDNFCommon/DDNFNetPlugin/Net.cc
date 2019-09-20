#include <string.h>
#include <atomic>
#include "Net.h"

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma  comment(lib,"Ws2_32.lib")
#ifndef LIBEVENT_SRC
#pragma  comment(lib,"libevent.lib")
#pragma  comment(lib,"libevent_core.lib")
#endif
#elif DDNF_PLATFORM == DDNF_PLATFORM_APPLE
#include <arpa/inet.h>
#endif

#include <iostream>

#include "event2/event.h"
#include "event2/bufferevent_struct.h"

using namespace DDNF;

/*
if any one want to upgrade the networking library(libEvent), please change the size of evbuffer as below:
*MODIFY--libevent/buffer.c
#define EVBUFFER_MAX_READ	4096
TO
#define EVBUFFER_MAX_READ	65536
*/

//1048576 = 1024 * 1024
#define DDNF_BUFFER_MAX_READ	1048576


DDNF::Net::Net()
{
	mIp.clear();
	mPort = 0;
	mEventBase = NULL;
	listener = NULL;
	mConnectListner = NULL;
	mIndexSocketDic.clear();
	mbServer = false;
	mbTCPStream = false;
	mMsgRecievedCount = 0;
	mbWorking = false;
	mNetMsgHandler = NULL;
	mNetEventHandler = NULL;
	mBufferSize = 0;
	mNeedRemovedSockes.clear();
	mNeedRemovedSockes.reserve(4);
}

bool DDNF::Net::Execute()
{
	ExecuteClose();

	if (mEventBase)
	{
		event_base_loop(mEventBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	}

	return true;
}


void DDNF::Net::InitAsClient(const char* strIP, const unsigned short nPort)
{
	mIp = strIP;
	mPort = nPort;

	InitClientNet();
}


int DDNF::Net::InitAsServer(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount /*= 4*/)
{
	mMaxConnect = nMaxClient;
	mPort = nPort;
	mCpuCount = nCpuCount;

	return InitServerNet();
}

int DDNF::Net::InitClientNet()
{
	std::string strIP = mIp;
	int nPort = mPort;

	struct sockaddr_in addr;
	struct bufferevent* bev = NULL;

#ifdef _MSC_VER
	WSADATA wsa_data;
	int errCode = WSAStartup(0x0201, &wsa_data);
	if (errCode != 0)
	{
		printf("WSAStartup Error = %d\n", errCode);
		return -1;
	}
#endif

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);

	//convert ip string from host byteorder to net byteorder
	if (evutil_inet_pton(AF_INET, strIP.c_str(), &addr.sin_addr) <= 0)
	{
		printf("inet_pton_error\n");
		return -1;
	}

	mEventBase = event_base_new();
	if (0 == mEventBase)
	{
		printf("event_base_new failed\n");
		return -1;
	}

	bev = bufferevent_socket_new(mEventBase, -1, BEV_OPT_CLOSE_ON_FREE);
	if (bev == NULL)
	{
		printf("bufferevent_socket_new\n");
		return -1;
	}

	int bRet = bufferevent_socket_connect(bev, (struct sockaddr*)&addr, sizeof(addr));
	if (0 != bRet)
	{
		printf("bufferevent_socket_connect error\n");
		return -1;
	}

	SOCK sockfd = bufferevent_getfd(bev);
	NetObject* pkNetObject = new NetObject(this, sockfd, addr, bev);

	if (!AddNetObject(0, pkNetObject))
	{
		assert(0);
		return -1;
	}

	mbServer = false;

	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, (void*)pkNetObject);
	bufferevent_enable(bev, EV_READ | EV_WRITE | EV_CLOSED | EV_TIMEOUT | EV_PERSIST);

	event_set_log_callback(&Net::log_cb);

	bufferevent_set_max_single_read(bev, DDNF_BUFFER_MAX_READ);
	bufferevent_set_max_single_write(bev, DDNF_BUFFER_MAX_READ);

	int nSizeRead = (int)bufferevent_get_max_to_read(bev);
	int nSizeWrite = (int)bufferevent_get_max_to_write(bev);

	std::cout << "want to connect " << mIp << " SizeRead: " << nSizeRead << std::endl;
	std::cout << "SizeWrite: " << nSizeWrite << std::endl;

	return (int)sockfd;
}

int DDNF::Net::InitServerNet()
{
	int nCpuCount = mCpuCount;
	int nPort = mPort;

	struct sockaddr_in sin;

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
	WSADATA wsa_data;
	int errCode = WSAStartup(0x0201, &wsa_data);
	if (errCode != 0)
	{
		printf("WSAStartup Error = %d\n", errCode);
		return -1;
	}
#endif
	//////////////////////////////////////////////////////////////////////////

	struct event_config* cfg = event_config_new();

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN

	mEventBase = event_base_new_with_config(cfg);

#else

	//event_config_avoid_method(cfg, "epoll");
	if (event_config_set_flag(cfg, EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST) < 0)
	{

		return -1;
	}

	if (event_config_set_num_cpus_hint(cfg, nCpuCount) < 0)
	{
		return -1;
	}

	mxBase = event_base_new_with_config(cfg);//event_base_new()

#endif
	event_config_free(cfg);

	//////////////////////////////////////////////////////////////////////////

	if (!mEventBase)
	{
		fprintf(stderr, "Could not initialize libevent!\n");
		Final();

		return -1;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(nPort);

	printf("server started with %d\n", nPort);

	listener = evconnlistener_new_bind(mEventBase, listener_cb, (void*)this,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
		(struct sockaddr*) & sin,
		sizeof(sin));

	if (!listener)
	{
		fprintf(stderr, "Could not create a listener!\n");
		Final();

		return -1;
	}

	mbServer = true;

	event_set_log_callback(&Net::log_cb);

	return mMaxConnect;
}

int DDNF::Net::ExpandBufferSize(unsigned int size)
{
	if (size > 0)
	{
		mBufferSize = size;
	}
	return mBufferSize;
}


bool DDNF::Net::Final()
{
	CloseSocketAll();

	if (listener)
	{
		evconnlistener_free(listener);
		listener = NULL;
	}

	if (mEventBase)
	{
		event_base_free(mEventBase);
		mEventBase = NULL;
	}

	return true;
}


bool DDNF::Net::SendMsg(const char* msg, const size_t nLen, const SOCK nSockIndex)
{
	if (nLen <= 0)
	{
		return false;
	}

	if (!mbWorking)
	{
		return false;
	}

	std::map<SOCK, NetObject*>::iterator it = mIndexSocketDic.find(nSockIndex);
	if (it != mIndexSocketDic.end())
	{
		NetObject* pNetObject = (NetObject*)it->second;
		if (pNetObject)
		{
			bufferevent* bev = (bufferevent*)pNetObject->GetUserData();
			if (NULL != bev)
			{
				bufferevent_write(bev, msg, nLen);

				++mMsgDispatchedCount;
				return true;
			}
		}
	}

	return false;
}


bool DDNF::Net::SendMsgToAllClient(const char* msg, const size_t nLen)
{
	if (nLen <= 0)
	{
		return false;
	}

	if (!mbWorking)
	{
		return false;
	}

	std::map<SOCK, NetObject*>::iterator it = mIndexSocketDic.begin();
	for (; it != mIndexSocketDic.end(); ++it)
	{
		NetObject* pNetObject = (NetObject*)it->second;
		if (pNetObject && !pNetObject->GetNeedRemove())
		{
			bufferevent* bev = (bufferevent*)pNetObject->GetUserData();
			if (NULL != bev)
			{
				bufferevent_write(bev, msg, nLen);

				++mMsgDispatchedCount;
			}
		}
	}

	return true;
}

bool DDNF::Net::SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const size_t nLen, const SOCK nSockIndex)
{
	std::string strOutData;
	int nAllLen = EnCode(nMsgID, msg, (uint32_t)nLen, strOutData);
	if (nAllLen == nLen + IMsgHead::Head::HEAD_LENGTH)
	{
		return SendMsg(strOutData.c_str(), strOutData.length(), nSockIndex);
	}

	return false;
}


bool DDNF::Net::SendMsgToAllClientWithOutHead(const int16_t nMsgID, const char* msg, const size_t nLen)
{
	std::string strOutData;
	int nAllLen = EnCode(nMsgID, msg, (uint32_t)nLen, strOutData);
	if (nAllLen == nLen + IMsgHead::Head::HEAD_LENGTH)
	{
		return SendMsgToAllClient(strOutData.c_str(), (uint32_t)strOutData.length());
	}

	return false;
}

bool DDNF::Net::SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const size_t nLen, const std::list<SOCK>& fdList)
{
	std::string strOutData;
	int nAllLen = EnCode(nMsgID, msg, (uint32_t)nLen, strOutData);
	if (nAllLen == nLen + IMsgHead::Head::HEAD_LENGTH)
	{
		return SendMsg(strOutData.c_str(), strOutData.length(), fdList);
	}

	return false;
}

bool DDNF::Net::CloseNetObject(const SOCK nSockIndex)
{
	std::map<SOCK, NetObject*>::iterator it = mIndexSocketDic.find(nSockIndex);
	if (it != mIndexSocketDic.end())
	{
		NetObject* pObject = it->second;
		pObject->SetNeedRemove(true);
		mNeedRemovedSockes.push_back(nSockIndex);

		return true;
	}

	return false;
}

bool DDNF::Net::AddNetObject(const SOCK nSockIndex, NetObject* pObject)
{
	//nSockIndex ±Ì æ0
	return mIndexSocketDic.insert(std::map<SOCK, NetObject*>::value_type(nSockIndex, pObject)).second;
}


DDNF::NetObject* DDNF::Net::GetNetObject(const SOCK nSockIndex)
{
	std::map<SOCK, NetObject*>::iterator it = mIndexSocketDic.find(nSockIndex);
	if (it != mIndexSocketDic.end())
	{
		return it->second;
	}

	return NULL;
}


bool DDNF::Net::IsServer()
{
	return mbServer;
}


bool DDNF::Net::Log(int severity, const char* msg)
{
	log_cb(severity, msg);
	return true;
}

void DDNF::Net::CloseObject(const SOCK nSockIndex)
{
	std::map<SOCK, NetObject*>::iterator it = mIndexSocketDic.find(nSockIndex);
	if (it != mIndexSocketDic.end())
	{
		mIndexSocketDic.erase(it);

		NetObject* pObject = it->second;

		struct bufferevent* bev = (bufferevent*)pObject->GetUserData();

		bufferevent_free(bev);

		delete pObject;
		pObject = NULL;
	}
}


void DDNF::Net::ExecuteClose()
{
	for (int i = 0; i < mNeedRemovedSockes.size(); ++i)
	{
		SOCK nSocketIndex = mNeedRemovedSockes[i];
		CloseObject(nSocketIndex);
	}

	mNeedRemovedSockes.clear();
}


bool DDNF::Net::CloseSocketAll()
{
	std::map<SOCK, NetObject*>::iterator it = mIndexSocketDic.begin();
	for (; it != mIndexSocketDic.end(); ++it)
	{
		SOCK nFD = it->first;
		mNeedRemovedSockes.push_back(nFD);
	}

	ExecuteClose();

	mNeedRemovedSockes.clear();

	return true;
}


void DDNF::Net::listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sa, int socklen, void* user_data)
{
	Net* pNet = (Net*)user_data;
	bool bClose = pNet->CloseNetObject(fd);
	if (bClose)
	{
		return;
	}

	if (pNet->mIndexSocketDic.size() >= pNet->mMaxConnect)
	{
		return;
	}

	struct event_base* mxBase = pNet->mEventBase;

	struct bufferevent* bev = bufferevent_socket_new(mxBase, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		fprintf(stderr, "Error constructing bufferevent!");
		return;
	}


	struct sockaddr_in* pSin = (sockaddr_in*)sa;

	NetObject* pObject = new NetObject(pNet, fd, *pSin, bev);
	pObject->GetNetHandle()->AddNetObject(fd, pObject);

	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, (void*)pObject);

	bufferevent_enable(bev, EV_READ | EV_WRITE | EV_CLOSED | EV_TIMEOUT | EV_PERSIST);

	//event_set_fatal_callback(event_fatal_cb);

	conn_eventcb(bev, BEV_EVENT_CONNECTED, (void*)pObject);

	bufferevent_set_max_single_read(bev, DDNF_BUFFER_MAX_READ);
	bufferevent_set_max_single_write(bev, DDNF_BUFFER_MAX_READ);
}

void DDNF::Net::conn_readcb(struct bufferevent* bev, void* user_data)
{
	NetObject* pObject = (NetObject*)user_data;
	if (!pObject)
	{
		return;
	}

	Net* pNet = (Net*)pObject->GetNetHandle();
	if (!pNet)
	{
		return;
	}

	if (pObject->GetNeedRemove())
	{
		return;
	}

	struct evbuffer* input = bufferevent_get_input(bev);
	if (!input)
	{
		return;
	}

	size_t len = evbuffer_get_length(input);
	unsigned char* pData = evbuffer_pullup(input, len);
	pObject->Append((const char*)pData, len);
	evbuffer_drain(input, len);

	if (pNet->mbTCPStream)
	{
		int len = pObject->GetMsgLength();
		if (len > 0)
		{
			if (pNet->mNetMsgHandler)
			{
				pNet->mNetMsgHandler(pObject->GetSocketFD(), 0, pObject->GetDataPtr(), len);

				++pNet->mMsgRecievedCount;
			}
			pObject->Remove(0, len);
		}
	}
	else
	{
		while (1)
		{
			if (!pNet->Dismantle(pObject))
			{
				break;
			}
		}
	}
}


void DDNF::Net::conn_writecb(struct bufferevent* bev, void* user_data)
{
	//  struct evbuffer *output = bufferevent_get_output(bev);
}

void DDNF::Net::conn_eventcb(struct bufferevent* bev, short events, void* user_data)
{
	NetObject* pObject = (NetObject*)user_data;
	Net* pNet = (Net*)pObject->GetNetHandle();
	std::cout << "Thread ID = " << std::this_thread::get_id() << " FD = " << pObject->GetSocketFD() << " Event ID =" << events << std::endl;

	if (events & BEV_EVENT_CONNECTED)
	{
		//must to set it's state before the "EventCB" functional be called[maybe user will send msg in the callback function]
		pNet->mbWorking = true;
	}
	else
	{
		if (!pNet->mbServer)
		{
			pNet->mbWorking = false;
		}
	}


	if (pNet->mNetEventHandler)
	{
		pNet->mNetEventHandler(pObject->GetSocketFD(),NET_EVENT(events), pNet);
	}

	if (events & BEV_EVENT_CONNECTED)
	{
		struct evbuffer* input = bufferevent_get_input(bev);
		struct evbuffer* output = bufferevent_get_output(bev);
		if (pNet->mBufferSize > 0)
		{
			evbuffer_expand(input, pNet->mBufferSize);
			evbuffer_expand(output, pNet->mBufferSize);
		}
		//printf("%d Connection successed\n", pObject->GetFd());/*XXX win32*/
	}
	else
	{
		pNet->CloseNetObject(pObject->GetSocketFD());
	}
}


void DDNF::Net::log_cb(int severity, const char* msg)
{
	//LOG(FATAL) << "severity:" << severity << " " << msg;
	std::cout << "severity:" << severity << " " << msg;
}

bool DDNF::Net::Dismantle(NetObject* pObject)
{
	bool bNeedDismantle = false;
	int len = pObject->GetMsgLength();
	if (len > IMsgHead::Head::HEAD_LENGTH)
	{
		MsgHead xHead;
		int nMsgBodyLength = DeCode(pObject->GetDataPtr(), len, xHead);
		if (nMsgBodyLength > 0 && xHead.GetMsgID() > 0)
		{
			if (mNetMsgHandler)
			{
				mNetMsgHandler(pObject->GetSocketFD(), xHead.GetMsgID(), pObject->GetDataPtr() + IMsgHead::Head::HEAD_LENGTH, nMsgBodyLength);

				++mMsgRecievedCount;
			}

			pObject->Remove(0, nMsgBodyLength + IMsgHead::Head::HEAD_LENGTH);

			bNeedDismantle = true;
		}
	}

	return bNeedDismantle;
}

int DDNF::Net::DeCode(const char* strData, const uint32_t unAllLen, MsgHead& xHead)
{
	if (unAllLen < IMsgHead::Head::HEAD_LENGTH)
	{
		return -1;
	}

	if (IMsgHead::Head::HEAD_LENGTH != xHead.DeCode(strData))
	{
		return -2;
	}

	if (xHead.GetBodyLength() > (unAllLen - IMsgHead::Head::HEAD_LENGTH))
	{
		return -3;
	}

	return xHead.GetBodyLength();
}

int DDNF::Net::EnCode(const uint16_t unMsgID, const char* strData, const uint32_t unDataLen, std::string& strOutData)
{
	MsgHead xHead;
	xHead.SetMsgID(unMsgID);
	xHead.SetBodyLength(unDataLen);

	char szHead[IMsgHead::Head::HEAD_LENGTH] = { 0 };
	xHead.EnCode(szHead);

	strOutData.clear();
	strOutData.append(szHead, IMsgHead::Head::HEAD_LENGTH);
	strOutData.append(strData, unDataLen);

	return xHead.GetBodyLength() + IMsgHead::Head::HEAD_LENGTH;
}

bool DDNF::Net::SendMsg(const char* msg, const size_t nLen, const std::list<SOCK>& fdList)
{
	std::list<SOCK>::const_iterator it = fdList.begin();
	for (; it != fdList.end(); ++it)
	{
		SendMsg(msg, nLen, *it);
	}

	return true;
}