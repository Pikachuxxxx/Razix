#pragma once

#include "Razix/Audio/RZSoundEngine.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/Version/RazixVersion.h"

#include "Razix/Utilities/RZCommandLineParser.h"
#include "Razix/Utilities/TRZSingleton.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Renderers/RZWorldRenderer.h"

#include "Razix/Scene/RZScene.h"
#include "Razix/Scene/RZSceneManager.h"

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
        bool isRZApplicationCreated = false;
        /* Statistic about the current frame */
        struct Stats
        {
            f32 DeltaTime        = 0;    //[x]
            u32 UpdatesPerSecond = 0;    //[x]
            u32 FramesPerSecond  = 0;    //[x]
            // API calls
            u32 NumDrawCalls      = 0;    //[x]
            u32 Draws             = 0;    //[x]
            u32 IndexedDraws      = 0;    //[x]
            u32 ComputeDispatches = 0;    //[x]
            // Memory stats
            f32 GPUMemoryUsed  = 0;    // in Gib [ ] // Needs VMA kind of allocator
            f32 TotalGPUMemory = 0;    // in Gib [x]
            f32 UsedRAM        = 0;    // in Gib [ ] // Needs platform specific implementation
            // Resource stats
            u32 MeshesRendered        = 0;    //[x]
            u32 VerticesCount         = 0;
            u32 TexturesInMemory      = 0;
            u32 DescriptorSetCapacity = 0;    //[ ] // Add this after the debug font renderer is done
            // Pass timings
            std::unordered_map<std::string, f32> PassTimings; /* Holds references to frame graph pass node Idx and it's CPU execution time */

            void reset()
            {
                DeltaTime         = 0.0f;
                NumDrawCalls      = 0;
                Draws             = 0;
                IndexedDraws      = 0;
                ComputeDispatches = 0;
                GPUMemoryUsed     = 0;
                UsedRAM           = 0;
                PassTimings.clear();
            }
        };

    public:
        RZCommandLineParser commandLineParser; /* Command line parser for that helps in setting Engine and Application options */

    private:
        // Engine Systems
        RZVirtualFileSystem           m_VirtualFileSystem;      /* The Virtual File Engine System for managing files								*/
        RZSceneManager                m_SceneManagerSystem;     /* Scene Manager Engine System for managing scenes in game world					*/
        Scripting::RZLuaScriptHandler m_LuaScriptHandlerSystem; /* Lua Script Handling Engine System for managing and executing scrip components	*/
        Graphics::RZWorldRenderer     m_WorldRenderer;          /* Razix world renderer that build and renders the frame graph passes in the scene  */
        Graphics::RZShaderLibrary     m_ShaderLibrary;          /* Shader library that pre-loads shaders into memory                                */

    public:
        /* Starts up the Engine and it's sub-systems */
        void Ignite();
        /* Post ignition after important engine systems */
        void PostGraphicsIgnite();

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

        void LoadEngineConfigFile();

        /* Get engine installation directory */
        inline const std::string& getEngineInstallationDir() { return m_EngineInstallationDir; }

        /// <summary>
        /// Gets the maximum number of frames that can be rendered
        /// </summary>
        const f32& getTargetFrameRate() const { return m_MaxFramesPerSecond; }

        /// <summary>
        /// Sets the maximum number of frames per second
        /// </summary>
        /// <param name="targetFPS"> The targeted FPS for the engine </param>
        void setTargetFrameRate(const f32& targetFPS) { m_MaxFramesPerSecond = targetFPS; }

        // TODO: Use a template method to get the systems automatically, hence use a system registration design for runtime and static systems with IRZSystem as parent
        RAZIX_INLINE Graphics::RZWorldRenderer& getWorldRenderer() { return m_WorldRenderer; }
        RAZIX_INLINE Graphics::RZRendererSettings& getWorldSettings() { return m_WorldSettings; }
        RAZIX_INLINE void                          setWorldSettings(const Graphics::RZRendererSettings& settings) { m_WorldSettings = settings; }
        RAZIX_INLINE Scripting::RZLuaScriptHandler& getScriptHandler() { return m_LuaScriptHandlerSystem; }
        RAZIX_INLINE Graphics::RZShaderLibrary& getShaderLibrary() { return m_ShaderLibrary; }

    private:
        Stats                        m_Stats;                                /* Current frame basic statistics	                                */
        f32                          m_MaxFramesPerSecond = 1000.0f / 60.0f; /* Maximum frames per second that will be rendered by the Engine	*/
        std::string                  m_EngineInstallationDir;                /* Where was the engine installed                                  */
        Graphics::RZRendererSettings m_WorldSettings;                        /* World Renderer Settings                                         */
    };
}    // namespace Razix