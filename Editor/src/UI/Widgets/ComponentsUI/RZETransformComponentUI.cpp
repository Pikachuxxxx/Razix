// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZETransformComponentUI.h"

#include "Razix/Scene/Components/LightComponent.h"
#include "Razix/Scene/Components/TransformComponent.h"

#include <QDoubleValidator>

namespace Razix {
    namespace Editor {

        glm::vec3 RPYToDirectionVector(float roll, float pitch, float yaw)
        {
            float x = -cos(yaw) * sin(pitch) * sin(roll) - sin(yaw) * cos(roll);
            float y = -sin(yaw) * sin(pitch) * sin(roll) + cos(yaw) * cos(roll);
            float z = cos(pitch) * sin(roll);
            return glm::vec3(x, y, z);
        }

        RZETransformComponentUI::RZETransformComponentUI(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            // Accept only numerical input
            ui.PosVal_X->setValidator(new QDoubleValidator(-100.0, 100.0, 2, this));
            ui.PosVal_Y->setValidator(new QDoubleValidator(-100.0, 100.0, 2, this));
            ui.PosVal_Z->setValidator(new QDoubleValidator(-100.0, 100.0, 2, this));
            ui.RotVal_X->setValidator(new QDoubleValidator(-100.0, 100.0, 2, this));
            ui.RotVal_Y->setValidator(new QDoubleValidator(-100.0, 100.0, 2, this));
            ui.RotVal_Z->setValidator(new QDoubleValidator(-100.0, 100.0, 2, this));
            ui.ScaleVal_X->setValidator(new QDoubleValidator(-100.0, 100.0, 2, this));
            ui.ScaleVal_Y->setValidator(new QDoubleValidator(-100.0, 100.0, 2, this));
            ui.ScaleVal_Z->setValidator(new QDoubleValidator(-100.0, 100.0, 2, this));

            // mAKE CONNECTIONS TO REFLECT EDITING
            connect(ui.PosVal_X, SIGNAL(returnPressed()), this, SLOT(OnPosXEdited()));
            connect(ui.PosVal_Y, SIGNAL(returnPressed()), this, SLOT(OnPosYEdited()));
            connect(ui.PosVal_Z, SIGNAL(returnPressed()), this, SLOT(OnPosZEdited()));

            connect(ui.RotVal_X, SIGNAL(returnPressed()), this, SLOT(OnRotXEdited()));
            connect(ui.RotVal_Y, SIGNAL(returnPressed()), this, SLOT(OnRotYEdited()));
            connect(ui.RotVal_Z, SIGNAL(returnPressed()), this, SLOT(OnRotZEdited()));

            connect(ui.ScaleVal_X, SIGNAL(returnPressed()), this, SLOT(OnScaleXEdited()));
            connect(ui.ScaleVal_Y, SIGNAL(returnPressed()), this, SLOT(OnScaleYEdited()));
            connect(ui.ScaleVal_Z, SIGNAL(returnPressed()), this, SLOT(OnScaleZEdited()));
        }

        RZETransformComponentUI::~RZETransformComponentUI()
        {
        }

        void RZETransformComponentUI::setEditingEntity(RZEntity entity)
        {
            m_Entity = entity;
            auto& tc = m_Entity.GetComponent<TransformComponent>();
            m_TC     = tc;
            // Set the values of the labels when we select a new entity to edit
            ui.PosVal_X->setText(std::to_string(tc.Translation.x).c_str());
            ui.PosVal_Y->setText(std::to_string(tc.Translation.y).c_str());
            ui.PosVal_Z->setText(std::to_string(tc.Translation.z).c_str());

            ui.RotVal_X->setText(std::to_string(tc.Rotation.x).c_str());
            ui.RotVal_Y->setText(std::to_string(tc.Rotation.y).c_str());
            ui.RotVal_Z->setText(std::to_string(tc.Rotation.z).c_str());

            ui.ScaleVal_X->setText(std::to_string(tc.Scale.x).c_str());
            ui.ScaleVal_Y->setText(std::to_string(tc.Scale.y).c_str());
            ui.ScaleVal_Z->setText(std::to_string(tc.Scale.z).c_str());
        }

