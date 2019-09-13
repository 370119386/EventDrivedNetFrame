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

#include "event2/event.h"
#include "event2/bufferevent_struct.h"

using namespace DDNF;

bool DDNF::Net::Execute()
{
	return true;
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
	//NetObject* pkNetObject = new NetObject(this, sockfd, &addr, bev);
	return -1;
}