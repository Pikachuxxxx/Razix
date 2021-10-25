#pragma once

#include "Razix/Core/Log.h"

namespace Razix {
    namespace Graphics {

        /**
         * A class that manages Textures/Image resources for the Engine
         * @brief It manages creation and conversion of Image resources, also stores in a custom Engine Format depending on how it's being used
         */
        // TODO: Derive Texture from a RazixResource/Asset class this way it gets a resource/asset UUID + serialization by default
        class Texture
        {
            // Texture Types
        public:
            /* The type of the texture */
            enum class Type
            {
                COLOR, DEPTH, CUBE
            };

            /* The format of the Texture resource */
            enum class Format
            {
                R8, R16, R32, RG8, RG16, RG32, RGB8, RGB16, RGB32, RGBA8, RGBA16, RGBA32,
                R8F, R16F, R32F, RG8F, RG16F, RG32F, RGB8F, RGB16F, RGB32F, RGBA8F, RGBA16F, RGBA32F
            };

            /* Wrap mode for the texture texels */
            enum class Wrapping
            {
                NONE, REPEAT, CLAMP, MIRRORED_REPEAT,
                CLAMP_TO_EDGE, CLAMP_TO_BORDER
            };

            /* Filtering for the Texture */
            enum class Filtering
            {
                NONE, LINEAR, NEAREST
            };

        public:
            /* Default constructor, texture resource is done on demand */
            Texture() = default;
            /* Virtual destructor enables the API implementation to delete it's resources */
            virtual ~Texture() {}

            /* Create a 2D texture resource from the given data */
            static Texture* Create2D(const std::string& name, uint32_t width, uint32_t height, void* data, Format format, Wrapping wrapMode, Filtering filterMode);
            static Texture* Create2DFromFile(const std::string& filePath, const std::string& name, Format format, Wrapping wrapMode, Filtering filterMode);

            /**
             * Calculates the Mip Map count based on the Width and Height of the texture
             *
             * @param width     The width of the Texture
             * @param height    The height of the texture
             */
            uint32_t calculateMipMapCount(uint32_t width, uint32_t height);

            std::string getName() const { return m_Name; }
            uint32_t getWidth() const { return m_Width; }
            uint32_t getHeight() const { return m_Height; }

        protected:
            virtual void Bind() = 0;
            virtual void Unbind() = 0;

            virtual void* GetHandle() const = 0;

        private:
            std::string     m_Name;             /* The name of the texture resource         */
            std::string     m_VirtualPath;      /* The virtual path of the texture          */
            uint32_t        m_Width;            /* The width of the texture                 */
            uint32_t        m_Height;           /* The height of the texture                */
            Type            m_TextureType;      /* The type of this texture                 */
            Format          m_InternalFormat;   /* The internal format of the texture data  */
            Wrapping        m_WrapMode;         /* Wrap mode of the texture                 */
            Filtering       m_FilterMode;       /* Filtering mode of the texture data       */
            bool            m_FlipX;            /* Flip the texture on X-axis during load   */
            bool            m_FlipY;            /* Flip the texture on Y-axis during load   */
        };
    }
}