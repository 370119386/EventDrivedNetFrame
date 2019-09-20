#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <stdarg.h>
#include "LogModule.h"
#include "easylogging++.h"
#include "LogPlugin.h"
#include "termcolor.hpp"

#if DDNF_PLATFORM != DDNF_PLATFORM_WIN
#include <execinfo.h>
#endif

INITIALIZE_EASYLOGGINGPP

using namespace DDNF;

unsigned int LogModule::idx = 0;

bool LogModule::CheckLogFileExist(const char* filename)
{
    std::stringstream stream;
    stream << filename << "." << ++idx;
    std::fstream file;
    file.open(stream.str(), std::ios::in);
    if (file)
    {
        return CheckLogFileExist(filename);
    }

    return false;
}

void LogModule::rolloutHandler(const char* filename, std::size_t size)
{
    std::stringstream stream;
    if (!CheckLogFileExist(filename))
    {
        stream << filename << "." << idx;
        rename(filename, stream.str().c_str());
    }
}

LogModule::LogModule(IPluginManager* p)
{
    m_pPluginManager = p;
}

bool LogModule::Awake()
{
	mnLogCountTotal = 0;

	el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
	el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);

	std::string strLogConfigName = m_pPluginManager->GetLogConfigName();
	if (strLogConfigName.empty())
	{
		strLogConfigName = m_pPluginManager->GetAppName();
	}

	string strAppLogName = "";
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
#ifdef DDNF_DEBUG_MODE
	strAppLogName = m_pPluginManager->GetConfigPath() + "DataCfg/Debug/logconfig/" + strLogConfigName + "_win.conf";
#else
	strAppLogName = m_pPluginManager->GetConfigPath() + "DataCfg/Release/logconfig/" + strLogConfigName + "_win.conf";
#endif

#else
#ifdef DDNF_DEBUG_MODE
	strAppLogName = m_pPluginManager->GetConfigPath() + "DataCfg/Debug/logconfig/" + strLogConfigName + ".conf";
#else
	strAppLogName = m_pPluginManager->GetConfigPath() + "DataCfg/Release/logconfig/" + strLogConfigName + ".conf";
#endif
#endif

	el::Configurations conf(strAppLogName);

	el::Configuration* pConfiguration = conf.get(el::Level::Debug, el::ConfigurationType::Filename);
	const std::string& strFileName = pConfiguration->value();
	pConfiguration->setValue(m_pPluginManager->GetConfigPath() + strFileName);

	std::cout << "LogConfig: " << strAppLogName << std::endl;

	el::Loggers::reconfigureAllLoggers(conf);
	el::Helpers::installPreRollOutCallback(rolloutHandler);

	return true;
}

bool LogModule::Init()
{
   
    return true;
}

bool LogModule::Shut()
{
    el::Helpers::uninstallPreRollOutCallback();

    return true;
}

bool LogModule::BeforeShut()
{
    return true;

}

bool LogModule::AfterInit()
{

    return true;
}

bool LogModule::Execute()
{
    return true;

}

bool LogModule::Log(const LOG_LEVEL nll, const char* format, ...)
{
    mnLogCountTotal++;

    char szBuffer[1024 * 10] = {0};

    va_list args;
    va_start(args, format);
    vsnprintf(szBuffer, sizeof(szBuffer) - 1, format, args);
    va_end(args);

    switch (nll)
    {
        case ILogModule::LL_DEBUG_NORMAL:
			{
				std::cout << termcolor::green;
				LOG(DEBUG) << mnLogCountTotal << " | " << m_pPluginManager->GetAppID()<< " | " << szBuffer;
			}
			break;
        case ILogModule::LL_INFO_NORMAL:
			{
				std::cout << termcolor::green;
				LOG(INFO) << mnLogCountTotal << " | " << m_pPluginManager->GetAppID() << " | " << szBuffer;
			}	
			break;
        case ILogModule::LL_WARING_NORMAL:
			{
				std::cout << termcolor::yellow;
				LOG(WARNING) << mnLogCountTotal << " | " << m_pPluginManager->GetAppID() << " | " << szBuffer;
			}
			break;
        case ILogModule::LL_ERROR_NORMAL:
			{
				std::cout << termcolor::red;
				LOG(ERROR) << mnLogCountTotal << " | " << m_pPluginManager->GetAppID() << " | " << szBuffer;
				//LogStack();
			}
			break;
        case ILogModule::LL_FATAL_NORMAL:
			{
				std::cout << termcolor::red;
				LOG(FATAL) << mnLogCountTotal << " | " << m_pPluginManager->GetAppID() << " | " << szBuffer;
			}
			break;
        default:
			{
				std::cout << termcolor::green;
				LOG(INFO) << mnLogCountTotal << " | " << m_pPluginManager->GetAppID() << " | " << szBuffer;
			}
			break;
    }

	std::cout<<termcolor::reset;

    return true;
}

