#pragma once

#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/RazixVersion.h"

#include "Razix/Utilities/RZCommandLineParser.h"
#include "Razix/Utilities/TRZSingleton.h"

#include "Razix/Scene/RZSceneManager.h"

#include "Razix/Graphics/Renderers/RZRenderStack.h"
#include "Razix/Scripting/RZLuaScriptHandler.h"

//! Some style guide rules are waved off for RZEngine class
namespace Razix {

    // TODO: Engine will also manage the Grpahics Context and API Renderer initialization ==> Window and app context (in consoles) must be given to the OS abstraction rather than the Application class
    // TODO: use this for stats CPU https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

    /* The Engine class that Starts and Manages all the Engine back end and runtime systems */
    class RAZIX_API RZEngine : public RZSingleton<RZEngine>
    {
        // All internal type definition go here
    public:
        /* Statistic about the current frame */
        struct Stats
        {
            float    DeltaTime             = 0;    //[x]
            uint32_t UpdatesPerSecond      = 0;    //[x]
            uint32_t FramesPerSecond       = 0;    //[x]
            uint32_t NumDrawCalls          = 0;    //[x]
            uint32_t Draws                 = 0;    //[x]
            uint32_t IndexedDraws          = 0;    //[x]
            uint32_t GPUMemoryUsed         = 0;    //[ ] // Needs VMA kind of allocator
            uint32_t TotalGPUMemory        = 0;    //[ ] // Needs VMA kind of allocator
            uint32_t UsedRAM               = 0;    //[ ] // Needs platform specific implementation
            uint32_t MeshesRendered        = 0;    //[x]
            uint32_t TexturesInMemory      = 0;
            uint32_t DescriptorSetCapacity = 0;    //[ ] // Add this after the debug font renderer is done

            void reset()
            {
                DeltaTime        = 0;
                NumDrawCalls     = 0;
                Draws            = 0;
                IndexedDraws     = 0;
                GPUMemoryUsed    = 0;
                UsedRAM          = 0;
            }
        };

    public:
        RZCommandLineParser commandLineParser; /* Command line parser for that helps in setting Engine and Application options */

    private:
        // Engine Systems
        RZVirtualFileSystem           m_VirtualFileSystem;      /* The Virtual File Engine System for managing files								*/
        RZSceneManager                m_SceneManagerSystem;     /* Scene Manager Engine System for managing scenes in game world					*/
        Scripting::RZLuaScriptHandler m_LuaScriptHandlerSystem; /* Lua Script Handling Engine System for managing and executing scrip components	*/
        Graphics::RZRenderStack       m_RenderStack;

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
            m_Stats.reset();
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
        RZSceneManager&                getSceneManager() { return m_SceneManagerSystem; }
        Scripting::RZLuaScriptHandler& getScriptHandler() { return m_LuaScriptHandlerSystem; }
        Graphics::RZRenderStack&       getRenderStack() { return m_RenderStack; }

    private:
        Stats m_Stats;                                /* Current frame basic statistics	                                */
        float m_MaxFramesPerSecond = 1000.0f / 60.0f; /* Maximum frames per second that will be rendered by the Engine	*/
    };
}    // namespace Razix