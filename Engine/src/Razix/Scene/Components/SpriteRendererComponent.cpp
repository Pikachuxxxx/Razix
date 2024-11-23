// clang-format off
#include "rzxpch.h"
// clang-format on
#include "SpriteRendererComponent.h"

#include "Razix/Gfx/RZSprite.h"

namespace Razix {

    SpriteRendererComponent::SpriteRendererComponent(Gfx::RZTextureHandle texture)
    {
        Sprite = new Gfx::RZSprite(texture);
    }

    SpriteRendererComponent::SpriteRendererComponent(glm::vec4 color)
    {
        Sprite = new Gfx::RZSprite(color);
    }
}    // namespace Razix
