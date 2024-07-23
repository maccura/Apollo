#include "boder_lines_prop_widget.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QButtonGroup>
#include "property_widget_factory.hpp"

namespace
{
ReportDesign::base_prop_widget *createBoderPropWidget(QObject *object, QList<QObject *> *objects, QWidget *parent)
{
    if (dynamic_cast<ReportDesign::base_design_item *>(object) == nullptr)
    {
        return nullptr;
    }
    return new ReportDesign::boder_lines_prop_widget(object, objects, parent);
}

bool registred = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(BORDER_PROPERTY), createBoderPropWidget);
}

namespace ReportDesign
{
boder_lines_prop_widget::boder_lines_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent):
    base_prop_widget(object, objects, parent), m_left(new QCheckBox(this)), m_top(new QCheckBox(this)),
    m_right(new QCheckBox(this)), m_bottom(new QCheckBox(this))
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout();
    vlayout->addWidget(new QLabel(tr("boder"), this));

    hlayout->addWidget(m_top);
    hlayout->addSpacing(10);
    hlayout->addWidget(new QLabel(tr("up"), this));
    hlayout->addStretch(0);

    hlayout->addWidget(m_bottom);
    hlayout->addSpacing(10);
    hlayout->addWidget(new QLabel(tr("bottom"), this));
    hlayout->addStretch(0);

    hlayout->addWidget(m_left);
    hlayout->addSpacing(10);
    hlayout->addWidget(new QLabel(tr("left"), this));
    hlayout->addStretch(0);

    hlayout->addWidget(m_right);
    hlayout->addSpacing(10);
    hlayout->addWidget(new QLabel(tr("right"), this));
    hlayout->addStretch(0);

    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->setSpacing(0);
    vlayout->addLayout(hlayout);
    vlayout->setContentsMargins(PROP_WIDGET_MARGINS);

    if (object)
    {
        updateBoder(object->property(PROPERTY_TO_STRING(BORDER_PROPERTY)).value<base_design_item::BorderSide>());
    }

    connect(m_top, &QCheckBox::stateChanged, [this]
    {
        slotCheakBox(m_top);
    });
    connect(m_bottom, &QCheckBox::stateChanged, [this]
    {
        slotCheakBox(m_bottom);
    });
    connect(m_left, &QCheckBox::stateChanged, [this]
    {
        slotCheakBox(m_left);
    });
    connect(m_right, &QCheckBox::stateChanged, [this]
    {
        slotCheakBox(m_right);
    });
}

void boder_lines_prop_widget::slotCheakBox(QAbstractButton *state)
{
    slotCheakBox(object(), state);
    slotCheakBox(objects(), state);
}

void boder_lines_prop_widget::slotCheakBox(QObject *object, QAbstractButton *state)
{
    if (!object)
    {
        return;
    }

    int flag = object->property(PROPERTY_TO_STRING(BORDER_PROPERTY)).toInt();
    if (state == m_top)
    {
        if (m_top->isChecked())
        {
            flag |= base_design_item::TopLine;
        }
        else
        {
            flag ^= base_design_item::TopLine;
        }
    }
    else if (state == m_bottom)
    {
        if (m_bottom->isChecked())
        {
            flag |= base_design_item::BottomLine;
        }
        else
        {
            flag ^= base_design_item::BottomLine;
        }
    }
    else if (state == m_left)
    {
        if (m_left->isChecked())
        {
            flag |= base_design_item::LeftLine;
        }
        else
        {
            flag ^= base_design_item::LeftLine;
        }
    }
    else if (state == m_right)
    {
        if (m_right->isChecked())
        {
            flag |= base_design_item::RightLine;
        }
        else
        {
            flag ^= base_design_item::RightLine;
        }
    }
    object->setProperty(PROPERTY_TO_STRING(BORDER_PROPERTY), flag);
}

void boder_lines_prop_widget::slotCheakBox(const QList<QObject *> &object, QAbstractButton *state)
{
    for (QObject *it : object)
    {
        slotCheakBox(it, state);
    }
}

void boder_lines_prop_widget::update(QString PropertyName)
{
    if (!object())
    {
        return;
    }

    if (PropertyName == PROPERTY_TO_STRING(BORDER_PROPERTY))
    {
        updateBoder(object()->property(PROPERTY_TO_STRING(BORDER_PROPERTY)).value<base_design_item::BorderSide>());
    }
}

void boder_lines_prop_widget::clearCheak()
{
    m_top->setChecked(false);
    m_bottom->setChecked(false);
    m_right->setChecked(false);
    m_left->setChecked(false);
}

void boder_lines_prop_widget::updateBoder(base_design_item::BorderSide flag)
{
    // clearCheak();
    if (flag & base_design_item::RightLine)
    {
        m_right->setChecked(true);
    }
    else
    {
        m_right->setChecked(false);
    }

    if (flag & base_design_item::TopLine)
    {
        m_top->setChecked(true);
    }
    else
    {
        m_top->setChecked(false);
    }

    if (flag & base_design_item::BottomLine)
    {
        m_bottom->setChecked(true);
    }
    else
    {
        m_bottom->setChecked(false);
    }

    if (flag & base_design_item::LeftLine)
    {
        m_left->setChecked(true);
    }
    else
    {
        m_left->setChecked(false);
    }
}

}
