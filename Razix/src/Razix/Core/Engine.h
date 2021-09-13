#pragma once

#include "Razix/Core/RazixVersion.h"
#include "Razix/Utilities/CommandLineParser.h"
#include "Razix/Utilities/TSingleton.h"

namespace Razix
{
	/// <summary>
	/// The Engine class that Starts and Manages all the Engine back-end and runtime systems
	/// </summary>
	class RAZIX_API Engine : public TSingleton<Engine>
	{
		// All internal type definition go here
	public:
		/// <summary>
		/// Statistic about the current frame
		/// </summary>
		struct Stats
		{
			uint32_t UpdatesPerSecond = 0;
			uint32_t FramesPerSecond = 0;
			uint32_t NumRenderedObjects = 0;
			uint32_t NumDrawCalls = 0;
			float FrameTime = 0;
			float TotalGPUMemory = 0;
			float UsedGPUMemory = 0;
			float UsedRam = 0;
		};

		CommandLineParser commandLineParser;

	public:
		Engine() {}
		~Engine() {}

		/// <summary>
		/// Starts up the Engine and it's sub-systems
		/// </summary>
		void Ignite();

		/// <summary>
		/// Shutdowns the engine and all the resources and systems
		/// </summary>
		void ShutDown();

		/// <summary>
		/// manages the Engine Runtime systems
		/// </summary>
		void Run();

		/// <summary>
		/// Gets the Statistics of the current engine state
		/// </summary>
		Stats& GetStatistics() { return m_Stats; }

		/// <summary>
		/// Resets the stats to the default value
		/// </summary>
		void ResetStats()
		{
			m_Stats.NumRenderedObjects = 0;
			m_Stats.FrameTime = 0.0f;
			m_Stats.UsedGPUMemory = 0.0f;
			m_Stats.UsedRam = 0.0f;
			m_Stats.NumDrawCalls = 0;
			m_Stats.TotalGPUMemory = 0.0f;
		}

		/// <summary>
		/// Gets the maximum number of frames that can be rendered
		/// </summary>
		const float& GetTargetFrameRate() const { return m_MaxFramesPerSecond; }

		/// <summary>
		/// Sets the maximum number of frames per second
		/// </summary>
		/// <param name="targetFPS"> The targeted FPS for the engine </param>
		void SetTargetFrameRate(const float& targetFPS) { m_MaxFramesPerSecond = targetFPS; }
	private:
		/// Current frame basic statistics
		Stats m_Stats;
		/// Maximum frames per second that will be rendered by the Engine
		float m_MaxFramesPerSecond = 1000.0f / 60.0f;
	};
}