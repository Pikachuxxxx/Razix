#pragma once

#include "Razix/Graphics/FrameGraph/RZFrameGraphTypeTraits.h"
#include "Razix/Graphics/FrameGraph/RZResourceEntry.h"
#include "Razix/Graphics/FrameGraph/RZResourceNode.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            /**
             * FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
             * Adapted from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
             */
            class RAZIX_API RZFrameGraph
            {
                friend class RZFrameGraphPassResources;

                /**
                 * Frame Graph Passes Builder Class
                 */
                class RZBuilder
                {
                public:
                    RZBuilder()                      = delete;
                    RZBuilder(const RZBuilder &)     = delete;
                    RZBuilder(RZBuilder &&) noexcept = delete;

                    RZBuilder &operator=(const RZBuilder &)     = delete;
                    RZBuilder &operator=(RZBuilder &&) noexcept = delete;

                    /**
                     * Used to declare for creating the resource
                     */
                    ENFORCE_CONCEPT RZFrameGraphResource create(const std::string_view name, typename T::CreateDesc &&);

                    RZFrameGraphResource read(RZFrameGraphResource id);
                    RZFrameGraphResource write(RZFrameGraphResource id);

                    /* Ensures that this pass is not culled during the frame graph compilation phase, single/hanging passes cans till exist and be executed */
                    RZBuilder &setAsStandAlonePass();

                private:
                    RZFrameGraph &m_FrameGraph;
                    RZPassNode   &m_PassNode;

                private:
                    RZBuilder(RZFrameGraph &, RZPassNode &);
                };

            public:
                RZFrameGraph()  = default;
                ~RZFrameGraph() = default;

                RZFrameGraph(const RZFrameGraph &)     = delete;
                RZFrameGraph(RZFrameGraph &&) noexcept = delete;

                RZFrameGraph &operator=(const RZFrameGraph &)     = delete;
                RZFrameGraph &operator=(RZFrameGraph &&) noexcept = delete;

                /**
                 * Callbacks to create the Frame Graph passes and execution using lambdas
                 */
                template<typename PassData, typename SetupFunc, typename ExecuteFunc>
                const PassData &addCallbackPass(const std::string_view name, SetupFunc &&setupFunc, ExecuteFunc &&executeFunc);

                /**
                 * Callback to crate a standalone pass without and pass data
                 */
                template<typename SetupFunc, typename ExecuteFunc>
                void addCallbackPass(const std::string_view name, SetupFunc &&setupFunc, ExecuteFunc &&executeFunc);

                /* Compiles the Frame Graph passes and culls any unused passes/resources */
                void compile();
                /* Executes the Frame Graph passes */
                void execute(void *renderContext = nullptr, void *allocator = nullptr);

                void exportToGraphViz(std::ostream &) const;

                /* Imports a external resource into the frame graph */
                ENFORCE_CONCEPT RZFrameGraphResource import(const std::string_view name, typename T::CreateDesc &&, T &&);

                /* Tell whether or no the current resource is valid to read/write */
                bool isValid(RZFrameGraphResource id);

                ENFORCE_CONCEPT typename const T::CreateDesc &getDescriptor(RZFrameGraphResource id);

                const RZResourceNode  &getResourceNode(RZFrameGraphResource id) const;
                RZResourceEntry &getResourceEntry(RZFrameGraphResource id);

                // Export function to dot format for GraphViz
                friend std::ostream &
                operator<<(std::ostream &, const RZFrameGraph &);

            private:
                std::vector<RZPassNode>      m_PassNodes;
                std::vector<RZResourceNode>  m_ResourceNodes;
                std::vector<RZResourceEntry> m_ResourceRegistry;

            private:
                RZPassNode& createPassNode(const std::string_view name, std::unique_ptr<RZFrameGraphPassConcept> &&);

                ENFORCE_CONCEPT RZFrameGraphResource createResource(const std::string_view name, typename T::CreateDesc &&);

                RZResourceNode &createResourceNode(const std::string_view name, uint32_t resourceID);

                RZFrameGraphResource cloneResource(RZFrameGraphResource id);
            };

            //-----------------------------------------------------------------------------------
            // RZFrameGraphPassResources Class
            //-----------------------------------------------------------------------------------

            /**
             * Holds the list of all the resources of a Frame Graph Pass
             */
            class RZFrameGraphPassResources
            {
                friend class RZFrameGraph;

            public:
                RZFrameGraphPassResources()  = delete;
                ~RZFrameGraphPassResources() = default;

                ENFORCE_CONCEPT T &get(RZFrameGraphResource id);

                ENFORCE_CONCEPT typename const T::CreateDesc &getDescriptor(RZFrameGraphResource id) const;

            private:
                RZFrameGraph &m_FrameGraph;
                RZPassNode   &m_PassNode;

            private:
                RZFrameGraphPassResources(RZFrameGraph &, RZPassNode &);
            };

        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
