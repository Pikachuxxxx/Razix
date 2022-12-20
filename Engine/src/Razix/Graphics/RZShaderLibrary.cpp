// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZShaderLibrary.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/RZSplashScreen.h"

#include "Razix/Graphics/API/RZShader.h"

namespace Razix {
    namespace Graphics {

        void RZShaderLibrary::StartUp()
        {
            // Instance is automatically created once the system is Started Up
            RAZIX_CORE_INFO("[Shader Library] Starting Up Shader Library to laod shaders");
            //Razix::RZSplashScreen::Get().setLogString("STATIC_INITIALIZATION : Starting VFS...");
            Razix::RZSplashScreen::Get().setLogString("Shader Library ...");

            // TODO: Load shader caches and partition them into RZShader which is a derivative of RZAsset. so this also a discussion for another day
            // pre-load some shaders
            // Basic shaders
            loadShader("//RazixContent/Shaders/Razix/default.rzsf");
            //-------------------------------------------------------------------
            // Renderer related shaders
            loadShader("//RazixContent/Shaders/Razix/grid.rzsf");
            loadShader("//RazixContent/Shaders/Razix/forward_renderer.rzsf");
            loadShader("//RazixContent/Shaders/Razix/imgui.rzsf");
            loadShader("//RazixContent/Shaders/Razix/sprite.rzsf");
            loadShader("//RazixContent/Shaders/Razix/sprite_textured.rzsf");
            //-------------------------------------------------------------------
            // Frame Graph Pass Shaders
            // Composite Pass
            loadShader("//RazixContent/Shaders/Razix/composite_pass.rzsf");
        }

        void RZShaderLibrary::ShutDown()
        {
            // FIXME: This shut down is called after Graphics is done so...do whatever the fuck is necessary
            // Destroy all the shaders
            for (RZShader* shader: m_Shaders)
                shader->Destroy();

            RAZIX_CORE_ERROR("[Shader Library] Shutting Down Shader Library");
        }

        void RZShaderLibrary::loadShader(std::string shaderPath)
        {
            RZShader* shader = RZShader::Create(shaderPath RZ_DEBUG_NAME_TAG_STR_E_ARG(shaderPath));
            m_Shaders.push_back(shader);
        }

        RZShader* RZShaderLibrary::getShader(std::string shaderName)
        {
            for (RZShader* shader: m_Shaders)
                if (shader->getName() == shaderName)
                    return shader;

            // TODO: If fails load it by searching all the VFS or return a default shader
        }

    }    // namespace Graphics
}    // namespace Razix