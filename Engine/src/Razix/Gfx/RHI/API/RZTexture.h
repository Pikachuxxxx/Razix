
#pragma once

#include "Razix/Core/Log/RZLog.h"

#include "Razix/Gfx/Resources/IRZResource.h"

namespace Razix {
    namespace Gfx {

#define RZ_TEX_DEFAULT_ARRAY_LAYER 0
#define RZ_TEX_DEFAULT_MIP_IDX     0
#define RZ_TEX_BITS_PER_PIXEL      32
#define RZ_TEX_CHANNELS_PER_PIXEL  4
#define RZ_TEX_BITS_PER_CHANNELS   8
#define RZ_TEX_MAX_LAYERS          64    // MAX HARD LIMIT
#define RZ_MAX_TEX_MIPS            16    // 8k texture 7680x4320 (rounded off for brevity)

        // Forward Declaration
        struct RZTextureDesc;

        /**
         * A class that manages Textures/Image resources for the Engine
         * @brief It manages creation and conversion of Image resources, also stores in a custom Engine Format depending on how it's being used
         * 
         * All will be under RZTexture and will use the TextureType struct to identify the type of the Texture
         * We will use a common set of functions under RZTexture to create various textures
         */
        // TODO: Add support and Utility functions for sRGB textures
        class RAZIX_API RZTexture : public IRZResource<RZTexture>
        {
        public:
            RAZIX_VIRTUAL_DESCTURCTOR(RZTexture)

            GET_INSTANCE_SIZE;

            static u32           CalculateMipMapCount(u32 width, u32 height);
            static TextureFormat BitsToTextureFormat(u32 bits);

            virtual void  Resize(u32 width, u32 height) = 0;
            virtual void* GetAPIHandlePtr() const       = 0;
            // TODO: Add support for reading z/array layer
            // TODO: Extend this to set data at desired mip level and array index
            virtual void    GenerateMipsAndViews() {}
            virtual void    UploadToBindlessSet() {}
            virtual int32_t ReadPixels(u32 x, u32 y) = 0;

            inline const RZTextureDesc& getDescription() const { return m_Desc; }
            inline const std::string&   getName() const { return m_Desc.name; }
            inline u32                  getWidth() const { return m_Desc.width; }
            inline u32                  getHeight() const { return m_Desc.height; }
            inline u32                  getSize() const { return m_Desc.size; }
            inline u32                  getTotalSize() const { return m_Desc.size * m_Desc.dataSize; }
            inline u32                  getElementSize() const { return m_Desc.dataSize; }
            inline TextureType          getType() const { return m_Desc.type; }
            inline TextureFormat        getFormat() const { return m_Desc.format; }
            inline std::string          getPath() const { return m_Desc.filePath; }
            inline u32                  getCurrentMipLevel() const { return m_CurrentMipRenderingLevel; }
            inline void                 setCurrentMipLevel(u32 idx) { m_CurrentMipRenderingLevel = idx; }
            inline u32                  getBaseArrayLayer() const { return m_BaseArrayLayer; }
            inline void                 setCurrentArrayLayer(u32 idx) { m_BaseArrayLayer = idx; }
            inline u32                  getLayersCount() const { return m_Desc.layers; }
            inline u32                  getMipsCount() const { return m_TotalMipLevels; }
            inline ImageLayout          getCurrentLayout() const { return m_CurrentLayout; }

        protected:
            RZTextureDesc m_Desc                     = {};
            u32           m_TotalMipLevels           = 1;
            u32           m_CurrentMipRenderingLevel = RZ_TEX_DEFAULT_MIP_IDX;
            u32           m_BaseArrayLayer           = RZ_TEX_DEFAULT_ARRAY_LAYER;
            u32           m_BitsPerPixel             = RZ_TEX_BITS_PER_PIXEL;
            ImageLayout   m_CurrentLayout            = ImageLayout::kNewlyCreated;

        private:
            static void Create(void* where, const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            // only resource manager can create an instance of this class
            friend class RZResourceManager;
        };
    }    // namespace Gfx
}    // namespace Razix
