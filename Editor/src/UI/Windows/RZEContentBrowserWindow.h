#pragma once

#include <QFrame>
#include <QFileSystemModel>

#include "generated/ui_RZEContentBrowserWindow.h"

namespace Razix {
    namespace Editor {
        class RZEContentBrowserWindow : public QFrame
        {
            Q_OBJECT

        public:
            RZEContentBrowserWindow(QWidget *parent = nullptr);
            ~RZEContentBrowserWindow();

        private slots:
            void OnTreeViewFolderClicked(const QModelIndex& index);

        private:
            Ui::ContentBrowserWindow ui;
            // Make two models instead of one
            // to filter them separately
            QFileSystemModel* dirModel;
            QFileSystemModel* fileModel;
        };
    }    // namespace Editor
}    // namespace Razix
