#pragma once

#include "Razix/Graphics/API/RZTexture.h"
#include "Razix/Graphics/API/RZVertexBuffer.h"
#include "Razix/Graphics/API/RZIndexBuffer.h"
#include "Razix/Graphics/API/RZShader.h"

#include <glm/glm.hpp>

namespace Razix {
    namespace Graphics {

        struct RZVeretx2D
        {
            glm::vec2 Position;
            glm::vec4 Color;
            glm::vec2 UV;
        };
        
        /// TODO: Use a orthographics projection matrix for aspect ratio

        /**
         * A Sprite is a 2D renderable that can be used to draw textures, particles effects, fonts and anything in 2D
         */
        class RAZIX_API RZSprite
        {
        public:
            /**
             * Creates an sprite with a given color
             * 
             * @param position The position of the sprite
             * @param rotation The rotation of the sprite
             * @param scale The dimensions of the sprite
             * @parma color The color of the sprite
             */
            RZSprite(const glm::vec2& position = glm::vec2(0.0f, 0.0f), const float& rotation = 0.0f, const glm::vec2& scale = glm::vec2(1.0f, 1.0f), const glm::vec4& color = glm::vec4(1.0f));
            /**
             * Crates a sprite with a given texture
             * 
             * @param texture The texture with which the sprite will drawn
             * @param position The position of the sprite
             * @param rotation The rotation of the sprite
             * @param scale The dimensions of the sprite
             */
            RZSprite(RZTexture2D* texture, const glm::vec2& position, const float& rotation, const glm::vec2& scale);
            virtual ~RZSprite() = default;

            void destroy();

            /* Gets the default UV values that is common for any sprite */
            static const std::array<glm::vec2, 4>& GetDefaultUVs();
            /* Gets the UV values given the minimum and maximum dimensions/range, useful while generating sprite sheet UVs */
            static const std::array<glm::vec2, 4>& RZSprite::GetUVs(const glm::vec2& min, const glm::vec2& max);

            void setSpriteSheet(RZTexture2D* texture, const glm::vec2& index, const glm::vec2& cellSize, const glm::vec2& spriteSize);
            
            RZTexture2D* getTexture() const { return m_Texture; }
            RAZIX_INLINE void setTexture(RZTexture2D* texture) { m_Texture = texture; }
            RAZIX_INLINE glm::vec2 getPosition() const { return m_Position; }
            void setPosition(const glm::vec2& vector2) { m_Position = vector2; };
            RAZIX_INLINE glm::vec2 getScale() const { return m_Scale; }
            void setScale(const glm::vec2& scale) { m_Scale = scale; }
            RAZIX_INLINE const glm::vec4& getColour() const { return m_Color; }
            void setColour(const glm::vec4& color);
            RAZIX_INLINE float getRotation() { return m_Rotation; }
            void setRotation(float rotation) { m_Rotation = rotation; }
            RAZIX_INLINE const std::array<glm::vec2, 4>& getUVs() const { return m_UVs; }

            // getter for shader, buffers and sets
            RZShader* getSimpleShader() { return m_SpriteShader; }
            RZVertexBuffer* getVertexBuffer() { return m_VBO; }
            RZIndexBuffer* getIndexBuffer() { return m_IBO; }
            RZDescriptorSet* getTexturedSpriteSet(uint32_t index) { return m_TexturedSpriteDescriptorSets[index]; }

        private:
            RZTexture2D*                    m_Texture;
            glm::vec2                       m_Position;// Do we really need them? Remove this, will be done via push_contants and updated via transform component
            float                           m_Rotation;// Do we really need them? Remove this, will be done via push_contants and updated via transform component
            glm::vec2                       m_Scale;   // Do we really need them? Remove this, will be done via push_contants and updated via transform component
            glm::vec4                       m_Color;
            std::array<glm::vec2, 4>        m_UVs;
            bool                            m_IsAnimated = false;
            bool                            m_IsTextured = false;

            RZShader*                       m_SpriteShader;
            RZShader*                       m_TexturedSpriteShader;
            RZShader*                       m_SpriteSheetShader;

            RZVertexBuffer*                 m_VBO;
            RZIndexBuffer*                  m_IBO;
            std::vector<RZDescriptorSet*>   m_SimpleSpriteDescriptorSets;
            std::vector<RZDescriptorSet*>   m_TexturedSpriteDescriptorSets;
            std::vector<RZDescriptorSet*>   m_SpriteSheetDescriptorSets;


            // TODO: Create the Buffers : VBO, IBO
            // TODO: Create descriptor sets
            // TODO: Create sets with texture
            // TODO: create shaders for sprite sheet to update the UVs using a uniform index buffer instead of updating the vertex buffer with the new UVs
            // TODO: create sets for sprite sheet which is same as textured ones
            // TODO: send it via push constants to update it and draw a animated sprite
            // TODO: Test normal push constants for movement

        private:
            void createBuffers();
            void updateDescriptorSets();
        };
    }
}
