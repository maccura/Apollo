#include "geometry_prop_widget.hpp"
#include "property_widget_factory.hpp"
#include "item/base_design_item.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace
{
ReportDesign::base_prop_widget *createGeometryPropWidget(QObject *object, QList<QObject *> *objects, QWidget *parent)
{
    if (dynamic_cast<ReportDesign::base_design_item *>(object) == nullptr)
    {
        return nullptr;
    }
    return new ReportDesign::geometry_prop_widget(object, objects, parent);
}
bool registred = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(GEOMETRY_PROPERTY), createGeometryPropWidget);
bool registred2 = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(GEOMETRY_LOCKED_PROPERTY), createGeometryPropWidget);
}

namespace ReportDesign
{

geometry_prop_widget::geometry_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent):
    base_prop_widget(object, objects, parent), m_XLineEdit(new prop_line_edit(tr("X(mm)"), this)),
    m_YLineEdit(new prop_line_edit(tr("Y(mm)"), this)), m_WLineEdit(new prop_line_edit(tr("W(mm)"), this)),
    m_HLineEdit(new prop_line_edit(tr("H(mm)"), this)), m_lockButton(new image_button(this, ":/geometry_unlock", ":/geometry_unlock", ":/geometry_lock"))
{
    init();
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout();
    QVBoxLayout *vlayoutLine = new QVBoxLayout();

    vlayout->addWidget(new QLabel(tr("geometry"), this));
    hlayout->addWidget(m_XLineEdit);
    hlayout->addWidget(m_YLineEdit);
    vlayoutLine->addLayout(hlayout);
    hlayout = new QHBoxLayout();
    hlayout->addWidget(m_WLineEdit);
    hlayout->addWidget(m_HLineEdit);
    vlayoutLine->addLayout(hlayout);
    hlayout = new QHBoxLayout();
    hlayout->addSpacing(1);
    hlayout->addLayout(vlayoutLine);
    hlayout->addWidget(m_lockButton);
    vlayout->addLayout(hlayout);

    vlayout->setContentsMargins(PROP_WIDGET_MARGINS);

    connect(m_lockButton, SIGNAL(clicked(bool)), this, SLOT(slotGeometryLock(bool)));
    connect(m_XLineEdit, SIGNAL(contentEditFinished(QString)), this, SLOT(slotGeometryChange()));
    connect(m_YLineEdit, SIGNAL(contentEditFinished(QString)), this, SLOT(slotGeometryChange()));
    connect(m_WLineEdit, SIGNAL(contentEditFinished(QString)), this, SLOT(slotGeometryChange()));
    connect(m_HLineEdit, SIGNAL(contentEditFinished(QString)), this, SLOT(slotGeometryChange()));

    base_design_item *item = dynamic_cast<ReportDesign::base_design_item *>(object);
    if (item)
    {
        connect(item, &base_design_item::posChanging, this, &geometry_prop_widget::slotPosChange);
        connect(item, &base_design_item::geometryChanged, this, &geometry_prop_widget::slotPosChange);
    }
}

void geometry_prop_widget::slotPosChange()
{
    if (object())
    {
        UpdateGeometry(object()->property(PROPERTY_TO_STRING(GEOMETRY_PROPERTY)).toRect());
    }
}

geometry_prop_widget::~geometry_prop_widget()
{

}

void geometry_prop_widget::UpdateGeometryLock(bool cheaked)
{
    m_lockButton->setChecked(cheaked);
    m_XLineEdit->setReadOnly(cheaked);
    m_YLineEdit->setReadOnly(cheaked);
    m_WLineEdit->setReadOnly(cheaked);
    m_HLineEdit->setReadOnly(cheaked);

}

void geometry_prop_widget::UpdateGeometry(QRect rect)
{
    m_XLineEdit->setContent(QString::number(valueInUnits(rect.x())));
    m_YLineEdit->setContent(QString::number(valueInUnits(rect.y())));
    m_WLineEdit->setContent(QString::number(valueInUnits(rect.width())));
    m_HLineEdit->setContent(QString::number(valueInUnits(rect.height())));
}

