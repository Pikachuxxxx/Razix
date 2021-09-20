<br>
<img width=200" align="left" src="https://github.com/Pikachuxxxx/Razix/blob/master/Branding/RazixLogo.png"> <h1> Razix Engine </h1>
Razix is a High Performance Research Engine for production pipeline with emphasis on experimenting with different rendering techniques. Razix supports Windows, Mac, Linux, PSVita, PS3 and PS4 systems.

 ---

# About
Cross-Platform 2D and 3D engine with multi render API support (OpenGL, Vulkan DirectX 11, GXM, GCM, GNM and GNMX). Supports a wide range of Renders with extreme emphasis on scene optimization and implementing state-of-the art rendering techniques. The engine architecture supports a very educational and optimized design.
                                                                                                          
View the [Trello Board](https://trello.com/b/yvWKH1Xr/razix-engine) and the [Architecture notes](https://drive.google.com/file/d/1y5ZFf-h02z3cx6WmUzR8giKScvORzmwx/view?usp=sharing)

# Features
- Support for Windows, Linux, macOS, PSVita, PS3 and PS4 systems.
- Support for OpenGL, Vulkan DirectX 11, GXM, GCM, GNM and GNMX.
- 3D audio using OpenAL and Scream and Sulpha.
- Rendering 3D models with deferred PBR shading.
- Editor GUI using ImGui.
- Multi Physics engine support.
- 3D physics using PhysX, Bullet and Havok.
- 2D physic using Box2D.
- Basic lua scripting support for entities.
- Extremely detailed profiling using Tracy, RenderDoc and Razor integrated deep into the engine systems.
- Custom Animation and state machine engine and supports Havok Animation system
- Supports GLSL, HLSL and PSSL shading languages to create custom materials
- Supports Hull, Domain, Geometry, Compute shaders for all Platforms
- Asset streaming pipeline and custom asset format

# To be Updated...

<details>
  <summary>Click to view changelog </summary>

# Razix Engine ChangeLog

## Version - 0.14.0 [Development] - 02/07/2021
    - Added Engine Singleton
    - Added Engine Ignition with proper logging
    - Added VFS to Engine Ignition
    - Changed Entrypoint and VFS initiation

## Version - 0.13.0 [Development] - 27/06/2021
    - Added OS independent FileSystem (Windows only implementation for now)
    - Added Virtual File system
    - Added Window Icon

## Version - 0.12.0 [Development]
    - Refactored Premake build system to be more robust
    - Added Window and Input Abstraction for Windows(GLFW only)
    - Added OpenGL Context abstraction for graphics context

## Version - 0.11.0 [Development]
    - Added basic layer system and ImGui
    - Added OS abstraction
    - Added Event system

## Version - 0.10.0 [Development]
    - Added Application and Entry point
    - Added Logging system using spdlog
    - Added dependencies (glad, glfw, imgui, spdlog, premake)
    - Added premake build system

</details>
