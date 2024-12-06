// clang-format off
#include "rzxpch.h"
// clang-format on
#include "DianaRenderer.h"

namespace Razix {
    namespace Graphics {
        namespace Diana {

            Razix::RZWorld* BuildRazixWorld(RZScene* scenes, RZSceneView* views)
            {
                return nullptr;
            }

            Razix::Graphics::Drawables BuildDrawables(RZScene* scene)
            {
                Drawables d;
                return d;
            }

            Razix::Graphics::Batches BuildBatches(Drawables* drawables)
            {
                Batches b;
                return b;
            }

            Razix::RZSceneView* ExtractSceneViews(RZScene* scene)
            {
                return nullptr;
            }

            Razix::Graphics::DrawData* ExtractBatchDrawData(Batch* batch)
            {
                return nullptr;
            }

            Razix::Graphics::DrawData* ExtractDrawableDrawData(Drawable* drawable)
            {
                return nullptr;
            }
        }    // namespace Diana
    }        // namespace Graphics
}    // namespace Razix