#pragma once

#include <QSortFilterProxyModel>

class QSpdLogProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    QSpdLogProxyModel(QObject* parent = nullptr);
};
