#pragma once

#include "Razix/Gfx/FrameGraph/RZBlackboard.h"
#include "Razix/Gfx/FrameGraph/RZFrameGraphResourcesTypeTraits.h"
#include "Razix/Gfx/FrameGraph/RZResourceEntry.h"
#include "Razix/Gfx/FrameGraph/RZResourceNode.h"

namespace Razix {
    namespace Gfx {

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
          */

        // Resource builder for Setup function of the PassNode with read/write access
        class RZPassResourceBuilder;
        // Resource builder for Execute function of the PassNode with minimal get access
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
            // We don't want dangling frame graph resources and nodes
            RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(RZFrameGraph)

            template<typename PassData, typename SetupFunc, typename ExecuteFunc, typename ResizeFunc>
            const PassData& addCallbackPass(const std::string_view name, SetupFunc&& setupFunc, ExecuteFunc&& executeFunc, ResizeFunc&& resizeFunc)
            {
                // Compile time checks to make sure that the lambda functions are valid and have the right signature to be called by the pass
                static_assert(std::is_invocable_v<SetupFunc, PassData&, RZPassResourceBuilder&>, "Invalid setup callback, check the signature again");
                static_assert(std::is_invocable_v<ExecuteFunc, const PassData&, RZPassResourceDirectory&>, "Invalid exec callback, check the signature again");
                static_assert(std::is_invocable_v<ResizeFunc, RZPassResourceDirectory&, u32, u32>, "Invalid resize callback, check the signature again");
                // Also make sure the ExecuteFunc isn't too big
                static_assert(sizeof(ExecuteFunc) < 1024, "Execute function captures too much");
                static_assert(sizeof(ResizeFunc) < 1024, "Resize function captures too much");

                // Now that the checks are done, let's create the pass and PassNode
                auto* pass = new RZFrameGraphCodePass<PassData, ExecuteFunc, ResizeFunc>(std::forward<ExecuteFunc>(executeFunc), std::forward<ResizeFunc>(resizeFunc));
                // Create the PassNode in the graph
                RZPassNode& passNode = createPassNodeRef(name, std::unique_ptr<RZFrameGraphCodePass<PassData, ExecuteFunc, ResizeFunc>>(pass));

                // Create a builder for this PassNode
                // SetupFunc gets PassNode via RZPassResourceBuilder and ExecFunc gets PassNode via RZPassResourceDirectory
                RZPassResourceBuilder* builder = CreateBuilder(*this, passNode);

                /**
                 * SetupFunc is captured by reference because we immediately execute where as ExecuteFunc execution is deferred so it's captured by value
                 */

                // Call the setup function for the pass
                std::invoke(setupFunc, pass->data, *builder);

                return pass->data;
            }

            template<typename PassData, typename SetupFunc, typename ExecuteFunc>
            const PassData& addCallbackPass(const std::string_view name, SetupFunc&& setupFunc, ExecuteFunc&& executeFunc)
            {
                auto emptyLambda = [=](RZPassResourceDirectory&, u32, u32) {};
                return addCallbackPass<PassData>(name, setupFunc, std::forward<ExecuteFunc>(executeFunc), emptyLambda);
            }

            template<typename SetupFunc, typename ExecuteFunc, typename ResizeFunc>
            void addCallbackPass(const std::string_view name, SetupFunc&& setupFunc, ExecuteFunc&& executeFunc, ResizeFunc&& resizeFunc)
            {
                struct NoData
                {};
                addCallbackPass<NoData>(name, setupFunc, std::forward<ExecuteFunc>(executeFunc), std::forward<ResizeFunc>(resizeFunc));
            }

            template<typename SetupFunc, typename ExecuteFunc>
            void addCallbackPass(const std::string_view name, SetupFunc&& setupFunc, ExecuteFunc&& executeFunc)
            {
                struct NoData
                {};
                auto emptyLambda = [=](RZPassResourceDirectory&, u32, u32) {};

                addCallbackPass<NoData>(name, setupFunc, std::forward<ExecuteFunc>(executeFunc), emptyLambda);
            }

