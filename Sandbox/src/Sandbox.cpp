#include <Razix.h>

using namespace Razix;

class Sandbox : public Razix::RZApplication
{
private:
    struct ViewProjectionUniformBuffer
    {
        alignas(16)glm::mat4 view       = glm::mat4(1.0f);
        alignas(16)glm::mat4 projection = glm::mat4(1.0f);

    }viewProjUBOData;

public:
    Sandbox() : RZApplication("/Sandbox/","Sandbox")
    {

    }

    ~Sandbox()              
    {

    }

    void OnStart() override
    {
        Graphics::RZTexture::Filtering filtering = {};
        filtering.minFilter = Graphics::RZTexture::Filtering::FilterMode::LINEAR;
        filtering.magFilter = Graphics::RZTexture::Filtering::FilterMode::LINEAR;

        Graphics::RZTexture2D* logoTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/RazixLogo.png", "TextureAttachmentGLTest", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE, filtering);

        float vertices[3 * 3] = {
           -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f,
        };

        // This buffer layout will be somehow combined with the vertex buffers and passed to the pipeline for the Input Assembly stage
        bufferLayout.push<glm::vec3>("Position");

        triVBO = Graphics::RZVertexBuffer::Create(sizeof(float) * 9, vertices, Graphics::BufferUsage::STATIC);
        triVBO->AddBufferLayout(bufferLayout);

        viewProjUniformBuffer = Graphics::RZUniformBuffer::Create(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

        cmdBuffer = Graphics::RZCommandBuffer::Create();
        cmdBuffer->Init();

        // Create the shader
        defaultShader = Graphics::RZShader::Create("//RazixContent/Shaders/Razix/default.rzsf");
        std::cout << "Hmmmmmmm" << std::endl;
    }

    void OnUpdate(const RZTimestep& dt) override 
    {
        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.39f, 0.33f, 0.43f);
        }
        else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::VULKAN) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.99f, 0.33f, 0.43f);

            // Set the view port
            cmdBuffer->UpdateViewport(RZApplication::Get().getWindow().getWidth(), RZApplication::Get().getWindow().getHeight());

            // Update the uniform buffer data
            viewProjUBOData.view = glm::mat4(1.0f);
            viewProjUBOData.projection = glm::mat4(1.0f);
            viewProjUniformBuffer->SetData(sizeof(ViewProjectionUniformBuffer), &viewProjUBOData);

            // Bind the Vertex and Index buffers
            triVBO->Bind(cmdBuffer);

            // Record the draw commands to be submitted
            cmdBuffer->BeginRecording();
            {   
                cmdBuffer->Draw(3, 0, 0, 0);
            }
            cmdBuffer->EndRecording();

            cmdBuffer->Execute();

        }
        else if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Graphics::RenderAPI::DIRECTX11)
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.04f, 0.44f, 0.66f);
    }

private:
    Graphics::RZVertexBufferLayout  bufferLayout;
    Graphics::RZVertexBuffer*       triVBO;
    Graphics::RZUniformBuffer*      viewProjUniformBuffer;
    Graphics::RZCommandBuffer*      cmdBuffer;
    Graphics::RZShader*             defaultShader;
    // TODO: 1. Create Descriptor sets + UBO binding
    // TODO: 2. Shader binding to sets and UBO
    // TODO: 3. Create Render pass
    // TODO: 4. Create Framebuffer
    // TODO: 5. Create Pipeline
    // TODO: 6. Assemble everything together
    // TODO: 7. Submit commands and render stuff + sync check
};

Razix::RZApplication* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new Sandbox();
}