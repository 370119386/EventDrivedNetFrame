#ifndef __I__NET__H__
#define __I__NET__H__

#include "DDNFCommon/DDNFPluginModule/Platform.h"

#include <cstring>
#include <cerrno>
#include <cstdio>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <cassert>

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
#include <WinSock2.h>
#include <windows.h>
#elif DDNF_PLATFORM == DDNF_PLATFORM_APPLE || DDNF_PLATFORM == DDNF_PLATFORM_LINUX || DDNF_PLATFORM == DDNF_PLATFORM_ANDROID

#if DDNF_PLATFORM == DDNF_PLATFORM_APPLE

#include <libkern/OSByteOrder.h>

#endif

#include <netinet/in.h>

#ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif

#include <sys/socket.h>
#include <unistd.h>

#endif

#pragma pack(push, 1)

namespace DDNF
{
	enum NET_EVENT
	{
		NET_EVENT_EOF = 0x10,
		NET_EVENT_ERROR = 0x20,
		NET_EVENT_TIMEOUT = 0x40,
		NET_EVENT_CONNECTED = 0x80,
	};

	struct IMsgHead
	{
		enum Head
		{
			HEAD_LENGTH = 6,
		};

		virtual int EnCode(char* strData) = 0;

		virtual int DeCode(const char* strData) = 0;

		virtual uint16_t GetMsgID() const = 0;

		virtual void SetMsgID(uint16_t nMsgID) = 0;

		virtual uint32_t GetBodyLength() const = 0;

		virtual void SetBodyLength(uint32_t nLength) = 0;

		static int64_t DDNF_HTONLL(int64_t nData)
		{
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
			return htonll(nData);
#elif DDNF_PLATFORM == DDNF_PLATFORM_APPLE || DDNF_PLATFORM == DDNF_PLATFORM_APPLE_IOS
			return OSSwapHostToBigInt64(nData);
#else
			return htobe64(nData);
#endif
		}

		static int64_t DDNF_NTOHLL(int64_t nData)
		{
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
			return ntohll(nData);
#elif DDNF_PLATFORM == DDNF_PLATFORM_APPLE || DDNF_PLATFORM == DDNF_PLATFORM_APPLE_IOS
			return OSSwapBigToHostInt64(nData);
#elif DDNF_PLATFORM == DDNF_PLATFORM_ANDROID
			return betoh64(nData);
#else
			return be64toh(nData);
#endif
		}

		static int32_t DDNF_HTONL(int32_t nData)
		{
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
			return htonl(nData);
#elif DDNF_PLATFORM == DDNF_PLATFORM_APPLE || DDNF_PLATFORM == DDNF_PLATFORM_APPLE_IOS
			return OSSwapHostToBigInt32(nData);
#else
			return htobe32(nData);
#endif
		}

		static int32_t DDNF_NTOHL(int32_t nData)
		{
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
			return ntohl(nData);
#elif DDNF_PLATFORM == DDNF_PLATFORM_APPLE || DDNF_PLATFORM == DDNF_PLATFORM_APPLE_IOS
			return OSSwapBigToHostInt32(nData);
#elif DDNF_PLATFORM == DDNF_PLATFORM_ANDROID
			return betoh32(nData);
#else
			return be32toh(nData);
#endif
		}

		static int16_t DDNF_HTONS(int16_t nData)
		{
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
			return htons(nData);
#elif DDNF_PLATFORM == DDNF_PLATFORM_APPLE || DDNF_PLATFORM == DDNF_PLATFORM_APPLE_IOS
			return OSSwapHostToBigInt16(nData);
#else
			return htobe16(nData);
#endif
		}

		static int16_t DDNF_NTOHS(int16_t nData)
		{
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
			return ntohs(nData);
#elif DDNF_PLATFORM == DDNF_PLATFORM_APPLE || DDNF_PLATFORM == DDNF_PLATFORM_APPLE_IOS
			return OSSwapBigToHostInt16(nData);
#elif DDNF_PLATFORM == DDNF_PLATFORM_ANDROID
			return betoh16(nData);
#else
			return be16toh(nData);
#endif
		}
	};

	class MsgHead : public IMsgHead
	{
	public:
		MsgHead()
		{
			size = 0;
			msgId = 0;
		}

	protected:
		inline void EnCodeHTONS_INT16(char* data,uint16_t value)
		{
			uint16_t nValue = DDNF_HTONS(value);
			memcpy(data, (void*)(&nValue), sizeof(value));
		}

		inline void EnCodeHTONL_INT32(char* data, uint32_t value)
		{
			uint32_t nValue = DDNF_HTONL(value);
			memcpy(data, (void*)(&nValue), sizeof(value));
		}

		inline void DeCodeNTOHS_INT16(const char* data,uint16_t& value)
		{
			memcpy(&value, data, sizeof(value));
			value = DDNF_NTOHS(value);
		}

		inline void DeCodeNTOHS_INT32(const char* data, uint32_t& value)
		{
			memcpy(&value, data, sizeof(value));
			value = DDNF_NTOHL(value);
		}

	public:
		// Message Head[ MsgID(2) | MsgSize(4) ]
		virtual int EnCode(char* data)
		{
			uint32_t nOffset = 0;
			EnCodeHTONS_INT16(data + nOffset, msgId);
			nOffset += sizeof(msgId);
			uint32_t nPkgSize = size + (uint32_t)IMsgHead::HEAD_LENGTH;
			EnCodeHTONL_INT32(data + nOffset,nPkgSize);
			nOffset += sizeof(nPkgSize);
			assert(nOffset == IMsgHead::HEAD_LENGTH);
			return nOffset;
		}

