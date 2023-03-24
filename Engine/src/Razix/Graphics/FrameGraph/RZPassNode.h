#pragma once

#include "Razix/Graphics/FrameGraph/RZFrameGraphPass.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraphResource.h"
#include "Razix/Graphics/FrameGraph/RZGraphNode.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            /* Defines a pass node in the frame graph */
            class RZPassNode final : public RZGraphNode
            {
                friend class RZFrameGraph;

            public:
                bool canCreateResouce(RZFrameGraphResource resourceID) const;
                bool canReadResouce(RZFrameGraphResource resourceID) const;
                bool canWriteResouce(RZFrameGraphResource resourceID) const;
                bool isStandAlone() const;
                bool canExecute() const;

            private:
                std::unique_ptr<RZFrameGraphPassConcept> m_Exec;

                std::vector<RZFrameGraphResource> m_Creates;
                std::vector<RZFrameGraphResource> m_Reads;
                std::vector<RZFrameGraphResource> m_Writes;

                bool m_IsStandAlone{false};

            private:
                RZPassNode(const std::string_view name, u32 id, std::unique_ptr<RZFrameGraphPassConcept> &&);

                RZFrameGraphResource read(RZFrameGraphResource id);
                RZFrameGraphResource write(RZFrameGraphResource id);
            };
        }    // namespace FrameGraph
    }    // namespace Graphics
}    // namespace Razix
