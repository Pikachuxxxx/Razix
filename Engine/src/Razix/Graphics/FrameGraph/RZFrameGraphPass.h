#pragma once

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/
namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            // Forward Decelerations
            class RZFrameGraphPassResources;    // List of all Frame Graph Resources

            struct RZFrameGraphPassConcept
            {
                RZFrameGraphPassConcept()          = default;
                virtual ~RZFrameGraphPassConcept() = default;

                virtual void operator()(RZFrameGraphPassResources &, void *) = 0;

                // delete these functions
                RZFrameGraphPassConcept(const RZFrameGraphPassConcept &)                = delete;
                RZFrameGraphPassConcept(RZFrameGraphPassConcept &&) noexcept            = delete;
                RZFrameGraphPassConcept &operator=(const RZFrameGraphPassConcept &)     = delete;
                RZFrameGraphPassConcept &operator=(RZFrameGraphPassConcept &&) noexcept = delete;
            };

            template<typename Data, typename ExecuteFunc>
            struct RZFrameGraphPass final : RZFrameGraphPassConcept
            {
                explicit RZFrameGraphPass(ExecuteFunc &&exec)
                    : execFunction{std::forward<ExecuteFunc>(exec)} {}

                void operator()(RZFrameGraphPassResources &resources, void *context) override
                {
                    execFunction(data, resources, context);
                }

                ExecuteFunc execFunction;
                Data        data{};
            };
        }    // namespace FrameGraph

    }    // namespace Graphics
}    // namespace Razix