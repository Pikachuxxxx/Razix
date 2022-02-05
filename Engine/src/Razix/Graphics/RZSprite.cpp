#include "rzxpch.h"
#include "RZSprite.h"

namespace Razix {
    namespace Graphics {

        RZSprite::RZSprite(const glm::vec2& position /*= glm::vec2(0.0f, 0.0f)*/, const float& rotation /*= 0.0f*/, const glm::vec2& scale /*= glm::vec2(1.0f, 1.0f)*/, const glm::vec4& color /*= glm::vec4(1.0f)*/)
            : m_Position(position), m_Rotation(rotation), m_Scale(scale), m_Color(color), m_Texture(nullptr)
        {
            m_UVs = GetDefaultUVs();

            // Load the shaders before hand
            m_SpriteShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/sprite.rzsf");

            // Create the vertex buffer and index buffer
            createBuffers();
            // Create the sets
            updateDescriptorSets();
        }

        RZSprite::RZSprite(RZTexture2D* texture, const glm::vec2& position, const float& rotation, const glm::vec2& scale)
            : m_Position(position), m_Rotation(rotation), m_Scale(scale), m_Color(glm::vec4(1.0f)), m_Texture(texture)
        {
            RAZIX_UNIMPLEMENTED_METHOD
            m_IsTextured = true;
            m_UVs = GetDefaultUVs();
            m_TexturedSpriteShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/textured_sprite.rzsf");
            
            // Create the vertex buffer and index buffer
            createBuffers();
            // Create the sets
            updateDescriptorSets();
        }

        void RZSprite::destroy()
        {
            // Destroy all the sets
            m_VBO->Destroy();
            m_IBO->Destroy();
            m_Texture->Release(true);

            m_SpriteShader->Destroy();
            //m_SpriteSheetShader->Destroy();
            //m_TexturedSpriteShader->Destroy();

            for (size_t i = 0; i < 3; i++) {
                //m_SimpleSpriteDescriptorSets[i]->Destroy();
                //m_SpriteSheetDescriptorSets[i]->Destroy();
                m_TexturedSpriteDescriptorSets[i]->Destroy();
            }
        }

        const std::array<glm::vec2, 4>& RZSprite::GetDefaultUVs()
        {
            // Starting from top left CW
            static std::array<glm::vec2, 4> results;
            {
                results[0] = glm::vec2(0, 1);
                results[1] = glm::vec2(1, 1);
                results[2] = glm::vec2(1, 0);
                results[3] = glm::vec2(0, 0);
            }
            return results;
        }

        const std::array<glm::vec2, 4>& RZSprite::GetUVs(const glm::vec2& min, const glm::vec2& max)
        {
            static std::array<glm::vec2, 4> results;
            {
                results[0] = glm::vec2(min.x, max.y);
                results[1] = max;
                results[2] = glm::vec2(max.x, min.y);
                results[3] = min;
            }
            return results;
        }

        void RZSprite::setSpriteSheet(RZTexture2D* texture, const glm::vec2& index, const glm::vec2& cellSize, const glm::vec2& spriteSize)
        {
            RAZIX_UNIMPLEMENTED_METHOD
            m_IsAnimated = true;
            m_Texture = texture;
            glm::vec2 min = { (index.x * cellSize.x) / texture->getWidth(), (index.y * cellSize.y) / texture->getHeight() };
            glm::vec2 max = { ((index.x + spriteSize.x) * cellSize.x) / texture->getWidth(), ((index.y + spriteSize.y) * cellSize.y) / texture->getHeight() };

            m_UVs = GetUVs(min, max);

            m_SpriteSheetShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/sprite_sheet.rzsf");

            // Update the sets with the sprite sheet
            updateDescriptorSets();

            // Update the vertex data
            static std::array<RZVeretx2D, 4> vertices;
            {
                vertices[0].Position = glm::vec2(m_Position.x - (m_Scale.x / 2), m_Position.y + (m_Scale.y / 2));  // v1 top left
                vertices[1].Position = glm::vec2(m_Position.x + (m_Scale.x / 2), m_Position.y + (m_Scale.y / 2));  // v2 top right
                vertices[2].Position = glm::vec2(m_Position.x + (m_Scale.x / 2), m_Position.y - (m_Scale.y / 2));  // v3 bottom right
                vertices[3].Position = glm::vec2(m_Position.x - (m_Scale.x / 2), m_Position.y - (m_Scale.y / 2));  // v4 bottom left

                vertices[0].Color = m_Color;
                vertices[1].Color = m_Color;
                vertices[2].Color = m_Color;
                vertices[3].Color = m_Color;

                vertices[0].UV = m_UVs[0];
                vertices[1].UV = m_UVs[1];
                vertices[2].UV = m_UVs[2];
                vertices[3].UV = m_UVs[3];
            }

            m_VBO->SetSubData(sizeof(RZVeretx2D) * 4, vertices.data(), 0);
        }

