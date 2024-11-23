# Razix Engine Release Notes

for verions 0.4x.yy.Dev

#### Version - 0.43.0 [Development] - 16/09/2023
    - Graphics API uses handles and resource manager/pools
    - improved frame graph; data-driven frame graph wip;
    - gained even more performance
    - fixed PBR lighting model
    - Editor minor improvements

#### Version - 0.42.0 [Development] - 02/07/2023
    - Single command buffer per frame
    - Improved synchronization and command buffer management 
    - Gained performance
    - Minor improvements and fixes
    - AABB boundingboxes can be visualised for all meshes now
    - Disabled imgui; fg resource barriers WIP

#### Version - 0.41.2 [Development] - 04/06/2023    
    - Patch for crash
#### Version - 0.41.1 [Development] - 04/06/2023
    - Patch for crash

#### Version - 0.41.0 [Development] - 04/06/2023
    
    - Added custom mesh and model formats and loaders for engine and RazixAssetPacker project
    - Improved editor controls
    - Improves scene Hierarchy and resolved issues
    - Added FrameGraph Editor project


#### Version - 0.40.0 [Development] - 08/04/2023
![](./Branding/DemoImages/ReleaseNotes/Razix_Engine_PBR_Lighting_Demo_3.png)
    
    - Added Frame Graph inspired from Frostbiet engine
    - added GBuffer, Shadow mapping, PBR + IBL lighting model
    - Imporved editing => Added material editor
    - Single world renderer to build the entire frame graph
    - Redesigned rendering architecture
    - used new vulkan dynamic rendering extension

#### Known Issues
- Distrubution build in not working
- CSM cascade matrices calculation is wrong
