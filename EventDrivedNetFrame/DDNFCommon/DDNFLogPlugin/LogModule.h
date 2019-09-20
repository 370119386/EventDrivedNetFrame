#ifndef __DDNF__LOG__MODULE__H__
#define __DDNF__LOG__MODULE__H__

#include "DDNFCommon/DDNFPluginModule/ILogModule.h"
#include "DDNFCommon/DDNFCore/Performance.hpp"

namespace DDNF
{
	class LogModule
		: public ILogModule
	{
	public:

		LogModule(IPluginManager* p);
		virtual ~LogModule() {}

		virtual bool Awake();
		virtual bool Init();
		virtual bool Shut();

		virtual bool BeforeShut();
		virtual bool AfterInit();

		virtual bool Execute();

		///////////////////////////////////////////////////////////////////////
		virtual void LogStack();

		//virtual bool LogElement(const NF_LOG_LEVEL nll, const NFGUID ident, const std::string& strElement, const std::string& strDesc, const char* func = "", int line = 0);
		//virtual bool LogProperty(const NF_LOG_LEVEL nll, const NFGUID ident, const std::string& strProperty, const std::string& strDesc, const char* func = "", int line = 0);
		//virtual bool LogRecord(const NF_LOG_LEVEL nll, const NFGUID ident, const std::string& strRecord, const std::string& strDesc, const int nRow, const int nCol, const char* func = "", int line = 0);
		//virtual bool LogRecord(const NF_LOG_LEVEL nll, const NFGUID ident, const std::string& strRecord, const std::string& strDesc, const char* func = "", int line = 0);
		//virtual bool LogObject(const NF_LOG_LEVEL nll, const NFGUID ident, const std::string& strDesc, const char* func = "", int line = 0);

		//virtual bool LogNormal(const NF_LOG_LEVEL nll, const NFGUID ident, const std::string& strInfo, const char* func = "", int line = 0);
		//virtual bool LogNormal(const NF_LOG_LEVEL nll, const NFGUID ident, const std::string& strInfo, const int64_t nDesc, const char* func = "", int line = 0);
		//virtual bool LogNormal(const NF_LOG_LEVEL nll, const NFGUID ident, const std::string& strInfo, const std::string& strDesc, const char* func = "", int line = 0);
		//virtual bool LogNormal(const NF_LOG_LEVEL nll, const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0);

		virtual bool LogDebug(const std::string& strLog, const char* func = "", int line = 0);
		virtual bool LogInfo(const std::string& strLog, const  char* func = "", int line = 0);
		virtual bool LogWarning(const std::string& strLog, const char* func = "", int line = 0);
		virtual bool LogError(const std::string& strLog, const char* func = "", int line = 0);
		virtual bool LogFatal(const std::string& strLog, const char* func = "", int line = 0);

		virtual bool LogDebug(const std::ostringstream& stream, const char* func = "", int line = 0);
		virtual bool LogInfo(const std::ostringstream& stream, const  char* func = "", int line = 0);
		virtual bool LogWarning(const std::ostringstream& stream, const char* func = "", int line = 0);
		virtual bool LogError(const std::ostringstream& stream, const char* func = "", int line = 0);
		virtual bool LogFatal(const std::ostringstream& stream, const char* func = "", int line = 0);

		//virtual bool LogDebug(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0);
		//virtual bool LogInfo(const NFGUID ident, const std::string& strLog, const  char* func = "", int line = 0);
		//virtual bool LogWarning(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0);
		//virtual bool LogError(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0);
		//virtual bool LogFatal(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0);

		//virtual bool LogDebug(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0);
		//virtual bool LogInfo(const NFGUID ident, const std::ostringstream& stream, const  char* func = "", int line = 0);
		//virtual bool LogWarning(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0);
		//virtual bool LogError(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0);
		//virtual bool LogFatal(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0);

		//virtual bool LogDebugFunctionDump(const NFGUID ident, const int nMsg, const std::string& strArg, const char* func = "", const int line = 0);
		virtual bool ChangeLogLevel(const std::string& strLevel);

		virtual void StackTrace();

	protected:
		virtual bool Log(const LOG_LEVEL nll, const char* format, ...);

		static bool CheckLogFileExist(const char* filename);
		static void rolloutHandler(const char* filename, std::size_t size);

	private:
		static unsigned int idx;
		uint64_t mnLogCountTotal;
		std::list<Performance> mxPerformanceList;
	};
}
#endif//__DDNF__LOG__MODULE__H__