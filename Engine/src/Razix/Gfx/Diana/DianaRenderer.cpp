// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DianaRenderer.h"

namespace Razix {
    namespace Gfx {
        namespace Diana {

            Razix::RZWorld* BuildRazixWorld(RZScene* scenes, RZSceneView* views)
            {
                return nullptr;
            }

            Razix::Gfx::Drawables BuildDrawables(RZScene* scene)
            {
                Drawables d;
                return d;
            }

            Razix::Gfx::Batches BuildBatches(Drawables* drawables)
            {
                Batches b;
                return b;
            }

            Razix::RZSceneView* ExtractSceneViews(RZScene* scene)
            {
                return nullptr;
            }

            Razix::Gfx::DrawData* ExtractBatchDrawData(Batch* batch)
            {
                return nullptr;
            }

            Razix::Gfx::DrawData* ExtractDrawableDrawData(Drawable* drawable)
            {
                return nullptr;
            }
        }    // namespace Diana
    }    // namespace Gfx
}    // namespace Razix