#ifndef __I__LOG__MODULE__H__
#define __I__LOG__MODULE__H__

#include "IModule.h"

namespace DDNF
{
	class ILogModule
		: public IModule
	{

	public:

		enum LOG_LEVEL
		{
			LL_DEBUG_NORMAL,
			LL_INFO_NORMAL,
			LL_WARING_NORMAL,
			LL_ERROR_NORMAL,
			LL_FATAL_NORMAL,
		};

		//virtual bool LogElement(const LOG_LEVEL nll, const NFGUID ident, const std::string& strElement, const std::string& strDesc, const char* func = "", int line = 0) = 0;
		//virtual bool LogProperty(const LOG_LEVEL nll, const NFGUID ident, const std::string& strProperty, const std::string& strDesc, const char* func = "", int line = 0) = 0;
		//virtual bool LogObject(const LOG_LEVEL nll, const NFGUID ident, const std::string& strDesc, const char* func = "", int line = 0) = 0;
		//virtual bool LogRecord(const LOG_LEVEL nll, const NFGUID ident, const std::string& strRecord, const std::string& strDesc, const int nRow, const int nCol, const char* func = "", int line = 0) = 0;
		//virtual bool LogRecord(const LOG_LEVEL nll, const NFGUID ident, const std::string& strRecord, const std::string& strDesc, const char* func = "", int line = 0) = 0;

		//virtual bool LogNormal(const LOG_LEVEL nll, const NFGUID ident, const std::string& strInfo, const int64_t nDesc, const char* func = "", int line = 0) = 0;
		//virtual bool LogNormal(const LOG_LEVEL nll, const NFGUID ident, const std::string& strInfo, const std::string& strDesc, const  char* func = "", int line = 0) = 0;
		//virtual bool LogNormal(const LOG_LEVEL nll, const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;

		virtual bool LogDebug(const std::string& strLog, const char* func = "", int line = 0) = 0;
		virtual bool LogInfo(const std::string& strLog, const  char* func = "", int line = 0) = 0;
		virtual bool LogWarning(const std::string& strLog, const char* func = "", int line = 0) = 0;
		virtual bool LogError(const std::string& strLog, const char* func = "", int line = 0) = 0;
		virtual bool LogFatal(const std::string& strLog, const char* func = "", int line = 0) = 0;

		virtual bool LogDebug(const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
		virtual bool LogInfo(const std::ostringstream& stream, const  char* func = "", int line = 0) = 0;
		virtual bool LogWarning(const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
		virtual bool LogError(const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
		virtual bool LogFatal(const std::ostringstream& stream, const char* func = "", int line = 0) = 0;

		//virtual bool LogDebug(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0) = 0;
		//virtual bool LogInfo(const NFGUID ident, const std::string& strLog, const  char* func = "", int line = 0) = 0;
		//virtual bool LogWarning(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0) = 0;
		//virtual bool LogError(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0) = 0;
		//virtual bool LogFatal(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0) = 0;

		//virtual bool LogDebug(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
		//virtual bool LogInfo(const NFGUID ident, const std::ostringstream& stream, const  char* func = "", int line = 0) = 0;
		//virtual bool LogWarning(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
		//virtual bool LogError(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
		//virtual bool LogFatal(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;

		virtual void StackTrace() = 0;
	};
}

#endif//__I__LOG__MODULE__H__