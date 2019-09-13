#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <future>
#include <functional>
#include <atomic>
#include <cstring>
#include "PluginManager.h"
//#include "NFComm/NFCore/NFException.h"
#include "DDNFCommon/DDNFPluginModule/Platform.h"
//#include "NFComm/NFLogPlugin/easylogging++.h"

#if DDNF_PLATFORM != DDNF_PLATFORM_WIN
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <execinfo.h>
#include <setjmp.h>

#if DDNF_PLATFORM == DDNF_PLATFORM_LINUX
#include <sys/prctl.h>
#endif

#endif

bool bExitApp = false;
std::thread gThread;
std::string strArgvList;
std::string strPluginName;
std::string strDataPath;
std::string strAppName;
std::string strAppID;
std::string strTitleName;


void MainExecute();

void ReleaseNF()
{
	DDNF::PluginManager::Instance()->BeforeShut();
	DDNF::PluginManager::Instance()->Shut();
	DDNF::PluginManager::Instance()->Finalize();

	DDNF::PluginManager::Instance()->Release();
}

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
#pragma comment( lib, "DbgHelp" )
bool ApplicationCtrlHandler(DWORD fdwctrltype)
{
	switch (fdwctrltype)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	{
		bExitApp = true;
	}
		return true;
	default:
		return false;
	}
}

