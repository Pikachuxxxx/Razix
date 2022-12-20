#pragma once

#include "Razix/Core/RZLog.h"

#include "Razix/Graphics/API/RZDescriptorSet.h"

namespace Razix {
    namespace Graphics {

        /**
         * A class that manages Textures/Image resources for the Engine
         * @brief It manages creation and conversion of Image resources, also stores in a custom Engine Format depending on how it's being used
         */
        // TODO: Derive Texture from a RazixResource/Asset class this way it gets a resource/asset UUID + serialization by default
        // TODO: Calculate size properly for manually set texture data
        // TODO: Add support and Utility functions for sRGB textures
        class RAZIX_API RZTexture : public RZRoot
        {
            // Texture internal Types
        public:
            /* The type of the texture */
            enum class Type
            {
                COLOR_1D = 0,
                COLOR_2D,
                COLOR_3D,
                COLOR_RT,
                DEPTH,
                STENCIL,
                CUBE
            };

            /* The format of the Texture resource */
            enum class Format
            {
                R8,
                R32_INT,
                R32_UINT,
                R32F,
                RG8,
                RGB8,
                RGBA8,
                RGB16,
                RGBA16,
                RGB32,
                RGBA32,
                RGBA32F,
                RGB,
                RGBA,
                DEPTH,
                STENCIL,
                DEPTH_STENCIL,
                SCREEN,
                BGRA8_UNORM
            };

            /* Wrap mode for the texture texels */
            enum class Wrapping
            {
                REPEAT,
                MIRRORED_REPEAT,
                CLAMP_TO_EDGE,
                CLAMP_TO_BORDER
            };

            /* Filtering for the Texture */
            struct Filtering
            {
                enum class FilterMode
                {
                    LINEAR,
                    NEAREST
                };
                FilterMode minFilter = FilterMode::LINEAR;
                FilterMode magFilter = FilterMode::LINEAR;

                Filtering() {}
                Filtering(FilterMode min, FilterMode max)
                    : minFilter(min), magFilter(max) {}
            };

        public:
            /* Default constructor, texture resource is done on demand */
            RZTexture() = default;
            /* Virtual destructor enables the API implementation to delete it's resources */
            virtual ~RZTexture() {}

            /**
             * Calculates the Mip Map count based on the Width and Height of the texture
             *
             * @param width     The width of the Texture
             * @param height    The height of the texture
             */
            static uint32_t calculateMipMapCount(uint32_t width, uint32_t height);
            static Format   bitsToTextureFormat(uint32_t bits);

            // TODO: Move this to the RXBaseAsset class in later designs
            /* Releases the texture data and it's underlying resources */
            virtual void Release(bool deleteImage = true) = 0;

            /* Returns the name of the texture resource */
            std::string getName() const { return m_Name; }
            /* returns the width of the texture */
            uint32_t getWidth() const { return m_Width; }
            /* returns the height of the texture */
            uint32_t getHeight() const { return m_Height; }
            /* Gets the size of the texture resource */
            uint64_t getSize() const { return m_Size; }
            /* Returns the type of the texture */
            Type getType() const { return m_TextureType; }
            /* Returns the internal format of the texture */
            Format getFormat() const { return m_Format; }
            /* Returns the virtual path of the texture resource */
            std::string getPath() const { return m_VirtualPath; }

            /* Binds the Texture resource to the Pipeline */
            virtual void Bind(uint32_t slot) = 0;
            /* Unbinds the Texture resource from the pipeline */
            virtual void Unbind(uint32_t slot) = 0;

            /* Gets the handle to the underlying API texture instance */
            virtual void* GetHandle() const = 0;

            /* Generates the descriptor set for the texture */
            void             generateDescriptorSet();
            RZDescriptorSet* getDescriptorSet() { return m_DescriptorSet; }

        protected:
            std::string      m_Name;          /* The name of the texture resource         */
            std::string      m_VirtualPath;   /* The virtual path of the texture          */
            uint32_t         m_Width;         /* The width of the texture                 */
            uint32_t         m_Height;        /* The height of the texture                */
            uint64_t         m_Size;          /* The size of the texture resource         */
            Type             m_TextureType;   /* The type of this texture                 */
            Format           m_Format;        /* The internal format of the texture data  */
            Wrapping         m_WrapMode;      /* Wrap mode of the texture                 */
            Filtering        m_FilterMode;    /* Filtering mode of the texture data       */
            bool             m_FlipX;         /* Flip the texture on X-axis during load   */
            bool             m_FlipY;         /* Flip the texture on Y-axis during load   */
            RZDescriptorSet* m_DescriptorSet; /* Descriptor set for the image             */
        };

        //-----------------------------------------------------------------------------------
        // Texture 2D
        //-----------------------------------------------------------------------------------

        /* 2D Texture interface */
        class RAZIX_API RZTexture2D : public RZTexture
        {
        public:
            /**
             * Create a 2D texture resource from the given data
             * 
             * @param name          The name of the texture resource 
             * @param width         The Width of the texture
             * @param height        The height of the texture
             * @param data          The pixel data to fill the texture with
             * @param format        The internal format of the texture
             * @param wrapMode      The wrapping mode of the texture
             * @param filterMode    The filtering mode of the texture
             * @returns Pointer to Texture object of the underlying API
             */
            static RZTexture2D* Create(RZ_DEBUG_NAME_TAG_F_ARG const std::string& name, uint32_t width, uint32_t height, void* data, Format format, Wrapping wrapMode = RZTexture::Wrapping::CLAMP_TO_EDGE, Filtering filterMode = Filtering{});
            /**
             * Create a 2D texture resource from the given file
             * 
             * @param filePath      The filepath to load the texture resource from
             * @param name          The name of the texture resource 
             * @param format        The internal format of the texture
             * @param wrapMode      The wrapping mode of the texture
             * @param filterMode    The filtering mode of the texture
             * @returns Pointer to Texture object of the underlying API
             */
            static RZTexture2D* CreateFromFile(
                RZ_DEBUG_NAME_TAG_F_ARG const std::string& filePath, const std::string& name, Wrapping wrapMode, Filtering filterMode = Filtering{});

            /**
             * Sets the pixel data for the 2D Texture 
             * 
             * @param pixels The pixel data to set
             */
            virtual void SetData(const void* pixels) = 0;
        };

        //-----------------------------------------------------------------------------------
        // Depth Texture
        //-----------------------------------------------------------------------------------

        class RAZIX_API RZDepthTexture : public RZTexture
        {
        public:
            static RZDepthTexture* Create(uint32_t width, uint32_t height);

            virtual void Resize(uint32_t width, uint32_t height) = 0;
        };

        //-----------------------------------------------------------------------------------
        // Render Texture
        //-----------------------------------------------------------------------------------

        class RAZIX_API RZRenderTexture : public RZTexture
        {
        public:
            static RZRenderTexture* Create(RZ_DEBUG_NAME_TAG_F_ARG
                                               uint32_t width,
                uint32_t height, Format format = RZTexture::Format::SCREEN, Wrapping wrapMode = RZTexture::Wrapping::REPEAT, Filtering filterMode = Filtering{});

            virtual void Resize(uint32_t width, uint32_t height RZ_DEBUG_NAME_TAG_E_ARG) = 0;

            virtual int32_t ReadPixels(uint32_t x, uint32_t y) = 0;
        };
    }    // namespace Graphics
}    // namespace Razix