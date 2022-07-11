// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSprite.h"

#include "razix/Core/RZApplication.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Razix {
    namespace Graphics {

        RZSprite::RZSprite(const glm::vec4& color /*= glm::vec4(1.0f)*/)
            : m_Color(color), m_Texture(nullptr)
        {
            m_UVs = GetDefaultUVs();

            // Load the shaders before hand
            m_SpriteShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/sprite.rzsf");

            // Create the vertex buffer and index buffer
            createBuffers();
            // Create the sets
            updateDescriptorSets();
        }

        RZSprite::RZSprite(RZTexture2D* texture)
            : m_Color(glm::vec4(1.0f)), m_Texture(texture)
        {
            m_IsTextured           = true;
            m_UVs                  = GetDefaultUVs();
            m_TexturedSpriteShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/sprite_textured.rzsf");

            // Create the vertex buffer and index buffer
            createBuffers();
            // Create the sets
            updateDescriptorSets();
        }

        RZSprite::RZSprite()
            : m_Color(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)), m_Texture(nullptr)
        {
            m_UVs = GetDefaultUVs();

            // Load the shaders before hand
            m_SpriteShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/sprite.rzsf");

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
            if (m_Texture != nullptr)
                m_Texture->Release(true);

            if (m_IsTextured)
                m_TexturedSpriteShader->Destroy();
            else
                m_SpriteShader->Destroy();

            if (m_TexturedSpriteDescriptorSets.size() > 0 && m_IsTextured) {
                for (size_t i = 0; i < 3; i++) {
                    //m_SimpleSpriteDescriptorSets[i]->Destroy();
                    //m_SpriteSheetDescriptorSets[i]->Destroy();
                    m_TexturedSpriteDescriptorSets[i]->Destroy();
                }
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

        void RZSprite::setSpriteSheet(const glm::vec2& cellIndex, const glm::vec2& sheetDimension)
        {
            m_IsAnimated = true;
            //glm::vec2 min = { (index.x * cellSize.x) / m_Texture->getWidth(), (index.y * cellSize.y) / m_Texture->getHeight() };
            //glm::vec2 max = { ((index.x + spriteSize.x) * cellSize.x) / m_Texture->getWidth(), ((index.y + spriteSize.y) * cellSize.y) / m_Texture->getHeight() };

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            float x           = (int) (cellIndex.x - 1) % (int) sheetDimension.x;
            float y           = -(cellIndex.y - 1) / (int) sheetDimension.x;
            float frameWidth  = m_Texture->getWidth() / sheetDimension.x;
            float frameHeight = m_Texture->getHeight() / sheetDimension.y;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////

            //m_UVs = GetUVs(min, max);

            double x_range = (double) RZApplication::Get().getWindow()->getWidth();
            double y_range = (double) RZApplication::Get().getWindow()->getHeight();

            glm::mat4 view = glm::ortho(-x_range, +x_range, -y_range, y_range);

            // Update the vertex data
            static std::array<RZVeretx2D, 4> vertices;
            {
                vertices[0].Position = view * glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);    // v1 top left
                vertices[1].Position = view * glm::vec4(1.0f, -1.0f, 0.0f, 1.0f);     // v2 top right
                vertices[2].Position = view * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);      // v3 bottom right
                vertices[3].Position = view * glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);     // v4 bottom left

                vertices[0].Color = m_Color;
                vertices[1].Color = m_Color;
                vertices[2].Color = m_Color;
                vertices[3].Color = m_Color;

                vertices[0].UV = glm::vec2((x * frameWidth) / m_Texture->getWidth(), (y * frameHeight) / m_Texture->getHeight());
                vertices[1].UV = glm::vec2(((x + 1) * frameWidth) / m_Texture->getWidth(), (y * frameHeight) / m_Texture->getHeight());
                vertices[2].UV = glm::vec2(((x + 1) * frameWidth) / m_Texture->getWidth(), ((y + 1) * frameHeight) / m_Texture->getHeight());
                vertices[3].UV = glm::vec2((x * frameWidth) / m_Texture->getWidth(), ((y + 1) * frameHeight) / m_Texture->getHeight());
            }
            m_VBO->SetData(sizeof(RZVeretx2D) * 4, vertices.data());
        }

        RZShader* RZSprite::getShader()
        {
            if (m_IsTextured)
                return m_TexturedSpriteShader;
            else
                return m_SpriteShader;
        }

        RZDescriptorSet* RZSprite::getDescriptorSet(uint32_t index)
        {
            if (m_IsTextured)
                return m_TexturedSpriteDescriptorSets[index];
            else
                return nullptr;
        }

        void RZSprite::createBuffers()
        {
            // All the sprites are NDC full screen quads, pos rot and scale is defined by the push_constants
            // The reason we pass them to constructor is for de-serialization not to manipulate the vert and index data, it's same for all sprites

            static std::array<RZVeretx2D, 4> vertices;
            {
                double x_range = (double) RZApplication::Get().getWindow()->getWidth();
                double y_range = (double) RZApplication::Get().getWindow()->getHeight();

                glm::mat4 view = glm::ortho(-x_range, +x_range, -y_range, y_range);
                std::cout << "Ortho matrix : " << glm::to_string(view) << std::endl;

                vertices[0].Position = view * glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);    // v1 top left
                vertices[1].Position = view * glm::vec4(1.0f, -1.0f, 0.0f, 1.0f);     // v2 top right
                vertices[2].Position = view * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);      // v3 bottom right
                vertices[3].Position = view * glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);     // v4 bottom left

                vertices[0].Color = m_Color;
                vertices[1].Color = m_Color;
                vertices[2].Color = m_Color;
                vertices[3].Color = m_Color;

                vertices[0].UV = m_UVs[0];
                vertices[1].UV = m_UVs[1];
                vertices[2].UV = m_UVs[2];
                vertices[3].UV = m_UVs[3];
            }

            uint16_t indices[6] = {
                0, 1, 2, 2, 3, 0};

            // Create the vertex buffer
            m_VBO = RZVertexBuffer::Create(sizeof(RZVeretx2D) * 4, vertices.data(), BufferUsage::DYNAMIC, "Sprite");
            RZVertexBufferLayout layout;
            layout.push<glm::vec4>("Position");
            layout.push<glm::vec4>("Color");
            layout.push<glm::vec2>("UV");
            m_VBO->AddBufferLayout(layout);

            // Create the index buffer
            m_IBO = RZIndexBuffer::Create(indices, 6, "Sprite", BufferUsage::STATIC);
        }

        void RZSprite::updateVertexData()
        {
            double x_range = (double) RZApplication::Get().getWindow()->getWidth();
            double y_range = (double) RZApplication::Get().getWindow()->getHeight();

            glm::mat4 view = glm::ortho(-x_range, +x_range, -y_range, y_range);

            static std::array<RZVeretx2D, 4> vertices;
            {
                vertices[0].Position = view * glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);    // v1 top left
                vertices[1].Position = view * glm::vec4(1.0f, -1.0f, 0.0f, 1.0f);     // v2 top right
                vertices[2].Position = view * glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);      // v3 bottom right
                vertices[3].Position = view * glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);     // v4 bottom left

                vertices[0].Color = m_Color;
                vertices[1].Color = m_Color;
                vertices[2].Color = m_Color;
                vertices[3].Color = m_Color;

                vertices[0].UV = m_UVs[0];
                vertices[1].UV = m_UVs[1];
                vertices[2].UV = m_UVs[2];
                vertices[3].UV = m_UVs[3];
            }

            m_VBO->SetData(sizeof(RZVeretx2D) * 4, vertices.data());
        }

        void RZSprite::updateDescriptorSets()
        {
            // TODO: Delete them only if they have been allocated not in a batched way as below
            if (m_TexturedSpriteDescriptorSets.size() > 0) {
                for (size_t i = 0; i < 3; i++) {
                    m_TexturedSpriteDescriptorSets[i]->Destroy();
                }
            }
            //if (m_SpriteSheetDescriptorSets.size() > 0) {
            //    for (size_t i = 0; i < 3; i++) {
            //        m_SpriteSheetDescriptorSets[i]->Destroy();
            //    }
            //}
            m_TexturedSpriteDescriptorSets.clear();
            //m_SpriteSheetDescriptorSets.clear();

            // Create the descriptor sets for normal sprite
            if (m_IsTextured) {
                auto setInfos = m_TexturedSpriteShader->getSetsCreateInfos();

                for (size_t i = 0; i < 3; i++) {
                    for (auto& setInfo: setInfos) {
                        // Fill the descriptors with buffers and textures
                        for (auto& descriptor: setInfo.second) {
                            if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
                                descriptor.texture = m_Texture;
                        }
                        auto descSet = Graphics::RZDescriptorSet::Create(setInfo.second);
                        m_TexturedSpriteDescriptorSets.push_back(descSet);
                    }
                }
            }
            //else if (m_IsAnimated) {
            //    auto setInfos = m_SpriteSheetShader->getSetInfos();
            //
            //    for (size_t i = 0; i < 3; i++) {
            //        for (auto& setInfo : setInfos) {
            //            // Fill the descriptors with buffers and textures
            //            for (auto& descriptor : setInfo.descriptors) {
            //                if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER)
            //                    descriptor.texture = m_Texture;
            //            }
            //            auto descSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
            //            m_SpriteSheetDescriptorSets.push_back(descSet);
            //        }
            //    }
            //}
        }
    }    // namespace Graphics
}    // namespace Razix