#ifndef __DDNF__PLATFORM__H__
#define __DDNF__PLATFORM__H__

#define DDNF_PLATFORM_WIN 1
#define DDNF_PLATFORM_LINUX 2
#define DDNF_PLATFORM_APPLE 3
#define DDNF_PLATFORM_SYMBIAN 4
#define DDNF_PLATFORM_APPLE_IOS 5
#define DDNF_PLATFORM_ANDROID 6

#define DDNF_COMPILER_MSVC 1
#define DDNF_COMPILER_GNUC 2
#define DDNF_COMPILER_BORL 3
#define DDNF_COMPILER_WINSCW 4
#define DDNF_COMPILER_GCCE 5

#define DDNF_ENDIAN_LITTLE 1
#define DDNF_ENDIAN_BIG 2

//#define DDNF_ENABLE_SSL 1

#if DDNF_ENABLE_SSL
#define EVENT__HAVE_OPENSSL
#endif

#define DDNF_ARCHITECTURE_32 1
#define DDNF_ARCHITECTURE_64 2

/* Finds the compiler type and version.
*/
#if defined( __GCCE__ )
#   define DDNF_COMPILER DDNF_COMPILER_GCCE
#   define DDNF_COMP_VER _MSC_VER
//# include <staticlibinit_gcce.h> // This is a GCCE toolchain workaround needed when compiling with GCCE
#elif defined( __WINSCW__ )
#   define DDNF_COMPILER DDNF_COMPILER_WINSCW
#   define DDNF_COMP_VER _MSC_VER
#elif defined( _MSC_VER )
#   define DDNF_COMPILER DDNF_COMPILER_MSVC
#   define DDNF_COMP_VER _MSC_VER
#elif defined( __GNUC__ )
#   define DDNF_COMPILER DDNF_COMPILER_GNUC
#   define DDNF_COMP_VER (((__GNUC__)*100) + \
                        (__GNUC_MINOR__*10) + \
                        __GNUC_PATCHLEVEL__)

#elif defined( __BORLANDC__ )
#   define DDNF_COMPILER DDNF_COMPILER_BORL
#   define DDNF_COMP_VER __BCPLUSPLUS__
#   define __FUNCTION__ __FUNC__
#else
#   pragma error "No known compiler. Abort! Abort!"

#endif

/* See if we can use __forceinline or if we need to use __inline instead */
#if DDNF_COMPILER == DDNF_COMPILER_MSVC
#   if DDNF_COMP_VER >= 1200
#       define FORCEINLINE __forceinline
#   endif
#elif defined(__MINGW32__)
#   if !defined(FORCEINLINE)
#       define FORCEINLINE __inline
#   endif
#else
#   define FORCEINLINE __inline
#endif

/* Finds the current platform */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined( __WIN32__ ) || defined( _WIN32 ) || defined(_WINDOWS) || defined(WIN) || defined(_WIN64) || defined( __WIN64__ )
#   define DDNF_PLATFORM DDNF_PLATFORM_WIN
//////////////////////////////////////////////////////////////////////////
#elif defined( __APPLE_CC__) || defined(__APPLE__) || defined(__OSX__)
// Device                                                     Simulator
// Both requiring OS version 4.0 or greater
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 40000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 40000
#       define DDNF_PLATFORM DDNF_PLATFORM_APPLE_IOS
#   else
#       define DDNF_PLATFORM DDNF_PLATFORM_APPLE
#   endif
//////////////////////////////////////////////////////////////////////////
#elif defined(linux) && defined(__arm__)
// TODO: This is NOT the correct way to detect the Tegra 2 platform but it works for now.
// It doesn't appear that GCC defines any platform specific macros.
#   define DDNF_PLATFORM DDNF_PLATFORM_TEGRA2
#elif defined(__ANDROID__)
#   define DDNF_PLATFORM DDNF_PLATFORM_ANDROID
//////////////////////////////////////////////////////////////////////////
#elif defined( __native_client__ )
#   define DDNF_PLATFORM DDNF_PLATFORM_NACL
#   ifndef DDNF_STATIC_LIB
#       error DDNF must be built as static for NaCl (DDNF_STATIC=true in cmake)
#   endif
#   ifdef DDNF_BUILD_RENDERSYSTDDNF_D3D9
#       error d3d9 is nor supported on NaCl (ONF_BUILD_RENDERSYSTDDNF_D3D9 false in cmake)
#   endif
#   ifdef DDNF_BUILD_RENDERSYSTDDNF_GL
#       error gl is nor supported on NaCl (ONF_BUILD_RENDERSYSTDDNF_GL=false in cmake)
#   endif
#   ifndef DDNF_BUILD_RENDERSYSTDDNF_GLES2
#       error GLES2 render system is needed for NaCl (ONF_BUILD_RENDERSYSTDDNF_GLES2=false in cmake)
#   endif
#else
#   define DDNF_PLATFORM DDNF_PLATFORM_LINUX
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Find the arch type */
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#   define DDNF_ARCH_TYPE DDNF_ARCHITECTURE_64
#else
#   define DDNF_ARCH_TYPE DDNF_ARCHITECTURE_32
#endif

