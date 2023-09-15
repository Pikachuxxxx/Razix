// clang-format off
#include "rzepch.h"
// clang-format on
#include <QAction>
#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMenu>
#include <QScrollBar>
#include <QTreeView>
#include <spdlog/logger.h>

#include "qspdlog/qspdlog.hpp"

#include "qspdlog/qabstract_spdlog_toolbar.hpp"
#include "qspdlog_model.hpp"
#include "qspdlog_proxy_model.hpp"
#include "qspdlog_style_dialog.hpp"
#include "qt_logger_sink.hpp"

QSpdLog::QSpdLog(QWidget* parent)
    : QWidget(parent), _sourceModel(new QSpdLogModel), _proxyModel(new QSpdLogProxyModel), _view(new QTreeView)
{
    Q_INIT_RESOURCE(qspdlog_resources);
    _view->setModel(_proxyModel);
    _view->setObjectName("qspdlogTreeView");

    QHeaderView* header = _view->header();
    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
        header,
        &QHeaderView::customContextMenuRequested,
        this,
        [this, header](const QPoint& pos) {
            QMenu contextMenu;
            contextMenu.setObjectName("qspdlogHeaderContextMenu");
            for (int i = 0; i < _sourceModel->columnCount(); ++i) {
                QString columnHeader =
                    _sourceModel->headerData(i, Qt::Horizontal).toString();
                QAction* action = contextMenu.addAction(columnHeader);
                action->setCheckable(true);
                action->setChecked(!header->isSectionHidden(i));
                action->setData(i);

                connect(
                    action,
                    &QAction::toggled,
                    this,
                    [this, header](bool checked) {
                        QAction* action = qobject_cast<QAction*>(sender());
                        if (action)
                            header->setSectionHidden(action->data().toInt(), !checked);
                    });
            }

            contextMenu.exec(header->mapToGlobal(pos));
        });

    _proxyModel->setSourceModel(_sourceModel);

    connect(
        _sourceModel,
        &QAbstractItemModel::rowsAboutToBeInserted,
        this,
        [this](const QModelIndex& parent, int first, int last) {
            auto bar          = _view->verticalScrollBar();
            _scrollIsAtBottom = bar ? (bar->value() == bar->maximum()) : false;
        });

    _view->setRootIsDecorated(false);

    _sink = std::make_shared<qt_logger_sink_mt>(_sourceModel);

    setLayout(new QVBoxLayout);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addWidget(_view);
}

QSpdLog::~QSpdLog()
{
    std::static_pointer_cast<qt_logger_sink_mt>(_sink)->invalidate();
}

void QSpdLog::clear()
{
    _sourceModel->clear();
}

void QSpdLog::registerToolbar(QAbstractSpdLogToolBar* toolbarInterface)
{
    toolbarInterface->setParent(this);
    _toolbars.push_back(toolbarInterface);

    QLineEdit* filter                = toolbarInterface->filter();
    QAction*   regex                 = toolbarInterface->regex();
    QAction*   caseSensitive         = toolbarInterface->caseSensitive();
    QAction*   style                 = toolbarInterface->style();
    QComboBox* autoScrollPolicyCombo = toolbarInterface->autoScrollPolicy();

    auto updateFilter = [this, filter, regex, caseSensitive]() {
        filterData(
            filter->text(), regex->isChecked(), caseSensitive->isChecked());
    };

    connect(filter, &QLineEdit::textChanged, this, updateFilter);
    connect(regex, &QAction::toggled, this, updateFilter);
    connect(caseSensitive, &QAction::toggled, this, updateFilter);
    connect(style, &QAction::triggered, this, [this]() {
        QSpdLogStyleDialog dialog;
        dialog.setModel(_sourceModel);
        dialog.setObjectName("qspdlogStyleDialog");
        if (!dialog.exec())
            return;

        QSpdLogStyleDialog::Style value = dialog.result();

        _sourceModel->setLoggerBackground(
            value.loggerName, value.backgroundColor);

        _sourceModel->setLoggerForeground(value.loggerName, value.textColor);
    });
    connect(
        autoScrollPolicyCombo,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &QSpdLog::updateAutoScrollPolicy);
}

void QSpdLog::removeToolbar(QAbstractSpdLogToolBar* toolbarInterface)
{
    _toolbars.erase(
        std::remove(_toolbars.begin(), _toolbars.end(), toolbarInterface),
        _toolbars.end());
}

void QSpdLog::filterData(
    const QString& text, bool isRegularExpression, bool isCaseSensitive)
{
    _proxyModel->setFilterCaseSensitivity(
        isCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);

    if (isRegularExpression) {
        QRegularExpression regex(text);

        if (!regex.isValid())
            return;

        _proxyModel->setFilterRegularExpression(text);
    } else {
        _proxyModel->setFilterFixedString(text);
    }
}

void QSpdLog::setAutoScrollPolicy(AutoScrollPolicy policy)
{
    QObject::disconnect(_scrollConnection);

    switch (policy) {
        case AutoScrollPolicy::AutoScrollPolicyEnabled: {
            _scrollConnection = connect(
                _sourceModel,
                &QSpdLogModel::rowsInserted,
                _view,
                &QTreeView::scrollToBottom);
            break;
        }

        case AutoScrollPolicy::AutoScrollPolicyEnabledIfBottom: {
            _scrollConnection = connect(
                _sourceModel,
                &QSpdLogModel::rowsInserted,
                this,
                [this]() {
                    // We can't check if the scrollbar is at the bottom here because
                    // the new rows are already inserted and the position of the
                    // scrollbar may not be at the bottom of the widget anymore.
                    // That's why the scroll position is checked before actually
                    // adding the rows (AKA in the rowsAboutToBeInserted signal).
                    if (_scrollIsAtBottom)
                        _view->scrollToBottom();
                });
            break;
        }

        default: {
            // The connection is already disconnected. No need for handling the
            // AutoScrollPolicyDisabled case.
            break;
        }
    }

    for (auto& toolbar: _toolbars) {
        QComboBox* policyComboBox = toolbar->autoScrollPolicy();
        if (!policyComboBox)
            continue;

        auto blocked = policyComboBox->blockSignals(true);
        policyComboBox->setCurrentIndex(static_cast<int>(policy));
        policyComboBox->blockSignals(blocked);
    }
}

void QSpdLog::updateAutoScrollPolicy(int index)
{
    AutoScrollPolicy policy = static_cast<AutoScrollPolicy>(index);
    setAutoScrollPolicy(policy);
}

spdlog::sink_ptr QSpdLog::sink()
{
    return _sink;
}

std::size_t QSpdLog::itemsCount() const
{
    return static_cast<std::size_t>(_proxyModel->rowCount());
}

void QSpdLog::setMaxEntries(std::optional<std::size_t> maxEntries)
{
    _sourceModel->setMaxEntries(maxEntries);
}

std::optional<std::size_t> QSpdLog::getMaxEntries() const
{
    return _sourceModel->getMaxEntries();
}

void QSpdLog::setLoggerForeground(
    std::string_view loggerName, std::optional<QColor> brush)
{
    _sourceModel->setLoggerForeground(loggerName, brush);
}

std::optional<QColor> QSpdLog::getLoggerForeground(std::string_view loggerName) const
{
    return _sourceModel->getLoggerForeground(loggerName);
}

void QSpdLog::setLoggerBackground(
    std::string_view loggerName, std::optional<QBrush> brush)
{
    _sourceModel->setLoggerBackground(loggerName, brush);
}

std::optional<QBrush> QSpdLog::getLoggerBackground(std::string_view loggerName) const
{
    return _sourceModel->getLoggerBackground(loggerName);
}
