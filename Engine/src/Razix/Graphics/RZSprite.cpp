// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZSprite.h"

#include "razix/Core/RZApplication.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Razix {
    namespace Graphics {

        RZSprite::RZSprite(const glm::vec4& color /*= glm::vec4(1.0f)*/)
            : m_Color(color)
        {
            m_UVs = GetDefaultUVs();

            // Load the shaders before hand
            m_SpriteShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Sprite);    // Graphics::RZShader::Create("//RazixContent/Shaders/Razix/sprite.rzsf" RZ_DEBUG_NAME_TAG_STR_E_ARG("Sprite.rzsf"));

            // Create the vertex buffer and index buffer
            createBuffers();
            // Create the sets
            updateDescriptorSets();
        }

        RZSprite::RZSprite(RZTextureHandle texture)
            : m_Color(glm::vec4(1.0f))
        {
            m_Texture = texture;

            m_IsTextured           = true;
            m_UVs                  = GetDefaultUVs();
            m_TexturedSpriteShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::SpriteTextured);

            // Create the vertex buffer and index buffer
            createBuffers();
            // Create the sets
            updateDescriptorSets();
        }

        RZSprite::RZSprite()
            : m_Color(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))
        {
            m_UVs = GetDefaultUVs();

            // Load the shaders before hand
            m_SpriteShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::Sprite);

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

            RZResourceManager::Get().getPool<RZTexture>().release(m_Texture);

            if (m_IsTextured)
                RZResourceManager::Get().destroyShader(m_TexturedSpriteShader);
            else
                RZResourceManager::Get().destroyShader(m_SpriteShader);

            if (m_TexturedSpriteDescriptorSets.size() > 0 && m_IsTextured) {
                for (sz i = 0; i < 3; i++) {
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
#if 0
m_IsAnimated = true;
            //glm::vec2 min = { (index.x * cellSize.x) / m_Texture->getWidth(), (index.y * cellSize.y) / m_Texture->getHeight() };
            //glm::vec2 max = { ((index.x + spriteSize.x) * cellSize.x) / m_Texture->getWidth(), ((index.y + spriteSize.y) * cellSize.y) / m_Texture->getHeight() };

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            f32 x           = static_cast<f32>((int) (cellIndex.x - 1) % (int) sheetDimension.x);
            f32 y           = -(cellIndex.y - 1) / (int) sheetDimension.x;
            f32 frameWidth  = m_Texture->getWidth() / sheetDimension.x;
            f32 frameHeight = m_Texture->getHeight() / sheetDimension.y;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////

            //m_UVs = GetUVs(min, max);

            d32 x_range = (d32) RZApplication::Get().getWindow()->getWidth();
            d32 y_range = (d32) RZApplication::Get().getWindow()->getHeight();

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
#endif
        }

        RZShaderHandle RZSprite::getShader()
        {
            if (m_IsTextured)
                return m_TexturedSpriteShader;
            else
                return m_SpriteShader;
        }

        RZDescriptorSet* RZSprite::getDescriptorSet(u32 index)
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
                d32 x_range = (d32) RZApplication::Get().getWindow()->getWidth();
                d32 y_range = (d32) RZApplication::Get().getWindow()->getHeight();

                glm::mat4 view = glm::ortho(-x_range, +x_range, -y_range, y_range);

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

            u32 indices[6] = {
                0, 1, 2, 2, 3, 0};

            // Create the vertex buffer
            m_VBO = RZVertexBuffer::Create(sizeof(RZVeretx2D) * 4, vertices.data(), BufferUsage::Dynamic RZ_DEBUG_NAME_TAG_STR_E_ARG("Sprite"));
            RZVertexBufferLayout layout;
            layout.push<glm::vec4>("Position");
            layout.push<glm::vec4>("Color");
            layout.push<glm::vec2>("UV");
            m_VBO->AddBufferLayout(layout);

            // Create the index buffer
            m_IBO = RZIndexBuffer::Create(RZ_DEBUG_NAME_TAG_STR_F_ARG("Sprite") indices, 6, BufferUsage::Static);
        }

        void RZSprite::updateVertexData()
        {
            d32 x_range = (d32) RZApplication::Get().getWindow()->getWidth();
            d32 y_range = (d32) RZApplication::Get().getWindow()->getHeight();

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
                for (sz i = 0; i < 3; i++) {
                    m_TexturedSpriteDescriptorSets[i]->Destroy();
                }
            }
            //if (m_SpriteSheetDescriptorSets.size() > 0) {
            //    for (sz i = 0; i < 3; i++) {
            //        m_SpriteSheetDescriptorSets[i]->Destroy();
            //    }
            //}
            m_TexturedSpriteDescriptorSets.clear();
            //m_SpriteSheetDescriptorSets.clear();

            // Create the descriptor sets for normal sprite
            if (m_IsTextured) {
                auto setInfos = RZResourceManager::Get().getShaderResource(m_TexturedSpriteShader)->getDescriptorsPerHeapMap();

                for (sz i = 0; i < 3; i++) {
                    for (auto& setInfo: setInfos) {
                        // Fill the descriptors with buffers and textures
                        for (auto& descriptor: setInfo.second) {
                            if (descriptor.bindingInfo.type == Graphics::DescriptorType::ImageSamplerCombined)
                                descriptor.texture = m_Texture;
                        }
                        auto descSet = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG(__FUNCTION__));
                        m_TexturedSpriteDescriptorSets.push_back(descSet);
                    }
                }
            }
            //else if (m_IsAnimated) {
            //    auto setInfos = m_SpriteSheetShader->getSetInfos();
            //
            //    for (sz i = 0; i < 3; i++) {
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