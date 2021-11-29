#include <Razix.h>

using namespace Razix;

class Sandbox : public Razix::RZApplication
{
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

        cmdBuffer = Graphics::RZCommandBuffer::Create();
        cmdBuffer->Init();
    }

    void OnUpdate(const RZTimestep& dt) override 
    {
        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL) {
            Razix::Graphics::RZGraphicsContext::GetContext()->ClearWithColor(0.97f, 0.58f, 0.25f);

            cmdBuffer->UpdateViewport(RZApplication::Get().getWindow().getWidth(), RZApplication::Get().getWindow().getHeight());

            triVBO->Bind(cmdBuffer);

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
    Graphics::RZVertexBufferLayout bufferLayout;
    Graphics::RZVertexBuffer* triVBO;
    Graphics::RZCommandBuffer* cmdBuffer;
};

Razix::RZApplication* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application");
    return new Sandbox();
}