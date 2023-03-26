// clang-format off
#include "rzepch.h"
// clang-format on
#include "RZEContentBrowserWindow.h"

#include "Razix/Core/RZApplication.h"

namespace Razix {
    namespace Editor {
        RZEContentBrowserWindow::RZEContentBrowserWindow(QWidget *parent)
            : QFrame(parent)
        {
            ui.setupUi(this);

            setObjectName(this->windowTitle());

            dirModel = new QFileSystemModel(this);
            // Set filter
            dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
            // Set the root path --> Project root path
            dirModel->setRootPath(Razix::RZApplication::Get().getAppFilePath().c_str());

            ui.directoryTreeView->setModel(dirModel);

             // FILES

            fileModel = new QFileSystemModel(this);

            // Set filter
            fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);

            // QFileSystemModel requires root path
            fileModel->setRootPath(Razix::RZApplication::Get().getAppFilePath().c_str());

            // Attach the model to the view
            ui.folderListView->setModel(fileModel);

            connect(ui.directoryTreeView, &QTreeView::clicked, this, &RZEContentBrowserWindow::OnTreeViewFolderClicked);
        }

        RZEContentBrowserWindow::~RZEContentBrowserWindow()
        {}

        void RZEContentBrowserWindow::OnTreeViewFolderClicked(const QModelIndex& index)
        {
            // TreeView clicked
            // 1. We need to extract path
            // 2. Set that path into our ListView

            // Get the full path of the item that's user clicked on
            QString mPath = dirModel->fileInfo(index).absoluteFilePath();
            ui.folderListView->setRootIndex(fileModel->setRootPath(mPath));
        }

    }    // namespace Editor
}    // namespace Razix
