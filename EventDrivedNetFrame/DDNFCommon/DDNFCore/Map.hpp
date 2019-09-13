#ifndef __DDNF__MAP__H__
#define __DDNF__MAP__H__

#include <map>
#include <list>
#include <string>
#include <iostream>
#include <typeinfo>
namespace DDNF
{
    template <typename T , typename TD>
    class Map
    {
    public:
        typedef std::map<T, TD*> MapOBJECT;

        Map() {};
        virtual ~Map()
        {

        };

        virtual bool AddElement(const T& name, TD* data)
        {
            typename MapOBJECT::iterator itr = mObjectList.find(name);
            if (itr == mObjectList.end())
            {
                mObjectList.insert(typename MapOBJECT::value_type(name, data));
                return true;
            }

            return false;
        }

        virtual TD* RemoveElement(const T& name)
        {
            TD* pData = NULL;
            typename MapOBJECT::iterator itr = mObjectList.find(name);
            if (itr != mObjectList.end())
            {
                pData = itr->second;
                mObjectList.erase(itr);
            }

            return pData;
        }

        virtual TD* GetElement(const T& name)
        {
            typename MapOBJECT::iterator itr = mObjectList.find(name);
            if (itr != mObjectList.end())
            {
                return itr->second;
            }
            else
            {
                return NULL;
            }
        }

        virtual TD* First()
        {
            if (mObjectList.size() <= 0)
            {
                return NULL;
            }

            mObjectCurIter = mObjectList.begin();
            if (mObjectCurIter != mObjectList.end())
            {
                return mObjectCurIter->second;
            }
            else
            {
                return NULL;
            }
        }

        virtual TD* Next()
        {
            if (mObjectCurIter == mObjectList.end())
            {
                return NULL;
            }

            ++mObjectCurIter;
            if (mObjectCurIter != mObjectList.end())
            {
                return mObjectCurIter->second;
            }
            else
            {
                return NULL;
            }
        }

        virtual TD* First(T& name)
        {
            if (mObjectList.size() <= 0)
            {
                return NULL;
            }

            mObjectCurIter = mObjectList.begin();
            if (mObjectCurIter != mObjectList.end())
            {
                name = mObjectCurIter->first;
                return mObjectCurIter->second;
            }
            else
            {
                return NULL;
            }
        }

        virtual TD* Next(T& name)
        {
            if (mObjectCurIter == mObjectList.end())
            {
                return NULL;
            }

            mObjectCurIter++;
            if (mObjectCurIter != mObjectList.end())
            {
                name = mObjectCurIter->first;
                return mObjectCurIter->second;
            }
            else
            {
                return NULL;
            }
        }

        int Count()
        {
            return mObjectList.size();
        }

        bool ClearAll()
        {
            mObjectList.clear();
            return true;
        }
    private:
        MapOBJECT     mObjectList;
        typename MapOBJECT::iterator mObjectCurIter;
    };
}//!DDNF
#endif//__DDNF__MAP__H__
