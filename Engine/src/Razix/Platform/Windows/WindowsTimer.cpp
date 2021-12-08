#include "rzxpch.h"
#include "Razix/Utilities/RZTimer.h"

namespace Razix
{
	static double GetQueryPerformancePeriod()
	{
		LARGE_INTEGER result;
		QueryPerformanceFrequency(&result);
		return 1.0 / static_cast<double>(result.QuadPart);
	}
	const double freq = GetQueryPerformancePeriod();

	TimeStamp RZTimer::Now()
	{
		TimeStamp temp;
		QueryPerformanceCounter(&temp);
		return temp;
	}

	double RZTimer::Duration(TimeStamp start, TimeStamp end, double timeResolution)
	{
		return (end.QuadPart - start.QuadPart) * timeResolution * freq;
	}

	float RZTimer::Duration(TimeStamp start, TimeStamp end, float timeResolution)
	{
		return static_cast<float>((end.QuadPart - start.QuadPart) * timeResolution * freq);
	}
}