//bool LogModule::LogElement(const LOG_LEVEL nll, const NFGUID ident, const std::string& strElement, const std::string& strDesc, const char* func, int line)
//{
//    if (line > 0)
//    {
//        Log(nll, "[ELEMENT] Indent[%s] Element[%s] %s %s %d", ident.ToString().c_str(), strElement.c_str(), strDesc.c_str(), func, line);
//    }
//    else
//    {
//        Log(nll, "[ELEMENT] Indent[%s] Element[%s] %s", ident.ToString().c_str(), strElement.c_str(), strDesc.c_str());
//    }
//
//    return true;
//}
//
//bool LogModule::LogProperty(const LOG_LEVEL nll, const NFGUID ident, const std::string& strProperty, const std::string& strDesc, const char* func, int line)
//{
//    if (line > 0)
//    {
//        Log(nll, "[PROPERTY] Indent[%s] Property[%s] %s %s %d", ident.ToString().c_str(), strProperty.c_str(), strDesc.c_str(), func, line);
//    }
//    else
//    {
//        Log(nll, "[PROPERTY] Indent[%s] Property[%s] %s", ident.ToString().c_str(), strProperty.c_str(), strDesc.c_str());
//    }
//
//    return true;
//
//}
//
//bool LogModule::LogRecord(const LOG_LEVEL nll, const NFGUID ident, const std::string& strRecord, const std::string& strDesc, const int nRow, const int nCol, const char* func, int line)
//{
//    if (line > 0)
//    {
//        Log(nll, "[RECORD] Indent[%s] Record[%s] Row[%d] Col[%d] %s %s %d", ident.ToString().c_str(), strRecord.c_str(), nRow, nCol, strDesc.c_str(), func, line);
//    }
//    else
//    {
//        Log(nll, "[RECORD] Indent[%s] Record[%s] Row[%d] Col[%d] %s", ident.ToString().c_str(), strRecord.c_str(), nRow, nCol, strDesc.c_str());
//    }
//
//    return true;
//
//}
//
//bool LogModule::LogRecord(const LOG_LEVEL nll, const NFGUID ident, const std::string& strRecord, const std::string& strDesc, const char* func, int line)
//{
//    if (line > 0)
//    {
//        Log(nll, "[RECORD] Indent[%s] Record[%s] %s %s %d", ident.ToString().c_str(), strRecord.c_str(), strDesc.c_str(), func, line);
//    }
//    else
//    {
//        Log(nll, "[RECORD] Indent[%s] Record[%s] %s", ident.ToString().c_str(), strRecord.c_str(), strDesc.c_str());
//    }
//
//    return true;
//}
//
//bool LogModule::LogObject(const LOG_LEVEL nll, const NFGUID ident, const std::string& strDesc, const char* func, int line)
//{
//    if (line > 0)
//    {
//        Log(nll, "[OBJECT] Indent[%s] %s %s %d", ident.ToString().c_str(), strDesc.c_str(), func, line);
//    }
//    else
//    {
//        Log(nll, "[OBJECT] Indent[%s] %s", ident.ToString().c_str(), strDesc.c_str());
//    }
//
//    return true;
//
//}

void LogModule::LogStack()
{

    //To Add
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
    time_t t = time(0);
    char szDmupName[MAX_PATH];
    tm* ptm = localtime(&t);

    sprintf(szDmupName, "%d_%d_%d_%d_%d_%d.dmp",  ptm->tm_year + 1900, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    
    HANDLE hDumpFile = CreateFileA(szDmupName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    //dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;

    
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

    CloseHandle(hDumpFile);
#else
	int size = 16;
	void * array[16];
	int stack_num = backtrace(array, size);
	char ** stacktrace = backtrace_symbols(array, stack_num);
	for (int i = 0; i < stack_num; ++i)
	{
		//printf("%s\n", stacktrace[i]);
        
		LOG(FATAL) << stacktrace[i];
	}

	free(stacktrace);
#endif
 
    
}

//bool LogModule::LogNormal(const LOG_LEVEL nll, const NFGUID ident, const std::string& strInfo, const std::string& strDesc, const char* func, int line)
//{
//    if (line > 0)
//    {
//        Log(nll, "Indent[%s] %s %s %s %d", ident.ToString().c_str(), strInfo.c_str(), strDesc.c_str(), func, line);
//    }
//    else
//    {
//        Log(nll, "Indent[%s] %s %s", ident.ToString().c_str(), strInfo.c_str(), strDesc.c_str());
//    }
//
//    return true;
//}
//
//bool LogModule::LogNormal(const LOG_LEVEL nll, const NFGUID ident, const std::string& strInfo, const int64_t nDesc, const char* func, int line)
//{
//    if (line > 0)
//    {
//        Log(nll, "Indent[%s] %s %lld %s %d", ident.ToString().c_str(), strInfo.c_str(), nDesc, func, line);
//    }
//    else
//    {
//        Log(nll, "Indent[%s] %s %lld", ident.ToString().c_str(), strInfo.c_str(), nDesc);
//    }
//
//    return true;
//}
//
//bool LogModule::LogNormal(const LOG_LEVEL nll, const NFGUID ident, const std::ostringstream& stream, const char* func, int line)
//{
//    if (line > 0)
//    {
//        Log(nll, "Indent[%s] %s %s %d", ident.ToString().c_str(), stream.str().c_str(), func, line);
//    }
//    else
//    {
//        Log(nll, "Indent[%s] %s", ident.ToString().c_str(), stream.str().c_str());
//    }
//
//    return true;
//}
//
//bool LogModule::LogNormal(const LOG_LEVEL nll, const NFGUID ident, const std::string& strInfo, const char* func /*= ""*/, int line /*= 0*/)
//{
//    if (line > 0)
//    {
//        Log(nll, "Indent[%s] %s %s %d", ident.ToString().c_str(), strInfo.c_str(), func, line);
//    }
//    else
//    {
//        Log(nll, "Indent[%s] %s", ident.ToString().c_str(), strInfo.c_str());
//    }
//
//    return true;
//}
//
//bool LogModule::LogDebugFunctionDump(const NFGUID ident, const int nMsg, const std::string& strArg,  const char* func /*= ""*/, const int line /*= 0*/)
//{
//    //#ifdef NF_DEBUG_MODE
//    LogNormal(ILogModule::LL_WARING_NORMAL, ident, strArg + "MsgID:", nMsg, func, line);
//    //#endif
//    return true;
//}

bool LogModule::ChangeLogLevel(const std::string& strLevel)
{
    el::Level logLevel = el::LevelHelper::convertFromString(strLevel.c_str());
    el::Logger* pLogger = el::Loggers::getLogger("default");
    if (NULL == pLogger)
    {
        return false;
    }

    el::Configurations* pConfigurations = pLogger->configurations();
    if (NULL == pConfigurations)
    {
        return false;
    }

    switch (logLevel)
    {
        case el::Level::Fatal:
        {
            el::Configuration errorConfiguration(el::Level::Error, el::ConfigurationType::Enabled, "false");
            pConfigurations->set(&errorConfiguration);
        }
        case el::Level::Error:
        {
            el::Configuration warnConfiguration(el::Level::Warning, el::ConfigurationType::Enabled, "false");
            pConfigurations->set(&warnConfiguration);
        }
        case el::Level::Warning:
        {
            el::Configuration infoConfiguration(el::Level::Info, el::ConfigurationType::Enabled, "false");
            pConfigurations->set(&infoConfiguration);
        }
        case el::Level::Info:
        {
            el::Configuration debugConfiguration(el::Level::Debug, el::ConfigurationType::Enabled, "false");
            pConfigurations->set(&debugConfiguration);

        }
        case el::Level::Debug:
            break;
        default:
            break;
    }

    el::Loggers::reconfigureAllLoggers(*pConfigurations);
    //LogNormal(+ILogModule::LL_INFO_NORMAL, NFGUID(), "[Log] Change log level", strLevel, __FUNCTION__, __LINE__);
    return true;
}

bool LogModule::LogDebug(const std::string& strLog, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL::LL_DEBUG_NORMAL, "%s %s %d", strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL::LL_DEBUG_NORMAL, "%s", strLog.c_str());
     }

     return true;
}

bool LogModule::LogInfo(const std::string& strLog, const  char* func, int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL::LL_INFO_NORMAL, "%s %s %d", strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL::LL_INFO_NORMAL, "%s", strLog.c_str());
     }

     return true;
}

bool LogModule::LogWarning(const std::string& strLog, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL::LL_WARING_NORMAL, "%s %s %d", strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL::LL_WARING_NORMAL, "%s", strLog.c_str());
     }

     return true;
}