void CreateDumpFile(const std::string& strDumpFilePathName, EXCEPTION_POINTERS* pException)
{
    //Dump
    HANDLE hDumpFile = CreateFile(strDumpFilePathName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

    CloseHandle(hDumpFile);
}

long ApplicationCrashHandler(EXCEPTION_POINTERS* pException)
{
    time_t t = time(0);
    char szDmupName[MAX_PATH];
    tm* ptm = localtime(&t);

    sprintf_s(szDmupName, "%04d_%02d_%02d_%02d_%02d_%02d.dmp",  ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    CreateDumpFile(szDmupName, pException);

    FatalAppExit(-1,  szDmupName);

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void CloseXButton()
{
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
	HWND hWnd = GetConsoleWindow();
	if (hWnd)
	{
		HMENU hMenu = GetSystemMenu(hWnd, FALSE);
		EnableMenuItem(hMenu, SC_CLOSE, MF_DISABLED | MF_BYCOMMAND);
	}
#endif
}

void ThreadFunc()
{
    while (!bExitApp)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        std::string s;
		std::getline(std::cin, s);
        if ( 0 == _stricmp( s.c_str(), "exit" ) )
        {
            bExitApp = true;
            gThread.detach();
        }
		else if (s.find("show") == 0)
		{
		//	NFDataList data;
		//	data.Split(s, " ");
		//	NFGUID id(data.String(1));
		}
    }
}

void CreateBackThread()
{
    gThread = std::thread(std::bind(&ThreadFunc));
    auto f = std::async (std::launch::deferred, std::bind(ThreadFunc));
    std::cout << "CreateBackThread, thread ID = " << gThread.get_id() << std::endl;
}

void InitDaemon()
{
#if DDNF_PLATFORM != DDNF_PLATFORM_WIN
    daemon(1, 0);

    // ignore signals
    signal(SIGINT,  SIG_IGN);
    signal(SIGHUP,  SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
#endif
}

void PrintfLogo()
{
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif

    std::cout << "\n" << std::endl;
    std::cout << "************************************************" << std::endl;
    std::cout << "**                                            **" << std::endl;
    std::cout << "**                 DataDrivedNetFrame         **" << std::endl;
    std::cout << "**   Copyright (c) 2011-2018, ShaoJun.Shen    **" << std::endl;
    std::cout << "**             All rights reserved.           **" << std::endl;
    std::cout << "**                                            **" << std::endl;
    std::cout << "************************************************" << std::endl;
    std::cout << "\n" << std::endl;
	std::cout << "-d Run itas daemon mode, only on linux" << std::endl;
	std::cout << "-x Close the 'X' button, only on windows" << std::endl;
	std::cout << "Instance: name.xml File's name to instead of \"Plugin.xml\" when programs be launched, all platform" << std::endl;
	std::cout << "Instance: \"ID=number\", \"Server=GameServer\"  when programs be launched, all platform" << std::endl;
	std::cout << "\n" << std::endl;

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}


void ProcessParameter(int argc, char* argv[])
{
    for (int i = 0; i < argc; i++)
	{
		strArgvList += " ";
		strArgvList += argv[i];
	}

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ApplicationCtrlHandler, true);

	if (strArgvList.find("-x") != std::string::npos)
	{
		CloseXButton();
	}
#else
    //run it as a daemon process
	if (strArgvList.find("-d") != std::string::npos)
	{
		InitDaemon();
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
#endif

    if (strArgvList.find("Path=") != std::string::npos)
    {
        for (int i = 0; i < argc; i++)
        {
            strDataPath = argv[i];
            if (strDataPath.find("Path=") != std::string::npos)
            {
                strDataPath.erase(0, 5);
                break;
            }
        }

		//DDNF::PluginManager::Instance()->SetConfigPath(strDataPath);
    }
    else
    {
        //NFPluginManager::GetSingletonPtr()->SetConfigPath("../");
    }

	if (strArgvList.find(".xml") != std::string::npos)
	{
		for (int i = 0; i < argc; i++)
		{
			strPluginName = argv[i];
			if (strPluginName.find(".xml") != std::string::npos)
			{
				break;
			}
		}

		//DDNF::PluginManager::Instance()->SetConfigName(strPluginName);
	}

    if (strArgvList.find("Server=") != std::string::npos)
	{
		for (int i = 0; i < argc; i++)
		{
			strAppName = argv[i];
			if (strAppName.find("Server=") != std::string::npos)
			{
                strAppName.erase(0, 7);
				break;
			}
		}

		//DDNF::PluginManager::Instance()->SetAppName(strAppName);
	}

	if (strArgvList.find("ID=") != std::string::npos)
	{
		for (int i = 0; i < argc; i++)
		{
			strAppID = argv[i];
			if (strAppID.find("ID=") != std::string::npos)
			{
                strAppID.erase(0, 3);
				break;
			}
		}

		int nAppID = 0;
        //if(DDNF_StrTo(strAppID, nAppID))
        //{
        //    NFPluginManager::GetSingletonPtr()->SetAppID(nAppID);
        //}
	}

	if (strArgvList.find("Docker=") != std::string::npos)
	{
		std::string strDockerFlag = "0";
		for (int i = 0; i < argc; i++)
		{
			strDockerFlag = argv[i];
			if (strDockerFlag.find("Docker=") != std::string::npos)
			{
                strDockerFlag.erase(0, 7);
				break;
			}
		}

		int nDockerFlag = 0;
        //if(DDNF_StrTo(strDockerFlag, nDockerFlag))
        //{
        //    NFPluginManager::GetSingletonPtr()->SetRunningDocker(nDockerFlag);
        //}
	}
	
	strTitleName = strAppName + strAppID;// +" PID" + NFGetPID();
	strTitleName.replace(strTitleName.find("Server"), 6, "");
	strTitleName = "NF" + strTitleName;
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
	SetConsoleTitle(strTitleName.c_str());
#elif DDNF_PLATFORM == DDNF_PLATFORM_LINUX
	prctl(PR_SET_NAME, strTitleName.c_str());
	//setproctitle(strTitleName.c_str());
#endif
}

#if DDNF_PLATFORM != DDNF_PLATFORM_WIN
void NFCrashHandler(int sig) {
	// FOLLOWING LINE IS ABSOLUTELY NEEDED AT THE END IN ORDER TO ABORT APPLICATION
	//el::base::debug::StackTrace();
	//el::Helpers::logCrashReason(sig, true);


	//LOG(FATAL) << "crash sig:" << sig;

	int size = 16;
	void * array[16];
	int stack_num = backtrace(array, size);
	char ** stacktrace = backtrace_symbols(array, stack_num);
	for (int i = 0; i < stack_num; ++i)
	{
		//printf("%s\n", stacktrace[i]);
		//LOG(FATAL) << stacktrace[i];
	}

	free(stacktrace);
}
#endif

void MainExecute()
{

	uint64_t nIndex = 0;
    while (!bExitApp)
    {
		nIndex++;

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
        __try
#else
		try
#endif
		{
#ifdef DDNF_COROUTINE
			//NFPluginManager::Instance()->ExecuteCoScheduler();
#else
			//NFPluginManager::GetSingletonPtr()->Execute();
#endif
		}
#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
        __except (ApplicationCrashHandler(GetExceptionInformation()))
        {
        }
#else
	catch(const std::exception& e)
	{
		//NFException::StackTrace(11);
	}
	catch(...)
	{
		//NFException::StackTrace(11);
	}
#endif
    }
}

int main(int argc, char* argv[])
{
	std::cout << "__cplusplus:" << __cplusplus << std::endl;

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
#else
	signal(SIGSEGV, NFCrashHandler);
	//el::Helpers::setCrashHandler(CrashHandler);
#endif

    //ProcessParameter(argc, argv);


	PrintfLogo();
	//CreateBackThread();

	DDNF::PluginManager::Instance()->LoadPlugin();
	DDNF::PluginManager::Instance()->Awake();
	DDNF::PluginManager::Instance()->Init();
	DDNF::PluginManager::Instance()->AfterInit();
	DDNF::PluginManager::Instance()->CheckConfig();
	DDNF::PluginManager::Instance()->ReadyExecute();

#if DDNF_PLATFORM == DDNF_PLATFORM_WIN
	MainExecute();
#else
	while (1)
	{
		//DDNF_CRASH_TRY
		MainExecute();
		//DDNF_CRASH_END_TRY
	}
#endif

	

	ReleaseNF();

    return 0;
}