            /* Imports a external resource into the frame graph, for valid resources types only */
            ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RAZIX_NO_DISCARD RZFrameGraphResource import(const std::string_view name, typename T::Desc&& desc, T&& resource)
            {
                // same as createResource but we pass an actual resource instead of empty constructor to ResourceEntry
                const uint32_t resourceId = static_cast<uint32_t>(m_ResourceRegistry.size());
                m_ResourceRegistry.emplace_back(RZResourceEntry{resourceId, std::forward<typename T::Desc>(desc), std::forward<T>(resource), kRESOURCE_INITIAL_VERSION, true});    // Non-empty T constructor
                // Create the node for this resource in the graph
                RZFrameGraphResource id = createResourceNodeRef(name, resourceId).m_ID;

                // Register the name, this makes code based frame graph pass resources compatible with data driven passes
                m_Blackboard.add(std::string(name), id);

                return id;
            }

            ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE const typename T::Desc& getDescriptor(RZFrameGraphResource id)
            {
                return getResourceEntryRef(id).getDescriptor<T>();
            }

            bool                         parse(const std::string& path);
            RAZIX_NO_DISCARD RZPassNode& parsePass(const std::string& passPath);

            void compile();
            void execute(void* transientAllocator);
            void resize(u32 width, u32 height);
            void exportToGraphViz(std::ostream&) const;
            void exportToGraphViz(const std::string& location) const;
            void destroy();
            bool isValid(RZFrameGraphResource id);

            const std::string&     getResourceName(RZFrameGraphResource id) const;
            const std::string&     getResourceEntryName(RZFrameGraphResource id) const;
            const RZResourceNode&  getResourceNode(RZFrameGraphResource id) const;
            const RZResourceEntry& getResourceEntry(RZFrameGraphResource id) const;

            inline RZBlackboard&     getBlackboard() { return m_Blackboard; }
            inline const RZPassNode& getPassNode(u32 idx) const { return m_PassNodes[idx]; }
            inline static void       ResetFirstFrame() { m_IsFirstFrame = true; }
            inline static bool       IsFirstFrame() { return m_IsFirstFrame; }
            inline u32               getPassNodesSize() const { return static_cast<u32>(m_PassNodes.size()); }
            inline u32               getResourceNodesSize() const { return static_cast<u32>(m_ResourceNodes.size()); }
            inline u32               getCompiledResourceEntriesSize() const { return static_cast<u32>(m_CompiledResourceEntries.size()); }
            std::vector<u32>         getCompiledResourceEntries() const { return m_CompiledResourceEntries; }
            std::vector<u32>         getCompiledResourceNodes() const { return m_CompiledResourceIndices; }
            std::vector<u32>         getCompiledPassNodes() const { return m_CompiledPassIndices; }

            // Export function to dot format for GraphViz
            friend std::ostream& operator<<(std::ostream&, const RZFrameGraph&);

            const AliasingBook& getAliasBook() const { return m_TestAliasbook; }

        private:
            std::vector<RZPassNode>      m_PassNodes;
            std::vector<RZResourceNode>  m_ResourceNodes;
            std::vector<RZResourceEntry> m_ResourceRegistry;
            RZBlackboard                 m_Blackboard;
            std::vector<u32>             m_CompiledPassIndices;
            std::vector<u32>             m_CompiledResourceIndices;
            std::vector<u32>             m_CompiledResourceEntries;

            AliasingBook m_TestAliasbook;

            static bool m_IsFirstFrame;

        private:
            ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RAZIX_NO_DISCARD RZFrameGraphResource createResource(const std::string_view name, typename T::Desc&& desc)
            {
                // Create a new Resource entry
                const auto resourceId = static_cast<uint32_t>(m_ResourceRegistry.size());
                m_ResourceRegistry.emplace_back(RZResourceEntry{resourceId, std::forward<typename T::Desc>(desc), T{}, kRESOURCE_INITIAL_VERSION, false});    // Empty T{} constructor because it's not an imported resource
                // Create the node for this resource in the graph
                RZFrameGraphResource id = createResourceNodeRef(name, resourceId).m_ID;
                return id;
            }

