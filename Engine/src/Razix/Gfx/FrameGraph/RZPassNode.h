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
        namespace FrameGraph {

            /* Defines a pass node in the frame graph */
            class RAZIX_API RZPassNode final : public RZGraphNode
            {
                // Since we deleted the public constructor of RZGraphNode because we stated that only FrameGraph
                // can create nodes we keep that promise by making FrameGraph a friend
                friend class RZFrameGraph;

                // Pass to look into PassNodes uses 2 classes so we allow them to be friends
                friend class RZPassResourceBuilder;
                friend class RZPassResourceDirectory;

                /**
                 * Creates a pass node using a name, id and a lambda execution function
                 */
                RZPassNode(const std::string_view name, u32 id, std::unique_ptr<IRZFrameGraphPass>&& exec);

            public:
                /* To check whether or no this node has a copy of the resource already created or not */
                bool
                canCreateResouce(RZFrameGraphResource resourceID) const;
                /* Used to check whether the given resource ID is allowed to be read by the node or not */
                bool canReadResouce(RZFrameGraphResource resourceID) const;
                /* Used to check whether the given resource ID is allowed to write by the node or not */
                bool canWriteResouce(RZFrameGraphResource resourceID) const;
                /* Whether or not the pass is stand alone */
                RAZIX_INLINE bool isStandAlone() const { return m_IsStandAlone; }
                /* Whether or not the pass is data driven */
                RAZIX_INLINE bool isDataDriven() const { return m_IsDataDriven; }
                /** 
                 * To check whether or not to be culled
                 * if either the node has refcount > 0 or is stand alone it shouldn't be culled and allowed to be executed 
                 */
                bool canExecute() const;

                RAZIX_INLINE const std::vector<RZFrameGraphResource>& getCreatResources() const { return m_Creates; }
                RAZIX_INLINE const std::vector<RZFrameGraphResourceAcessView>& getInputResources() const { return m_Reads; }
                RAZIX_INLINE const std::vector<RZFrameGraphResourceAcessView>& getOutputResources() const { return m_Writes; }

            private:
                std::unique_ptr<IRZFrameGraphPass> m_Exec; /* The execution lambda function to be called for the pass */
                // TODO: Implement this
                //std::unique_ptr<RZFrameGraphPassConcept> m_Update; /* The update lambda function to be called for the pass */

                /**
                 * m_Creates is used to lazily call create, storing them in the framegraph requires us to create at the start of each frame
                 * but storing them in the PassNode will only allocate memory when needed by pass, this helps with memory aliasing and deferred creation
                 */

                std::vector<RZFrameGraphResource>          m_Creates;              /* List of all the resources created on this node                */
                std::vector<RZFrameGraphResourceAcessView> m_Reads;                /* List of all the resource views that are read by this node     */
                std::vector<RZFrameGraphResourceAcessView> m_Writes;               /* List of all the resources view that are written by this node  */
                bool                                       m_IsStandAlone = false; /* Whether or no the pass is stand alone                         */
                bool                                       m_IsDataDriven = false; /* Whether or no the it's data driven                            */
                Department                                 m_Department;           /* The department this pass belongs to */
                Memory::BudgetInfo                         m_CurrentPassBudget;    /* Pass current budget tracking */

            private:
                /**
                 * Marks a resource as read for the current pass node
                 * Store it's flags to create an access view
                 */
                RZFrameGraphResource registerResourceForRead(RZFrameGraphResource id, u32 flags);
                /**
                 * Marks a resource as write for the current pass node
                 * Store it's flags to create an access view
                 */
                RZFrameGraphResource registerResourceForWrite(RZFrameGraphResource id, u32 flags);
            };
        }    // namespace FrameGraph
    }        // namespace Gfx
}    // namespace Razix
