#pragma once

#include "Razix/Core/Core.h"

#include <stdint.h>
#include <atomic>

namespace Razix
{
	/// <summary>
	/// Counts the reference of the Object as it's ownership is being handed over
	/// </summary>
	struct RAZIX_API ReferenceCounter
	{
		/// Global object reference count
		std::atomic<int> count = 0;
	public:
		/// <summary>
		/// Increases the reference count and returns whether or not the object is empty referenced
		/// </summary>
		inline bool Ref() { count++; return count != 0; }

		/// <summary>
		/// Increases the reference count and returns the value
		/// </summary>
		inline int RefVal() { count++; return count; }

		/// <summary>
		/// Dereferences the object dependency
		/// </summary>
		inline int Unref() { --count; bool deleted = count == 0; return deleted; }

		/// <summary>
		/// Gets the total reference count of the object
		/// </summary>
		inline int GetRefCount() const { return count; }

		/// <summary>
		/// Initializes the initial reference count
		/// </summary>
		inline void InitRefCount(int p_value = 1) { count = p_value; }
	};
}