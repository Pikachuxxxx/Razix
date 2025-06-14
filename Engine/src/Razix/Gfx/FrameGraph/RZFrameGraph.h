#pragma once

#include "Razix/Gfx/FrameGraph/RZBlackboard.h"
#include "Razix/Gfx/FrameGraph/RZFrameGraphResourcesTypeTraits.h"
#include "Razix/Gfx/FrameGraph/RZResourceEntry.h"
#include "Razix/Gfx/FrameGraph/RZResourceNode.h"

namespace Razix {
    namespace Gfx {
        namespace FrameGraph {

            /**
             * How does a pass gets the PassNode?
             * 
             * We need a pass node because it is the owner of the resources for that pass and contains access and views
             * 
             * --> FG has the actual resources (Resource Entry)
             * --> whereas, the PassNode has What resources
             * 
             * Combining these 2 we make a class per Pass and pass is to the Setup and Exec functions
             * 
             * Now we write a wrapper over it to make managing PassNode & FG interaction simple
             * 
             * But both the Setup and Exec functions doesn't need same privilege so we make 2 classes
             *  1. Setup needs rights to create and mark as read/write but it can't actually get the resource because registration is not done yet
             *  2. Where as Execute only needs a way to get the registered resource by verifying and no other privileges
             * 
             * So we name them Builder for Setup function and PassResourceDirectory for the Execute function
             * 
             * TODO: Derive them from a common class?
             */

            /* Resource builder for Setup function of the PassNode with read/write access */
            class RZPassResourceBuilder;
            /* Resource builder for Execute function of the PassNode with minimal get access */
            class RZPassResourceDirectory;

            // TODO!!!: Add budget info to RZApplication and RZFrameGraph

            /**
             * FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
             * Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
             * With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
             * Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
             */
            class RAZIX_API RZFrameGraph
            {
                friend class RZPassResourceBuilder;
                friend class RZPassResourceDirectory;

            public:
                RZFrameGraph() {}
                /* We don't want dangling frame graph resources and nodes */
                RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(RZFrameGraph)

                // TODO: Add OnUpdate and OnResize to the frame graph callback functions

                /**
                 * Callbacks to create the Frame Graph passes and execution using lambdas
                 * 
                 * SetupFunc is captured by reference as we execute it immediately
                 * ExecuteFunc is captured by value because we defer it's execution
                 */
                template<typename PassData, typename SetupFunc, typename ExecuteFunc, typename ResizeFunc>
                const PassData &addCallbackPass(const std::string_view name, SetupFunc &&setupFunc, ExecuteFunc &&executeFunc, ResizeFunc &&resizeFunc)
                {
                    // Compile time checks to make sure that the lambda functions are valid and have the right signature to be called by the pass
                    static_assert(std::is_invocable_v<SetupFunc, PassData &, RZPassResourceBuilder &>, "Invalid setup callback, check the signature again");
                    static_assert(std::is_invocable_v<ExecuteFunc, const PassData &, RZPassResourceDirectory &>, "Invalid exec callback, check the signature again");
                    static_assert(std::is_invocable_v<ResizeFunc, RZPassResourceDirectory &, u32, u32>, "Invalid resize callback, check the signature again");
                    // Also make sure the ExecuteFunc isn't too big
                    static_assert(sizeof(ExecuteFunc) < 1024, "Execute function captures too much");
                    static_assert(sizeof(ResizeFunc) < 1024, "Resize function captures too much");

                    // Now that the checks are done, let's create the pass and PassNode
                    auto *pass = new RZFrameGraphCodePass<PassData, ExecuteFunc, ResizeFunc>(std::forward<ExecuteFunc>(executeFunc), std::forward<ResizeFunc>(resizeFunc));
                    // Create the PassNode in the graph
                    RZPassNode &passNode = createPassNode(name, std::unique_ptr<RZFrameGraphCodePass<PassData, ExecuteFunc, ResizeFunc>>(pass));

                    // Create a builder for this PassNode
                    // SetupFunc gets PassNode via RZPassResourceBuilder and ExecFunc gets PassNode via RZPassResourceDirectory
                    RZPassResourceBuilder *builder = CreateBuilder(*this, passNode);

                    /**
                     * SetupFunc is captured by reference because we immediately execute where as ExecuteFunc execution is deferred so it's captured by value
                     */

                    // Call the setup function for the pass
                    std::invoke(setupFunc, pass->data, *builder);

                    // Return the data instance
                    return pass->data;
                }