bool LogModule::LogError(const std::string& strLog, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL::LL_ERROR_NORMAL, "%s %s %d", strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL::LL_ERROR_NORMAL, "%s", strLog.c_str());
     }

     return true;
}

bool LogModule::LogFatal(const std::string& strLog, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL::LL_FATAL_NORMAL, "%s %s %d", strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL::LL_FATAL_NORMAL, "%s", strLog.c_str());
     }

     return true;
}


bool LogModule::LogDebug(const std::ostringstream& stream, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL::LL_DEBUG_NORMAL, "%s %s %d", stream.str().c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL::LL_DEBUG_NORMAL, "%s", stream.str().c_str());
     }

     return true;
}

bool LogModule::LogInfo(const std::ostringstream& stream, const  char* func, int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL::LL_INFO_NORMAL, "%s %s %d", stream.str().c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL::LL_INFO_NORMAL, "%s", stream.str().c_str());
     }

     return true;
}

bool LogModule::LogWarning(const std::ostringstream& stream, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL::LL_WARING_NORMAL, "%s %s %d", stream.str().c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL::LL_WARING_NORMAL, "%s", stream.str().c_str());
     }

     return true;
}

bool LogModule::LogError(const std::ostringstream& stream, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL::LL_ERROR_NORMAL, "%s %s %d", stream.str().c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL::LL_ERROR_NORMAL, "%s", stream.str().c_str());
     }

     return true;
}

bool LogModule::LogFatal(const std::ostringstream& stream, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL::LL_FATAL_NORMAL, "%s %s %d", stream.str().c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL::LL_FATAL_NORMAL, "%s", stream.str().c_str());
     }

     return true;
}


//bool LogModule::LogDebug(const NFGUID ident, const std::string& strLog, const char* func , int line)
//{
//    if (line > 0)
//     {
//         Log(NF_LOG_LEVEL::NLL_DEBUG_NORMAL, "Indent[%s] %s %s %d", ident.ToString().c_str(), strLog.c_str(), func, line);
//     }
//     else
//     {
//         Log(NF_LOG_LEVEL::NLL_DEBUG_NORMAL, "Indent[%s] %s", ident.ToString().c_str(), strLog.c_str());
//     }
//
//     return true;
//}
//
//bool LogModule::LogInfo(const NFGUID ident, const std::string& strLog, const  char* func, int line)
//{
//    if (line > 0)
//     {
//         Log(NF_LOG_LEVEL::NLL_INFO_NORMAL, "Indent[%s] %s %s %d", ident.ToString().c_str(), strLog.c_str(), func, line);
//     }
//     else
//     {
//         Log(NF_LOG_LEVEL::NLL_INFO_NORMAL, "Indent[%s] %s", ident.ToString().c_str(), strLog.c_str());
//     }
//
//     return true;
//}
//
//bool LogModule::LogWarning(const NFGUID ident, const std::string& strLog, const char* func , int line)
//{
//    if (line > 0)
//     {
//         Log(NF_LOG_LEVEL::NLL_WARING_NORMAL, "Indent[%s] %s %s %d", ident.ToString().c_str(), strLog.c_str(), func, line);
//     }
//     else
//     {
//         Log(NF_LOG_LEVEL::NLL_WARING_NORMAL, "Indent[%s] %s", ident.ToString().c_str(), strLog.c_str());
//     }
//
//     return true;
//}
//
//bool LogModule::LogError(const NFGUID ident, const std::string& strLog, const char* func , int line)
//{
//     if (line > 0)
//     {
//         Log(NF_LOG_LEVEL::NLL_ERROR_NORMAL, "Indent[%s] %s %s %d", ident.ToString().c_str(), strLog.c_str(), func, line);
//     }
//     else
//     {
//         Log(NF_LOG_LEVEL::NLL_ERROR_NORMAL, "Indent[%s] %s", ident.ToString().c_str(), strLog.c_str());
//     }
//
//     return true;
//}
//
//bool LogModule::LogFatal(const NFGUID ident, const std::string& strLog, const char* func , int line)
//{
//     if (line > 0)
//     {
//         Log(NF_LOG_LEVEL::NLL_FATAL_NORMAL, "Indent[%s] %s %s %d", ident.ToString().c_str(), strLog.c_str(), func, line);
//     }
//     else
//     {
//         Log(NF_LOG_LEVEL::NLL_FATAL_NORMAL, "Indent[%s] %s", ident.ToString().c_str(), strLog.c_str());
//     }
//
//     return true;
//}
//
//
//bool LogModule::LogDebug(const NFGUID ident, const std::ostringstream& stream, const char* func , int line)
//{
//    if (line > 0)
//    {
//        Log(NF_LOG_LEVEL::NLL_DEBUG_NORMAL, "Indent[%s] %s %s %d", ident.ToString().c_str(), stream.str().c_str(), func, line);
//    }
//    else
//    {
//        Log(NF_LOG_LEVEL::NLL_DEBUG_NORMAL, "Indent[%s] %s", ident.ToString().c_str(), stream.str().c_str());
//    }
//
//    return true;
//}
//
//bool LogModule::LogInfo(const NFGUID ident, const std::ostringstream& stream, const  char* func, int line)
//{
//    if (line > 0)
//    {
//        Log(NF_LOG_LEVEL::NLL_INFO_NORMAL, "Indent[%s] %s %s %d", ident.ToString().c_str(), stream.str().c_str(), func, line);
//    }
//    else
//    {
//        Log(NF_LOG_LEVEL::NLL_INFO_NORMAL, "Indent[%s] %s", ident.ToString().c_str(), stream.str().c_str());
//    }
//
//    return true;
//}
//
//bool LogModule::LogWarning(const NFGUID ident, const std::ostringstream& stream, const char* func , int line)
//{
//    if (line > 0)
//    {
//        Log(NF_LOG_LEVEL::NLL_WARING_NORMAL, "Indent[%s] %s %s %d", ident.ToString().c_str(), stream.str().c_str(), func, line);
//    }
//    else
//    {
//        Log(NF_LOG_LEVEL::NLL_WARING_NORMAL, "Indent[%s] %s", ident.ToString().c_str(), stream.str().c_str());
//    }
//
//    return true;
//}
//
//bool LogModule::LogError(const NFGUID ident, const std::ostringstream& stream, const char* func , int line)
//{
//    if (line > 0)
//    {
//        Log(NF_LOG_LEVEL::NLL_ERROR_NORMAL, "Indent[%s] %s %s %d", ident.ToString().c_str(), stream.str().c_str(), func, line);
//    }
//    else
//    {
//        Log(NF_LOG_LEVEL::NLL_ERROR_NORMAL, "Indent[%s] %s", ident.ToString().c_str(), stream.str().c_str());
//    }
//
//    return true;
//}
//
//bool LogModule::LogFatal(const NFGUID ident, const std::ostringstream& stream, const char* func , int line)
//{
//    if (line > 0)
//    {
//        Log(NF_LOG_LEVEL::NLL_FATAL_NORMAL, "Indent[%s] %s %s %d", ident.ToString().c_str(), stream.str().c_str(), func, line);
//    }
//    else
//    {
//        Log(NF_LOG_LEVEL::NLL_FATAL_NORMAL, "Indent[%s] %s", ident.ToString().c_str(), stream.str().c_str());
//    }
//
//    return true;
//}

