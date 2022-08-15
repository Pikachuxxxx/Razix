// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZETransformComponentUI.h"

#include "Razix/Scene/Components/TransformComponent.h"

#include <QDoubleValidator>

namespace Razix {
    namespace Editor {
        RZETransformComponentUI::RZETransformComponentUI(QWidget* parent)
            : QWidget(parent)
        {
            ui.setupUi(this);

            // Accept only numerical input
            ui.PosVal_X->setValidator(new QDoubleValidator(-100000.0, 100000.0, 2, this));
            ui.PosVal_Y->setValidator(new QDoubleValidator(-100000.0, 100000.0, 2, this));
            ui.PosVal_Z->setValidator(new QDoubleValidator(-100000.0, 100000.0, 2, this));
            ui.RotVal_X->setValidator(new QDoubleValidator(-100000.0, 100000.0, 2, this));
            ui.RotVal_Y->setValidator(new QDoubleValidator(-100000.0, 100000.0, 2, this));
            ui.RotVal_Z->setValidator(new QDoubleValidator(-100000.0, 100000.0, 2, this));
            ui.ScaleVal_X->setValidator(new QDoubleValidator(-100000.0, 100000.0, 2, this));
            ui.ScaleVal_Y->setValidator(new QDoubleValidator(-100000.0, 100000.0, 2, this));
            ui.ScaleVal_Z->setValidator(new QDoubleValidator(-100000.0, 100000.0, 2, this));

            // mAKE CONNECTIONS TO REFLECT EDITING
            connect(ui.PosVal_X, SIGNAL(returnPressed()), this, SLOT(OnPosXEdited()));
            connect(ui.PosVal_Y, SIGNAL(returnPressed()), this, SLOT(OnPosZEdited()));
            connect(ui.PosVal_Z, SIGNAL(returnPressed()), this, SLOT(OnPosXEdited()));
            connect(ui.RotVal_X, SIGNAL(returnPressed()), this, SLOT(OnRotXEdited()));
            connect(ui.RotVal_Y, SIGNAL(returnPressed()), this, SLOT(OnRotZEdited()));
            connect(ui.RotVal_Z, SIGNAL(returnPressed()), this, SLOT(OnRotXEdited()));
            connect(ui.ScaleVal_X, SIGNAL(returnPressed()), this, SLOT(OnScaleXEdited()));
            connect(ui.ScaleVal_Y, SIGNAL(returnPressed()), this, SLOT(OnScaleZEdited()));
            connect(ui.ScaleVal_Z, SIGNAL(returnPressed()), this, SLOT(OnScaleXEdited()));
        }

        RZETransformComponentUI::~RZETransformComponentUI()
        {}

        void RZETransformComponentUI::setEditingEntity(RZEntity entity)
        {
            m_Entity = entity;
            auto& tc = m_Entity.GetComponent<TransformComponent>();

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
            TransformComponent m_TC;
            m_TC.Translation = glm::vec3(ui.PosVal_X->text().toDouble(), ui.PosVal_Y->text().toDouble(), ui.PosVal_Z->text().toDouble());
            auto& tc         = m_Entity.GetComponent<TransformComponent>();
            tc               = m_TC;
            std::cout << ui.PosVal_X->text().toDouble() << std::endl;
        }

        void RZETransformComponentUI::OnPosYEdited()
        {
            TransformComponent m_TC;
            m_TC.Translation = glm::vec3(ui.PosVal_X->text().toDouble(), ui.PosVal_Y->text().toDouble(), ui.PosVal_Z->text().toDouble());
            auto& tc         = m_Entity.GetComponent<TransformComponent>();
            tc               = m_TC;
        }

        void RZETransformComponentUI::OnPosZEdited()
        {
            TransformComponent m_TC;
            m_TC.Translation = glm::vec3(ui.PosVal_X->text().toDouble(), ui.PosVal_Y->text().toDouble(), ui.PosVal_Z->text().toDouble());
            auto& tc         = m_Entity.GetComponent<TransformComponent>();
            tc               = m_TC;
        }

        void RZETransformComponentUI::OnRotXEdited()
        {
            TransformComponent m_TC;
            m_TC.Rotation = glm::vec3(ui.RotVal_X->text().toDouble(), ui.RotVal_Y->text().toDouble(), ui.RotVal_Z->text().toDouble());
            auto& tc      = m_Entity.GetComponent<TransformComponent>();
            tc            = m_TC;
        }

        void RZETransformComponentUI::OnRotYEdited()
        {
            TransformComponent m_TC;
            m_TC.Rotation = glm::vec3(ui.RotVal_X->text().toDouble(), ui.RotVal_Y->text().toDouble(), ui.RotVal_Z->text().toDouble());
            auto& tc      = m_Entity.GetComponent<TransformComponent>();
            tc            = m_TC;
        }

        void RZETransformComponentUI::OnRotZEdited()
        {
            TransformComponent m_TC;
            m_TC.Rotation = glm::vec3(ui.RotVal_X->text().toDouble(), ui.RotVal_Y->text().toDouble(), ui.RotVal_Z->text().toDouble());
            auto& tc      = m_Entity.GetComponent<TransformComponent>();
            tc            = m_TC;
        }

        void RZETransformComponentUI::OnScaleXEdited()
        {
            TransformComponent m_TC;
            m_TC.Scale = glm::vec3(ui.ScaleVal_X->text().toDouble(), ui.ScaleVal_Y->text().toDouble(), ui.ScaleVal_Z->text().toDouble());
            auto& tc   = m_Entity.GetComponent<TransformComponent>();
            tc         = m_TC;
        }

        void RZETransformComponentUI::OnScaleYEdited()
        {
            TransformComponent m_TC;
            m_TC.Scale = glm::vec3(ui.ScaleVal_X->text().toDouble(), ui.ScaleVal_Y->text().toDouble(), ui.ScaleVal_Z->text().toDouble());
            auto& tc   = m_Entity.GetComponent<TransformComponent>();
            tc         = m_TC;
        }

        void RZETransformComponentUI::OnScaleZEdited()
        {
            TransformComponent m_TC;
            m_TC.Scale = glm::vec3(ui.ScaleVal_X->text().toDouble(), ui.ScaleVal_Y->text().toDouble(), ui.ScaleVal_Z->text().toDouble());
            auto& tc   = m_Entity.GetComponent<TransformComponent>();
            tc         = m_TC;
        }

    }    // namespace Editor
}    // namespace Razix