                template<typename PassData, typename SetupFunc, typename ExecuteFunc>
                const PassData &addCallbackPass(const std::string_view name, SetupFunc &&setupFunc, ExecuteFunc &&executeFunc)
                {
                    auto emptyLambda = [=](RZPassResourceDirectory &, u32, u32) {};

                    return addCallbackPass<PassData>(name, setupFunc, std::forward<ExecuteFunc>(executeFunc), emptyLambda);
                }

                /**
                 * Callback to crate a standalone pass without any pass data
                 */
                template<typename SetupFunc, typename ExecuteFunc, typename ResizeFunc>
                void addCallbackPass(const std::string_view name, SetupFunc &&setupFunc, ExecuteFunc &&executeFunc, ResizeFunc &&resizeFunc)
                {
                    struct NoData
                    {};
                    addCallbackPass<NoData>(name, setupFunc, std::forward<ExecuteFunc>(executeFunc), std::forward<ResizeFunc>(resizeFunc));
                }

                template<typename SetupFunc, typename ExecuteFunc>
                void addCallbackPass(const std::string_view name, SetupFunc &&setupFunc, ExecuteFunc &&executeFunc)
                {
                    struct NoData
                    {};

                    auto emptyLambda = [=](RZPassResourceDirectory &, u32, u32) {};

                    addCallbackPass<NoData>(name, setupFunc, std::forward<ExecuteFunc>(executeFunc), emptyLambda);
                }

                /* Imports a external resource into the frame graph, for valid resources types only */
                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RAZIX_NO_DISCARD RZFrameGraphResource import(const std::string_view name, typename T::Desc &&desc, T &&resource)
                {
                    // same as createResource but we pass an actual resource instead of empty constructor to ResourceEntry
                    const uint32_t resourceId = static_cast<uint32_t>(m_ResourceRegistry.size());
                    m_ResourceRegistry.emplace_back(RZResourceEntry{resourceId, std::forward<typename T::Desc>(desc), std::forward<T>(resource), kResourceInitialVersion, true});    // Non-empty T constructor
                    // Create the node for this resource in the graph
                    RZFrameGraphResource id = createResourceNode(name, resourceId).m_ID;

                    // Register the name, this makes code based frame graph pass resources compatible with data driven passes
                    m_Blackboard.add(std::string(name), id);

                    return id;
                }

                /* Gets the resource descriptor */
                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE const typename T::Desc &getDescriptor(RZFrameGraphResource id)
                {
                    return getResourceEntry(id).getDescriptor<T>();
                }

#if 0    // RISKY TO CALL WITHOUT PASS_NODE VERFICATION
                /* Gets the resource */
                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE typename const T &get(RZFrameGraphResource id)
                {
                    // This is risky, if called without checking if the pass has read/write resource can cause a crash,
                    // also cannot be called in SetupFunc before resources are registered
                    return getResourceEntry(id).get<T>();
                }
#endif
                // TODO: clean this cluster fuck order of methods

                /* parse the frame graph from a given JSON file */
                bool parse(const std::string &path);
                /* Parses a Built in/User defined pass from a file */
                RAZIX_NO_DISCARD RZPassNode &parsePass(const std::string &passPath);
                /* Compiles the Frame Graph passes and culls any unused passes/resources */
                void compile();
                /* Executes the Frame Graph passes */
                void execute(void *transientAllocator);
                /* Resize the frame graph */
                void resize(u32 width, u32 height);
                /* Exports it GraphViz format */
                void exportToGraphViz(std::ostream &) const;
                /* Exports it in GraphViz format to given location */
                void exportToGraphViz(const std::string &location) const;
                /* Destroy the frame graph and it's resources */
                void destroy();

