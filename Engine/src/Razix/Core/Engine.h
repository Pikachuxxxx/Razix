#pragma once

#include "Razix/Core/RazixVersion.h"
#include "Razix/Utilities/CommandLineParser.h"
#include "Razix/Utilities/TRazixSingleton.h"

//! The style guide rules are waved off for RZEngine class
namespace Razix
{
	/* The Engine class that Starts and Manages all the Engine backend and runtime systems */
	class RAZIX_API RZEngine : public RZSingleton<RZEngine>
	{
		// All internal type definition go here
	public:
		/* Statistic about the current frame */
		struct Stats
		{
			uint32_t UpdatesPerSecond = 0;
			uint32_t FramesPerSecond = 0;
			float FrameTime = 0;
		};

    public:
		CommandLineParser commandLineParser;

	public:
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
			//m_Stats.NumRenderedObjects = 0;
			m_Stats.FrameTime = 0.0f;
			//m_Stats.UsedGPUMemory = 0.0f;
			//m_Stats.UsedRam = 0.0f;
			//m_Stats.NumDrawCalls = 0;
			//m_Stats.TotalGPUMemory = 0.0f;
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