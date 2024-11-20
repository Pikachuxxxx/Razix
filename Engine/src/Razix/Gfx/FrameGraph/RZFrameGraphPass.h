#pragma once

#include "Razix/Core/Memory/RZMemoryBudgets.h"
#include "Razix/Core/RZDepartments.h"

#include "Razix/Gfx/RHI/API/RZAPIHandles.h"

namespace Razix {
    struct SceneDrawParams;
}
namespace Razix {

    enum class SceneDrawGeometryMode;

    namespace Gfx {
        struct RenderingInfo;
        enum class Resolution : u32;
    }    // namespace Graphics
}    // namespace Razix

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license. (Data-driven + customization)
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/
namespace Razix {
    namespace Gfx {
        namespace FrameGraph {

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

                virtual void operator()(RZPassNode &node, RZPassResourceDirectory &resources)  = 0;
                virtual void resize(RZPassResourceDirectory &resources, u32 width, u32 height) = 0;

            protected:
                Department         m_Department; /* The department this pass belongs to */
                Memory::BudgetInfo m_PassBudget; /* Pass current budget tracking */
            };

            /* Encapsulation of the pass lambda and its data, the best way to store lambdas as members is using templates */
            template<typename Data, typename ExecuteFunc, typename ResizeFunc>    // done in FG so redundant here, typename = std::enable_if_t<is_valid_pass_exec_function<ExecuteFunc>()>> // Checks for the signature of the exec function
            struct RZFrameGraphCodePass final : IRZFrameGraphPass
            {
                explicit RZFrameGraphCodePass(ExecuteFunc &&exec, ResizeFunc &&resize)
                    : execFunction{std::forward<ExecuteFunc>(exec)}, resizeFunction{std::forward<ResizeFunc>(resize)} {}

                void operator()(RZPassNode &node, RZPassResourceDirectory &resources) override
                {
                    // Note: Node isn't is used here it's for the RZFrameGraphDataPass
                    execFunction(data, resources);
                }

                void resize(RZPassResourceDirectory &resources, u32 width, u32 height) override
                {
                    resizeFunction(resources, width, height);
                }

                ExecuteFunc execFunction;   /* Pass Execution function                                  */
                ResizeFunc  resizeFunction; /* Pass Resize function                                     */
                Data        data{};         /* Pass data that contains the list of FrameGraphResources  */
            };

            struct FrameGraphDataPassDesc
            {
                Gfx::RZShaderHandle     shader;
                Gfx::RZPipelineHandle   pipeline;
                Razix::SceneDrawGeometryMode geometryMode;
                Resolution                   resolution;
                bool                         enableResize;
                glm::vec2                    extent;
                u32                          layers;
            };

            struct RZFrameGraphDataPass final : IRZFrameGraphPass
            {
                RZFrameGraphDataPass(RZShaderHandle shader, RZPipelineHandle pipeline, Razix::SceneDrawGeometryMode geometryMode, Resolution res, bool resize, glm::vec2 extents, u32 layers);

                Gfx::RZShaderHandle     m_shader;
                Gfx::RZPipelineHandle   m_pipeline;
                Razix::SceneDrawGeometryMode m_geometryMode;
                Resolution                   m_resolution;
                bool                         m_enableResize;
                glm::vec2                    m_extent;
                u32                          m_layers;

                void operator()(RZPassNode &node, RZPassResourceDirectory &resources) override;
                void resize(RZPassResourceDirectory &resources, u32 width, u32 height) override;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix