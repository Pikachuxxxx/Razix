#pragma once

#include "Razix/Audio/RZSoundEngine.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/Version/RazixVersion.h"

#include "Razix/Core/RZEngineSettings.h"

#include "Razix/Utilities/RZCommandLineParser.h"
#include "Razix/Utilities/TRZSingleton.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Renderers/RZWorldRenderer.h"

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
        // TODO: Hide this
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

    private:
        // TODO: Use a template method to get the systems automatically, hence use a system registration design for runtime and static systems with IRZSystem as parent
        // Engine Systems
        RZVirtualFileSystem           m_VirtualFileSystem;      /* The Virtual File Engine System for managing files								*/
        RZSceneManager                m_SceneManagerSystem;     /* Scene Manager Engine System for managing scenes in game world					*/
        Scripting::RZLuaScriptHandler m_LuaScriptHandlerSystem; /* Lua Script Handling Engine System for managing and executing scrip components	*/
        Gfx::RZWorldRenderer          m_WorldRenderer;          /* Razix world renderer that build and renders the frame graph passes in the scene  */
        Gfx::RZShaderLibrary          m_ShaderLibrary;          /* Shader library that pre-loads shaders into memory                                */

    public:
        /* Starts up the Engine and it's sub-systems */
        void Ignite();
        /* Post ignition after important engine systems, usually called after Gfx context and RZApp has been initialized */
        void PostGraphicsIgnite();

        /* Shutdowns the engine and all the resources and systems */
        void ShutDown();

        /* manages the Engine Runtime systems */
        void Run();

        /* Loads and fills the Engine config Settings */
        void LoadEngineConfigFile();

        /* Gets the command line parser */
        RZCommandLineParser getCommandLineParser() { return m_CommandLineParser; }
        /* Get engine installation directory */
        RAZIX_INLINE const std::string& getEngineInstallationDir() { return m_EngineInstallationDir; }
        /* Gets the Statistics of the current engine state */
        RAZIX_INLINE Stats& GetStatistics() { return m_Stats; }
        /* Reset Statistics of the current engine state */
        RAZIX_INLINE void ResetStats() { m_Stats.reset(); }
        /* Gets the World Renderer default global settings */
        RAZIX_INLINE Gfx::RZRendererSettings& getWorldSettings() { return m_WorldSettings; }
        /* Assigns the World Renderer default global settings */
        RAZIX_INLINE void setWorldSettings(const Gfx::RZRendererSettings& settings) { m_WorldSettings = settings; }
        /* Gets the Global Engine settings loaded from the ini file */
        RAZIX_INLINE const EngineSettings& getGlobalEngineSettings() { return m_EngineSettings; }

        // TODO: Use a template method to get the systems automatically, hence use a system registration design for runtime and static systems with IRZSystem as parent
        RAZIX_INLINE Gfx::RZWorldRenderer& getWorldRenderer() { return m_WorldRenderer; }
        RAZIX_INLINE Scripting::RZLuaScriptHandler& getScriptHandler() { return m_LuaScriptHandlerSystem; }
        RAZIX_INLINE Gfx::RZShaderLibrary& getShaderLibrary() { return m_ShaderLibrary; }

    private:
        RZCommandLineParser     m_CommandLineParser;     /* Razix command line args passed to app/engine                   */
        Stats                   m_Stats;                 /* Current frame basic statistics	                               */
        std::string             m_EngineInstallationDir; /* Where was the engine installed                                 */
        Gfx::RZRendererSettings m_WorldSettings;         /* World Renderer Settings                                        */
        EngineSettings          m_EngineSettings;        /* Global Engine wide settings                                    */
    };
}    // namespace Razix