void LogModule::StackTrace(/*const NF_LOG_LEVEL nll = NFILogModule::NLL_FATAL_NORMAL*/)
{
#if DDNF_PLATFORM != DDNF_PLATFORM_WIN

    int size = 8;
    void * array[8];
    int stack_num = backtrace(array, size);
    char ** stacktrace = backtrace_symbols(array, stack_num);
    for (int i = 0; i < stack_num; ++i)
    {
    	//printf("%s\n", stacktrace[i]);
    	LOG(FATAL) << stacktrace[i];
    }

    free(stacktrace);
#else

	static const int MAX_STACK_FRAMES = 8;
	
	void *pStack[MAX_STACK_FRAMES];
 
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);
	WORD frames = CaptureStackBackTrace(0, MAX_STACK_FRAMES, pStack, NULL);
 
	LOG(FATAL) << "stack traceback: " << std::endl;
	for (WORD i = 0; i < frames; ++i) {
		DWORD64 address = (DWORD64)(pStack[i]);
 
		DWORD64 displacementSym = 0;
		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = MAX_SYM_NAME;
 
		DWORD displacementLine = 0;
		IMAGEHLP_LINE64 line;
		//SymSetOptions(SYMOPT_LOAD_LINES);
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
 
		if (SymFromAddr(process, address, &displacementSym, pSymbol)
		 && SymGetLineFromAddr64(process, address, &displacementLine, &line)) {
			LOG(FATAL) << "\t" << pSymbol->Name << " at " << line.FileName << ":" << line.LineNumber << "(0x" << std::hex << pSymbol->Address << std::dec << ")" << std::endl;
		}
		else {
			LOG(FATAL) << "\terror: " << GetLastError() << std::endl;
		}
	}
#endif
}
