#include "line_prop_edit.hpp"
#include "property_widget_factory.hpp"
#include "item/base_design_item.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QButtonGroup>
namespace
{
ReportDesign::base_prop_widget *createLinePropWidget(QObject *object, QList<QObject *> *objects, QWidget *parent)
{
    if (dynamic_cast<ReportDesign::base_design_item *>(object) == nullptr)
    {
        return nullptr;
    }
    return new ReportDesign::line_prop_edit(object, objects, parent);
}

bool registred = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(BORDERLINESIZE_PROPERTY), createLinePropWidget);
bool registred2 = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(BORDERCOLOR_PROPERTY), createLinePropWidget);
}

namespace ReportDesign
{
line_prop_edit::line_prop_edit(QObject *object, QList<QObject *> *objects, QWidget *parent):
    base_prop_widget(object, objects, parent), m_lineColorButton(new prop_color_button(this)), m_lineWidthSpinbox(new prop_spinbox_widget(this))
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout();
    vlayout->addWidget(new QLabel(tr("line"), this));
    hlayout->addWidget(m_lineColorButton, 1);
    hlayout->addWidget(m_lineWidthSpinbox, 1);
    hlayout->addItem(new QSpacerItem(40, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
    m_lineWidthSpinbox->setMinimumWidth(87);
    vlayout->addLayout(hlayout);
    vlayout->setContentsMargins(PROP_WIDGET_MARGINS);

    if (object)
    {
        m_lineWidthSpinbox->setValue(object->property(PROPERTY_TO_STRING(BORDERLINESIZE_PROPERTY)).value<int>());
        m_lineColorButton->setColor(object->property(PROPERTY_TO_STRING(BORDERCOLOR_PROPERTY)).value<QColor>());
    }

    connect(m_lineColorButton, &prop_color_button::colorChange, [this](QColor color)
    {
        if (this->object())
        {
            this->object()->setProperty(PROPERTY_TO_STRING(BORDERCOLOR_PROPERTY), color);
        }

        for (QObject *it : this->objects())
        {
            it->setProperty(PROPERTY_TO_STRING(BORDERCOLOR_PROPERTY), color);
        }
    });

    connect(m_lineWidthSpinbox, SIGNAL(valueChanged(QString)), this, SLOT(slotLineSizeChange(QString)));
}

void line_prop_edit::slotLineSizeChange(QString size)
{
    if (object())
    {
        object()->setProperty(PROPERTY_TO_STRING(BORDERLINESIZE_PROPERTY), size.toInt());
    }

    for (QObject *it : objects())
    {
        it->setProperty(PROPERTY_TO_STRING(BORDERLINESIZE_PROPERTY), size.toInt());
    }
}

void line_prop_edit::update(QString PropertyName)
{
    if (!object())
    {
        return;
    }

    if (PropertyName == PROPERTY_TO_STRING(BORDERLINESIZE_PROPERTY))
    {
        m_lineWidthSpinbox->setValue(object()->property(PROPERTY_TO_STRING(BORDERLINESIZE_PROPERTY)).value<int>());
    }
    else if (PropertyName == PROPERTY_TO_STRING(BORDERCOLOR_PROPERTY))
    {
        m_lineColorButton->setColor(object()->property(PROPERTY_TO_STRING(BORDERCOLOR_PROPERTY)).value<QColor>());
    }
}
}
