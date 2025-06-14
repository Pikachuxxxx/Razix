#pragma once

#include <string>
#include <unordered_map>

#include "Razix/Core/RZDataTypes.h"

namespace Razix {

    /**
     * Department Usage Examples Table
     * 
     * | Department       | Example Systems/Render Passes                                                           
     * |------------------|-----------------------------------------------------------------------------------------
     * | Core             | Global Initialization, GPU uploads (GlobalFrameData, GPULights...) etc.              
     * | Environment      | GBuffer Passes (Static, Decals, Deformable Surfaces), Water Simulation, Volumetric Fog  
     * | Character        | GBuffer for Animated Characters, Hair Strand Simulation                 
     * | Lighting         | Volumetric Clouds, Shadow Maps, Apply Lighting, Lens Flares, Fog                        
     * | RayTracing       | Ray-Traced Reflections, Ray-Tracing Updates, Blended Ray Tracing                        
     * | VFX              | GPU Particles, Alpha Rendering, Post Alpha Effects, Decals (Spawned)                    
     * | UI               | GUI Rendering, Post Tone Mapping (UI Layer), Debug HUD                                  
     * | Debug            | Debug Render Passes (ImGui, Grid, Debug lines for editor etc.)                           
     * | Physics          | Collision Updates, Rigid Body Simulations, Water Interactions                           
     * | Scripting        | Scripted Effects, Gameplay Scripting etc.                               
     * | Audio            | Spatial Audio Processing                         
     * | Networking       | Client/server Communication, Debug Networking                 
     *
     * Notes:
     * - This table provides examples of how different rendering passes or engine systems are associated with departments.
     * - The department names serve as high-level categories for managing and profiling performance.
     * - Render passes with shared functionality (e.g., water simulation under both Physics and Environment) are categorized based on their primary role.
     * - Some systems, like Debug and Core, span multiple areas but serve specific purposes in performance monitoring or foundational engine behavior.
     */

    /**
     * Different departments that uses the engine, render graph passes are and engine systems are registered to one of the departments
     * Departments are used for debugging/profiling purposes to track memory usage and CPU/GPU timings
     */
    enum class Department
    {
        NONE,    // Unsafe
        Core,
        Environment,
        Character,
        Lighting,
        RayTracing,
        VFX,
        UI,
        Debug,
        Physics,
        Scripting,
        Audio,
        Network,
        COUNT    // Total number of departments
    };

    struct DepartmentInfo
    {
        const char* debugName;    /* Short name for debugging   */
        const char* friendlyName; /* User-friendly name         */
    };

    struct DepartmentHasher
    {
        std::size_t operator()(Department dept) const noexcept
        {
            return static_cast<std::size_t>(dept);
        }
    };

    const static std::unordered_map<Department, DepartmentInfo, DepartmentHasher> s_DepartmentInfo = {
        {Department::NONE, {"None", "Unowned by anyone"}},
        {Department::Core, {"Core", "Core Systems (Engine/Rendering related)"}},
        {Department::Environment, {"Environment", "Environment"}},
        {Department::Character, {"Character", "Character Systems"}},
        {Department::Lighting, {"Lighting", "Lighting"}},
        {Department::RayTracing, {"RayTracing", "Ray Tracing"}},
        {Department::VFX, {"VFX", "Visual Effects"}},
        {Department::UI, {"UI", "UI for Game and Debug"}},
        {Department::Physics, {"Physics", "Physics simulation"}},
        {Department::Scripting, {"Scripting", "Visual and Lua gameplay scripting"}},
        {Department::Audio, {"Audio", "Game audio"}},
        {Department::Network, {"Network", "Networking related"}},
        {Department::Debug, {"Debug", "Anything related to debugging"}},
    };

    const static std::unordered_map<std::string, Department> s_StringToDepartment = {
        {"NONE", Department::NONE},
        {"Core", Department::Core},
        {"Environment", Department::Environment},
        {"Character", Department::Character},
        {"Lighting", Department::Lighting},
        {"RayTracing", Department::RayTracing},
        {"VFX", Department::VFX},
        {"UI", Department::UI},
        {"Physics", Department::Physics},
        {"Scripting", Department::Scripting},
        {"Audio", Department::Audio},
        {"Network", Department::Network},
        {"Debug", Department::Debug},
    };

    static const u8 s_TotalDepartments = (u8) Department::COUNT;

}    // namespace Razix