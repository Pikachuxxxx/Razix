#pragma once

#include "Razix/Core/RazixVersion.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Utilities/RZCommandLineParser.h"
#include "Razix/Utilities/TRZSingleton.h"

#include "Razix/Scene/RZSceneManager.h"

#include "Razix/Scripting/LuaScriptHandler.h"

//! The style guide rules are waved off for RZEngine class
namespace Razix

	// TODO: Engine will also manage the Grpahics Context and API Renderer initialization ==> Window and app context (in consoles) must be given to the OS abstraction rather than the Application class
{
	/* The Engine class that Starts and Manages all the Engine back end and runtime systems */
	class RAZIX_API RZEngine : public RZSingleton<RZEngine>
	{
		// All internal type definition go here
	public:
		/* Statistic about the current frame */
		struct Stats
		{
			uint32_t				UpdatesPerSecond = 0;
			uint32_t				FramesPerSecond = 0;
			float					FrameTime = 0;
            uint32_t				NumDrawCalls = 0;
		};

    public:
		RZCommandLineParser			commandLineParser;			/* Command line parser for that helps in setting Engine and Application options */

	private:
		// Engine Systems
		RZVirtualFileSystem			m_VirtualFileSystem;		/* The Virtual File Engine System for managing files								*/
		RZSceneManager				m_SceneManagerSystem;		/* Scene Manager Engine System for managing scenes in game world					*/
		Scripting::LuaScriptHandler m_LuaScriptHandlerSystem;	/* Lua Script Handling Engine System for managing and executing scrip components	*/

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

		// TODO: Use a template method to get the systems automatically, hence use a system registration design with IRZSystem as parent
		RZSceneManager& getSceneManager() { return m_SceneManagerSystem; }

	private:
		Stats m_Stats;										/* Current frame basic statistics									*/
		float m_MaxFramesPerSecond = 1000.0f / 60.0f;		/* Maximum frames per second that will be rendered by the Engine	*/
	};
}