            static RZPassResourceBuilder* CreateBuilder(RZFrameGraph& fg, RZPassNode& passNode);

            RZResourceNode&                       getResourceNodeRef(RZFrameGraphResource id);
            RZResourceEntry&                      getResourceEntryRef(RZFrameGraphResource id);
            RAZIX_NO_DISCARD RZPassNode&          createPassNodeRef(const std::string_view name, std::unique_ptr<IRZFrameGraphPass>&& func);
            RAZIX_NO_DISCARD RZResourceNode&      createResourceNodeRef(const std::string_view name, u32 resourceID);
            RAZIX_NO_DISCARD RZFrameGraphResource cloneResource(RZFrameGraphResource id);
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
            ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RAZIX_NO_DISCARD inline RZFrameGraphResource create(const std::string_view name, typename T::Desc&& desc)
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
            RZPassResourceBuilder& setAsStandAlonePass();
            RZPassResourceBuilder& setDepartment(Department dept);
            RZPassResourceBuilder& setCPUTime(f32 time);

            Department         getDepartment();
            Memory::BudgetInfo getPassCurrentBudget();
            f32                getPassCurrentCPUTimeBudget();
            u32                getPassCurrentMemoryBudget();

        private:
            RZFrameGraph& m_FrameGraph; /* Reference to the FrameGraph                                   */
            RZPassNode&   m_PassNode;   /* Current Pass node that this builder is building resources for */

        private:
            RZPassResourceBuilder(RZFrameGraph& frameGraph, RZPassNode& passNode);
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
            ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RAZIX_NO_DISCARD T& get(RZFrameGraphResource id)
            {
                RAZIX_ASSERT(m_PassNode.canReadResouce(id) || m_PassNode.canCreateResouce(id) || m_PassNode.canWriteResouce(id), "Trying to get invalid resource, pass doesn't have access");
                // This is a safe way that doesn't violate the design of the frame graph PassNodes
                return m_FrameGraph.getResourceEntryRef(id).get<T>();
            }

            ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE const typename T::Desc& getDescriptor(RZFrameGraphResource id) const
            {
                RAZIX_ASSERT(m_PassNode.canReadResouce(id) || m_PassNode.canCreateResouce(id) || m_PassNode.canWriteResouce(id), "Trying to get invalid resource, pass doesn't have access");
                // This is a safe way that doesn't violate the design of the frame graph PassNodes
                return m_FrameGraph.getResourceEntryRef(id).getDescriptor<T>();
            }

            /* Verifies the type of the resource ID */
            ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RAZIX_NO_DISCARD bool verifyType(RZFrameGraphResource id)
            {
                RAZIX_ASSERT(m_PassNode.canReadResouce(id) || m_PassNode.canCreateResouce(id) || m_PassNode.canWriteResouce(id), "Trying to get invalid resource, pass doesn't have access");
                // This is a safe way that doesn't violate the design of the frame graph PassNodes
                auto res = m_FrameGraph.getResourceEntryRef(id).getModel<T>();
                return res ? true : false;
            }

            /* Gets the resource name using the resource ID, this ie because we don't know if T::Desc will have a name member or not */
            ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE RAZIX_NO_DISCARD const std::string& getResourceName(RZFrameGraphResource id)
            {
                RAZIX_ASSERT(m_PassNode.canReadResouce(id) || m_PassNode.canCreateResouce(id) || m_PassNode.canWriteResouce(id), "Trying to get invalid resource, pass doesn't have access");
                // This is a safe way that doesn't violate the design of the frame graph PassNodes
                return m_FrameGraph.getResourceNodeRef(id).getName();
            }

        private:
            RZFrameGraph& m_FrameGraph;
            RZPassNode&   m_PassNode;

        private:
            RZPassResourceDirectory(RZFrameGraph& frameGraph, RZPassNode& passNode);
        };
    }    // namespace Gfx
}    // namespace Razix
