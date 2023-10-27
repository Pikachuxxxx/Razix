// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZScene.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Scene/RZEntity.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

namespace Razix {

    RZScene::RZScene()
    {
        // TODO: Find a way to Add Camera skybox and environment settings
        // Default entities created in a scene
        // 1. Default primary camera component entity
        // 2. Skybox entity with TODO components
        // 3. Environment settings entity with TODO components
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        m_Registry.on_construct<HierarchyComponent>().connect<&HierarchyComponent::OnConstruct>();
        m_Registry.on_destroy<HierarchyComponent>().connect<&HierarchyComponent::OnDestroy>();
    }

    RZScene::RZScene(std::string sceneName)
        : m_SceneName(sceneName)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
    }

    void RZScene::updatePhysics()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        RAZIX_UNIMPLEMENTED_METHOD
    }

    void RZScene::update()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
        // GPU/CPU culling can be done here as well

        // Update the Hierarchy Transformations
        // First update only those entities without and Hierarchy Component
        auto nonHierarchyTransformsView = m_Registry.view<TransformComponent>(entt::exclude<HierarchyComponent>);
        for (auto& entity: nonHierarchyTransformsView)
            m_Registry.get<TransformComponent>(entity).SetWorldTransform(glm::mat4(1.0f));

        // Now Recursively update the Entities with children
        auto hierarchyView = m_Registry.view<HierarchyComponent>();
        for (auto& entity: hierarchyView) {
            const auto hierarchy = m_Registry.try_get<HierarchyComponent>(entity);
            // Update only the children, always start from root parent to child and update recursively
            if (hierarchy && hierarchy->Parent == entt::null) {
                updateTransform(entity);
            }
        }
    }

    void RZScene::updateTransform(entt::entity entity)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        const auto hierarchy = m_Registry.try_get<HierarchyComponent>(entity);
        if (hierarchy) {
            auto transform = m_Registry.try_get<TransformComponent>(entity);
            if (transform) {
                if (hierarchy->Parent != entt::null) {
                    auto parentTransform = m_Registry.try_get<TransformComponent>(hierarchy->Parent);
                    if (parentTransform) {
                        transform->SetWorldTransform(parentTransform->GetWorldTransform());
                    } else {
                        transform->SetWorldTransform(glm::mat4(1.0f));
                    }
                }
            } else {
                transform->SetWorldTransform(glm::mat4(1.0f));
            }

            entt::entity child = hierarchy->First;
            while (child != entt::null) {
                auto hierarchyComponent = m_Registry.try_get<HierarchyComponent>(child);
                auto next               = hierarchyComponent ? hierarchyComponent->Next : entt::null;
                updateTransform(child);
                child = next;
            }
        }
    }

    void RZScene::drawScene(Graphics::RZPipelineHandle pipeline, SceneDrawGeometryMode geometryMode)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        auto cmdBuffer = Graphics::RHI::GetCurrentCommandBuffer();

        auto  shaderHandle    = Graphics::RZResourceManager::Get().getPipelineResource(pipeline)->getDesc().shader;
        auto& sceneDrawParams = Graphics::RZResourceManager::Get().getShaderResource(shaderHandle)->getSceneDrawParams();

        u32 startSetIdx = 0;

        //-----------------------------
        // Frame Data
        if (sceneDrawParams.enableFrameData) {
            Graphics::RHI::BindDescriptorSet(pipeline, cmdBuffer, Graphics::RHI::Get().getFrameDataSet(), BindingTable_System::SET_IDX_FRAME_DATA);
            startSetIdx++;
        }
        //-----------------------------
        if (sceneDrawParams.enableMaterials)
            startSetIdx++;
        // Bindless Textures (Can be used for any texture, not just mats, even RTs used a bindables hence this is always bound)
        //if (!sceneDrawParams.disableBindlessTextures)
        //    Graphics::RHI::EnableBindlessTextures(pipeline, cmdBuffer);
        //-----------------------------
        // Scene Lighting Data
        if (sceneDrawParams.enableLights) {
            Graphics::RHI::BindDescriptorSet(pipeline, cmdBuffer, Graphics::RHI::Get().getSceneLightsDataSet(), BindingTable_System::SET_IDX_LIGHTING_DATA);
            startSetIdx++;
        }
        //-----------------------------
        // User Sets
        if (sceneDrawParams.userSets.size() > 0)
            Graphics::RHI::BindUserDescriptorSets(pipeline, cmdBuffer, sceneDrawParams.userSets, startSetIdx);
        //-----------------------------

        if (geometryMode == SceneDrawGeometryMode::SceneGeometry) {
            auto mesh_group = m_Registry.group<MeshRendererComponent>(entt::get<TransformComponent>);
            for (auto entity: mesh_group) {
                // Draw the mesh renderer components
                const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                if (!mrc.Mesh)
                    continue;

                // Bind push constants, VBO, IBO and draw
                glm::mat4 transform = mesh_trans.GetGlobalTransform();

                //-----------------------------
                Graphics::RZPushConstant modelMatrixPC;
                modelMatrixPC.shaderStage = Graphics::ShaderStage::Vertex;
                modelMatrixPC.offset      = 0;
                struct PCD
                {
                    glm::mat4 mat;
                } pcData{};
                pcData.mat         = transform;
                modelMatrixPC.size = sizeof(PCD);
                modelMatrixPC.data = &pcData;
                if (sceneDrawParams.overridePushConstantData != nullptr) {
                    modelMatrixPC.size = sceneDrawParams.overridePushConstantDataSize;
                    modelMatrixPC.data = sceneDrawParams.overridePushConstantData;
                }
                Graphics::RHI::BindPushConstant(pipeline, cmdBuffer, modelMatrixPC);
                //-----------------------------
                Graphics::RZMaterial* mat = nullptr;

                if (sceneDrawParams.enableMaterials) {    // @ BindingTable_System::SET_IDX_MATERIAL_DATA Only UBO is uploaded
                    mat = mrc.Mesh->getMaterial();
                    if (!mat)
                        continue;
                    mat->Bind();
                    Graphics::RHI::BindDescriptorSet(pipeline, cmdBuffer, mat->getDescriptorSet(), BindingTable_System::SET_IDX_MATERIAL_DATA);
                }

                mrc.Mesh->getVertexBuffer()->Bind(cmdBuffer);
                mrc.Mesh->getIndexBuffer()->Bind(cmdBuffer);

                ////-----------------------------
                //// Material Data
                //if (sceneDrawParams.enableMaterials)
                //    Graphics::RHI::BindDescriptorSet(pipeline, cmdBuffer, mat->getDescriptorSet(), BindingTable_System::SET_IDX_MATERIAL_DATA);
                ////-----------------------------

                Graphics::RHI::DrawIndexed(cmdBuffer, mrc.Mesh->getIndexCount());
            }
        } else if (geometryMode == SceneDrawGeometryMode::Cubemap) {
            if (!m_Cube)
                m_Cube = Graphics::MeshFactory::CreatePrimitive(Graphics::Cube);

            m_Cube->getVertexBuffer()->Bind(cmdBuffer);
            m_Cube->getIndexBuffer()->Bind(cmdBuffer);

            // TODO: Bind mat and desc sets and PCs

            Graphics::RHI::DrawIndexed(cmdBuffer, m_Cube->getIndexCount());

        } else if (geometryMode == SceneDrawGeometryMode::ScreenQuad) {
            if (!m_ScreenQuad)
                m_ScreenQuad = Graphics::MeshFactory::CreatePrimitive(Graphics::ScreenQuad);

            m_ScreenQuad->getVertexBuffer()->Bind(cmdBuffer);
            m_ScreenQuad->getIndexBuffer()->Bind(cmdBuffer);

            // TODO: Bind mat and desc sets and PCs

            Graphics::RHI::DrawIndexed(cmdBuffer, m_ScreenQuad->getIndexCount());

        } else if (geometryMode == SceneDrawGeometryMode::UI) {
            // If we ever have engine UI system other than ImGui we handle it's drawing here
        } else if (geometryMode == SceneDrawGeometryMode::Custom) {
            // Custom so we don't do anything here
            return;
        }
    }

    void RZScene::Destroy()
    {
        // Meshes
        auto mrcs = this->GetComponentsOfType<MeshRendererComponent>();
        for (auto& mesh: mrcs)
            mesh.Mesh->Destroy();

        // Sprites
        auto sprites = this->GetComponentsOfType<SpriteRendererComponent>();
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

        // This line causes a nodiscard warning ignore it
#pragma warning(push)
#pragma warning(disable : 4834)
        entt::snapshot{m_Registry}.entities(defArchive).component<RAZIX_COMPONENTS>(defArchive);
#pragma warning(pop)
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
        // This will cause a not all control paths return a value warning, it's fine cause it doesn't make the engine to run without a camera!
    }

    // Serialization Functions
    template<class Archive>
    void RZScene::save(Archive& archive) const
    {
        archive(cereal::make_nvp("UUID", m_SceneUUID.prettyString()));
        archive(cereal::make_nvp("SceneName", m_SceneName));
        archive(cereal::make_nvp("Total Entities", (u32) m_Registry.alive()));
    }

    template<class Archive>
    void RZScene::load(Archive& archive)
    {
        std::string uuid_string;
        archive(cereal::make_nvp("UUID", uuid_string));
        m_SceneUUID = RZUUID::FromStrFactory(uuid_string);
        archive(cereal::make_nvp("SceneName", m_SceneName));
    }

    template<typename T>
    void RZScene::OnComponentAdded(RZEntity entity, T& component)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);
        m_Registry.on_construct<T>().connect<&T::OnConstruct>();
    }
}    // namespace Razix