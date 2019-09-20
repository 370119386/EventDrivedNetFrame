#ifndef __DDNF__PERFORMANCE__H__
#define __DDNF__PERFORMANCE__H__

#include <iostream>
#include <time.h>
#include "DDNFCommon/DDNFPluginModule/Platform.h"

namespace DDNF
{
	class Performance
	{
	private:
		int64_t time = 0;
		int64_t checkTime = 0;

	public:
		Performance()
		{
			time = DDNFGetTimeMS();
		}

		bool CheckTimePoint(const int milliSecond = 1)
		{
			checkTime = DDNFGetTimeMS();
			if (checkTime > milliSecond + time)
			{
				return true;
			}

			return false;
		}

		int64_t TimeScope()
		{
			return checkTime - time;
		}
	};
}
#endif//__DDNF__PERFORMANCE__H__