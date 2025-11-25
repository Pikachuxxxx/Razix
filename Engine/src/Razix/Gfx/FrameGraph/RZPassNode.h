#pragma once

#include "Razix/Core/Memory/RZMemoryBudgets.h"
#include "Razix/Core/RZDepartments.h"

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/

#include "Razix/Gfx/FrameGraph/RZFrameGraphPass.h"
#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"
#include "Razix/Gfx/FrameGraph/RZGraphNode.h"

namespace Razix {
    namespace Gfx {

        class RAZIX_API RZPassNode final : public RZGraphNode
        {
            // Since we deleted the public constructor of RZGraphNode because we stated that only FrameGraph
            // can create nodes we keep that promise by making FrameGraph a friend
            friend class RZFrameGraph;

            // Pass to look into PassNodes uses 2 classes so we allow them to be friends
            friend class RZPassResourceBuilder;
            friend class RZPassResourceDirectory;

            RZPassNode(const RZString& name, u32 id, std::unique_ptr<IRZFrameGraphPass>&& exec);

        public:
            // TODO: Make these inline as well?
            bool canExecute() const;
            bool canCreateResouce(RZFrameGraphResource resourceID) const;
            bool canReadResouce(RZFrameGraphResource resourceID) const;
            bool canWriteResouce(RZFrameGraphResource resourceID) const;

            void createDeferredResourceView(RZFrameGraphResource id, rz_handle resHandle, RZFrameGraphResourceAcessView& accessView);

            inline rz_gfx_resource_view_handle getResourceViewHandle(RZFrameGraphResource id)
            {
                return getResourceAccessView(id).resViewHandle;
            }

            inline const RZFrameGraphResourceAcessView& getResourceAccessView(RZFrameGraphResource id)
            {
                return getResourceAccessView<const RZFrameGraphResourceAcessView>(id);
            }

            inline RZFrameGraphResourceAcessView& getResourceAccessViewRef(RZFrameGraphResource id)
            {
                return getResourceAccessView<RZFrameGraphResourceAcessView&>(id);
            }

            inline const auto&               getCreateResources() const { return m_Creates; }
            inline const auto&               getInputResources() const { return m_Reads; }
            inline const auto&               getOutputResources() const { return m_Writes; }
            inline bool                      isStandAlone() const { return m_IsStandAlone; }
            inline bool                      isDataDriven() const { return m_IsDataDriven; }
            inline Department                getDepartment() const { return m_Department; }
            inline const Memory::BudgetInfo& getCurrentPassBudget() const { return m_CurrentPassBudget; }

        private:
            std::unique_ptr<IRZFrameGraphPass> m_Exec;
            // TODO: Implement this
            //std::unique_ptr<IRZFrameGraphPass> m_Update; /* The update lambda function to be called for the pass */

            /**
             * m_Creates is used to lazily call create, storing them in the framegraph requires us to create at the start of each frame
             * but storing them in the PassNode will only allocate memory when needed by pass, this helps with memory aliasing and deferred creation
             * 
             * The same also goes for resource views, we store them in the PassNode and only create them when the pass is executed
             * FIXME: But this also means extra tracking of rz_gfx_resource_view_desc for deferred creation after the actual resource is created, waste of memory!
             */

            RZDynamicArray<RZFrameGraphResource>                           m_Creates           = {};
            RZHashMap<RZFrameGraphResource, RZFrameGraphResourceAcessView> m_Reads             = {};
            RZHashMap<RZFrameGraphResource, RZFrameGraphResourceAcessView> m_Writes            = {};
            bool                                                           m_IsStandAlone      = false;
            bool                                                           m_IsDataDriven      = false;
            Department                                                     m_Department        = Department::NONE;
            Memory::BudgetInfo                                             m_CurrentPassBudget = {};

        private:
            template<typename T>
            T& getResourceAccessView(RZFrameGraphResource id)
            {
                auto ri = m_Reads.find(id);
                if (ri != m_Reads.end())
                    return ri->second;
                auto wi = m_Writes.find(id);
                if (wi != m_Writes.end())
                    return wi->second;

                static RZFrameGraphResourceAcessView defaultView{};
                return const_cast<T&>(defaultView);
            }

            RZFrameGraphResource registerResourceForRead(RZFrameGraphResource id, rz_gfx_resource_view_desc viewDesc);
            RZFrameGraphResource registerResourceForWrite(RZFrameGraphResource id, rz_gfx_resource_view_desc viewDesc);
            void                 destroyDeferredResourceViews();
            void                 recreateDeferredResourceViews();
        };
    }    // namespace Gfx
}    // namespace Razix
