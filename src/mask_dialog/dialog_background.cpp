#include "dialog_background.hpp"

namespace ReportDesign
{
dialog_background::dialog_background(QWidget *parent): QLabel(parent)
{
    init();
}

dialog_background::~dialog_background()
{

}

void dialog_background::init()
{
    QWidget *parent = parentWidget();
    if (parent)
    {
        setGeometry(0, 0, parent->width(), parent->height());
    }
    setObjectName("backLabel");
    showFullScreen();
}

void dialog_background::mousePressEvent(QMouseEvent *event)
{

}

void dialog_background::mouseMoveEvent(QMouseEvent *event)
{

}

void dialog_background::mouseDoubleClickEvent(QMouseEvent *event)
{

}

}
