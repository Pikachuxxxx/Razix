// clang-format off
#include "rzxpch.h"
// clang-format on
#include "SpriteRendererComponent.h"

#include "Razix/Graphics/RZSprite.h"

namespace Razix {

    SpriteRendererComponent::SpriteRendererComponent(Graphics::RZTextureHandle texture)
    {
        Sprite = new Graphics::RZSprite(texture);
    }

    SpriteRendererComponent::SpriteRendererComponent(glm::vec4 color)
    {
        Sprite = new Graphics::RZSprite(color);
    }
}    // namespace Razix
