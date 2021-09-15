#pragma once

#include "Razix/Core/Core.h"

namespace Razix
{
	/// <summary>
	/// A Singleton type class template
	/// We return a static const reference instead of a lazy heap creation
	/// Note: [Design Suggested in Game Engine Architecture by Jason Gregory at p.199-201]
	/// </summary>
	template <class T>
	class TSingleton
	{
	public:
		/// <summary>
		/// Gets the reference to the Singleton Object
		/// </summary>
		static T& Get()
		{
			static T instance;
			return instance;
		}
	protected:
		TSingleton() {}
		~TSingleton() {}
	private:
		///	Make the singleton non-copyable
		NONCOPYABLE(TSingleton);
	};

	// TODO: Test with DCLP before using
	// TODO: Do not use this yet!
	//template <class T>
	//class ThreadSafeSingleton
	//{
	//public:
	//	static T& Get()
	//	{
	//		std::lock_guard<std::mutex> lock(m_mConstructed); //Lock is required here though, to prevent multiple threads initialising multiple instances of the class when it turns out it has not been initialised yet
	//		static TSingleton instance;
	//		return instance;
	//	}
	//
	//protected:
	//	//Only allow the class to be created and destroyed by itself
	//	ThreadSafeSingleton() { }
	//	~ThreadSafeSingleton() { }
	//
	//	static std::mutex m_mConstructed;
	//
	//private:
	//	NONCOPYABLE(ThreadSafeSingleton);
	//};
}