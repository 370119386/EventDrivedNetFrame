#include "DDNFCommon/DDNFPluginModule/Platform.h"

#ifdef DDNF_DEBUG_MODE

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "Core_d.lib" )
//#pragma comment( lib, "NFMessageDefine_d.lib" )
#pragma comment( lib, "libprotobuf_d.lib" )
#pragma comment( lib, "libevent_core.lib" )

#elif DDNF_PLATFORM == DDNF_PLATFORM_LINUX || DDNF_PLATFORM == DDNF_PLATFORM_ANDROID
#pragma comment( lib, "libevent_core.a" )
#pragma comment( lib, "libevent.a" )
//#pragma comment( lib, "NFMessageDefine_d.a" )
#elif DDNF_PLATFORM == DDNF_PLATFORM_APPLE || DDNF_PLATFORM == DDNF_PLATFORM_APPLE_IOS
#endif

#else

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "Core.lib" )
//#pragma comment( lib, "NFMessageDefine.lib" )
#pragma comment( lib, "libprotobuf.lib" )
#pragma comment( lib, "libevent_core.lib" )

#elif DDNF_PLATFORM == DDNF_PLATFORM_LINUX || DDNF_PLATFORM == DDNF_PLATFORM_ANDROID
#pragma comment( lib, "libevent.a" )
#pragma comment( lib, "libevent_core.a" )
//#pragma comment( lib, "NFMessageDefine.a" )
#elif DDNF_PLATFORM == DDNF_PLATFORM_APPLE || DDNF_PLATFORM == DDNF_PLATFORM_APPLE_IOS
#endif

#endif
