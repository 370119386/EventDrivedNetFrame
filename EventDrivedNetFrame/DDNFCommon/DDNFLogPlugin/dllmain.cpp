#include "DDNFCommon/DDNFPluginModule/Platform.h"

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
#pragma comment( lib, "Dbghelp.lib" )
#endif


#ifdef DDNF_DEBUG_MODE

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN

#ifdef DDNF_DYNAMIC_PLUGIN
//#pragma comment( lib, "NFCore_d.lib" )


#else
//#pragma comment( lib, "NFCore_d.lib" )

#endif

//
#elif DDNF_PLATFORM == DDNF_PLATFORM_LINUX || DDNF_PLATFORM == DDNF_PLATFORM_ANDROID
//#pragma comment( lib, "NFCore_d.a" )

#elif DDNF_PLATFORM == DDNF_PLATFORM_APPLE || DDNF_PLATFORM == DDNF_PLATFORM_APPLE_IOS
#endif

#else

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
#ifdef DDNF_DYNAMIC_PLUGIN
//#pragma comment( lib, "NFCore.lib" )

#else
//#pragma comment( lib, "NFCore.lib" )
#endif

//
#elif DDNF_PLATFORM == DDNF_PLATFORM_LINUX || DDNF_PLATFORM == DDNF_PLATFORM_ANDROID
#pragma comment( lib, "NFCore.a" )

#elif DDNF_PLATFORM == DDNF_PLATFORM_APPLE || DDNF_PLATFORM == DDNF_PLATFORM_APPLE_IOS
#endif

#endif