                /* Tell whether or no the current resource is valid to read/write */
                bool isValid(RZFrameGraphResource id);
                /* Gets the pass node */
                const RZPassNode &getPassNode(u32 idx) const { return m_PassNodes[idx]; }
                /* Get the resource node for a given frame graph resource */
                RZResourceNode &getResourceNode(RZFrameGraphResource id);
                /* Get the resource entry for a given frame graph resource */
                RZResourceEntry &getResourceEntry(RZFrameGraphResource id);
                /* Gets the blackboard database */
                RZBlackboard &getBlackboard() { return m_Blackboard; }

                // Export function to dot format for GraphViz
                friend std::ostream &operator<<(std::ostream &, const RZFrameGraph &);

                const std::string &getResourceName(RZFrameGraphResource id) const;

                inline static void ResetFirstFrame() { m_IsFirstFrame = true; }
                inline static bool IsFirstFrame() { return m_IsFirstFrame; }
                inline u32         getPassNodesSize() const { return static_cast<u32>(m_PassNodes.size()); }
                inline u32         getResourceNodesSize() const { return static_cast<u32>(m_ResourceNodes.size()); }

            private:
                std::vector<RZPassNode>      m_PassNodes;        /* List of all the pass nodes in the frame graph                             */
                std::vector<RZResourceNode>  m_ResourceNodes;    /* List of the all the resources nodes (including clones) in the frame graph */
                std::vector<RZResourceEntry> m_ResourceRegistry; /* List of Resource entries for each unique resource in the frame graph      */
                RZBlackboard                 m_Blackboard;       /* Blackboard stores a database of per pass node resources                   */

                static bool m_IsFirstFrame;

            private:
                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RAZIX_NO_DISCARD RZFrameGraphResource createResource(const std::string_view name, typename T::Desc &&desc)
                {
                    // Create a new Resource entry
                    const auto resourceId = static_cast<uint32_t>(m_ResourceRegistry.size());
                    m_ResourceRegistry.emplace_back(RZResourceEntry{resourceId, std::forward<typename T::Desc>(desc), T{}, kResourceInitialVersion, false});    // Empty T{} constructor because it's not an imported resource
                    // Create the node for this resource in the graph
                    RZFrameGraphResource id = createResourceNode(name, resourceId).m_ID;
                    return id;
                }

                RAZIX_NO_DISCARD RZPassNode          &createPassNode(const std::string_view name, std::unique_ptr<IRZFrameGraphPass> &&func);
                RAZIX_NO_DISCARD RZResourceNode      &createResourceNode(const std::string_view name, u32 resourceID);
                RAZIX_NO_DISCARD RZFrameGraphResource cloneResource(RZFrameGraphResource id);
                static RZPassResourceBuilder         *CreateBuilder(RZFrameGraph &fg, RZPassNode &passNode);
            };

            //-----------------------------------------------------------------------------------
            // RZPassResourceBuilder Class
            //-----------------------------------------------------------------------------------
            /**
             * Frame Graph Pass resource Builder Class
             * used for creating pass resources and marking them as read/write for a PassNode
             */
            class RAZIX_API RZPassResourceBuilder final
            {
                friend class RZFrameGraph;

            public:
                /* Deleting public empty constructor as the members variables are references and we don't want to have dangling framegraph and also not let the user, but instead the fg creates it */
                RAZIX_DELETE_PUBLIC_CONSTRUCTOR(RZPassResourceBuilder)

                /* Since FrameGraph is RAZIX_NONCOPYABLE_IMMOVABLE_CLASS, and we hold references to it, it's better we do the same */
                RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(RZPassResourceBuilder)

                /**
                 * Used to declare for creating the resource using the resource description
                 * 
                 * Using SFINANE this template function rejects overload sets for types that don't conform to concept rules
                 * Only the types that enforce resource entry concept rules can be created, others are rejected under SFINAE
                 * 
                 * Restricts failure of creating types which will be used by concept which might call non existent stuff 
                 */
                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RAZIX_NO_DISCARD inline RZFrameGraphResource create(const std::string_view name, typename T::Desc &&desc)
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
                RAZIX_NO_DISCARD RZFrameGraphResource write(RZFrameGraphResource id, u32 flags = kFlagsNone);