        void RZETransformComponentUI::OnPosXEdited()
        {
            m_TC.Translation = glm::vec3(ui.PosVal_X->text().toDouble(), ui.PosVal_Y->text().toDouble(), ui.PosVal_Z->text().toDouble());
            auto& tc         = m_Entity.GetComponent<TransformComponent>();
            tc               = m_TC;
        }

        void RZETransformComponentUI::OnPosYEdited()
        {
            m_TC.Translation = glm::vec3(ui.PosVal_X->text().toDouble(), ui.PosVal_Y->text().toDouble(), ui.PosVal_Z->text().toDouble());
            auto& tc         = m_Entity.GetComponent<TransformComponent>();
            tc               = m_TC;
        }

        void RZETransformComponentUI::OnPosZEdited()
        {
            m_TC.Translation = glm::vec3(ui.PosVal_X->text().toDouble(), ui.PosVal_Y->text().toDouble(), ui.PosVal_Z->text().toDouble());
            auto& tc         = m_Entity.GetComponent<TransformComponent>();
            tc               = m_TC;
        }

        void RZETransformComponentUI::OnRotXEdited()
        {
            m_TC.Rotation = glm::vec3(glm::radians(ui.RotVal_X->text().toDouble()), glm::radians(ui.RotVal_Y->text().toDouble()), glm::radians(ui.RotVal_Z->text().toDouble()));
            auto& tc      = m_Entity.GetComponent<TransformComponent>();
            tc            = m_TC;

            // Set the light direction if the entity has a light component
            if (m_Entity.HasComponent<LightComponent>()) {
                auto& lc = m_Entity.GetComponent<LightComponent>();
                lc.light.setDirection(RPYToDirectionVector(m_TC.Rotation.x, m_TC.Rotation.y, m_TC.Rotation.z));
            }
        }

        void RZETransformComponentUI::OnRotYEdited()
        {
            m_TC.Rotation = glm::vec3(glm::radians(ui.RotVal_X->text().toDouble()), glm::radians(ui.RotVal_Y->text().toDouble()), glm::radians(ui.RotVal_Z->text().toDouble()));
            auto& tc      = m_Entity.GetComponent<TransformComponent>();
            tc            = m_TC;

            // Set the light direction if the entity has a light component
            if (m_Entity.HasComponent<LightComponent>()) {
                auto& lc = m_Entity.GetComponent<LightComponent>();
                lc.light.setDirection(RPYToDirectionVector(m_TC.Rotation.x, m_TC.Rotation.y, m_TC.Rotation.z));
            }
        }

        void RZETransformComponentUI::OnRotZEdited()
        {
            m_TC.Rotation = glm::vec3(glm::radians(ui.RotVal_X->text().toDouble()), glm::radians(ui.RotVal_Y->text().toDouble()), glm::radians(ui.RotVal_Z->text().toDouble()));
            auto& tc      = m_Entity.GetComponent<TransformComponent>();
            tc            = m_TC;

            // Set the light direction if the entity has a light component
            if (m_Entity.HasComponent<LightComponent>()) {
                auto& lc = m_Entity.GetComponent<LightComponent>();
                lc.light.setDirection(RPYToDirectionVector(m_TC.Rotation.x, m_TC.Rotation.y, m_TC.Rotation.z));
            }
        }

        void RZETransformComponentUI::OnScaleXEdited()
        {
            m_TC.Scale = glm::vec3(ui.ScaleVal_X->text().toDouble(), ui.ScaleVal_Y->text().toDouble(), ui.ScaleVal_Z->text().toDouble());
            auto& tc   = m_Entity.GetComponent<TransformComponent>();
            tc         = m_TC;
        }

        void RZETransformComponentUI::OnScaleYEdited()
        {
            m_TC.Scale = glm::vec3(ui.ScaleVal_X->text().toDouble(), ui.ScaleVal_Y->text().toDouble(), ui.ScaleVal_Z->text().toDouble());
            auto& tc   = m_Entity.GetComponent<TransformComponent>();
            tc         = m_TC;
        }

        void RZETransformComponentUI::OnScaleZEdited()
        {
            m_TC.Scale = glm::vec3(ui.ScaleVal_X->text().toDouble(), ui.ScaleVal_Y->text().toDouble(), ui.ScaleVal_Z->text().toDouble());
            auto& tc   = m_Entity.GetComponent<TransformComponent>();
            tc         = m_TC;
        }

    }    // namespace Editor
}    // namespace Razix