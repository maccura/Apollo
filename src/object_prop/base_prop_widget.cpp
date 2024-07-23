#include "base_prop_widget.hpp"
#include <QPainter>
namespace ReportDesign
{
base_prop_widget::base_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent): QWidget(parent),
    m_object(object), m_objects(objects)
{

}
}
