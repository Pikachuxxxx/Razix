#pragma once

#include "Razix/Core/Core.h"

namespace Razix
{
	namespace Utilities
	{
	public:
		/// <summary>
		/// A Singleton type class template
		/// We return a static const reference instead of a lazy heap creation
		/// Note: [Design Suggested in Game Engine Architecture by Jason Gregory at p.199-201]
		/// </summary>
		template <class T>
		class TSingleton
		{
			static T& Get()
			{
				UNIMPLEMENTED
			}

			static void Release()
			{
				UNIMPLEMENTED
			}
		};
	protected:
		TSingleton() {}
		~TSingleton() {}

		NONCOPYABLE
	}
}