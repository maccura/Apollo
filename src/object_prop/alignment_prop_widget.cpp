#include "alignment_prop_widget.hpp"
#include "property_widget_factory.hpp"
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
namespace
{
ReportDesign::base_prop_widget *createAlignmentPropWidget(QObject *object, QList<QObject *> *objects, QWidget *parent)
{
    if (dynamic_cast<ReportDesign::base_design_item *>(object) == nullptr)
    {
        return nullptr;
    }
    return new ReportDesign::alignment_prop_widget(object, objects, parent);
}
bool registred = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(ITEM_ALIGNMENT_PROPERTY), createAlignmentPropWidget);
}

namespace ReportDesign
{

alignment_prop_widget::alignment_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent):
    base_prop_widget(object, objects, parent)
{
    initEditor();
}

void alignment_prop_widget::update(QString PropertyName)
{

}

page_scene *alignment_prop_widget::page()
{
    base_design_item *item = dynamic_cast<base_design_item *>(object());
    if (item)
    {
        return item->page();
    }
}

void alignment_prop_widget::slotBringToFront()
{
    if (page())
    {
        page()->bringToFront();
    }
}

void alignment_prop_widget::slotSendToBack()
{
    if (page())
    {
        page()->sendToBack();
    }
}

void alignment_prop_widget::slotAlignToLeft()
{
    if (page())
    {
        page()->alignToLeft();
    }
}

void alignment_prop_widget::slotAlignToRight()
{
    if (page())
    {
        page()->alignToRigth();
    }
}

void alignment_prop_widget::slotAlignToVCenter()
{
    if (page())
    {
        page()->alignToVCenter();
    }
}

void alignment_prop_widget::slotAlignToTop()
{
    if (page())
    {
        page()->alignToTop();
    }
}

void alignment_prop_widget::slotAlignToBottom()
{
    if (page())
    {
        page()->alignToBottom();
    }
}

void alignment_prop_widget::slotAlignToHCenter()
{
    if (page())
    {
        page()->alignToHCenter();
    }
}

void alignment_prop_widget::slotSameHeight()
{
    if (page())
    {
        page()->sameHeight();
    }
}

void alignment_prop_widget::slotSameWidth()
{
    if (page())
    {
        page()->sameWidth();
    }
}

void alignment_prop_widget::initEditor()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    m_alignToLeft = new image_button(this, ":/alignToLeft", ":/alignToLeft_c");
    m_alignToLeft->setToolTip(tr("Align to left"));
    connect(m_alignToLeft, SIGNAL(clicked(bool)), this, SLOT(slotAlignToLeft()));

    m_alignToRight = new image_button(this, ":/alignToRight", ":/alignToRight_c");
    m_alignToRight->setToolTip(tr("Align to right"));
    connect(m_alignToRight, SIGNAL(clicked(bool)), this, SLOT(slotAlignToRight()));

    m_alignToTop = new image_button(this, ":/alignToTop", ":/alignToTop_c");
    m_alignToTop->setToolTip(tr("Align to top"));
    connect(m_alignToTop, SIGNAL(clicked(bool)), this, SLOT(slotAlignToTop()));

    m_alignToBottom = new image_button(this, ":/alignToBottom", ":/alignToBottom_c");
    m_alignToBottom->setToolTip(tr("Align to bottom"));
    connect(m_alignToBottom, SIGNAL(clicked(bool)), this, SLOT(slotAlignToBottom()));

    m_alignToVCenter = new image_button(this, ":/alignToVCenter", ":/alignToVCenter_c");
    m_alignToVCenter->setToolTip(tr("Align to horizontal center"));
    connect(m_alignToVCenter, SIGNAL(clicked(bool)), this, SLOT(slotAlignToVCenter()));

    m_alignToHCenter = new image_button(this, ":/alignToHCenter", ":/alignToHCenter_c");
    m_alignToHCenter->setToolTip(tr("Align to vertical center"));

    connect(m_alignToHCenter, SIGNAL(clicked(bool)), this, SLOT(slotAlignToHCenter()));

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout();

    hlayout->addSpacing(10);
    hlayout->addWidget(new QLabel(tr("alignment")));
    vlayout->addLayout(hlayout);
    vlayout->addSpacing(5);

    hlayout = new QHBoxLayout();
    hlayout->addWidget(m_alignToLeft);
    hlayout->addWidget(m_alignToRight);
    hlayout->addWidget(m_alignToTop);
    hlayout->addWidget(m_alignToBottom);
    hlayout->addWidget(m_alignToVCenter);
    hlayout->addWidget(m_alignToHCenter);
    hlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addLayout(hlayout);
    vlayout->setContentsMargins(2, PROP_WIDGET_MARGINS.top() - 6, 0, 0);
}

} //namespace LimeReport
