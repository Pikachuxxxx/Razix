#pragma once

#include "Razix/Core/RazixVersion.h"
#include "Razix/Utilities/RZCommandLineParser.h"
#include "Razix/Utilities/TRZSingleton.h"

//! The style guide rules are waved off for RZEngine class
namespace Razix
{
	/* The Engine class that Starts and Manages all the Engine back end and runtime systems */
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
            uint32_t NumDrawCalls = 0;
		};

    public:
		RZCommandLineParser commandLineParser;	/* Command line parser for that helps in setting Engine and Application options */

	public:
		/* Starts up the Engine and it's sub-systems */
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
			m_Stats.FrameTime = 0.0f;
			m_Stats.NumDrawCalls = 0;
		}

		/// <summary>
		/// Gets the maximum number of frames that can be rendered
		/// </summary>
		const float& getTargetFrameRate() const { return m_MaxFramesPerSecond; }

		/// <summary>
		/// Sets the maximum number of frames per second
		/// </summary>
		/// <param name="targetFPS"> The targeted FPS for the engine </param>
		void setTargetFrameRate(const float& targetFPS) { m_MaxFramesPerSecond = targetFPS; }

	private:
		Stats m_Stats;										/* Current frame basic statistics */
		float m_MaxFramesPerSecond = 1000.0f / 60.0f;		/* Maximum frames per second that will be rendered by the Engine */
	};
}