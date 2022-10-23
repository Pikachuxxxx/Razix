#pragma once

// TODO: This should be removed!!!!!
#include <glm/glm.hpp>

#include "Razix/Graphics/API/RZTexture.h"
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
        SpriteRendererComponent(Graphics::RZTexture2D* texture);
        SpriteRendererComponent(const SpriteRendererComponent&) = default;

       template<class Archive>
        void load(Archive& archive)
        {
            //Sprite = new Graphics::RZSprite;
            std::string texturePath;
            archive(cereal::make_nvp("TexturePath", texturePath));
            if (!texturePath.empty()) {
                Graphics::RZTexture2D* texture = Graphics::RZTexture2D::CreateFromFile(texturePath, texturePath, "sprite", Graphics::RZTexture2D::Wrapping::CLAMP_TO_EDGE);
                Sprite                         = new Graphics::RZSprite(texture);
            } else {
                glm::vec4 color;
                archive(cereal::make_nvp("Color", color));
                Sprite = new Graphics::RZSprite(color);
            }
        }

        template<class Archive>
        void save(Archive& archive) const

        {
            if (Sprite->getTexture() != nullptr)
                archive(cereal::make_nvp("TexturePath", Sprite->getTexture()->getPath()));
            archive(cereal::make_nvp("Color", Sprite->getColour()));
        }
    };

}    // namespace Razix