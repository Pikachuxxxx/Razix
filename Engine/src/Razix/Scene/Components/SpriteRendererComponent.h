#pragma once

#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RZSprite.h"

namespace Razix {

    namespace Graphics {
        class RZSprite;
    }

    /**
     * Renders a sprite in the Screen space, currently can be rendered only as a 2D entity within the view
     */
    struct RAZIX_API SpriteRendererComponent
    {
        Graphics::RZSprite* Sprite = nullptr;

        SpriteRendererComponent() {}
        SpriteRendererComponent(glm::vec4 color);
        SpriteRendererComponent(Graphics::RZTextureHandle texture);
        SpriteRendererComponent(const SpriteRendererComponent&) = default;

        template<class Archive>
        void load(Archive& archive)
        {
            //Sprite = new Graphics::RZSprite;
            std::string texturePath;
            archive(cereal::make_nvp("TexturePath", texturePath));
            if (!texturePath.empty()) {
                //Graphics::RZTextureHandle handle = RZResourceManager::Get().createTextureFromFile({.name = "sprite", .wrapping = Graphics::RZTextureProperties::Wrapping::CLAMP_TO_BORDER}, texturePath);
                //Sprite                           = new Graphics::RZSprite(handle);
            } else {
                glm::vec4 color;
                archive(cereal::make_nvp("Color", color));
                Sprite = new Graphics::RZSprite(color);
            }
        }

        template<class Archive>
        void save(Archive& archive) const

        {
            if (Sprite->getTexture().isValid())
                archive(cereal::make_nvp("TexturePath", "Sprite->getTexture()->getPath()"));
            archive(cereal::make_nvp("Color", Sprite->getColour()));
        }
    };

}    // namespace Razix