// For generating compiler warnings - should work on any compiler
// As a side note, if you start your message with 'Warning: ', the MSVC
// IDE actually does catch a warning :)
#define DDNF_QUOTE_INPLACE(x) # x
#define DDNF_QUOTE(x) DDNF_QUOTE_INPLACE(x)
#define DDNF_WARN( x )  message( __FILE__ "(" QUOTE( __LINE__ ) ") : " x "\n" )

//----------------------------------------------------------------------------
// Windows Settings
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#pragma warning(disable:4091) 
#include <Windows.h>
#define DDNF_EXPORT extern "C"  __declspec(dllexport)

#include <Dbghelp.h>
// If we're not including this from a client build, specify that the stuff
// should get exported. Otherwise, import it.
#   if defined( DDNF_STATIC_LIB )
// Linux compilers don't have symbol import/export directives.
#       define _DDNFExport
#       define _DDNFPrivate
#   else
#       if defined( DDNF_NONCLIENT_BUILD )
#           define _DDNFExport __declspec( dllexport )
#       else
#           if defined( __MINGW32__ )
#               define _DDNFExport
#           else
#               define _DDNFExport __declspec( dllimport )
#           endif
#       endif
#       define _DDNFPrivate
#   endif
// Win32 compilers use _DEBUG for specifying debug builds.
// for MinGW, we set DEBUG
#   if defined(_DEBUG) || defined(DEBUG)
#       define DDNF_DEBUG_MODE 1
#   endif

// Disable unicode support on MingW for GCC 3, poorly supported in stdlibc++
// STLPORT fixes this though so allow if found
// MinGW C++ Toolkit supports unicode and sets the define __MINGW32_TOOLBOX_UNICODE__ in _mingw.h
// GCC 4 is also fine
#if defined(__MINGW32__)
# if DDNF_COMP_VER < 400
#  if !defined(_STLPORT_VERSION)
#   include<_mingw.h>
#   if defined(__MINGW32_TOOLBOX_UNICODE__) || DDNF_COMP_VER > 345
#    define DDNF_UNICODE_SUPPORT 1
#   else
#    define DDNF_UNICODE_SUPPORT 0
#   endif
#  else
#   define DDNF_UNICODE_SUPPORT 1
#  endif
# else
#  define DDNF_UNICODE_SUPPORT 1
# endif
#else
#  define DDNF_UNICODE_SUPPORT 1
#endif

#endif // DDNF_PLATFORM == DDNF_PLATFORM_WIN
//----------------------------------------------------------------------------
// Android Settings
/*
#if NF_PLATFORM == NF_PLATFORM_ANDROID
#   define _NFExport
#   define NF_UNICODE_SUPPORT 1
#   define NF_DEBUG_MODE 0
#   define _NFPrivate
#     define CLOCKS_PER_SEC  1000
//  pragma def were found here: http://www.inf.pucrs.br/~eduardob/disciplinas/SistEmbarcados/Mobile/Nokia/Tools/Carbide_vs/WINSCW/Help/PDF/C_Compilers_Reference_3.2.pdf
#     pragma warn_unusedarg off
#     pragma warn_emptydecl off
#     pragma warn_possunwant off
// A quick define to overcome different names for the same function
#   define stricmp strcasecmp
#   ifdef DEBUG
#       define NF_DEBUG_MODE 1
#   else
#       define NF_DEBUG_MODE 0
#   endif
#endif
*/
//----------------------------------------------------------------------------
// Linux/Apple/iOs/Android/Symbian/Tegra2/NaCl Settings
#if DDNF_PLATFORM == DDNF_PLATFORM_LINUX || DDNF_PLATFORM == DDNF_PLATFORM_APPLE || DDNF_PLATFORM == DDNF_PLATFORM_APPLE_IOS || DDNF_PLATFORM == DDNF_PLATFORM_ANDROID || DDNF_PLATFORM == DDNF_PLATFORM_TEGRA2 || DDNF_PLATFORM == DDNF_PLATFORM_NACL

//#include <syswait.h>

// Enable GCC symbol visibility
#   if defined( DDNF_GCC_VISIBILITY )
#       define _DDNFExport  __attribute__ ((visibility("default")))
#       define _DDNFPrivate __attribute__ ((visibility("hidden")))
#   else
#       define _DDNFExport
#       define _DDNFPrivate
#   endif

// A quick define to overcome different names for the same function
#   define stricmp strcasecmp

