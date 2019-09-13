#ifndef __I__PLUGIN__MANAGER__H__
#define __I__PLUGIN__MANAGER__H__

#include <functional>
#include <list>
#include "Platform.h"

typedef std::function<bool (const std::string& strFileName, std::string& strContent)> GET_FILECONTENT_FUNCTOR;
typedef void (* CoroutineFunction)(void* arg);
typedef void(*AsyncFunction)(const std::string& strData, std::string& strContent);

template<typename DerivedType, typename BaseType>
class TIsDerived
{
public:
    static int AnyFunction(BaseType* base)
    {
        return 1;
    }

    static  char AnyFunction(void* t2)
    {
        return 0;
    }

    enum
    {
        Result = (sizeof(int) == sizeof(AnyFunction((DerivedType*)NULL))),
    };
};

#define FIND_MODULE(classBaseName, className)  \
	assert((TIsDerived<classBaseName, IModule>::Result));


namespace DDNF
{
	class IPlugin;
	class IModule;

	class IPluginManager
	{
	public:
		IPluginManager()
		{

		}

		/////////////////////

		virtual bool LoadPlugin()
		{
			return true;
		}

		virtual bool Awake()
		{
			return true;
		}

		virtual bool Init()
		{

			return true;
		}

		virtual bool AfterInit()
		{
			return true;
		}

		virtual bool CheckConfig()
		{
			return true;
		}

		virtual bool ReadyExecute()
		{
			return true;
		}

		virtual bool Execute()
		{
			return true;
		}

		virtual bool BeforeShut()
		{
			return true;
		}

		virtual bool Shut()
		{
			return true;
		}

		virtual bool Finalize()
		{
			return true;
		}

		virtual bool OnReloadPlugin()
		{
			return true;
		}

		/////////////////////

		template <typename T>
		T* FindModule()
		{
			IModule* pLogicModule = FindModule(typeid(T).name());
			if (pLogicModule)
			{
				if (!TIsDerived<T, IModule>::Result)
				{
					return NULL;
				}
				//TODO OSX上dynamic_cast返回了NULL
#if DDNF_PLATFORM == DDNF_PLATFORM_APPLE
				T * pT = (T*)pLogicModule;
#else
				T* pT = dynamic_cast<T*>(pLogicModule);
#endif
				assert(NULL != pT);

				return pT;
			}
			assert(NULL);
			return NULL;
		}

		virtual bool ReLoadPlugin(const std::string& strPluginDLLName) = 0;

		virtual void Registered(IPlugin* plugin) = 0;

		virtual void UnRegistered(IPlugin* plugin) = 0;

		virtual IPlugin* FindPlugin(const std::string& strPluginName) = 0;

		//virtual void AddModule(const std::string& strModuleName, IModule* pModule) = 0;

		//virtual void AddTestModule(const std::string& strModuleName, IModule* pModule) = 0;

		//virtual void RemoveModule(const std::string& strModuleName) = 0;

		virtual IModule* FindModule(const std::string& strModuleName) = 0;

		//virtual IModule* FindTestModule(const std::string& strModuleName) = 0;

		//virtual std::list<IModule*> Modules() = 0;

		//virtual int GetAppID() const = 0;
		//virtual void SetAppID(const int nAppID) = 0;

		//virtual bool IsRunningDocker() const = 0;
		//virtual void SetRunningDocker(bool bDocker) = 0;

		//virtual bool IsStaticPlugin() const = 0;

		//virtual INT64 GetInitTime() const = 0;
		//virtual INT64 GetNowTime() const = 0;

		//virtual const std::string& GetConfigPath() const = 0;
		//virtual void SetConfigPath(const std::string& strPath) = 0;

		//virtual void SetConfigName(const std::string& strFileName) = 0;
		//virtual const std::string& GetConfigName() const = 0;

		//virtual const std::string& GetAppName() const = 0;
		//virtual void SetAppName(const std::string& strAppName) = 0;

		//virtual const std::string& GetLogConfigName() const = 0;
		//virtual void SetLogConfigName(const std::string& strName) = 0;

		//virtual IPlugin* GetCurrentPlugin() = 0;
		//virtual IModule* GetCurrenModule() = 0;

		//virtual void SetCurrentPlugin(IPlugin* pPlugin) = 0;
		//virtual void SetCurrenModule(IModule* pModule) = 0;


		//virtual void SetGetFileContentFunctor(GET_FILECONTENT_FUNCTOR fun) = 0;
		//virtual bool GetFileContent(const std::string& strFileName, std::string& strContent) = 0;

		//virtual void ExecuteCoScheduler() = 0;
		//virtual void YieldCo(const int64_t nSecond) = 0;
		//virtual void YieldCo() = 0;
		//virtual void Async(AsyncFunction fun) = 0;
	};
}

#endif//__I__PLUGIN__MANAGER__H__
