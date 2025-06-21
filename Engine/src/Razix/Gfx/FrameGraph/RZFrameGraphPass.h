#pragma once

namespace Razix {
    struct SceneDrawParams;
}
namespace Razix {

    enum class SceneDrawGeometryMode;

    namespace Gfx {
        struct RenderingInfo;
        enum class Resolution : u32;
    }    // namespace Gfx
}    // namespace Razix

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license. (Data-driven + customization)
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/
namespace Razix {
    namespace Gfx {

        // Forward Decelerations
        class RZPassResourceDirectory;    // List of all Resources in the current pass node
        class RZPassNode;

        /**
          * This is type erasure all over again, the PassNode needs to store a lambda function to execute and some data to pass to the lambda
          * now unlike resource entry, we don't need to know anything about the function or data that need to be passed it can be done discretely
          * only while creating the function using FG we need to see if it's valid signature apart from that PassNode needs no other info,
          * Now we don't have to call any specific method on these erased types so we don't need a complex manager class like ResourceEntry
          * just simple () will do just fine on all pairs of Data Func pairs
          * 
          * So we create a pass class to store Data and a Func and since we call a common function operator() on it
          * we can store a interface (aka Concept) class in the PassNode that will encapsulate this Func and Data 
          * 
          * Dawid Kurek names it concept as it's some form of Type Erasure but for me I like to see it as a simple interface
          */

        /**
          * Frame Graph pass is nothing but a Lambda function and we use a typeless way to call it for every pass node
          */
        struct IRZFrameGraphPass
        {
            IRZFrameGraphPass() {}
            RAZIX_VIRTUAL_DESCTURCTOR(IRZFrameGraphPass)

            RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(IRZFrameGraphPass)

            virtual void operator()(RZPassNode& node, RZPassResourceDirectory& resources)  = 0;
            virtual void resize(RZPassResourceDirectory& resources, u32 width, u32 height) = 0;
        };

        template<typename Data, typename ExecuteFunc, typename ResizeFunc>    
        struct RZFrameGraphCodePass final : IRZFrameGraphPass
        {
            explicit RZFrameGraphCodePass(ExecuteFunc&& exec, ResizeFunc&& resize)
                : execFunction{std::forward<ExecuteFunc>(exec)}, resizeFunction{std::forward<ResizeFunc>(resize)} {}

            RAZIX_VIRTUAL_DESCTURCTOR(RZFrameGraphCodePass)

            void operator()(RZPassNode& node, RZPassResourceDirectory& resources) override
            {
                // Note: Node isn't is used here it's for the RZFrameGraphDataPass
                execFunction(data, resources);
            }

            void resize(RZPassResourceDirectory& resources, u32 width, u32 height) override
            {
                resizeFunction(resources, width, height);
            }

            ExecuteFunc execFunction;
            ResizeFunc  resizeFunction;
            Data        data{};
        };

        struct FrameGraphDataPassDesc
        {
            Gfx::RZShaderHandle          shader;
            Gfx::RZPipelineHandle        pipeline;
            Razix::SceneDrawGeometryMode geometryMode;
            Resolution                   resolution;
            float2                       extent;
            u32                          layers;
        };

        struct RZFrameGraphDataPass final : IRZFrameGraphPass
        {
            RZFrameGraphDataPass(FrameGraphDataPassDesc desc);

            RAZIX_VIRTUAL_DESCTURCTOR(RZFrameGraphDataPass)

            FrameGraphDataPassDesc m_Desc;

            void operator()(RZPassNode& node, RZPassResourceDirectory& resources) override;
            void resize(RZPassResourceDirectory& resources, u32 width, u32 height) override;
        };
    }    // namespace Gfx
}    // namespace Razix