// Unlike the Win32 compilers, Linux compilers seem to use DEBUG for when
// specifying a debug build.
// (??? this is wrong, on Linux debug builds aren't marked in any way unless
// you mark it yourself any way you like it -- zap ???)
#   if defined(_DEBUG) || defined(DEBUG)
#       define DDNF_DEBUG_MODE 1
#   endif

// Always enable unicode support for the moment
// Perhaps disable in old versions of gcc if necessary
#define DDNF_UNICODE_SUPPORT 1
#define MAX_PATH 255

#define DDNF_EXPORT extern "C" __attribute ((visibility("default")))

#endif


//----------------------------------------------------------------------------
// Endian Settings
// check for BIG_ENDIAN config flag, set NF_ENDIAN correctly
#ifdef DDNF_CONFIG_BIG_ENDIAN
#    define DDNF_ENDIAN DDNF_ENDIAN_BIG
#else
#    define DDNF_ENDIAN DDNF_ENDIAN_LITTLE
#endif

#include <stdint.h>
#include <chrono>
#include <functional>
#include <assert.h>
#include <string>
#include <map>
#include <iostream>

namespace DDNF
{
	// Integer formats of fixed bit width
	typedef uint32_t UINT32;
	typedef uint16_t UINT16;
	typedef uint8_t UINT8;
	typedef int32_t INT32;
	typedef int16_t INT16;
	typedef int8_t INT8;
	typedef uint64_t UINT64;
	typedef int64_t INT64;
	typedef int64_t SOCK;
}

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
#include <crtdbg.h>
#define DDNFASSERT(exp_, msg_, file_, func_)  \
    std::string strInfo("Message:");        \
    strInfo += msg_ + std::string(" don't exist or some warning") + std::string("\n\nFile:") + std::string(file_) + std::string("\n Function:") + func_; \
    MessageBoxA(0, strInfo.c_str(), ("Error_"#exp_), MB_RETRYCANCEL | MB_ICONERROR); \
    assert(0);
#else
#define DDNFASSERT(exp_, msg_, file_, func_)
#endif


//#define GOOGLE_GLOG_DLL_DECL=

///////////////////////////////////////////////////////////////
#include <string>
#include <algorithm>
#include <cmath>
#include <time.h>
#include <sstream>
#include <stdio.h>
//#include <common/lexical_cast.hpp>

#ifndef _MSC_VER
#include <sys/time.h>
#include <unistd.h>
#define EPOCHFILETIME 11644473600000000ULL
#else
#include <WinSock2.h>
#include <windows.h>
#include <time.h>
#include <process.h>
#define EPOCHFILETIME 11644473600000000Ui64
#endif

#define ELPP_DISABLE_DEFAULT_CRASH_HANDLING

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
#define DDNFSPRINTF sprintf_s
#define DDNFSTRICMP stricmp
#define DDNFSLEEP(s) Sleep(s) //millisecond
#define DDNFGetPID() lexical_cast<std::string>(getpid())
#else
#define DDNFSPRINTF snprintf
#define DDNFSTRICMP strcasecmp
#define DDNFSLEEP(s) usleep(s*1000) //millisecond
#define DDNFGetPID() lexical_cast<std::string>(getpid())
#endif

//#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
#ifndef DDNF_DYNAMIC_PLUGIN
#define DDNF_DYNAMIC_PLUGIN 1
#endif
//#endif

#if DDNF_PLATFORM != DDNF_PLATFORM_WIN
#ifndef DDNF_USE_COROUTINE
//#define DDNF_USE_COROUTINE 1
#endif
#endif

//using tcmalloc
//#define DDNF_USE_TCMALLOC 1

//using actor mode--begin
#define DDNF_ACTOR_THREAD_COUNT 2
#define DDNF_USE_ACTOR

//use actor mode--end

#define GET_CLASS_NAME(className) (#className)

#define DDNF_SHARE_PTR std::shared_ptr
#define DDNF_NEW new

//�����ڴ�й¶���
#define DDNF_MDDNF_LEACK_CHECK		(1)

/*
template<typename DTYPE>
bool DDNF_StrTo(const std::string& strValue, DTYPE& nValue)
{
	try
	{
		nValue = lexical_cast<DTYPE>(strValue);
		return true;
	}
	catch (...)
	{
		return false;
	}

	return false;
}*/

inline bool IsZeroFloat(const float fValue, float epsilon = 1e-6)
{
	return std::abs(fValue) <= epsilon;
}

inline bool IsZeroDouble(const double dValue, double epsilon = 1e-15)
{
	return std::abs(dValue) <= epsilon;
}

inline int64_t DDNFGetTimeMSEx()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

//millisecond
inline int64_t DDNFGetTimeMS()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

//second
inline int64_t DDNFGetTimeS()
{
	return DDNFGetTimeMS() / 1000;
}

//Protobuf Using Dlls
/*
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
#ifndef PROTOBUF_USE_DLLS
#define PROTOBUF_USE_DLLS
#endif
#endif
*/

#endif//__DDNF__PLATFORM__H__