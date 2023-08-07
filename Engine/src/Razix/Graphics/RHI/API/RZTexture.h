#pragma once

#include "Razix/Core/RZLog.h"

#include "Razix/Graphics/Resources/IRZResource.h"

#include "Razix/Graphics/RHI/API/Data/RZTextureData.h"
#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"

#include "Razix/Graphics/RHI/API/RZAPICreateStructs.h"

#define GET_INSTANCE_SIZE static u32 GetInstanceSize()

namespace Razix {
    namespace Graphics {

        // Forward Declaration
        struct RZTextureDesc;

        /**
         * A class that manages Textures/Image resources for the Engine
         * @brief It manages creation and conversion of Image resources, also stores in a custom Engine Format depending on how it's being used
         */
        // TODO!!!: All will be under RZTexture and will use the RZTextureProperties::Type struct to identify the type of the Texture
        // We will use a common set of functions under RZTexture to create various textures and of course the API ones can still be split
        // into different classes derived from RZTexture
        // TODO: Calculate size properly for manually set texture data
        // TODO: Add support and Utility functions for sRGB textures
        // TODO: Hide CreateXXX Functions and Replace all pointers with Handles!!!
        class RAZIX_API RZTexture : public IRZResource<RZTexture>
        {
        public:
            /* Default constructor, texture resource is done on demand */
            RZTexture() {}
            /* Virtual destructor enables the API implementation to delete it's resources */
            virtual ~RZTexture() {}

            RAZIX_NONCOPYABLE_CLASS(RZTexture)

            GET_INSTANCE_SIZE;

            /**
             * Calculates the Mip Map count based on the Width and Height of the texture
             *
             * @param width     The width of the Texture
             * @param height    The height of the texture
             */
            static u32                         calculateMipMapCount(u32 width, u32 height);
            static RZTextureProperties::Format bitsToTextureFormat(u32 bits);

            /* Binds the Texture resource to the Pipeline */
            virtual void Bind(u32 slot) = 0;
            /* Unbinds the Texture resource from the pipeline */
            virtual void Unbind(u32 slot) = 0;

            /* Resize the texture */
            virtual void Resize(u32 width, u32 height) {}

            /* Gets the handle to the underlying API texture instance */
            virtual void* GetAPIHandlePtr() const = 0;

            virtual void SetData(const void* pixels) {}

            virtual int32_t ReadPixels(u32 x, u32 y) = 0;

            virtual void GenerateMips(){}

            const RZTextureDesc& getDescription() const { return m_Desc; }

            /* Returns the name of the texture resource */
            std::string getName() const { return m_Desc.name; }
            /* returns the width of the texture */
            u32 getWidth() const { return m_Desc.width; }
            /* returns the height of the texture */
            u32 getHeight() const { return m_Desc.height; }
            /* Gets the size of the texture resource */
            uint64_t getSize() const { return m_Size; }
            /* Returns the type of the texture */
            RZTextureProperties::Type getType() const { return m_Desc.type; }
            void                      setType(RZTextureProperties::Type type) { m_Desc.type = type; }
            /* Returns the internal format of the texture */
            RZTextureProperties::Format getFormat() const { return m_Desc.format; }
            /* Returns the virtual path of the texture resource */
            std::string                    getPath() const { return m_VirtualPath; }
            RZTextureProperties::Filtering getFilterMode() { return m_Desc.filtering; }
            RZTextureProperties::Wrapping  getWrapMode() { return m_Desc.wrapping; }

            /* Generates the descriptor set for the texture */
            void             generateDescriptorSet();
            RZDescriptorSet* getDescriptorSet() { return m_DescriptorSet; }

            void setMipLevel(u32 idx) { m_CurrentMipRenderingLevel = idx; }

            RAZIX_INLINE bool isRT() const { return m_IsRenderTexture; }

        protected:
            std::string      m_VirtualPath;                      /* The virtual path of the texture                             */
            uint64_t         m_Size;                             /* The size of the texture resource                            */
            RZDescriptorSet* m_DescriptorSet;                    /* Descriptor set for the image                                */
            RZTextureDesc    m_Desc;                             /* Texture properties and create desc                          */
            u32              m_TotalMipLevels           = 1;     /* Total Mips, Calculated by a formula except for RZCubeMap    */
            u32              m_CurrentMipRenderingLevel = 0;     /* Current mip level to which we are rendering to (as RT)      */
            bool             m_IsRenderTexture          = true;  /* Any texture not imported from file and created is a RT      */

        private:
            static void Create(void* where, const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            static void CreateFromFile(void* where, const RZTextureDesc& desc, const std::string& filePath RZ_DEBUG_NAME_TAG_E_ARG);

            friend class RZResourceManager;
        };

#if 0
//-----------------------------------------------------------------------------------
        // Texture 2D
        //-----------------------------------------------------------------------------------

        /* 2D Texture interface */
        class RAZIX_API RZTexture2D : public RZTexture, public IRZResource<RZTexture2D>
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
            static RZTexture2D* Create(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc);

            /**
             * Creates an Empty 2D array texture that can be used as an Render Target
             */
            static RZTexture2D* CreateArray(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc);

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
                RZ_DEBUG_NAME_TAG_F_ARG const std::string& filePath, const RZTextureDesc& desc);

            GET_INSTANCE_SIZE

            /**
             * Sets the pixel data for the 2D Texture 
             * 
             * @param pixels The pixel data to set
             */
            virtual void SetData(const void* pixels) = 0;
        };

        //-----------------------------------------------------------------------------------
        // Texture 3D
        //-----------------------------------------------------------------------------------
        /* 3D Texture interface */
        class RAZIX_API RZTexture3D : public RZTexture, public IRZResource<RZTexture3D>
        {
        public:
            /**
             * Create a 3D texture resource from the given data
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
            static RZTexture3D* Create(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc);

            GET_INSTANCE_SIZE
        };

        //-----------------------------------------------------------------------------------
        // Cube Map Texture
        //-----------------------------------------------------------------------------------
        class RAZIX_API RZCubeMap : public RZTexture, public IRZResource<RZCubeMap>
        {
        public:
            static RZCubeMap* Create(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc);

        protected:
        };

        //-----------------------------------------------------------------------------------
        // Depth Texture
        //-----------------------------------------------------------------------------------

        class RAZIX_API RZDepthTexture : public RZTexture, public IRZResource<RZDepthTexture>
        {
        public:
            static RZDepthTexture* Create(const RZTextureDesc& desc);
        };

        //-----------------------------------------------------------------------------------
        // Render Texture
        //-----------------------------------------------------------------------------------

        // TODO: Remove this and add method in RZTexture to bind any texture as a Render Target, currently this class creates a 2D render target only

        class RAZIX_API RZRenderTexture : public RZTexture, public IRZResource<RZRenderTexture>
        {
        public:
            static RZRenderTexture* Create(RZ_DEBUG_NAME_TAG_F_ARG const RZTextureDesc& desc);
        };
#endif
    }    // namespace Graphics
}    // namespace Razix