        void RZSprite::setColour(const glm::vec4& color)
        {
            m_Color = color;

            // Update the vertex data
            static std::array<RZVeretx2D, 4> vertices;
            {
                vertices[0].Position = glm::vec2(m_Position.x - (m_Scale.x / 2), m_Position.y + (m_Scale.y / 2));  // v1 top left
                vertices[1].Position = glm::vec2(m_Position.x + (m_Scale.x / 2), m_Position.y + (m_Scale.y / 2));  // v2 top right
                vertices[2].Position = glm::vec2(m_Position.x + (m_Scale.x / 2), m_Position.y - (m_Scale.y / 2));  // v3 bottom right
                vertices[3].Position = glm::vec2(m_Position.x - (m_Scale.x / 2), m_Position.y - (m_Scale.y / 2));  // v4 bottom left

                vertices[0].Color = m_Color;
                vertices[1].Color = m_Color;
                vertices[2].Color = m_Color;
                vertices[3].Color = m_Color;

                vertices[0].UV = m_UVs[0];
                vertices[1].UV = m_UVs[1];
                vertices[2].UV = m_UVs[2];
                vertices[3].UV = m_UVs[3];
            }

            m_VBO->SetSubData(sizeof(RZVeretx2D) * 4, vertices.data(), 0);
        }

        void RZSprite::createBuffers()
        {
            // All the sprites are NDC full screen quads, pos rot and scale is defined by the push_constants
            // The reason we pass them to constructor is for de-serialization not to manipulate the vert and index data, it's same for all sprites

             static std::array<RZVeretx2D, 4> vertices;
             {
                 vertices[0].Position = glm::vec2(m_Position.x - (m_Scale.x / 2), m_Position.y + (m_Scale.y / 2));  // v1 top left
                 vertices[1].Position = glm::vec2(m_Position.x + (m_Scale.x / 2), m_Position.y + (m_Scale.y / 2));  // v2 top right
                 vertices[2].Position = glm::vec2(m_Position.x + (m_Scale.x / 2), m_Position.y - (m_Scale.y / 2));  // v3 bottom right
                 vertices[3].Position = glm::vec2(m_Position.x - (m_Scale.x / 2), m_Position.y - (m_Scale.y / 2));  // v4 bottom left

                 vertices[0].Color = m_Color;
                 vertices[1].Color = m_Color;
                 vertices[2].Color = m_Color;
                 vertices[3].Color = m_Color;

                 vertices[0].UV = m_UVs[0];
                 vertices[1].UV = m_UVs[1];
                 vertices[2].UV = m_UVs[2];
                 vertices[3].UV = m_UVs[3];
             }

             uint32_t indices[6] = {
                 0, 1, 2, 2, 3, 0
             };

             // Create the vertex buffer
             m_VBO = RZVertexBuffer::Create(sizeof(RZVeretx2D) * 4, vertices.data(), BufferUsage::DYNAMIC);
             RZVertexBufferLayout layout;
             layout.push<glm::vec2>("Position");
             layout.push<glm::vec4>("Color");
             layout.push<glm::vec2>("UV");
             m_VBO->AddBufferLayout(layout);

             // Create the index buffer
             m_IBO = RZIndexBuffer::Create(indices, 6, BufferUsage::STATIC);
        }

        void RZSprite::updateDescriptorSets()
        {
            if (m_TexturedSpriteDescriptorSets.size() > 0 && m_SpriteSheetDescriptorSets.size() > 0) {
                for (size_t i = 0; i < 3; i++) {
                    m_TexturedSpriteDescriptorSets[i]->Destroy();
                    m_SpriteSheetDescriptorSets[i]->Destroy();
                }
            }
            m_TexturedSpriteDescriptorSets.clear();
            m_SpriteSheetDescriptorSets.clear();
  
            // Create the descriptor sets for normal sprite
            if (m_IsTextured) {
                auto setInfos = m_TexturedSpriteShader->getSetInfos();

                for (size_t i = 0; i < 3; i++) {
                    for (auto& setInfo : setInfos) {
                        // Fill the descriptors with buffers and textures
                        for (auto& descriptor : setInfo.descriptors) {
                            if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
                                descriptor.texture = m_Texture;
                        }
                        auto descSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
                        m_TexturedSpriteDescriptorSets.push_back(descSet);
                    }
                }
            }
            else if (m_IsAnimated) {
                auto setInfos = m_SpriteSheetShader->getSetInfos();

                for (size_t i = 0; i < 3; i++) {
                    for (auto& setInfo : setInfos) {
                        // Fill the descriptors with buffers and textures
                        for (auto& descriptor : setInfo.descriptors) {
                            if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
                                descriptor.texture = m_Texture;
                        }
                        auto descSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
                        m_SpriteSheetDescriptorSets.push_back(descSet);
                    }
                }
            }
        }
    }
}