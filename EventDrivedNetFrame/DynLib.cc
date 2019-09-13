#include "DynLib.h"
#include <string>

bool DDNF::DynLib::Load()
{
    std::string strLibPath = "./";
    strLibPath += mstrName;
    mInst = (DYNLIB_HANDLE)DYNLIB_LOAD(strLibPath.c_str());

    return mInst != 0;
}

bool DDNF::DynLib::UnLoad()
{
    DYNLIB_UNLOAD(mInst);
    return true;
}

void* DDNF::DynLib::GetSymbol(const char* szProcName)
{
    return (DYNLIB_HANDLE)DYNLIB_GETSYM(mInst, szProcName);
}