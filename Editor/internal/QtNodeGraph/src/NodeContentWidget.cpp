#include "NodeContentWidget.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QLabel>

NodeContentWidget::NodeContentWidget(QWidget* parent)
    : QWidget(parent)
{
    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    auto textLbl = new QLabel("Test_String_0");
    layout->addWidget(textLbl);
    layout->addWidget(new QTextEdit("Placeholder text"));
}
