#include "shape_prop_widget.hpp"
#include "property_widget_factory.hpp"
#include "item/base_design_item.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QButtonGroup>
namespace
{
ReportDesign::base_prop_widget *createShapePropWidget(QObject *object, QList<QObject *> *objects, QWidget *parent)
{
    if (dynamic_cast<ReportDesign::base_design_item *>(object) == nullptr)
    {
        return nullptr;
    }
    return new ReportDesign::shape_prop_widget(object, objects, parent);
}

bool registred = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(SHAPE_PROPERTY), createShapePropWidget);
}

namespace ReportDesign
{
shape_prop_widget::shape_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent):
    base_prop_widget(object, objects, parent), m_lineShape(new prop_combox(this))
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout();
    vlayout->addWidget(new QLabel(tr("shape"), this));
    hlayout->addWidget(m_lineShape, 1);
    hlayout->addItem(new QSpacerItem(40, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
    vlayout->addLayout(hlayout);
    vlayout->setContentsMargins(PROP_WIDGET_MARGINS);

    if (object)
    {
        m_lineShape->addItem(tr("Horizontal Line"), 0);
        m_lineShape->addItem(tr("Vertical Line"), 1);
        m_lineShape->setCurrentIndex(object->property(PROPERTY_TO_STRING(SHAPE_PROPERTY)).value<int>());
    }

    connect(m_lineShape, &prop_combox::currentTextChanged, [this](QString text)
    {
        int vaule = m_lineShape->currentData().toInt();
        if (this->object())
        {
            this->object()->setProperty(PROPERTY_TO_STRING(SHAPE_PROPERTY), vaule);
        }

        for (QObject *it : this->objects())
        {
            it->setProperty(PROPERTY_TO_STRING(SHAPE_PROPERTY), vaule);
        }
    });
}

void shape_prop_widget::update(QString PropertyName)
{
    if (!object())
    {
        return;
    }

    if (PropertyName == PROPERTY_TO_STRING(SHAPE_PROPERTY))
    {
        m_lineShape->setCurrentIndex(object()->property(PROPERTY_TO_STRING(SHAPE_PROPERTY)).toInt());
    }
}

}
