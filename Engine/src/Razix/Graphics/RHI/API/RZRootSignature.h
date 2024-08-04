#pragma once

namespace Razix {
    namespace Graphics {

        struct RZDescriptor;

        // [Source]: https://logins.github.io/graphics/2020/06/26/DX12RootSignatureObject.html

        /**
         * Root Signature describes the parameters passed to the various stages of the rendering pipeline
         * 
         * Abstracts over ID3D12RootSignature and vkPipelineLayout (either using API provided here or
         * from shader reflection for auto-gen) 
         */
        class RAZIX_API RZRootSignature : public IRZResource<RZShader>
        {
        public:
            RZRootSignature() {}
            /* Virtual destructor enables the API implementation to delete it's resources */
            RAZIX_VIRTUAL_DESCTURCTOR(RZRootSignature);
            /* Unless we have variants we don't want it to be copied around */
            RAZIX_NONCOPYABLE_CLASS(RZRootSignature);

            GET_INSTANCE_SIZE;

        private:
            /**
             * Creates a Root Signature 
             */
            static void Create(void* where, std::vector<RZDescriptor>& descriptors RZ_DEBUG_NAME_TAG_E_ARG);

            friend class RZResourceManager;
        };
    }    // namespace Graphics
}    // namespace Razix