#pragma once

#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include <glm/glm.hpp>

namespace Razix {
    namespace Graphics {

        struct RZVeretx2D
        {
            glm::vec4 Position;
            glm::vec4 Color;
            glm::vec2 UV;
        };

        /// We use a orthographic projection matrix for aspect ratio, this will be updated by the Renderer2D when resized, the layout(set = 0, binding = 0) will be shared by all the sprites in the engine to draw this

        /**
         * A Sprite is a 2D renderable that can be used to draw textures, particles effects, fonts and anything in 2D
         */
        // TODO: Refactor the sprite class to use the same default VBO, IBO, Shaders and Vertex Data use some static methods to use that cause Transform will be used by the Renderer by them
        class RAZIX_API RZSprite : public RZRoot
        {
        public:
            RZSprite();
            /**
             * Creates an sprite with a given color
             * 
             * @param position The position of the sprite
             * @param rotation The rotation of the sprite
             * @param scale The dimensions of the sprite
             * @parma color The color of the sprite
             */
            RZSprite(const glm::vec4& color);
            /**
             * Crates a sprite with a given texture
             * 
             * @param texture The texture with which the sprite will drawn
             * @param position The position of the sprite
             * @param rotation The rotation of the sprite
             * @param scale The dimensions of the sprite
             */
            RZSprite(RZTexture2D* texture);

            virtual ~RZSprite() = default;

            void destroy();

            /* Gets the default UV values that is common for any sprite */
            static const std::array<glm::vec2, 4>& GetDefaultUVs();
            /* Gets the UV values given the minimum and maximum dimensions/range, useful while generating sprite sheet UVs */
            static const std::array<glm::vec2, 4>& GetUVs(const glm::vec2& min, const glm::vec2& max);

            void setSpriteSheet(const glm::vec2& cellIndex, const glm::vec2& sheetDimension);

            RZTexture2D*      getTexture() const { return m_Texture; }
            RAZIX_INLINE void setTexture(RZTexture2D* texture) { m_Texture = texture; }

            const glm::vec4&  getColour() const { return m_Color; }
            RAZIX_INLINE void setColour(const glm::vec4& color)
            {
                m_Color = color;
                updateVertexData();
            }

            RAZIX_INLINE const std::array<glm::vec2, 4>& getUVs() const { return m_UVs; }

            // getter for shader, buffers and sets
            RZVertexBuffer* getVertexBuffer() { return m_VBO; }
            RZIndexBuffer*  getIndexBuffer() { return m_IBO; }

            RZShader*        getShader();
            RZDescriptorSet* getDescriptorSet(uint32_t index);

        private:
            glm::vec4                     m_Color;
            RZTexture2D*                  m_Texture;
            std::array<glm::vec2, 4>      m_UVs;
            bool                          m_IsAnimated = false;
            bool                          m_IsTextured = false;
            RZShader*                     m_SpriteShader;
            RZShader*                     m_TexturedSpriteShader;
            RZVertexBuffer*               m_VBO;
            RZIndexBuffer*                m_IBO;
            std::vector<RZDescriptorSet*> m_TexturedSpriteDescriptorSets;

        private:
            void createBuffers();
            void updateVertexData();
            void updateDescriptorSets();
        };
    }    // namespace Graphics
}    // namespace Razix
