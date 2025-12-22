#pragma once

#include "Razix/Audio/RZSoundEngine.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/Version/RazixVersion.h"

#include "Razix/Core/RZEngineSettings.h"

#include "Razix/Core/Utils/RZCommandLineParser.h"
#include "Razix/Core/Utils/TRZSingleton.h"

//#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/Renderers/RZWorldRenderer.h"

#include "Razix/Scripting/RZLuaScriptHandler.h"

//! Some style guide rules are waved off for RZEngine class
namespace Razix {

    // TODO: Engine will also manage the Graphics Context and API Renderer initialization ==> Window and app context (in consoles) must be given to the OS abstraction rather than the Application class
    // TODO: use this for stats CPU https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

    /* The Engine class that Starts and Manages all the Engine back end and runtime systems */
    class RAZIX_API RZEngine : public RZSingleton<RZEngine>
    {
        // All internal type definition go here
    public:
        // TODO: Hide this or move to RZEngineSettings.h
        bool isRZApplicationCreated = false;
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
            RZHashMap<RZString, f32> PassTimings; /* Holds references to frame graph pass node Idx and it's CPU execution time */

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
        void Ignite();
        void PostGraphicsIgnite();
        void ShutDown();
        void Run();
        void LoadEngineConfigFile();

        inline bool                           isEngineInTestMode() const { return m_IsEngineInTestMode; }
        inline void                           setEngineInTestMode() { m_IsEngineInTestMode = true; }
        inline RZCommandLineParser&           getCommandLineParser() { return m_CommandLineParser; }
        inline Stats&                         GetStatistics() { return m_Stats; }
        inline void                           ResetStats() { m_Stats.reset(); }
        inline Gfx::RZRendererSettings&       getWorldSettings() { return m_WorldSettings; }
        inline void                           setWorldSettings(const Gfx::RZRendererSettings& settings) { m_WorldSettings = settings; }
        inline const EngineSettings&          getGlobalEngineSettings() { return m_EngineSettings; }
        inline Gfx::RZWorldRenderer&          getWorldRenderer() { return m_WorldRenderer; }
        inline Scripting::RZLuaScriptHandler& getScriptHandler() { return m_LuaScriptHandlerSystem; }
        //inline Gfx::RZShaderLibrary&          getShaderLibrary() { return m_ShaderLibrary; }

    private:
        RZCommandLineParser           m_CommandLineParser;
        Stats                         m_Stats;
        Gfx::RZRendererSettings       m_WorldSettings;
        EngineSettings                m_EngineSettings;
        RZVirtualFileSystem           m_VirtualFileSystem;
        Scripting::RZLuaScriptHandler m_LuaScriptHandlerSystem;
        Gfx::RZWorldRenderer          m_WorldRenderer;
        bool                          m_IsEngineInTestMode = false;
        //Gfx::RZShaderLibrary          m_ShaderLibrary;
    };
}    // namespace Razix
