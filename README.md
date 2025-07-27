<br>
<img width=200" align="left" src="https://github.com/Pikachuxxxx/Razix/blob/master/Branding/RazixLogo.png"> <h1> Razix Engine </h1>

[![CI Build](https://github.com/Pikachuxxxx/Razix/actions/workflows/ci.yml/badge.svg)](https://github.com/Pikachuxxxx/Razix/actions/workflows/ci.yml)
[![Release Build](https://github.com/Pikachuxxxx/Razix/actions/workflows/release.yml/badge.svg)](https://github.com/Pikachuxxxx/Razix/actions/workflows/release.yml)
[![Scheduled Build](https://github.com/Pikachuxxxx/Razix/actions/workflows/schedule.yml/badge.svg)](https://github.com/Pikachuxxxx/Razix/actions/workflows/schedule.yml)

Razix is a High Performance Engine for experimenting with different rendering techniques. Razix supports Windows, Mac, Linux. 

THIS ENGINE IS BEING MADE FOR A GAME< not a generic engine, I'll have the GDD/Story etc. soon here!


# About
Razix is a cross-Platform 3D engine with multi render API support (Vulkan, DirectX 12). It's being built on a new design principle of **"Everything is as asset"** to replace traditonal 
gameobject and ECS systems and primarily focusing on making a game one day soon. I have a game idea that I'll be showcasing using the engine in 2025 hopefully, 
eventually I hope razix will be mature enough one day. It's also my portfolio + practice sandbox for making a trippy game.

View the [Trello Board](https://trello.com/b/yvWKH1Xr/razix-engine) and the [Architecture notes on Draw.io](https://drive.google.com/file/d/1y5ZFf-h02z3cx6WmUzR8giKScvORzmwx/view?usp=sharing)

# Features
- Built using C++17. (Engine will be reverted back to C++14 once entt/sol have been removed and only Jolt will be compiled in C++17 mode)
	- C++14 over C++11 because `constexpr` and `sizeof` and improved `lamdbas` are needed for framegraph.
- Support for Windows, Linux, macOS.
- Support for Vulkan & DirectX 12.
- **Data Driven** Framegraph based on EA's frostbite engine (based on [skaarj
1989's Framegraph implementation](https://github.com/skaarj1989/FrameGraph))  
    - **Uses Custom [Node Graph Editor](https://github.com/Pikachuxxxx/QtNodeGraph)**

| Node based <br /> Frame Graph Editor  | <img src="./Branding/DemoImages/ReleaseNotes/Frame_Graph_Editor_design_demo_5.png" width="400"> |  
| ------------- | ------------- |

| FrameGraph lifetimes  | <img src="./Branding/DemoImages/ReleaseNotes/FGLifetimesTool.png" width="400"> |  
| ------------- | ------------- |

- 3D audio using OpenAL
- Complete Deferred pipeline + Bindless resources + nanite-like renderers(Future support for customizable render graph editor for complete control).
- Runtime tools using ImGui + Blender Support as level editor
- Multi Physics engine support.
- 3D physics using Jolt.
- Basic lua scripting support
- Extremely detailed profiling using Tracy and RenderDoc integrated
- Supports HLSL shading language to create custom materials + Custom Razix Shaders files for easy compilation
- Resource Pool base management and custom asset format
- Data drive pipeline architecture - FrameGraph uses JSON files
- Custom [Razix STL](https://github.com/Pikachuxxxx/RZSTL) and [Razix Memory](https://github.com/Pikachuxxxx/RazixMemory)
- **WorldRenderer** Passes
	- Lighting and shadows 
		- Directional lights with CSM (Cascaded Shadow Mapping)
		- Point lights
		- Spot lights
	- Deferred PBR 
	- IBL
	- Skybox (HDR & Procedural)
	- SSAO
	- Debug Drawing
	- ImGUi
	- Tonemapping
	- FXAA
	- TAA (WIP)

**Experimental features:**
  - Machine Learning sandbox (custom NNs class + Transformers WIP) CPU only for now, later might be ported over the engine RHI layer
  - Department based memory/frame time budget profiling (inspiration from SMS GDC talk and Insomniac engine)
  - I have a secret RazixGPT runst LLM might use it for game NPCs one day.

DirectX11, OpenGL has been deprecated. Only high-level APIs and RTX level GPUs will be supported. PSVita(GXM) and PS3(GCM) will be supported sometime far in future.

# RoadMap

## Building

### Pre-Requisites
Assumes you have VULKAN_SDK installed and the env variable is configured properly

1. Clone the Repository
```
git clone https://github.com/Pikachuxxxx/Razix.git
cd Razix
```
2. Generate Premake Project Files
Windows (PowerShell)

### From the repo root:
Windows
```
cd Scripts
.\u005cGeneratePremakeProjects.bat
```
Linux/macOS (Bash)
```
# From the repo root:
chmod +x Scripts/generate_projects.sh
./Scripts/generate_projects.sh
```
Razix uses Premake5 for project generation. Scripts are provided for different platforms.
No need to clone submodules.


### Build the Engine

Windows (Visual Studio)
Open build/Razix.sln in Visual Studio 2022 or newer
Select a configuration: Debug, Release, or Dist
Press Ctrl+Shift+B or select Build > Build Solution

either run the game or run some tests

## V 1.0.0 - RC
![](./Docs/Architecture/RazixEngine-RoadMap-V1.0.0RC.png)

# Contributing
More information will be update on creating PRs and other ways to contribute to Razix Engine.
### Style Guide
Check the [style guide](./Docs/RazixStyleGuide.md) for Razix Style enforcements

# Screenshots/Demos
<img src="./Branding/DemoImages/ReleaseNotes/Razix_Engine_V.0.49.0.png">

# Architecture

#### :warning: Still a work in progress
![](./Docs/Architecture/RazixEngine-Architecture-V.1.0.0.RC.png)

**ex. Lua Scriping API cheatsheet** ![Lua Scriping API cheatsheet](./Docs/RazixLuaScriptingAPICheatSheet.png)

**Check the Docs folder or [Architecture notes on Draw.io](https://drive.google.com/file/d/1y5ZFf-h02z3cx6WmUzR8giKScvORzmwx/view?usp=sharing) for a detailed architecture description**

| Core Systems | Frame Graph |
| ----------- | ---------- |
| <img src="./Docs/Architecture/RazixEngine-CoreSystems.png" width="300">  |  <img src="./Docs/Architecture/Razix Engine-FrameGraph.png" width="300"> |

# Changelog and Release Notes

[Click to view changelog](./Docs/CHANGELOG.md)

[Click to view ReleaseNotes](./Docs/ReleaseNotes.md)

# License
Razix Engine is distributed under Apache 2.0 license. [LICENSE](https://github.com/Pikachuxxxx/Razix/blob/master/LICENSE)
