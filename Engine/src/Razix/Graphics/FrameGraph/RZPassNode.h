#pragma once

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/

#include "Razix/Graphics/FrameGraph/RZFrameGraphPass.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"
#include "Razix/Graphics/FrameGraph/RZGraphNode.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            /* Defines a pass node in the frame graph */
            class RZPassNode final : public RZGraphNode
            {
                // Since we deleted the public constructor of RZGraphNode because we stated that only FrameGraph
                // can create nodes we keep that promise by making FrameGraph a friend
                friend class RZFrameGraph;

            public:
                /* Not sure why this is needed ??? */
                bool canCreateResouce(RZFrameGraphResource resourceID) const;
                /* Used to check whether the given resource ID is allowed to be read by the node or not */
                bool canReadResouce(RZFrameGraphResource resourceID) const;
                /* Used to check whether the given resource ID is allowed to write by the node or not */
                bool canWriteResouce(RZFrameGraphResource resourceID) const;
                /* Whether or not the pass is stand alone */
                bool isStandAlone() const;
                /** 
                 * To check whether or not to be culled
                 * if either the node has refcount > 0 or is stand alone it shouldn't be culled and allowed to be executed 
                 */
                bool canExecute() const;

            private:
                std::unique_ptr<RZFrameGraphPassConcept> m_Exec; /* The execution lambda function to be called for the pass */
                // TODO: Implement this
                //std::unique_ptr<RZFrameGraphPassConcept> m_Update; /* The update lambda function to be called for the pass */
                //std::unique_ptr<RZFrameGraphPassConcept> m_Update; /* The resize lambda function to be called for the pass */

                std::vector<RZFrameGraphResource>          m_Creates;    // WHY???


                std::vector<RZFrameGraphResourceAcessView> m_Reads;      /* List of all the resource views that are read by this node */
                std::vector<RZFrameGraphResourceAcessView> m_Writes;     /* List of all the resources view that are written by this node */

                bool m_IsStandAlone{false};

            private:
                RZPassNode(const std::string_view name, u32 id, std::unique_ptr<RZFrameGraphPassConcept> &&);

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
    }        // namespace Graphics
}    // namespace Razix
