#include "rzxpch.h"
#include "RZTimer.h"

namespace Razix
{
	RZTimer::RZTimer() 
		: m_Start(Now()), m_Frequency()
	{
		m_LastTime = m_Start;
	}

	float RZTimer::GetTimeMS()
	{
		float time = Duration(m_LastTime, Now(), 1000.0f);
		m_LastTime = Now();
		return time;
	}
}