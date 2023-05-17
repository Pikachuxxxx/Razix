// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZScene.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Scene/RZEntity.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZModel.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"

namespace Razix {

    RZScene::RZScene()
    {
        // TODO: Find a way to Add Camera skybox and environment settings
        // Default entities created in a scene
        // 1. Default primary camera component entity
        // 2. Skybox entity with TODO components
        // 3. Environment settings entity with TODO components
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    }

    RZScene::RZScene(std::string sceneName)
        : m_SceneName(sceneName)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    }

    void RZScene::updatePhysics()
    {
        RAZIX_UNIMPLEMENTED_METHOD
    }

    void RZScene::drawScene(Graphics::RZPipeline* pipeline, Graphics::RZDescriptorSet* frameDataSet, Graphics::RZDescriptorSet* sceneLightsSet, std::vector<Graphics::RZDescriptorSet*> userSets)
    {
        auto cmdBuffer = Graphics::RHI::GetCurrentCommandBuffer();

        return;
        // Get the list of entities and their transform component together
        auto& group = m_Registry.group<Razix::Graphics::RZModel>(entt::get<TransformComponent>);
        for (auto entity: group) {
            const auto& [model, trans] = group.get<Razix::Graphics::RZModel, TransformComponent>(entity);

            auto& meshes = model.getMeshes();

            // Bind push constants, VBO, IBO and draw
            glm::mat4 transform = trans.GetTransform();

            //-----------------------------
            Graphics::RZPushConstant modelMatrixPC;
            modelMatrixPC.shaderStage = Graphics::ShaderStage::VERTEX;
            modelMatrixPC.offset      = 0;
            struct PCD
            {
                glm::mat4 mat;
            } pcData{};
            pcData.mat         = transform;
            modelMatrixPC.data = &pcData;
            modelMatrixPC.size = sizeof(PCD);

            // TODO: this needs to be done per mesh with each model transform multiplied by the parent Model transform (Done when we have per mesh entities instead of a model component)
            Graphics::RHI::BindPushConstant(pipeline, cmdBuffer, modelMatrixPC);
            //-----------------------------
            // Bind IBO and VBO
            for (auto& mesh: meshes) {
                mesh->getVertexBuffer()->Bind(cmdBuffer);
                mesh->getIndexBuffer()->Bind(cmdBuffer);

                mesh->getMaterial()->Bind();

                // Combine System Desc sets with material sets and Bind them
                std::vector<Graphics::RZDescriptorSet*> setsToBindInOrder = {frameDataSet, mesh->getMaterial()->getDescriptorSet(), sceneLightsSet};
                if (!userSets.empty())
                    setsToBindInOrder.insert(setsToBindInOrder.end(), userSets.begin(), userSets.end());

                Graphics::RHI::BindDescriptorSets(pipeline, cmdBuffer, setsToBindInOrder);

                Graphics::RHI::DrawIndexed(Graphics::RHI::GetCurrentCommandBuffer(), mesh->getIndexCount());
            }
        }

        auto& mesh_group = m_Registry.group<MeshRendererComponent>(entt::get<TransformComponent>);
        for (auto entity: mesh_group) {
            // Draw the mesh renderer components
            const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

            // Bind push constants, VBO, IBO and draw
            glm::mat4 transform = mesh_trans.GetTransform();

            //-----------------------------
            Graphics::RZPushConstant modelMatrixPC;
            modelMatrixPC.shaderStage = Graphics::ShaderStage::VERTEX;
            modelMatrixPC.offset      = 0;
            struct PCD
            {
                glm::mat4 mat;
            } pcData{};
            pcData.mat         = transform;
            modelMatrixPC.data = &pcData;
            modelMatrixPC.size = sizeof(PCD);

            // TODO: this needs to be done per mesh with each model transform multiplied by the parent Model transform (Done when we have per mesh entities instead of a model component)
            Graphics::RHI::BindPushConstant(pipeline, cmdBuffer, modelMatrixPC);
            //-----------------------------

            mrc.Mesh->getMaterial()->Bind();

            mrc.Mesh->getVertexBuffer()->Bind(cmdBuffer);
            mrc.Mesh->getIndexBuffer()->Bind(cmdBuffer);

            // Combine System Desc sets with material sets and Bind them
            std::vector<Graphics::RZDescriptorSet*> setsToBindInOrder = {frameDataSet, mrc.Mesh->getMaterial()->getDescriptorSet(), sceneLightsSet};
            if (!userSets.empty())
                setsToBindInOrder.insert(setsToBindInOrder.end(), userSets.begin(), userSets.end());

            Graphics::RHI::BindDescriptorSets(pipeline, cmdBuffer, setsToBindInOrder);

            Graphics::RHI::DrawIndexed(cmdBuffer, mrc.Mesh->getIndexCount());
        }
    }

    void RZScene::Destroy()
    {
        // Destroy models
        auto& mcs = this->GetComponentsOfType<Graphics::RZModel>();
        for (Graphics::RZModel model: mcs)
            model.Destroy();

        // Meshes
        auto& mrcs = this->GetComponentsOfType<MeshRendererComponent>();
        for (auto& mesh: mrcs)
            mesh.Mesh->Destroy();

        // Sprites
        auto& sprites = this->GetComponentsOfType<SpriteRendererComponent>();
        for (auto& sprite: sprites)
            sprite.Sprite->destroy();
    }

    RZEntity RZScene::createEntity(const std::string& name /*= std::string()*/)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        RZEntity entity = {m_Registry.create(), this};
        // By default an entity has 3 components
        // 1. ID Component - Automatically allocates a UUID to the entity
        // 2. Tag component - Add a name/tag to the entity for human readable identification
        // 3. Transform
        entity.AddComponent<IDComponent>();

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag   = name.empty() ? "Entity" : name;

        entity.AddComponent<TransformComponent>();

        return entity;
    }

    void RZScene::destroyEntity(RZEntity entity)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        m_Registry.destroy(entity);
    }

    void RZScene::saveScene()
    {
        serialiseScene(m_ScenePath);
    }

    void RZScene::reloadScene()
    {
        RAZIX_UNIMPLEMENTED_METHOD
    }

    void RZScene::serialiseScene(const std::string& filePath)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        std::string fullFilePath;
        bool        nope = RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, fullFilePath);
        RAZIX_CORE_WARN("[Scene] Saving scene to - {0} ({1})", filePath, fullFilePath);

        if (!nope) {
            std::string path = "//Scenes/";
            RZVirtualFileSystem::Get().resolvePhysicalPath(path, fullFilePath, true);

            fullFilePath += (m_SceneName + std::string(".rzscn"));
        }

        std::ofstream             opAppStream(fullFilePath);
        cereal::JSONOutputArchive defArchive(opAppStream);
        defArchive(cereal::make_nvp("Razix Scene", *this));

        entt::snapshot{m_Registry}.entities(defArchive).component<RAZIX_COMPONENTS>(defArchive);
    }

    void RZScene::deSerialiseScene(const std::string& filePath)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        // Cache the scene file path
        m_ScenePath = filePath;    // Remember this is a VFS path

        std::string fullFilePath;
        RZVirtualFileSystem::Get().resolvePhysicalPath(filePath, fullFilePath);
        RAZIX_CORE_WARN("[Scene] Loading scene from - {0} ({1})", filePath, fullFilePath);

        std::ifstream AppStream;
        AppStream.open(fullFilePath, std::ifstream::in);
        cereal::JSONInputArchive inputArchive(AppStream);
        inputArchive(cereal::make_nvp("Razix Scene", *this));
        //inputArchive(*this);
        entt::snapshot_loader{m_Registry}.entities(inputArchive).component<RAZIX_COMPONENTS>(inputArchive);
    }

    RZSceneCamera& RZScene::getSceneCamera()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        auto view = m_Registry.view<CameraComponent>();
        for (auto& entity: view) {
            // check it it's primary and only then return only a single camera component
            return view.get<CameraComponent>(entity).Camera;
        }
    }

    template<typename T>
    void RZScene::OnComponentAdded(RZEntity entity, T& component, bool enable)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        if (enable)
            m_Registry.on_construct<T>().connect<&T::OnConstruct>();
        else
            m_Registry.on_construct<T>().disconnect<&T::OnConstruct>();
    }
}    // namespace Razix