void geometry_prop_widget::init()
{
    if (object())
    {
        QRect rect =  object()->property(PROPERTY_TO_STRING(GEOMETRY_PROPERTY)).toRect();
        bool lock = object()->property(PROPERTY_TO_STRING(GEOMETRY_LOCKED_PROPERTY)).toBool();
        UpdateGeometry(rect);
        UpdateGeometryLock(lock);
    }
}

void geometry_prop_widget::update(QString PropertyName)
{
    if (!object())
    {
        return;
    }

    // 位置
    if (PropertyName == PROPERTY_TO_STRING(GEOMETRY_PROPERTY))
    {
        UpdateGeometry(object()->property(PROPERTY_TO_STRING(GEOMETRY_PROPERTY)).toRect());
    }// 位置锁定
    else if (PropertyName == PROPERTY_TO_STRING(GEOMETRY_LOCKED_PROPERTY))
    {
        UpdateGeometryLock(object()->property(PROPERTY_TO_STRING(GEOMETRY_LOCKED_PROPERTY)).toBool());
    }
}

QRectF geometry_prop_widget::rectInUnits(QRectF rect)
{
    base_design_item *item = dynamic_cast<base_design_item *>(object());
    if (!item)
    {
        return QRectF();
    }

    switch (item->unitType())
    {
    case ReportDesign::base_design_item::Millimeters:
        return QRectF(rect.x() / item->unitFactor(),
                      rect.y() / item->unitFactor(),
                      rect.width() / item->unitFactor(),
                      rect.height() / item->unitFactor());
    case ReportDesign::base_design_item::Inches:
        return QRectF(rect.x() / (item->unitFactor() * 10),
                      rect.y() / (item->unitFactor() * 10),
                      rect.width() / (item->unitFactor() * 10),
                      rect.height() / (item->unitFactor() * 10));
    }
    return QRectF();
}

qreal geometry_prop_widget::valueInUnits(qreal value)
{
    base_design_item *item = dynamic_cast<base_design_item *>(object());
    if (item)
    {
        switch (item->unitType())
        {
        case ReportDesign::base_design_item::Millimeters:
            return value / item->unitFactor();
        case ReportDesign::base_design_item::Inches:
            return value / (item->unitFactor() * 10);
        }
    }
    return 0;
}

qreal geometry_prop_widget::valueInReportUnits(qreal value)
{
    base_design_item *item = dynamic_cast<base_design_item *>(object());
    switch (item->unitType())
    {
    case ReportDesign::base_design_item::Millimeters:
        return value * item->unitFactor();
    case ReportDesign::base_design_item::Inches:
        return value * (item->unitFactor() * 10);
    }
    return 0;
}

void geometry_prop_widget::slotGeometryChange()
{
    QRect geometryRect;
    bool okX = false, okY = false, okW = false, okH = false;
    double x = m_XLineEdit->content().toDouble(&okX);
    double y = m_YLineEdit->content().toDouble(&okY);
    double w = m_WLineEdit->content().toDouble(&okW);
    double h = m_HLineEdit->content().toDouble(&okH);
    if (!(okX && okY && okW && okH) ||
        !object())
    {
        return;
    }

    geometryRect.setRect(valueInReportUnits(x), valueInReportUnits(y), valueInReportUnits(w), valueInReportUnits(h));
    object()->setProperty(PROPERTY_TO_STRING(GEOMETRY_PROPERTY), geometryRect);

    //
    for (QObject *it : objects())
    {
        it->setProperty(PROPERTY_TO_STRING(GEOMETRY_PROPERTY), geometryRect);
    }
}

void geometry_prop_widget::slotGeometryLock(bool cheaked)
{
    m_XLineEdit->setReadOnly(cheaked);
    m_YLineEdit->setReadOnly(cheaked);
    m_WLineEdit->setReadOnly(cheaked);
    m_HLineEdit->setReadOnly(cheaked);

    if (!object())
    {
        return;
    }
    object()->setProperty(PROPERTY_TO_STRING(GEOMETRY_LOCKED_PROPERTY), cheaked);

    //
    for (QObject *it : objects())
    {
        it->setProperty(PROPERTY_TO_STRING(GEOMETRY_LOCKED_PROPERTY), cheaked);
    }
}
}
