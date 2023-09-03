#pragma once

#include "Razix/Graphics/FrameGraph/RZFrameGraphResourceTypeChecks.h"
#include "Razix/Graphics/FrameGraph/RZResourceEntry.h"
#include "Razix/Graphics/FrameGraph/RZResourceNode.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            constexpr u32 kFlagsNone = ~0;

            /**
             * FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
             * Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
             * With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
             * Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
             */
            class RAZIX_API RZFrameGraph
            {
                friend class RZFrameGraphPassResourcesDirectory;

            public:
                // TODO: Separate the builder into it's own file

                //-----------------------------------------------------------------------------------
                /**
                 * Frame Graph Passes Builder Class, used for creating FrameGraph resources and marking them as read/write for a pass node
                 */
                class RZBuilder final
                {
                    friend class RZFrameGraph;

                public:
                    /* Deleting public empty constructor as the members variables are references and we don't want to have dangling framegraph and also not let the user, but instead the fg creates it */
                    RAZIX_DELETE_PUBLIC_CONSTRUCTOR(RZBuilder)

                    /* Since FrameGraph is RAZIX_NONCOPYABLE_NONMOVABLE_CLASS, and we hold references to it, it's better we do the same */
                    RAZIX_NONCOPYABLE_NONMOVABLE_CLASS(RZBuilder)

                    /**
                     * Used to declare for creating the resource using the resource description
                     * 
                     * Using SFINANE this template function rejects overload sets for types that don't conform to concept rules
                     * Only the types that enforce resource entry concept rules can be created, others are rejected under SFINAE
                     * 
                     * Restricts failure of creating types which will be used by concept which might call non existent stuff 
                     */
                    ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE inline RZFrameGraphResource create(const std::string_view name, typename T::Desc &&)
                    {
                        const auto id = m_FrameGraph.createResource<T>(name, std::move(desc));
                        return m_PassNode.m_Creates.emplace_back(id);
                    }

                    /**
                     * Marks the resource as a readable resources for the current pass node
                     * 
                     * @param id ID of the FrameGraphResource which u32
                     * @param flags Binding info can be passes as flags to manage descriptor tables and barriers
                     */
                    RZFrameGraphResource read(RZFrameGraphResource id, u32 flags = kFlagsNone);
                    /**
                     * Marks the resource as a writable (render target) resources for the current pass node
                     * 
                     * @param id ID of the FrameGraphResource which u32
                     * @param flags Binding info can be passes as flags to manage descriptor tables and barriers
                     */
                    RZFrameGraphResource write(RZFrameGraphResource id, u32 flags = kFlagsNone);

                    /* Ensures that this pass is not culled during the frame graph compilation phase, single/hanging passes cans till exist and be executed */
                    RZBuilder &setAsStandAlonePass();

                private:
                    RZFrameGraph &m_FrameGraph; /* Reference to the FrameGraph                                      */
                    RZPassNode   &m_PassNode;   /* Current Pass node that this builder is building resources for    */

                private:
                    RZBuilder(RZFrameGraph &frameGraph, RZPassNode &passNode);
                };
                //-----------------------------------------------------------------------------------

            public:
                /* We don't want dangling frame graph resources and nodes */
                RAZIX_NONCOPYABLE_NONMOVABLE_CLASS(RZFrameGraph)

                // TODO: Add OnUpdate and OnResize to the frame graph callback functions

                /**
                 * Callbacks to create the Frame Graph passes and execution using lambdas
                 */
                template<typename PassData, typename SetupFunc, typename ExecuteFunc>
                const PassData &addCallbackPass(const std::string_view name, SetupFunc &&setupFunc, ExecuteFunc &&executeFunc)
                {
                }

                /**
                 * Callback to crate a standalone pass without and pass data
                 */
                template<typename SetupFunc, typename ExecuteFunc>
                void addCallbackPass(const std::string_view name, SetupFunc &&setupFunc, ExecuteFunc &&executeFunc)
                {
                }

                /* Imports a external resource into the frame graph */
                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RZFrameGraphResource import(const std::string_view name, typename T::Desc &&, T &&)
                {
                }

                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE typename const T::Desc &getDescriptor(RZFrameGraphResource id)
                {
                }

                /* Compiles the Frame Graph passes and culls any unused passes/resources */
                void compile();
                /* Executes the Frame Graph passes */
                void execute(void *renderContext = nullptr, void *allocator = nullptr);
                /* Exports it GraphViz format */
                void exportToGraphViz(std::ostream &) const;

                /* Tell whether or no the current resource is valid to read/write */
                bool                  isValid(RZFrameGraphResource id);
                const RZResourceNode &getResourceNode(RZFrameGraphResource id) const;
                RZResourceEntry      &getResourceEntry(RZFrameGraphResource id);

                // Export function to dot format for GraphViz
                friend std::ostream &operator<<(std::ostream &, const RZFrameGraph &);

            private:
                std::vector<RZPassNode>      m_PassNodes;
                std::vector<RZResourceNode>  m_ResourceNodes;
                std::vector<RZResourceEntry> m_ResourceRegistry;

            private:
                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RZFrameGraphResource createResource(const std::string_view name, typename T::Desc &&)
                {
                }

                RZPassNode          &createPassNode(const std::string_view name, std::unique_ptr<IRZFrameGraphPass> &&);
                RZResourceNode      &createResourceNode(const std::string_view name, u32 resourceID);
                RZFrameGraphResource cloneResource(RZFrameGraphResource id);
            };

            // WHAT'S THE POINT OF THIS?
            //-----------------------------------------------------------------------------------
            // RZFrameGraphPassResourcesDirectory Class
            //-----------------------------------------------------------------------------------

            /**
             * Holds the list of all the resources of a Frame Graph Pass
             */
            class RZFrameGraphPassResourcesDirectory
            {
                friend class RZFrameGraph;

            public:
                RAZIX_DELETE_PUBLIC_CONSTRUCTOR(RZFrameGraphPassResourcesDirectory);

                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE T                      &get(RZFrameGraphResource id);
                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE typename const T::Desc &getDescriptor(RZFrameGraphResource id) const;

            private:
                RZFrameGraph &m_FrameGraph;
                RZPassNode   &m_PassNode;

            private:
                RZFrameGraphPassResourcesDirectory(RZFrameGraph &frameGraph, RZPassNode &passNode);
            };

        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix
