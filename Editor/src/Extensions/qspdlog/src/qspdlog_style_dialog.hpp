#pragma once

#include <QDialog>
#include <optional>

class QSpdLogModel;

class QSpdLogStyleDialog : public QDialog
{
    Q_OBJECT

public:
    struct Style {
        std::string loggerName;
        std::optional<QColor> backgroundColor;
        std::optional<QColor> textColor;
    };

public:
    explicit QSpdLogStyleDialog(QWidget* parent = nullptr);
    ~QSpdLogStyleDialog() override;

    Style result() const;
    void setModel(const QSpdLogModel* model);

private:
    Style _result;
    const QSpdLogModel* _model;
};