                /* Ensures that this pass is not culled during the frame graph compilation phase, single/hanging passes cans till exist and be executed */
                RZPassResourceBuilder &setAsStandAlonePass();
                RZPassResourceBuilder &setDepartment(Department dept);
                RZPassResourceBuilder &setCPUTime(f32 time);

                Department         getDepartment();
                Memory::BudgetInfo getPassCurrentBudget();
                f32                getPassCurrentCPUTimeBudget();
                u32                getPassCurrentMemoryBudget();

            private:
                RZFrameGraph &m_FrameGraph; /* Reference to the FrameGraph                                   */
                RZPassNode   &m_PassNode;   /* Current Pass node that this builder is building resources for */

            private:
                RZPassResourceBuilder(RZFrameGraph &frameGraph, RZPassNode &passNode);
            };

            //-----------------------------------------------------------------------------------
            // RZPassResourceDirectory Class
            //-----------------------------------------------------------------------------------
            /**
             * Safe way to access resources from the Execute function for the current pass
             * 
             * SetupFunc gets PassNode via Builder and ExecFunc gets PassNode via RZPassResourceDirectory
             * 
             * We can't directly ask the frame graph to give resource, 
             * they must be verified against the pass, if they are readable/writable 
             * by the pass or if the pass created it. For this very reason the Execute 
             * function gets this Combo of FG and PassNode for this pass that holds the 
             * resources it can read/write, we properly verify and access them
             * 
             * Now you might ask why we have getDescriptor in FG class, we only prevent
             * illegal access of resources not it's Desc, logically also it's fine if you think about it
             */
            class RAZIX_API RZPassResourceDirectory
            {
                friend class RZFrameGraph;

            public:
                RAZIX_DELETE_PUBLIC_CONSTRUCTOR(RZPassResourceDirectory);

                /* Gets the resource that is read/write by the current pass */
                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RAZIX_NO_DISCARD T &get(RZFrameGraphResource id)
                {
                    RAZIX_ASSERT(m_PassNode.canReadResouce(id) || m_PassNode.canCreateResouce(id) || m_PassNode.canWriteResouce(id), "Trying to get invalid resource, pass doesn't have access");
                    // This is a safe way that doesn't violate the design of the frame graph PassNodes
                    return m_FrameGraph.getResourceEntry(id).get<T>();
                }

                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE const typename T::Desc &getDescriptor(RZFrameGraphResource id) const
                {
                    RAZIX_ASSERT(m_PassNode.canReadResouce(id) || m_PassNode.canCreateResouce(id) || m_PassNode.canWriteResouce(id), "Trying to get invalid resource, pass doesn't have access");
                    // This is a safe way that doesn't violate the design of the frame graph PassNodes
                    return m_FrameGraph.getResourceEntry(id).getDescriptor<T>();
                }

                /* Verifies the type of the resource ID */
                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RAZIX_NO_DISCARD bool verifyType(RZFrameGraphResource id)
                {
                    RAZIX_ASSERT(m_PassNode.canReadResouce(id) || m_PassNode.canCreateResouce(id) || m_PassNode.canWriteResouce(id), "Trying to get invalid resource, pass doesn't have access");
                    // This is a safe way that doesn't violate the design of the frame graph PassNodes
                    auto res = m_FrameGraph.getResourceEntry(id).getModel<T>();
                    return res ? true : false;
                }

                /* Gets the resource name using the resource ID, this ie because we don't know if T::Desc will have a name member or not */
                ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RAZIX_NO_DISCARD const std::string &getResourceName(RZFrameGraphResource id)
                {
                    RAZIX_ASSERT(m_PassNode.canReadResouce(id) || m_PassNode.canCreateResouce(id) || m_PassNode.canWriteResouce(id), "Trying to get invalid resource, pass doesn't have access");
                    // This is a safe way that doesn't violate the design of the frame graph PassNodes
                    return m_FrameGraph.getResourceNode(id).getName();
                }

            private:
                RZFrameGraph &m_FrameGraph;
                RZPassNode   &m_PassNode;

            private:
                RZPassResourceDirectory(RZFrameGraph &frameGraph, RZPassNode &passNode);
            };
            //-----------------------------------------------------------------------------------
        }    // namespace FrameGraph
    }        // namespace Gfx
}    // namespace Razix
