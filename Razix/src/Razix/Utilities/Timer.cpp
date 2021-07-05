#include "rzxpch.h"
#include "Timer.h"

namespace Razix
{
	Timer::Timer() 
		: m_Start(Now()), m_Frequency()
	{
		m_LastTime = m_Start;
	}

	float Timer::GetTimeMS()
	{
		float time = Duration(m_LastTime, Now(), 1000.0f);
		m_LastTime = Now();
		return time;
	}
}