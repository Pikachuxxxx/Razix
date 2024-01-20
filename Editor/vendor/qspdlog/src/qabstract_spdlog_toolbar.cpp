#include <qspdlog/qabstract_spdlog_toolbar.hpp>
#include <qspdlog/qspdlog.hpp>

QAbstractSpdLogToolBar::~QAbstractSpdLogToolBar()
{
    if (_parent) {
        _parent->removeToolbar(this);
        _parent = nullptr;
    }
}

void QAbstractSpdLogToolBar::setParent(QSpdLog* parent) { _parent = parent; }