		// Message Head[ MsgID(2) | MsgSize(4) ]
		virtual int DeCode(const char* data)
		{
			uint32_t nOffset = 0;
			DeCodeNTOHS_INT16(data + nOffset,msgId);
			nOffset += sizeof(msgId);
			DeCodeNTOHS_INT32(data + nOffset,size);
			nOffset += sizeof(size);
			assert(nOffset == IMsgHead::HEAD_LENGTH);
			return nOffset;
		}

		virtual uint16_t GetMsgID() const
		{
			return msgId;
		}

		virtual void SetMsgID(uint16_t nMsgID)
		{
			msgId = nMsgID;
		}

		virtual uint32_t GetBodyLength() const
		{
			return size;
		}

		virtual void SetBodyLength(uint32_t nLength)
		{
			size = nLength;
		}

	protected:
		uint32_t size;
		uint16_t msgId;
	};

	//Handle Accepted Net Message
	typedef std::function<void(const SOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen)> NET_MESSAGE_HANDLER_FUNCTOR;
	typedef std::shared_ptr<NET_MESSAGE_HANDLER_FUNCTOR> NET_MESSAGE_HANDLER_FUNCTOR_PTR;

	class INet;
	class NetObject;

	typedef std::function<void(const SOCK nSockIndex, const NET_EVENT nEvent, INet * pNet)> NET_EVENT_HANDLER_FUNCTOR;
	typedef std::shared_ptr<NET_EVENT_HANDLER_FUNCTOR> NET_EVENT_HANDLER_FUNCTOR_PTR;

	class INet
	{
	public:
		virtual ~INet() {}

		virtual bool Execute() = 0;

		//as client
		virtual void InitAsClient(const char* strIP, const unsigned short nPort) = 0;

		//as server
		virtual int InitAsServer(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount = 4) = 0;

		//virtual int ExpandBufferSize(const unsigned int size) = 0;

		//virtual bool Final() = 0;

		////send a message with out msg-head[auto add msg-head in this function]
		//virtual bool SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const size_t nLen, const SOCK nSockIndex = 0) = 0;

		////send a message with out msg-head[need to add msg-head for this message by youself]
		//virtual bool SendMsg(const char* msg, const size_t nLen, const SOCK nSockIndex) = 0;

		////send a message to all client[need to add msg-head for this message by youself]
		//virtual bool SendMsgToAllClient(const char* msg, const size_t nLen) = 0;

		////send a message with out msg-head to all client[auto add msg-head in this function]
		//virtual bool SendMsgToAllClientWithOutHead(const int16_t nMsgID, const char* msg, const size_t nLen) = 0;

		virtual bool CloseNetObject(const SOCK nSockIndex) = 0;

		virtual NetObject* GetNetObject(const SOCK nSockIndex) = 0;

		virtual bool AddNetObject(const SOCK nSockIndex, NetObject* pObject) = 0;

		//virtual bool IsServer() = 0;

		//virtual bool Log(int severity, const char* msg) = 0;
	};

	class NetObject
	{
	public:
		NetObject(INet* pkNet, SOCK fd, sockaddr_in& addr,void* pBev)
		{
			m_pvUserData = pBev;
			m_fd = fd;
			m_pkNet = pkNet;
			memset(&m_pkAddr, 0, sizeof(m_pkAddr));
			m_pkAddr = addr;
			m_pkBuffer.reserve(4096);
			m_pkEncryptKey.clear();
			m_iNetState = 0;
			m_pkAccount.clear();
		}

		int Append(const char* str, size_t nLen)
		{
			m_pkBuffer.append(str, nLen);

			return (int)m_pkBuffer.length();
		}

		int CopyTo(char* dst, uint32_t start, uint32_t length)
		{
			if (start + length > m_pkBuffer.length())
			{
				return 0;
			}

			memcpy(dst, m_pkBuffer.data() + start, length);

			return length;
		}

		int Remove(uint32_t nStart, uint32_t nLen)
		{
			if (nStart + nLen > m_pkBuffer.length())
			{
				return 0;
			}

			m_pkBuffer.erase(nStart, nLen);

			return (int)m_pkBuffer.length();
		}

		inline const char* GetDataPtr() const
		{
			return m_pkBuffer.data();
		}

		inline int GetMsgLength() const
		{
			return (int)m_pkBuffer.length();
		}

		inline void* GetUserData()
		{
			return m_pvUserData;
		}

		inline void SetEncrptKey(const std::string& encrptKey)
		{
			m_pkEncryptKey = encrptKey;
		}

		inline const std::string& GetEncrptKey() const
		{
			return m_pkEncryptKey; 
		}

		inline void SetNetState(int iNetState)
		{
			m_iNetState = iNetState;
		}

		inline int32_t GetNetState() const 
		{
			return m_iNetState; 
		}

		inline void SetAccount(const std::string& account)
		{
			m_pkAccount = account;
		}

		inline const std::string& GetAccount() const
		{
			return m_pkAccount;
		}

		inline INet* GetNetHandle() const
		{
			return m_pkNet;
		}

		inline void SetNeedRemove(bool flag)
		{
			m_bNeedRemove = flag;
		}

		inline bool GetNeedRemove() const
		{
			return m_bNeedRemove;
		}

		inline SOCK GetSocketFD() const
		{
			return m_fd;
		}

	private:
		INet* m_pkNet;
		SOCK m_fd;
		sockaddr_in m_pkAddr;
		void* m_pvUserData;
		std::string m_pkBuffer;
		std::string m_pkEncryptKey;
		int32_t m_iNetState;
		std::string m_pkAccount;
		bool m_bNeedRemove;

	};
}

#pragma pack(pop)

#endif//__I__NET__H__