#include "page_prop_editor_widget.hpp"
#include <QVBoxLayout>
#include "property_widget_factory.hpp"
#include "item/base_design_item.hpp"
namespace
{

ReportDesign::base_prop_widget *createPagePropWidget(QObject *object, QList<QObject *> *objects, QWidget *parent)
{
    if (dynamic_cast<ReportDesign::base_design_item *>(object) == nullptr)
    {
        return nullptr;
    }
    return new ReportDesign::page_prop_editor_widget(object, objects, parent);
}
bool registred = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(PAGE_SIZE_PROPERTY), createPagePropWidget);
bool registred2 = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(PAGE_ORIENTATION_PROPERTY), createPagePropWidget);
}

namespace ReportDesign
{
void page_prop_editor_widget::translateEnumItemName()
{
    tr("Portrait");
    tr("Landscape");
}

page_prop_editor_widget::page_prop_editor_widget(QObject *object, QList<QObject *> *objects, QWidget *parent): base_prop_widget(object, objects, parent),
    m_pageSizeLabel(new QLabel(tr("page size"), this)), m_pageOrientationLabel(new QLabel(tr("page orientation"), this)),
    m_pageSize(new prop_combox(this)), m_pageOrientation(new prop_combox(this))
{
    init();
}

void page_prop_editor_widget::update(QString PropertyName)
{
    if (!object())
    {
        return;
    }
    if (PropertyName == PROPERTY_TO_STRING(PAGE_SIZE_PROPERTY))
    {
        updatePageProperty(m_pageSize, object()->property(PROPERTY_TO_STRING(PAGE_SIZE_PROPERTY)).toInt());
    }
    else if (PropertyName == PROPERTY_TO_STRING(PAGE_ORIENTATION_PROPERTY))
    {
        updatePageProperty(m_pageOrientation, object()->property(PROPERTY_TO_STRING(PAGE_ORIENTATION_PROPERTY)).toInt());
    }
}

void page_prop_editor_widget::init()
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addWidget(m_pageSizeLabel);
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(m_pageSize);
    hlayout->addSpacing(40);
    vlayout->addLayout(hlayout);
    vlayout->addSpacing(5);
    vlayout->addWidget(m_pageOrientationLabel);

    hlayout = new QHBoxLayout();
    hlayout->addWidget(m_pageOrientation);
    hlayout->addSpacing(40);
    vlayout->addLayout(hlayout);
    vlayout->setContentsMargins(PROP_WIDGET_MARGINS);

    QMetaEnum pageSizePropEnum = object()->metaObject()->property(object()->metaObject()->indexOfProperty(PROPERTY_TO_STRING(PAGE_SIZE_PROPERTY))).enumerator();
    initCombox(m_pageSize, pageSizePropEnum, object()->property(PROPERTY_TO_STRING(PAGE_SIZE_PROPERTY)).toInt());
    QMetaEnum pageOrientationPropEnum = object()->metaObject()->property(object()->metaObject()->indexOfProperty(PROPERTY_TO_STRING(PAGE_ORIENTATION_PROPERTY))).enumerator();
    initCombox(m_pageOrientation, pageOrientationPropEnum, object()->property(PROPERTY_TO_STRING(PAGE_ORIENTATION_PROPERTY)).toInt());
}

void page_prop_editor_widget::slotPageSizeChange(int index)
{
    QObject *page = object();
    if (page)
    {
        page->setProperty(PROPERTY_TO_STRING(PAGE_SIZE_PROPERTY), m_pageSize->itemData(index));
    }

    foreach (QObject *item, objects())
    {
        if (item->metaObject()->indexOfProperty(PROPERTY_TO_STRING(PAGE_SIZE_PROPERTY)) != -1)
        {
            item->setProperty(PROPERTY_TO_STRING(PAGE_SIZE_PROPERTY), m_pageSize->itemData(index));
        }
    }
}

void page_prop_editor_widget::slotPageOrientationChange(int index)
{
    QObject *page = object();
    if (page)
    {
        page->setProperty(PROPERTY_TO_STRING(PAGE_ORIENTATION_PROPERTY), m_pageOrientation->itemData(index));
    }

    foreach (QObject *item, objects())
    {
        if (item->metaObject()->indexOfProperty(PROPERTY_TO_STRING(PAGE_ORIENTATION_PROPERTY)) != -1)
        {
            item->setProperty(PROPERTY_TO_STRING(PAGE_ORIENTATION_PROPERTY), m_pageOrientation->itemData(index));
        }
    }
}

void page_prop_editor_widget::initCombox(prop_combox *comBox, QMetaEnum propEnum, int curentpropEnum)
{
    int cruIndex = -1;
    for (int i = 0; i < propEnum.keyCount(); ++i)
    {
        if (curentpropEnum == propEnum.value(i))
        {
            cruIndex = i;
        }
        comBox->insertItem(i, tr(propEnum.key(i)), propEnum.value(i));
    }

    if (cruIndex != -1)
    {
        comBox->setCurrentIndex(cruIndex);
    }

    if (comBox == m_pageSize)
    {
        connect(comBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotPageSizeChange(int)));
    }
    else if (comBox == m_pageOrientation)
    {
        connect(comBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotPageOrientationChange(int)));
    }
}

void page_prop_editor_widget::updatePageProperty(prop_combox *comBox, int Property)
{
    for (int i = 0; i < comBox->count(); ++i)
    {
        if (comBox->itemData(i).toInt() == Property)
        {
            comBox->setCurrentIndex(i);
            return;
        }
    }
}
}
