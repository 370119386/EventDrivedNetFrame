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

namespace DDNF
{
	class Net : public INet
	{
	public:
		virtual bool Execute() override;

	protected:
		int InitClientNet();

	private:
		std::string mIp;
		int mPort;
		struct event_base* mEventBase;
		struct evconnlistener* mConnectListner;
	};
}
#endif//__DDNF__NET__H__